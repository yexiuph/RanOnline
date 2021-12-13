#include "pch.h"
#include "MiniPartySlot.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "BasicLineBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMiniPartySlot::CMiniPartySlot () :
	m_pPlayerName ( NULL ),
	m_pHP ( NULL )
{
	//for ( int i = 0; i < GLCI_NUM; i++ )
	//{
	//	m_pClass[i] = NULL;
	//}

	memset( m_pNumber, 0, sizeof(m_pNumber) );
}

CMiniPartySlot::~CMiniPartySlot ()
{
}

void CMiniPartySlot::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	int nTextAlign = TEXT_ALIGN_CENTER_X;
	D3DCOLOR dwFontColor = NS_UITEXTCOLOR::DEFAULT;
	D3DCOLOR dwNameColor = NS_UITEXTCOLOR::PARTYNAME;


	CBasicLineBox* pLineBoxCombo = new CBasicLineBox;
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCombo->CreateBaseBoxMiniParty ( "MINIPARTY_SLOT_BACK" );
	RegisterControl ( pLineBoxCombo );

	//{	//	클래스 표시
	//	CString strClassKeyword[GLCI_NUM] = 
	//	{
	//		"MINIPARTY_SLOT_CLASS_FIGHTER",
	//		"MINIPARTY_SLOT_CLASS_ARMS",
	//		"MINIPARTY_SLOT_CLASS_ARCHER",
	//		"MINIPARTY_SLOT_CLASS_SPIRIT"
	//	};

	//	for ( int i = 0; i < GLCI_NUM; i++ )
	//	{
	//		m_pClass[i] = CreateControl ( strClassKeyword[i].GetString() );
	//	}
	//}

	{	//	번호 표시
		CString strNumberKeyword( "MINIPARTY_SLOT_NUMBER" );
		
		for ( int i = 0; i < MAXPARTY; ++i )
		{
			CString strKeyword;
			strKeyword.Format ( "%s%d", strNumberKeyword, i );

			m_pNumber[i] = CreateControl ( strKeyword.GetString () );
		}
	}

	{	//	피 표시
		CBasicProgressBar*	pProgressBar = new CBasicProgressBar;
		pProgressBar->CreateSub ( this, "MINIPARTY_SLOT_HP", UI_FLAG_DEFAULT, HP_BAR );
		pProgressBar->CreateOverImage ( "MINIPARTY_SLOT_HP_OVERIMAGE" );
		pProgressBar->SetPercent ( 0.3f );
		pProgressBar->SetVisibleSingle( FALSE );
		RegisterControl ( pProgressBar );
		m_pHP = pProgressBar;
	}

	//	플레이어 이름
	m_pPlayerName = CreateStaticControl ( "MINIPARTY_SLOT_PLAYER_NAME", pFont9, dwNameColor, nTextAlign );
}

CUIControl*	CMiniPartySlot::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextBox* CMiniPartySlot::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

//void CMiniPartySlot::SetClass ( int nClassType )
//{
//	if ( nClassType < GLCI_FIGHTER && GLCI_SPIRIT < nClassType )
//	{
//		GASSERT ( 0 && "클래스타입의 인덱스범위가 경계를 벗어납니다." );
//		return ;
//	}
//
//	for ( int i = 0; i < GLCI_NUM; i++ )
//	{
//		m_pClass[i]->SetVisibleSingle ( FALSE );
//	}
//
//	m_pClass[nClassType]->SetVisibleSingle ( TRUE );
//}

void CMiniPartySlot::SetNumber ( int nSlotNumber )
{
	if ( nSlotNumber < 0 && MAXPARTY <= nSlotNumber )
	{
		GASSERT ( 0 && "클래스타입의 인덱스범위가 경계를 벗어납니다." );
		return ;
	}

	for ( int i = 0; i < MAXPARTY; ++i )
	{
		m_pNumber[i]->SetVisibleSingle ( FALSE );
	}

	m_pNumber[nSlotNumber]->SetVisibleSingle ( TRUE );
}

void CMiniPartySlot::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case HP_BAR:
		{
			if ( CHECK_MOUSEIN_RBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_MOUSEIN_RB_EVENT );
			}
		}
		break;
	}
}