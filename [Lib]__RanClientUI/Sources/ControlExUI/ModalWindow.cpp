#include "pch.h"
#include "ModalWindow.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "InnerInterface.h"
#include "OuterInterface.h"
#include "BasicLineBox.h"
#include "UITextControl.h"
#include "DXInputString.h"

#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CModalWindow::nLIMIT_CHAR = 16;
const int CModalWindow::nMONEY_UNIT = 3;
const int CModalWindow::nLIMIT_NUMBER_CHAR = 11;
const int CModalWindow::nLIMIT_MONEY_CHAR = 11;
int CModalWindow::m_nMODAL_NEWID = 1;
const CString CModalWindow::strMONEY_SYMBOL = _T(",");

static bool bINTERFACE_TEST = true;

CModalWindow::CModalWindow () :
	m_pOK ( NULL ),
	m_pCANCEL ( NULL ),
	m_pYES ( NULL ),
	m_pNO ( NULL ),
	m_p1BUTTON ( NULL ),
	m_p2BUTTONLEFT ( NULL ),
	m_p2BUTTONRIGHT ( NULL ),
	m_pTextBox ( NULL ),
	m_pEditBox ( NULL ),
	m_nModalType ( MODAL_INFOMATION ),
	m_bUseInner ( true ),
	m_bTEST_MULTI ( false ),
	m_nData1 ( -1 ),
	m_nData2 ( -1 )
{
}

CModalWindow::~CModalWindow ()
{
}

void CModalWindow::CreateBaseModal ( char* szWindowKeyword )
{
	if ( bINTERFACE_TEST )
	{
	}
	else
	{
		CreateTitle ( "MODAL_WINDOW_TITLE", "MODAL_WINDOW_TITLE_LEFT", "MODAL_WINDOW_TITLE_MID", "MODAL_WINDOW_TITLE_RIGHT", "MODAL_WINDOW_TEXTBOX", NULL );
	//	CreateCloseButton ( "MODAL_WINDOW_CLOSE", "MODAL_WINDOW_CLOSE_F", "MODAL_WINDOW_CLOSE_OVER" );
		CreateBody ( "MODAL_WINDOW_BODY", "MODAL_WINDOW_BODY_LEFT", "MODAL_WINDOW_BODY_UP", "MODAL_WINDOW_BODY_MAIN", "MODAL_WINDOW_BODY_DOWN", "MODAL_WINDOW_BODY_RIGHT" );

		ResizeControl ( szWindowKeyword );
	}
}

void CModalWindow::CreateSubControl ()
{
	if ( bINTERFACE_TEST )
	{
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicLineBox* pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessage ( "BASIC_MODAL_LINE_BOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );	

		pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessageUp ( "BASIC_MODAL_LINE_TEXTBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );

		CBasicTextBox* pMapMoveTextBox = new CBasicTextBox;
		pMapMoveTextBox->CreateSub ( this, "BASIC_MODAL_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pMapMoveTextBox->SetFont ( pFont );
		pMapMoveTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		pMapMoveTextBox->ResetAlignFlag ();
		RegisterControl ( pMapMoveTextBox );
		m_pTextBox = pMapMoveTextBox;

		m_pOK = CreateTextButton ( "MODAL_OK", MODAL_OK, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 0 ) );
		m_pOK->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
		m_pCANCEL = CreateTextButton ( "MODAL_CANCEL", MODAL_CANCEL, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 1 ) );
		m_pCANCEL->SetShortcutKey ( DIK_ESCAPE );
		m_pYES = CreateTextButton ( "MODAL_YES", MODAL_YES, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 2 ) );
		m_pYES->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
		m_pNO = CreateTextButton ( "MODAL_NO", MODAL_NO, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 3 ) );
		m_pNO->SetShortcutKey ( DIK_ESCAPE );

		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox ( "BASIC_MODAL_EDIT_BOX_BACK" );
		RegisterControl ( pBasicLineBox );
		m_pEditBoxBack = pBasicLineBox;

		CUIEditBox* pEditBox = new CUIEditBox;
		pEditBox->CreateSub ( this, "BASIC_MODAL_EDIT_BOX", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, MODAL_EDITBOX );
		pEditBox->CreateCarrat ( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput ( nLIMIT_CHAR );
		pEditBox->SetFont ( pFont );		
		RegisterControl ( pEditBox );
		m_pEditBox = pEditBox;	

		m_p1BUTTON = CreateControl ( "BASIC_MODAL_1BUTTON" );
		m_p2BUTTONLEFT = CreateControl ( "BASIC_MODAL_2BUTTON_LEFT" );
		m_p2BUTTONRIGHT = CreateControl ( "BASIC_MODAL_2BUTTON_RIGHT" );
	}
}

