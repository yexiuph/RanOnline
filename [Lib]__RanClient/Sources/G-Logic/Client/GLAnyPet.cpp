#include "pch.h"
#include "GLAnyPet.h"
#include "GLGaeaClient.h"
#include "GLContrlMsg.h"
#include "DxShadowMap.h"
#include "GLItemMan.h"
#include "GLCharDefine.h"
#include "DxEffGroupPlayer.h"
#include "GLFactEffect.h"
#include "GLStrikeM.h"

//#include "GLSkill.h"

GLAnyPet::GLAnyPet(void) :
	m_pd3dDevice(NULL),
	m_pSkinChar(NULL),

	m_vDir(D3DXVECTOR3(0,0,-1)),
	m_vDirOrig(D3DXVECTOR3(0,0,-1)),
	m_vPos(D3DXVECTOR3(0,0,0)),
	m_dwActionFlag(EM_PETACT_STOP),

	m_vTarPos(D3DXVECTOR3(0,0,0)),

	m_vMax(6,20,6),
	m_vMin(-6,0,-6),
	m_fHeight(20.f),

	m_vMaxOrg(6,20,6),
	m_vMinOrg(-6,0,-6),

	m_wAniSub(1)
{
	m_sPetSkinPackState.Init();
}

GLAnyPet::~GLAnyPet(void)
{
	DeleteDeviceObjects ();
}

void GLAnyPet::ASSIGN ( PSDROPPET pDropPet )
{
	m_emTYPE			= pDropPet->m_emTYPE;
	m_dwGUID			= pDropPet->m_dwGUID;
	m_sPetID			= pDropPet->m_sPetID;
	m_dwOwner			= pDropPet->m_dwOwner;
	m_wStyle			= pDropPet->m_wStyle;
	m_wColor			= pDropPet->m_wColor;
	m_nFull				= pDropPet->m_nFull;
	m_vPos				= pDropPet->m_vPos;
	m_vDir				= pDropPet->m_vDir;
	m_vTarPos			= pDropPet->m_vTarPos;

	m_sMapID			= pDropPet->m_sMapID;
	m_dwCellID			= pDropPet->m_dwCellID;

	m_fRunSpeed			= pDropPet->m_fRunSpeed;
	m_fWalkSpeed		= pDropPet->m_fWalkSpeed;

	m_dwActionFlag		= pDropPet->m_dwActionFlag;
		
	m_wAniSub			= pDropPet->m_wAniSub;

	m_dwPetID			= pDropPet->m_dwPetID;

	m_dwOwner			= pDropPet->m_dwOwner;

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = pDropPet->m_PutOnItems[i];
	}

	StringCchCopy ( m_szName, PETNAMESIZE+1, pDropPet->m_szName );
}

HRESULT GLAnyPet::Create ( PSDROPPET pDropPet, NavigationMesh* pNavi, LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 기본 데이터 설정
	ASSIGN ( pDropPet );

	m_pd3dDevice = pd3dDevice;

	m_sPetSkinPackState.bUsePetSkinPack = pDropPet->m_petSkinPackData.bUsePetSkinPack;
	m_sPetSkinPackState.petSkinMobID    = pDropPet->m_petSkinPackData.sMobID;
	m_sPetSkinPackState.fPetScale	    = pDropPet->m_petSkinPackData.fScale;

	// 초기 위치와 방향 설정
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );
	m_matTrans = matYRot * matTrans;

	if ( pNavi )
	{
		m_actorMove.Create ( pNavi, m_vPos, -1 );
	}

	if ( FAILED ( SkinLoad ( pd3dDevice ) ) )
		return E_FAIL;

	UpdateSuit ();

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	// 만약 움직이고 있다면... 내부메시지 발생
	if ( IsSTATE ( EM_PETACT_MOVE ) )
	{
		GLMSG::SNETPET_GOTO_BRD NetMsgBrd;
		NetMsgBrd.dwGUID =  m_dwGUID;
		NetMsgBrd.vCurPos = m_vPos;
		NetMsgBrd.vTarPos = m_vTarPos;
		NetMsgBrd.dwFlag  = m_dwActionFlag;

		MsgGoto ( ( NET_MSG_GENERIC* ) &NetMsgBrd );
	}

	// 퍼니액션중이면...
	if ( IsSTATE ( EM_PETACT_FUNNY ) )
	{
		m_pSkinChar->SELECTANI ( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub );
	}

	// 스킬 발동중이면 이펙트 표시
	if ( m_sActiveSkillID != NATIVEID_NULL () )
	{
		PGLCHARCLIENT pTarChar = GLGaeaClient::GetInstance().GetChar ( m_dwOwner );
		// 주인에게 붙는 이펙트 발동(변경)
		if ( pTarChar )
		{
			STARGETID sTargetID( CROW_PC, pTarChar->m_dwGaeaID, pTarChar->GetPosition () );
			FACTEFF::NewSkillFactEffect ( sTargetID, pTarChar->GetCharSkin (), m_sActiveSkillID, pTarChar->GetMatrix (), sTargetID.vPos );
		}
	}

	// SAD 액션중이면
	if ( IsSTATE ( EM_PETACT_SAD ) )
	{
		GLLandManClient* pLandMan = GLGaeaClient::GetInstance().GetActiveMap ();
		PGLCHARCLIENT pChar = pLandMan->GetChar ( m_dwOwner );
		if ( pChar )
		{
			// SAD 애니메이션
			m_pSkinChar->SELECTANI ( AN_GESTURE, AN_SUB_04 );

			// 방향 전환
			D3DXVECTOR3 vDirection = pChar->GetPosition () - m_vPos;
			D3DXVec3Normalize ( &m_vDir, &vDirection );
			m_vDir = vDirection;
		}
	}

	return S_OK;
}

