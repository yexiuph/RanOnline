#include "pch.h"
#include "GLPetClient.h"
#include "DxGlobalStage.h"
#include "GLGaeaClient.h"
#include "GLContrlMsg.h"
#include "GLItemMan.h"
#include "GLTradeClient.h"
#include "RanFilter.h"

#include "../[Lib]__Engine/Sources/Common/StringUtils.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextcontrol.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "../[Lib]__RanClientUI/Sources/ControlExUI/ModalWindow.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/ModalCallerID.h"

#include "GLCharDefine.h"
#include "DxEffGroupPlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GLPetClient::ReqGoto ( D3DXVECTOR3 vTarPos, bool bToPickUp )
{
	GLMSG::SNETPET_REQ_GOTO NetMsg;

	D3DXVECTOR3 vPos(vTarPos);

	// �÷��� ����
	ReSetAllSTATE ();

	// ���� �������� �ֿ췯 ���ٸ�
	if ( bToPickUp )
	{
		BOOL bGoto = m_actorMove.GotoLocation ( vTarPos+D3DXVECTOR3(0,+10,0),
							                vTarPos+D3DXVECTOR3(0,-10,0) );

		if ( !bGoto ) 
		{
			m_actorMove.Stop ();
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
			SetSTATE ( EM_PETACT_STOP );
			m_sSkillTARGET.RESET ();
			return;
		}

		// �������� �ֿﶧ�� ������ �پ��.
		SetSTATE ( EM_PETACT_MOVE );
		SetSTATE ( EM_PETACT_RUN_PICKUP_ITEM );
		SetMoveState ( IsSTATE ( EM_PETACT_MOVE ) );
	
		NetMsg.dwGUID  = m_dwGUID;
		NetMsg.vCurPos = m_vPos;
		NetMsg.vTarPos = vTarPos;
		NetMsg.dwFlag  = m_dwActionFlag;

		NETSENDTOFIELD ( &NetMsg );

		return;
	}

	// ������ �̵�Ÿ��
	STARGETID sTarget = m_pOwner->GetTargetID ();

	// ���κ��� �ռ�������
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetPosition ();
	D3DXVECTOR3 vDist = m_vPos - sTarget.vPos;
	float fPetToTar = D3DXVec3Length(&vDist);
	vDist = vOwnerPos - sTarget.vPos;
	float fOwnerToTar = D3DXVec3Length(&vDist);

	vDist = vOwnerPos - m_vPos;
	float fDist = D3DXVec3Length(&vDist);

	// �̵����϶��� ��ٷ��ش�
	if ( fDist < GLCONST_PET::fWalkArea && m_pOwner->IsACTION ( GLAT_MOVE ) )
	{
		m_actorMove.Stop ();
		m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		SetSTATE ( EM_PETACT_WAIT );

		GLMSG::SNETPET_REQ_STOP NetMsg;
		NetMsg.dwGUID = m_dwGUID;
		NetMsg.dwFlag = m_dwActionFlag;
		NetMsg.vPos	  = m_vPos;
		NETSENDTOFIELD ( &NetMsg );
		return;
	}

	BOOL bGoto = m_actorMove.GotoLocation ( vTarPos+D3DXVECTOR3(0,+10,0),
							                vTarPos+D3DXVECTOR3(0,-10,0) );

	// Ȥ�� ������ ���̸�
	if ( !bGoto )
	{
		for ( WORD i = 0; i < 8; ++i )
		{
			D3DXVECTOR3 vRandPos = GLPETDEFINE::RANDPOS[i];

			vTarPos = vPos + vRandPos;
			bGoto = m_actorMove.GotoLocation ( vTarPos+D3DXVECTOR3(0,+10,0),
							                   vTarPos+D3DXVECTOR3(0,-10,0) );
			if ( bGoto ) break;
		}
	}

	if ( !bGoto ) 
	{
		m_actorMove.Stop ();
		m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		SetSTATE ( EM_PETACT_STOP );
		return;
	}

	// [����] 
	// �̵��޽��� ���۽� �׻� ������ Run ���¸� ����� �Ѵ�.
	SetSTATE ( EM_PETACT_MOVE );
	SetMoveState ( m_pOwner->IsSTATE ( EM_ACT_RUN ) );

	NetMsg.dwGUID  = m_dwGUID;
	NetMsg.vCurPos = m_vPos;
	NetMsg.vTarPos = vTarPos;
	NetMsg.dwFlag  = m_dwActionFlag;

	NETSENDTOFIELD ( &NetMsg );
}

