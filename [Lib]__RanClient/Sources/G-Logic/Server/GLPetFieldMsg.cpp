#include "pch.h"
#include "./GLPetField.h"
#include "./GLGaeaserver.h"
#include "../[Lib]__Engine/Sources/Common/StringUtils.h"

#include "RanFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT	GLPetField::MsgGoto ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_GOTO* pNetMsg = ( GLMSG::SNETPET_REQ_GOTO* ) nmg;

	// 서버와 클라이언트의 팻위치가 서로 많이 어긋나면 위치를 강제한다
	D3DXVECTOR3 vDist = m_vPos - pNetMsg->vCurPos;
	float fDist = D3DXVec3Length(&vDist);
	if ( fDist > 60.0f )
	{
		// 자신에게 위치를 강제함

		// 주변에 알림
	}

	m_vTarPos = pNetMsg->vTarPos;

	// 이동처리
	BOOL bGoto = m_actorMove.GotoLocation ( m_vTarPos+D3DXVECTOR3(0,+10,0),
							                m_vTarPos+D3DXVECTOR3(0,-10,0) );

	if ( !bGoto )	return E_FAIL;

	m_dwActionFlag = pNetMsg->dwFlag;

	if ( IsSTATE ( EM_PETACT_RUN ) ) m_actorMove.SetMaxSpeed ( m_fRunSpeed );
	else						     m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
	
	// 주변에 알림
	GLMSG::SNETPET_GOTO_BRD NetMsgBrd;
	NetMsgBrd.dwGUID =  m_dwGUID;
	NetMsgBrd.vCurPos = pNetMsg->vCurPos;
	NetMsgBrd.vTarPos = m_vTarPos;
	NetMsgBrd.dwFlag  = m_dwActionFlag;
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBrd );

	return S_OK;
}

HRESULT	GLPetField::MsgStop ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_STOP* pNetMsg = ( GLMSG::SNETPET_REQ_STOP* ) nmg;

	// 클라이언트의 멈춤 메시지에 따라 서버의 위치를 항상 클라이언트와 똑같이 강제한다.
	m_dwActionFlag = pNetMsg->dwFlag;
	m_vPos		   = pNetMsg->vPos;
	m_actorMove.Stop ();
	m_actorMove.SetPosition ( m_vPos, -1 );

	// 주변에 알림
	GLMSG::SNETPET_STOP_BRD NetMsgBrd;
	NetMsgBrd.dwGUID = m_dwGUID;
	NetMsgBrd.dwFlag = pNetMsg->dwFlag;
	NetMsgBrd.vPos	 = pNetMsg->vPos;
	NetMsgBrd.bStopAttack = pNetMsg->bStopAttack;
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBrd );

	return S_OK;
}

HRESULT	GLPetField::MsgUpdateMoveState ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_UPDATE_MOVESTATE* pNetMsg = 
		( GLMSG::SNETPET_REQ_UPDATE_MOVESTATE* ) nmg;

	m_dwActionFlag = pNetMsg->dwFlag;

	if ( IsSTATE ( EM_PETACT_RUN ) )	m_actorMove.SetMaxSpeed ( m_fRunSpeed );
	else								m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
	
	// 주변에 알림
	GLMSG::SNETPET_REQ_UPDATE_MOVESTATE_BRD NetMsgBrd;
	NetMsgBrd.dwGUID = m_dwGUID;
	NetMsgBrd.dwFlag = pNetMsg->dwFlag;

	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBrd );

	return S_OK;
}

HRESULT GLPetField::MsgRename ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_RENAME *pNetMsg = (GLMSG::SNETPET_REQ_RENAME*) nmg;

	GLMSG::SNETPET_REQ_RENAME_FB NetMsgFB;

	SINVENITEM* pINVENITEM = m_pOwner->m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( m_pOwner->CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )	return S_FALSE;

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType != ITEM_PET_RENAME )
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}
	
	CString strChaName(pNetMsg->szName);
	strChaName.Trim(_T(" ")); // 앞뒤 공백제거

	// 스페이스 찾기, 캐릭터이름 4글자 이하 에러, 캐릭터 만들 수 없음.
	if ((strChaName.FindOneOf(" ") != -1) || (strChaName.GetLength() < 4)) 
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_LENGTH;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	BOOL bFILTER0 = STRUTIL::CheckString( strChaName );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strChaName );
	if ( bFILTER0 || bFILTER1 )
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_BAD_NAME;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_pOwner->m_dwClientID, &NetMsgFB );
		return S_FALSE;			
	}


