#include "pch.h"
#include "ConftModalWindow.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "BasicLineBox.h"
#include "UITextControl.h"
#include "BasicComboBox.h"
#include "BasicComboBoxRollOver.h"
#include <string>
#include "InnerInterface.h"
#include "GLogicData.h"
#include "RANPARAM.h"
#include "ModalCallerID.h"

#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CConftModalWindow::CConftModalWindow () :
	m_CallerID ( NO_ID ),
	m_pOK ( NULL ),
	m_pCANCEL ( NULL ),
	m_pYES ( NULL ),
	m_pNO ( NULL ),
	m_pTextBox ( NULL ),
	m_RollOverID ( NO_ID ),
	m_bReqModal ( FALSE )
{
}

CConftModalWindow::~CConftModalWindow ()
{
}

void CConftModalWindow::CreateBaseModal ( char* szWindowKeyword )
{
	CreateTitle ( "MODAL_WINDOW_TITLE", "MODAL_WINDOW_TITLE_LEFT", "MODAL_WINDOW_TITLE_MID", "MODAL_WINDOW_TITLE_RIGHT", "MODAL_WINDOW_TEXTBOX", NULL );
	CreateBody ( "MODAL_WINDOW_BODY", "MODAL_WINDOW_BODY_LEFT", "MODAL_WINDOW_BODY_UP", "MODAL_WINDOW_BODY_MAIN", "MODAL_WINDOW_BODY_DOWN", "MODAL_WINDOW_BODY_RIGHT" );

	ResizeControl ( szWindowKeyword );
}

