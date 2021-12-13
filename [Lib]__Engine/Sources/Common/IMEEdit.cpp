/******************************************************************************\
*	IMEEdit.cpp : 구현 파일입니다.
*
*	다음은 Microsoft 소스 코드 샘플의 일부입니다. 
*	Copyright (c) Microsoft Corporation.
*	All rights reserved. 
*
*	이 소스 코드는 Microsoft 개발 도구 및/또는 
*	WinHelp 설명서에 대해 추가적으로 제공되는 내용입니다.
*	Microsoft 샘플 프로그램에 대한 자세한 내용은 
*	해당 개발 도구나 설명서를 참조하십시오.
\******************************************************************************/

#include "pch.h"
#include "./IMEEdit.h"
#include "./DxInputString.h"
#include "./DxFontMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CIMEEdit

CIMEEdit::CIMEEdit()
{
	OSVERSIONINFO osVI;

	// 버퍼 위치를 초기화합니다.
	m_xEndPos = m_xInsertPos = m_xCaretPos = 0;

	// 버퍼를 초기화합니다.
	SecureZeroMemory( m_szBuffer, sizeof(m_szBuffer) );
	SecureZeroMemory( m_szBufferMbs, sizeof(m_szBufferMbs) );
	SecureZeroMemory( m_szBackup, sizeof(m_szBackup) );

	// 멤버 변수를 초기화합니다.
	m_bEnterKeyDown = false;		// 입력 종료 여부 판단에 사용
	m_bKeyUse = TRUE;
	m_bWideCaret = false;

	m_fStat = FALSE;				// 점선 밑줄을 표시하지 않습니다.
	m_fShowInvert = FALSE;			// 현재 컴퍼지션 문자열을 변환하지 않습니다.
	m_nComSize = 0;					// 컴퍼지션 문자열 크기(바이트)에 대한 변수를 지웁니다.
	m_nCharSet = DEFAULT_CHARSET;		
	m_hFont = NULL;		
	m_fIsNT = FALSE;
	m_fIsXP = FALSE;

	// 시스템 버전을 확인합니다. 
	SecureZeroMemory( &osVI, sizeof( OSVERSIONINFO ) );	// OSVERSIONINFO 구조를 초기화합니다.
	osVI.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );    

	if (GetVersionEx( &osVI ))		// OS 버전 정보를 가져옵니다.
	{
		if( osVI.dwPlatformId == VER_PLATFORM_WIN32_NT )
		{
			m_fIsNT = TRUE;

			// 부 버전을 확인합니다.
			if(osVI.dwMinorVersion) 
				m_fIsXP = TRUE;		// Windows XP 이상인 경우
		}
	}
	else	// GetVersionEx 함수가 실패하면 다른 함수를 사용하여 다시 시도합니다.
	{
		if (GetVersion() < 0x80000000)
			m_fIsNT = TRUE;
	}
}

CIMEEdit::~CIMEEdit()
{
	if (m_hFont)
		DeleteObject(m_hFont);
}

const char* CIMEEdit::GetString ()
{
	SecureZeroMemory( m_szBufferMbs, sizeof(m_szBufferMbs) );
	WideCharToMultiByte ( m_nCodePage, NULL, m_szBuffer, -1, m_szBufferMbs, BUFFERSIZE*2+1, NULL, NULL );

	return m_szBufferMbs;

	/*
	WCHAR		wChar;
	INT			nXStart, nXEnd;
	wchar_t		szTempStr[BUFFERSIZE+1];
	CStringW	strDes, strSour( m_szBuffer );

	// Draw dotted line under composition string
	if ((m_fStat) && (m_dwProperty & IME_PROP_AT_CARET) && (m_nLanguage != KOREAN) )
	{
		// Calculate start position of composition string and move pen to start position
		memcpy(szTempStr, m_szBuffer, WCHARSIZE * m_xInsertPos);
		szTempStr[m_xInsertPos] = L'\0';

		// Calculate end position of composition string and draw dotted line
		m_szComStr[m_nComSize / WCHARSIZE] = L'\0';

		nXStart = wcslen( szTempStr );
		strDes += strSour.Mid( 0, nXStart );

		INT nLRLen(0);

		if (m_nLanguage == JAPANESE)
		{
			for (int i = 0; ; i++)
			{
				if (m_dwCompCls[i] == m_nComAttrSize)
					break;

				memcpy(szTempStr, &m_szComStr[m_dwCompCls[i]], WCHARSIZE * (m_dwCompCls[i+1] - m_dwCompCls[i]));
				szTempStr[m_dwCompCls[i+1] - m_dwCompCls[i]] = L'\0';
				nXEnd = nXStart + GetWidthOfString(szTempStr);

				CStringW strTempStr( szTempStr );
				INT nEnd = wcslen( szTempStr );

				if (m_bComAttr[m_dwCompCls[i]] == 0x00)
				{
					strDes += L'[' + strTempStr.Mid( 0, nEnd ) + L']';
				}
				else
				{
					if (m_bComAttr[m_dwCompCls[i]] == 0x01)	wChar = L'_';
					else									wChar = L'-';

					strDes += wChar + strTempStr.Mid( 0, nEnd );
				}

				nLRLen += nEnd;
			}
		}

		INT nLen = nXStart + nLRLen;
		strDes += strSour.Mid( nLen, strSour.GetLength() - nLen );

		WideCharToMultiByte ( m_nCodePage, NULL, strDes, -1, m_szBufferMbs, BUFFERSIZE*2, NULL, NULL );
	}
	else
	{
		WideCharToMultiByte ( m_nCodePage, NULL, m_szBuffer, -1, m_szBufferMbs, BUFFERSIZE*2, NULL, NULL );
	}
	*/
}