HRESULT GLAnyPet::SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SNATIVEID loadPetID = m_sPetID;
	if( IsUsePetSkinPack() )
	{
		loadPetID = m_sPetSkinPackState.petSkinMobID;
	}

	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( loadPetID );
	if ( !pCrowData )					   return E_FAIL;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		pd3dDevice, 
		TRUE );

	if ( !pSkinChar )	return E_FAIL;

	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice );
	
	m_pSkinChar->SELECTANI ( AN_GUARD_N, (EMANI_SUBTYPE)0 );

	// 현재 펫 에니메이션중에 플레이가 불가능한 애니메이션을 찾는다.
	if( IsUsePetSkinPack() )
	{
		if( !m_pSkinChar->GETANIAMOUNT( AN_GUARD_N, AN_SUB_NONE )  ) m_sPetSkinPackState.bNotFoundGuard = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_RUN, AN_SUB_NONE ) )		 m_sPetSkinPackState.bNotFoundRun = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_WALK, AN_SUB_NONE ) )	 m_sPetSkinPackState.bNotFoundWalk = TRUE;	
		//		m_pSkinChar->GETANIAMOUNT( AN_GESTURE, AN_SUB_NONE ) )
		if( !m_pSkinChar->GETANIAMOUNT( AN_ATTACK, AN_SUB_NONE ) )	 m_sPetSkinPackState.bNotFoundAttack = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_GESTURE, AN_SUB_04 ) )	 m_sPetSkinPackState.bNotFoundSad	 = TRUE;
	}


	return S_OK;
}

HRESULT GLAnyPet::UpateAnimation ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT	GLAnyPet::UpdatePetState ( float fTime, float fElapsedTime )
{
	if ( IsSTATE ( EM_PETACT_STOP ) )
	{
		// 이동 메시지를 기다린다.
	}

	if ( IsSTATE ( EM_PETACT_MOVE ) )
	{
		if ( !m_actorMove.PathIsActive() )
		{
			ReSetSTATE ( EM_PETACT_MOVE );
			SetSTATE ( EM_PETACT_STOP );
			m_actorMove.Stop ();
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		}
	}

	if ( IsSTATE ( EM_PETACT_WAIT ) )
	{
		// 다음 메시지를 기다린다.
	}
	
	return S_OK;
}

