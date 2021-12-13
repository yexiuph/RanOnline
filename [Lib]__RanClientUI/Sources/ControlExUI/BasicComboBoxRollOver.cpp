#include "pch.h"
#include "BasicComboBoxRollOver.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "BasicLineBox.h"
#include "UITextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CBasicComboBoxRollOver::nSTARTLINE = 0;
const int CBasicComboBoxRollOver::nLIMITLINE = 100;
const int CBasicComboBoxRollOver::nNOT_SELECTED = -1;
const D3DCOLOR	CBasicComboBoxRollOver::dwMOUSEOVERCOLOR = D3DCOLOR_ARGB(0xFF,0x69,0x69,0x69);

CBasicComboBoxRollOver::CBasicComboBoxRollOver()
	: m_nSelectIndex ( nNOT_SELECTED )
	, m_pLineBox(NULL)
	, m_pTextBox(NULL)
	, m_pScrollBar(NULL)
{
}

CBasicComboBoxRollOver::~CBasicComboBoxRollOver ()
{
}

void CBasicComboBoxRollOver::CreateBaseComboBoxRollOver ( char* szComboBoxControl, bool bSelectInit /*= TRUE*/ )
{
	CreateComboBoxRollOverImage (bSelectInit);
	CreateScrollBar ();
	CreateSelectionImage ();

    CUIControl TempControl;
	TempControl.Create ( 1, szComboBoxControl );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CBasicComboBoxRollOver::CreateSelectionImage ()
{
}

void CBasicComboBoxRollOver::CreateComboBoxRollOverImage ( bool bSelectInit )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox;
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_COMBO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCombo->CreateBaseBoxCombo ( "BASIC_COMBOBOX_ROLLOVER_BACK" );
	RegisterControl ( pLineBoxCombo );

	//	�ǵ������� �������� ������ �ʰ� �մϴ�.
	pLineBoxCombo->SetUseRenderLine ( CBasicLineBox::TOP, FALSE );

	//	��Ʈ
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	�ؽ�Ʈ �ڽ�
	CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx;
	pTextBox->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, COMBOBOX_ROLLOVER_TEXT_BOX );
	pTextBox->CreateMouseOver ( dwMOUSEOVERCOLOR );
	pTextBox->SetFont ( pFont8 );
	pTextBox->SetSensitive ( true );
	pTextBox->SetLimitLine ( nLIMITLINE );	
	pTextBox->SetSelectInit( bSelectInit );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;
}

void CBasicComboBoxRollOver::CreateScrollBar ()
{
	//	��ũ�ѹ� �޸� ���� ����, ȸ�� ���
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox;
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_COMBO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCombo->CreateBaseBoxCombo ( "BASIC_COMBOBOX_ROLLOVER_SCROLLBAR_BOX" );
	RegisterControl ( pLineBoxCombo );
	m_pLineBox = pLineBoxCombo;

	//	�簢 �ڽ� ����� ���� ������� �������ϰ� �� ���İ� ������
	//	���ϴ� ũ���� �̹����Դϴ�. ��, 'COMBOBOX_ROLLOVER_SCROLLBAR'�� ������
	//	��ŭ�� ũ��� ��������. �׷���, �� ���Ŀ� ��ũ�ѹٰ� ��� ������¡�� �Ǵ���,
	//	XSIZE�� �������Ǿ�� �ȵǱ� ������ �� �÷��״� �� ���� ���Դϴ�.
	pLineBoxCombo->ResetAlignFlagLine ();
	pLineBoxCombo->SetAlignFlagLine ( CBasicLineBox::LEFT, UI_FLAG_YSIZE );
	pLineBoxCombo->SetAlignFlagLine ( CBasicLineBox::RIGHT,UI_FLAG_YSIZE );
	pLineBoxCombo->SetAlignFlagLine ( CBasicLineBox::BOTTOM, UI_FLAG_BOTTOM );
	pLineBoxCombo->SetAlignFlag ( UI_FLAG_YSIZE | UI_FLAG_RIGHT );

	int nTotalLine = m_pTextBox->GetVisibleLine();

	//	��ũ�ѹ�
	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
	pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, BASIC_COMBOBOX_ROLLOVER_SCROLLBAR );
	pScrollBar->CreateBaseScrollBar ( "BASIC_COMBOBOX_ROLLOVER_SCROLLBAR" );
	pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
	RegisterControl ( pScrollBar );
	m_pScrollBar = pScrollBar;
}