#ifdef TH_PARAM	
	// 태국어 문자 조합 체크
	if ( !m_pCheckString ) return E_FAIL;

	if ( !m_pCheckString(strChaName) )
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_THAICHAR_ERROR;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}
#endif

#ifdef VN_PARAM
	// 베트남 문자 조합 체크 
	if( STRUTIL::CheckVietnamString( strChaName ) )
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_VNCHAR_ERROR;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

#endif 


	if ( GLGaeaServer::GetInstance().IsEmulatorMode() )
	{
		// 클라이언트에 처리 결과 전송.
		StringCchCopy ( NetMsgFB.szName, PETNAMESIZE+1, pNetMsg->szName );
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_OK;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);

		//	[자신에게] 인밴에 아이탬 제거.
		m_pOwner->DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	}

	// 이름변경 적용
	StringCchCopy( m_szName, PETNAMESIZE+1, strChaName.GetString() );

	// 펫 액션 로그
	GLITEMLMT::GetInstance().ReqPetAction( m_dwPetID, pINVENITEM->sItemCustom.sNativeID, EMPET_ACTION_RENAME, 0 );

	// DB에 저장요청	
	CRenamePet* pDBAction = new CRenamePet ( m_pOwner->m_dwClientID, m_pOwner->m_dwCharID, m_dwPetID, m_szName, pNetMsg->wPosX, pNetMsg->wPosY );
	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDBAction );
	}

	return S_OK;
}

HRESULT GLPetField::RenameFeedBack ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_RENAME_FROMDB_FB *pNetMsg = (GLMSG::SNETPET_REQ_RENAME_FROMDB_FB *)nmg;
	
	// 클라이언트에 처리 결과 전송.
	GLMSG::SNETPET_REQ_RENAME_FB NetMsgFB;

	if ( pNetMsg->emFB == EMPET_REQ_RENAME_FB_FROM_DB_FAIL )
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FROM_DB_FAIL;
		StringCchCopy ( NetMsgFB.szName, PETNAMESIZE+1, pNetMsg->szName );
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}
	else if ( pNetMsg->emFB == EMPET_REQ_RENAME_FB_FROM_DB_OK )
	{
		StringCchCopy ( NetMsgFB.szName, PETNAMESIZE+1, pNetMsg->szName );
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_OK;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);

		// 주변에 이름변경을 알림.
		GLMSG::SNETPET_REQ_RENAME_BRD NetMsgBrd;
		NetMsgBrd.dwGUID = m_dwGUID;
		StringCchCopy ( NetMsgBrd.szName, PETNAMESIZE+1, pNetMsg->szName );
		m_pOwner->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsgBrd );

		//	[자신에게] 인밴에 아이탬 제거.
		m_pOwner->DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	}

	return S_OK;
}