void CConftModalWindow::CreateSubControl ()
{
	//	텍스트 박스
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	WORD wAlignFlag = UI_FLAG_XSIZE | UI_FLAG_YSIZE;

	{	//	외부 라인 박스
		CBasicLineBox* pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MODAL", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxModal ( "CONFT_MODAL_LINEBOX" );		
		RegisterControl ( pLineBox );
	}

	//	위에 서술형 문장
	m_pTextBox = new CBasicTextBox;
	m_pTextBox->CreateSub ( this, "CONFT_MODAL_TEXT_BOX", wAlignFlag );
	m_pTextBox->SetFont ( pFont8 );
	m_pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );	
	RegisterControl ( m_pTextBox );

	//	버튼들
	m_pOK = CreateTextButton ( "CONFT_MODAL_OK", MODAL_CONFT_OK, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 0 ) );
	m_pOK->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	m_pCANCEL = CreateTextButton ( "CONFT_MODAL_CANCEL", MODAL_CONFT_CANCEL, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 1 ) );
	m_pCANCEL->SetShortcutKey ( DIK_ESCAPE );
	m_pYES = CreateTextButton ( "CONFT_MODAL_YES", MODAL_CONFT_YES, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 2 ) );
	m_pYES->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	m_pNO = CreateTextButton ( "CONFT_MODAL_NO", MODAL_CONFT_NO, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 3 ) );
	m_pNO->SetShortcutKey ( DIK_ESCAPE );		

	//	기본 텍스트

	CBasicTextBox* pTextBox = NULL;

	//	영역
	pTextBox = CreateStaticControl ( "CONFT_MODAL_RANGELIMIT_STATIC", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->AddText ( (char*)ID2GAMEWORD ( "CONFT_MODAL_CONDITION", 0 ) );

	//	회복제
	pTextBox = CreateStaticControl ( "CONFT_MODAL_USABLEHP_STATIC", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->AddText ( (char*)ID2GAMEWORD ( "CONFT_MODAL_CONDITION", 1 ) );

	//	체력
	pTextBox = CreateStaticControl ( "CONFT_MODAL_HPSTATE_STATIC_MY", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->AddText ( (char*)ID2GAMEWORD ( "CONFT_MODAL_CONDITION", 2 ) );

	//	체력
	pTextBox = CreateStaticControl ( "CONFT_MODAL_HPSTATE_STATIC_HIS", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	pTextBox->AddText ( (char*)ID2GAMEWORD ( "CONFT_MODAL_CONDITION", 2 ) );		

	m_pMyTextBox = CreateStaticControl ( "CONFT_MODAL_HPSTATE_TEXTBOX_MY", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMyTextBox->AddText ( "테스트" );
	m_pHisTextBox = CreateStaticControl ( "CONFT_MODAL_HPSTATE_TEXTBOX_HIS", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pHisTextBox->AddText ( "가나다라마바사" );

	//	기능
	m_pRangeLimitButton = CreateFlipButton ( "CONFT_MODAL_RANGELIMIT", "CONFT_MODAL_RANGELIMIT_F", CONFT_MODAL_RANGELIMIT_BUTTON );

	//	콤보박스
	m_pComboBoxUsableHPOpen = new CBasicComboBox;
	m_pComboBoxUsableHPOpen->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, CONFT_MODAL_USABLEHP_COMBO_OPEN );
	m_pComboBoxUsableHPOpen->CreateBaseComboBox ( "CONFT_MODAL_USABLEHP_COMBO_OPEN" );			
	RegisterControl ( m_pComboBoxUsableHPOpen );

	m_pComboBoxHPStateMyOpen = new CBasicComboBox;
	m_pComboBoxHPStateMyOpen->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, CONFT_MODAL_HPSTATEMY_COMBO_OPEN );
	m_pComboBoxHPStateMyOpen->CreateBaseComboBox ( "CONFT_MODAL_HPSTATE_MY_COMBO_OPEN" );			
	RegisterControl ( m_pComboBoxHPStateMyOpen );

	m_pComboBoxHPStateHisOpen = new CBasicComboBox;
	m_pComboBoxHPStateHisOpen->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, CONFT_MODAL_HPSTATEHIS_COMBO_OPEN );
	m_pComboBoxHPStateHisOpen->CreateBaseComboBox ( "CONFT_MODAL_HPSTATE_HIS_COMBO_OPEN" );			
	RegisterControl ( m_pComboBoxHPStateHisOpen );

	//	롤 오버 ( 언제나 가장 마지막에 뿌려져야합니다. )
	m_pComboBoxUsableHPRollOver = new CBasicComboBoxRollOver;
	m_pComboBoxUsableHPRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, CONFT_MODAL_USABLEHP_COMBO_ROLLOVER );
	m_pComboBoxUsableHPRollOver->CreateBaseComboBoxRollOver ( "CONFT_MODAL_USABLEHP_COMBO_ROLLOVER" );			
	m_pComboBoxUsableHPRollOver->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pComboBoxUsableHPRollOver );

	m_pComboBoxHPStateMyRollOver = new CBasicComboBoxRollOver;
	m_pComboBoxHPStateMyRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, CONFT_MODAL_HPSTATEMY_COMBO_ROLLOVER );
	m_pComboBoxHPStateMyRollOver->CreateBaseComboBoxRollOver ( "CONFT_MODAL_HPSTATE_MY_COMBO_ROLLOVER" );
	m_pComboBoxHPStateMyRollOver->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pComboBoxHPStateMyRollOver );

	m_pComboBoxHPStateHisRollOver = new CBasicComboBoxRollOver;
	m_pComboBoxHPStateHisRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, CONFT_MODAL_HPSTATEHIS_COMBO_ROLLOVER );
	m_pComboBoxHPStateHisRollOver->CreateBaseComboBoxRollOver ( "CONFT_MODAL_HPSTATE_HIS_COMBO_ROLLOVER" );
	m_pComboBoxHPStateHisRollOver->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pComboBoxHPStateHisRollOver );
}

CBasicTextButton* CConftModalWindow::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );

	return pButton;
}

