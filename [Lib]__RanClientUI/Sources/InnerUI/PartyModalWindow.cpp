#include "pch.h"
#include "./PartyModalWindow.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"

#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "BasicLineBox.h"
#include "UITextControl.h"
#include "BasicComboBox.h"
#include "BasicComboBoxRollOver.h"
#include "InnerInterface.h"
#include "GLogicData.h"
#include "RANPARAM.h"
#include "ModalCallerID.h"
#include "GLCharDefine.h"
#include "GLPartyClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPartyModalWindow::CPartyModalWindow () :
	m_CallerID ( NO_ID ),
	m_pOK ( NULL ),
	m_pCANCEL ( NULL ),
	m_pYES ( NULL ),
	m_pNO ( NULL ),
	m_pTextBox ( NULL ),
	m_RollOverID ( NO_ID ),
	m_bReqModal ( FALSE ),
	m_bFIX ( FALSE )
{
}

CPartyModalWindow::~CPartyModalWindow ()
{
}

void CPartyModalWindow::CreateBaseModal ( char* szWindowKeyword )
{
	CreateTitle ( "MODAL_WINDOW_TITLE", "MODAL_WINDOW_TITLE_LEFT", "MODAL_WINDOW_TITLE_MID", "MODAL_WINDOW_TITLE_RIGHT", "MODAL_WINDOW_TEXTBOX", NULL );
	CreateBody ( "MODAL_WINDOW_BODY", "MODAL_WINDOW_BODY_LEFT", "MODAL_WINDOW_BODY_UP", "MODAL_WINDOW_BODY_MAIN", "MODAL_WINDOW_BODY_DOWN", "MODAL_WINDOW_BODY_RIGHT" );

	ResizeControl ( szWindowKeyword );
}

void CPartyModalWindow::CreateSubControl ()
{
	//	�ؽ�Ʈ �ڽ�
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	WORD wAlignFlag = UI_FLAG_XSIZE | UI_FLAG_YSIZE;

	{	//	�ܺ� ���� �ڽ�
		CBasicLineBox* pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MODAL", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxModal ( "PARTY_MODAL_LINEBOX" );		
		RegisterControl ( pLineBox );
	}

	CBasicTextBox* pTextBox = NULL;

	{	//	���� ������ ����
		pTextBox = new CBasicTextBox;
		pTextBox->CreateSub ( this, "PARTY_MODAL_TEXT_BOX", wAlignFlag );
		pTextBox->SetFont ( pFont8 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );	
		RegisterControl ( pTextBox );
		m_pTextBox = pTextBox;
	}

	{	//	��ư��
		m_pOK = CreateTextButton ( "PARTY_MODAL_OK", MODAL_PARTY_OK, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 0 ) );
		m_pOK->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
		m_pCANCEL = CreateTextButton ( "PARTY_MODAL_CANCEL", MODAL_PARTY_CANCEL, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 1 ) );
		m_pCANCEL->SetShortcutKey ( DIK_ESCAPE );
		m_pYES = CreateTextButton ( "PARTY_MODAL_YES", MODAL_PARTY_YES, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 2 ) );
		m_pYES->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
		m_pNO = CreateTextButton ( "PARTY_MODAL_NO", MODAL_PARTY_NO, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 3 ) );
		m_pNO->SetShortcutKey ( DIK_ESCAPE );		
	}

	D3DCOLOR dwColor = NS_UITEXTCOLOR::DEFAULT;
	int nAlign = TEXT_ALIGN_LEFT;

	{	//	�⺻ �ؽ�Ʈ
		//	����
		pTextBox = CreateStaticControl ( "PARTY_MODAL_ITEM_STATIC", pFont8, dwColor, nAlign );
		pTextBox->AddText ( (char*)ID2GAMEWORD ( "PARTY_MODAL_CONDITION", 0 ) );

		//	ȸ����
		pTextBox = CreateStaticControl ( "PARTY_MODAL_MONEY_STATIC", pFont8, dwColor, nAlign );
		pTextBox->AddText ( (char*)ID2GAMEWORD ( "PARTY_MODAL_CONDITION", 1 ) );
	}

	{	//	���
		{
			CBasicComboBox* pComboBox = NULL;
			CBasicComboBoxRollOver*	pComboBoxRollOver = NULL;

			//	�޺��ڽ�
			{
				pComboBox = new CBasicComboBox;
				pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, PARTY_MODAL_ITEM_COMBO_OPEN );
				pComboBox->CreateBaseComboBox ( "PARTY_MODAL_ITEM_COMBO_OPEN" );			
				RegisterControl ( pComboBox );
				m_pComboBoxItemOpen = pComboBox;

				pComboBox = new CBasicComboBox;
				pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, PARTY_MODAL_MONEY_COMBO_OPEN );
				pComboBox->CreateBaseComboBox ( "PARTY_MODAL_MONEY_COMBO_OPEN" );			
				RegisterControl ( pComboBox );
				m_pComboBoxMoneyOpen = pComboBox;
			}

			//	�� ���� ( ������ ���� �������� �ѷ������մϴ�. )
			{
				pComboBoxRollOver = new CBasicComboBoxRollOver;
				pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, PARTY_MODAL_ITEM_COMBO_ROLLOVER );
				pComboBoxRollOver->CreateBaseComboBoxRollOver ( "PARTY_MODAL_ITEM_COMBO_ROLLOVER" );			
				pComboBoxRollOver->SetVisibleSingle ( FALSE );
				RegisterControl ( pComboBoxRollOver );
				m_pComboBoxItemRollOver = pComboBoxRollOver;

				pComboBoxRollOver = new CBasicComboBoxRollOver;
				pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, PARTY_MODAL_MONEY_COMBO_ROLLOVER );
				pComboBoxRollOver->CreateBaseComboBoxRollOver ( "PARTY_MODAL_MONEY_COMBO_ROLLOVER" );
				pComboBoxRollOver->SetVisibleSingle ( FALSE );
				RegisterControl ( pComboBoxRollOver );
				m_pComboBoxMoneyRollOver = pComboBoxRollOver;
			}
		}
	}
}