HRESULT GLAnyPet::FrameMove( float fTime, float fElapsedTime )
{
	HRESULT hr(S_OK);

	UpdatePetState ( fTime, fElapsedTime );
	
	// ============== Update about Actor (Begin) ============== /

	hr = m_actorMove.Update ( fElapsedTime );
	if ( FAILED(hr) )	return E_FAIL;

	// ============== Update about Actor (end)   ============== /

	// ============== Update about Skin (Begin) ============== /

	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );
	m_matTrans = matYRot * matTrans;

	m_pSkinChar->SetPosition ( m_vPos );

	if( IsUsePetSkinPack() && IsSTATE ( EM_PETACT_MOVE ) )
	{
		float fSpeedGap1, fSpeedGap2, fTempTime, fTempElapsedTime;

		fSpeedGap1 = 1.0f / m_sPetSkinPackState.fPetScale;
		if( IsSTATE( EM_PETACT_RUN ) && m_sPetSkinPackState.bNotFoundRun ) fSpeedGap2 = m_fRunSpeed / m_fWalkSpeed;
		else															  fSpeedGap2 = 1.0f;

		fTempTime		 = fTime * fSpeedGap1 * fSpeedGap2;
		fTempElapsedTime = fElapsedTime * fSpeedGap1 * fSpeedGap2;

		m_pSkinChar->FrameMove ( fTempTime, fTempElapsedTime );
	}else{
		m_pSkinChar->FrameMove ( fTime, fElapsedTime );
	}

	// ============== Update about Skin (End)   ============== /

	// 현재 위치 업데이트
	m_vPos = m_actorMove.Position();

	// 방향 업데이트
	D3DXVECTOR3 vMovement = m_actorMove.NextPosition();
	if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
	{
		D3DXVECTOR3 vDirection = vMovement - m_vPos;
		if ( !DxIsMinVector(vDirection,0.2f) )
		{
			D3DXVec3Normalize ( &vDirection, &vDirection );
			m_vDir = vDirection;
		}
	}

	//	Note : AABB 계산.
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;

	return S_OK;
}

HRESULT GLAnyPet::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		if( IsUsePetSkinPack() )
		{
			D3DXMATRIX matScale, matRender;
			D3DXMatrixScaling( &matScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale );
			matRender = matScale * m_matTrans;
			m_pSkinChar->Render( pd3dDevice, matRender, FALSE, TRUE, TRUE );
		}else{
			m_pSkinChar->Render( pd3dDevice, m_matTrans );
		}
	}

	return S_OK;
}

HRESULT GLAnyPet::RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	return S_OK;

	//	Note : 그림자 랜더링.
	//
	hr = DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matTrans, pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT GLAnyPet::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}


HRESULT GLAnyPet::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT GLAnyPet::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT GLAnyPet::InvalidateDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT GLAnyPet::DeleteDeviceObjects ()
{
	SAFE_DELETE(m_pSkinChar);
	m_actorMove.ResetMovedData();
	m_actorMove.Stop();
	m_actorMove.Release();

	RESET ();

	return S_OK;
}

HRESULT GLAnyPet::UpdateSuit ()
{
	if( IsUsePetSkinPack() ) return S_OK;

	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sPetID );
	if ( !pCrowData )					   return E_FAIL;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		m_pd3dDevice, 
		TRUE );

	if ( !pSkinChar )					  return E_FAIL;

	const PETSTYLE &sSTYLE = GLCONST_PET::GetPetStyle ( m_emTYPE );

	// 스타일 갱신

	if ( sSTYLE.wSTYLENum > m_wStyle )
	{
		std::string strSTYLE_CPS = sSTYLE.strSTYLE_CPS[m_wStyle];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	현재 장착 스킨.

		if ( pCharPart && strcmp(strSTYLE_CPS.c_str(),pCharPart->m_szFileName) )
		{
			m_pSkinChar->SetPiece ( strSTYLE_CPS.c_str(), m_pd3dDevice );
		}
	}

	// 컬러 갱신
	m_pSkinChar->SetHairColor( m_wColor );

	// 악세서리 모양 갱신
	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( m_PutOnItems[i].sNativeID );
		if ( pSlotItem ) m_pSkinChar->SetPiece ( pSlotItem->GetPetWearingFile (), m_pd3dDevice );
		else
		{
			EMPIECECHAR emPiece = GetPieceFromSlot ( (ACCESSORYTYPE)i );
			if ( emPiece != PIECE_SIZE ) m_pSkinChar->ResetPiece ( emPiece );
		}
	}

	return S_OK;
}

void GLAnyPet::MsgGoto ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_GOTO_BRD* pNetMsg = ( GLMSG::SNETPET_GOTO_BRD* ) nmg;

	m_dwActionFlag = pNetMsg->dwFlag;

	BOOL bSucceed = m_actorMove.GotoLocation
	(
		pNetMsg->vTarPos+D3DXVECTOR3(0,+10,0),
		pNetMsg->vTarPos+D3DXVECTOR3(0,-10,0)
	);
	
	BOOL bReqRun = pNetMsg->dwFlag&EM_PETACT_RUN;

	if ( IsSTATE ( EM_PETACT_RUN ) )
	{
		if( IsUsePetSkinPack() && m_sPetSkinPackState.bNotFoundRun  )
		{
			m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		}else{
			m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
		}
		m_actorMove.SetMaxSpeed ( m_fRunSpeed );
	}
	else
	{
		m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
	}
}