HRESULT	GLPetField::MsgChangeColor ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_CHANGE_COLOR *pNetMsg = (GLMSG::SNETPET_REQ_CHANGE_COLOR*) nmg;

	GLMSG::SNETPET_REQ_CHANGE_COLOR_FB NetMsgFB;

	if ( !GLCONST_PET::IsChangeablePetColor ( m_emTYPE ) )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_UNCHANGEABLE;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	SINVENITEM* pINVENITEM = m_pOwner->m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( m_pOwner->CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_COOLTIME;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType != ITEM_PET_COLOR )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( IsUsePetSkinPack() )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	m_wColor = pNetMsg->wColor;

	CSetPetColor* pDbAction = new CSetPetColor ( m_pOwner->m_dwCharID, m_dwPetID, m_wColor );
	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
	}

	m_pOwner->DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	// 펫 액션 로그
	GLITEMLMT::GetInstance().ReqPetAction( m_dwPetID, pINVENITEM->sItemCustom.sNativeID, EMPET_ACTION_COLOR, 0 );

	NetMsgFB.emFB   = EMPET_REQ_COLOR_CHANGE_FB_OK;
	NetMsgFB.wColor = pNetMsg->wColor;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);

	GLMSG::SNETPET_REQ_CHANGE_COLOR_BRD NetMsgBRD;
	NetMsgBRD.dwGUID = m_dwGUID;
	NetMsgBRD.wColor = pNetMsg->wColor;
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* ) &NetMsgBRD );

	return S_OK;
}
HRESULT	GLPetField::MsgChangeStyle ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_CHANGE_STYLE *pNetMsg = (GLMSG::SNETPET_REQ_CHANGE_STYLE*) nmg;

	GLMSG::SNETPET_REQ_CHANGE_STYLE_FB NetMsgFB;

	SINVENITEM* pINVENITEM = m_pOwner->m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( m_pOwner->CheckCoolTime( pINVENITEM->sItemCustom.sNativeID ) )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_COOLTIME;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pINVENITEM->sItemCustom.sNativeID );
	if ( pITEM->sBasicOp.emItemType != ITEM_PET_STYLE )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( IsUsePetSkinPack() )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( m_wColor == GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLE_COLOR[m_wStyle] )
	{
		m_wColor = GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLE_COLOR[pNetMsg->wStyle];

		CSetPetColor* pDbAction = new CSetPetColor ( m_pOwner->m_dwCharID, m_dwPetID, m_wColor );
		if ( GLGaeaServer::GetInstance().GetDBMan() )
		{
			GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
		}
	}

	m_wStyle = pNetMsg->wStyle;

	CSetPetStyle* pDbAction = new CSetPetStyle ( m_pOwner->m_dwCharID, m_dwPetID, m_wStyle );
	if ( GLGaeaServer::GetInstance().GetDBMan() )
	{
		GLGaeaServer::GetInstance().GetDBMan()->AddJob ( pDbAction );
	}

	m_pOwner->DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	// 펫 액션 로그
	GLITEMLMT::GetInstance().ReqPetAction( m_dwPetID, pINVENITEM->sItemCustom.sNativeID, EMPET_ACTION_STYLE, 0 );
	
	NetMsgFB.emFB   = EMPET_REQ_STYLE_CHANGE_FB_OK;
	NetMsgFB.wStyle = pNetMsg->wStyle;
	NetMsgFB.wColor = m_wColor;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);

	GLMSG::SNETPET_REQ_CHANGE_STYLE_BRD NetMsgBRD;
	NetMsgBRD.dwGUID = m_dwGUID;
	NetMsgBRD.wStyle = pNetMsg->wStyle;
	NetMsgBRD.wColor = m_wColor;
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* ) &NetMsgBRD );
	
	return S_OK;
}