void CIMEEdit::SetString ( const char* szChange )
{
	if ( szChange && strlen(szChange) )
	{
		int nLen = MultiByteToWideChar( m_nCodePage, 
			NULL, 
			szChange, 
			-1, 
			m_szBuffer, 
			BUFFERSIZE+1 );
		m_xEndPos = m_xInsertPos = m_xCaretPos = nLen-1;
	}
	else
	{
		ClearBuffer();
	}

	//if( DXInputString::m_bCaratMove )
	//{
	//	// 이 부분을 처리해야 제한길이만큼 입력했을 때 버그가 발생하지 않는다 ( 준혁 )
	//	memcpy( m_szBackup, m_szBuffer, WCHARSIZE * (BUFFERSIZE+1) );
	//}

	if( m_fStat )
	{
		// Turn off composition flag
		m_fStat = FALSE;

		// IME 속성에 IME_PROP_AT_CARET이 없으면 수준 3 기능을 무시합니다.
		if ( m_dwProperty&IME_PROP_AT_CARET )
		{
			OnImeEndComposition(0,0);
		}
	}
}

int CIMEEdit::SetInsertPos( int xPos )
{
	if( DXInputString::m_bCaratMove == FALSE )
		return m_xEndPos;

	m_xCaretPos = xPos;

	if( m_xCaretPos > m_xEndPos )
		m_xCaretPos = m_xEndPos;

	m_xInsertPos = m_xCaretPos;

	return m_xCaretPos;
}

BEGIN_MESSAGE_MAP(CIMEEdit, CEdit)
	//{{AFX_MSG_MAP(CIMEEdit)
	ON_WM_PAINT()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HRESULT CIMEEdit::Render ( LPDIRECT3DDEVICEQ pd3dDevice, int x, int y, DWORD dwColor, CD3DFontPar *pFont )
{
	if ( !pd3dDevice )	return E_FAIL;
	if ( !pFont )		return E_FAIL;

	int nXStart(0), nXEnd(0);
	wchar_t uzTempStr[BUFFERSIZE+1]={0};
	TCHAR szTempStr[BUFFERSIZE*2+1]={0};

	pFont->DrawText ( (float)x, (float)y, dwColor, GetString() );

	//CDebugSet::ToView ( 1, 8, "[Render]%s", GetString() );

	// Draw dotted line under composition string
	if ( (m_fStat) && (m_dwProperty&IME_PROP_AT_CARET) && (m_nLanguage!=KOREAN) )
	{
		// Calculate start position of composition string and move pen to start position
		memcpy(uzTempStr, m_szBuffer, WCHARSIZE * m_xInsertPos);	 
		uzTempStr[m_xInsertPos] = L'\0';
		WideCharToMultiByte ( m_nCodePage, NULL, uzTempStr, -1, szTempStr, BUFFERSIZE*2+1, NULL, NULL );

		SIZE sizeExtend;
		pFont->GetTextExtent ( szTempStr, sizeExtend );
		nXStart = X_INIT + sizeExtend.cx;

		DWORD dwHeight = pFont->GetFontHeight();

		//dc.MoveTo(nXStart, Y_INIT + tm.tmHeight);

		// 컴퍼지션 문자열의 끝 위치를 계산하고 점선을 그립니다.
		m_szComStr[m_nComSize/WCHARSIZE] = L'\0';

		if ( m_nLanguage==JAPANESE )
		{
			for (int i = 0; ; i++)
			{
				if ( m_dwCompCls[i]==m_nComAttrSize )	break;

				memcpy ( uzTempStr, &m_szComStr[m_dwCompCls[i]], WCHARSIZE * (m_dwCompCls[i+1] - m_dwCompCls[i]) );
				uzTempStr[m_dwCompCls[i+1]-m_dwCompCls[i]] = L'\0';
				WideCharToMultiByte ( m_nCodePage, NULL, uzTempStr, -1, szTempStr, BUFFERSIZE*2+1, NULL, NULL );

				SIZE sizeExtend;
				pFont->GetTextExtent ( szTempStr, sizeExtend );

				nXEnd = nXStart + sizeExtend.cx;

				/*if (m_bComAttr[m_dwCompCls[i]] == 0x00)
				pPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));	// For input characters
				else if (m_bComAttr[m_dwCompCls[i]] == 0x01)
					pPen = new CPen(PS_SOLID, 2, RGB(0, 0, 0));	// For editable phrase
				else
					pPen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));	// For converted characters

				dc.LineTo(nXEnd - 1, Y_INIT + tm.tmHeight);
				dc.MoveTo(nXEnd + 1, Y_INIT + tm.tmHeight);*/

					nXStart = nXEnd;
			}
		}
		else
		{
			WideCharToMultiByte ( m_nCodePage, NULL, m_szComStr, -1, szTempStr, BUFFERSIZE*2+1, NULL, NULL );

			SIZE sizeExtend;
			pFont->GetTextExtent ( szTempStr, sizeExtend );

			nXEnd = nXStart + sizeExtend.cx;
			//pPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));
			//dc.LineTo(nXEnd, Y_INIT + tm.tmHeight);
		}
	}

	// 현재 컴퍼지션 문자열(중국어 번체 및 중국어 간체)을 반전합니다.
	//if (m_fShowInvert)
	//{
	//	// 반전 문자열의 시작 위치를 계산합니다.
	//	memcpy(szTempStr, m_szBuffer, WCHARSIZE * m_nInvertStart);	 
	//	szTempStr[m_nInvertStart] = L'\0';
	//	cRect.top = Y_INIT;
	//	cRect.left = X_INIT + GetWidthOfString(szTempStr);

	//	// 반전 문자열의 끝 위치를 계산합니다.
	//	cRect.bottom = cRect.top + tm.tmHeight;
	//	memcpy(szTempStr, &m_szBuffer[m_nInvertStart], WCHARSIZE * (m_nInvertEnd - m_nInvertStart));	 
	//	szTempStr[m_nInvertEnd - m_nInvertStart] = L'\0';
	//	cRect.right = cRect.left + GetWidthOfString(szTempStr);

	//	dc.InvertRect(cRect);  
	//}

	//ShowCaret();
	//SelectObject(dc, pOldFont);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CIMEEdit message handlers

void CIMEEdit::OnPaint() 
{
	CPaintDC dc(this); //  그리기를 위한 디바이스 컨텍스트입니다.

	TEXTMETRIC	tm;
	int			nXStart, nXEnd;
	CRect		cRect;
	HFONT		pOldFont = (HFONT)SelectObject(dc, m_hFont);
	CPen		*pPen, *pOldPen;
	wchar_t		szTempStr[BUFFERSIZE+1]={0};
	RECT		rcBounds;

	HideCaret();

	// 배경을 흰색으로 채웁니다.
	GetClientRect(&rcBounds);
	dc.FillRect(&rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));

	// 버퍼의 텍스트를 표시합니다.
	TextOutW(dc, X_INIT, Y_INIT, m_szBuffer, (int)wcslen(m_szBuffer));

	dc.GetTextMetrics(&tm);

	// 컴퍼지션 문자열 아래에 점선을 그립니다.
	if ( (m_fStat) && (m_dwProperty & IME_PROP_AT_CARET) && (m_nLanguage != KOREAN) )
	{
		// 컴퍼지션 문자열의 시작 위치를 계산하고 시작 위치로 펜을 이동합니다.
		memcpy(szTempStr, m_szBuffer, WCHARSIZE * m_xInsertPos);	 
		szTempStr[m_xInsertPos] = L'\0';
		nXStart = X_INIT + GetWidthOfString(szTempStr);
		dc.MoveTo(nXStart, Y_INIT + tm.tmHeight);

		// 컴퍼지션 문자열의 끝 위치를 계산하고 점선을 그립니다.
		m_szComStr[m_nComSize / WCHARSIZE] = L'\0';

		if (m_nLanguage == JAPANESE)
		{
			for (int i = 0; ; i++)
			{
				if (m_dwCompCls[i] == m_nComAttrSize)
					break;
				memcpy(szTempStr, &m_szComStr[m_dwCompCls[i]], WCHARSIZE * (m_dwCompCls[i+1] - m_dwCompCls[i]));	 
				szTempStr[m_dwCompCls[i+1] - m_dwCompCls[i]] = L'\0';
				nXEnd = nXStart + GetWidthOfString(szTempStr);

				if (m_bComAttr[m_dwCompCls[i]] == 0x00)
					pPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));	// 입력 문자에 사용됩니다.
				else if (m_bComAttr[m_dwCompCls[i]] == 0x01)
					pPen = new CPen(PS_SOLID, 2, RGB(0, 0, 0));	// 편집 가능한 구에 사용됩니다.
				else 
					pPen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));	// 변환된 문자에 사용됩니다.

				pOldPen = dc.SelectObject(pPen);
				dc.LineTo(nXEnd - 1, Y_INIT + tm.tmHeight);
				dc.MoveTo(nXEnd + 1, Y_INIT + tm.tmHeight);
				dc.SelectObject(pOldPen);
				delete pPen;
				nXStart = nXEnd;
			}
		}
		else
		{
			nXEnd = nXStart + GetWidthOfString(m_szComStr);
			pPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));
			pOldPen = dc.SelectObject(pPen);
			dc.LineTo(nXEnd, Y_INIT + tm.tmHeight);
			dc.SelectObject(pOldPen);
			delete pPen;
		}
	}

	// 현재 컴퍼지션 문자열(중국어 번체 및 중국어 간체)을 반전합니다.
	if (m_fShowInvert)
	{
		// 반전 문자열의 시작 위치를 계산합니다.
		memcpy(szTempStr, m_szBuffer, WCHARSIZE * m_nInvertStart);	 
		szTempStr[m_nInvertStart] = L'\0';
		cRect.top = Y_INIT;
		cRect.left = X_INIT + GetWidthOfString(szTempStr);

		// 반전 문자열의 끝 위치를 계산합니다.
		cRect.bottom = cRect.top + tm.tmHeight;
		memcpy(szTempStr, &m_szBuffer[m_nInvertStart], WCHARSIZE * (m_nInvertEnd - m_nInvertStart));	 
		szTempStr[m_nInvertEnd - m_nInvertStart] = L'\0';
		cRect.right = cRect.left + GetWidthOfString(szTempStr);

		dc.InvertRect(cRect);  
	}

	ShowCaret();
	SelectObject(dc, pOldFont);
}

