#include "pch.h"
#include "BasicComboBox.h"
#include "BasicLineBox.h"
#include "BasicLineBoxButton.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "UITextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicComboBox::CBasicComboBox ()
{
}

CBasicComboBox::~CBasicComboBox ()
{
}

void CBasicComboBox::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case BASIC_COMBOBOX_BUTTON_DOWN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					AddMessageEx ( UI_MSG_COMBOBOX_ROLLOVER );
				}
			}
		}
		break;
	}
}

void CBasicComboBox::CreateBaseComboBox ( char* szComboBoxControl )
{	
	CreateComboImage ();
	CreateTextBox ();

    CUIControl TempControl;
	TempControl.Create ( 1, szComboBoxControl );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );	
}

void CBasicComboBox::CreateComboImage ()
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox;
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_COMBO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCombo->CreateBaseBoxCombo ( "BASIC_COMBOBOX_BACK" );
	RegisterControl ( pLineBoxCombo );

	CBasicLineBoxButton* pLineBoxComboButton = new CBasicLineBoxButton;
	pLineBoxComboButton->CreateSub ( this, "BASIC_LINE_BOX_BUTTON", UI_FLAG_RIGHT, BASIC_COMBOBOX_BUTTON_DOWN  );
	pLineBoxComboButton->CreateBaseButton ( "BASIC_COMBOBOX_BUTTON_DOWN", "BASIC_COMBOBOX_BUTTON_DOWN_IMAGE", "BASIC_COMBOBOX_BUTTON_DOWN_IMAGE_F" );
	RegisterControl ( pLineBoxComboButton );
}

void CBasicComboBox::CreateTextBox ()
{
	//	폰트
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	텍스트 박스
	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "BASIC_COMBOBOX_TEXTBOX", UI_FLAG_XSIZE, BASIC_COMBOBOX_TEXTBOX );
	pTextBox->SetFont ( pFont8 );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;
}

void CBasicComboBox::SetText ( CString str )
{
	m_pTextBox->SetOneLineText ( str, NS_UITEXTCOLOR::WHITE );
}