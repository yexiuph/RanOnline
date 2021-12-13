#include "pch.h"

#include "SecPassSetPage.h"
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

#include "../[Lib]__Engine/Sources/Common/StringUtils.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/UIEditBoxMan.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int	CSecPassSetPage::nLIMIT_ID = 16;
const int	CSecPassSetPage::nLIMIT_PW = 16;

#define IsNumber(ch)      ((ch) >= '0' && (ch) <= '9')


CSecPassSetPage::CSecPassSetPage ()
{
}

CSecPassSetPage::~CSecPassSetPage ()
{
}

void CSecPassSetPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nBUTTONSIZE = CBasicTextButton::SIZE18;

	CBasicTextBox* pTextBox = NULL;
	pTextBox = CreateStaticControl ( "SECPASS_SETPAGE_ID", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SECPASS_CHECKPAGE_IDPW", 0 ) );
	pTextBox = CreateStaticControl ( "SECPASS_SETPAGE_PW", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SECPASS_SETPAGE_PW", 0 ) );
	pTextBox = CreateStaticControl ( "SECPASS_SETPAGE_PWCHECK", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SECPASS_SETPAGE_PW", 1 ) );

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "SECPASS_SETPAGE_ID_BACK" );
		RegisterControl ( pBasicLineBox );

		pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "SECPASS_SETPAGE_PW_BACK" );
		RegisterControl ( pBasicLineBox );

		pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "SECPASS_SETPAGE_PWCHECK_BACK" );
		RegisterControl ( pBasicLineBox );
	}

	{
		CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan;
		pEditBoxMan->CreateSub ( this, "SETSECPASS_EDIT_MAN", UI_FLAG_DEFAULT, SETSECPASS_EDIT_MAN );
		pEditBoxMan->CreateEditBox ( SECPASS_SETPAGE_EDIT_ID, "SECPASS_SETPAGE_EDIT_ID", "SECPASS_SETPAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_ID );
		pEditBoxMan->CreateEditBox ( SECPASS_SETPAGE_EDIT_PW, "SECPASS_SETPAGE_EDIT_PW", "SECPASS_SETPAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PW );
		pEditBoxMan->CreateEditBox ( SECPASS_SETPAGE_EDIT_PWCHECK, "SECPASS_SETPAGE_EDIT_PWCHECK", "SECPASS_SETPAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PW );		
		pEditBoxMan->SetHide ( SECPASS_SETPAGE_EDIT_PW, TRUE );
		pEditBoxMan->SetHide ( SECPASS_SETPAGE_EDIT_PWCHECK, TRUE );
		RegisterControl( pEditBoxMan );
		m_pEditBoxMan = pEditBoxMan;
	}

	CBasicTextButton* pOKButton = new CBasicTextButton;
	pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, SETSECPASS_OK );
	pOKButton->CreateBaseButton ( "SECPASS_SETPAGE_OK", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SECPASS_SETPAGE_OKCANCEL", 0 ) );
	pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	RegisterControl ( pOKButton );

	CBasicTextButton* pQuitButton = new CBasicTextButton;
	pQuitButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, SETSECPASS_CANCEL );
	pQuitButton->CreateBaseButton ( "SECPASS_SETPAGE_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SECPASS_SETPAGE_OKCANCEL", 1 ) );
	pQuitButton->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( pQuitButton );

}	

void CSecPassSetPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
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
		if ( m_pEditBoxMan->IsMODE_NATIVE() ) m_pEditBoxMan->DoMODE_TOGGLE(); 


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

void CSecPassSetPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SETSECPASS_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
				
				CString strID = m_pEditBoxMan->GetEditString ( SECPASS_SETPAGE_EDIT_ID );
				CString strPW = m_pEditBoxMan->GetEditString ( SECPASS_SETPAGE_EDIT_PW );
				CString strPWCHECK = m_pEditBoxMan->GetEditString ( SECPASS_SETPAGE_EDIT_PWCHECK );
				
				if (  !strID.GetLength () || !strPW.GetLength () || !strPWCHECK.GetLength () )  // 비번을 입력해주세요.
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "LOGINSTAGE_5" ) );
					ResetAll();
					return ;
				}

				if ( strID.GetLength() < 4 || strID.GetLength() > 16 )
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "SECID_LENGTH_ERROR" ) );
					ResetAll();
					return ;
				}

				if ( strPW.GetLength() < 8 || strPW.GetLength() > 16 )
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "SECPASS_LENGTH_ERROR" ) );
					ResetAll();
					return ;
				}

				if( strPW != strPWCHECK ) // 비번 설정 불일치
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "SECPASS_NOTEQUAL_ERROR" ) );
					ResetAll();
					return ;
				}

				if ( !CheckString ( strID ) )
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "LOGIN_PAGE_ID_ERROR" ) );
					ResetAll();
					return ;
				}

				if ( !CheckString ( strPW ) )
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "LOGIN_PAGE_PW_ERROR" ) );
					ResetAll();
					return ;
				}

				if ( strID == strPW ) 
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "SECPASS_EQUAL_ERROR" ) );
					ResetAll();
					return ;
				}

				// ID PW 첫문자
				// 영문만 입력

				if ( IsNumber( strID.GetAt(0) ) || IsNumber( strPW.GetAt(0) ) )
				{
					DoModalOuter ( ID2GAMEEXTEXT ( "SECPASS_FIRST_NUMBER" ) );
					ResetAll();
					return ;
				}

				//	컨트롤로 부터 아이디와 암호를 가져와 서버에 전송
				CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();

				int nServerGroup, nServerChannel;
				COuterInterface::GetInstance().GetConnectServerInfo( nServerGroup, nServerChannel );

#if defined(MY_PARAM) || defined(MYE_PARAM)
				pNetClient->TerraSndPassCheck( COuterInterface::GetInstance().GetDaumGID(), strPW, 0 );
#elif defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM)
				pNetClient->DaumSndPassCheck( COuterInterface::GetInstance().GetDaumGID(), strPW, 0 );
#elif defined ( JP_PARAM )	// 2차 패스워드
				pNetClient->ExciteSndPassCheck( strID, strPW, 0 );
#endif

				DoModalOuter ( ID2GAMEEXTEXT ("LOGINSTAGE_6"), MODAL_INFOMATION, CANCEL, OUTER_MODAL_WAITCONFIRM );
				COuterInterface::GetInstance().SetModalCallWindowID( GetWndID() );
			}
		}
		break;

	case SETSECPASS_CANCEL:
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
	}
}

BOOL CSecPassSetPage::CheckString( CString strTemp )
{	
	strTemp = strTemp.Trim();

	// 문자열 체크 - 들어가면 안되는 특수문자 : ~!@#$%^&*+|":?><\=`',.;[]{}()
	if( STRUTIL::CheckString( strTemp ) )
	{
		return FALSE;
	}

    return TRUE;
}

void CSecPassSetPage::ResetAll ()
{
	m_pEditBoxMan->EndEdit ();

	m_pEditBoxMan->ClearEdit ( SECPASS_SETPAGE_EDIT_ID );
	m_pEditBoxMan->ClearEdit ( SECPASS_SETPAGE_EDIT_PW );
	m_pEditBoxMan->ClearEdit ( SECPASS_SETPAGE_EDIT_PWCHECK );

	m_pEditBoxMan->Init();
	m_pEditBoxMan->BeginEdit();

	if( m_pEditBoxMan->IsMODE_NATIVE() )
	{
		m_pEditBoxMan->DoMODE_TOGGLE();
	}
}

void CSecPassSetPage::SetVisibleSingle ( BOOL bVisible )
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


void CSecPassSetPage::GoNextTab()
{
	if( m_pEditBoxMan )
		m_pEditBoxMan->GoNextTab();
}

void CSecPassSetPage::SetCharToEditBox( TCHAR cKey )
{
	if( !m_pEditBoxMan ) return;
	
	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	strTemp += cKey;

	m_pEditBoxMan->SetEditString( nID, strTemp );
}

void CSecPassSetPage::DelCharToEditBox()
{
	if( !m_pEditBoxMan ) return;

	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	INT nLenth = strTemp.GetLength();
	strTemp = strTemp.Left( nLenth - 1 );

	m_pEditBoxMan->SetEditString( nID, strTemp );
}