void CIMEEdit::OnSetFocus(CWnd* pOldWnd) 
{
	m_bWideCaret = false;

	CreateSolidCaret(1,FONT_HEIGHT);

	//	enter 키 초기화.
	m_bEnterKeyDown = false;

	// 캐럿 위치를 설정합니다.
	ShowCaretOnView();	

	ShowCaret();
}

void CIMEEdit::OnKillFocus(CWnd* pNewWnd) 
{
	HIMC	hIMC;

	//	enter 키 초기화.
	m_bEnterKeyDown = false;

	//	케럿 숨기기
	HideCaret();

	// 고스트 캐럿을 삭제합니다.
	Invalidate();

	// 컴퍼지션이 끝나지 않았으면 컴퍼지션 문자열을 완료합니다.
	if ( (m_fStat) && (m_nLanguage != KOREAN) )
	{
		hIMC = ImmGetContext(this->m_hWnd);
		if (hIMC) 
		{
			ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
			ImmReleaseContext(this->m_hWnd,hIMC);
		}
	}
}

/*****************************************************************************\
* 함수: ShowCaretOnView
*
* 캐럿 위치를 계산하고 캐럿을 다시 표시합니다.
\*****************************************************************************/
void CIMEEdit::ShowCaretOnView()
{
	wchar_t *szTmpStr = new(wchar_t[m_xCaretPos + 1]);

	HideCaret();

	// 캐럿 위치를 계산하고 캐럿 위치를 설정합니다.
	memcpy(szTmpStr, m_szBuffer, WCHARSIZE * (m_xCaretPos));	 
	szTmpStr[m_xCaretPos] = L'\0';
	SetCaretPos(CPoint(X_INIT + GetWidthOfString(szTmpStr), Y_INIT));
	ShowCaret();

	// IME 속성에 IME_PROP_AT_CARET이 없으면 컴퍼지션 창의
	// 위치를 변경합니다.
	if ( !(m_dwProperty & IME_PROP_AT_CARET) )
		SetCompositionWindowPos();

	delete [] szTmpStr;
}