HRESULT GLPetField::MsgChangeActiveSkill ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_SKILLCHANGE* pNetMsg = ( GLMSG::SNETPET_REQ_SKILLCHANGE* ) nmg;

	if ( !CheckSkill ( pNetMsg->sSkillID ) && pNetMsg->sSkillID != NATIVEID_NULL() ) return E_FAIL;

	if ( m_sActiveSkillID != NATIVEID_NULL() )
	{
		if( m_sActiveSkillID.wMainID != 26 )
		{
			CDebugSet::ToLogFile( "ERR : GLPetField::MsgChangeActiveSkill_01, MID = %d, SID = %d",  
																		m_sActiveSkillID.wMainID,
																		m_sActiveSkillID.wSubID );
		}

		// DB에 스킬 변경 저장
		PETSKILL sPetSkill( m_sActiveSkillID, 0 );
		CSetPetSkill* pDbAction = new CSetPetSkill ( m_pOwner->m_dwCharID, m_dwPetID, sPetSkill, false );
		GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
		if ( pDBMan ) pDBMan->AddJob ( pDbAction );
	}

	// 빈슬롯일경우
	if ( pNetMsg->sSkillID == NATIVEID_NULL() )
	{
		//	Memo :	이미 활성스킬이 빈슬롯으로 처리되었다.
		//			여기서 메세지가 중복으로 처리될 가능성을 막는다.
		if( m_sActiveSkillID == NATIVEID_NULL() )
			return S_OK;

		// 보조스킬 팩터에 추가
		m_pOwner->m_sPETSKILLFACT.RESET ();

		// 스킬제거
		GLMSG::SNETPET_REMOVE_SKILLFACT NetMsgRemoveSkill;
		GLGaeaServer::GetInstance().SENDTOCLIENT( m_pOwner->m_dwClientID,&NetMsgRemoveSkill );

		// 주변에 알림
		GLMSG::SNETPET_REQ_SKILLCHANGE_BRD NetMsgBRD;
		NetMsgBRD.dwGUID   = m_dwGUID;
		NetMsgBRD.dwTarID  = m_pOwner->m_dwGaeaID;
		NetMsgBRD.sSkillID = pNetMsg->sSkillID;
		m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* ) &NetMsgBRD );

		if( m_sActiveSkillID.wMainID != 26 )
		{
			CDebugSet::ToLogFile( "ERR : GLPetField::MsgChangeActiveSkill_02, MID = %d, SID = %d",  
				m_sActiveSkillID.wMainID,
				m_sActiveSkillID.wSubID );
		}

		// DB에 현재스킬을 비활성으로 설정
		PETSKILL sPetSkill( m_sActiveSkillID, 0 );
		CSetPetSkill* pDbAction = new CSetPetSkill ( m_pOwner->m_dwCharID, m_dwPetID, sPetSkill, false );
		GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
		if ( pDBMan ) pDBMan->AddJob ( pDbAction );

		m_sActiveSkillID = pNetMsg->sSkillID;

		return S_OK;
	}

	m_sActiveSkillID = pNetMsg->sSkillID;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID );
	if ( !pSkill )	return E_FAIL;

	PETSKILL_MAP_ITER iter = m_ExpSkills.find ( m_sActiveSkillID.dwID );
	if ( iter == m_ExpSkills.end() ) return E_FAIL;
	PETSKILL sPetSkill = (*iter).second;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sPetSkill.wLevel];

	//AccountSkill ( m_sActiveSkillID );

	// 스킬들이 케릭터 능력 보조스킬이라면...
	switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_PET_HEAL:
	case SKILL::EMFOR_PET_SUPPROT:
	case SKILL::EMFOR_PET_BACKUP_ATK:
	case SKILL::EMFOR_PET_BACKUP_DEF:
		break;

	case SKILL::EMFOR_PET_AUTOMEDISON:
		break;

	case SKILL::EMFOR_PET_PROTECTITEMS:
		m_pOwner->m_bProtectPutOnItem = true;
		break;

	case SKILL::EMFOR_PET_GETALLITEMS:
	case SKILL::EMFOR_PET_GETRAREITEMS:
	case SKILL::EMFOR_PET_GETPOTIONS:
	case SKILL::EMFOR_PET_GETMONEY:
	case SKILL::EMFOR_PET_GETSTONE:
		break;
	};

	// 보조스킬 팩터에 추가
	m_pOwner->m_sPETSKILLFACT.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
	m_pOwner->m_sPETSKILLFACT.fMVAR = sSKILL_DATA.fBASIC_VAR;
	m_pOwner->m_sPETSKILLFACT.fAGE = sSKILL_DATA.fLIFE;
	m_pOwner->m_sPETSKILLFACT.sNATIVEID = m_sActiveSkillID;

	// 클라이언트에 팩터 추가 알림
	GLMSG::SNETPET_ADD_SKILLFACT NetMsgAddSkill;
	NetMsgAddSkill.sNATIVEID = m_sActiveSkillID;
	NetMsgAddSkill.emTYPE	 = pSkill->m_sAPPLY.emBASIC_TYPE;
	NetMsgAddSkill.fMVAR	 = sSKILL_DATA.fBASIC_VAR;
	NetMsgAddSkill.fAge		 = sSKILL_DATA.fLIFE;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgAddSkill);

	// 주변에 알림
	GLMSG::SNETPET_REQ_SKILLCHANGE_BRD NetMsgBRD;
	NetMsgBRD.dwGUID   = m_dwGUID;
	NetMsgBRD.sSkillID = pNetMsg->sSkillID;
	NetMsgBRD.dwTarID  = m_pOwner->m_dwGaeaID;
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* ) &NetMsgBRD );

	if( m_sActiveSkillID.wMainID != 26 )
	{
		CDebugSet::ToLogFile( "ERR : GLPetField::MsgChangeActiveSkill_03, MID = %d, SID = %d",  
														m_sActiveSkillID.wMainID,
														m_sActiveSkillID.wSubID );
	}

	// DB에 스킬 변경 저장
	CSetPetSkill* pDbAction = new CSetPetSkill ( m_pOwner->m_dwCharID, m_dwPetID, sPetSkill, true );
	GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
	if ( pDBMan ) pDBMan->AddJob ( pDbAction );
	
	return S_OK;
}

