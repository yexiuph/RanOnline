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
//		- 수정 : 2000/12/15 - 문자열 입력시 Input ScanCode 기능 제한.
//		- 수정 : 2000/12/21 - TAB, ESCAPE, RETURN 키 인식.
//                          - TAB 문자 무시.
//		- 수정 : 2002/08/08 - 싱글턴 페턴으로 수정.
//		- 수정 : 2002/08/09 - OnChar() 문제, ImmGetOpenStatus(m_hIMC)의
//							상태 값에 따라 입력 여부를 결정하게 수정.
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

	// InputString이 비활성화일 때 부모 윈도우에 강제로 포커스를 반환
	// WebBrowser때문에 포커스 반환없이 리턴 처리함 ( 준혁 )
	#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(CH_PARAM) || defined(TH_PARAM) ||  defined ( TW_PARAM ) || defined ( HK_PARAM ) || defined(PH_PARAM) || defined(ID_PARAM) || defined (JP_PARAM) || defined(GS_PARAM) || defined(_RELEASED) // 필리핀 아이템샵
		return m_bOn;
	#endif

	if ( pWnd != (CWnd*)(&m_editIME) )
	{
		OffInput ();
		return FALSE;
	}

	return m_bOn;
}