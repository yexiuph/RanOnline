#include "pch.h"
#include "./GLCharacter.h"
#include "./GLGaeaClient.h"
#include "./GLPartyClient.h"

#include "../DxGlobalStage.h"

#include "DxInputDevice.h"
#include "editmeshs.h"
#include "DxMethods.h"
#include "DxViewPort.h"
#include "DxShadowMap.h"
#include "EditMeshs.h"
#include "GLogicData.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/ModalCallerID.h"
#include "../[Lib]__RanClientUI/Sources/ControlExUI/ModalWindow.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/DamageDisplay.h"
#include "stl_Func.h"

#include "GLFactEffect.h"
#include "GlFriendClient.h"
#include "GLCrowRenList.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/NameDisplayMan.h"

#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/MapRequireCheck.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/CharacterWindow.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/QBoxButton.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/ItemSearchResultWindow.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundLib.h"
#include "../[Lib]__Engine/Sources/DxEffect/DxEffectMan.h"
#include "../[Lib]__Engine/Sources/DxEffect/DxEffGroupPlayer.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/WaitServerDialogue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT GLCharacter::UpdateClientState ( GLMSG::SNETPC_UPDATE_STATE *pNetMsg )
{
	//pNetMsg->DECODE ( m_dwGaeaID );
	m_sHP = pNetMsg->sHP;
	m_sMP = pNetMsg->sMP;
	m_sSP = pNetMsg->sSP;

	m_fIncHP = 0.0f;
	m_fIncMP = 0.0f;
	m_fIncSP = 0.0f;

	return S_OK;
}

void GLCharacter::DisableSkillFact()
{

	EMSLOT emRHand = GetCurRHand();	
	SITEM* pRightItem = GET_SLOT_ITEMDATA(emRHand);	

	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		if ( m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL() ) continue;
	
        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
		if ( !pSkill ) continue;

		// 스킬 자신 버프
		if ( pSkill->m_sBASIC.emIMPACT_TAR != TAR_SELF || pSkill->m_sBASIC.emIMPACT_REALM != REALM_SELF ) continue;
				
		GLITEM_ATT emSKILL_RITEM = pSkill->m_sBASIC.emUSE_RITEM;

		// 스킬 도구 종속 없음
		if ( emSKILL_RITEM == ITEMATT_NOCARE )	continue;

		// 스킬 과 무기가 불일치
		if( !pRightItem || !CHECHSKILL_ITEM(emSKILL_RITEM,pRightItem->sSuitOp.emAttack) )
		{
			FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );
			DISABLESKEFF( i );
		}
	}
}


void GLCharacter::ReceiveDamage ( WORD wDamage, DWORD dwDamageFlag, STARGETID sACTOR )
{
	if ( dwDamageFlag & DAMAGE_TYPE_SHOCK )	ReceiveShock ();
	
	//	Note : 화면에 타격 값 출력.
	D3DXVECTOR3 vPos = GetPosBodyHeight();
	CInnerInterface::GetInstance().SetDamage( vPos, wDamage, dwDamageFlag, UI_UNDERATTACK );

	if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
	{
		// 강한타격 이펙트
		D3DXVECTOR3 vDIR = sACTOR.vPos - m_vPos;

		D3DXVECTOR3 vDIR_ORG(1,0,0);
		float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

		D3DXMATRIX matTrans;
		D3DXMatrixRotationY ( &matTrans, fdir_y );
		matTrans._41 = m_vPos.x;
		matTrans._42 = m_vPos.y + 10.0f;
		matTrans._43 = m_vPos.z;

		//	Note : 자기 위치 이펙트 발생시킴.
		DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR );
	}

	//	Note : 타격값 반영.
	GLCHARLOGIC::RECEIVE_DAMAGE ( wDamage );

	//	Note : 방어스킬의 이펙트가 있을때 발동시킴.
	SKT_EFF_HOLDOUT ( sACTOR, dwDamageFlag );
}

void GLCharacter::ReceiveAVoid ()
{
	D3DXVECTOR3 vPos = GetPosBodyHeight();
	CInnerInterface::GetInstance().SetDamage( vPos, 0, DAMAGE_TYPE_NONE, UI_ATTACK );
}

void GLCharacter::ReceiveSwing ()
{
	if ( IsValidBody() && !IsCtrlBlockBody() )		m_pSkinChar->DOSHOCKMIX();
}

void GLCharacter::ReceiveShock ()
{
	if ( IsValidBody() && !IsCtrlBlockBody() )		TurnAction ( GLAT_SHOCK );
}

void GLCharacter::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_SUMMON_ATTACK_AVOID:
		{
			GLMSG::SNET_SUMMON_ATTACK_AVOID *pNetMsg = (GLMSG::SNET_SUMMON_ATTACK_AVOID *) nmg;
			GLCOPY* pActor = GLGaeaClient::GetInstance().GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
			if ( pActor )	pActor->ReceiveAVoid ();
		}
		break;
	case NET_MSG_GCTRL_SUMMON_ATTACK_DAMAGE:
		{
			GLMSG::SNET_SUMMON_ATTACK_DAMAGE *pNetMsg = (GLMSG::SNET_SUMMON_ATTACK_DAMAGE *) nmg;

			GLCOPY* pActor = GLGaeaClient::GetInstance().GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
			if ( pActor )
			{
				STARGETID sACTOR(GetCrow(),GetCtrlID(),GetPosition());
				//pActor->ReceiveDamage ( pNetMsg->nDamage, pNetMsg->bShock, pNetMsg->bCritical, sACTOR );
				pActor->ReceiveDamage ( pNetMsg->nDamage, pNetMsg->dwDamageFlag, sACTOR );
			}

			if ( (pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRITICAL ) || ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW ) )	
				DxViewPort::GetInstance().CameraQuake ( 0.5f, 1.0f/25.0f, 1.0f );
		}
		break;
	case NET_MSG_GCTRL_ATTACK_AVOID:
		{
			GLMSG::SNETPC_ATTACK_AVOID *pNetMsg = (GLMSG::SNETPC_ATTACK_AVOID *) nmg;
			GLCOPY* pActor = GLGaeaClient::GetInstance().GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
			if ( pActor )	pActor->ReceiveAVoid ();
		}
		break;

	case NET_MSG_GCTRL_ATTACK_DAMAGE:
		{
			GLMSG::SNETPC_ATTACK_DAMAGE *pNetMsg = (GLMSG::SNETPC_ATTACK_DAMAGE *) nmg;
			
			GLCOPY* pActor = GLGaeaClient::GetInstance().GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
			if ( pActor )
			{
				STARGETID sACTOR(GetCrow(),GetCtrlID(),GetPosition());	
				pActor->ReceiveDamage ( pNetMsg->nDamage, pNetMsg->dwDamageFlag, sACTOR );
			}

			if ( (pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRITICAL ) || ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW ) )	
				DxViewPort::GetInstance().CameraQuake ( 0.5f, 1.0f/25.0f, 1.0f );
		}
		break;

	case NET_MSG_GCTRL_DEFENSE_SKILL_ACTIVE:
		{
			MsgDefenseSkillActive( (GLMSG::SNETPC_DEFENSE_SKILL_ACTIVE*)nmg );
		}
		break;

	case NET_MSG_GCTRL_ACTION_BRD:
		{
			GLMSG::SNET_ACTION_BRD *pNetMsg = (GLMSG::SNET_ACTION_BRD *)nmg;
			
			if ( pNetMsg->emAction==GLAT_FALLING && m_Action>=GLAT_FALLING )	break;

			TurnAction ( pNetMsg->emAction );
		}
		break;

	case NET_MSG_GCTRL_UPDATE_STATE:
		{
			GLMSG::SNETPC_UPDATE_STATE *pNetMsg = (GLMSG::SNETPC_UPDATE_STATE *)nmg;
			UpdateClientState ( pNetMsg );
		}
		break;

	case NET_MSG_GCTRL_UPDATE_EXP:
		{
			GLMSG::SNETPC_UPDATE_EXP *pNetMsg = (GLMSG::SNETPC_UPDATE_EXP *)nmg;

			if ( pNetMsg->lnNowExp < m_sExperience.lnNow )
			{
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("DIS_EXP"), m_sExperience.lnNow-pNetMsg->lnNowExp );
			}
			else if ( pNetMsg->lnNowExp > m_sExperience.lnNow )
			{
				#ifndef NDEBUG
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("INC_EXP"), pNetMsg->lnNowExp-m_sExperience.lnNow );
				#endif
			}

			m_sExperience.lnNow = pNetMsg->lnNowExp;
		}
		break;

	case NET_MSG_GCTRL_UPDATE_MONEY:
		{
			GLMSG::SNETPC_UPDATE_MONEY *pNetMsg = (GLMSG::SNETPC_UPDATE_MONEY *)nmg;

			m_lnMoney = pNetMsg->lnMoney;
		}
		break;

	case NET_MSG_GCTRL_UPDATE_SP:
		{
			GLMSG::SNETPC_UPDATE_SP *pNetMsg = (GLMSG::SNETPC_UPDATE_SP *)nmg;

			m_sSP.wNow = pNetMsg->wNowSP;
		}
		break;

	case NET_MSG_GCTRL_UPDATE_LP:
		{
			GLMSG::SNETPC_UPDATE_LP *pNetMsg = (GLMSG::SNETPC_UPDATE_LP *)nmg;

			int nDxLiving = pNetMsg->nLP - m_nLiving;
			m_nLiving = pNetMsg->nLP;
			
			if ( nDxLiving == 0 )	break;

			if ( nDxLiving > 0 )
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("INC_LIVING"), nDxLiving );
			else
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("DIS_LIVING"), nDxLiving );
		}	
		break;

	case NET_MSG_GCTRL_UPDATE_SKP:
		{
			GLMSG::SNETPC_UPDATE_SKP *pNetMsg = (GLMSG::SNETPC_UPDATE_SKP *)nmg;

			int nDx = int(pNetMsg->dwSkillPoint) - int(m_dwSkillPoint);

			m_dwSkillPoint = pNetMsg->dwSkillPoint;

			if ( nDx > 0 )
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("INC_SKP"), nDx );
			else
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("DIS_SKP"), nDx );
		}
		break;

	case NET_MSG_GCTRL_UPDATE_BRIGHT:
		{
			GLMSG::SNETPC_UPDATE_BRIGHT *pNetMsg = (GLMSG::SNETPC_UPDATE_BRIGHT *)nmg;

			int nDx = int(pNetMsg->nBright) - int(m_nBright);

			m_nBright = pNetMsg->nBright;
		
			if ( nDx > 0 )
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("INC_BRIGHT"), nDx );
			else
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("DIS_BRIGHT"), nDx );
		}
		break;

	case NET_MSG_GCTRL_UPDATE_STATS:
		{
			GLMSG::SNETPC_UPDATE_STATS *pNetMsg = (GLMSG::SNETPC_UPDATE_STATS *)nmg;

			int nDx = int(pNetMsg->wStatsPoint) - int(m_wStatsPoint);

			m_wStatsPoint = pNetMsg->wStatsPoint;

			if ( nDx > 0 )
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("INC_STATS"), nDx );
			else
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("DIS_STATS"), nDx );
		}
		break;

	case NET_MSG_GCTRL_PICKUP_MONEY:
		{
			GLMSG::SNETPC_PICKUP_MONEY *pNetMsg = (GLMSG::SNETPC_PICKUP_MONEY *)nmg;
		
			m_lnMoney = pNetMsg->lnMoney;
			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("PICKUP_MONEY"), pNetMsg->lnPickUp );
			DxSoundLib::GetInstance()->PlaySound ( "PICKUP_ITEM" );
		}
		break;

	case NET_MSG_GCTRL_PICKUP_ITEM:
		{
			GLMSG::SNETPC_PICKUP_ITEM *pNetMsg = (GLMSG::SNETPC_PICKUP_ITEM *)nmg;
		
			SITEM * pITEM = GLItemMan::GetInstance().GetItem ( pNetMsg->sNID_ITEM );
			if ( pITEM )
			{
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("PICKUP_ITEM"), pITEM->GetName() );
				DxSoundLib::GetInstance()->PlaySound ( "PICKUP_ITEM" );
			}
		} 
		break;

	case NET_MSG_GCTRL_REQ_HOLD_FB:
		{
			GLMSG::SNETPC_REQ_HOLD_FB *pNetMsg = (GLMSG::SNETPC_REQ_HOLD_FB *)nmg;
			
			switch ( pNetMsg->emHoldMsg )
			{
			case NET_MSG_GCTRL_REQ_HOLD_TO_STORAGE:
				{
					switch ( pNetMsg->emHoldFB )
					{
					case EMHOLD_FB_NONKEEPSTORAGE:
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("STORAGE_SPAN_END") );
						break;
					};
				}
				break;
			};
		};
		break;

	case NET_MSG_GCTRL_INVEN_INSERT:
		{
			GLMSG::SNETPC_INVEN_INSERT *pNetMsg = (GLMSG::SNETPC_INVEN_INSERT *)nmg;

#if defined(VN_PARAM) //vietnamtest%%%
			if( pNetMsg->bVietnamInven )
			{
				m_cVietnamInventory.InsertItem ( pNetMsg->Data.sItemCustom, pNetMsg->Data.wPosX, pNetMsg->Data.wPosY );
			}else{
				m_cInventory.InsertItem ( pNetMsg->Data.sItemCustom, pNetMsg->Data.wPosX, pNetMsg->Data.wPosY, pNetMsg->bAllLine );
			}
#else
			m_cInventory.InsertItem ( pNetMsg->Data.sItemCustom, pNetMsg->Data.wPosX, pNetMsg->Data.wPosY, pNetMsg->bAllLine );
#endif

			// PET
			// 팻카드 정보를 요청한다.
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( pNetMsg->Data.sItemCustom.sNativeID );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && pNetMsg->Data.sItemCustom.dwPetID != 0 )
			{
				GLMSG::SNETPET_REQ_PETCARDINFO NetMsg;
				NetMsg.dwPetID = pNetMsg->Data.sItemCustom.dwPetID;
				NETSENDTOFIELD ( &NetMsg );
			}

			// VEHICLE
			// 탈것 정보를 요청한다.
			pItem = GLItemMan::GetInstance().GetItem ( pNetMsg->Data.sItemCustom.sNativeID );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_VEHICLE && pNetMsg->Data.sItemCustom.dwVehicleID != 0)
			{
				GLMSG::SNET_VEHICLE_REQ_ITEM_INFO NetMsg;
				NetMsg.dwVehicleID = pNetMsg->Data.sItemCustom.dwVehicleID;
				NETSENDTOFIELD ( &NetMsg );
			}
		}
		break;

	case NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN_FB:
		{
			GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN_FB *pNetMsg = (GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN_FB *)nmg;

			m_cInventory.InsertItem ( pNetMsg->Data.sItemCustom, pNetMsg->wNewPosX, pNetMsg->wNewPosY );
			m_cVietnamInventory.DeleteItem( pNetMsg->wPosX, pNetMsg->wPosY );
		}
		break;

	case NET_MSG_GCTRL_INVEN_DELETE:
		{
			GLMSG::SNETPC_INVEN_DELETE *pNetMsg = (GLMSG::SNETPC_INVEN_DELETE *)nmg;
/*
			// PET
			// 팻카드 정보를 제거해준다.
			SINVENITEM* pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( pInvenItem )
			{
				PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
				if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
			}
*/
#if defined(VN_PARAM) //vietnamtest%%%
			if( pNetMsg->bVietnamInven )
			{
				m_cVietnamInventory.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			}else{
				m_cInventory.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			}
#else
			m_cInventory.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );
#endif
		}
		break;

	case NET_MSG_GCTRL_ITEM_COOLTIME_UPDATE:
		{
			GLMSG::SNETPC_ITEM_COOLTIME_UPDATE *pNetMsg = (GLMSG::SNETPC_ITEM_COOLTIME_UPDATE *)nmg;

			ITEM_COOLTIME sCoolTime;
			sCoolTime.dwID	= pNetMsg->dwID;
			sCoolTime.dwCoolID	= pNetMsg->dwCoolID;
			sCoolTime.tCoolTime	= pNetMsg->tCoolTime;
			sCoolTime.tUseTime	= pNetMsg->tUseTime;

			SetCoolTime ( sCoolTime, pNetMsg->emCoolType );
		}
		break;

	case NET_MSG_GCTRL_ITEM_COOLTIME_ERROR:
		{
			GLMSG::SNET_ITEM_COOLTIME_ERROR *pNetMsg = (GLMSG::SNET_ITEM_COOLTIME_ERROR *)nmg;
			
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( pNetMsg->sNativeID );
			if ( !pItem )	break;

			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, 
				ID2GAMEINTEXT("ITEM_COOLTIME"), pItem->GetName() );
		}
		break;
	case NET_MSG_GCTRL_INVEN_DEL_INSERT:
		{
			GLMSG::SNETPC_INVEN_DEL_AND_INSERT *pNetMsg = (GLMSG::SNETPC_INVEN_DEL_AND_INSERT *)nmg;
/*
			// PET
			// 팻카드 정보를 제거해준다.
			SINVENITEM* pInvenItem = m_cInventory.GetItem ( pNetMsg->wDelX, pNetMsg->wDelX );
			if ( pInvenItem )
			{
				PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
				if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
			}
*/
#if defined(VN_PARAM) //vietnamtest%%%
			if( pNetMsg->bVietnamInven )
			{
				m_cVietnamInventory.DeleteItem ( pNetMsg->wDelX, pNetMsg->wDelY );
				m_cVietnamInventory.InsertItem ( pNetMsg->sInsert.sItemCustom, pNetMsg->sInsert.wPosX, pNetMsg->sInsert.wPosY );
			}else{
				m_cInventory.DeleteItem ( pNetMsg->wDelX, pNetMsg->wDelY );
				m_cInventory.InsertItem ( pNetMsg->sInsert.sItemCustom, pNetMsg->sInsert.wPosX, pNetMsg->sInsert.wPosY );

				// PET
				// 팻카드 정보를 요청한다.
				SITEM* pItem = GLItemMan::GetInstance().GetItem ( pNetMsg->sInsert.sItemCustom.sNativeID );
				if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && pNetMsg->sInsert.sItemCustom.dwPetID != 0  )
				{
					GLMSG::SNETPET_REQ_PETCARDINFO NetMsg;
					NetMsg.dwPetID = pNetMsg->sInsert.sItemCustom.dwPetID;
					NETSENDTOFIELD ( &NetMsg );
				}

				// VEHICLE
				// 탈것 정보를 요청한다.
				pItem = GLItemMan::GetInstance().GetItem ( pNetMsg->sInsert.sItemCustom.sNativeID );
				if ( pItem && pItem->sBasicOp.emItemType == ITEM_VEHICLE && pNetMsg->sInsert.sItemCustom.dwVehicleID != 0 ) 
				{
					GLMSG::SNET_VEHICLE_REQ_ITEM_INFO NetMsg;
					NetMsg.dwVehicleID = pNetMsg->sInsert.sItemCustom.dwVehicleID;
					NETSENDTOFIELD ( &NetMsg );
				}
			}
