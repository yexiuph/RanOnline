#include "pch.h"
#include "GLPetClient.h"
#include "GLGaeaClient.h"
#include "GLContrlMsg.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/PetWindow.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UItextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "GLItemMan.h"
#include "DxEffGroupPlayer.h"
#include "GLFactEffect.h"

void GLPetClient::MsgGetRightOfItem ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_GETRIGHTOFITEM_FB* pNetMsg = ( GLMSG::SNETPET_REQ_GETRIGHTOFITEM_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_GETRIGHTOFITEM_FB_OK:
		{
			m_vecDroppedItems.clear();
			for ( WORD i = 0; i < pNetMsg->wSum; ++i )
			{
				m_vecDroppedItems.push_back ( pNetMsg->DroppedItems[i] );
			}
		}
		m_bSkillProcessing = true;
		break;
	};
}

void GLPetClient::MsgSkillChange ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_ADD_SKILLFACT* pNetMsg = ( GLMSG::SNETPET_ADD_SKILLFACT* ) nmg;

	m_pOwner->m_sPETSKILLFACT.sNATIVEID = pNetMsg->sNATIVEID;
	m_pOwner->m_sPETSKILLFACT.emTYPE	= pNetMsg->emTYPE;
	m_pOwner->m_sPETSKILLFACT.fMVAR		= pNetMsg->fMVAR;
	m_pOwner->m_sPETSKILLFACT.fAGE		= pNetMsg->fAge;

	// ���� ����Ʈ ����
	FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_pOwner->m_dwGaeaID,m_pOwner->GetPosition() ), m_pOwner->GetSkinChar (), m_sActiveSkillID );
	
	m_sActiveSkillID = pNetMsg->sNATIVEID;

	//	������ ��ų�� ��� ��ų ���� �߰���.
	if ( m_sActiveSkillID != SNATIVEID(false) )
	{
		// ������ Ÿ���̵ȴ�.
		STARGETID sTargetID( CROW_PC, m_pOwner->m_dwGaeaID, m_pOwner->GetPosition () );
		FACTEFF::NewSkillFactEffect ( sTargetID, m_pOwner->GetSkinChar (), m_sActiveSkillID, m_matTrans, sTargetID.vPos );
	}

	// ��ų����
	StartSkillProc ();

}

void GLPetClient::MsgLearnSkill ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_LEARNSKILL_FB* pNetMsg = ( GLMSG::SNETPET_REQ_LEARNSKILL_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_LEARNSKILL_FB_OK:
		{
			SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( !pInvenItem )	return;

			SITEM* pItem = GLItemMan::GetInstance().GetItem ( pInvenItem->sItemCustom.sNativeID );
			if ( !pItem )		return;

			// ��ų����
			LEARN_SKILL ( pItem->sSkillBookOp.sSkill_ID );

			// ��ī���� ������ �������ش�.
			PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find ( m_dwPetID );
			if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
			{
				SPETCARDINFO& sPet = (*iter).second;
				PETSKILL sPetSkill( pItem->sSkillBookOp.sSkill_ID, 0 );
				sPet.m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
			}

			// ��ų ��� ����Ʈ �߰�
			STARGETID sTargetID(CROW_PET, m_dwGUID, m_vPos);
			DxEffGroupPlayer::GetInstance().NewEffGroup
			(
				GLCONST_CHAR::strSKILL_LEARN_EFFECT.c_str(),
				m_matTrans,
				&sTargetID
			);

			// ��ų�޺��ڽ� ��������
			CInnerInterface::GetInstance().GetPetWindow()->SetSkillCombo();

			CInnerInterface::GetInstance().PrintMsgText ( 
				NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_OK"), m_szName );
		}
		break;
	case EMPET_REQ_LEARNSKILL_FB_FAIL:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_FAIL") );
		break;
	case EMPET_REQ_LEARNSKILL_FB_NOITEM:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_NOITEM") );
		break;
	case EMPET_REQ_LEARNSKILL_FB_BADITEM:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_BADITEM") );
		break;
	case EMPET_REQ_LEARNSKILL_FB_ALREADY:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_ALREADY") );
		break;
	};
}

void GLPetClient::MsgUpdateFull ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_UPDATE_CLIENT_PETFULL* pNetMsg = ( GLMSG::SNETPET_UPDATE_CLIENT_PETFULL* ) nmg;
	m_nFull = pNetMsg->nFull;

	// ��ī���� ������ �������ش�.
	PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find ( m_dwPetID );
	if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
	{
		SPETCARDINFO& sPet = (*iter).second;
		sPet.m_nFull = pNetMsg->nFull;
	}
}