void GLPetClient::ReqStop ( bool bStopAttack )
{
	ReSetAllSTATE ();
	SetSTATE ( EM_PETACT_STOP );

	m_actorMove.Stop ();
	m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
	m_sSkillTARGET.RESET ();

	GLMSG::SNETPET_REQ_STOP NetMsg;
	NetMsg.dwGUID      = m_dwGUID;
	NetMsg.dwFlag      = m_dwActionFlag;
	NetMsg.vPos	       = m_vPos;
	NetMsg.bStopAttack = bStopAttack;
	NETSENDTOFIELD ( &NetMsg );
}

void GLPetClient::ReqGiveFood ( WORD wPosX, WORD wPosY )
{
	// ��Ȱ�� ���¿����� ��Ḧ �� �� �ִ�.
	//if ( !IsVALID() )							return;
	if ( GLTradeClient::GetInstance().Valid() )	return;

	GLCharacter* pOwner = GLGaeaClient::GetInstance().GetCharacter();
	SINVENITEM* pInvenItem = pOwner->m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem ) return;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem ) return;

	// ��ī�� ���� üũ
	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )	return;

	SITEM* pHold = pOwner->GET_SLOT_ITEMDATA ( SLOT_HOLD );
	if ( !pHold ) return;

	// ��� ���� üũ
	if ( pHold->sBasicOp.emItemType != ITEM_PET_FOOD )	return;

	SPETCARDINFO sPet;

	if ( !DxGlobalStage::GetInstance().IsEmulator() )
	{
		// ��ī���� ������ ������ ���� ���̴�.
		PETCARDINFO_MAP_ITER iter = pOwner->m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
		if ( iter==pOwner->m_mapPETCardInfo.end() ) return;
		sPet = (*iter).second;
	}
	else
	{
		sPet.m_nFull  = m_nFull;
		sPet.m_emTYPE = m_emTYPE;
	}

	// �������� ����á����
	if ( sPet.m_nFull >= 1000 ) return;

	GLMSG::SNETPET_REQ_GIVEFOOD NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.emPetTYPE = sPet.m_emTYPE;

	NETSENDTOFIELD ( &NetMsg );
}

void GLPetClient::ReqInputName ( WORD wPosX, WORD wPosY )
{
	if ( !IsVALID() )							return; // Ȱ�����¿����� �̸����� ����
	if ( GLTradeClient::GetInstance().Valid() )	return;

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )	return;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_RENAME_FB_FB_NOITEM") );
		return;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_PET_RENAME )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_RENAME_FB_FB_BADITEM") );
		return;
	}

	DoModal( ID2GAMEINTEXT("INVEN_CHAR_RENAME"), MODAL_INPUT, EDITBOX_PETNAME, MODAL_PET_RENAME );

	m_pOwner->m_wInvenPosX1 = wPosX;
	m_pOwner->m_wInvenPosY1 = wPosY;
}

void GLPetClient::ReqRename ( const char* szCharName )
{
	if( !szCharName ) return;

	CString strTEMP( szCharName );

#ifdef TH_PARAM	
	// �±��� ���� ���� üũ

	if ( !m_pCheckString ) return;
	
	if ( !m_pCheckString(strTEMP) )
	{
		CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_THAICHAR_ERROR"));
		return;
	}
#endif

#ifdef VN_PARAM
	// ��Ʈ�� ���� ���� üũ 
	if( STRUTIL::CheckVietnamString( strTEMP ) )
	{
		CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_VNCHAR_ERROR"));
		return;
	}

#endif 



	BOOL bFILTER0 = STRUTIL::CheckString( strTEMP );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strTEMP );
	if ( bFILTER0 || bFILTER1 )
	{
		CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("CHARACTER_BADNAME") );
		return;
	}

	if ( strlen ( szCharName ) > PETNAMESIZE )
	{
		// �ʹ� ���.
		CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_RENAME_FB_TOOLONG") );
		return;
	}

	GLMSG::SNETPET_REQ_RENAME NetMsg;
	NetMsg.wPosX = m_pOwner->m_wInvenPosX1;
	NetMsg.wPosY = m_pOwner->m_wInvenPosY1;
	StringCchCopy ( NetMsg.szName, PETNAMESIZE+1, szCharName );
	NETSENDTOFIELD ( &NetMsg );

	m_pOwner->m_wInvenPosX1 = 0;
	m_pOwner->m_wInvenPosY1 = 0;
}