/*****************************************************************************\
* 함수: GetWidthOfString
*
* 문자열 너비(픽셀)를 계산합니다. 
*
* 인수:
*	LPCTSTR szStr - 문자열입니다.
\*****************************************************************************/
int CIMEEdit::GetWidthOfString(wchar_t *szStr)
{
	SIZE		Size;
	CClientDC	dc(this);
	HFONT		pOld = (HFONT)SelectObject(dc, m_hFont);

	GetTextExtentPoint32W(dc, szStr, (int)wcslen(szStr), &Size);

	SelectObject(dc, pOld);

	return Size.cx;
}

/*****************************************************************************\
* 함수: OnImeStartComposition
*
* WM_IME_STARTCOMPOSITION 메시지를 받고 컴퍼지션을 시작합니다
*
* 인수:
*	WPARAM wParam - 사용하지 않습니다.
*	LPARAM lParam - 사용하지 않습니다.
\*****************************************************************************/
void CIMEEdit::OnImeStartComposition(WPARAM, LPARAM)
{
	// 버퍼를 확인합니다. 꽉 찼으면 버퍼를 지웁니다. 
	if (m_xEndPos >= (BUFFERSIZE-1))	// 버퍼를 확인합니다.
		ClearBuffer();

	// 한국어 문자 집합인 경우 캐럿을 변경합니다. 
	if (m_nLanguage == KOREAN)
	{
		m_bWideCaret = true;

		CreateSolidCaret(FONT_WIDTH, FONT_HEIGHT);	// 넓은 캐럿을 만듭니다.
		ShowCaret();
	}

	// 문자열 버퍼를 백업합니다. 
	memcpy(m_szBackup, m_szBuffer, WCHARSIZE * BUFFERSIZE+1);
}

/*****************************************************************************\
* 함수: OnImeStartComposition
*
* WM_IME_COMPOSITION 메시지 및 컴퍼지션 문자열을 받습니다.
*
* 인수:
*	WPARAM wParam - 컴퍼지션 문자열에 대한 마지막 변경 내용을 나타내는 
*					DBCS 문자입니다.
*	LPARAM lParam - 변경 플래그입니다.
\*****************************************************************************/
BOOL CIMEEdit::OnImeComposition(WPARAM, LPARAM lParam)
{
	HIMC	hIMC;
	int		i, nTmpInc;

	hIMC = ImmGetContext(this->m_hWnd);
	if (hIMC == NULL) 
		return TRUE;

	// 문자열 버퍼를 복원합니다.
	if (m_fStat)
		memcpy(m_szBuffer, m_szBackup, WCHARSIZE * (BUFFERSIZE+1));

	if (lParam & GCS_RESULTSTR)
	{
		m_fShowInvert = FALSE;

		// 결과 문자열을 가져옵니다.
		SecureZeroMemory( m_szComStr, sizeof(m_szComStr) );
		m_nComSize = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, (LPVOID)m_szComStr, WCHARSIZE * (BUFFERSIZE+1));

		if ( m_nComSize > 0)
		{
			// 문자열 버퍼에 컴퍼지션 문자열을 삽입합니다.
			nTmpInc = InsertCompStr();

			// 삽입/캐럿 위치를 이동하고 끝 위치를 증가시킵니다.
			memcpy(m_szBackup, m_szBuffer, WCHARSIZE * (BUFFERSIZE+1));
			m_xEndPos += nTmpInc;
			m_xInsertPos += nTmpInc;
			m_xCaretPos = m_xInsertPos;
		}
	}
	else if (lParam & GCS_COMPSTR)
	{
		// IME 속성에  IME_PROP_AT_CARET이 없으면 수준 3 기능을 무시합니다.
		if ( !(m_dwProperty & IME_PROP_AT_CARET) )
			return FALSE;

		// 컴퍼지션 문자열을 가져옵니다.
		SecureZeroMemory( m_szComStr, sizeof(m_szComStr) );
		m_nComSize = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, (LPVOID)m_szComStr, WCHARSIZE * (BUFFERSIZE+1));

		if (m_nLanguage != KOREAN)
		{
			// 컴퍼지션 특성 및 컴퍼지션 문자열의 커서 위치를 가져옵니다.
			if (lParam & GCS_COMPATTR)
			{
				SecureZeroMemory( m_bComAttr, sizeof(m_bComAttr) );
				m_nComAttrSize = ImmGetCompositionStringW(hIMC, GCS_COMPATTR, m_bComAttr, sizeof(m_bComAttr));
			}

			if (lParam & GCS_CURSORPOS)
				m_nComCursorPos = ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, NULL, 0);

			if (lParam & GCS_COMPCLAUSE)
			{
				SecureZeroMemory( m_dwCompCls, sizeof(m_dwCompCls) );
				ImmGetCompositionStringW(hIMC, GCS_COMPCLAUSE, m_dwCompCls,sizeof(m_dwCompCls));
			}

			if ((m_nLanguage == TRADITIONAL_CHINESE) || (m_nLanguage == SIMPLIFIED_CHINESE && !m_fIsXP ))
			{
				m_fShowInvert = TRUE;
				nTmpInc = 0;
				for (i = m_nComCursorPos; i < (int)m_nComAttrSize; i++)
				{
					if (m_bComAttr[i] == 1)
						nTmpInc++;
					else
						break;
				}
				m_nInvertStart = m_xInsertPos + m_nComCursorPos;
				m_nInvertEnd = m_nInvertStart + nTmpInc;
			}
		}

		if (m_nComSize > 0)
		{
			// 문자열 버퍼에 컴퍼지션 문자열을 삽입합니다.
			nTmpInc = InsertCompStr();

			// 캐럿 위치를 이동합니다.
			//switch( m_nLanguage )
			//{
			//case KOREAN:	// 준혁
			//	// 한국은 샘플코드 상으로 두꺼운 캐럿이 보여야 하지만 
			//	// 두꺼운 캐럿이 구현이 안되어서 캐럿 위치를 하나 증가한다.
			//	m_xCaretPos = m_xInsertPos + nTmpInc;
			//	break;
			//case JAPANESE:
			//	m_xCaretPos = m_xInsertPos + m_nComSize / WCHARSIZE;
			//	break;
			//default:
			//	m_xCaretPos = m_xInsertPos + m_nComCursorPos;
			//	break;
			//}

			// 아래는 샘플의 원본 코드
			if (m_nLanguage != KOREAN)
			{
				if (m_nLanguage == JAPANESE)
					m_xCaretPos = m_xInsertPos + m_nComSize / WCHARSIZE;
				else
					m_xCaretPos = m_xInsertPos + m_nComCursorPos;
			}
		}
	}
	else
	{
		// 컴퍼지션 문자열이 없습니다.
		m_nComSize = 0;
	}

	ImmReleaseContext(this->m_hWnd, hIMC);

	ShowCaretOnView();
	Invalidate();
	return TRUE;
}