void GLPetClient::MsgRename ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_RENAME_FB* pNetMsg = ( GLMSG::SNETPET_REQ_RENAME_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_RENAME_FB_FB_OK:
		{
			CInnerInterface::GetInstance().PrintMsgText ( 
				NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPET_REQ_RENAME_FB_FB_OK"), m_szName, pNetMsg->szName );
			StringCchCopy ( m_szName, PETNAMESIZE+1, pNetMsg->szName );

			// ��ī���� ������ �������ش�.
			PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find ( m_dwPetID );
			if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
			{
				SPETCARDINFO& sPet = (*iter).second;
				StringCchCopy ( sPet.m_szName, PETNAMESIZE+1, pNetMsg->szName );
			}
		}
		break;

	case EMPET_REQ_RENAME_FB_FB_FAIL:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FROM_DB_FAIL") );
		break;

	case EMPET_REQ_RENAME_FB_FB_NOITEM:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_NOITEM") );
		break;

	case EMPET_REQ_RENAME_FB_FB_BADITEM:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_BADITEM") );
		break;

	case EMPET_REQ_RENAME_FB_FB_LENGTH:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("NEWCHAR_NAME_TOO_SHORT") );	
		break;

	case EMPET_REQ_RENAME_FB_BAD_NAME:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("CHARACTER_BADNAME") );
		break;

	case EMPET_REQ_RENAME_FB_FROM_DB_FAIL:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FROM_DB_FAIL"), pNetMsg->szName );
		break;
	case EMPET_REQ_RENAME_FB_FB_THAICHAR_ERROR:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_THAICHAR_ERROR"));
		break;
	case EMPET_REQ_RENAME_FB_FB_VNCHAR_ERROR:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_VNCHAR_ERROR"));
		break;
	};
}

void GLPetClient::MsgChangeColor ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_CHANGE_COLOR_FB* pNetMsg = ( GLMSG::SNETPET_REQ_CHANGE_COLOR_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_COLOR_CHANGE_FB_OK:
		{
			m_wColor = pNetMsg->wColor;
			UpdateSuit ( FALSE );

			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPET_REQ_COLOR_CHANGE_FB_OK") );
		}
		break;

	case EMPET_REQ_STYLECOLOR_CHANGE_FB_FAIL:
		ColorChange( m_wColor );
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_FAIL") );
		break;

	case EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM:
		ColorChange( m_wColor );
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM") );
		break;

	case EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM:
		ColorChange( m_wColor );
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM") );
		break;
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_UNCHANGEABLE:
		ColorChange( m_wColor );
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_UNCHANGEABLE") );
		break;
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_COOLTIME:		
		ColorChange( m_wColor );
		break;
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK:
		ColorChange( m_wColor );
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK") );
		break;
	};
}

void GLPetClient::MsgChangeStyle ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_CHANGE_STYLE_FB* pNetMsg = ( GLMSG::SNETPET_REQ_CHANGE_STYLE_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_STYLE_CHANGE_FB_OK:
		{
			m_wStyle = pNetMsg->wStyle;
			m_wColor = pNetMsg->wColor;
			UpdateSuit ( TRUE );
			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPET_REQ_STYLE_CHANGE_FB_OK") );
		}
		break;

	case EMPET_REQ_STYLECOLOR_CHANGE_FB_FAIL:
		StyleChange( m_wStyle );
		ColorChange( m_wColor );
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_FAIL") );
		break;

	case EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM:
		StyleChange( m_wStyle );
		ColorChange( m_wColor );
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM") );
		break;

	case EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM:
		StyleChange( m_wStyle );
		ColorChange( m_wColor );
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM") );
		break;
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_COOLTIME:
		StyleChange( m_wStyle );
		ColorChange( m_wColor );
		break;
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK:
		StyleChange( m_wStyle );
		ColorChange( m_wColor );
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK") );
		break;
	};
}

void GLPetClient::MsgSlotExHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_FB* pNetMsg = ( GLMSG::SNETPET_REQ_SLOT_EX_HOLD_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_SLOT_EX_HOLD_FB_FAIL:

		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_OK:
		{
			// ������ ������ BackUp
			SITEMCUSTOM sSlotItemCustom = GetSlotitembySuittype ( pNetMsg->emSuit );

			// �տ��� �������� �ֿ��� ������Ű��
			SetSlotItem ( pNetMsg->emSuit, m_pOwner->GET_HOLD_ITEM () );

			// ��ī���� ������ �������ش�.
			PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find ( m_dwPetID );
			if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
			{
				SPETCARDINFO& sPet = (*iter).second;
				sPet.m_PutOnItems[(WORD)pNetMsg->emSuit-(WORD)SUIT_PET_A] = m_pOwner->GET_HOLD_ITEM ();
			}

			// �����ƴ� �������� �տ� ���
			m_pOwner->HOLD_ITEM ( sSlotItemCustom );

			UpdateSuit ( FALSE );
		}
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH:
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
		break;
	};
}