// Hold <-> Slot
HRESULT	GLPetField::MsgAccHoldExSlot ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD* pNetMsg = ( GLMSG::SNETPET_REQ_SLOT_EX_HOLD* ) nmg;
	
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_FB NetMsgFB;

	//	Note : 공격중이나 스킬 시전 중에 슬롯 변경을 수행 할 수 없다고 봄.
	//
	if ( m_pOwner->IsACTION(GLAT_ATTACK) || m_pOwner->IsACTION(GLAT_SKILL) ) return E_FAIL;

	SITEMCUSTOM sHoldItemCustom = m_pOwner->GET_HOLD_ITEM ();
	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype ( pNetMsg->emSuit );

	SITEM* pHoldItem = GLItemMan::GetInstance().GetItem ( sHoldItemCustom.sNativeID );
	SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );

	if ( !pHoldItem || !pSlotItem ) 
	{
		// 일반오류
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( pHoldItem->sBasicOp.emItemType != ITEM_SUIT )						return E_FAIL;

	if ( pHoldItem->sSuitOp.emSuit != SUIT_PET_A && pHoldItem->sSuitOp.emSuit != SUIT_PET_B )
	{
		// 팻용 아이템이 아닐때
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	// 서로 다른 타입일때
	if ( pHoldItem->sSuitOp.emSuit != pSlotItem->sSuitOp.emSuit )
	{
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL; 
	}

	if ( !CheckSlotItem( pHoldItem->sBasicOp.sNativeID, pNetMsg->emSuit ) ) return E_FAIL;

	// 손에든 아이템을 팻에게 장착시키고
	SetSlotItem ( pNetMsg->emSuit, sHoldItemCustom );

	// 장착됐던 아이템을 손에 든다
	m_pOwner->HOLD_ITEM ( sSlotItemCustom );

	// 자신에게 알림
	NetMsgFB.emFB		 = EMPET_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.emSuit		 = pNetMsg->emSuit;
	NetMsgFB.sItemCustom = m_PutOnItems[pNetMsg->emSuit];
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);

	// 다른 클라이언트 화면에 아이템이 바뀐 팻이 출력되어야 한다
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD NetMsgBRD;
	NetMsgBRD.dwGUID	  = m_dwGUID;
	NetMsgBRD.emSuit	  = pNetMsg->emSuit;
	NetMsgBRD.sItemCustom = GetSlotitembySuittype ( pNetMsg->emSuit );
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );

	// Require DB갱신
	
	return S_OK;
}

// Hold -> Slot
HRESULT	GLPetField::MsgAccHoldToSlot ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_HOLD_TO_SLOT* pNetMsg = ( GLMSG::SNETPET_REQ_HOLD_TO_SLOT* ) nmg;

	GLMSG::SNETPET_REQ_HOLD_TO_SLOT_FB NetMsgFB;

	//	Note : 공격중이나 스킬 시전 중에 슬롯 변경을 수행 할 수 없다고 봄.
	//
	if ( m_pOwner->IsACTION(GLAT_ATTACK) || m_pOwner->IsACTION(GLAT_SKILL) ) return E_FAIL;

	SITEMCUSTOM sHoldItemCustom = m_pOwner->GET_HOLD_ITEM ();

	SITEM* pHoldItem = GLItemMan::GetInstance().GetItem ( sHoldItemCustom.sNativeID );

	if ( !pHoldItem ) 
	{
		// 일반오류
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( pHoldItem->sBasicOp.emItemType != ITEM_SUIT )						return E_FAIL;

	if ( pHoldItem->sSuitOp.emSuit != SUIT_PET_A && pHoldItem->sSuitOp.emSuit != SUIT_PET_B )
	{
		// 팻용 아이템이 아닐때
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	// 서로 다른 타입일때
	if ( pHoldItem->sSuitOp.emSuit != pNetMsg->emSuit )
	{
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL; 
	}

	if ( !CheckSlotItem( pHoldItem->sBasicOp.sNativeID, pNetMsg->emSuit ) ) return E_FAIL;

	// 손에든 아이템을 팻에게 장착시키고
	SetSlotItem ( pNetMsg->emSuit, sHoldItemCustom );

	// 손에든 아이템을 제거하고
	m_pOwner->RELEASE_HOLD_ITEM ();

	// 자신에게 알림
	NetMsgFB.emFB		 = EMPET_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.emSuit		 = pNetMsg->emSuit;
	NetMsgFB.sItemCustom = GetSlotitembySuittype ( pNetMsg->emSuit );
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);

	// 다른 클라이언트 화면에 아이템이 바뀐 팻이 출력되어야 한다
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD NetMsgBRD;
	NetMsgBRD.dwGUID	  = m_dwGUID;
	NetMsgBRD.emSuit	  = pNetMsg->emSuit;
	NetMsgBRD.sItemCustom = GetSlotitembySuittype ( pNetMsg->emSuit );
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );

	// Require DB갱신
	
	return S_OK;
}

