#include "pch.h"
#include "./SelectCharacterCard.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "BasicLineBox.h"
#include "OuterInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSelectCharacterCard::CSelectCharacterCard () :
	m_uCharRemain ( USHRT_MAX )
{
}

CSelectCharacterCard::~CSelectCharacterCard ()
{
}

void CSelectCharacterCard::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	좌측 이미지
	CreateControl ( "SELECT_CHARACTER_CARD_USABLE_IMAGE" );
//	CreateControl ( "SELECT_CHARACTER_CARD_USED_IMAGE" );

    //	우측 테두리
	{
		{
			CBasicLineBox* pBasicLineBox = new CBasicLineBox;
			pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pBasicLineBox->CreateBaseBoxOuter ( "SELECT_CHARACTER_CARD_USABLE_BACK" );
			RegisterControl ( pBasicLineBox );
		}

		{
			CBasicTextBox* pTextBox = new CBasicTextBox;
			pTextBox->CreateSub ( this, "SELECT_CHARACTER_CARD_USABLE_TEXTBOX_STATIC" );
			pTextBox->SetFont ( pFont );
			pTextBox->AddText ( ID2GAMEWORD("SELECT_CHARACTER_CARD_STATIC",0), NS_UITEXTCOLOR::PALEGOLDENROD );
			RegisterControl ( pTextBox );
		}

		{
			CBasicLineBox* pBasicLineBox = new CBasicLineBox;
			pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pBasicLineBox->CreateBaseBoxOuter ( "SELECT_CHARACTER_CARD_USABLE_TEXTBOX_BACK" );
			RegisterControl ( pBasicLineBox );
		}

		{
			CBasicTextBox* pTextBox = new CBasicTextBox;
			pTextBox->CreateSub ( this, "SELECT_CHARACTER_CARD_USABLE_TEXTBOX" );
			pTextBox->SetFont ( pFont );
			pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
			RegisterControl ( pTextBox );
			m_pUsableTextBox = pTextBox;
		}
	}
}

void CSelectCharacterCard::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIOuterWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	const USHORT uCharRemain = COuterInterface::GetInstance().GetCharRemain ();

	if ( m_uCharRemain != uCharRemain )
	{
		CString strTemp;
		strTemp.Format ( "%u", uCharRemain );
		m_pUsableTextBox->SetText ( strTemp, NS_UITEXTCOLOR::PALEGOLDENROD );

		m_uCharRemain = uCharRemain;
	}
}