CBasicTextButton* CModalWindow::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE19;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl( pButton );

	return pButton;
}

void CModalWindow::DoModal ( const CString& strText, int nModalTitle, int nModalType, UIGUID CallerID )
{
	if ( nModalTitle < MODAL_INFOMATION || MODAL_NOTITLE < nModalTitle )
	{
		GASSERT ( 0 && "[심각] 모달 타이틀 인덱스가 잘못되었습니다." );
		return ;
	}

	if ( MODAL_NOTITLE == nModalTitle )
	{
		SetTitleName ( NULL );
	}
	else
	{
		SetTitleName ( (char*)ID2GAMEWORD ( "MODAL_TITLE", nModalTitle ) );
	}

	SetCallerID ( CallerID );

	m_pTextBox->SetText ( strText );

	m_nModalType = nModalType;	
	ReArrangeControl ( nModalType );
}

void CModalWindow::ReArrangeControl ( int nModalType )
{
	m_pOK->SetVisibleSingle ( FALSE );
	m_pCANCEL->SetVisibleSingle ( FALSE );
	m_pYES->SetVisibleSingle ( FALSE );
	m_pNO->SetVisibleSingle ( FALSE );
	m_pEditBox->SetVisibleSingle ( FALSE );
	m_pEditBoxBack->SetVisibleSingle ( FALSE );

	const UIRECT& rcParentPos = GetGlobalPos ();
	D3DXVECTOR2	vParent ( rcParentPos.left, rcParentPos.top );	

	switch ( nModalType )
	{
	case OKCANCEL:
		{
			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONLEFT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pOK->SetLocalPos ( vDummy );
				m_pOK->SetGlobalPos ( vPos );
				m_pOK->SetVisibleSingle ( TRUE );
			}

			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONRIGHT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pCANCEL->SetLocalPos ( vDummy );
				m_pCANCEL->SetGlobalPos ( vPos );
				m_pCANCEL->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case YESNO:
		{
			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONLEFT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pYES->SetLocalPos ( vDummy );
				m_pYES->SetGlobalPos ( vPos );
				m_pYES->SetVisibleSingle ( TRUE );
			}

			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONRIGHT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pNO->SetLocalPos ( vDummy );
				m_pNO->SetGlobalPos ( vPos );
				m_pNO->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case OK:
		{
			const UIRECT& rc2BUTTONPOS = m_p1BUTTON->GetLocalPos ();
			D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
			D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

			m_pOK->SetLocalPos ( vDummy );
			m_pOK->SetGlobalPos ( vPos );
			m_pOK->SetVisibleSingle ( TRUE );
		}
		break;

	case CANCEL:
		{
			const UIRECT& rc2BUTTONPOS = m_p1BUTTON->GetLocalPos ();
			D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
			D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

			m_pCANCEL->SetLocalPos ( vDummy );
			m_pCANCEL->SetGlobalPos ( vPos );
			m_pCANCEL->SetVisibleSingle ( TRUE );
		}
		break;

	case EDITBOX:
	case EDITBOX_PETNAME:
	case EDITBOX_NUMBER:
	case EDITBOX_MONEY:
	case MODAL_EDITBOX_LOTTERY:
	case MODAL_FILE_NAME:
		{
			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONLEFT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pOK->SetLocalPos ( vDummy );
				m_pOK->SetGlobalPos ( vPos );
				m_pOK->SetVisibleSingle ( TRUE );
			}

			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONRIGHT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pCANCEL->SetLocalPos ( vDummy );
				m_pCANCEL->SetGlobalPos ( vPos );
				m_pCANCEL->SetVisibleSingle ( TRUE );
			}

			m_pEditBoxBack->SetVisibleSingle ( TRUE );

			m_pEditBox->SET_MONEY_FORMAT ( 0 );
			m_pEditBox->SetAlign ( EDIT_LEFT_ALIGN );

			if( EDITBOX_PETNAME == nModalType )
			{
				m_pEditBox->SetLimitInput( 12 ); // 펫 이름은 한글 6자로 제한
			}
			else if ( EDITBOX_NUMBER == nModalType )
			{
				m_pEditBox->DoUSE_NUMBER ( true );
				m_pEditBox->SetLimitInput (	nLIMIT_NUMBER_CHAR );
				m_pEditBox->SetAlign ( EDIT_RIGHT_ALIGN );
			}
			else if ( EDITBOX_MONEY == nModalType )
			{
				m_pEditBox->SET_MONEY_FORMAT ( nMONEY_UNIT, strMONEY_SYMBOL );
				m_pEditBox->SetLimitInput ( nLIMIT_MONEY_CHAR );
				m_pEditBox->SetAlign ( EDIT_RIGHT_ALIGN );
			}
			else if ( nModalType == MODAL_EDITBOX_LOTTERY )
			{
				m_pEditBox->SetLimitInput ( 10 );
			}
			else if ( nModalType == MODAL_FILE_NAME )
			{
				m_pEditBox->SetLimitInput ( 20 );	//	 16 + 1 + 3
			}
			else
			{
				m_pEditBox->SetLimitInput ( nLIMIT_CHAR );
			}

			m_pEditBox->SetVisibleSingle ( TRUE );
			m_pEditBox->EndEdit ();

			m_pEditBox->ClearEdit ();
			m_pEditBox->BeginEdit ();
		}
		break;
	}
}