// Slot -> Hold
HRESULT	GLPetField::MsgAccSlotToHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_SLOT_TO_HOLD* pNetMsg = ( GLMSG::SNETPET_REQ_SLOT_TO_HOLD* ) nmg;
	
	GLMSG::SNETPET_REQ_SLOT_TO_HOLD_FB NetMsgFB;

	//	Note : 공격중이나 스킬 시전 중에 슬롯 변경을 수행 할 수 없다고 봄.
	//
	if ( m_pOwner->IsACTION(GLAT_ATTACK) || m_pOwner->IsACTION(GLAT_SKILL) ) return E_FAIL;

	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype ( pNetMsg->emSuit );
	SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );

	if ( !pSlotItem ) 
	{
		// 일반오류
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	// 장착된 아이템을 손에 든다
	m_pOwner->HOLD_ITEM ( sSlotItemCustom );

	// 장착된 아이템을 제거한다.
	ReSetSlotItem ( pNetMsg->emSuit );

	// 자신에게 알림
	NetMsgFB.emFB		 = EMPET_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.emSuit		 = pNetMsg->emSuit;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);

	// 다른 클라이언트 화면에 아이템이 바뀐 팻이 출력되어야 한다
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD NetMsgBRD;
	NetMsgBRD.dwGUID	  = m_dwGUID;
	NetMsgBRD.emSuit	  = pNetMsg->emSuit;
	NetMsgBRD.sItemCustom = SITEMCUSTOM ( NATIVEID_NULL() );
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );

	// Require DB갱신
	
	return S_OK;
}

HRESULT	GLPetField::MsgLearnSkill ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_LEARNSKILL* pNetMsg = ( GLMSG::SNETPET_REQ_LEARNSKILL* ) nmg;

	GLMSG::SNETPET_REQ_LEARNSKILL_FB NetMsgFB;
	SINVENITEM* pInvenItem(NULL);

	// 창고에 있는 아이템 여부
	if ( pNetMsg->bStorage )
	{
		if ( !m_pOwner->IsKEEP_STORAGE (pNetMsg->dwChannel) )
		{
			NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_INVALIDSTORAGE;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
			return E_FAIL;
		}

		pInvenItem = m_pOwner->m_cStorage[pNetMsg->dwChannel].FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
		if ( !pInvenItem )
		{
			NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_NOITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
			return E_FAIL;
		}
	}
	else
	{
		pInvenItem = m_pOwner->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
		if ( !pInvenItem )
		{
			NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_NOITEM;
			GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
			return E_FAIL;
		}		
	}

	if ( m_pOwner->CheckCoolTime( pInvenItem->sItemCustom.sNativeID ) )	return E_FAIL;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );

	// 팻용 스킬인지 유무????
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_PET_SKILL )
	{
		NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_BADITEM;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	SNATIVEID sSKILL_ID(pItem->sSkillBookOp.sSkill_ID.dwID);

	if ( ISLEARNED_SKILL( sSKILL_ID ) )
	{
		//	이미 습득한 스킬.
		NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_ALREADY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	// 스킬습득
	LEARN_SKILL ( sSKILL_ID );

	NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_OK;
	NetMsgFB.wPosX = pNetMsg->wPosX;
	NetMsgFB.wPosY = pNetMsg->wPosY;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);

	// 스킬 배움을 알림 (이펙트)
	GLMSG::SNETPET_REQ_LEARNSKILL_BRD NetMsgBRD;
	NetMsgBRD.dwGUID = m_dwGUID;
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );

	// 아이템 소모시킴(인벤토리/창고)
	if ( pNetMsg->bStorage ) m_pOwner->DoDrugStorageItem( pNetMsg->dwChannel, pNetMsg->wPosX, pNetMsg->wPosY );
	else					 m_pOwner->DoDrugInvenItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	// 펫 액션 로그
	GLITEMLMT::GetInstance().ReqPetAction( m_dwPetID, pInvenItem->sItemCustom.sNativeID, EMPET_ACTION_SKILL, 0 );

	if( sSKILL_ID.wMainID != 26 )
	{
		CDebugSet::ToLogFile( "ERR : GLPetField::MsgLearnSkill, MID = %d, SID = %d",  
																sSKILL_ID.wMainID,
																sSKILL_ID.wSubID );
	}

	// DB 저장
	PETSKILL sPetSkill( sSKILL_ID, 0 );
	CSetPetSkill* pDbAction = new CSetPetSkill ( m_pOwner->m_dwCharID, m_dwPetID, sPetSkill, false );
	GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
	if ( pDBMan ) pDBMan->AddJob ( pDbAction );

	return S_OK;
}