void CConftModalWindow::DoModal ( CString strText, UIGUID CallerID, CString strMyName, CString strHisName )
{
	if ( (CallerID == MODAL_CONFLICT_ONE_TAR) ||
		 (CallerID == MODAL_CONFLICT_PARTY_TAR) ||
		 (CallerID == MODAL_CONFLICT_GUILD_TAR ) )
		m_bReqModal = FALSE;
	else
		m_bReqModal = TRUE;

	SetCallerID ( CallerID );

	//	내용 설정
	if ( m_pTextBox )
	{
		m_pTextBox->SetText ( strText );
	}

	//	이름 설정
	if ( m_pMyTextBox )
	{
		m_pMyTextBox->SetText ( strMyName );
	}

	if ( m_pHisTextBox )
	{
		m_pHisTextBox->SetText ( strHisName );
	}

	if ( m_bReqModal )
	{
		SetTitleName ( (char*)ID2GAMEWORD ( "CONFT_MODAL_TITLE", 0 ) );

		m_pOK->SetVisibleSingle ( TRUE );
		m_pCANCEL->SetVisibleSingle ( TRUE );
		m_pYES->SetVisibleSingle ( FALSE );
		m_pNO->SetVisibleSingle ( FALSE );	
	}
	else
	{
		SetTitleName ( (char*)ID2GAMEWORD ( "CONFT_MODAL_TITLE", 1 ) );

		m_pOK->SetVisibleSingle ( FALSE );
		m_pCANCEL->SetVisibleSingle ( FALSE );
		m_pYES->SetVisibleSingle ( TRUE );
		m_pNO->SetVisibleSingle ( TRUE );
	}

	LoadDefaultCondition ();
}

void CConftModalWindow::ResizeControl ( char* szWindowKeyword )
{
    CUIControl TempControl;
	TempControl.Create ( 1, szWindowKeyword );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}


CUIControl*	CConftModalWindow::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

void DoConftModal ( CString strText, UIGUID CallerID, CString strMyName, CString strHisName )
{
	CConftModalWindow* pConftModalWindow = CInnerInterface::GetInstance().GetConftModalWindow ();
	if ( pConftModalWindow )
	{
		pConftModalWindow->DoModal ( strText, CallerID, strMyName, strHisName );
		CInnerInterface::GetInstance().ShowGroupFocus ( CONFT_MODAL_WINDOW );
	}
}

CBasicTextBox* CConftModalWindow::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CBasicButton* CConftModalWindow::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );

	return pButton;
}

void	CConftModalWindow::LoadComboData ()
{
	CString strTemp;
	{	
		m_pComboBoxUsableHPRollOver->ClearText ();
		for ( int i = 0; i < GLCONST_CHAR::EMCONFT_RC_TYPENUM; i++ )
		{
			if ( USHRT_MAX <= GLCONST_CHAR::wCONFT_RC_TYPE[i] )
			{
				strTemp = ID2GAMEWORD ( "UNLIMITED" );
			}
			else
			{
				strTemp.Format ( "%d", GLCONST_CHAR::wCONFT_RC_TYPE[i] );
			}
			
			m_pComboBoxUsableHPRollOver->AddText ( strTemp );
		}
	}

	{	
		m_pComboBoxHPStateMyRollOver->ClearText ();
		for ( int i = 0; i < GLCONST_CHAR::EMCONFT_STATE_TYPENUM; i++ )
		{
			strTemp.Format ( "%1.1f", GLCONST_CHAR::fCONFT_STATE_TYPE[i] );
			m_pComboBoxHPStateMyRollOver->AddText ( strTemp );
		}

		m_pComboBoxHPStateHisRollOver->ClearText ();
		for ( int i = 0; i < GLCONST_CHAR::EMCONFT_STATE_TYPENUM; i++ )
		{
			strTemp.Format ( "%1.1f", GLCONST_CHAR::fCONFT_STATE_TYPE[i] );
			m_pComboBoxHPStateHisRollOver->AddText ( strTemp );
		}
	}
}

void	CConftModalWindow::LoadDefaultCondition ()
{
	LoadComboData ();

	LoadUsableHP ();
	LoadHPStateMy ();
	LoadHPStateHis ();
	LoadRangeLimit ();
}

void CConftModalWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( m_RollOverID == NO_ID )
	{
		CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	}
	else
	{
		ResetMessageEx ();

		CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		CUIControl* pControl = m_ControlContainer.FindControl ( m_RollOverID );
		if ( !pControl )
		{
			GASSERT ( 0 && "심각한 오류, 노드가 널입니다." );
			return ;
		}

		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		DWORD dwControlMsg = pControl->GetMessageEx ();
		NS_UIDEBUGSET::BlockBegin ();
		if ( dwControlMsg ) TranslateUIMessage ( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd ();

		//	스크롤바에 관계된 메시지가 아니고		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = GetMessageEx ();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bFirstLBUP )
				{
					m_RollOverID = NO_ID;
					pControl->SetVisibleSingle ( FALSE );					
				}
				m_bFirstLBUP = FALSE;
			}
		}
	}

	//CDebugSet::ToView ( 1, 18, "%d", m_RollOverID );
}