void CModalWindow::ResizeControl ( char* szWindowKeyword )
{
    CUIControl TempControl;
	TempControl.Create ( 1, szWindowKeyword );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

CUIControl*	CModalWindow::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void	CModalWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIModal::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case MODAL_YES:
	case MODAL_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( IsEditBoxType() ) // MEMO : 에디터 박스 타입의 모달이면...
				{
					if ( CHECK_KEYFOCUSED(dwMsg) && !DXInputString::GetInstance().CheckEnterKeyDown() )
						return;
				}

				switch( m_nModalType )
				{
				case MODAL_EDITBOX_LOTTERY:
					{
						if( m_pEditBox->GetEditLength() != 10 )
							return ;
					}
					break;

				case EDITBOX_NUMBER:	m_pEditBox->DoUSE_NUMBER( false );	break;
				case EDITBOX_MONEY:		m_pEditBox->SET_MONEY_FORMAT( 0 );	break;
				}

				m_pEditBox->EndEdit ();

				//	메시지
				//
				dwMsg &= ~UIMSG_KEY_FOCUSED;
				SetMessageEx ( dwMsg | UIMSG_MODAL_OK );

				if ( m_bTEST_MULTI )
				{
					CInnerInterface::GetInstance().CLOSE_MODAL ( GetWndID (), true );
					return ;
				}

				if ( m_bUseInner )
					CInnerInterface::GetInstance().HideGroup ( GetWndID (), true );
				else
					COuterInterface::GetInstance().HideGroup ( GetWndID (), true );				
			}
		}
		break;

	case MODAL_NO:
	case MODAL_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				switch( m_nModalType )
				{
				case EDITBOX_NUMBER:	m_pEditBox->DoUSE_NUMBER( false );	break;
				case EDITBOX_MONEY:		m_pEditBox->SET_MONEY_FORMAT( 0 );	break;
				}

				m_pEditBox->EndEdit ();

				//	메시지
				//
				dwMsg &= ~UIMSG_KEY_FOCUSED;
				SetMessageEx ( dwMsg | UIMSG_MODAL_CANCEL );
				// AddMessageEx ( UIMSG_MODAL_CANCEL );			

				if ( m_bTEST_MULTI )
				{
					CInnerInterface::GetInstance().CLOSE_MODAL ( GetWndID (), true );
					return ;
				}

				if ( m_bUseInner )
					CInnerInterface::GetInstance().HideGroup ( GetWndID (), true );
				else
					COuterInterface::GetInstance().HideGroup ( GetWndID (), true );
			}
		}
		break;
	}
}