void GLPetClient::MsgHoldToSlot ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_HOLD_TO_SLOT_FB* pNetMsg = ( GLMSG::SNETPET_REQ_HOLD_TO_SLOT_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_SLOT_EX_HOLD_FB_FAIL:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_FAIL") );
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_OK:
		{
			// �տ��� �������� �ֿ��� ������Ű��
			SetSlotItem ( pNetMsg->emSuit, m_pOwner->GET_HOLD_ITEM () );

			// ��ī���� ������ �������ش�.
			PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find ( m_dwPetID );
			if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
			{
				SPETCARDINFO& sPet = (*iter).second;
				sPet.m_PutOnItems[(WORD)pNetMsg->emSuit-(WORD)SUIT_PET_A] = m_pOwner->GET_HOLD_ITEM ();
			}

			// �տ��� �������� �����ϰ�
			m_pOwner->RELEASE_HOLD_ITEM ();

			UpdateSuit ( FALSE );
		}
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH") );
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM") );
		break;
	};
}

void GLPetClient::MsgSlotToHold ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_SLOT_TO_HOLD_FB* pNetMsg = ( GLMSG::SNETPET_REQ_SLOT_TO_HOLD_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_SLOT_EX_HOLD_FB_FAIL:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_FAIL") );
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_OK:
		{
			// ������ ������ BackUp
			SITEMCUSTOM sSlotItemCustom = GetSlotitembySuittype ( pNetMsg->emSuit );

			// ������ ������ ����
			ReSetSlotItem ( pNetMsg->emSuit );

			// ��ī���� ������ �������ش�.
			PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find ( m_dwPetID );
			if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
			{
				SPETCARDINFO& sPet = (*iter).second;
				WORD i = (WORD)pNetMsg->emSuit-(WORD)SUIT_PET_A;
				if ( i < ACCETYPESIZE ) sPet.m_PutOnItems[i] = SITEMCUSTOM ( NATIVEID_NULL() );
			}

			// �����ƴ� �������� �տ� ���
			m_pOwner->HOLD_ITEM ( sSlotItemCustom );

			UpdateSuit ( FALSE );
		}
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH") );
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
		CInnerInterface::GetInstance().PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM") );
		break;
	};
}

void GLPetClient::MsgRemoveSlotItem ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_FB* pNetMsg = ( GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_FB* ) nmg;
	
	WORD wPosX(0), wPosY(0);

	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype ( pNetMsg->emSuit );

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

	// �κ��� �ֱ�
	m_pOwner->m_cInventory.InsertItem ( sSlotItemCustom, wPosX, wPosY );

	// ���Ծ����� ����
	ReSetSlotItem ( pNetMsg->emSuit );

	UpdateSuit ( FALSE );
}

void GLPetClient::MsgAttack ( NET_MSG_GENERIC* nmg )
{
	// �̵����̸� �̵��� ��ģ�� ���� ����� ���Ѵ�.
	if ( m_actorMove.PathIsActive () ) return;

	GLMSG::SNETPET_ATTACK* pNetMsg = ( GLMSG::SNETPET_ATTACK* ) nmg;

	if ( pNetMsg->sTarID.emCrow == CROW_PC )
	{
		GLLandManClient* pLandMan = GLGaeaClient::GetInstance().GetActiveMap ();
		PGLCHARCLIENT pChar = pLandMan->GetChar ( pNetMsg->sTarID.dwID );
		if ( pChar )
		{
			// ���� ��ȯ
			D3DXVECTOR3 vDirection = pChar->GetPosition () - m_vPos;
			D3DXVec3Normalize ( &m_vDir, &vDirection );
			m_vDir = vDirection;

			ReSetAllSTATE ();
			SetSTATE ( EM_PETACT_ATTACK );
			m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
		}
	}
	else if ( pNetMsg->sTarID.emCrow == CROW_MOB )
	{
		GLLandManClient* pLandMan = GLGaeaClient::GetInstance().GetActiveMap ();
		PGLCROWCLIENT pCrow = pLandMan->GetCrow ( pNetMsg->sTarID.dwID );
		if ( pCrow )
		{
			// ���� ��ȯ
			D3DXVECTOR3 vDirection = pCrow->GetPosition () - m_vPos;
			D3DXVec3Normalize ( &m_vDir, &vDirection );
			m_vDir = vDirection;

			ReSetAllSTATE ();
			SetSTATE ( EM_PETACT_ATTACK );
			m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
		}
	}
	else if ( pNetMsg->sTarID.emCrow == CROW_SUMMON )
	{
		GLLandManClient* pLandMan = GLGaeaClient::GetInstance().GetActiveMap ();
		PGLANYSUMMON pSummon = pLandMan->GetSummon ( pNetMsg->sTarID.dwID );
		if ( pSummon )
		{
			// ���� ��ȯ
			D3DXVECTOR3 vDirection = pSummon->GetPosition () - m_vPos;
			D3DXVec3Normalize ( &m_vDir, &vDirection );
			m_vDir = vDirection;

			ReSetAllSTATE ();
			SetSTATE ( EM_PETACT_ATTACK );
			m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
		}
	}

	// Ÿ�̸� �ʱ�ȭ
	m_fAttackTimer = 0.0f;
}