#else
			m_cInventory.DeleteItem ( pNetMsg->wDelX, pNetMsg->wDelY );
			m_cInventory.InsertItem ( pNetMsg->sInsert.sItemCustom, pNetMsg->sInsert.wPosX, pNetMsg->sInsert.wPosY );
			// PET
			// 팻카드 정보를 요청한다.
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( pNetMsg->sInsert.sItemCustom.sNativeID );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && pNetMsg->sInsert.sItemCustom.dwPetID != 0 )
			{
				GLMSG::SNETPET_REQ_PETCARDINFO NetMsg;
				NetMsg.dwPetID = pNetMsg->sInsert.sItemCustom.dwPetID;
				NETSENDTOFIELD ( &NetMsg );
			}

			// VEHICLE
			// 탈것 정보를 요청한다.
			pItem = GLItemMan::GetInstance().GetItem ( pNetMsg->sInsert.sItemCustom.sNativeID );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_VEHICLE && pNetMsg->sInsert.sItemCustom.dwVehicleID != 0 )
			{
				GLMSG::SNET_VEHICLE_REQ_ITEM_INFO NetMsg;
				NetMsg.dwVehicleID = pNetMsg->sInsert.sItemCustom.dwVehicleID;
				NETSENDTOFIELD ( &NetMsg );
			}
