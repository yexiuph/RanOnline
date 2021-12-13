#include "pch.h"
#include "ConftModalWindow.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicComboBox.h"
#include "BasicComboBoxRollOver.h"
#include "InnerInterface.h"
#include "RANPARAM.h"
#include "GLogicData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CConftModalWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case CONFT_MODAL_USABLEHP_COMBO_OPEN:
		{
			if ( m_bReqModal )	//	신청시에만 동작
			{
				if ( CHECK_MOUSE_IN ( dwMsg ) )
				{
					if ( dwMsg & UIMSG_LB_DOWN )
					{
						m_pComboBoxUsableHPRollOver->SetVisibleSingle ( TRUE );
						m_pComboBoxUsableHPRollOver->SetScrollPercent ( 0.0f );
						m_pComboBoxHPStateMyRollOver->SetVisibleSingle ( FALSE );					
						m_pComboBoxHPStateHisRollOver->SetVisibleSingle ( FALSE );

						m_RollOverID = CONFT_MODAL_USABLEHP_COMBO_ROLLOVER;
						m_bFirstLBUP = TRUE;
					}
				}
			}
		}
		break;

	case CONFT_MODAL_USABLEHP_COMBO_ROLLOVER:
		{
			if ( m_bReqModal )	//	신청시에만 동작
			{
				if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
				{	
					DWORD dwMsg = GetMessageEx ();
					dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
					ResetMessageEx ();
					AddMessageEx ( dwMsg );
					break;
				}

				if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{				
					int nIndex = m_pComboBoxUsableHPRollOver->GetSelectIndex ();
					if ( nIndex < 0 ) return ;
					RANPARAM::wCONFT_RECOVER = GLCONST_CHAR::wCONFT_RC_TYPE[nIndex];
					LoadUsableHP ();
				}
			}
		}
		break;

	case CONFT_MODAL_HPSTATEMY_COMBO_OPEN:
		{
			if ( m_bReqModal )	//	신청시에만 동작
			{
				if ( CHECK_MOUSE_IN ( dwMsg ) )
				{
					if ( dwMsg & UIMSG_LB_DOWN )
					{
						m_pComboBoxUsableHPRollOver->SetVisibleSingle ( FALSE );
						m_pComboBoxHPStateHisRollOver->SetVisibleSingle ( FALSE );
						m_pComboBoxHPStateMyRollOver->SetVisibleSingle ( TRUE );
						m_pComboBoxHPStateMyRollOver->SetScrollPercent ( 0.0f );
						
						m_RollOverID = CONFT_MODAL_HPSTATEMY_COMBO_ROLLOVER;
						m_bFirstLBUP = TRUE;
					}
				}
			}
		}
		break;

	case CONFT_MODAL_HPSTATEMY_COMBO_ROLLOVER:
		{
			if ( m_bReqModal )	//	신청시에만 동작
			{
				if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
				{	
					DWORD dwMsg = GetMessageEx ();
					dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
					ResetMessageEx ();
					AddMessageEx ( dwMsg );
					break;
				}

				if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					if ( m_bReqModal )
					{
						int nIndex = m_pComboBoxHPStateMyRollOver->GetSelectIndex ();
						if ( nIndex < 0 ) return ;
						RANPARAM::fCONFT_HP_RATE = GLCONST_CHAR::fCONFT_STATE_TYPE[nIndex];
					}

					LoadHPStateMy ();
				}
			}
		}
		break;

	case CONFT_MODAL_HPSTATEHIS_COMBO_OPEN:
		{
			if ( m_bReqModal )	//	신청시에만 동작
			{
				if ( CHECK_MOUSE_IN ( dwMsg ) )
				{
					if ( dwMsg & UIMSG_LB_DOWN )
					{
						m_pComboBoxUsableHPRollOver->SetVisibleSingle ( FALSE );
						m_pComboBoxHPStateMyRollOver->SetVisibleSingle ( FALSE );
						m_pComboBoxHPStateHisRollOver->SetVisibleSingle ( TRUE );
						m_pComboBoxHPStateHisRollOver->SetScrollPercent ( 0.0f );
						
						m_RollOverID = CONFT_MODAL_HPSTATEHIS_COMBO_ROLLOVER;
						m_bFirstLBUP = TRUE;
					}
				}
			}
		}
		break;

	case CONFT_MODAL_HPSTATEHIS_COMBO_ROLLOVER:
		{
			if ( m_bReqModal )	//	신청시에만 동작
			{
				if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
				{				
					DWORD dwMsg = GetMessageEx ();
					dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
					ResetMessageEx ();
					AddMessageEx ( dwMsg );
					break;
				}

				if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					if ( m_bReqModal )
					{
						int nIndex = m_pComboBoxHPStateHisRollOver->GetSelectIndex ();
						if ( nIndex < 0 ) return ;
						RANPARAM::fCONFT_TAR_HP_RATE = GLCONST_CHAR::fCONFT_STATE_TYPE[nIndex];
					}

					LoadHPStateHis ();
				}
			}
		}
		break;

	//case CONFT_MODAL_RANGELIMIT_BUTTON:
	//	{
	//		if ( m_bReqModal )	//	신청시에만 동작
	//		{
	//			if ( CHECK_MOUSE_IN ( dwMsg ) )
	//			{
	//				if ( dwMsg & UIMSG_LB_DOWN )
	//				{
	//					if ( m_pRangeLimitButton )
	//					{
	//						RANPARAM::bCONFT_BOUND = !RANPARAM::bCONFT_BOUND;
	//						LoadRangeLimit ();
	//					}
	//				}
	//			}
	//		}
	//	}
	//	break;

	case MODAL_CONFT_YES:
	case MODAL_CONFT_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//	메시지
				//
				AddMessageEx ( UIMSG_MODAL_OK );
				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;

	case MODAL_CONFT_NO:
	case MODAL_CONFT_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//	메시지
				//
				AddMessageEx ( UIMSG_MODAL_CANCEL );
				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;
	}
}