HRESULT	GLPetField::MsgRemoveSlotItem ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM* pNetMsg = ( GLMSG::SNETPET_REQ_REMOVE_SLOTITEM* ) nmg;

	WORD wPosX(0), wPosY(0);
	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype ( pNetMsg->emSuit );

	SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );
	if ( !pSlotItem ) 
	{
		// 일반 오류
		return E_FAIL;
	}

	BOOL bOk = m_pOwner->m_cInventory.FindInsrtable ( pSlotItem->sBasicOp.wInvenSizeX, pSlotItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
	if ( !bOk )
	{
		//	인밴이 가득찻음.
		return E_FAIL;
	}

	// 인벤에 넣기
	m_pOwner->m_cInventory.InsertItem ( sSlotItemCustom, wPosX, wPosY );

	// 슬롯아이템 제거
	ReSetSlotItem ( pNetMsg->emSuit );

	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_FB NetMsgFB;
	NetMsgFB.emSuit = pNetMsg->emSuit;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);

	// 슬롯아이템 제거를 알림
	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_BRD NetMsgBRD;
	NetMsgBRD.dwGUID = m_dwGUID;
	NetMsgBRD.emSuit = pNetMsg->emSuit;
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );

	return S_OK;
}

HRESULT GLPetField::MsgPetSkinPackItem ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_SKINPACKOPEN *pNetMsg = (GLMSG::SNETPET_SKINPACKOPEN *)nmg;
	GLMSG::SNETPET_SKINPACKOPEN_FB NetMsgFB;
    size_t i;

	if ( !m_pOwner->VALID_HOLD_ITEM() )							return S_FALSE;

	SITEMCUSTOM& sHOLDITEM = m_pOwner->m_PutOnItems[SLOT_HOLD];
	SITEM* pHold = GLItemMan::GetInstance().GetItem ( sHOLDITEM.sNativeID );

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )									return E_FAIL;
	if ( pInvenItem->sItemCustom.dwPetID != m_dwPetID )	return E_FAIL;

	if ( !pHold || pHold->sBasicOp.emItemType != ITEM_PET_SKIN_PACK || !pInvenItem || pHold->sBasicOp.sNativeID != pNetMsg->sItemId )										
	{
		NetMsgFB.emFB = EMPET_PETSKINPACKOPEN_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}	

	if ( m_pOwner->CheckCoolTime( pHold->sBasicOp.sNativeID ) ) return E_FAIL;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_PET_CARD  )
	{
		NetMsgFB.emFB = EMPET_PETSKINPACKOPEN_FB_PETCARD_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	if ( !pHold->sPetSkinPack.VALID() )
	{
		NetMsgFB.emFB = EMPET_PETSKINPACKOPEN_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);
		return E_FAIL;
	}

	float fNowRate = seqrandom::getpercent();
	float fCurRate = 0.0f;
	for( i = 0; i < pHold->sPetSkinPack.vecPetSkinData.size(); i++ )
	{
		float fPreRate = fCurRate;
		fCurRate += pHold->sPetSkinPack.vecPetSkinData[i].fRate;

		if( ( fPreRate <= fNowRate ) && ( fNowRate < fCurRate ) )
			break;
		
	}

	m_sPetSkinPackData.Init();
	m_sPetSkinPackData.bUsePetSkinPack = TRUE;
	m_sPetSkinPackData.sMobID		   = pHold->sPetSkinPack.vecPetSkinData[i].sMobID;
	m_sPetSkinPackData.fScale		   = pHold->sPetSkinPack.vecPetSkinData[i].fScale;	
	m_sPetSkinPackData.dwPetSkinTime   = pHold->sPetSkinPack.dwPetSkinTime;
	m_sPetSkinPackData.startTime	   = CTime::GetCurrentTime();

	pInvenItem->sItemCustom.tBORNTIME = m_sPetSkinPackData.startTime.GetTime();
	pInvenItem->sItemCustom.tDISGUISE = pHold->sPetSkinPack.dwPetSkinTime;

	int nScale = (int)(m_sPetSkinPackData.fScale * 100.0f);

	CPetSkinPack* pDbAction = new CPetSkinPack ( m_pOwner->m_dwCharID, m_dwPetID, m_sPetSkinPackData.sMobID.wMainID, m_sPetSkinPackData.sMobID.wSubID,
												 nScale, m_sPetSkinPackData.dwPetSkinTime );
	GLDBMan* pDBMan = GLGaeaServer::GetInstance().GetDBMan ();
	if ( pDBMan ) pDBMan->AddJob ( pDbAction );

	// 펫 액션 로그
	GLITEMLMT::GetInstance().ReqPetAction( m_dwPetID, pInvenItem->sItemCustom.sNativeID, EMPET_ACTION_PETSKINPACK, 0 );

	//	성공시 아이탬 제거.