void GLAnyPet::MsgStop ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_STOP_BRD* pNetMsg = ( GLMSG::SNETPET_STOP_BRD* ) nmg;

	// 멈춰야 한다면 현재 네비게이션을 끝까지 가게 해줘야 한다.
	BOOL bReqStop = pNetMsg->dwFlag&EM_PETACT_STOP;
	if ( bReqStop && !pNetMsg->bStopAttack )	return;
	
	m_dwActionFlag = pNetMsg->dwFlag;
	
	// 기다려야 하거나 어텍모션을 멈춰야 한다면
	if ( IsSTATE ( EM_PETACT_WAIT ) || pNetMsg->bStopAttack )
	{
		m_actorMove.Stop ();
		m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
	}
}

void GLAnyPet::MsgSkillChange ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_SKILLCHANGE_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_SKILLCHANGE_BRD* ) nmg;

	// 이전 이펙트 제거
	PGLCHARCLIENT pTarChar = GLGaeaClient::GetInstance().GetChar ( pNetMsg->dwTarID );
	if ( pTarChar )
	{
		FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC, pTarChar->m_dwGaeaID, pTarChar->GetPosition () ), pTarChar->GetCharSkin (), m_sActiveSkillID );
	}

	// 스킬변경
	m_sActiveSkillID = pNetMsg->sSkillID;

	// 이펙트 처리
	if ( m_sActiveSkillID != SNATIVEID(false) )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID.wMainID, m_sActiveSkillID.wSubID );
		if ( !pSkill )								return;

		// 자신에게 발동되는 이펙트 발동(변경)
		STARGETID sTARG( CROW_PET,m_dwGUID, m_vPos );

		EMELEMENT emELMT = EMELEMENT_SPIRIT;

		if ( pSkill->m_sEXT_DATA.emSELFBODY==SKILL::EMTIME_FIRST )
			DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &m_vDir );

		SANIMSTRIKE sStrike;
		sStrike.m_emPiece = PIECE_HAIR;
		sStrike.m_emEffect = EMSF_TARGET;
		sStrike.m_dwFrame = 0;

		if ( pSkill->m_sEXT_DATA.emSELFZONE01==SKILL::EMTIME_FIRST )
			SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

		if ( pSkill->m_sEXT_DATA.emSELFZONE02==SKILL::EMTIME_FIRST )
			SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

		// 주인에게 붙는 이펙트 발동(변경)
		if ( pTarChar )
		{
			STARGETID sTargetID( CROW_PC, pTarChar->m_dwGaeaID, pTarChar->GetPosition () );
			FACTEFF::NewSkillFactEffect ( sTargetID, pTarChar->GetCharSkin (), m_sActiveSkillID, pTarChar->GetMatrix (), sTargetID.vPos );
		}
	}
}

void GLAnyPet::MsgUpdateMoveState ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_UPDATE_MOVESTATE_BRD* pNetMsg = 
		( GLMSG::SNETPET_REQ_UPDATE_MOVESTATE_BRD* ) nmg;

	m_dwActionFlag = pNetMsg->dwFlag;

	if ( IsSTATE ( EM_PETACT_RUN ) )	m_actorMove.SetMaxSpeed ( m_fRunSpeed );
	else								m_actorMove.SetMaxSpeed ( m_fWalkSpeed );

	if ( IsSTATE ( EM_PETACT_MOVE ) )
	{
		if ( IsSTATE ( EM_PETACT_RUN ) ) m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
		else 							 m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
	}
}

void GLAnyPet::MsgRename ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_RENAME_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_RENAME_BRD* ) nmg;
	StringCchCopy ( m_szName, PETNAMESIZE+1, pNetMsg->szName );
}

void GLAnyPet::MsgChangeColor ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_CHANGE_COLOR_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_CHANGE_COLOR_BRD* ) nmg;
	m_wColor = pNetMsg->wColor;

	UpdateSuit ();
}

void GLAnyPet::MsgChangeStyle ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_CHANGE_STYLE_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_CHANGE_STYLE_BRD* ) nmg;

	m_wStyle = pNetMsg->wStyle;
	m_wColor = pNetMsg->wColor;
	UpdateSuit ();
}

void GLAnyPet::MsgChangeAccessory ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD* ) nmg;

	SetSlotItem ( pNetMsg->emSuit, pNetMsg->sItemCustom );
	UpdateSuit ();
}