/*****************************************************************************\
* 함수: OnImeEndComposition
*
* WM_IME_ENDCOMPOSITION 메시지를 받고 컴퍼지션을 마칩니다.
*
* 인수:
*	WPARAM wParam - 사용하지 않습니다. 
*	LPARAM lParam - 사용하지 않습니다.
\*****************************************************************************/
void CIMEEdit::OnImeEndComposition(WPARAM, LPARAM)
{
	// 반전 플래그를 끕니다.
	m_fShowInvert = FALSE;

	// 한국어 문자 집합인 경우 캐럿을 일반 스타일로 변경합니다.
	if (m_nLanguage == KOREAN)
	{
		m_bWideCaret = false;

		CreateSolidCaret(1,FONT_HEIGHT);
	}

	ShowCaret();
}

/*****************************************************************************\
* 함수: InsertCompStr
*
* 문자열 버퍼에 컴퍼지션 문자열을 삽입합니다.
*
* 인수:
*	WPARAM wParam - 사용하지 않습니다. 
*	LPARAM lParam - 사용하지 않습니다.
*
* 반환 값:
*	TCHAR에서 컴퍼지션 문자열의 크기입니다.
\*****************************************************************************/
int CIMEEdit::InsertCompStr()
{
	HIMC	hIMC;
	int		nTmpPos, nTmpInc;
	wchar_t	szTmpStr[BUFFERSIZE+1]={0};

	// 삽입 위치 뒤의 하위 문자열을 백업합니다.
	nTmpPos = m_xEndPos - m_xInsertPos;
	memcpy(szTmpStr, &m_szBackup[m_xInsertPos], WCHARSIZE * nTmpPos);
	szTmpStr[nTmpPos] = L'\0';

	// 버퍼 오버플로를 확인합니다.
	if ((m_nComSize % 2) != 0)
		m_nComSize++;
	nTmpInc = m_nComSize / WCHARSIZE;

	if ((m_xEndPos + nTmpInc) < 0)
		return 0;

	if ((m_xEndPos + nTmpInc) >= BUFFERSIZE)
	{
		if (m_xInsertPos > 0)
		{
			// 삽입 위치 앞의 하위 문자열을 삭제합니다.
			memcpy(m_szBackup, szTmpStr, WCHARSIZE * (BUFFERSIZE+1));
			ClearBuffer();
			m_xEndPos = nTmpPos;
		}
		else
		{
			// 컴퍼지션 문자열을 취소합니다.
			hIMC = ImmGetContext(this->m_hWnd);
			if (hIMC)
			{
				ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
				ImmReleaseContext(this->m_hWnd,hIMC);
				ClearBuffer();
			}
		}
	}

	// 컴퍼지션 문자열을 문자열 버퍼로 복사합니다.
	memcpy(&m_szBuffer[m_xInsertPos], m_szComStr, m_nComSize);

	// 삽입 위치 뒤의 하위 문자열을 복원합니다.
	memcpy(&m_szBuffer[m_xInsertPos+nTmpInc], szTmpStr, WCHARSIZE * nTmpPos);
	m_szBuffer[m_xEndPos + nTmpInc] = L'\0';

	return nTmpInc;
}

void CIMEEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int		nTmpDec;
	wchar_t wChar, szTmpStr[BUFFERSIZE+1]={0};

	// 현재 상태를 확인합니다.
	//if (m_fStat)
	//	return;

	switch(nChar)
	{
		// Return key
	case _T('\r') :
		{
			// 삽입 위치 앞의 하위 문자열을 삭제합니다.
			//memcpy(m_szBuffer, &m_szBuffer[m_xInsertPos], WCHARSIZE * (m_xEndPos - m_xInsertPos));
			//m_xEndPos = m_xEndPos - m_xInsertPos;
			//m_xInsertPos = m_xCaretPos = 0;
			//m_szBuffer[m_xEndPos] = L'\0';
			m_bEnterKeyDown = true;
		}
		break;

		// Back space key
	case _T('\b') :
		{
			// 한 문자를 삭제합니다.
			if(m_xInsertPos == 0) break;
			nTmpDec = GetCombinedCharLength(m_xInsertPos-1);
			memcpy(&m_szBuffer[m_xInsertPos-nTmpDec], &m_szBuffer[m_xInsertPos], WCHARSIZE * (m_xEndPos - m_xInsertPos));
			m_xEndPos -= nTmpDec;
			m_xInsertPos -= nTmpDec;
			m_xCaretPos -= nTmpDec;
			m_szBuffer[m_xEndPos] = L'\0';
		}
		break;

	default :
		{
			if( m_bKeyUse )
			{
				if (nChar < TCHAR(0x20))
					break;
#ifdef _UNICODE
				wChar = nChar;
#else
				MultiByteToWideChar(m_nCodePage, 0, (char *)&nChar, 1, &wChar, 2);
#endif
				// 버퍼 오버플로를 확인합니다.
				if( m_xEndPos > (BUFFERSIZE-1) )	
					ClearBuffer();

				// 문자열 버퍼로 문자를 삽입합니다.
				memcpy(szTmpStr, &m_szBuffer[m_xInsertPos], WCHARSIZE * (m_xEndPos - m_xInsertPos));
				szTmpStr[m_xEndPos - m_xInsertPos] = L'\0';
				m_szBuffer[m_xInsertPos] = wChar;
				memcpy(&m_szBuffer[m_xInsertPos+1], szTmpStr, WCHARSIZE * (m_xEndPos - m_xInsertPos));
				m_xEndPos++;
				m_xInsertPos++;
				m_xCaretPos++;
				m_szBuffer[m_xEndPos] = L'\0';
			}
		}
		break;
	}

	memcpy( m_szBackup, m_szBuffer, WCHARSIZE * (BUFFERSIZE+1) );

	ShowCaretOnView();
	Invalidate();
}

void CIMEEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int nTmpDec;

	// IME 속성에 IME_PROP_AT_CARET이 없으면 컴퍼지션 중에 문자를 받지 않습니다.
	if ( !( m_dwProperty & IME_PROP_AT_CARET ) && m_fStat )
		return;

	if( DXInputString::m_bCaratMove == FALSE )
	{
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	switch( nChar )
	{
	case VK_LEFT:	// Left arrow key
		if(m_xInsertPos == 0) break;
		m_xInsertPos -= GetCombinedCharLength( m_xInsertPos - 1 );
		m_xCaretPos = m_xInsertPos;
		ShowCaretOnView();
		break;

	case VK_RIGHT:	// Right arrow key
		if(m_xInsertPos == m_xEndPos) break;
		m_xInsertPos +=  GetCombinedCharLength(m_xInsertPos);
		m_xCaretPos =  m_xInsertPos;
		ShowCaretOnView();
		break;

	case VK_DELETE:	// Delete key
		if ((m_xInsertPos == m_xEndPos) || (m_xEndPos > BUFFERSIZE)) break;
		nTmpDec = GetCombinedCharLength(m_xInsertPos);
		memcpy(&m_szBuffer[m_xInsertPos], &m_szBuffer[m_xInsertPos+nTmpDec], WCHARSIZE * (m_xEndPos - m_xInsertPos - nTmpDec));
		m_xEndPos -= nTmpDec;
		m_szBuffer[m_xEndPos] = L'\0';
		Invalidate();
		break;

	case VK_HOME:	// Home key
		if (m_xInsertPos == 0) break;
		m_xInsertPos = m_xCaretPos = 0;
		ShowCaretOnView();
		break;

	case VK_END:	// End key
		if (m_xInsertPos == m_xEndPos) break;
		m_xInsertPos = m_xCaretPos = m_xEndPos;
		ShowCaretOnView();
		break;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

/****************************************************************************\
* 함수: SetIMEMode
*
* IME 모드를 설정합니다.
*
* 인수:
*	HWND hWnd - 테스트 창의 핸들입니다.
*	DWORD dwNewConvMode - 변환 모드를 변경하는 옵션입니다.
*	DWORD dwNewSentMode - Sentence 모드를 변경하는 옵션입니다.
*	BOOL fFlag - TRUE이면 전달된 옵션을 설정하고, 그렇지 않으면 옵션을 해제합니다.
\**************************************************************************** */
void CIMEEdit::SetIMEMode ( HWND hWnd, DWORD dwNewConvMode, DWORD dwNewSentMode, BOOL fFlag )
{
	HIMC	hImc;
	DWORD	dwConvMode, dwSentMode;
	BOOL	fRet;

	//입력 컨텍스트를 가져옵니다.
	hImc = ImmGetContext(hWnd);
	if (hImc)
	{
		//현재 IME 상태를 가져옵니다.
		ImmGetConversionStatus ( hImc, &dwConvMode, &dwSentMode );

		//전달된 옵션을 사용하여 IME 상태를 변경합니다. 
		if (fFlag)
		{
			fRet = ImmSetConversionStatus ( hImc, dwConvMode | dwNewConvMode, dwSentMode | dwNewSentMode );
//			if ( (GetLanguage()==JAPANESE) && (dwNewConvMode&IME_CMODE_NATIVE) )
//			{
				// Note : 일본어 입력모드에서 항상 직접입력 모드로 시작하기 위해 !을 붙였다.
//				ImmSetOpenStatus ( hImc, !fFlag );
//			}
		}
		else
		{
			ImmSetConversionStatus ( hImc, dwConvMode&~dwNewConvMode, dwSentMode&~dwNewSentMode );
//			if ( (GetLanguage()==JAPANESE) && (dwNewConvMode&IME_CMODE_NATIVE) )
//			{
//				ImmSetOpenStatus ( hImc, fFlag );
//			}
		}

		//입력 컨텍스트를 해제합니다. 
		ImmReleaseContext ( hWnd, hImc );
	}
}

BOOL CIMEEdit::IsNativeMode ()
{
	HWND hWnd = GetSafeHwnd();

	HIMC hIMC = ImmGetContext(hWnd);
	if ( hIMC )
	{
		DWORD dwComposition, dwSentence;

		ImmGetConversionStatus(hIMC, &dwComposition, &dwSentence);

		// Release input context
		ImmReleaseContext ( hWnd, hIMC );

		if ( (dwComposition&IME_CMODE_NATIVE)!=NULL ) return TRUE;
	}

	return FALSE;
}

/*****************************************************************************\
* 함수: SetFont
*
* IME 창에서 WM_INPUTLANGCHANGE 메시지를 받습니다.
* 
* 이 샘플에서는 글꼴 또는 코드 페이지를 변경하지 않고 모든 동아시아
* 언어에 대해 서로 다른 네 가지 IME를 사용할 수 있도록, 키보드 레이아웃에서
* ANSI-유니코드 변환에 사용되는 글꼴 및 코드 페이지를 확인합니다.
* IME 수준 3을 지원하기 위한 요구 사항은 아닙니다.
*
* 인수:
*	HKL  hKeyboardLayout - 새 키보드 레이아웃의 문자 집합을 지정합니다.
*	wchar_t *szSelectedFont - 글꼴 이름입니다. 
\*****************************************************************************/
void CIMEEdit::SetFont(HKL  hKeyboardLayout, LPCTSTR szSelectedFont)
{
	if (szSelectedFont)		
	{
		// 선택한 글꼴을 적용합니다. 
#ifdef _UNICODE
		StringCchCopy( m_szWFontName, 50, szSelectedFont );
		WideCharToMultiByte(m_nCodePage, 0, m_szWFontName, -1, (char *)m_szMBFontName, 50, NULL, NULL);
#else
		StringCchCopy( m_szMBFontName, 50, szSelectedFont );
		MultiByteToWideChar(m_nCodePage, 0, (char *)szSelectedFont, (int)strlen(szSelectedFont)+1, m_szWFontName, 50);
#endif
	}
	else
	{
		// 키보드 레이아웃을 확인합니다.
		if (hKeyboardLayout == 0)
			hKeyboardLayout = GetKeyboardLayout(0);

		switch (LOWORD(hKeyboardLayout))
		{
		case LID_TRADITIONAL_CHINESE:	m_nLanguage = TRADITIONAL_CHINESE;	break;
		case LID_JAPANESE:				m_nLanguage = JAPANESE;				break;
		case LID_KOREAN:				m_nLanguage = KOREAN;				break;
		case LID_SIMPLIFIED_CHINESE:	m_nLanguage = SIMPLIFIED_CHINESE;	break;
		case LID_THAILAND:				m_nLanguage = THAILAND;				break;
		case LID_VIETNAM:				m_nLanguage = VIETNAM;				break;
		default:						m_nLanguage = DEFAULT;				break;
		}

		// 코드 페이지 및 문자 집합을 설정합니다.
		if (m_nLanguage)
			m_nCodePage = CD3DFontPar::nCodePage[m_nLanguage];
		else
			m_nCodePage = GetACP();

		m_nCharSet = CD3DFontPar::nCharSet[m_nLanguage];

		// 기본 글꼴을 적용합니다.
		StringCchCopyW( m_szWFontName, 50, (wchar_t *)CD3DFontPar::szDefaultFontName[m_nLanguage] );
		WideCharToMultiByte(m_nCodePage, 0, m_szWFontName, -1, (char *)m_szMBFontName, 50, NULL, NULL);

		// IME 속성을 가져옵니다.
		m_dwProperty = ImmGetProperty( hKeyboardLayout, IGP_PROPERTY );
	}

	// 현재 글꼴을 삭제합니다.
	if (m_hFont)
		DeleteObject(m_hFont);

	// 글꼴을 만듭니다.
	if (m_fIsNT)
	{
		m_hFont = CreateFontW(FONT_WIDTH, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, m_nCharSet,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, m_szWFontName);
	}
	else
	{
		m_hFont = CreateFontA(FONT_WIDTH, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, m_nCharSet,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, m_szMBFontName);
	}

	// 선택한 글꼴 또는 기본 글꼴을 만들지 못하면 기본 글꼴을 만듭니다.
	if (!m_hFont)
	{
		// 기본 글꼴을 적용합니다.
		StringCchCopyW( m_szWFontName, 50, (wchar_t *)CD3DFontPar::szDefaultFontName[DEFAULT] );
		WideCharToMultiByte(m_nCodePage, 0, m_szWFontName, -1, (char *)m_szMBFontName, 50, NULL, NULL);

		if (m_fIsNT)
		{
			m_hFont = CreateFontW(FONT_WIDTH, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, m_nCharSet,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, m_szWFontName);
		}
		else
		{
			m_hFont = CreateFontA(FONT_WIDTH, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, m_nCharSet,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, m_szMBFontName);
		}
	}

	ShowCaretOnView();
	Invalidate();
}

/*****************************************************************************\
* 함수: GetCombinedCharLength
*
* 결합된 문자의 길이를 가져옵니다. 
*
* 이 구현은 서로게이트만 지원합니다. 필요한 경우 결합된 문자를
* 처리해야 합니다.  
*
* 반환 값:
*	유니코드인 경우 
*		서로게이트 문자이면 2를 반환하고, 그렇지 않으면 1을 반환합니다.
*	유니코드가 아닌 경우
*		DBCS 문자이면 2를 반환하고, 그렇지 않으면 1을 반환합니다.
\*****************************************************************************/
int CIMEEdit::GetCombinedCharLength(int nTmpPos) 
{
	int	i, nRet = 1;

	// 서로게이트 문자를 확인합니다.
	for (i = 0; i <= nTmpPos; i++)
	{
		if ((0xD800 <= m_szBuffer[i]) && (m_szBuffer[i] <= 0xDBFF))
		{
			nRet = 2;
			++i;
		}
		else
			nRet = 1;
	}

	return nRet;
}

/*****************************************************************************\
* 함수: ClearBuffer
*
* 문자열 버퍼를 지웁니다.
\*****************************************************************************/
void CIMEEdit::ClearBuffer()
{
	SecureZeroMemory( m_szBuffer, sizeof(m_szBuffer) );

	m_xEndPos = m_xInsertPos = m_xCaretPos = 0;
}


/*****************************************************************************\
* 함수: SetCandiDateWindowPos
*
* 한국어/일본어 IME의 후보 창 위치를 설정합니다.
\*****************************************************************************/
void CIMEEdit::SetCandiDateWindowPos() 
{
	HIMC		hIMC;
	CClientDC	dc(this);
	TEXTMETRIC	tm;
	CANDIDATEFORM Candidate;

	hIMC = ImmGetContext(this->m_hWnd);
	if (hIMC) 
	{
		dc.GetTextMetrics(&tm);

		Candidate.dwIndex = 0;
		Candidate.dwStyle = CFS_EXCLUDE;

		ImmSetCandidateWindow(hIMC, &Candidate);
		ImmReleaseContext(this->m_hWnd,hIMC);

		// Note : 아래 코드는 샘플 코드 원본
		//dc.GetTextMetrics(&tm);

		//Candidate.dwIndex = 0;
		//Candidate.dwStyle = CFS_FORCE_POSITION;

		//if (m_nLanguage == JAPANESE) 
		//{
		//	// 편집 가능한 문자 가까이에 후보 창 위치를 설정합니다.
		//	wchar_t *szTmpStr = new(wchar_t[m_xInsertPos + m_nComCursorPos + 1]);
		//	memcpy(szTmpStr, m_szBuffer, WCHARSIZE * (m_xInsertPos + m_nComCursorPos));	 
		//	szTmpStr[m_xInsertPos + m_nComCursorPos] = L'\0';
		//	Candidate.ptCurrentPos.x = X_INIT + GetWidthOfString(szTmpStr);
		//}
		//else
		//{
		//	// 캐럿 위치 가까이에 후보 창 위치를 설정합니다.
		//	CPoint		point;

		//	point = GetCaretPos();
		//	Candidate.ptCurrentPos.x = point.x;
		//}
		//Candidate.ptCurrentPos.y = Y_INIT + tm.tmHeight + 1;
		//ImmSetCandidateWindow(hIMC, &Candidate);

		//ImmReleaseContext(this->m_hWnd,hIMC);
	}
}

/*****************************************************************************\
* 함수: SetCompositionWindowPos
*
* 중국어 번체 IME의 컴퍼지션 창 위치를 설정합니다.
\*****************************************************************************/
void CIMEEdit::SetCompositionWindowPos() 
{
	HIMC		hIMC;
	CPoint		point;
	COMPOSITIONFORM Composition;

	hIMC = ImmGetContext(this->m_hWnd);
	if (hIMC)
	{
		// 캐럿 위치 가까이에 컴퍼지션 창 위치를 설정합니다.
		point = GetCaretPos();
		Composition.dwStyle = CFS_POINT;
		Composition.ptCurrentPos.x = point.x;
		Composition.ptCurrentPos.y = point.y;
		ImmSetCompositionWindow(hIMC, &Composition);

		ImmReleaseContext(this->m_hWnd,hIMC);
	}
}

LRESULT CIMEEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
		// 입력 언어가 변경되면 설정을 변경합니다.
	case WM_INPUTLANGCHANGE:
		{
			// IME 변경시 버퍼가 클리어 되어서 이전 입력이 사라진다.
			// 샘플 코드에서 주석 처리를 했다. 문제가 없다면 이렇게 사용한다.
			//
			//ClearBuffer(); 

			// 변경된 입력 언어에 대한 새 글꼴을 만듭니다.
			SetFont((HKL) lParam, NULL);
			Invalidate();
			ShowCaretOnView();

			// IME 모드 변경에 대한 메시지를 보냅니다.
			//::PostMessage(GetParent()->m_hWnd, WM_SETINPUTLANG, 0L, 0L);
		}
		break;

		// 컴퍼지션을 시작합니다.
	case WM_IME_STARTCOMPOSITION:
		{
			// 컴퍼지션 플래그를 켭니다.
			m_fStat = TRUE;

			// IME 속성에 IME_PROP_AT_CARET이 없으면 수준 3 기능을 무시합니다.
			if ( !(m_dwProperty & IME_PROP_AT_CARET) )
				break;

			OnImeStartComposition(wParam, lParam);
		}
		return 0l;

		// 컴퍼지션 문자입니다.
	case WM_IME_COMPOSITION:
		{
			if (OnImeComposition(wParam, lParam))
				return 0l;
			// CEdit::WinProc를 호출하여 컴퍼지션 창을 표시합니다.
		}
		break;

		// 컴퍼지션을 끝냅니다.
	case WM_IME_ENDCOMPOSITION:
		{
			// 컴퍼지션 플래그를 끕니다.
			m_fStat = FALSE;

			// IME 속성에 IME_PROP_AT_CARET이 없으면 수준 3 기능을 무시합니다.
			if ( !(m_dwProperty & IME_PROP_AT_CARET) )
				break;

			OnImeEndComposition(wParam, lParam);
		}
		return 0l;

	case WM_IME_NOTIFY:
		switch (wParam)
		{
			// 후보 창 위치를 설정합니다.
		case IMN_OPENCANDIDATE:
			if (m_nLanguage == JAPANESE)
				SetCandiDateWindowPos();
			break;

			//// IME 모드의 변경을 감지하려면 
			//case IMN_SETCONVERSIONMODE:
			//case IMN_SETSENTENCEMODE:
			//	::PostMessage(GetParent()->m_hWnd, WM_RESETMODE, wParam, lParam);
			//	break;

			//	// 일본어 IME의 전환을 감지하려면
			//case IMN_SETOPENSTATUS:   
			//	::PostMessage(GetParent()->m_hWnd, WM_TOGGLE, wParam, lParam);
			//	break;
		}	
		break;

		//case WM_IME_SETCONTEXT:
		//	// 일본어 Win9x의 IME 모드 제어를 제외하고 이 메시지를 보낼 필요가 없습니다.
		//	if ((m_nLanguage ==  JAPANESE) && !m_fIsNT) 				
		//	{
		//		::PostMessage(GetParent()->m_hWnd, WM_SETMODE, wParam, lParam);
		//	}
		//	break;
	}

	return CEdit::WindowProc(message, wParam, lParam);
}

bool CIMEEdit::CheckEnterKeyDown ()
{
	bool bKEYDOWN = m_bEnterKeyDown;
	m_bEnterKeyDown = false;

	return bKEYDOWN;
}