//	m_pOwner->RELEASE_HOLD_ITEM ();
	m_pOwner->DoDrugSlotItem (SLOT_HOLD );

	NetMsgFB.emFB			  = EMPET_PETSKINPACKOPEN_FB_OK;
	NetMsgFB.sPetSkinPackData = m_sPetSkinPackData;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsgFB);

	//	[자신에게] 손 있었던 아이탬 제거.
//	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
//	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsg_ReleaseHold);	

	// 코스튬 처럼 sItemCustom 구조체를 사용한다.
	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pNetMsg->wPosX;
	NetItemUpdate.wPosY = pNetMsg->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_pOwner->m_dwClientID,&NetItemUpdate);

	GLMSG::SNETPET_SKINPACKOPEN_BRD NetMsgBRD;
	NetMsgBRD.dwGUID		   = m_dwGUID;
	NetMsgBRD.sPetSkinPackData = m_sPetSkinPackData;
	m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* )&NetMsgBRD );


	return S_OK;
}

void GLPetField::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_PET_REQ_GOTO:											
		MsgGoto ( nmg );
		break;

	case NET_MSG_PET_REQ_STOP:
		MsgStop ( nmg );
		break;

	case NET_MSG_PET_REQ_UPDATE_MOVE_STATE:
		MsgUpdateMoveState ( nmg );
		break;

	case NET_MSG_PET_REQ_SKILLCHANGE:
		MsgChangeActiveSkill ( nmg );
		break;

	case NET_MSG_PET_REQ_RENAME:
		MsgRename ( nmg );
		break;

	case NET_MSG_PET_REQ_RENAME_FROMDB_FB:
		RenameFeedBack ( nmg );
		break;

	case NET_MSG_PET_REQ_CHANGE_COLOR:
		MsgChangeColor ( nmg );
		break;
	case NET_MSG_PET_REQ_CHANGE_STYLE:
		MsgChangeStyle ( nmg );
		break;

	case NET_MSG_PET_REQ_SLOT_EX_HOLD:
		MsgAccHoldExSlot ( nmg );
		break;

	case NET_MSG_PET_REQ_HOLD_TO_SLOT:
		MsgAccHoldToSlot ( nmg );
		break;

	case NET_MSG_PET_REQ_SLOT_TO_HOLD:
		MsgAccSlotToHold ( nmg );
		break;

	case NET_MSG_PET_REQ_LEARNSKILL:
		MsgLearnSkill ( nmg );
		break;

	case NET_MSG_PET_REQ_FUNNY:
		{
			GLMSG::SNETPET_REQ_FUNNY* pNetMsg = ( GLMSG::SNETPET_REQ_FUNNY* ) nmg;
			GLMSG::SNETPET_REQ_FUNNY_BRD NetMsgBRD;
			NetMsgBRD.dwGUID = m_dwGUID;
			NetMsgBRD.wFunny = pNetMsg->wFunny;
			m_pOwner->SendMsgViewAround ( ( NET_MSG_GENERIC* ) &NetMsgBRD );

			m_wAniSub = pNetMsg->wFunny;
			SetSTATE ( EM_PETACT_FUNNY );
		}
		break;

	case NET_MSG_PET_REMOVE_SLOTITEM:
		MsgRemoveSlotItem ( nmg );
		break;

	case NET_MSG_PET_PETSKINPACKOPEN:
		MsgPetSkinPackItem ( nmg );
		break;

	default:
		GLGaeaServer::GetInstance().GetConsoleMsg()->Write (LOG_CONSOLE,
			                    "[WARNING]GLPetField::MsgProcess Illigal Message(%d)",
			                    nmg->nType);
		break;
	};
}