void GLAnyPet::MsgLearnSkill ( NET_MSG_GENERIC* nmg )
{
	// 구동 스킬에 따른 이펙트 표시
	STARGETID sTargetID(CROW_PET, m_dwGUID, m_vPos);
	DxEffGroupPlayer::GetInstance().NewEffGroup
	(
		GLCONST_CHAR::strSKILL_LEARN_EFFECT.c_str(),
		m_matTrans,
		&sTargetID
	);
}

void GLAnyPet::MsgRemoveSlotItem ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_BRD* ) nmg;
	ReSetSlotItem ( pNetMsg->emSuit );
	UpdateSuit ();
}

void GLAnyPet::MsgAttack ( NET_MSG_GENERIC* nmg )
{
	// 이동중이면 이동을 마친후 공격 모션을 취한다.
	if ( m_actorMove.PathIsActive () ) return;

	GLMSG::SNETPET_ATTACK_BRD* pNetMsg = ( GLMSG::SNETPET_ATTACK_BRD* ) nmg;
	m_dwActionFlag = EM_PETACT_ATTACK;

	if ( pNetMsg->sTarID.emCrow == CROW_PC )
	{
		GLLandManClient* pLandMan = GLGaeaClient::GetInstance().GetActiveMap ();
		PGLCHARCLIENT pChar = pLandMan->GetChar ( pNetMsg->sTarID.dwID );
		if ( pChar )
		{
			// 방향 전환
			D3DXVECTOR3 vDirection = pChar->GetPosition () - m_vPos;
			D3DXVec3Normalize ( &m_vDir, &vDirection );
			m_vDir = vDirection;

			m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
		}
		// 다른 PC 가 아니면 플레이어 케릭터이다
		else
		{
			// 방향 전환
			D3DXVECTOR3 vDirection = GLGaeaClient::GetInstance().GetCharacter()->GetPosition () - m_vPos;
			D3DXVec3Normalize ( &m_vDir, &vDirection );
			m_vDir = vDirection;

			m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
		}
	}
	else if ( pNetMsg->sTarID.emCrow == CROW_MOB )
	{
		GLLandManClient* pLandMan = GLGaeaClient::GetInstance().GetActiveMap ();
		PGLCROWCLIENT pCrow = pLandMan->GetCrow ( pNetMsg->sTarID.dwID );
		if ( pCrow )
		{
			// 방향 전환
			D3DXVECTOR3 vDirection = pCrow->GetPosition () - m_vPos;
			D3DXVec3Normalize ( &m_vDir, &vDirection );
			m_vDir = vDirection;

			m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
		}
	}
	else if ( pNetMsg->sTarID.emCrow == CROW_SUMMON )
	{
		GLLandManClient* pLandMan = GLGaeaClient::GetInstance().GetActiveMap ();
		PGLCROWCLIENT pCrow = pLandMan->GetCrow ( pNetMsg->sTarID.dwID );
		if ( pCrow )
		{
			// 방향 전환
			D3DXVECTOR3 vDirection = pCrow->GetPosition () - m_vPos;
			D3DXVec3Normalize ( &m_vDir, &vDirection );
			m_vDir = vDirection;

			m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
		}
	}
}

void GLAnyPet::MsgSad ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_SAD_BRD* pNetMsg = ( GLMSG::SNETPET_SAD_BRD* ) nmg;
	
	GLLandManClient* pLandMan = GLGaeaClient::GetInstance().GetActiveMap ();
	PGLCHARCLIENT pChar = pLandMan->GetChar ( pNetMsg->dwOwner );
	if ( pChar )
	{
		m_dwActionFlag = EM_PETACT_SAD;

		// SAD 애니메이션
		if( IsUsePetSkinPack() && m_sPetSkinPackState.bNotFoundSad )
		{
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		}else{
			m_pSkinChar->SELECTANI ( AN_GESTURE, AN_SUB_04 );
		}
		
		// 방향 전환
		D3DXVECTOR3 vDirection = pChar->GetPosition () - m_vPos;
		D3DXVec3Normalize ( &m_vDir, &vDirection );
		m_vDir = vDirection;
	}
}

void GLAnyPet::MsgFunny ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_FUNNY_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_FUNNY_BRD* ) nmg;
	m_wAniSub = pNetMsg->wFunny;
	m_pSkinChar->SELECTANI ( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub );
}