int CBasicComboBoxRollOver::AddText( CString str, bool bVisibleScroll /*= FALSE*/, bool bOneLine /*= FALSE*/ )
{
	int nRet;
	if( bOneLine )
		nRet = m_pTextBox->AddOneLineText( str, NS_UITEXTCOLOR::WHITE );
	else
		nRet = m_pTextBox->AddText( str, NS_UITEXTCOLOR::WHITE );

	int nTotal = m_pTextBox->GetTotalLine ();
	int nViewPerPage = m_pTextBox->GetVisibleLine();

	if ( nTotal <= nViewPerPage )
	{
		if( !bVisibleScroll ) 
		{
			m_pScrollBar->SetVisibleSingle ( FALSE );
			m_pLineBox->SetVisibleSingle ( FALSE );
		}else{
			m_pScrollBar->SetVisibleSingle ( TRUE );
			m_pLineBox->SetVisibleSingle ( TRUE );
		}
	}
	else
	{
		m_pScrollBar->SetVisibleSingle ( TRUE );
		m_pLineBox->SetVisibleSingle ( TRUE );

		m_pScrollBar->GetThumbFrame()->SetState ( nTotal, nViewPerPage );
	}

	return nRet;
}

void  CBasicComboBoxRollOver::ClearText ()
{
	m_pTextBox->ClearText ();

	m_pScrollBar->SetVisibleSingle ( FALSE );
	m_pLineBox->SetVisibleSingle ( FALSE );

	m_pScrollBar->GetThumbFrame()->SetState ( 1, 1 );
}

void CBasicComboBoxRollOver::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case BASIC_COMBOBOX_ROLLOVER_SCROLLBAR:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				AddMessageEx ( UI_MSG_COMBOBOX_ROLLOVER_SCROLL );
			}
		}
		break;

	case COMBOBOX_ROLLOVER_TEXT_BOX:
		{
			if ( CHECK_MOUSE_IN_LBDOWNLIKE ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_nSelectIndex = m_pTextBox->GetSelectPos ();
			}
		}
		break;
	}
}

void CBasicComboBoxRollOver::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_nSelectIndex = nNOT_SELECTED;
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( bFirstControl )
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();
		const int nTotalLine = m_pTextBox->GetTotalLine ();
		const int nLinePerOneView = m_pTextBox->GetVisibleLine();
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			if ( nPos < nSTARTLINE ) nPos = nSTARTLINE;
			m_pTextBox->SetCurLine ( nPos );			
		}
	}
}

void CBasicComboBoxRollOver::SetSelectIndex ( int nIndex )
{ 
	m_pTextBox->SetSelectPos( nIndex );
//	m_pTextBox->SetCurLine( nIndex ); 
	m_nSelectIndex = nIndex; 

	m_pTextBox->UpdateMouseOverImage();

}

CString	CBasicComboBoxRollOver::GetSelectText ( int nIndex )
{
	if ( !m_pTextBox )
	{
		GASSERT ( 0 && "m_pTextBox�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return CString();
	}

	return m_pTextBox->GetText ( nIndex );
}

void CBasicComboBoxRollOver::SetScrollPercent ( float fPercent )
{
	if ( !m_pScrollBar )
	{
		GASSERT ( 0 && "m_pScrollBar�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return;
	}

	m_pScrollBar->GetThumbFrame()->SetPercent ( fPercent );
}

void CBasicComboBoxRollOver::SetTextData( int nIndex, DWORD dwData )
{
	if ( !m_pTextBox )
	{
		GASSERT ( 0 && "m_pTextBox�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return;
	}

	m_pTextBox->SetTextData( nIndex, dwData );
}

DWORD CBasicComboBoxRollOver::GetTextData( int nIndex )
{
	if ( !m_pTextBox )
	{
		GASSERT ( 0 && "m_pTextBox�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return UINT_MAX;
	}

	return m_pTextBox->GetTextData( nIndex );
}

void CBasicComboBoxRollOver::SetVisibleThumb( bool bVisible )
{
	if ( !m_pScrollBar )
	{
		GASSERT ( 0 && "m_pScrollBar�� NULL�Դϴ�. CreateBaseComboBoxRollOver()�� ���� ȣ���Ͻʽÿ�." );
		return;
	}

	m_pScrollBar->SetVisibleThumb( bVisible );
}