#endif

			

		}
		break;

	case NET_MSG_GCTRL_PUTON_RELEASE:
		{
			GLMSG::SNETPC_PUTON_RELEASE *pNetMsg = (GLMSG::SNETPC_PUTON_RELEASE *)nmg;

			EMITEM_TYPE emTYPE_RELOAD = ITEM_NSIZE;
			SNATIVEID sNID_RELOAD = NATIVEID_NULL();
			EMSLOT emLHand = GetCurLHand();

			if ( pNetMsg->bRefresh )
			{
				//	재장전할 아이탬인지 알아봄.
				if ( pNetMsg->emSlot == emLHand && GLCHARLOGIC::VALID_SLOT_ITEM(pNetMsg->emSlot) )
				{
					SITEMCUSTOM sItemCustom = GLCHARLOGIC::GET_SLOT_ITEM(pNetMsg->emSlot);
					SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemCustom.sNativeID);
		
					if ( pItem->sBasicOp.emItemType==ITEM_ARROW || pItem->sBasicOp.emItemType==ITEM_CHARM )
					{
						emTYPE_RELOAD = pItem->sBasicOp.emItemType;
						sNID_RELOAD = sItemCustom.sNativeID;
					}
				}
			}

			//	SLOT 변경.
			GLCHARLOGIC::RELEASE_SLOT_ITEM ( pNetMsg->emSlot );
			GLCHARLOGIC::INIT_DATA ( FALSE, FALSE );

			//	형상 변경.
			UpdateSuit( TRUE );

			//	Note : 에니메이션 초기화.
			ReSelectAnimation();

			if ( emTYPE_RELOAD!=ITEM_NSIZE )
			{
				SINVENITEM* pInvenItem = m_cInventory.FindItem ( emTYPE_RELOAD, sNID_RELOAD );
				if ( !pInvenItem )	break;

				GLMSG::SNETPC_REQ_INVEN_TO_SLOT NetMsg;
				NetMsg.emToSlot = emLHand;
				NetMsg.wPosX = pInvenItem->wPosX;
				NetMsg.wPosY = pInvenItem->wPosY;

				NETSENDTOFIELD ( &NetMsg );
			}

			// 탈것 아이템이면 정보 비활성
			if ( pNetMsg->emSlot == SLOT_VEHICLE )
			{
				m_sVehicle.SetActiveValue( false );
				m_sVehicle.RESET ();
			}
		}
		break;

	case NET_MSG_GCTRL_PUTON_UPDATE:
		{
			GLMSG::SNETPC_PUTON_UPDATE *pNetMsg = (GLMSG::SNETPC_PUTON_UPDATE *)nmg;

			// 화살이나 부적같은 아이템이 자동 옮겨지는 경우
			if ( pNetMsg->emSlotRelease!=SLOT_TSIZE )
			{
				GLCHARLOGIC::RELEASE_SLOT_ITEM ( pNetMsg->emSlotRelease );
			}

			//	Note : 아이템 착용 요구치에 부족할 경우 정보 출력, SP 충당 수치.
			//

			//	SLOT 변경.
			GLCHARLOGIC::SLOT_ITEM ( pNetMsg->sItemCustom, pNetMsg->emSlot );

			if ( pNetMsg->emSlot != SLOT_HOLD )
			{
				WORD wACP = GLCHARLOGIC::CALC_ACCEPTP ( pNetMsg->sItemCustom.sNativeID );
				if ( wACP > 0 )		CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEMPUTON_ACCEPT_SP"), wACP );	

				GLCHARLOGIC::INIT_DATA ( FALSE, FALSE );

				//	형상 변경.
				UpdateSuit( TRUE );
				
				//	Note : 에니메이션 초기화.
				ReSelectAnimation();				
			}

			// 무기에 따른 버프를 초기화 한다.
			DisableSkillFact();

			// 탈것 아이템 장착시 정보를 가져옵니다.
			if ( pNetMsg->emSlot == SLOT_VEHICLE )
			{
				ReqVehicleUpdate();
			}
		}
		break;

	case NET_MSG_GCTRL_PUTON_CHANGE:
		{
			GLMSG::SNETPC_PUTON_CHANGE *pNetMsg = (GLMSG::SNETPC_PUTON_CHANGE *)nmg;

			// 주/보조무기 셋팅
			SetUseArmSub( pNetMsg->bUseArmSub );

			// 인터페이스 변경
			CInnerInterface::GetInstance().SetArmSwap( !pNetMsg->bUseArmSub );

			GLCHARLOGIC::INIT_DATA ( FALSE, FALSE, pNetMsg->fCONFT_HP_RATE );

			//	형상 변경.
			UpdateSuit( TRUE );

			//	Note : 에니메이션 초기화.
			ReSelectAnimation();

			// 무기에 따른 버프를 초기화 한다.
			DisableSkillFact();

		}
		break;

	case NET_MSG_GCTRL_REQ_LEVELUP_FB:
		{
			GLMSG::SNETPC_REQ_LEVELUP_FB *pNetMsg = (GLMSG::SNETPC_REQ_LEVELUP_FB *)nmg;

			STARGETID sTargetID(CROW_PC,m_dwGaeaID,m_vPos);
			DxEffGroupPlayer::GetInstance().NewEffGroup
			(
				GLCONST_CHAR::strLEVELUP_EFFECT.c_str(),
				m_matTrans,
				&sTargetID
			);

			LEVLEUP ( pNetMsg->bInitNowLevel );

			//	Note : 렙업후에 변경된 수치 적용.
			m_wLevel = pNetMsg->wLevel;
			m_wStatsPoint = pNetMsg->wStatsPoint;
			m_dwSkillPoint = pNetMsg->dwSkillPoint;

			// 주석돌려
			if ( m_dwUserLvl < USER_GM3 )
			{
				SMAPNODE *pMapNode = GLGaeaClient::GetInstance().FindMapNode ( pNetMsg->sMapID );
				if ( pMapNode )
				{
					GLLevelFile cLevelFile;
					if( cLevelFile.LoadFile ( pMapNode->strFile.c_str(), TRUE, NULL ) )

					{
						SLEVEL_REQUIRE* pRequire = cLevelFile.GetLevelRequire ();
						EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
						if( pRequire ) emReqFail = pRequire->ISCOMPLETE ( this );
						if( emReqFail == EMREQUIRE_LEVEL )
						{
							CInnerInterface::GetInstance().GetRequireCheck()->SetMapCheckType( EMMAPCHECK_LEVELUP );
							CInnerInterface::GetInstance().ShowGroupFocus ( MAP_REQUIRE_CHECK );
						}
					}

				}
			}		 
			


#ifndef CH_PARAM_USEGAIN 
			//** Add EventTime
			if( m_bEventStart )
			{
				if( m_bEventApply )
				{
					if( m_wLevel < m_EventStartLv || m_wLevel > m_EventEndLv )
					{
						m_bEventApply = FALSE;
						CInnerInterface::GetInstance().BONUS_TIME_EVENT_END();
					}
				}else{
					if( m_wLevel >= m_EventStartLv && m_wLevel <= m_EventEndLv )
					{
						m_bEventApply = TRUE;
						CTime crtTime = GLGaeaClient::GetInstance().GetCurrentTime();
						m_tLoginTime =  crtTime.GetTime();
						CInnerInterface::GetInstance().BONUS_TIME_EVENT_START( !m_bEventBuster );
					}
				}
			}
#endif

			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("LEVELUP"), m_wLevel );
		}
		break;

	case NET_MSG_GCTRL_REQ_STATSUP_FB:
		{
			GLMSG::SNETPC_REQ_STATSUP_FB *pNetMsg = (GLMSG::SNETPC_REQ_STATSUP_FB *)nmg;
			STATSUP(pNetMsg->emStats);
		}
		break;

	case NET_MSG_GCTRL_REQ_LEARNSKILL_FB:
		{
			GLMSG::SNETPC_REQ_LEARNSKILL_FB *pNetMsg = (GLMSG::SNETPC_REQ_LEARNSKILL_FB *)nmg;

			if ( pNetMsg->emCHECK == EMSKILL_OK )
			{
				//	Note : 스킬 정보 가져옴.
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pNetMsg->skill_id );
				if ( !pSkill )											break;

				m_ExpSkills.insert ( std::make_pair ( pNetMsg->skill_id.dwID, SCHARSKILL(pNetMsg->skill_id,0) ) );

				//	Note : 페시브 스킬이 변화 할때 초기 수치들을 모두 재 계산한다.
				//
				if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
				{
					INIT_DATA ( FALSE, FALSE );
				}

				STARGETID sTargetID(CROW_PC,m_dwGaeaID,m_vPos);
				DxEffGroupPlayer::GetInstance().NewEffGroup
				(
					GLCONST_CHAR::strSKILL_LEARN_EFFECT.c_str(),
					m_matTrans,
					&sTargetID
				);
			}
			else
			{
				//	결과 값에 따라 에러 메시지 출력.
			}
		}
		break;

	case NET_MSG_GCTRL_REQ_SKILLUP_FB:
		{
			GLMSG::SNETPC_REQ_SKILLUP_FB *pNetMsg = (GLMSG::SNETPC_REQ_SKILLUP_FB *)nmg;
			if ( pNetMsg->emCHECK == EMSKILL_LEARN_OK )
			{
				//	Note : 스킬 정보 가져옴.
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pNetMsg->sSkill.sNativeID );
				if ( !pSkill )											break;

				SKILL_MAP_ITER iter_del = m_ExpSkills.find ( pNetMsg->sSkill.sNativeID.dwID );
				if ( iter_del!=m_ExpSkills.end() )	m_ExpSkills.erase ( iter_del );

				m_ExpSkills[pNetMsg->sSkill.sNativeID.dwID] = pNetMsg->sSkill;

				//	Note : 페시브 스킬이 변화 할때 초기 수치들을 모두 재 계산한다.
				//
				if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
				{
					INIT_DATA ( FALSE, FALSE );
				}

				STARGETID sTargetID(CROW_PC,m_dwGaeaID,m_vPos);
				DxEffGroupPlayer::GetInstance().NewEffGroup
				(
					GLCONST_CHAR::strSKILL_UP_EFFECT.c_str(),
					m_matTrans,
					&sTargetID
				);
			}
		}
		break;

	case NET_MSG_GCTRL_REQ_SKILL_FB:
		{
			GLMSG::SNETPC_REQ_SKILL_FB *pNetMsg = (GLMSG::SNETPC_REQ_SKILL_FB *)nmg;
			if ( pNetMsg->emSKILL_FB != EMSKILL_OK )
			{
				if( pNetMsg->emSKILL_FB == EMSKILL_NOTREBIRTH )
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("NON_REBIRTH_SKILL_MSG"), pNetMsg->szName );
				}else{
					if ( IsACTION(GLAT_SKILL) )		TurnAction(GLAT_IDLE);
				}
			}
		}
		break;

	case NET_MSG_REQ_SKILL_REVIVEL_FAILED:
		{
			GLMSG::SNET_MSG_REQ_SKILL_REVIVEL_FAILED *pNetMsg = (GLMSG::SNET_MSG_REQ_SKILL_REVIVEL_FAILED *)nmg;
			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("NON_REBIRTH_SKILL_MSG_TARGET"), pNetMsg->szName );
		}
		break;

	case NET_MSG_GCTRL_SKILLCONSUME_FB:
		{
			GLMSG::SNETPC_SKILLCONSUME_FB *pNetMsg = (GLMSG::SNETPC_SKILLCONSUME_FB *)nmg;

			EMSLOT emLHand = GetCurLHand();
			
			SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
			if ( pItem && pItem->sDrugOp.bInstance )		m_PutOnItems[emLHand].wTurnNum = pNetMsg->wTurnNum;
			m_sHP.wNow = pNetMsg->wNowHP;
			m_sMP.wNow = pNetMsg->wNowMP;
			m_sSP.wNow = pNetMsg->wNowSP;
		}
		break;

	case NET_MSG_GCTRL_SKILLFACT_BRD:
		{
			GLMSG::SNETPC_SKILLFACT_BRD *pNetMsg = (GLMSG::SNETPC_SKILLFACT_BRD *)nmg;

			if( pNetMsg->emCrow != CROW_PC ) break;

			m_sHP.VARIATION ( pNetMsg->nVAR_HP );
			m_sMP.VARIATION ( pNetMsg->nVAR_MP );
			m_sSP.VARIATION ( pNetMsg->nVAR_SP );

			//	데미지 메시지.
			if ( pNetMsg->nVAR_HP < 0 )
			{
				if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_SHOCK )	ReceiveShock ();
				else					ReceiveSwing ();

				D3DXVECTOR3 vPos = GetPosBodyHeight();
				CInnerInterface::GetInstance().SetDamage( vPos, static_cast<WORD>(-pNetMsg->nVAR_HP), pNetMsg->dwDamageFlag, UI_UNDERATTACK );

				//	Note : 방어스킬의 이펙트가 있을때 발동시킴.
				STARGETID sACTOR(pNetMsg->sACTOR.GETCROW(),pNetMsg->sACTOR.GETID());
				sACTOR.vPos = GLGaeaClient::GetInstance().GetTargetPos ( sACTOR );

				SKT_EFF_HOLDOUT ( sACTOR, pNetMsg->dwDamageFlag );

				if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
				{
					// 강한타격 이펙트

					D3DXVECTOR3 vDIR = sACTOR.vPos - m_vPos;

					D3DXVECTOR3 vDIR_ORG(1,0,0);
					float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

					D3DXMATRIX matTrans;
					D3DXMatrixRotationY ( &matTrans, fdir_y );
					matTrans._41 = m_vPos.x;
					matTrans._42 = m_vPos.y + 10.0f;
					matTrans._43 = m_vPos.z;

					//	Note : 자기 위치 이펙트 발생시킴.
					DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR );
				}

			}

			//	힐링 메시지.
			//if ( pNetMsg->nVAR_HP > 0 )
			//{
			//	CPlayInterface::GetInstance().InsertText ( GetPosition(), static_cast<WORD>(pNetMsg->nVAR_HP), pNetMsg->bCRITICAL, 1 );
			//}
		}
		break;

	case NET_MSG_GCTRL_SKILLHOLD_BRD:
		{
			GLMSG::SNETPC_SKILLHOLD_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_BRD *)nmg;

			//	지속형 스킬의 경우 스킬 팩터 추가됨.
			if ( pNetMsg->skill_id != NATIVEID_NULL() )
			{
				RECEIVE_SKILLFACT ( pNetMsg->skill_id, pNetMsg->wLEVEL, pNetMsg->wSELSLOT );
				FACTEFF::NewSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, pNetMsg->skill_id, m_matTrans, m_vDir );
			}
		}
		break;

	case NET_MSG_GCTRL_SKILLHOLD_RS_BRD:
		{
			GLMSG::SNETPC_SKILLHOLD_RS_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_RS_BRD *)nmg;

			//	Note : 스킬 fact 들을 종료.
			//		바로 리샛 하지 않고 여기서 시간 조종하여 정상 종료되게 함. ( 이팩트 종료 때문. )
			for ( int i=0; i<SKILLFACT_SIZE; ++i )
			{
				if ( pNetMsg->bRESET[i] )
				{
					FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );

					DISABLESKEFF(i);
				}
			}
		}
		break;

	case NET_MSG_GCTRL_SKILLHOLDEX_BRD:
		{
			GLMSG::SNETPC_REQ_SKILLHOLDEX_BRD *pNetMsg = (GLMSG::SNETPC_REQ_SKILLHOLDEX_BRD *)nmg;
			m_sSKILLFACT[pNetMsg->wSLOT] = pNetMsg->sSKILLEF;
		}
		break;

	case NET_MSG_GCTRL_SKILL_CANCEL_BRD:
		{
			if ( IsACTION(GLAT_SKILL) )	TurnAction ( GLAT_IDLE );
		}
		break;

	case NET_MSG_GCTRL_STATEBLOW_BRD:
		{
			GLMSG::SNETPC_STATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_STATEBLOW_BRD *)nmg;

			if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )
				FACTEFF::DeleteBlowSingleEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSTATEBLOWS );

			SSTATEBLOW *pSTATEBLOW = NULL;
			if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )		pSTATEBLOW = &m_sSTATEBLOWS[0];
			else										pSTATEBLOW = &m_sSTATEBLOWS[pNetMsg->emBLOW-EMBLOW_SINGLE];

			pSTATEBLOW->emBLOW = pNetMsg->emBLOW;
			pSTATEBLOW->fAGE = pNetMsg->fAGE;
			pSTATEBLOW->fSTATE_VAR1 = pNetMsg->fSTATE_VAR1;
			pSTATEBLOW->fSTATE_VAR2 = pNetMsg->fSTATE_VAR2;

			//	Note : 효과 생성.
			FACTEFF::NewBlowEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, pSTATEBLOW->emBLOW, m_matTrans, m_vDir );
		}
		break;

	case NET_MSG_GCTRL_CURESTATEBLOW_BRD:
		{
			GLMSG::SNETPC_CURESTATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_CURESTATEBLOW_BRD *)nmg;

			for ( int i=0; i<EMBLOW_MULTI; ++i )
			{
				EMSTATE_BLOW emBLOW = m_sSTATEBLOWS[i].emBLOW;

				if ( emBLOW==EMBLOW_NONE )						continue;

				EMDISORDER emDIS = STATE_TO_DISORDER(emBLOW);
				if ( !(pNetMsg->dwCUREFLAG&emDIS) )				continue;

				DISABLEBLOW(i);
				FACTEFF::DeleteBlowEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, emBLOW );
			}
		}
		break;

	case NET_MSG_GCTRL_INVEN_DRUG_UPDATE:
		{
			GLMSG::SNETPC_INVEN_DRUG_UPDATE *pNetMsg = (GLMSG::SNETPC_INVEN_DRUG_UPDATE *)nmg;
			SINVENITEM* pInvenItem = NULL;
#if defined(VN_PARAM) //vietnamtest%%%
			if( pNetMsg->bVietnamInven )
			{
				pInvenItem = m_cVietnamInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			}else{
				pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			}
			if ( pInvenItem )
			{
				pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
			}
#else
			pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( pInvenItem )
			{
				pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
			}
#endif
			
		}
		break;

	case NET_MSG_GCTRL_PUTON_DRUG_UPDATE:
		{
			GLMSG::SNETPC_PUTON_DRUG_UPDATE *pNetMsg = (GLMSG::SNETPC_PUTON_DRUG_UPDATE *)nmg;
			m_PutOnItems[pNetMsg->emSlot].wTurnNum = pNetMsg->wTurnNum;
		}
		break;

	case NET_MSG_GCTRL_REQ_GETSTORAGE_FB:
		{
			GLMSG::SNETPC_REQ_GETSTORAGE_FB *pNetMsg = (GLMSG::SNETPC_REQ_GETSTORAGE_FB *)nmg;
			
			//	Note : 창고 정보 새로 받기 시도.
			//
			const DWORD dwChannel = pNetMsg->dwChannel;

			m_lnStorageMoney = pNetMsg->lnMoney;

			m_bStorage[dwChannel] = false;
			m_dwNumStorageItem[dwChannel] = pNetMsg->dwNumStorageItem;
			m_cStorage[dwChannel].DeleteItemAll ();

			if ( m_cStorage[dwChannel].GetNumItems() == m_dwNumStorageItem[dwChannel] )
			{
				m_bStorage[dwChannel] = true;
				m_dwNumStorageItem[dwChannel] = UINT_MAX;
			}
		}
		break;

	case NET_MSG_GCTRL_REQ_GETSTORAGE_ITEM:
		{
			GLMSG::SNETPC_REQ_GETSTORAGE_ITEM *pNetMsg = (GLMSG::SNETPC_REQ_GETSTORAGE_ITEM *)nmg;

			const SINVENITEM &sInvenItem = pNetMsg->Data;
			const DWORD dwChannel = pNetMsg->dwChannel;

			m_cStorage[dwChannel].InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY );

			if ( m_cStorage[dwChannel].GetNumItems() == m_dwNumStorageItem[dwChannel] )
			{
				m_bStorage[dwChannel] = true;
				m_dwNumStorageItem[dwChannel] = UINT_MAX;
			}

			// PET
			// 팻카드 정보를 요청한다.
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( sInvenItem.sItemCustom.sNativeID );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && sInvenItem.sItemCustom.dwPetID != 0 )
			{
				GLMSG::SNETPET_REQ_PETCARDINFO NetMsg;
				NetMsg.dwPetID = sInvenItem.sItemCustom.dwPetID;
				NETSENDTOFIELD ( &NetMsg );
			}
		}
		break;

	case NET_MSG_GCTRL_STORAGE_INSERT:
		{
			GLMSG::SNETPC_STORAGE_INSERT *pNetMsg = (GLMSG::SNETPC_STORAGE_INSERT *)nmg;

			const SINVENITEM &sInvenItem = pNetMsg->Data;
			const DWORD dwChannel = pNetMsg->dwChannel;

			m_cStorage[dwChannel].InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY );

			// PET
			// 팻카드 정보를 요청한다.
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( sInvenItem.sItemCustom.sNativeID );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && sInvenItem.sItemCustom.dwPetID != 0 )
			{
				GLMSG::SNETPET_REQ_PETCARDINFO NetMsg;
				NetMsg.dwPetID = sInvenItem.sItemCustom.dwPetID;
				NETSENDTOFIELD ( &NetMsg );
			}
		}
		break;

	case NET_MSG_GCTRL_STORAGE_DELETE:
		{
			GLMSG::SNETPC_STORAGE_DELETE *pNetMsg = (GLMSG::SNETPC_STORAGE_DELETE *)nmg;

			const DWORD dwChannel = pNetMsg->dwChannel;
/*
			// PET
			// 팻카드 정보를 제거해준다.
			SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( pInvenItem )
			{
				PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
				if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
			}
*/
			m_cStorage[dwChannel].DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );
		}
		break;

	case NET_MSG_GCTRL_STORAGE_ITEM_UPDATE:
		{
			GLMSG::SNETPC_STORAGE_ITEM_UPDATE *pNetMsg = (GLMSG::SNETPC_STORAGE_ITEM_UPDATE *)nmg;
			const DWORD dwChannel = pNetMsg->dwChannel;

			SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( !pInvenItem )		break;
			if ( pInvenItem->sItemCustom.sNativeID != pNetMsg->sItemCustom.sNativeID )	break;

			pInvenItem->sItemCustom = pNetMsg->sItemCustom;
		}
		break;

	case NET_MSG_GCTRL_STORAGE_DEL_INSERT:
		{
			GLMSG::SNETPC_STORAGE_DEL_AND_INSERT *pNetMsg = (GLMSG::SNETPC_STORAGE_DEL_AND_INSERT *)nmg;

			DWORD dwChannel = pNetMsg->dwChannel;
/*
			// PET
			// 팻카드 정보를 제거해준다.
			SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wDelX, pNetMsg->wDelX );
			if ( pInvenItem )
			{
				PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
				if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
			}
*/
			m_cStorage[dwChannel].DeleteItem ( pNetMsg->wDelX, pNetMsg->wDelY );
			m_cStorage[dwChannel].InsertItem ( pNetMsg->sInsert.sItemCustom, pNetMsg->sInsert.wPosX, pNetMsg->sInsert.wPosY );

			// PET
			// 팻카드 정보를 요청한다.
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( pNetMsg->sInsert.sItemCustom.sNativeID );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && pNetMsg->sInsert.sItemCustom.dwPetID != 0  )
			{
				GLMSG::SNETPET_REQ_PETCARDINFO NetMsg;
				NetMsg.dwPetID = pNetMsg->sInsert.sItemCustom.dwPetID;
				NETSENDTOFIELD ( &NetMsg );
			}
		}
		break;

	case NET_MSG_GCTRL_STORAGE_DRUG_UPDATE:
		{
			GLMSG::SNETPC_STORAGE_DRUG_UPDATE *pNetMsg = (GLMSG::SNETPC_STORAGE_DRUG_UPDATE *)nmg;
			
			DWORD dwChannel = pNetMsg->dwChannel;

			SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( pInvenItem )
			{
				pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
			}
		}
		break;

	case NET_MSG_GCTRL_STORAGE_UPDATE_MONEY:
		{
			GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY *pNetMsg = (GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY *)nmg;
			m_lnStorageMoney = pNetMsg->lnMoney;
		}
		break;

	case NET_MSG_GCTRL_REQ_SKILLQ_FB:
		{
			GLMSG::SNETPC_REQ_SKILLQUICK_FB *pNetMsg = (GLMSG::SNETPC_REQ_SKILLQUICK_FB *)nmg;
			m_sSKILLQUICK[pNetMsg->wSLOT] = pNetMsg->skill_id;

			//	Note : 런 스킬로 지정된 스킬이 존제하지 않을 경우 지정해줌.
			if ( GetskillRunSlot()==NATIVEID_NULL() )
			{
				ReqSkillRunSet(pNetMsg->wSLOT);
			}
		}
		break;

	case NET_MSG_GCTRL_REQ_ACTIONQ_FB:
		{
			GLMSG::SNETPC_REQ_ACTIONQUICK_FB *pNetMsg = (GLMSG::SNETPC_REQ_ACTIONQUICK_FB *)nmg;
			m_sACTIONQUICK[pNetMsg->wSLOT] = pNetMsg->sACT;
		}
		break;

	case NET_MSG_GCTRL_INVEN_ITEM_UPDATE:
		{
			GLMSG::SNET_INVEN_ITEM_UPDATE *pNetMsg = (GLMSG::SNET_INVEN_ITEM_UPDATE *)nmg;
			SINVENITEM* pInvenItem = NULL;
#if defined(VN_PARAM) //vietnamtest%%%
			if( pNetMsg->bVietnamInven )
			{
				pInvenItem = m_cVietnamInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			}else{
				pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			}
#else
			pInvenItem = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
#endif
			if ( !pInvenItem )		break;
			if ( pInvenItem->sItemCustom.sNativeID != pNetMsg->sItemCustom.sNativeID )	break;

			pInvenItem->sItemCustom = pNetMsg->sItemCustom;
		}
		break;

	case NET_MSG_GCTRL_INVEN_GRINDING_FB:
		{
			GLMSG::SNET_INVEN_GRINDING_FB *pNetMsg = (GLMSG::SNET_INVEN_GRINDING_FB *)nmg;

			switch ( pNetMsg->emGrindFB )
			{
			case EMGRINDING_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMGRINDING_FAIL") );

				if( pNetMsg->emANTIDISAPPEAR == EMANTIDISAPPEAR_USE )
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMGRINDING_ANTIDISAPPEAR") );
					DxSoundLib::GetInstance()->PlaySound ( "GRINDING_FAIL" );
				}
				else if ( pNetMsg->bTERMINATE )
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMGRINDING_TERMINATE") );
					DxSoundLib::GetInstance()->PlaySound ( "GRINDING_BROKEN" );
				}
				else if ( pNetMsg->bRESET )
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMGRINDING_RESET") );
					DxSoundLib::GetInstance()->PlaySound ( "GRINDING_RESET" );
				}
				else
				{
					DxSoundLib::GetInstance()->PlaySound ( "GRINDING_FAIL" );
				}
				break;

			case EMGRINDING_SUCCEED:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EMGRINDING_SUCCEED") );
					DxSoundLib::GetInstance()->PlaySound ( "GRINDING_SUCCEED" );
				}
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_BOXOPEN_FB:
		{
			GLMSG::SNET_INVEN_BOXOPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_BOXOPEN_FB *)nmg;
			switch ( pNetMsg->emFB )
			{
			case EMREQ_BOXOPEN_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_FAIL") );
				break;
			case EMREQ_BOXOPEN_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_OK") );
				break;
			case EMREQ_BOXOPEN_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOITEM") );
				break;
			case EMREQ_BOXOPEN_FB_NOBOX:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOBOX") );
				break;
			case EMREQ_BOXOPEN_FB_EMPTY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_EMPTY") );
				break;
			case EMREQ_BOXOPEN_FB_NOTINVEN:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOTINVEN") );
				break;
			case EMREQ_BOXOPEN_FB_INVALIDITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_INVALIDITEM") );
				break;
			};
		}
		break;
	
	case NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN_FB:
		{
			GLMSG::SNET_INVEN_RANDOMBOXOPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_RANDOMBOXOPEN_FB *)nmg;
			
			switch ( pNetMsg->emFB )
			{
			case EMINVEN_RANDOMBOXOPEN_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_FAIL") );
				break;

			case EMINVEN_RANDOMBOXOPEN_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_OK") );
				break;

			case EMINVEN_RANDOMBOXOPEN_FB_EMPTY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_EMPTY") );
				break;

			case EMINVEN_RANDOMBOXOPEN_FB_BADITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_BADITEM") );
				break;

			case EMINVEN_RANDOMBOXOPEN_FB_NOINVEN:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_NOINVEN") );
				break;

			case EMINVEN_RANDOMBOXOPEN_FB_MISS:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_MISS") );
				break;
			};
		}
		break;


	case NET_MSG_GCTRL_INVEN_DISJUNCTION_FB:
		{
			GLMSG::SNET_INVEN_DISJUNCTION_FB *pNetMsg = (GLMSG::SNET_INVEN_DISJUNCTION_FB *)nmg;
			
			switch ( pNetMsg->emFB )
			{
			case EMINVEN_DISJUNCTION_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_FAIL") );
				break;

			case EMINVEN_DISJUNCTION_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_OK") );
				break;

			case EMINVEN_DISJUNCTION_FB_BADITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_BADITEM") );
				break;

			case EMINVEN_DISJUNCTION_FB_NOINVEN:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_NOINVEN") );
				break;

			case EMINVEN_DISJUNCTION_FB_NONEED:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_NONEED") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_DISGUISE_FB:
		{
			GLMSG::SNET_INVEN_DISGUISE_FB *pNetMsg = (GLMSG::SNET_INVEN_DISGUISE_FB *)nmg;
		
			switch ( pNetMsg->emFB )
			{
			case EMREQ_DISGUISE_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_FAIL") );
				break;
			case EMREQ_DISGUISE_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_OK") );
				break;
			case EMREQ_DISGUISE_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_NOITEM") );
				break;
			case EMREQ_DISGUISE_FB_NODISGUISE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_NODISGUISE") );
				break;
			case EMREQ_DISGUISE_FB_NOTSUIT:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_NOTSUIT") );
				break;
			case EMREQ_DISGUISE_FB_DEFSUIT:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_DEFSUIT") );
				break;
			case EMREQ_DISGUISE_FB_ALREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_ALREADY") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_CLEANSER_FB:
		{
			GLMSG::SNET_INVEN_CLEANSER_FB *pNetMsg = (GLMSG::SNET_INVEN_CLEANSER_FB *)nmg;
		
			switch ( pNetMsg->emFB )
			{
			case EMREQ_CLEANSER_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_FAIL") );
				break;
			case EMREQ_CLEANSER_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_OK") );
				break;
			case EMREQ_CLEANSER_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_NOITEM") );
				break;
			case EMREQ_CLEANSER_FB_NOCLEANSER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_NOCLEANSER") );
				break;
			case EMREQ_CLEANSER_FB_NONEED:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_NONEED") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_DEL_ITEM_TIMELMT:
		{
			GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT *pNetMsg = (GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT *)nmg;
			
			SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pNetMsg->nidITEM );
			if ( !pITEM )	break;

			CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("INVEN_DEL_ITEM_TIMELMT"), pITEM->GetName() );
		}
		break;

	case NET_MSG_GCTRL_PUSHPULL_BRD:
		{
			GLMSG::SNET_PUSHPULL_BRD *pNetMsg = (GLMSG::SNET_PUSHPULL_BRD *)nmg;
			const D3DXVECTOR3 &vMovePos = pNetMsg->vMovePos;

			//	Note : 밀려날 위치로 이동 시도.
			//
			BOOL bSucceed = m_actorMove.GotoLocation
			(
				D3DXVECTOR3(vMovePos.x,vMovePos.y+5,vMovePos.z),
				D3DXVECTOR3(vMovePos.x,vMovePos.y-5,vMovePos.z)
			);

			if ( bSucceed )
			{
				//	Note : 밀려나는 엑션 시작.
				//
				m_sTargetID.vPos = vMovePos;
				TurnAction ( GLAT_PUSHPULL );

				//	Note : 밀리는 속도 설정.
				//
				m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );
			}
		}
		break;

	case NET_MSG_GCTRL_REGEN_GATE_FB:
		{
			GLMSG::SNETPC_REQ_REGEN_GATE_FB *pNetMsg = (GLMSG::SNETPC_REQ_REGEN_GATE_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMREGEN_GATE_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREGEN_GATE_FAIL") );
				}
				break;

			case EMREGEN_GATE_SUCCEED:
				{
					// Note : 시작귀환카드에서 사용될 정보를 받습니다.
					m_sStartMapID = pNetMsg->sMapID;
					m_dwStartGate = pNetMsg->dwGateID;
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREGEN_GATE_SUCCEED") );
				}
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_CHARRESET_FB:
		{
			GLMSG::SNETPC_REQ_CHARRESET_FB *pNetMsg = (GLMSG::SNETPC_REQ_CHARRESET_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMREGEN_CHARRESET_SUCCEED:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREGEN_CHARRESET_SUCCEED") );

					RESET_STATS_SKILL();

					m_wStatsPoint = (WORD) pNetMsg->dwSTATS_P;
					m_dwSkillPoint = pNetMsg->dwSKILL_P;

					m_sRunSkill = SNATIVEID(false);
					m_sActiveSkill = SNATIVEID(false);
					SetDefenseSkill( false );
					m_sREACTION.RESET();

					//	Note : 만약 SKILL 사용중이라면 모두 리셋.
					//
					if ( IsACTION(GLAT_SKILL) )
					{
						TurnAction(GLAT_IDLE);
					}
				}
				break;

			case EMREGEN_CHARRESET_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREGEN_CHARRESET_FAIL") );
				break;

			case EMREGEN_CHARRESET_ITEM_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREGEN_CHARRESET_ITEM_FAIL") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_FIRECRACKER_FB:
		{
			GLMSG::SNETPC_REQ_FIRECRACKER_FB *pNetMsg = (GLMSG::SNETPC_REQ_FIRECRACKER_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMREQ_FIRECRACKER_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_FIRECRACKER_FB_FAIL") );
				break;

			case EMREQ_FIRECRACKER_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_FIRECRACKER_FB_OK") );
				break;

			case EMREQ_FIRECRACKER_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_FIRECRACKER_FB_NOITEM") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_FIRECRACKER_BRD:
		{
			GLMSG::SNETPC_REQ_FIRECRACKER_BRD *pNetMsg = (GLMSG::SNETPC_REQ_FIRECRACKER_BRD *) nmg;
			
			SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pNetMsg->nidITEM );
			if ( !pITEM )	break;

			D3DXMATRIX matEffect;
			D3DXMatrixTranslation ( &matEffect, pNetMsg->vPOS.x, pNetMsg->vPOS.y, pNetMsg->vPOS.z );

			//	보이지 않는 타갯일 경우 타격 이팩트는 생략됨.
			DxEffGroupPlayer::GetInstance().NewEffGroup
			(
				pITEM->GetTargetEffect(),
				matEffect,
				&STARGETID(CROW_PC,0,pNetMsg->vPOS)
			);
		}
		break;

	case NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGETNUM_UPDATE:
		{
			GLMSG::SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE *pNetMsg = (GLMSG::SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE *) nmg;

			m_dwVietnamInvenCount = pNetMsg->dwVietnamInvenCount;
		}
		break;

	case NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGET_FB:
		{
			GLMSG::SNETPC_INVEN_VIETNAM_ITEMGET_FB *pNetMsg = (GLMSG::SNETPC_INVEN_VIETNAM_ITEMGET_FB *) nmg;

			m_dwVietnamInvenCount = pNetMsg->dwVietnamInvenCount;
			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("VIETNAM_USE_ITEMGET_ITEM") );
		}
		break;

	case NET_MSG_GCTRL_INVEN_VIETNAM_EXPGET_FB:
		{
			m_lVNGainSysMoney = 0;
//			m_lnMoney		  = 0;
			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("VIETNAM_USE_EXPGET_ITEM") );
		}
		break;

#if defined(VN_PARAM) //vietnamtest%%%
	case NET_MSG_VIETNAM_ALLINITTIME:
		{
			GLMSG::SNETPC_VIETNAM_ALLINITTIME *pNetMsg = (GLMSG::SNETPC_VIETNAM_ALLINITTIME *)nmg;
			m_dwVietnamGainType = GAINTYPE_MAX;
			m_sVietnamSystem.Init();
			m_sVietnamSystem.loginTime = pNetMsg->initTime;
			m_tLoginTime = m_sVietnamSystem.loginTime;
			GLGaeaClient::GetInstance().SetCurrentTime( m_tLoginTime );
			m_sVietnamSystem.gameTime  = 0;
//			CInnerInterface::GetInstance().SET_VNGAINTYPE_GAUGE( 0, 300 );
			CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VIETNAM_TIME_ALLINIT") );
		}
		break;
#endif

	case NET_MSG_GCTRL_INVEN_RESET_SKST_FB:
		{
			GLMSG::SNET_INVEN_RESET_SKST_FB *pNetMsg = (GLMSG::SNET_INVEN_RESET_SKST_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMREQ_RESET_SKST_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RESET_SKST_FB_FAIL") );
				break;
			case EMREQ_RESET_SKST_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_RESET_SKST_FB_OK"),
					pNetMsg->wITEM_NUM );

				RESET_STATS_SKILL(pNetMsg->wSTATS_POINT);
				break;
			case EMREQ_RESET_SKST_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RESET_SKST_FB_NOITEM") );
				break;
			case EMREQ_RESET_SKST_FB_NOINVEN:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RESET_SKST_FB_NOINVEN"), pNetMsg->wITEM_NUM );
				break;
			};
		}
		break;

	case NET_MSG_GM_MOVE2GATE_FB:
		{
			GLMSG::SNET_GM_MOVE2GATE_FB *pNetMsg = (GLMSG::SNET_GM_MOVE2GATE_FB *) nmg;
			SetPosition ( pNetMsg->vPOS );
			//DoActWait ();
		}
		break;

	case NET_MSG_GCTRL_CURE_FB:
		{
			GLMSG::SNETPC_REQ_CURE_FB *pNetMsg = (GLMSG::SNETPC_REQ_CURE_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMREGEN_CURE_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREGEN_CURE_FAIL") );
				break;

			case EMREGEN_CURE_SUCCEED:
				m_sHP = pNetMsg->sHP;

				for ( int i=0; i<EMBLOW_MULTI; ++i )
				{
					if ( m_sSTATEBLOWS[i].emBLOW==EMBLOW_NONE )		continue;

					EMDISORDER emDIS = STATE_TO_DISORDER(m_sSTATEBLOWS[i].emBLOW);
					if ( !(pNetMsg->dwCUREFLAG&emDIS) )				continue;

					m_sSTATEBLOWS[i].fAGE = 0.0f;
				}
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_CHARCARD_FB:
		{
			GLMSG::SNET_INVEN_CHARCARD_FB *pNetMsg = (GLMSG::SNET_INVEN_CHARCARD_FB *) nmg;
			switch ( pNetMsg->emFB )
			{
			case EMREQ_CHARCARD_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CHARCARD_FB_FAIL") );
				break;

			case EMREQ_CHARCARD_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_CHARCARD_FB_OK") );
				break;

			case EMREQ_CHARCARD_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CHARCARD_FB_NOITEM") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_STORAGECARD_FB:
		{
			GLMSG::SNET_INVEN_STORAGECARD_FB *pNetMsg = (GLMSG::SNET_INVEN_STORAGECARD_FB *) nmg;
			switch ( pNetMsg->emFB )
			{
			case EMREQ_STORAGECARD_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_FAIL") );
				break;

			case EMREQ_STORAGECARD_FB_OK:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_OK") );

					CTime tLMT(pNetMsg->tSTORAGE_LIMIT);
					CTimeSpan tSPAN(pNetMsg->tSPAN);
					CInnerInterface::GetInstance().PrintConsoleTextDlg ( ID2GAMEINTEXT("STORAGE_EX_STATE"),
						pNetMsg->wSTORAGE+1, tLMT.GetYear(), tLMT.GetMonth(), tLMT.GetDay(), tLMT.GetHour(),
						tSPAN.GetDays() );

					int nINDEX = pNetMsg->wSTORAGE-EMSTORAGE_CHANNEL_SPAN;
					m_bSTORAGE[nINDEX] = true;
					m_tSTORAGE[nINDEX] = pNetMsg->tSTORAGE_LIMIT;
				}
				break;

			case EMREQ_STORAGECARD_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_NOITEM") );
				break;

			case EMREQ_STORAGECARD_FB_INVNUM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_INVNUM") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_STORAGE_STATE:
		{
			GLMSG::SNETPC_STORAGE_STATE *pNetMsg = (GLMSG::SNETPC_STORAGE_STATE *) nmg;
			for ( int i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i  )
			{
				if ( m_bSTORAGE[i]==true && pNetMsg->bVALID[i]==false )
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_STORAGE_END"), i+1 );
				}

				m_bSTORAGE[i] = pNetMsg->bVALID[i];
			}
		}
		break;

	case NET_MSG_GCTRL_INVEN_INVENLINE_FB:
		{
			GLMSG::SNET_INVEN_INVENLINE_FB *pNetMsg = (GLMSG::SNET_INVEN_INVENLINE_FB *) nmg;
			switch ( pNetMsg->emFB )
			{
			case EMREQ_INVENLINE_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_FAIL") );
				break;

			case EMREQ_INVENLINE_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_OK") );
				
				//	Note : 인벤 줄수 설정.
				m_wINVENLINE = pNetMsg->wINVENLINE;

				//	Note : 현제 활성화된 인벤 라인 설정.
				//
				m_cInventory.SetAddLine ( GetOnINVENLINE(), true );
				CInnerInterface::GetInstance().SetInventorySlotViewSize ( EM_INVEN_DEF_SIZE_Y + GetOnINVENLINE() );
				break;

			case EMREQ_INVENLINE_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_NOITEM") );
				break;

			case EMREQ_INVENLINE_FB_MAXLINE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_MAXLINE") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_REMODELOPEN_FB:
		{
			GLMSG::SNET_INVEN_REMODELOPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_REMODELOPEN_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMREQ_REMODELOPEN_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_FAIL") );
				break;

			case EMREQ_REMODELOPEN_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_OK") );
				CInnerInterface::GetInstance().OpenItemRebuildWindow();
				break;

			case EMREQ_REMODELOPEN_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_NOITEM") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_GARBAGEOPEN_FB:
		{
			GLMSG::SNET_INVEN_GARBAGEOPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_GARBAGEOPEN_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMREQ_GARBAGEOPEN_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GARBAGEOPEN_FB_FAIL") );
				break;

			case EMREQ_GARBAGEOPEN_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_GARBAGEOPEN_FB_OK") );
				CInnerInterface::GetInstance().OpenItemGarbageWindow();
				break;

			case EMREQ_GARBAGEOPEN_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GARBAGEOPEN_FB_NOITEM") );
				break;
			};
		}
		break;

		// 락커 관리인과의 대화를 제외한 그밖의 방법으로 창고 열람 ( 긴급 창고 연결 카드... etc )
	case NET_MSG_GCTRL_INVEN_STORAGEOPEN_FB:
		{
			GLMSG::SNET_INVEN_STORAGEOPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_STORAGEOPEN_FB *) nmg;
			//SINVENITEM sItem( pNetMsg->wPosX, pNetMsg->wPosY );
			switch ( pNetMsg->emFB )
			{
			case EMREQ_STORAGEOPEN_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGEOPEN_FB_FAIL") );
				break;

			case EMREQ_STORAGEOPEN_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_STORAGEOPEN_FB_OK") );
				
				//	Note : 창고 열기.
				CInnerInterface::GetInstance().SetDefaultPosInterface( INVENTORY_WINDOW );
				CInnerInterface::GetInstance().SetDefaultPosInterface( STORAGE_WINDOW );
				CInnerInterface::GetInstance().ShowGroupFocus ( INVENTORY_WINDOW );
				CInnerInterface::GetInstance().SetStorageWindowOpen ( 0 );
				break;

			case EMREQ_STORAGEOPEN_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGEOPEN_FB_NOITEM") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_PREMIUMSET_FB:
		{
			GLMSG::SNET_INVEN_PREMIUMSET_FB *pNetMsg = (GLMSG::SNET_INVEN_PREMIUMSET_FB *) nmg;
			switch ( pNetMsg->emFB )
			{
			case EMREQ_PREMIUMSET_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_PREMIUMSET_FB_FAIL") );
				break;

			case EMREQ_PREMIUMSET_FB_OK:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_PREMIUMSET_FB_OK") );

					CTime tLMT(pNetMsg->tLMT);
					CTimeSpan tSPAN(pNetMsg->tSPAN);
					CInnerInterface::GetInstance().PrintConsoleTextDlg ( ID2GAMEINTEXT("PREMIUMSET_EX_STATE"),
						tLMT.GetYear(), tLMT.GetMonth(), tLMT.GetDay(), tLMT.GetHour(),
						tSPAN.GetDays() );

					m_tPREMIUM = pNetMsg->tLMT;
					m_bPREMIUM = true;

					//	Note : 현제 활성화된 인벤 라인 설정.
					//
					m_cInventory.SetAddLine ( GetOnINVENLINE(), true );
					CInnerInterface::GetInstance().SetInventorySlotViewSize ( EM_INVEN_DEF_SIZE_Y + GetOnINVENLINE() );
				}
				break;

			case EMREQ_PREMIUMSET_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_PREMIUMSET_FB_NOITEM") );
				break;

			case EMREQ_PREMIUMSET_FB_NOTINVEN:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_PREMIUMSET_FB_NOTINVEN") );
				break;
			};
		}
		break;

	case NET_MSG_CHAT_LOUDSPEAKER_FB:
		{
			GLMSG::SNETPC_CHAT_LOUDSPEAKER_FB *pNetMsg = (GLMSG::SNETPC_CHAT_LOUDSPEAKER_FB *) nmg;
			switch ( pNetMsg->emFB )
			{
			case EMCHAT_LOUDSPEAKER_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_LOUDSPEAKER_FAIL") );
				break;

			case EMCHAT_LOUDSPEAKER_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCHAT_LOUDSPEAKER_OK") );
				break;

			case EMCHAT_LOUDSPEAKER_NOITEM:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_LOUDSPEAKER_NOITEM") );
				break;

			case EMCHAT_LOUDSPEAKER_BLOCK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_LOUDSPEAKER_BLOCK") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_PREMIUM_STATE:
		{
			GLMSG::SNETPC_PREMIUM_STATE *pNetMsg = (GLMSG::SNETPC_PREMIUM_STATE *) nmg;
			
			if ( pNetMsg->bPREMIUM==false && m_bPREMIUM==true )
			{
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PREMIUMSERVICE_END") );
			}

			m_bPREMIUM = pNetMsg->bPREMIUM;
		}
		break;

	case NET_MSG_GCTRL_POSITIONCHK_BRD:
		{
			GLMSG::SNET_POSITIONCHK_BRD *pNetNsg = (GLMSG::SNET_POSITIONCHK_BRD *)nmg;
			m_vServerPos = pNetNsg->vPOS;
		}
		break;

	case NET_MSG_GCTRL_NPC_ITEM_TRADE_FB:
		{
			GLMSG::SNETPC_REQ_NPC_ITEM_TRADE_FB *pNetNsg = (GLMSG::SNETPC_REQ_NPC_ITEM_TRADE_FB *)nmg;
			switch ( pNetNsg->emFB )
			{
			case EMNPC_ITEM_TRADE_SUCCEED:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_SUCCEED") );
				break;
			case EMNPC_ITEM_TRADE_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_FAIL") );
				break;
			case EMNPC_ITEM_TRADE_ITEM_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_ITEM_FAIL") );
				break;
			case EMNPC_ITEM_TRADE_INSERT_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_INSERT_FAIL") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_GET_CHARGEDITEM_FROMDB_FB:
		{
			GLMSG::SNET_GET_CHARGEDITEM_FROMDB_FB* pNetMsg = (GLMSG::SNET_GET_CHARGEDITEM_FROMDB_FB*)nmg;
			switch ( pNetMsg->emFB )
			{
			case EMREQ_CHARGEDITEM_FROMDB_FB_END:
				CInnerInterface::GetInstance().SetItemBankInfo ();
				break;

			case EMREQ_CHARGEDITEM_FROMDB_FB_OK:
				ADDSHOPPURCHASE ( pNetMsg->szPurKey, pNetMsg->nidITEM );
				break;
			}
		}
		break;

	case NET_MSG_GCTRL_CHARGED_ITEM_GET_FB:
		{
			GLMSG::SNET_CHARGED_ITEM_GET_FB *pNetNsg = (GLMSG::SNET_CHARGED_ITEM_GET_FB *)nmg;
			switch ( pNetNsg->emFB )
			{
			case EMCHARGED_ITEM_GET_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_FAIL") );
				break;
			case EMCHARGED_ITEM_GET_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_OK") );
				break;
			case EMCHARGED_ITEM_GET_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_NOITEM") );
				break;
			case EMCHARGED_ITEM_GET_FB_NOINVEN:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_NOINVEN") );
				break;
			};
			
		}
		break;

	case NET_MSG_GCTRL_CHARGED_ITEM_DEL:
		{
			GLMSG::SNET_CHARGED_ITEM_DEL *pNetNsg = (GLMSG::SNET_CHARGED_ITEM_DEL *)nmg;
			DELSHOPPURCHASE ( pNetNsg->dwID );
			CInnerInterface::GetInstance().REFRESH_ITEMBANK ();
		}
		break;

	case NET_MSG_GCTRL_REVIVE_FB:
		{
			GLMSG::SNETPC_REQ_REVIVE_FB *pNetNsg = (GLMSG::SNETPC_REQ_REVIVE_FB *)nmg;

			switch ( pNetNsg->emFB )
			{
			case EMREQ_REVIVE_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_REVIVE_FB_FAIL") );
				break;
			case EMREQ_REVIVE_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_REVIVE_FB_OK") );
				break;
			case EMREQ_REVIVE_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_REVIVE_FB_NOITEM") );
				break;
			case EMREQ_REVIVE_FB_NOTUSE:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_REVIVE_FB_NOTUSE") );
				break;
			case EMREQ_REVIVE_FB_COOLTIME:
				break;
			};

			if ( pNetNsg->emFB != EMREQ_REVIVE_FB_OK )
			{
				ReqReBirth();
				CInnerInterface::GetInstance().CloseAllWindow ();
			}
		}
		break;

	case NET_MSG_GCTRL_GETEXP_RECOVERY_FB:
		{
// 경험치회복_정의_Normal
#if defined(RZ_PARAM) || defined( _RELEASED ) || defined ( KRT_PARAM ) || defined ( KR_PARAM ) 
			GLMSG::SNETPC_REQ_GETEXP_RECOVERY_FB *pNetMsg = (GLMSG::SNETPC_REQ_GETEXP_RECOVERY_FB *)nmg;

			// 회복할 경험치가 없습니다.
			if ( pNetMsg->nReExp <= 0 )	
			{
				CInnerInterface::GetInstance().PrintMsgText( 
						NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREQ_RECOVERY_FB_NOREEXP" ) );

				// 부활하기
				ReqReBirth();					
				// 열려진 창들 닫기
				CInnerInterface::GetInstance().CloseAllWindow ();

				return;
			}

			// 소지 금액이 모자르다면 부활시킨다.
			if ( pNetMsg->nDecMoney > m_lnMoney )	
			{
				CInnerInterface::GetInstance().PrintMsgText( 
						NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREQ_RECOVERY_FB_NOMONEY" ) );

				// 부활하기
				ReqReBirth();					
				// 열려진 창들 닫기
				CInnerInterface::GetInstance().CloseAllWindow ();

				return;				
			}

			CString strTemp;
			strTemp.Format ( ID2GAMEINTEXT("MODAL_RECOVERY_EXP"), 
							 pNetMsg->nDecExp, pNetMsg->nReExp, pNetMsg->nDecMoney );

			DoModal( strTemp, MODAL_QUESTION, OKCANCEL, MODAL_RECOVERY_EXP );
#endif

		}
		break;

	case NET_MSG_GCTRL_GETEXP_RECOVERY_NPC_FB:
		{
// 경험치회복_정의_Npc
#if defined(RZ_PARAM) || defined( _RELEASED ) || defined ( KRT_PARAM ) || defined ( KR_PARAM ) || defined ( TH_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( PH_PARAM ) || defined ( CH_PARAM ) || defined ( TW_PARAM ) || defined ( HK_PARAM ) || defined ( GS_PARAM )
			GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC_FB *pNetMsg = (GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC_FB *)nmg;

			// 회복할 경험치가 없습니다.
			if ( pNetMsg->nReExp <= 0 )	
			{
				CInnerInterface::GetInstance().PrintMsgText( 
						NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREQ_RECOVERY_NPC_FB_NOREEXP" ) );
				
				return;
			}

			// 소지 금액이 모자르다면 부활시킨다.
			if ( pNetMsg->nDecMoney > m_lnMoney )	
			{
				CInnerInterface::GetInstance().PrintMsgText( 
						NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREQ_RECOVERY_NPC_FB_NOMONEY" ) );				

				return;				
			}

			CString strTemp;
			strTemp.Format ( ID2GAMEINTEXT("MODAL_RECOVERY_NPC_EXP"), pNetMsg->nReExp, pNetMsg->nDecMoney );

			DoModal( strTemp, MODAL_QUESTION, OKCANCEL, MODAL_RECOVERY_NPC_EXP );

			CInnerInterface::GetInstance().GetModalWindow()->SetModalData( pNetMsg->dwNPCID, 0 );
#endif

		}
		break;
	
	case NET_MSG_GCTRL_RECOVERY_FB:
		{
// 경험치회복_정의_Normal
#if defined(RZ_PARAM) || defined( _RELEASED ) || defined ( KRT_PARAM ) || defined ( KR_PARAM )

			GLMSG::SNETPC_REQ_RECOVERY_FB *pNetNsg = (GLMSG::SNETPC_REQ_RECOVERY_FB *)nmg;

			switch ( pNetNsg->emFB )
			{
			case EMREQ_RECOVERY_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_FB_FAIL") );
				break;
			case EMREQ_RECOVERY_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_RECOVERY_FB_OK"),  pNetNsg->nReExp );
				break;
			case EMREQ_RECOVERY_FB_NOMONEY:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_FB_NOMONEY") );
				break;
			case EMREQ_RECOVERY_FB_NOREEXP:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_FB_NOREEXP") );
				break;
			case EMREQ_RECOVERY_FB_NOTUSE:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_FB_NOTUSE") );
				break;
			};

			if ( pNetNsg->emFB != EMREQ_RECOVERY_FB_OK )
			{
				ReqReBirth();
				CInnerInterface::GetInstance().CloseAllWindow ();
			}
#endif
		}
		break;

	case NET_MSG_GCTRL_RECOVERY_NPC_FB:
		{
// 경험치회복_정의_Npc
#if defined(RZ_PARAM) || defined( _RELEASED ) || defined ( KRT_PARAM ) || defined ( KR_PARAM ) || defined ( TH_PARAM ) || defined ( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( PH_PARAM ) || defined ( CH_PARAM ) || defined ( TW_PARAM ) || defined ( HK_PARAM ) || defined ( GS_PARAM )
			GLMSG::SNETPC_REQ_RECOVERY_NPC_FB *pNetNsg = (GLMSG::SNETPC_REQ_RECOVERY_NPC_FB *)nmg;

			switch ( pNetNsg->emFB )
			{
			case EMREQ_RECOVERY_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_NPC_FB_FAIL") );
				break;
			case EMREQ_RECOVERY_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_RECOVERY_NPC_FB_OK"), pNetNsg->nReExp );
				break;
			case EMREQ_RECOVERY_FB_NOMONEY:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_NPC_FB_NOMONEY") );
				break;
			case EMREQ_RECOVERY_FB_NOREEXP:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_NPC_FB_NOREEXP") );
				break;
			case EMREQ_RECOVERY_FB_NOTUSE:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_NPC_FB_NOTUSE") );
				break;
			};

			if ( pNetNsg->emFB != EMREQ_RECOVERY_FB_OK )
			{
				ReqReBirth();
				CInnerInterface::GetInstance().CloseAllWindow ();
			}
#endif
		}
		break;

	case NET_MSG_GCTRL_PMARKET_TITLE_FB:
		{
			GLMSG::SNETPC_PMARKET_TITLE_FB *pNetMsg = (GLMSG::SNETPC_PMARKET_TITLE_FB *) nmg;
			m_sPMarket.SetTITLE ( pNetMsg->szPMarketTitle );

			//	Note : 인터페이스에 갱신?
		}
		break;

	case NET_MSG_GCTRL_PMARKET_REGITEM_FB:
		{
			GLMSG::SNETPC_PMARKET_REGITEM_FB *pNetMsg = (GLMSG::SNETPC_PMARKET_REGITEM_FB *) nmg;
			
			switch ( pNetMsg->emFB )
			{
			case EMPMARKET_REGITEM_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_FAIL") );
				break;

			case EMPMARKET_REGITEM_FB_OK:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_OK") );

					SINVENITEM* pINVENITEM = m_cInventory.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
					if ( pINVENITEM )
					{
						m_sPMarket.RegItem ( *pINVENITEM, pNetMsg->dwMoney, pNetMsg->dwNum, pNetMsg->sSALEPOS );

						//	Note : 인터페이스에 갱신?
					}
				}
				break;
			
			case EMPMARKET_REGITEM_FB_MAXNUM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_MAXNUM") );
				break;

			case EMPMARKET_REGITEM_FB_REGITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_REGITEM") );
				break;

			case EMPMARKET_REGITEM_FB_NOSALE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_NOSALE") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_PMARKET_DISITEM_FB:
		{
			GLMSG::SNETPC_PMARKET_DISITEM_FB *pNetMsg = (GLMSG::SNETPC_PMARKET_DISITEM_FB *) nmg;

			m_sPMarket.DisItem ( pNetMsg->sSALEPOS );

			//	Note : 인터페이스에 갱신?
		}
		break;

	case NET_MSG_GCTRL_PMARKET_OPEN_FB:
		{
			GLMSG::SNETPC_PMARKET_OPEN_FB *pNetMsg = (GLMSG::SNETPC_PMARKET_OPEN_FB *) nmg;
			
			switch ( pNetMsg->emFB )
			{
			case EMPMARKET_OPEN_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_FAIL") );
				break;
			
			case EMPMARKET_OPEN_FB_OK:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_OK") );
					m_sPMarket.DoMarketOpen();
				}
				break;

			case EMPMARKET_OPEN_FB_ALREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_ALREADY") );
				break;
			
			case EMPMARKET_OPEN_FB_EMPTY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_EMPTY") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_PMARKET_BUY_FB:
		{
			GLMSG::SNETPC_PMARKET_BUY_FB *pNetMsg = (GLMSG::SNETPC_PMARKET_BUY_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMPMARKET_BUY_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_FAIL") );
				break;
			case EMPMARKET_BUY_FB_OK:
				if ( pNetMsg->dwGaeaID == m_dwGaeaID )
				{
					SSALEITEM *pSALEITEM = m_sPMarket.GetItem ( pNetMsg->sSALEPOS );
					if ( pSALEITEM )
					{
						SITEM *pITEM = GLItemMan::GetInstance().GetItem ( pSALEITEM->sITEMCUSTOM.sNativeID );
						if ( pITEM )
						{
							CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPMARKET_BUY_FB_OK_SELLER"), pITEM->GetName(), pNetMsg->dwNum );
						}
					}
				}
				else
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPMARKET_BUY_FB_OK") );
				}
				break;
			case EMPMARKET_BUY_FB_NUM:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_NUM") );

					PGLCHARCLIENT pCHAR = GLGaeaClient::GetInstance().GetChar ( pNetMsg->dwGaeaID );
					if ( pCHAR )
					{
						bool bSOLD = pNetMsg->dwNum == 0;
						pCHAR->m_sPMarket.SetSaleState ( pNetMsg->sSALEPOS, pNetMsg->dwNum, bSOLD );
					}
				}
				break;
			case EMPMARKET_BUY_FB_LOWMONEY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_LOWMONEY") );
				break;
			case EMPMARKET_BUY_FB_SOLD:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_SOLD") );
				break;
			case EMPMARKET_BUY_FB_NOINVEN:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_NOINVEN") );
				break;
			case EMPMARKET_BUY_FB_NOTIME:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_NOTIME") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD:
		{
			GLMSG::SNETPC_PMARKET_ITEM_UPDATE_BRD *pNetMsg = (GLMSG::SNETPC_PMARKET_ITEM_UPDATE_BRD *) nmg;

			SSALEITEM *pSALEITEM = m_sPMarket.GetItem ( pNetMsg->sSALEPOS );
			if ( pSALEITEM )
			{
				pSALEITEM->bSOLD = pNetMsg->bSOLD;
				pSALEITEM->dwNUMBER = pNetMsg->dwNUMBER;
				pSALEITEM->sITEMCUSTOM.wTurnNum = (WORD) pNetMsg->dwNUMBER;

				SINVENITEM *pINVENITEM = m_sPMarket.GetInven().GetItem(pNetMsg->sSALEPOS.wMainID,pNetMsg->sSALEPOS.wSubID);
				if ( pINVENITEM )
				{
					pINVENITEM->sItemCustom.wTurnNum = (WORD) pNetMsg->dwNUMBER;
				}
			}
		}
		break;

	case NET_MSG_GCTRL_PMARKET_CLOSE_BRD:
		{
			GLMSG::SNETPC_PMARKET_CLOSE_BRD *pNetNsg = (GLMSG::SNETPC_PMARKET_CLOSE_BRD *)nmg;

			m_sPMarket.DoMarketClose();
		}
		break;

	case NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT:
		{
			GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT *pNetResultMsg = (GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT *)nmg;	
			CInnerInterface::GetInstance().GetItemSearchResultWindow()->UpdateSearchResult( pNetResultMsg->sSearchResult, pNetResultMsg->dwSearchNum, pNetResultMsg->dwPageNum );
			CInnerInterface::GetInstance().HideGroup ( ITEM_SHOP_SEARCH_WINDOW );
			CInnerInterface::GetInstance().ShowGroupFocus ( ITEM_SEARCH_RESULT_WINDOW );

		}
		break;

	case NET_MSG_GCTRL_PLAYERKILLING_ADD:
		{
			GLMSG::SNETPC_PLAYERKILLING_ADD *pNetMsg = (GLMSG::SNETPC_PLAYERKILLING_ADD *) nmg;

			PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();	
			bool bBRIGHTEVENT    = GLGaeaClient::GetInstance().IsBRIGHTEVENT();
			bool bSCHOOLFREEPK   = GLGaeaClient::GetInstance().IsSchoolFreePk();
			bool bADD(false);

			if ( !( bBRIGHTEVENT||bSCHOOLFREEPK||pLand->m_bClubBattle||pLand->m_bClubDeathMatch ) )
			{
				bADD = ADD_PLAYHOSTILE ( pNetMsg->dwCharID, pNetMsg->bBAD );
			}

			// 학원간 자유피케 진행중이며 상대가 같은 학교일경우 적대행위 대상자로 구분하여 관리함.
			if ( bSCHOOLFREEPK && m_wSchool == pNetMsg->wSchoolID )
			{
				bADD = ADD_PLAYHOSTILE ( pNetMsg->dwCharID, pNetMsg->bBAD );
			}

			if ( bADD && !pNetMsg->bBAD )
			{
				if ( pNetMsg->bClubBattle )
				{
					PGLCHARCLIENT pCLIENT = (PGLCHARCLIENT) GLGaeaClient::GetInstance().GetCopyActor( pNetMsg->szName );
					if ( pCLIENT ) 
					{
						if ( m_sCLUB.IsBattle ( pCLIENT->GETCLUBID() ) )
						{
							CInnerInterface::GetInstance().PrintConsoleTextDlg ( ID2GAMEINTEXT("CLUB_PLAYHOSTILE_ADD"), 
								pCLIENT->GetClubName(), pNetMsg->szName );
						}
						else if ( m_sCLUB.IsBattleAlliance ( pCLIENT->GETALLIANCEID() ) )
						{
							CInnerInterface::GetInstance().PrintConsoleTextDlg ( ID2GAMEINTEXT("ALLIANCE_PLAYHOSTILE_ADD"), 
								pCLIENT->GetClubName(), pNetMsg->szName );
						}
					}
				}
				else CInnerInterface::GetInstance().PrintConsoleTextDlg ( ID2GAMEINTEXT("PK_PLAYHOSTILE_ADD"), pNetMsg->szName );
			}
		}
		break;

	case NET_MSG_GCTRL_PLAYERKILLING_DEL:
		{
			GLMSG::SNETPC_PLAYERKILLING_DEL *pNetMsg = (GLMSG::SNETPC_PLAYERKILLING_DEL *) nmg;
			DEL_PLAYHOSTILE ( pNetMsg->dwCharID );
		}
		break;

	case NET_MSG_GCTRL_INVEN_HAIR_CHANGE_FB:
		{
			GLMSG::SNETPC_INVEN_HAIR_CHANGE_FB *pNetMsg = (GLMSG::SNETPC_INVEN_HAIR_CHANGE_FB *) nmg;
		
			switch ( pNetMsg->emFB )
			{
			case EMINVEN_HAIR_CHANGE_FB_FAIL:
				HairStyleChange( m_wHair );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_FAIL") );
				break;
			case EMINVEN_HAIR_CHANGE_FB_OK:
				{
					m_wHair = (WORD) pNetMsg->dwID;
					UpdateSuit( TRUE );

					// 헤어스타일 변경
					HairStyleChange( m_wHair );

					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_OK") );
				}
				break;
			case EMINVEN_HAIR_CHANGE_FB_NOITEM:
				HairStyleChange( m_wHair );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_NOITEM") );
				break;
			case EMINVEN_HAIR_CHANGE_FB_BADITEM:
				HairStyleChange( m_wHair );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_BADITEM") );
				break;
			case EMINVEN_HAIR_CHANGE_FB_BADCLASS:
				HairStyleChange( m_wHair );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_BADCLASS") );
				break;
			case EMINVEN_HAIR_CHANGE_FB_COOLTIME:
				HairStyleChange( m_wHair );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_FB:
		{
			GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_FB *pNetMsg = (GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_FB*)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMINVEN_HAIR_CHANGE_FB_FAIL:
				HairColorChange( m_wHairColor );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_FAIL") );
				break;
			case EMINVEN_HAIR_CHANGE_FB_OK:
				{
					m_wHairColor = pNetMsg->wHairColor;
					UpdateSuit( TRUE );

					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_OK") );
				}
				break;
			case EMINVEN_HAIR_CHANGE_FB_NOITEM:
				HairColorChange( m_wHairColor );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_NOITEM") );
				break;
			case EMINVEN_HAIR_CHANGE_FB_BADITEM:
				HairColorChange( m_wHairColor );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_BADITEM") );
				break;
			case EMINVEN_HAIR_CHANGE_FB_BADCLASS:
				HairColorChange( m_wHairColor );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_BADCLASS") );
				break;
			case EMINVEN_HAIR_CHANGE_FB_COOLTIME:
				HairColorChange( m_wHairColor );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_FACE_CHANGE_FB:
		{
			GLMSG::SNETPC_INVEN_FACE_CHANGE_FB *pNetMsg = (GLMSG::SNETPC_INVEN_FACE_CHANGE_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMINVEN_FACE_CHANGE_FB_FAIL:
				FaceStyleChange( m_wFace );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_FACE_CHANGE_FB_FAIL") );
				break;
			case EMINVEN_FACE_CHANGE_FB_OK:
				{
					m_wFace = (WORD) pNetMsg->dwID;
					UpdateSuit( TRUE );

					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMINVEN_FACE_CHANGE_FB_OK") );
				}
				break;
			case EMINVEN_FACE_CHANGE_FB_NOITEM:
				FaceStyleChange( m_wFace );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_FACE_CHANGE_FB_NOITEM") );
				break;
			case EMINVEN_FACE_CHANGE_FB_BADITEM:
				FaceStyleChange( m_wFace );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_FACE_CHANGE_FB_BADITEM") );
				break;
			case EMINVEN_FACE_CHANGE_FB_BADCLASS:
				FaceStyleChange( m_wFace );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_FACE_CHANGE_FB_BADCLASS") );
				break;
			case EMINVEN_FACE_CHANGE_FB_COOLTIME:
				FaceStyleChange( m_wFace );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_INVEN_GENDER_CHANGE_FB:
		{
			GLMSG::SNETPC_INVEN_GENDER_CHANGE_FB *pNetMsg = (GLMSG::SNETPC_INVEN_GENDER_CHANGE_FB *) nmg;
			
			switch ( pNetMsg->emFB )
			{
			case EMINVEN_GENDER_CHANGE_FB_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_FAIL") );
				}
				break;
			case EMINVEN_GENDER_CHANGE_FB_OK:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_OK") );
					DoModal( ID2GAMEINTEXT("MODAL_GENDER_CHANGE_END"),  MODAL_INFOMATION, OK, MODAL_GENDER_CHANGE_END );	
				}
				break;
			case EMINVEN_GENDER_CHANGE_FB_NOITEM:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_NOITEM") );
				}
				break;
			case EMINVEN_GENDER_CHANGE_FB_ITEMTYPE:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_ITEMTYPE") );
				}
				break;
			case EMINVEN_GENDER_CHANGE_FB_BADCLASS:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_BADCLASS") );
				}
				break;
			case EMINVEN_GENDER_CHANGE_FB_NOTVALUE:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_NOTVALUE") );
				}
			}
		}
		break;

	case NET_MSG_GCTRL_INVEN_RENAME_FB:
		{
			GLMSG::SNETPC_INVEN_RENAME_FB *pNetMsg = (GLMSG::SNETPC_INVEN_RENAME_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMINVEN_RENAME_FB_OK:
				{
					StringCchCopy ( m_szName, CHAR_SZNAME, pNetMsg->szName );

					// 이름표 변경
					CNameDisplayMan *pDISP_NAME_MAN = CInnerInterface::GetInstance().GetDispName();
					if ( pDISP_NAME_MAN )
					{
						CROWREN sDISP_NAME;
						sDISP_NAME.INIT ( this );
						sDISP_NAME.SETTYPEFLAG ( this );
						pDISP_NAME_MAN->ADD_DISP_NAME ( sDISP_NAME );
					}
					
					if ( m_sCLUB.IsMember ( m_dwCharID ) )
					{
						GLCLUBMEMBER* pCMember = m_sCLUB.GetMember( m_dwCharID );
						if ( pCMember )
						{
							StringCchCopy ( pCMember->m_szName, CHAR_SZNAME, pNetMsg->szName );
						}
					}

					if ( m_sCLUB.IsMaster ( m_dwCharID ) )
					{
						StringCchCopy ( m_sCLUB.m_szMasterName, CHAR_SZNAME, pNetMsg->szName );
					}

					GLPARTY_CLIENT* pPMember = GLPartyClient::GetInstance().FindMember ( m_dwGaeaID );
					if ( pPMember )
					{
						StringCchCopy ( pPMember->m_szName, CHAR_SZNAME, pNetMsg->szName );
					}

					//	Note : 인터페이스에 변경 알림.
					CInnerInterface::GetInstance().REFRESH_FRIEND_LIST ();
					CInnerInterface::GetInstance().REFRESH_CLUB_LIST ();

					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMINVEN_RENAME_FB_OK"), pNetMsg->szName );
				}
				break;
			case EMINVEN_RENAME_FB_LENGTH:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("NEWCHAR_NAME_TOO_SHORT") );	
				break;
			case EMINVEN_RENAME_FROM_DB_BAD:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("CHARACTER_BADNAME") );
				break;
			case EMINVEN_RENAME_FROM_DB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FROM_DB_FAIL"), pNetMsg->szName );
				break;
			case EMINVEN_RENAME_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_FAIL") );
				break;
			case EMINVEN_RENAME_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_NOITEM") );
				break;
			case EMINVEN_RENAME_FB_BADITEM:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_BADITEM") );
				break;
			case EMINVEN_RENAME_FB_THAICHAR_ERROR:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_THAICHAR_ERROR") );
				break;
			case EMINVEN_RENAME_FB_VNCHAR_ERROR:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_VNCHAR_ERROR") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_QITEMFACT_BRD:
		{
			GLMSG::SNETPC_QITEMFACT_BRD *pNetMsg = (GLMSG::SNETPC_QITEMFACT_BRD *)nmg;
			
 			if ( pNetMsg->sFACT.emType == QUESTION_NONE )	break;

			// 탈것 탑승시 퀘션아이템 미 적용
			if ( m_bVehicle )	break;

			DxSoundLib::GetInstance()->PlaySound ( "QITEM_FACT" );
			CInnerInterface::GetInstance().SET_QUESTION_ITEM_ID ( pNetMsg->sFACT.emType );

			switch ( pNetMsg->sFACT.emType )
			{
			case QUESTION_SPEED_UP:
			case QUESTION_CRAZY:
			case QUESTION_ATTACK_UP:
			case QUESTION_EXP_UP:
			case QUESTION_LUCKY:
			case QUESTION_SPEED_UP_M:
			case QUESTION_MADNESS:
			case QUESTION_ATTACK_UP_M:
				m_sQITEMFACT.emType = pNetMsg->sFACT.emType;
				m_sQITEMFACT.fTime = pNetMsg->sFACT.fTime;
				m_sQITEMFACT.wParam1 = pNetMsg->sFACT.wParam1;
				m_sQITEMFACT.wParam2 = pNetMsg->sFACT.wParam2;
	
				CInnerInterface::GetInstance().SET_KEEP_QUESTION_ITEM_ID ( pNetMsg->sFACT.emType );
				break;

			case QUESTION_EXP_GET:
				{
					//	Note : 자기 위치 이펙트 발생시킴.
					STARGETID vTARID(CROW_PC,m_dwGaeaID,m_vPos);
					DxEffGroupPlayer::GetInstance().NewEffGroup ( "QI_expget.egp", m_matTrans, &vTARID );
				}
				break;

			case QUESTION_BOMB:
				{
					//	Note : 자기 위치 이펙트 발생시킴.
					STARGETID vTARID(CROW_PC,m_dwGaeaID,m_vPos);
					DxEffGroupPlayer::GetInstance().NewEffGroup ( "QI_bomb.egp", m_matTrans, &vTARID );
				}
				break;

			case QUESTION_MOBGEN:
				break;

			case QUESTION_HEAL:
				{
					//	Note : 자기 위치 이펙트 발생시킴.
					STARGETID vTARID(CROW_PC,m_dwGaeaID,m_vPos);
					DxEffGroupPlayer::GetInstance().NewEffGroup ( "QI_heal.egp", m_matTrans, &vTARID );
				}
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_QITEMFACT_END_BRD:
		{
			GLMSG::SNETPC_QITEMFACT_END_BRD *pNetMsg = (GLMSG::SNETPC_QITEMFACT_END_BRD *)nmg;

			m_sQITEMFACT.RESET ();
			CInnerInterface::GetInstance().RESET_KEEP_QUESTION_ITEM ();
		}
		break;

	case NET_MSG_GCTRL_EVENTFACT_BRD:
		{
			GLMSG::SNETPC_EVENTFACT_BRD *pNetMsg = (GLMSG::SNETPC_EVENTFACT_BRD *)nmg;

			switch ( pNetMsg->emType )
			{
			case EMGM_EVENT_SPEED:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMEVENTFACT_SPEED"), pNetMsg->wValue );
				break;

			case EMGM_EVENT_ASPEED:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMEVENTFACT_ASPEED"), pNetMsg->wValue );
				break;

			case EMGM_EVENT_ATTACK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMEVENTFACT_ATTACK"), pNetMsg->wValue );
				break;
			};

			m_sEVENTFACT.SetEVENT( pNetMsg->emType, pNetMsg->wValue );
		}
		break;

	case NET_MSG_GCTRL_EVENTFACT_END_BRD:
		{
			GLMSG::SNETPC_EVENTFACT_END_BRD *pNetMsg = (GLMSG::SNETPC_EVENTFACT_END_BRD *)nmg;

			switch( pNetMsg->emType )
			{
			case EMGM_EVENT_SPEED:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMEVENTFACT_SPEED_END") );
				break;

			case EMGM_EVENT_ASPEED:	
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMEVENTFACT_ASPEED_END") );
				break;

			case EMGM_EVENT_ATTACK:	
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMEVENTFACT_ATTACK_END") );
				break;
			}

			m_sEVENTFACT.ResetEVENT( pNetMsg->emType );
		}
		break;

	case NET_MSG_GCTRL_EVENTFACT_INFO:
		{
			GLMSG::SNETPC_EVENTFACT_INFO *pNetMsg = (GLMSG::SNETPC_EVENTFACT_INFO*)nmg;

			m_sEVENTFACT = pNetMsg->sEVENTFACT;
		}
		break;

	case NET_MSG_GCTRL_2_FRIEND_FB:
		{
			GLMSG::SNETPC_2_FRIEND_FB *pNetMsg = (GLMSG::SNETPC_2_FRIEND_FB *)nmg;
			switch ( pNetMsg->emFB )
			{
			case EM2FRIEND_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FAIL") );
				break;

			case EM2FRIEND_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EM2FRIEND_FB_OK") );
				break;

			case EM2FRIEND_FB_NO_ITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_NO_ITEM") );
				break;

			case EM2FRIEND_FB_FRIEND_CONDITION:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FRIEND_CONDITION") );
				break;

			case EM2FRIEND_FB_MY_CONDITION:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_MY_CONDITION") );
				break;

			case EM2FRIEND_FB_PK_CONDITION:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_PK_CONDITION") );
				break;

			case EM2FRIEND_FB_MAP_CONDITION:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_MAP_CONDITION") );
				break;

			case EM2FRIEND_FB_FRIEND_CHANNEL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FRIEND_CHANNEL") );
				break;

			case EM2FRIEND_FB_IMMOVABLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_IMMOVABLE") );
				break;

			case EM2FRIEND_FB_FRIEND_BLOCK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FRIEND_BLOCK") );
				break;

			case EM2FRIEND_FB_FRIEND_OFF:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EM2FRIEND_FB_FRIEND_OFF") );
				break;
			};
		}
		break;

	case NET_MSG_REBUILD_RESULT:	// ITEMREBUILD_MARK
		{
			GLMSG::SNET_REBUILD_RESULT* pNetMsg = (GLMSG::SNET_REBUILD_RESULT*)nmg;
			switch( pNetMsg->emResult )
			{
			case EMREBUILD_RESULT_MONEY:	// 확인 버튼 눌렀고, 금액이 맞지 않아서 오류 전송
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREBUILD_RESULT_MONEY" ) );
				break;

			case EMREBUILD_RESULT_DESTROY:	// 확인 버튼 눌렀고, 개조 실패, 아이템 소멸
				InitRebuildData();
				CInnerInterface::GetInstance().HideGroup( ITEM_REBUILD_WINDOW );
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREBUILD_RESULT_DESTROY" ) );
				break;

			case EMREBUILD_RESULT_SUCCESS:	// 확인 버튼 눌렀고, 개조 성공
				InitRebuildData();
				CInnerInterface::GetInstance().HideGroup( ITEM_REBUILD_WINDOW );
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREBUILD_RESULT_SUCCESS" ) );
				break;
			}
		}
		break;

	case NET_MSG_GCTRL_GARBAGE_RESULT_FB:
		{
			GLMSG::SNET_GARBAGE_RESULT_FB* pNetMsg = (GLMSG::SNET_GARBAGE_RESULT_FB*)nmg;

			switch( pNetMsg->emResult )
			{
			case EMGARBAGE_RESULT_FB_FAIL:
				InitGarbageData();
				CInnerInterface::GetInstance().PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMGARBAGE_RESULT_FB_FAIL" ) );
				break;

			case EMGARBAGE_RESULT_FB_OK:
				CInnerInterface::GetInstance().CloseItemGarbageWindow();
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMGARBAGE_RESULT_FB_OK" ) );
				break;

			case EMGARBAGE_RESULT_FB_NOITEM:
				CInnerInterface::GetInstance().CloseItemGarbageWindow();				
				CInnerInterface::GetInstance().PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMGARBAGE_RESULT_FB_NOITEM" ) );
				break;

			case EMGARBAGE_RESULT_FB_ITEMTYPE:
				InitGarbageData();
				CInnerInterface::GetInstance().PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMGARBAGE_RESULT_FB_ITEMTYPE" ) );
				break;
			}
		}

	case NET_MSG_REBUILD_MOVE_ITEM:
		{
			GLMSG::SNET_REBUILD_MOVE_ITEM* pNetMsg = (GLMSG::SNET_REBUILD_MOVE_ITEM*)nmg;
			m_sRebuildItem.SET( pNetMsg->wPosX, pNetMsg->wPosY );
		}
		break;

	case NET_MSG_REBUILD_COST_MONEY:
		{
			GLMSG::SNET_REBUILD_COST_MONEY* pNetMsg = (GLMSG::SNET_REBUILD_COST_MONEY*)nmg;
			m_i64RebuildCost = pNetMsg->i64CostMoney;

			if( m_lnMoney < m_i64RebuildCost )
			{
				InitRebuildData();
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREBUILD_RESULT_MONEY" ) );
				break;
			}

			ReqRebuildInputMoney( m_i64RebuildCost );
		}
		break;

	case NET_MSG_REBUILD_INPUT_MONEY:
		{
			GLMSG::SNET_REBUILD_INPUT_MONEY* pNetMsg = (GLMSG::SNET_REBUILD_INPUT_MONEY*)nmg;
			m_i64RebuildInput = pNetMsg->i64InputMoney;
		}
		break;

	case NET_MSG_GCTRL_UPDATE_LASTCALL:
		{
			GLMSG::SNETPC_UPDATE_LASTCALL *pNetMsg = (GLMSG::SNETPC_UPDATE_LASTCALL *)nmg;

			// Note : 직전귀환카드에서 사용될 정보를 받습니다.
			m_sLastCallMapID = pNetMsg->sLastCallMapID;
			m_vLastCallPos = pNetMsg->vLastCallPos;
		}
		break;

	case NET_MSG_GCTRL_UPDATE_STARTCALL:
		{
			GLMSG::SNETPC_UPDATE_STARTCALL *pNetMsg = (GLMSG::SNETPC_UPDATE_STARTCALL *)nmg;
			
			// 시작위치 변경
			m_sStartMapID = pNetMsg->sStartMapID;
			m_dwStartGate = pNetMsg->dwStartGateID;
			m_vSavePos	  = pNetMsg->vStartPos;
		}
		break;

	case NET_MSG_SMS_SEND_FB:
		{
			GLMSG::SNETPC_SEND_SMS_FB *pNetMsg = (GLMSG::SNETPC_SEND_SMS_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMSMS_SEND_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSMS_SEND_FB_FAIL") );
				break;
			case EMSMS_SEND_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMSMS_SEND_FB_OK") );
				break;
			case EMSMS_SEND_FROM_DB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSMS_SEND_FROM_DB_FAIL") );
				break;
			case EMSMS_SEND_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSMS_SEND_FB_NOITEM") );
				break;
			case EMSMS_SEND_FB_BADITEM:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSMS_SEND_FB_BADITEM") );
				break;
			};
		}
		break;

	case NET_MSG_SMS_PHONE_NUMBER_FB:
		{
			GLMSG::SNETPC_PHONE_NUMBER_FB *pNetMsg = (GLMSG::SNETPC_PHONE_NUMBER_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMSMS_PHONE_NUMBER_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_FAIL") );
				break;
			case EMSMS_PHONE_NUMBER_FB_OK:
				{
					StringCchCopy ( m_szPhoneNumber, SMS_RECEIVER, pNetMsg->szPhoneNumber );

					// 캐릭터 정보창에 폰 번호 변경
					CInnerInterface::GetInstance().GetCharacterWindow()->SetPhoneNumber( CString( pNetMsg->szPhoneNumber ) );

					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMSMS_PHONE_NUMBER_FB_OK"), pNetMsg->szPhoneNumber );
				}
				break;
			case EMSMS_PHONE_NUMBER_FROM_DB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSMS_PHONE_NUMBER_FROM_DB_FAIL"), pNetMsg->szPhoneNumber );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FB:
		{
			GLMSG::SNET_SET_SERVER_DELAYTIME_FB* pNetMsg = ( GLMSG::SNET_SET_SERVER_DELAYTIME_FB* ) nmg;
			if ( pNetMsg->bAllServer )
			{
                CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ADMIN_COLOR, "All Server DelayTime : %d MilliSeconds", pNetMsg->dwMillisec );
			}
			else
			{
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ADMIN_COLOR, "Server DelayTime : %d MilliSeconds", pNetMsg->dwMillisec );
			}
		}
		break;

	case NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FB:
		{
			GLMSG::SNET_SET_SERVER_SKIPPACKET_FB* pNetMsg = ( GLMSG::SNET_SET_SERVER_SKIPPACKET_FB* ) nmg;
			if ( pNetMsg->bAllServer )
			{
				if ( pNetMsg->wSkipPacketNum )
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ADMIN_COLOR, "All Server dose not process some packets" );
				else
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ADMIN_COLOR, "All Server process all packets" );
			}
			else
			{
				if ( pNetMsg->wSkipPacketNum )
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ADMIN_COLOR, "Server dose not process some packets" );
				else
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ADMIN_COLOR, "Server process all packets" );
			}
		}
		break;
		
	case NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FB:
		{
			GLMSG::SNET_SET_SERVER_CRASHTIME_FB* pNetMsg = ( GLMSG::SNET_SET_SERVER_CRASHTIME_FB* ) nmg;
			if ( pNetMsg->bAllServer )
			{
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ADMIN_COLOR, "After %d Seconds, All Server Force ShutDown", pNetMsg->dwSec );
			}
			else
			{
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::ADMIN_COLOR, "After %d Seconds, Server Force ShutDown", pNetMsg->dwSec );
			}
		}
		break;

#if defined(_RELEASED) || defined(KRT_PARAM)
	case NET_MSG_GM_SHOWMETHEMONEY:
		{
			GLMSG::SNET_GM_SHOWMETHEMONEY* pNetMsg = (GLMSG::SNET_GM_SHOWMETHEMONEY*)nmg;
			m_lnMoney = pNetMsg->llMoney;
		}
		break;
#endif

	case NET_MSG_GCTRL_TRADE_FB:
	case NET_MSG_GCTRL_TRADE_AGREE_TAR:
	case NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR:
	case NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR:
	case NET_MSG_GCTRL_TRADE_MONEY_TAR:
	case NET_MSG_GCTRL_TRADE_COMPLETE_TAR:
	case NET_MSG_GCTRL_TRADE_CANCEL_TAR:
		{
			GLTradeClient::GetInstance().MsgProcess ( nmg );
		}
		break;

	case NET_MSG_GCTRL_CONFRONT_FB:
	case NET_MSG_GCTRL_CONFRONT_START2_CLT:
	case NET_MSG_GCTRL_CONFRONT_FIGHT2_CLT:
	case NET_MSG_GCTRL_CONFRONT_END2_CLT:
	case NET_MSG_GCTRL_CONFRONTPTY_START2_CLT:
	case NET_MSG_GCTRL_CONFRONTPTY_END2_CLT:
	case NET_MSG_GCTRL_CONFRONT_RECOVE:
	case NET_MSG_GCTRL_CONFRONT_END2_CLT_MBR:
	case NET_MSG_GCTRL_CONFRONTCLB_START2_CLT:
		{
			MsgProcessConfront( nmg );
		}
		break;

	case NET_MSG_GCTRL_REQ_QUEST_START_FB:
	case NET_MSG_GCTRL_QUEST_PROG_STREAM:
	case NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM:
	case NET_MSG_GCTRL_QUEST_PROG_INVEN:
	case NET_MSG_GCTRL_QUEST_PROG_DEL:
	case NET_MSG_GCTRL_QUEST_END_STREAM:
	case NET_MSG_GCTRL_QUEST_END_DEL:
	case NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB:
	case NET_MSG_GCTRL_QUEST_PROG_MOBKILL:
	case NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL:
	case NET_MSG_GCTRL_QUEST_PROG_QITEM:
	case NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM:
	case NET_MSG_GCTRL_QUEST_PROG_REACHZONE:
	case NET_MSG_GCTRL_QUEST_PROG_TIMEOVER:
	case NET_MSG_GCTRL_QUEST_PROG_TIME:
	case NET_MSG_GCTRL_QUEST_PROG_NONDIE:
	case NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP:
	case NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT:
	case NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE:
	case NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN:
	case NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP:
	case NET_MSG_GCTRL_QUEST_COMPLETE_FB:
		{
			MsgProcessQuest( nmg );
		}
		break;

	case NET_MSG_GCTRL_CLUB_INFO_2CLT:
	case NET_MSG_GCTRL_CLUB_DEL_2CLT:
	case NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION:
	case NET_MSG_GCTRL_CLUB_MEMBER_2CLT:
	case NET_MSG_GCTRL_CLUB_NEW_FB:
	case NET_MSG_GCTRL_CLUB_MEMBER_REQ_ASK:
	case NET_MSG_GCTRL_CLUB_MEMBER_REQ_FB:
	case NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT:
	case NET_MSG_GCTRL_CLUB_RANK_2CLT:
	case NET_MSG_GCTRL_CLUB_RANK_FB:
	case NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB:
	case NET_MSG_GCTRL_CLUB_MEMBER_STATE:
	case NET_MSG_GCTRL_CLUB_MEMBER_POS:
	case NET_MSG_GCTRL_CLUB_DISSOLUTION_FB:
	case NET_MSG_GCTRL_CLUB_MEMBER_DEL_2CLT:
	case NET_MSG_GCTRL_CLUB_MEMBER_DEL_FB:
	case NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB:
	case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK:
	case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB:
	case NET_MSG_GCTRL_CLUB_AUTHORITY_CLT:

	case NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB:
	case NET_MSG_GCTRL_CLUB_COMMISSION_FB:
	case NET_MSG_GCTRL_CLUB_STORAGE_RESET:
	case NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY:
	case NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM:
	case NET_MSG_GCTRL_CLUB_STORAGE_INSERT:
	case NET_MSG_GCTRL_CLUB_STORAGE_DELETE:
	case NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS:
	case NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM:
	case NET_MSG_GCTRL_CLUB_NOTICE_FB:
	case NET_MSG_GCTRL_CLUB_NOTICE_CLT:
	case NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT:
	case NET_MSG_GCTRL_CLUB_SUBMASTER_FB:
	case NET_MSG_GCTRL_CLUB_SUBMASTER_BRD:
		{
			MsgProcessClub( nmg );
		}
		break;

	case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ASK:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_CLT:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT:
		{
			MsgProcessAlliance( nmg );
		}
		break;

	case NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB:
	case NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK:
	case NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT:
	case NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2:
	case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB:
	case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK:
	case NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT:
	case NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB:
	case NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE:
	case NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE:
		{
			MsgProcessClubBattle( nmg );
		}
		break;

	case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB:
		{
			MsgProcessAllianceBattle( nmg );
		}
		break;

	case NET_MSG_MGAME_ODDEVEN_FB:
		{
			MsgProcessMiniGame( nmg );
		}
		break;

#ifdef CH_PARAM_USEGAIN //chinatest%%%
	case NET_MSG_CHINA_GAINTYPE:
		{
			GLMSG::SNETPC_CHINA_GAINTYPE *pNetMsg = (GLMSG::SNETPC_CHINA_GAINTYPE *) nmg;
			if( pNetMsg->dwGainType == 1 )
			{
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CHINA_GAIN_50PERCENT") );
			}else if( pNetMsg->dwGainType == 2 )
			{
//				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CHINA_GAIN_0PERCENT") );				
//				CInnerInterface::GetInstance().WAITSERVER_DIALOGUE_OPEN ( ID2GAMEINTEXT("WAITSERVER_MESSAGE"), WAITSERVER_CLOSEGAME );
//				CInnerInterface::GetInstance().WAITSERVER_DIALOGUE_OPEN ( ID2GAMEINTEXT("CHINA_GAIN_CLOSEGAME"), WAITSERVER_CLOSEGAME, 30 );
				DoModal( ID2GAMEINTEXT("CHINA_GAIN_CLOSEGAME"), MODAL_INFOMATION, OK, MODAL_CLOSEGAME );
			}/*else if( pNetMsg->dwGainType == 3 )
			{
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CHINA_GAIN_15MINUTE") );				
			}*/else if( pNetMsg->dwGainType == 4 )
			{
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CHINA_GAIN_1HOUR") );				
			}
		}
		break;
#endif

#if defined(VN_PARAM) //vietnamtest%%%
	case NET_MSG_VIETNAM_GAINTYPE:
		{
			GLMSG::SNETPC_VIETNAM_GAINTYPE *pNetMsg = (GLMSG::SNETPC_VIETNAM_GAINTYPE *) nmg;
			if( pNetMsg->dwGainType == GAINTYPE_HALF )
			{
				m_dwVietnamGainType = GAINTYPE_HALF;
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VIETNAM_GAINTYPE_HALF") );
			}else if( pNetMsg->dwGainType == GAINTYPE_EMPTY )
			{
				m_dwVietnamGainType = GAINTYPE_EMPTY;
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VIETNAM_GAINTYPE_EMPTY") );
			}
		}
		break;
#endif

	case NET_MSG_VIETNAM_GAIN_MONEY:
		{
			GLMSG::SNETPC_VIETNAM_GAIN_MONEY *pNetMsg = (GLMSG::SNETPC_VIETNAM_GAIN_MONEY *) nmg;
			m_lVNGainSysMoney = pNetMsg->gainMoney;
		}
		break;

	case NET_MSG_VIETNAM_GAIN_EXP:
		{
			GLMSG::SNETPC_VIETNAM_GAIN_EXP *pNetMsg = (GLMSG::SNETPC_VIETNAM_GAIN_EXP *) nmg;
			m_lVNGainSysExp = pNetMsg->gainExp;
		}
		break;

#if defined(VN_PARAM) //vietnamtest%%%
	case NET_MSG_VIETNAM_TIME_REQ_FB:
		{
			GLMSG::SNETPC_VIETNAM_TIME_REQ_FB *pNetMsg = (GLMSG::SNETPC_VIETNAM_TIME_REQ_FB *)nmg;
			m_sVietnamSystem.loginTime = pNetMsg->loginTime;
			m_sVietnamSystem.gameTime  = pNetMsg->gameTime;
			m_VietnamGameTime  = pNetMsg->gameTime;

		}
		break;
#endif



#ifndef CH_PARAM_USEGAIN //** Add EventTime
	case NET_MSG_GM_LIMIT_EVENT_APPLY_START:
		{
			GLMSG::SNET_GM_LIMIT_EVENT_APPLY_START *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_APPLY_START *) nmg;
			m_bEventBuster    = TRUE;
			m_tLoginTime      = pNetMsg->loginTime;
			m_RemainEventTime = 0;
			m_RemainBusterTime = m_EventBusterTime;
			CInnerInterface::GetInstance().BONUS_TIME_BUSTER_START();
			
		}
		break;

	case NET_MSG_GM_LIMIT_EVENT_APPLY_END:
		{
			GLMSG::SNET_GM_LIMIT_EVENT_APPLY_END *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_APPLY_END *) nmg;
			m_bEventBuster    = FALSE;
			m_tLoginTime      = pNetMsg->loginTime;
			GLGaeaClient::GetInstance().SetCurrentTime( m_tLoginTime );
			m_RemainEventTime  = m_EventStartTime;
			m_RemainBusterTime = m_EventBusterTime;
			CInnerInterface::GetInstance().BONUS_TIME_BUSTER_END();

		}
		break;

	case NET_MSG_GM_LIMIT_EVENT_BEGIN_FB:
		{
			GLMSG::SNET_GM_LIMIT_EVENT_BEGIN_FB *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_BEGIN_FB *) nmg;
			/*CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, 
														  "%d 레벨 부터 %d 레벨 까지 %d분 플레이 %d 부스터 %.2f 경험치 %.2f 드랍율",
														  pNetMsg->start_Lv, pNetMsg->end_Lv, pNetMsg->play_Time, pNetMsg->buster_Time,
														  pNetMsg->expGain_Rate, pNetMsg->itemGain_Rate );*/

			m_bEventStart = TRUE;
			m_bEventBuster = FALSE;

			m_EventStartLv = pNetMsg->start_Lv;
			m_EventEndLv   = pNetMsg->end_Lv;

			m_EventBusterTime = pNetMsg->buster_Time;
			m_EventStartTime  = pNetMsg->play_Time;

			m_RemainEventTime  = m_EventStartTime;
			m_RemainBusterTime = m_EventBusterTime;

			if( m_wLevel >= m_EventStartLv && m_wLevel <= m_EventEndLv )
			{
				CTime crtTime = GLGaeaClient::GetInstance().GetCurrentTime();
				m_tLoginTime  = crtTime.GetTime();
				m_bEventApply = TRUE;
				CInnerInterface::GetInstance().BONUS_TIME_EVENT_START( !m_bEventBuster );
			}else{
				m_bEventApply = FALSE;
				CInnerInterface::GetInstance().BONUS_TIME_EVENT_END();
			}
			
			
		}
		break;

	case NET_MSG_GM_LIMIT_EVENT_END_FB:
		{
			//CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, "이벤트 종료!!" );

			m_bEventApply  = FALSE;
			m_bEventStart  = FALSE;
			m_bEventBuster = FALSE;
			CInnerInterface::GetInstance().BONUS_TIME_EVENT_END();
		}
		break;

	case NET_MSG_GM_LIMIT_EVENT_TIME_REQ_FB:
		{
			GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ_FB *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ_FB *)nmg;

			m_bEventStart  = pNetMsg->bEventStart;
			if( m_bEventStart )
			{
				m_tLoginTime   = pNetMsg->loginTime;
				GLGaeaClient::GetInstance().SetCurrentTime( m_tLoginTime );
				m_EventStartLv = pNetMsg->start_Lv;
				m_EventEndLv   = pNetMsg->end_Lv;
				m_EventBusterTime = pNetMsg->buster_Time;
				m_EventStartTime  = pNetMsg->play_Time;

				if( m_wLevel >= m_EventStartLv && m_wLevel <= m_EventEndLv )
				{
					m_bEventApply  = TRUE;
					CInnerInterface::GetInstance().BONUS_TIME_EVENT_START( !m_bEventBuster );
				}else{
					m_bEventApply  = FALSE;
					CInnerInterface::GetInstance().BONUS_TIME_EVENT_END();
				}
			}
			


		}
		break;
#endif

	case NET_MSG_GCTRL_ACTIVE_VEHICLE_FB:
		{
			GLMSG::SNETPC_ACTIVE_VEHICLE_FB* pNetMsg = (GLMSG::SNETPC_ACTIVE_VEHICLE_FB*) nmg;

			switch( pNetMsg->emFB)
			{
			case EMVEHICLE_SET_FB_RESET:
				{
					break;
				}
			case EMVEHICLE_SET_FB_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_FAIL") );
					break;
				}			
			case EMVEHICLE_SET_FB_OK:
				{
 					if ( pNetMsg->bActive )
					{
						SetVehicle(  true );
					}
					else
					{
						if ( pNetMsg->bLeaveFieldServer )	m_bIsVehicleActive = true;
						SetVehicle(  false );
						
					}

					break;
				}

			case EMVEHICLE_SET_FB_NOTENOUGH_OIL:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_NOTENOUGH_OIL") );
					break;
				}
			case EMVEHICLE_SET_FB_MAP_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_MAP_FAIL") );
					break;
				}

			case EMVEHICLE_SET_FB_NO_ITEM:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_NO_ITEM") );
					break;
				}
			}

			
		}
		break;

	case NET_MSG_GCTRL_LANDEFFECT:
		{
			GLMSG::SNETPC_LANDEFFECT* pNetMsg = ( GLMSG::SNETPC_LANDEFFECT* ) nmg;
			memcpy( m_sLandEffect, pNetMsg->sLandEffect, sizeof(m_sLandEffect) );
		}
		break;


	case NET_MSG_GCTRL_GET_VEHICLE_FB:
		{
			GLMSG::SNETPC_GET_VEHICLE_FB* pNetMsg = (GLMSG::SNETPC_GET_VEHICLE_FB*) nmg;

			switch( pNetMsg->emFB ) 
			{
			case EMVEHICLE_GET_FB_OK:
				{
					m_PutOnItems[SLOT_VEHICLE].dwVehicleID = pNetMsg->dwGUID;
					m_sVehicle.m_dwGUID = pNetMsg->dwGUID;
					m_sVehicle.m_emTYPE = pNetMsg->emTYPE;
					m_sVehicle.m_dwOwner = pNetMsg->dwOwner;
					m_sVehicle.m_sVehicleID = pNetMsg->sVehicleID;
					m_sVehicle.m_nFull	= pNetMsg->nFull;
					for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
					{
						m_sVehicle.m_PutOnItems[i] = pNetMsg->PutOnItems[i];
					}
					
					m_sVehicle.SetActiveValue( true );
					
					m_sVehicle.ITEM_UPDATE();

					m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
					m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();
					
					UpdateSuit ( FALSE );
					INIT_DATA( FALSE, FALSE );

					if ( m_bIsVehicleActive ) 
					{
						ReqSetVehicle( true );
						m_bIsVehicleActive = FALSE;
					}
				}
				break;
			case EMVEHICLE_GET_FB_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "VEHICLE_GET_FB_FAIL" ));
				}
				break;
			case EMVEHICLE_GET_FB_INVALIDITEM:
				{
					CInnerInterface::GetInstance().PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "VEHICLE_GET_FB_INVALIDITEM" ));					
				}
				break;
			case EMVEHICLE_GET_FB_NOITEM:
				{
					CInnerInterface::GetInstance().PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "VEHICLE_GET_FB_NOITEM" ));					
				}
				break;			
			case EMVEHICLE_GET_FB_NODATA:
				{
					if ( m_PutOnItems[SLOT_VEHICLE].sNativeID != NATIVEID_NULL() ) 
						m_PutOnItems[SLOT_VEHICLE].dwVehicleID = 0;
					
					CInnerInterface::GetInstance().PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "VEHICLE_GET_FB_NODATA" ));
				}
				break;

			}
		break;
			
		}
	case NET_MSG_GCTRL_UNGET_VEHICLE_FB:
		{
			GLMSG::SNETPC_UNGET_VEHICLE_FB* pNetMsg = (GLMSG::SNETPC_UNGET_VEHICLE_FB*) nmg;

			m_sVehicle.SetActiveValue( false );
			m_sVehicle.RESET();
		}
		break;

	case NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_FB:
		{
			GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_FB* ) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( 
					NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL") );

				break;

			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK:
				{
					// 장착된 아이템 BackUp
					SITEMCUSTOM sSlotItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );

					// 손에든 아이템을 팻에게 장착시키고
					m_sVehicle.SetSlotItem ( pNetMsg->emSuit, GET_HOLD_ITEM () );
					m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
					m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();


					// 탈것의 정보를 갱신해준다.
					VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( m_sVehicle.m_dwGUID );
					if ( iter!= m_mapVEHICLEItemInfo.end() )
					{
						SVEHICLEITEMINFO& sVehicle = (*iter).second;
						sVehicle.m_PutOnItems[(WORD)pNetMsg->emSuit-(WORD)SUIT_VEHICLE_SKIN] = GET_HOLD_ITEM ();
					}

					// 장착됐던 아이템을 손에 든다
					HOLD_ITEM ( sSlotItemCustom );
					UpdateSuit ( FALSE );
					INIT_DATA( FALSE, FALSE );
				}
				break;

			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH:
				CInnerInterface::GetInstance().PrintMsgText ( 
					NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH") );
				break;

			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
				CInnerInterface::GetInstance().PrintMsgText ( 
					NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM") );
				break;
			};
		}
		break;
	case NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT_FB:
		{
			GLMSG::SNET_VEHICLE_REQ_HOLD_TO_SLOT_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_HOLD_TO_SLOT_FB* ) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( 
					NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL") );
				break;

			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK:
				{
					// 손에든 아이템을 팻에게 장착시키고
					m_sVehicle.SetSlotItem ( pNetMsg->emSuit, GET_HOLD_ITEM () );
					m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
					m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();					

					// 탈것의 정보를 갱신해준다.
					VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( m_sVehicle.m_dwGUID );
					if ( iter!= m_mapVEHICLEItemInfo.end() )
					{
						SVEHICLEITEMINFO& sVehicle = (*iter).second;
						sVehicle.m_PutOnItems[(WORD)pNetMsg->emSuit-(WORD)SUIT_VEHICLE_SKIN] = GET_HOLD_ITEM ();
					}

					// 손에든 아이템을 제거하고
					RELEASE_HOLD_ITEM ();
					UpdateSuit ( FALSE );
					INIT_DATA( FALSE, FALSE );
				}
				break;

			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH:
				CInnerInterface::GetInstance().PrintMsgText ( 
					NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH") );
				break;

			case EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
				CInnerInterface::GetInstance().PrintMsgText ( 
					NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM") );
				break;
			};
		}
		break;
	case NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD_FB:
		{
			GLMSG::SNET_VEHICLE_REQ_SLOT_TO_HOLD_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_SLOT_TO_HOLD_FB* ) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( 
					NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL") );
				break;

			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK:
				{
					// 장착된 아이템 BackUp
					SITEMCUSTOM sSlotItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );

					// 장착된 아이템 제거
					m_sVehicle.ReSetSlotItem ( pNetMsg->emSuit );
					m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
					m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();

					// 탈것의 정보를 갱신해준다.
					VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( m_sVehicle.m_dwGUID );
					if ( iter!= m_mapVEHICLEItemInfo.end() )
					{
						SVEHICLEITEMINFO& sVehicle = (*iter).second;
						WORD i = (WORD)pNetMsg->emSuit - (WORD ) SUIT_VEHICLE_SKIN;
						if ( i < ACCE_TYPE_SIZE ) sVehicle.m_PutOnItems[i] = SITEMCUSTOM ( NATIVEID_NULL() );
					}

					// 장착됐던 아이템을 손에 든다
					HOLD_ITEM ( sSlotItemCustom );
					UpdateSuit ( FALSE );
					INIT_DATA( FALSE, FALSE );
				}
				break;

			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH:
				CInnerInterface::GetInstance().PrintMsgText ( 
					NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH") );
				break;

			case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
				CInnerInterface::GetInstance().PrintMsgText ( 
					NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM") );
				break;
			}
		}
		break;
	
	case NET_MSG_VEHICLE_REMOVE_SLOTITEM_FB:
		{
			GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB* ) nmg;
	
			WORD wPosX(0), wPosY(0);

			SITEMCUSTOM	sSlotItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );

			SITEM* pSlotItem = GLItemMan::GetInstance().GetItem ( sSlotItemCustom.sNativeID );
			if ( !pSlotItem ) 
			{
				// 일반 오류
				return;
			}

			BOOL bOk = m_cInventory.FindInsrtable ( pSlotItem->sBasicOp.wInvenSizeX, pSlotItem->sBasicOp.wInvenSizeY, wPosX, wPosY );
			if ( !bOk )
			{
				//	인밴이 가득찻음.
				return;
			}

			// 인벤에 넣기
			m_cInventory.InsertItem ( sSlotItemCustom, wPosX, wPosY );

			// 슬롯아이템 제거
			m_sVehicle.ReSetSlotItem ( pNetMsg->emSuit );

			m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
			m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();


			// 탈것의 정보를 갱신해준다.
			VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( m_sVehicle.m_dwGUID );
			if ( iter!= m_mapVEHICLEItemInfo.end() )
			{
				SVEHICLEITEMINFO& sVehicle = (*iter).second;
				WORD i = (WORD)pNetMsg->emSuit - (WORD ) SUIT_VEHICLE_SKIN;
				if ( i < ACCE_TYPE_SIZE ) sVehicle.m_PutOnItems[i] = SITEMCUSTOM ( NATIVEID_NULL() );
			}

			UpdateSuit ( FALSE );
			INIT_DATA( FALSE, FALSE );
		}
		break;
	case NET_MSG_VEHICLE_ACCESSORY_DELETE:
		{
			GLMSG::SNET_VEHICLE_ACCESSORY_DELETE* pNetMsg = (GLMSG::SNET_VEHICLE_ACCESSORY_DELETE*) nmg;
			
			if ( m_sVehicle.IsActiveValue() && m_sVehicle.m_dwGUID == pNetMsg->dwVehicleNum )
			{
                m_sVehicle.m_PutOnItems[pNetMsg->accetype] = SITEMCUSTOM ();			
				m_sVehicle.ITEM_UPDATE();
				m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
				m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();
				UpdateSuit ( FALSE );
				INIT_DATA( FALSE, FALSE );
			}

			VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( pNetMsg->dwVehicleNum  );
			if ( iter!=m_mapVEHICLEItemInfo.end() )
			{
				SVEHICLEITEMINFO& sVehicle = (*iter).second;
				// 팻카드의 정보 갱신
				sVehicle.m_PutOnItems[pNetMsg->accetype] = SITEMCUSTOM ();	
			}


		}
		break;
	case NET_MSG_VEHICLE_UPDATE_CLIENT_BATTERY:
		{
			GLMSG::SNET_VEHICLE_UPDATE_CLIENT_BATTERY* pNetMsg = ( GLMSG::SNET_VEHICLE_UPDATE_CLIENT_BATTERY* ) nmg;
			m_sVehicle.m_nFull = pNetMsg->nFull;

			
			VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( m_sVehicle.m_dwGUID );
			if ( iter!=m_mapVEHICLEItemInfo.end() )
			{
				SVEHICLEITEMINFO& sVehicle = (*iter).second;
				// 팻카드의 정보 갱신
				sVehicle.m_nFull = pNetMsg->nFull;
			}

		}
        break;
	case NET_MSG_VEHICLE_REQ_GIVE_BATTERY_FB:
		{
			GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY_FB* ) nmg;

			switch( pNetMsg->emFB )
			{
			case EMVEHICLE_REQ_GIVE_BATTERY_FB_OK:
				{
					SITEM* pItem = GLItemMan::GetInstance().GetItem ( pNetMsg->sBatteryID );
					SITEM* pVehicleItem	= GLItemMan::GetInstance().GetItem ( pNetMsg->sItemID );
					if ( pItem && pVehicleItem )
					{
						// 팻포만감 갱신
						if ( m_sVehicle.m_dwGUID == pNetMsg->dwVehicleID ) m_sVehicle.IncreaseFull ( pItem->sDrugOp.wCureVolume, pItem->sDrugOp.bRatio );

						if ( DxGlobalStage::GetInstance().IsEmulator() )
						{
							CInnerInterface::GetInstance().PrintMsgText ( 
								NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMVEHICLE_REQ_GIVE_BATTERY_FB_OK"), pVehicleItem->GetName(), pItem->sDrugOp.wCureVolume );					
							break;
						}


						VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( pNetMsg->dwVehicleID );
						if ( iter!=m_mapVEHICLEItemInfo.end() )
						{
							SVEHICLEITEMINFO& sVehicle = (*iter).second;
							// 팻카드의 정보 갱신
							sVehicle.m_nFull = pNetMsg->nFull;

							CInnerInterface::GetInstance().PrintMsgText ( 
								NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMVEHICLE_REQ_GIVE_BATTERY_FB_OK"), pVehicleItem->GetName(), pItem->sDrugOp.wCureVolume );
							break;
						}

					}
				}
                break;
			case EMVEHICLE_REQ_GIVE_BATTERY_FB_FAIL:
                break;
			case EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDBATTERY:
                break;
			case EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDITEM:
                break;
			case EMVEHICLE_REQ_GET_BATTERY_FROMDB_OK:
                break;
			case EMVEHICLE_REQ_GET_BATTERY_FROMDB_FAIL:
                break;
			}
		}
		break;
	case NET_MSG_VEHICLE_REQ_ITEM_INFO_FB:
		{
			GLMSG::SNET_VEHICLE_REQ_ITEM_INFO_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_ITEM_INFO_FB* ) nmg;

			// DB에 없으면 그냥 초기값으로 넘어오는 경우가 있으므로 체크해준다.
			if ( pNetMsg->emTYPE == VEHICLE_TYPE_NONE ) break;

			SVEHICLEITEMINFO sVehicleItemInfo;
			sVehicleItemInfo.m_emTYPE = pNetMsg->emTYPE;
			sVehicleItemInfo.m_nFull  = pNetMsg->nFull;

			for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
			{
				sVehicleItemInfo.m_PutOnItems[i] = pNetMsg->PutOnItems[i];
			}

			if ( !pNetMsg->bTrade )
			{
				VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( pNetMsg->dwVehicleID );
				if ( iter != m_mapVEHICLEItemInfo.end() ) m_mapVEHICLEItemInfo.erase ( iter );

				m_mapVEHICLEItemInfo.insert ( std::make_pair(pNetMsg->dwVehicleID, sVehicleItemInfo) );
			}
			else
			{
				m_mapVEHICLEItemInfoTemp.insert ( std::make_pair(pNetMsg->dwVehicleID, sVehicleItemInfo) );
			}						

		}

		break;

	case NET_QBOX_OPTION_MEMBER:
		{
			GLMSG::SNET_QBOX_OPTION_MEMBER* pNetMsg = ( GLMSG::SNET_QBOX_OPTION_MEMBER* ) nmg;
			CInnerInterface::GetInstance().GetQBoxButton()->SetQBoxEnable(pNetMsg->bQBoxEnable);			
		}
		break;

	case NET_MSG_UPDATE_TRACING_CHARACTER:
		{
			NET_UPDATE_TRACINGCHAR* pNetMsg = ( NET_UPDATE_TRACINGCHAR* ) nmg;
			if( pNetMsg->updateNum == 0 )
				m_bTracingUser = FALSE;
			else
				m_bTracingUser = TRUE;
		}
		break;
	case NET_MSG_REQ_ATTENDLIST_FB:
		{
			GLMSG::SNETPC_REQ_ATTENDLIST_FB* pNetMsg = ( GLMSG::SNETPC_REQ_ATTENDLIST_FB* ) nmg;
			
			m_vecAttend.clear();

			for ( int i = 0; i < (int)pNetMsg->dwAttenNum; ++i )
			{
				m_vecAttend.push_back( pNetMsg->sAttend[i] );	
			}
			
			m_dwComboAttend = pNetMsg->dwAttendCombo;
			
			m_tAttendLogin = pNetMsg->tLoginTime;
			m_dwAttendTime = pNetMsg->dwAttendTime;
			CInnerInterface::GetInstance().REFRESH_ATTENDBOOK();

		}
		break;
	case NET_MSG_REQ_ATTENDANCE_FB:
		{
			GLMSG::SNETPC_REQ_ATTENDANCE_FB* pNetMsg = ( GLMSG::SNETPC_REQ_ATTENDANCE_FB* ) nmg;

			switch( pNetMsg->emFB )
			{
			case EMREQ_ATTEND_FB_OK:
				{
					m_dwComboAttend = pNetMsg->dwComboAttend;
					USER_ATTEND_INFO sAttend;
					sAttend.tAttendTime = pNetMsg->tAttendTime;
					sAttend.bAttendReward = pNetMsg->bAttendReward;
					m_vecAttend.push_back( sAttend );

					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_ATTEND_FB_OK"), m_dwComboAttend );
				}
				break;
			case EMREQ_ATTEND_FB_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_ATTEND_FB_FAIL") );
				}
				break;
			case EMREQ_ATTEND_FB_ALREADY:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_ATTEND_FB_ALREADY") );
				}
				break;
			case EMREQ_ATTEND_FB_ATTENTIME:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_ATTEND_FB_ATTENTIME"),m_dwAttendTime );
				}
				break;
			}

			CInnerInterface::GetInstance().REFRESH_ATTENDBOOK();

		}
		break;
	case NET_MSG_REQ_ATTEND_REWARD_CLT:
		{
			GLMSG::SNETPC_REQ_ATTEND_REWARD_CLT* pNetMsg = ( GLMSG::SNETPC_REQ_ATTEND_REWARD_CLT* ) nmg;

			SITEM* pItem = GLItemMan::GetInstance().GetItem( pNetMsg->idAttendReward );
			
			if ( pItem )
			{
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("ATTEND_REWARD_ITEM"), pItem->GetName()  );
			}
			else
			{
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ATTEND_REWARD_NOITEM") );
			}

		}
		break;
	case NET_MSG_GCTRL_NPC_RECALL_FB:
		{
			GLMSG::SNET_INVEN_NPC_RECALL_FB* pNetMsg = ( GLMSG::SNET_INVEN_NPC_RECALL_FB* ) nmg;

			switch( pNetMsg->emFB )
			{
			case EMREQ_NPC_RECALL_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_FAIL") );
				break;
			case EMREQ_NPC_RECALL_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_OK") );
				break;
			case EMREQ_NPC_RECALL_FB_NOITEM:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_NOITEM") );
				break;
			case EMREQ_NPC_RECALL_FB_NPC:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_NPC") );
				break;
			case EMREQ_NPC_RECALL_FB_POS:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_POS") );
				break;
			}
		}
		break;
	case NET_MSG_GCTRL_INVEN_ITEM_MIX_FB:
		{
			GLMSG::SNET_INVEN_ITEM_MIX_FB* pNetMsg = ( GLMSG::SNET_INVEN_ITEM_MIX_FB* ) nmg;
			CString strMsg;

			bool bSucc = false;
			bool bFail = false;

			DWORD dwColor = NS_UITEXTCOLOR::DISABLE;

			switch( pNetMsg->emFB )
			{
			case EMITEM_MIX_FB_FAIL:
				{				
					strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_FAIL");
					bFail = true;
				}
				break;
			case EMITEM_MIX_FB_OK:
				{				
					SITEM* pItem = GLItemMan::GetInstance().GetItem( pNetMsg->sNativeID );
					if ( pItem ) 
					{
						strMsg = CInnerInterface::GetInstance().MakeString ( ID2GAMEINTEXT("EMITEM_MIX_FB_OK") , pItem->GetName(), pNetMsg->wTurnNum );
						dwColor = NS_UITEXTCOLOR::PALEGREEN;
						bSucc = true;
					}					
				}				
				break;
			case EMITEM_MIX_FB_ERROR:
				{				
					strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_ERROR");
				}
				break;				
			case EMITEM_MIX_FB_NOMIX:
				strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NOMIX");
				break;
			case EMITEM_MIX_FB_NOMIXNUM:
				strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NOMIXNUM");
				break;
			case EMITEM_MIX_FB_NOITEM:
				strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NOITEM");
				break;
			case EMITEM_MIX_FB_NOMONEY:
				strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NOMONEY");
				break;
			case EMITEM_MIX_FB_NOINVEN:
				strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NOINVEN");
				break;
			case EMITEM_MIX_FB_NONPC:
				strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NONPC");
				break;
			}
			
			CInnerInterface::GetInstance().PrintMsgText ( dwColor, strMsg );
			CInnerInterface::GetInstance().SetItemMixResult( strMsg, bSucc, bFail );
			
			GLGaeaClient::GetInstance().GetCharacter()->ResetItemMix();
		}
		break;
	case NET_MSG_REQ_GATHERING_FB:
		{
			GLMSG::SNETPC_REQ_GATHERING_FB* pNetMsg = ( GLMSG::SNETPC_REQ_GATHERING_FB* ) nmg;
			
			switch ( pNetMsg->emFB )
			{
			case EMREQ_GATHER_FB_OK:
				{					
					CInnerInterface::GetInstance().PrintMsgText 
						( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_GATHER_FB_OK") );
					
					m_dwANISUBGESTURE = pNetMsg->dwAniSel;
					TurnAction( GLAT_GATHERING );
					CInnerInterface::GetInstance().SET_GATHER_GAUGE( pNetMsg->dwGaeaID, (float)pNetMsg->dwTime );
					CInnerInterface::GetInstance().ShowGroupBottom( GATHER_GAUGE );

				}
				break;
			case EMREQ_GATHER_FB_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgText 
						( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_FAIL") );
				}
				break;
			case EMREQ_GATHER_FB_DISTANCE:
				{
					CInnerInterface::GetInstance().PrintMsgText 
						( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_DISTANCE") );
				}
				break;
			case EMREQ_GATHER_FB_NOTTYPE:
				{
					CInnerInterface::GetInstance().PrintMsgText 
						( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_NOTTYPE") );
				}
				break;
			case EMREQ_GATHER_FB_NOCROW:
				{
					CInnerInterface::GetInstance().PrintMsgText 
						( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_NOCROW") );
				}
				break;
			case EMREQ_GATHER_FB_USE:
				{
					CInnerInterface::GetInstance().PrintMsgText 
						( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_USE") );
				}
				break;
			case EMREQ_GATHER_FB_GATHERING:
				{
					CInnerInterface::GetInstance().PrintMsgText 
						( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_GATHERING") );
				}
				break;
			}
		}
		break;
	case NET_MSG_REQ_GATHERING_RESULT:
		{
			GLMSG::SNETPC_REQ_GATHERING_RESULT* pNetMsg = ( GLMSG::SNETPC_REQ_GATHERING_RESULT* ) nmg;
			
			switch ( pNetMsg->emFB )
			{
			case EMREQ_GATHER_RESULT_SUCCESS:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, 
							ID2GAMEINTEXT("EMREQ_GATHER_RESULT_SUCCESS") );

					m_dwANISUBGESTURE = 0;	//	성공
				}
				break;
			case EMREQ_GATHER_RESULT_SUCCESS_EX:
				{
					CInnerInterface::GetInstance().PrintMsgText 
						( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_RESULT_SUCCESS_EX") );	
					
					m_dwANISUBGESTURE = 0;	//	성공
				}
				break;
			case EMREQ_GATHER_RESULT_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgText 
						( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_RESULT_FAIL") );	

					m_dwANISUBGESTURE = 1;	//	실패
				}
				break;
			case EMREQ_GATHER_RESULT_ITEMFAIL:
				{
					CInnerInterface::GetInstance().PrintMsgText 
						( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_RESULT_ITEMFAIL") );					
					
					m_dwANISUBGESTURE = 1;	//	실패
				}
				break;
			}			
			
			CInnerInterface::GetInstance().HideGroup( GATHER_GAUGE );
		}
		break;
	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess() 분류되지 않은 메시지 수신. TYPE[%d]", nmg->nType );
		break;
	};


}


void GLCharacter::MsgDefenseSkillActive( GLMSG::SNETPC_DEFENSE_SKILL_ACTIVE* nmg )
{
	BOOL bMove(FALSE);
	D3DXVECTOR3 vMoveTo;

	PGLSKILL pRunSkill = GLSkillMan::GetInstance().GetData ( nmg->sNativeID );
	if ( !pRunSkill )										return;

	GLCOPY* pCOPY = GLGaeaClient::GetInstance().GetCopyActor ( nmg->emTarCrow, nmg->dwTarID );
	if ( !pCOPY )	return;

	if ( IsACTION(GLAT_SKILL) && IsDefenseSkill() )
	{
		return;
	}
	
	// 스킬 캔슬 메세지
	if ( IsACTION(GLAT_SKILL) )
	{
		TurnAction(GLAT_IDLE);
		GLMSG::SNETPC_SKILL_CANCEL NetMsg;
		NETSENDTOFIELD ( &NetMsg );
	}

	
	STARGETID	sTID( nmg->emTarCrow, nmg->dwTarID);
	sTID.vPos = pCOPY->GetPosition();

	SetDefenseSkill( true );

	m_sActiveSkill = nmg->sNativeID;
	SkillReaction ( sTID, DXKEY_UP, false, bMove, vMoveTo );

	//	Note : Reaction 에서 이동을 요청한 경우.
	//
	if ( bMove )
	{
		ActionMoveTo ( 0.0f, vMoveTo+D3DXVECTOR3(0,+5,0), vMoveTo+D3DXVECTOR3(0,-5,0), FALSE, TRUE );
	}
}