void GLPetClient::ReqInputColor ( WORD wPosX, WORD wPosY )
{
	if ( !IsVALID() )							return; // Ȱ�����¿����� ���� ����
	if ( GLTradeClient::GetInstance().Valid() )	return;

	if ( !GLCONST_PET::IsChangeablePetColor ( m_emTYPE ) )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_UNCHANGEABLE") );
		return;
	}

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )	return;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM") );
		return;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_PET_COLOR )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM") );
		return;
	}

	if ( IsUsePetSkinPack() )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK") );
		return;
	}

	DoModal( ID2GAMEINTEXT("MODAL_PETCOLOR_INFO"), MODAL_INFOMATION, OK, MODAL_PETCOLOR_INFO );

	m_pOwner->m_wInvenPosX1 = wPosX;
	m_pOwner->m_wInvenPosY1 = wPosY;
}
void GLPetClient::ReqChangeColor ( WORD wColor )
{
	GLMSG::SNETPET_REQ_CHANGE_COLOR NetMsg;
	NetMsg.wColor = wColor;
	NetMsg.wPosX  = m_pOwner->m_wInvenPosX1;
	NetMsg.wPosY  = m_pOwner->m_wInvenPosY1;
	NETSENDTOFIELD ( &NetMsg );

	m_pOwner->m_wInvenPosX1 = 0;
	m_pOwner->m_wInvenPosY1 = 0;
}
void GLPetClient::ReqInputStyle ( WORD wPosX, WORD wPosY )
{
	if ( !IsVALID() )							return; // Ȱ�����¿����� ���� ����
	if ( GLTradeClient::GetInstance().Valid() )	return;

	PETSTYLE sPetStyle = GLCONST_PET::GetPetStyle ( m_emTYPE );
	if ( sPetStyle.wSTYLENum == 0 )				return; // chf ������ ����!!

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )	return;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM") );
		return;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_PET_STYLE )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM") );
		return;
	}

	if ( IsUsePetSkinPack() )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK") );
		return;
	}

	CInnerInterface::GetInstance().ShowGroupFocus( PETSTYLECARD_WINDOW );

	m_pOwner->m_wInvenPosX2 = wPosX;
	m_pOwner->m_wInvenPosY2 = wPosY;
}
void GLPetClient::ReqChangeStyle ( WORD wStyle )
{
	GLMSG::SNETPET_REQ_CHANGE_STYLE NetMsg;
	NetMsg.wStyle = wStyle;
	NetMsg.wPosX  = m_pOwner->m_wInvenPosX2;
	NetMsg.wPosY  = m_pOwner->m_wInvenPosY2;
	NETSENDTOFIELD ( &NetMsg );

	m_pOwner->m_wInvenPosX2 = 0;
	m_pOwner->m_wInvenPosY2 = 0;
}

// ������ Ŭ���ϱ⸸ �ϸ� ȣ��Ǿ����� �Ѵ�.
void GLPetClient::ReqChangeAccessory ( EMSUIT emSUIT )
{
	if ( GLTradeClient::GetInstance().Valid() )		return;
	//if ( emSUIT == ACCETYPESIZE )					return;

	//	Note : �������̳� ��ų ���� �߿� ���� ������ ���� �� �� ���ٰ� ��.
	//
	if ( m_pOwner->IsACTION(GLAT_ATTACK) || m_pOwner->IsACTION(GLAT_SKILL) ) return;

	SITEMCUSTOM sHoldItemCustom = m_pOwner->GET_HOLD_ITEM ();
	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype ( emSUIT );

	//	SLOT <-> HOLD
	if ( sHoldItemCustom.sNativeID != NATIVEID_NULL() && sSlotItemCustom.sNativeID != NATIVEID_NULL() )
	{
		SITEM* pHoldItem = GLItemMan::GetInstance().GetItem ( sHoldItemCustom.sNativeID );
		if ( !pHoldItem ) 
		{
			// �Ϲ� ����
			return;
		}

		if ( !CheckSlotItem( pHoldItem->sBasicOp.sNativeID, emSUIT ) ) return;
		
		SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );
		if ( !pSlotItem ) 
		{
			// �Ϲ� ����
			return;
		}

		// Ÿ���� �ٸ���
		if ( pHoldItem->sSuitOp.emSuit != pSlotItem->sSuitOp.emSuit ) return;

		GLMSG::SNETPET_REQ_SLOT_EX_HOLD NetMsg;
		NetMsg.emSuit = emSUIT;
		NETSENDTOFIELD ( &NetMsg );
	}
	// HOLD -> SLOT
	else if ( sHoldItemCustom.sNativeID != NATIVEID_NULL() )
	{
		SITEM* pHoldItem = GLItemMan::GetInstance().GetItem ( sHoldItemCustom.sNativeID );
		if ( !pHoldItem ) 
		{
			// �Ϲ� ����
			return;
		}

		if ( !CheckSlotItem( pHoldItem->sBasicOp.sNativeID, emSUIT ) ) return;

		// Ÿ���� �ٸ���
		if ( pHoldItem->sSuitOp.emSuit != emSUIT ) return;

		GLMSG::SNETPET_REQ_HOLD_TO_SLOT NetMsg;
		NetMsg.emSuit = emSUIT;
		NETSENDTOFIELD ( &NetMsg );
	}
	// SLOT -> HOLD
	else if ( sSlotItemCustom.sNativeID != NATIVEID_NULL() )
	{
		GLMSG::SNETPET_REQ_SLOT_TO_HOLD NetMsg;
		NetMsg.emSuit = emSUIT;
		NETSENDTOFIELD ( &NetMsg );
	}
}