void GLPetClient::MsgSad ( NET_MSG_GENERIC* nmg )
{
	ReSetAllSTATE ();
	SetSTATE ( EM_PETACT_SAD );
	
	// SAD �ִϸ��̼�
	if( IsUsePetSkinPack() && m_sPetSkinPackState.bNotFoundSad )
	{
		m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
	}else{
		m_pSkinChar->SELECTANI ( AN_GESTURE, AN_SUB_04 );
	}
	
	// ���� ��ȯ
	D3DXVECTOR3 vDirection = m_pOwner->GetPosition () - m_vPos;
	D3DXVec3Normalize ( &m_vDir, &vDirection );
	m_vDir = vDirection;
}

void GLPetClient::MsgAccessoryDelete ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_ACCESSORY_DELETE* pNetMsg = (GLMSG::SNETPET_ACCESSORY_DELETE*) nmg;
	m_PutOnItems[pNetMsg->accetype] = SITEMCUSTOM ();
	UpdateSuit ( FALSE );
}



void GLPetClient::MsgRemoveSkillFact ( NET_MSG_GENERIC* nmg )
{
	// ����Ʈ ����
	FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_pOwner->m_dwGaeaID,m_pOwner->GetPosition() ), m_pOwner->GetSkinChar (), m_sActiveSkillID );

	m_pOwner->m_sPETSKILLFACT.RESET ();
	m_sActiveSkillID = NATIVEID_NULL();
	m_bSkillProcessing = false;

}

void GLPetClient::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_PET_GETRIGHTOFITEM_FB:							MsgGetRightOfItem ( nmg );
		break;

	case NET_MSG_PET_REQ_SKILLCHANGE_FB:						MsgSkillChange ( nmg );
		break;

	case NET_MSG_PET_REQ_RENAME_FB:								MsgRename ( nmg );
		break;

	case NET_MSG_PET_UPDATECLIENT_FULL:							MsgUpdateFull ( nmg );
		break;

	case NET_MSG_PET_REQ_CHANGE_COLOR_FB:						MsgChangeColor ( nmg );
		break;

	case NET_MSG_PET_REQ_CHANGE_STYLE_FB:						MsgChangeStyle ( nmg );
		break;

	case NET_MSG_PET_REQ_SLOT_EX_HOLD_FB:						MsgSlotExHold ( nmg );
		break;

	case NET_MSG_PET_REQ_HOLD_TO_SLOT_FB:						MsgHoldToSlot ( nmg );
		break;

	case NET_MSG_PET_REQ_SLOT_TO_HOLD_FB:						MsgSlotToHold ( nmg );
		break;

	case NET_MSG_PET_ADD_SKILLFACT:								MsgSkillChange ( nmg );
		break;

	case NET_MSG_PET_REMOVE_SKILLFACT:							MsgRemoveSkillFact ( nmg );
		break;

	case NET_MSG_PET_REQ_LEARNSKILL_FB:							MsgLearnSkill ( nmg );
		break;

	case NET_MSG_PET_REMOVE_SLOTITEM_FB:						MsgRemoveSlotItem ( nmg );
		break;

	case NET_MSG_PET_ATTACK:									MsgAttack ( nmg );
		break;

	case NET_MSG_PET_SAD:										MsgSad ( nmg );
		break;

	case NET_MSG_PET_ACCESSORY_DELETE:							MsgAccessoryDelete ( nmg );
		break;

	default:
		CDebugSet::ToListView ( "[WARNING]GLPetField::MsgProcess Illigal Message(%d)", nmg->nType );
	  	break;
	};
}