void	CConftModalWindow::LoadUsableHP ()
{
	CString strTemp;

	WORD wConftRecover = 0;

	if ( m_bReqModal )
	{		
		bool bOK = false;

		for ( int i = 0; i < GLCONST_CHAR::EMCONFT_RC_TYPENUM; ++i )
		{
			if ( RANPARAM::wCONFT_RECOVER == GLCONST_CHAR::wCONFT_RC_TYPE[i] )
			{
				bOK = true; 
				wConftRecover = RANPARAM::wCONFT_RECOVER;		
				break;
			}

		}

		if ( !bOK )
		{
			RANPARAM::wCONFT_RECOVER = GLCONST_CHAR::wCONFT_RC_TYPE[0];
			wConftRecover = RANPARAM::wCONFT_RECOVER;
		}
		
	}
	else
	{		
		SCONFT_OPTION* pConftOption = CInnerInterface::GetInstance().GetConftOption ();
		wConftRecover = pConftOption->wRECOVER;	
	}

	if ( USHRT_MAX <= wConftRecover )
	{
		strTemp = ID2GAMEWORD ( "UNLIMITED" );
	}
	else
	{
		strTemp.Format ( "%d", wConftRecover );
	}

	m_pComboBoxUsableHPOpen->SetText ( strTemp );
}

void	CConftModalWindow::LoadHPStateMy ()
{
	CString strTemp;

	if ( m_bReqModal )
	{		

		bool bOK = false;

		for ( int i = 0; i < GLCONST_CHAR::EMCONFT_STATE_TYPENUM; ++ i ) 
		{
			if ( RANPARAM::fCONFT_HP_RATE == GLCONST_CHAR::fCONFT_STATE_TYPE[i] )
			{
				bOK = true;
				break;
			}
		}

		if ( !bOK ) RANPARAM::fCONFT_HP_RATE = GLCONST_CHAR::fCONFT_STATE_TYPE[0];
		
		strTemp.Format ( "%1.1f", RANPARAM::fCONFT_HP_RATE );

	}
	else
	{		
		SCONFT_OPTION* pConftOption = CInnerInterface::GetInstance().GetConftOption ();
		strTemp.Format ( "%1.1f", pConftOption->fHP_RATE );	
	}

	m_pComboBoxHPStateMyOpen->SetText ( strTemp );
}

void	CConftModalWindow::LoadHPStateHis ()
{
	CString strTemp;

	if ( m_bReqModal )
	{	

		bool bOK = false;

		for ( int i = 0; i < GLCONST_CHAR::EMCONFT_STATE_TYPENUM; ++ i ) 
		{
			if ( RANPARAM::fCONFT_TAR_HP_RATE == GLCONST_CHAR::fCONFT_STATE_TYPE[i] )
			{
				bOK = true;
				break;
			}
		}

		if ( !bOK ) RANPARAM::fCONFT_TAR_HP_RATE = GLCONST_CHAR::fCONFT_STATE_TYPE[0];
		
		strTemp.Format ( "%1.1f", RANPARAM::fCONFT_TAR_HP_RATE );
	}
	else
	{		
		SCONFT_OPTION* pConftOption = CInnerInterface::GetInstance().GetConftOption ();
		strTemp.Format ( "%1.1f", pConftOption->fTAR_HP_RATE );	
	}

	m_pComboBoxHPStateHisOpen->SetText ( strTemp );
}

void CConftModalWindow::LoadRangeLimit ()
{	
	if ( m_bReqModal )
	{
		//	NOTE
		//		대련 영역 무조건 ON
		//		의도된 코드입니다.
		RANPARAM::bCONFT_BOUND = TRUE;
		m_pRangeLimitButton->SetFlip ( RANPARAM::bCONFT_BOUND );
	}
	else
	{
		SCONFT_OPTION* pConftOption = CInnerInterface::GetInstance().GetConftOption ();
		m_pRangeLimitButton->SetFlip ( pConftOption->bBOUND );		
	}
}