void	CModalWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;
	
	CUIModal::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pEditBox->IsVisible() && m_pEditBox->IsBegin() )
	{
		static bool bVALID_INPUT_BACK = true;

		if ( bVALID_INPUT_BACK && !m_pEditBox->IsVALID_INPUT () )
		{
			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("INVALID_KEYBOARD_INPUT") );
		}
		bVALID_INPUT_BACK = m_pEditBox->IsVALID_INPUT ();
	}
}

void	CModalWindow::SetModalData ( int nData1, int nData2 )
{
	m_nData1 = nData1;
	m_nData2 = nData2;
}

void	CModalWindow::GetModalData ( int* pnData1, int* pnData2 )
{
	*pnData1 = m_nData1;
	*pnData2 = m_nData2;
}

bool	CModalWindow::IsEditBoxType()
{
	return ( EDITBOX <= m_nModalType && m_nModalType <= MODAL_FILE_NAME );
}

void DoModal ( const CString& strText, int nModalTitle, int nModalType, UIGUID CallerID, BOOL bHide )
{
	//	CInnerInterface::GetInstance().SetModalWindowOpen ();

	CModalWindow* pModalWindow = CInnerInterface::GetInstance().GetModalWindow ();
	if ( pModalWindow )
	{
		//		CInnerInterface::GetInstance().HideGroup ( MODAL_WINDOW, true );
		if ( CInnerInterface::GetInstance().IsVisibleGroup ( MODAL_WINDOW ) )
		{
			//			GASSERT ( 0 && "이미 켜져있습니다." );
			CInnerInterface::GetInstance().HideGroup ( MODAL_WINDOW, true );
		}

		pModalWindow->SetHide ( bHide );
		pModalWindow->SetUseInner ( true );
		pModalWindow->DoModal ( strText, nModalTitle, nModalType, CallerID );
		CInnerInterface::GetInstance().ShowGroupFocus ( MODAL_WINDOW, true );
	}
}

void DoModalOuter ( const CString& strText, int nModalTitle, int nModalType, UIGUID CallerID, BOOL bHide )
{
	UIGUID RenderStage = COuterInterface::GetInstance().GetRenderStage ();
	CModalWindow* pModalWindow = COuterInterface::GetInstance().GetModalWindow ();
	if ( pModalWindow )
	{
		COuterInterface::GetInstance().HideGroup ( MODAL_WINDOW_OUTER, true );

		pModalWindow->SetPrevPageID ( RenderStage );
		pModalWindow->SetHide ( bHide );
		pModalWindow->SetUseInner ( false );
		pModalWindow->DoModal ( strText, nModalTitle, nModalType, CallerID );
		COuterInterface::GetInstance().ShowGroupFocus ( MODAL_WINDOW_OUTER, true );
	}
}

void SetModalData ( int nData1, int nData2 )
{
	CModalWindow* pModalWindow = CInnerInterface::GetInstance().GetModalWindow ();
	if ( pModalWindow ) pModalWindow->SetModalData ( nData1, nData2 );
}