CBasicTextButton* CPartyModalWindow::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );

	return pButton;
}

void CPartyModalWindow::DoModal ( CString strText, UIGUID CallerID )
{
	if ( CallerID == MODAL_PARTY_TAR )
	{
		m_bReqModal = FALSE;		
	}
	else
	{
		m_bReqModal = TRUE;
	}

	//	NOTE
	//		�̹� ��Ƽ�� �����Ǿ� �ְų�,
	//		��Ƽ ��û�� ���� ��쿡 ���� ����� ��ﶧ��
	//		����Ÿ ������ �Ұ��ϴ�.
	m_bFIX = FALSE;
	if ( GLPartyClient::GetInstance().GetMaster() || !m_bReqModal )
	{
		m_bFIX = TRUE;
	}

	SetCallerID ( CallerID );

	//	���� ����
	if ( m_pTextBox )
	{
		m_pTextBox->SetText ( strText );
	}

	if ( m_bReqModal )
	{
		SetTitleName ( (char*)ID2GAMEWORD ( "PARTY_MODAL_TITLE", 0 ) );

		m_pOK->SetVisibleSingle ( TRUE );
		m_pCANCEL->SetVisibleSingle ( TRUE );
		m_pYES->SetVisibleSingle ( FALSE );
		m_pNO->SetVisibleSingle ( FALSE );	
	}
	else
	{
		SetTitleName ( (char*)ID2GAMEWORD ( "PARTY_MODAL_TITLE", 1 ) );

		m_pOK->SetVisibleSingle ( FALSE );
		m_pCANCEL->SetVisibleSingle ( FALSE );
		m_pYES->SetVisibleSingle ( TRUE );
		m_pNO->SetVisibleSingle ( TRUE );
	}

	LoadDefaultCondition ();
}

void CPartyModalWindow::ResizeControl ( char* szWindowKeyword )
{
    CUIControl TempControl;
	TempControl.Create ( 1, szWindowKeyword );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}


CUIControl*	CPartyModalWindow::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

void DoPartyModal ( CString strText, UIGUID CallerID )
{
	CPartyModalWindow* pPartyModalWindow = CInnerInterface::GetInstance().GetPartyModalWindow ();
	if ( pPartyModalWindow )
	{
		pPartyModalWindow->DoModal ( strText, CallerID );
		CInnerInterface::GetInstance().ShowGroupFocus ( PARTY_MODAL_WINDOW );
	}
}

CBasicTextBox* CPartyModalWindow::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CBasicButton* CPartyModalWindow::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );

	return pButton;
}

void	CPartyModalWindow::LoadComboData ()
{
	CString strTemp;
	{
		m_pComboBoxItemRollOver->ClearText ();
		for ( int i = 1; i < EMPTYITEMOPT_SIZE; i++ )
		{
			m_pComboBoxItemRollOver->AddText( (char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", i) );
		}

		m_pComboBoxMoneyRollOver->ClearText ();
		for ( int i = 1; i < EMPTYMONEYOPT_SIZE; i++ )
		{
			m_pComboBoxMoneyRollOver->AddText( (char*)ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", i) );
		}
	}
}

void	CPartyModalWindow::LoadDefaultCondition ()
{
	LoadComboData ();

	LoadItem ();
	LoadMoney ();
}

void CPartyModalWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
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
			GASSERT ( 0 && "�ɰ��� ����, ��尡 ���Դϴ�." );
			return ;
		}

		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		DWORD dwControlMsg = pControl->GetMessageEx ();
		NS_UIDEBUGSET::BlockBegin ();
		if ( dwControlMsg ) TranslateUIMessage ( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd ();

		//	��ũ�ѹٿ� ����� �޽����� �ƴϰ�		
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

void	CPartyModalWindow::LoadItem ()
{
	CString strTemp;

	if ( m_bReqModal )
	{		
		if( RANPARAM::dwPARTY_GET_ITEM == 0 || RANPARAM::dwPARTY_GET_ITEM >= EMPTYITEMOPT_SIZE )
		{
			RANPARAM::dwPARTY_GET_ITEM = 3;
		}

		m_pComboBoxItemOpen->SetText ( (char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", RANPARAM::dwPARTY_GET_ITEM) );
	}
	else
	{		
		SPARTY_OPT* pPartyOption = CInnerInterface::GetInstance().GetPartyOption ();
		m_pComboBoxItemOpen->SetText ( (char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", pPartyOption->emGET_ITEM) );
	}
}

void	CPartyModalWindow::LoadMoney ()
{
	CString strTemp;

	if ( m_bReqModal )
	{	
		if( RANPARAM::dwPARTY_GET_MONEY == 0 || RANPARAM::dwPARTY_GET_MONEY >= EMPTYMONEYOPT_SIZE )
		{
			RANPARAM::dwPARTY_GET_MONEY = 3;
		}

		m_pComboBoxMoneyOpen->SetText ( (char*)ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", RANPARAM::dwPARTY_GET_MONEY) );
	}
	else
	{		
		SPARTY_OPT* pPartyOption = CInnerInterface::GetInstance().GetPartyOption ();
		m_pComboBoxMoneyOpen->SetText ( (char*)ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", pPartyOption->emGET_MONEY) );
	}
}