void GLAnyPet::MsgAccessoryDelete ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_ACCESSORY_DELETE_BRD* pNetMsg = (GLMSG::SNETPET_ACCESSORY_DELETE_BRD*) nmg;
	m_PutOnItems[pNetMsg->accetype] = SITEMCUSTOM ();
	UpdateSuit ();
}

void GLAnyPet::MsgPetSkinPack ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_SKINPACKOPEN_BRD* pNetMsg = (GLMSG::SNETPET_SKINPACKOPEN_BRD*) nmg;

	m_sPetSkinPackState.bUsePetSkinPack = pNetMsg->sPetSkinPackData.bUsePetSkinPack;
	m_sPetSkinPackState.petSkinMobID	   = pNetMsg->sPetSkinPackData.sMobID;
	m_sPetSkinPackState.fPetScale	   = pNetMsg->sPetSkinPackData.fScale;

	// 생성 효과
	D3DXMATRIX matEffect;
	D3DXVECTOR3 vPos = GetPosition ();
	D3DXMatrixTranslation ( &matEffect, vPos.x, vPos.y, vPos.z );

	std::string strGEN_EFFECT = GLCONST_CHAR::strPET_GEN_EFFECT.c_str();
	STARGETID sTargetID(CROW_PET,m_dwGUID,vPos);
	DxEffGroupPlayer::GetInstance().NewEffGroup( strGEN_EFFECT.c_str(), matEffect, &sTargetID );				
	SkinLoad( m_pd3dDevice );

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	GetSkinChar()->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = ( m_vMaxOrg.y - m_vMinOrg.y ) * m_sPetSkinPackState.fPetScale;

	if( !IsUsePetSkinPack() ) UpdateSuit ();
}

void GLAnyPet::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
{
	D3DXMATRIX matTrans;

	//	스트라이크 지점에서.
	BOOL bTRANS(FALSE);
	if ( emPOS == SKILL::EMPOS_STRIKE )
	{
		//	타겟의 위치.
		D3DXVECTOR3 vTARPOS = m_vPos + m_vDir * 60.0f;

		//	타격 위치를 알아냄.
		STRIKE::SSTRIKE sStrike;
		STRIKE::CSELECTOR Selector ( vTARPOS, m_pSkinChar->m_PartArray );
		BOOL bOk = Selector.SELECT ( sStrikeEff, sStrike );
		if ( bOk )
		{
			bTRANS = TRUE;

			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
		}
	}

	//	지면기준 목표점에 회전 속성 넣어서.
	if ( !bTRANS )
	{
		bTRANS = TRUE;

		D3DXMATRIX matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * m_matTrans;
	}

	//	Note : 자기 위치 이펙트 발생시킴.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, pTarget );
}

void GLAnyPet::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_PET_GOTO_BRD:						MsgGoto ( nmg );			break;
	case NET_MSG_PET_REQ_UPDATE_MOVE_STATE_BRD:		MsgUpdateMoveState ( nmg ); break;
	case NET_MSG_PET_STOP_BRD:						MsgStop ( nmg );			break;
	case NET_MSG_PET_REQ_SKILLCHANGE_BRD:			MsgSkillChange ( nmg );		break;
	case NET_MSG_PET_REQ_RENAME_BRD:				MsgRename ( nmg );			break;
	case NET_MSG_PET_REQ_CHANGE_COLOR_BRD:			MsgChangeColor ( nmg );		break;
	case NET_MSG_PET_REQ_CHANGE_STYLE_BRD:			MsgChangeStyle ( nmg );		break;
	case NET_MSG_PET_REQ_SLOT_EX_HOLD_BRD:			MsgChangeAccessory ( nmg );	break;
	case NET_MSG_PET_REQ_LEARNSKILL_BRD:			MsgLearnSkill ( nmg );		break;
	case NET_MSG_PET_REQ_FUNNY_BRD:					MsgFunny ( nmg );			break;
	case NET_MSG_PET_REMOVE_SLOTITEM_BRD:			MsgRemoveSlotItem ( nmg );	break;
	case NET_MSG_PET_ATTACK_BRD:					MsgAttack ( nmg );			break;
	case NET_MSG_PET_SAD_BRD:						MsgSad ( nmg );				break;
	case NET_MSG_PET_ACCESSORY_DELETE_BRD:			MsgAccessoryDelete ( nmg ); break;
	case NET_MSG_PET_PETSKINPACKOPEN_BRD:			MsgPetSkinPack ( nmg );		break;

	default:
		CDebugSet::ToListView ( "[WARNING]GLPetField::MsgProcess Illigal Message(%d)", nmg->nType );
	  	break;
	};
}