void GLPetClient::ReqRemoveSlotItem ( EMSUIT emSuit )
{
	WORD wPosX(0), wPosY(0);
	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype ( emSuit );

	SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );
	if ( !pSlotItem ) 
	{
		// �Ϲ� ����
		return;
	}

	BOOL bOk = m_pOwner->m_cInventory.FindInsrtable ( pSlotItem->sBasicOp.wInvenSizeX, pSlotItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
	if ( !bOk )
	{
		//	�ι��� ��������.
		return;
	}

	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM NetMsg;
	NetMsg.emSuit = emSuit;

	NETSENDTOFIELD ( &NetMsg );
}

void GLPetClient::ReqChangeActiveSkill ( SNATIVEID sSkillID )
{
	// ��ų ��Ҹ� ����
	if ( sSkillID == NATIVEID_NULL() )
	{
		GLMSG::SNETPET_REQ_SKILLCHANGE NetMsg;
		NetMsg.sSkillID = NATIVEID_NULL();
		NETSENDTOFIELD ( &NetMsg );
		return;
	}
	
	if ( !CheckSkill ( sSkillID ) )
	{
		// ����� �ʾҰų� ��Ÿ�� �����
		return;
	}

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	if ( !pSkill )	return;

	STARGETID sTARG( CROW_PET,m_dwGUID, m_vPos );

	//	Note : ��ų ���۰� ���ÿ� ��Ÿ���� ����Ʈ. �ߵ�.
	//
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

	GLMSG::SNETPET_REQ_SKILLCHANGE NetMsg;
	NetMsg.sSkillID = sSkillID;
	NETSENDTOFIELD ( &NetMsg );
}


void GLPetClient::ReqLearnSkill ( WORD wPosX, WORD wPosY )
{
	if ( !IsVALID () )							return;
	if ( GLTradeClient::GetInstance().Valid() )	return;
	//if( m_pOwner->ValidRebuildOpen() )			return;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )	return;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );

	// �ֿ� ��ų���� ����????
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_PET_SKILL )	return;

	SNATIVEID sSKILL_ID = pItem->sSkillBookOp.sSkill_ID;

	if ( ISLEARNED_SKILL( sSKILL_ID ) )
	{
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_ALREADY") );
		return;
	}

	//	Note : ��ų ������ ��û�մϴ�.
	//
	GLMSG::SNETPET_REQ_LEARNSKILL NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	
	NETSENDTOFIELD ( &NetMsg );

	return;
}

void GLPetClient::ReqLearnSkill ( DWORD dwChannel, WORD wPosX, WORD wPosY )
{
	if ( !IsVALID () )								return;
	if ( !m_pOwner->IsVALID_STORAGE(dwChannel) )	return;
	if ( GLTradeClient::GetInstance().Valid() )		return;
	//if( m_pOwner->ValidRebuildOpen() )			return;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_pOwner->m_cStorage[dwChannel].FindPosItem ( wPosX, wPosY );
	if ( !pInvenItem )							return;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
	
	// �ֿ� ��ų���� ����????
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_PET_SKILL )	return;

	SNATIVEID sSKILL_ID = pItem->sSkillBookOp.sSkill_ID;

	if ( ISLEARNED_SKILL( sSKILL_ID ) )
	{
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_ALREADY") );
		return;
	}

	//	Note : ��ų ������ ��û�մϴ�.
	//
	GLMSG::SNETPET_REQ_LEARNSKILL NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.bStorage = true;
	NetMsg.dwChannel = dwChannel;
	
	NETSENDTOFIELD ( &NetMsg );

	return;
}