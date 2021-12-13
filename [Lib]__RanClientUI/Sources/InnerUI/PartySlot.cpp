#include "pch.h"
#include "PartySlot.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "BasicTextButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPartySlot::CPartySlot () :
	m_pPlayerName ( NULL ),
	m_pQuitButton ( NULL ),
	m_pAuthorityButton( NULL ),
	m_pHP ( NULL ),
	m_pHPLineBack ( NULL ),
	m_pMapText ( NULL ),
	m_pMapLineBack ( NULL ),
	m_pPosText ( NULL ),
	m_bMaster ( FALSE )
{
	for ( int i = 0; i < GLCI_NUM_EX; i++ )
	{
		m_pClass[i] = NULL;
	}
}

CPartySlot::~CPartySlot ()
{
}

void CPartySlot::SetSameMap ( BOOL bSameMap )
{
	if ( m_pHP ) m_pHP->SetVisibleSingle ( bSameMap );
	if ( m_pHPLineBack ) m_pHPLineBack->SetVisibleSingle ( bSameMap );
	if ( m_pPosText ) m_pPosText->SetVisibleSingle ( bSameMap );

	if ( m_pMapText ) m_pMapText->SetVisibleSingle ( !bSameMap );
	if ( m_pMapLineBack ) m_pMapLineBack->SetVisibleSingle ( !bSameMap );
}

void CPartySlot::CreateSubControl ()
{
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	int nTextAlign = TEXT_ALIGN_LEFT;
	D3DCOLOR dwFontColor = NS_UITEXTCOLOR::DEFAULT;
	D3DCOLOR dwNameColor = NS_UITEXTCOLOR::PARTYNAME;

	{	//	Ŭ���� ǥ��
		CString strClassKeyword[GLCI_NUM_EX] = 
		{
			"PARTY_SLOT_CLASS_FIGHTER",
			"PARTY_SLOT_CLASS_ARMS",
			"PARTY_SLOT_CLASS_ARCHER",
			"PARTY_SLOT_CLASS_SPIRIT",
			"PARTY_SLOT_CLASS_EXTREME_M",
			"PARTY_SLOT_CLASS_EXTREME_W"
		};

		for ( int i = 0; i < GLCI_NUM_EX; i++ )
		{
			m_pClass[i] = CreateControl ( strClassKeyword[i].GetString() );
		}
	}

	{
		//	�� ǥ�� �ڽ�
		m_pHPLineBack = CreateControl ( "PARTY_SLOT_HP_BACK" );

		//	�� ǥ��
		CBasicProgressBar*	pProgressBar = new CBasicProgressBar;
		pProgressBar->CreateSub ( this, "PARTY_SLOT_HP", UI_FLAG_DEFAULT, PARTY_HP_BAR );
		pProgressBar->CreateOverImage ( "PARTY_SLOT_HP_OVERIMAGE" );
//		pProgressBar->SetPercent ( 0.3f );
		RegisterControl ( pProgressBar );
		m_pHP = pProgressBar;

		//	���� ǥ�� �ڽ�
		m_pMapLineBack = CreateControl ( "PARTY_SLOT_POS_BACK" );

		//	������ �ؽ�Ʈ
		nTextAlign = TEXT_ALIGN_CENTER_X;
		m_pMapText = CreateStaticControl ( "PARTY_SLOT_POS_TEXT", pFont8, dwFontColor, nTextAlign );
//		m_pMapText->SetOneLineText ( "�����п� ���� 1��" );
	}

	{
		nTextAlign = TEXT_ALIGN_LEFT;
		//	�÷��̾� �̸�
		m_pPlayerName = CreateStaticControl ( "PARTY_SLOT_PLAYER_NAME", pFont8, dwNameColor, nTextAlign );
//		m_pPlayerName->SetOneLineText ( "�����ٹݹݰ�����", NS_UITEXTCOLOR::ORANGE );

		//	�÷��̾� ������
		nTextAlign = TEXT_ALIGN_CENTER_X;
		m_pPosText = CreateStaticControl ( "PARTY_SLOT_PLAYER_POS", pFont8, dwFontColor, nTextAlign );
//		CString strPos;
//		strPos.Format ( "%d/%d", 500, -100 );
//		m_pPosText->SetOneLineText ( strPos );
	
		//	Ż�� ��ư
		m_pQuitButton = CreateTextButton ( "PARTY_SLOT_QUIT_BUTTON", PARTY_QUIT_BUTTON, (char*)ID2GAMEWORD ( "QUIT_PARTY", 0 ) );
		m_pAuthorityButton = CreateTextButton ( "PARTY_SLOT_AUTHORITY_BUTTON", PARTY_AUTH_BUTTON, (char*)ID2GAMEWORD ( "AUTHORITY_PARTY", 0 ) );
	}

	SetSameMap ( TRUE );
}


CBasicTextButton*	 CPartySlot::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );
	return pTextButton;
}

CUIControl*	CPartySlot::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextBox* CPartySlot::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CPartySlot::SetPlayerName ( CString strName )
{
	if ( m_pPlayerName )
	{
		m_pPlayerName->SetOneLineText ( strName, NS_UITEXTCOLOR::PALEGOLDENROD );
	}
}

void CPartySlot::SetPlayerPos ( CString strPos )
{
	if ( m_pPosText ) m_pPosText->SetOneLineText ( strPos );
}

void CPartySlot::SetHP ( float fPercent )
{
	if ( m_pHP ) m_pHP->SetPercent ( fPercent );
}

void CPartySlot::SetMapName ( CString strMapName )
{
	if ( m_pMapText ) m_pMapText->SetOneLineText ( strMapName );
}

void CPartySlot::SetClass ( int nClassType )
{
	if ( nClassType < GLCI_FIGHTER_M && GLCI_SPIRIT_M < nClassType )
	{
		GASSERT ( 0 && "Ŭ����Ÿ���� �ε��������� ��踦 ����ϴ�." );
		return ;
	}

	if ( nClassType >=  GLCI_NUM_EX ) nClassType = nClassType - GLCI_NUM_EX;

	for ( int i = 0; i < GLCI_NUM_EX; i++ )
	{
		m_pClass[i]->SetVisibleSingle ( FALSE );
	}

	m_pClass[nClassType]->SetVisibleSingle ( TRUE );
}

void CPartySlot::SetVisibleQuitButton ( BOOL bVisible )
{
	if ( m_pQuitButton ) m_pQuitButton->SetVisibleSingle ( bVisible );
}

void CPartySlot::SetVisibleAuthButton( BOOL bVisible )
{
	if ( m_pAuthorityButton ) m_pAuthorityButton->SetVisibleSingle ( bVisible );
}

void CPartySlot::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case PARTY_QUIT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_MOUSEIN_LBUP_EVENT );
			}
		}
		break;

	case PARTY_HP_BAR:
		{
			if ( CHECK_MOUSEIN_RBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_MOUSEIN_RBUP_EVENT );
			}
		}
		break;

	case PARTY_AUTH_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				AddMessageEx( UIMSG_MOUSEIN_LBUP_AUTH );
			}
		}
		break;
	}
}

void CPartySlot::SetMaster ( BOOL bMaster )
{
	m_bMaster = bMaster;

	if ( m_bMaster )
		m_pQuitButton->SetOneLineText ( (char*)ID2GAMEWORD ( "QUIT_PARTY", 0 ) );
	else
		m_pQuitButton->SetOneLineText ( (char*)ID2GAMEWORD ( "QUIT_PARTY", 1 ) );
}