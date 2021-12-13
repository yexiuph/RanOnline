#include "pch.h"

#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "BasicTextBoxEx.h"
#include "UITextControl.h"
#include "BasicLineBox.h"
#include "OuterInterface.h"
#include "ModalWindow.h"
#include "s_NetClient.h"
#include "DxGlobalStage.h"
#include "DxInputString.h"
#include "DebugSet.h"
#include "SecPassCheckPage.h"

#include "../[Lib]__Engine/Sources/Common/StringUtils.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/UIEditBoxMan.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int	CSecPassCheckPage::nLIMIT_ID = 16;
int	CSecPassCheckPage::nLIMIT_PW = 16;

CSecPassCheckPage::CSecPassCheckPage ()
{
}

CSecPassCheckPage::~CSecPassCheckPage ()
{
}

void CSecPassCheckPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nBUTTONSIZE = CBasicTextButton::SIZE18;

	CBasicTextBox* pTextBox = NULL;
	pTextBox = CreateStaticControl ( "SECPASS_CHECKPAGE_ID", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SECPASS_CHECKPAGE_IDPW", 0 ) );
	pTextBox = CreateStaticControl ( "SECPASS_CHECKPAGE_PW", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SECPASS_CHECKPAGE_IDPW", 1 ) );

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "SECPASS_CHECKPAGE_ID_BACK" );
		RegisterControl ( pBasicLineBox );

		pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "SECPASS_CHECKPAGE_PW_BACK" );
		RegisterControl ( pBasicLineBox );

	}	

	{
		CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan;
		pEditBoxMan->CreateSub ( this, "CHECKSECPASS_EDIT_MAN", UI_FLAG_DEFAULT, CHECKSECPASS_EDIT_MAN );
        pEditBoxMan->CreateEditBox ( SECPASS_CHECKPAGE_EDIT_ID, "SECPASS_CHECKPAGE_EDIT_ID", "SECPASS_CHECKPAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_ID );
		pEditBoxMan->CreateEditBox ( SECPASS_CHECKPAGE_EDIT_PW, "SECPASS_CHECKPAGE_EDIT_PW", "SECPASS_CHECKPAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PW );		
		pEditBoxMan->SetHide ( SECPASS_CHECKPAGE_EDIT_PW, TRUE );
		RegisterControl( pEditBoxMan );
		m_pEditBoxMan = pEditBoxMan;
	}

	CBasicTextButton* pOKButton = new CBasicTextButton;
	pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, CHECKSECPASS_OK );
	pOKButton->CreateBaseButton ( "SECPASS_CHECKPAGE_OK", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SECPASS_CHECKPAGE_OKCANCEL", 0 ) );
	pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	RegisterControl ( pOKButton );

	CBasicTextButton* pQuitButton = new CBasicTextButton;
	pQuitButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, CHECKSECPASS_CANCEL );
	pQuitButton->CreateBaseButton ( "SECPASS_CHECKPAGE_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SECPASS_CHECKPAGE_OKCANCEL", 1 ) );
	pQuitButton->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( pQuitButton );

	CBasicTextButton* pReInputButton = new CBasicTextButton;
	pReInputButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, CHECKSECPASS_REINPUT );
	pReInputButton->CreateBaseButton ( "SECPASS_CHECKPAGE_REINPUT", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SECPASS_CHECKPAGE_OKCANCEL", 2 ) );
	RegisterControl ( pReInputButton );	

}	

void CSecPassCheckPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
	if ( !pNetClient->IsOnline() )		//네트워크 연결이 끊어진 경우
	{
		if ( !COuterInterface::GetInstance().IsVisibleGroup ( MODAL_WINDOW_OUTER ) )
		{
			if( !COuterInterface::GetInstance().IsLoginCancel() )
			{
				DoModalOuter ( ID2GAMEEXTEXT ("LOGINSTAGE_1"), MODAL_INFOMATION, OK, OUTER_MODAL_RECONNECT );
			}
		}
	}
	//	탭 이동
	if ( m_pEditBoxMan )
	{
		CUIControl* pParent = m_pEditBoxMan->GetTopParent ();
		if ( !pParent )	pParent = this;	//	만약 이클래스가 최상위 컨트롤인 경우
		BOOL bFocus = ( pParent->IsFocusControl() );

		if ( bFocus )
		{
			if ( UIKeyCheck::GetInstance()->Check ( DIK_TAB, DXKEY_DOWN ) )
			{
				m_pEditBoxMan->GoNextTab ();
			}
		}
	}
}

void CSecPassCheckPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case CHECKSECPASS_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제

				CString strID = m_pEditBoxMan->GetEditString ( SECPASS_CHECKPAGE_EDIT_ID );
				CString strPW = m_pEditBoxMan->GetEditString ( SECPASS_CHECKPAGE_EDIT_PW );

				if ( !strID.GetLength () || !strPW.GetLength () )  // 비번을 입력해주세요.
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "LOGINSTAGE_5" ) );  
					ResetAll();
					return ;
				}

				//if ( !CheckString ( strID ) )
				//{
				//	DoModalOuter ( ID2GAMEEXTEXT ( "LOGIN_PAGE_ID_ERROR" ) );
				//	ResetAll();
				//	return ;
				//}

				if ( !CheckString ( strPW ) )
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "LOGIN_PAGE_PW_ERROR" ) );
					ResetAll();
					return ;
				}

				if ( strPW.GetLength() < 8 || strPW.GetLength() > 16 )
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "SECPASS_LENGTH_ERROR" ) );  
					ResetAll();
					return ;
				}

//				if( strID != COuterInterface::GetInstance().GetDaumGID() )
//				{
//					DoModalOuter ( ID2GAMEEXTEXT ("LOGINSTAGE_11") );
//					ResetAll();
//					return ;
//				}

				//	컨트롤로 부터 아이디와 암호를 가져와 서버에 전송
				CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();

				int nServerGroup, nServerChannel;
				COuterInterface::GetInstance().GetConnectServerInfo( nServerGroup, nServerChannel );

#if defined(MY_PARAM) || defined(MYE_PARAM)
				pNetClient->TerraSndPassCheck( strID, strPW, 1 );
#elif defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM)
				pNetClient->DaumSndPassCheck( strID, strPW, 1 );
#elif defined ( JP_PARAM )	// 2차 패스워드
				pNetClient->ExciteSndPassCheck( strID, strPW, 1 );
#endif

				DoModalOuter ( ID2GAMEEXTEXT ("LOGINSTAGE_6"), MODAL_INFOMATION, CANCEL, OUTER_MODAL_WAITCONFIRM );
				COuterInterface::GetInstance().SetModalCallWindowID( GetWndID() );
			}
		}
		break;

	case CHECKSECPASS_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient();
				if( pNetClient->IsOnline() == true )
				{
					pNetClient->CloseConnect();
					COuterInterface::GetInstance().SetCancelToLogin();
				}

				COuterInterface::GetInstance().ToSelectServerPage ( GetWndID () );
			}
		}
		break;
	case CHECKSECPASS_REINPUT:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				COuterInterface::GetInstance().HideGroup( GetWndID() );
				COuterInterface::GetInstance().ShowGroupFocus( SECPASS_SETPAGE );
			}
		}
	}
}

BOOL CSecPassCheckPage::CheckString( CString strTemp )
{	
	strTemp = strTemp.Trim();

	// 문자열 체크 - 들어가면 안되는 특수문자 : ~!@#$%^&*+|":?><\=`',.;[]{}()
	if( STRUTIL::CheckString( strTemp ) )
	{
		return FALSE;
	}

    return TRUE;
}

void CSecPassCheckPage::ResetAll ()
{
	m_pEditBoxMan->EndEdit ();

	m_pEditBoxMan->ClearEdit ( SECPASS_CHECKPAGE_EDIT_ID );
	m_pEditBoxMan->ClearEdit ( SECPASS_CHECKPAGE_EDIT_PW );

	m_pEditBoxMan->Init();
	m_pEditBoxMan->BeginEdit();

	if( m_pEditBoxMan->IsMODE_NATIVE() )
	{
		m_pEditBoxMan->DoMODE_TOGGLE();
	}
}

void CSecPassCheckPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if ( bVisible )
	{
		if( GLCONST_CHAR::nUI_KEYBOARD == 2 )
		{
			m_pEditBoxMan->DisableKeyInput();
			DXInputString::GetInstance().DisableKeyInput();
		}

		ResetAll();
		
		COuterInterface::GetInstance().OpenMessageWindow( GetWndID(), bVisible );
		COuterInterface::GetInstance().ResetCancelToLogin();
	}
	else
	{
		m_pEditBoxMan->EndEdit ();

		if( GLCONST_CHAR::nUI_KEYBOARD == 2 )
		{
			m_pEditBoxMan->UsableKeyInput();
			DXInputString::GetInstance().UsableKeyInput();
		}

		COuterInterface::GetInstance().OpenMessageWindow( GetWndID(), bVisible );
	}
}

void CSecPassCheckPage::GoNextTab()
{
	if( m_pEditBoxMan )
		m_pEditBoxMan->GoNextTab();
}

void CSecPassCheckPage::SetCharToEditBox( TCHAR cKey )
{
	if( !m_pEditBoxMan ) return;
	
	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	strTemp += cKey;

	m_pEditBoxMan->SetEditString( nID, strTemp );
}

void CSecPassCheckPage::DelCharToEditBox()
{
	if( !m_pEditBoxMan ) return;

	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	INT nLenth = strTemp.GetLength();
	strTemp = strTemp.Left( nLenth - 1 );

	m_pEditBoxMan->SetEditString( nID, strTemp );
}