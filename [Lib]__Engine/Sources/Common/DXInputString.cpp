//-----------------------------------------------------------------------
//
//		CLASS   DXInputString
//
//		Using IME -
//		Link : "imm32.lib"
//
//		coding : 2000/07/11 ~ 12
//		by jdh
//
//		- ���� : 2000/12/15 - ���ڿ� �Է½� Input ScanCode ��� ����.
//		- ���� : 2000/12/21 - TAB, ESCAPE, RETURN Ű �ν�.
//                          - TAB ���� ����.
//		- ���� : 2002/08/08 - �̱��� �������� ����.
//		- ���� : 2002/08/09 - OnChar() ����, ImmGetOpenStatus(m_hIMC)��
//							���� ���� ���� �Է� ���θ� �����ϰ� ����.
//
//-----------------------------------------------------------------------
#include "pch.h"
#include "./DXInputString.h"
#include "./IMEEdit.h"
#include "./DxFontMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL DXInputString::m_bCaratMove = FALSE;

DXInputString& DXInputString::GetInstance()
{
	static DXInputString Instance;
	return Instance;
}

DXInputString::DXInputString()
	: m_bOn( FALSE )
	, m_pParentWnd( NULL )
{
	m_bCaratMove = TRUE;
}

void DXInputString::Create ( CWnd *_pWnd, CRect &rtPosition )
{
	m_pParentWnd = _pWnd;
	m_editIME.Create ( NULL, rtPosition, m_pParentWnd, 1201 );
	m_editIME.SetFont ( 0, NULL );
}

void DXInputString::Move ( CRect &rtPosition )
{
	m_editIME.MoveWindow ( rtPosition.left, rtPosition.top, rtPosition.Width(), rtPosition.Height(), FALSE );
}

BOOL DXInputString::OnInput ()
{
	m_bOn = TRUE;
	m_editIME.SetFocus();

	return TRUE;
}

BOOL DXInputString::OffInput ()
{
	m_bOn = FALSE;
	m_pParentWnd->SetFocus();

	Move ( CRect(0,0,0,0) );

	return TRUE;
}

BOOL DXInputString::IsOn ()
{
	CWnd *pWnd = m_pParentWnd->GetFocus();
	if ( !pWnd )
	{
		OffInput ();
		return FALSE;
	}

	// InputString�� ��Ȱ��ȭ�� �� �θ� �����쿡 ������ ��Ŀ���� ��ȯ
	// WebBrowser������ ��Ŀ�� ��ȯ���� ���� ó���� ( ���� )
	#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(CH_PARAM) || defined(TH_PARAM) ||  defined ( TW_PARAM ) || defined ( HK_PARAM ) || defined(PH_PARAM) || defined(ID_PARAM) || defined (JP_PARAM) || defined(GS_PARAM) || defined(_RELEASED) // �ʸ��� �����ۼ�
		return m_bOn;
	#endif

	if ( pWnd != (CWnd*)(&m_editIME) )
	{
		OffInput ();
		return FALSE;
	}

	return m_bOn;
}