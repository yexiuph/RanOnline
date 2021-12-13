#if !defined(AFX_IMEEDIT_H__F425503C_0F3C_4C7D_A2E5_A9A7E35AC07B__INCLUDED_)
#define AFX_IMEEDIT_H__F425503C_0F3C_4C7D_A2E5_A9A7E35AC07B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IMEEdit.h : 헤더 파일입니다.
//

// CIMEEdit에 대한 상수를 정의합니다.
#define X_INIT					0		
#define Y_INIT					0
#define FONT_HEIGHT				9
#define FONT_WIDTH				9
#define BUFFERSIZE				1024
#define WCHARSIZE				2

// 언어 ID를 정의합니다.
#define LID_TRADITIONAL_CHINESE	0x0404
#define LID_JAPANESE			0x0411
#define LID_KOREAN				0x0412
#define LID_SIMPLIFIED_CHINESE	0x0804
#define LID_THAILAND			0x041E
#define LID_VIETNAM				0x042A

/////////////////////////////////////////////////////////////////////////////
// CIMEEdit window
class CD3DFontPar;

class CIMEEdit : public CEdit
{
	// Construction
public:
	CIMEEdit();

	// Attributes
protected:
	bool	m_bEnterKeyDown;			// 입력 종료 여부 판단에 사용

	int		m_xEndPos;					// 버퍼의 끝 위치입니다. 
	int		m_xInsertPos;				// 버퍼의 삽입 위치입니다.
	int		m_xCaretPos;				// 버퍼의 캐럿 위치입니다.
	int		m_nComSize;					// 컴퍼지션 문자열 크기입니다.
	int		m_nComCursorPos;			// 컴퍼지션 문자열의 현재 커서 위치입니다.
	DWORD	m_nComAttrSize;				// 컴퍼지션 문자열의 특성 크기입니다.
	int		m_nInvertStart;				// 반전 문자열의 시작 위치입니다.
	int		m_nInvertEnd;				// 반전 문자열의 끝 위치입니다.
	DWORD	m_dwProperty;				// IME 속성입니다.
	UINT	m_nCodePage;				// 코드 페이지입니다.
	BYTE	m_nCharSet;					// 문자 집합입니다.
	HFONT	m_hFont;					// 글꼴입니다.
	BOOL	m_fShowInvert;				// true이면 샘플에서 반전 문자열을 표시합니다. 
	BOOL	m_fStat;					// true이면 IME 합성 문자열입니다.
	BYTE	m_bComAttr[BUFFERSIZE+1];	// 컴퍼지션 문자열의 특성입니다.
	DWORD	m_dwCompCls[BUFFERSIZE+1];	// 컴퍼지션입니다. 
	wchar_t	m_szBuffer[BUFFERSIZE+1];	// 입력 문자열의 버퍼입니다.
	wchar_t	m_szComStr[BUFFERSIZE+1];	// 컴퍼지션 문자열의 버퍼입니다.
	wchar_t	m_szBackup[BUFFERSIZE+1];	// 마지막 문자열 백업을 위한 버퍼입니다.

	char	m_szBufferMbs[BUFFERSIZE*2+1];
	BOOL	m_bKeyUse;					// 키보드 사용여부
	bool	m_bWideCaret;

public:
	wchar_t m_szWFontName[50];			// 글꼴 이름(유니코드)입니다.
	char	m_szMBFontName[50];			// 글꼴 이름(멀티바이트)입니다.
	int		m_nLanguage;				// 언어의 내부 인덱스입니다.
	BOOL	m_fIsNT;					// true이면 NT이고, 그렇지 않으면 Win9X입니다.
	BOOL	m_fIsXP;					// true이면 Windows XP 이상 버전입니다.

public:
	const char* GetString ();
	void SetString ( const char* szChange );

public:
	int GetInsertPos()				{ return m_xCaretPos; }
	int SetInsertPos( int xPos );

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, int x, int y, DWORD dwColor, CD3DFontPar *pFont );

public:
	bool CheckEnterKeyDown ();
	bool CheckComposition ()		{ return m_fStat!=FALSE; }
	bool CheckWideCaret ()			{ return m_bWideCaret; }

public:
	int GetLanguage()				{ return m_nLanguage; }
	void DisableKeyInput()			{ m_bKeyUse = FALSE; }
	void UsableKeyInput()			{ m_bKeyUse = TRUE; }

public:
	BOOL IsNativeMode();

	void ConverseNativeToEng()		{ SetIMEMode ( GetSafeHwnd(), IME_CMODE_NATIVE, 0, FALSE ); }
	void ConverseEngToNative()		{ SetIMEMode ( GetSafeHwnd(), IME_CMODE_NATIVE, 0, TRUE ); }
	void ConverseFullToHalf()		{ SetIMEMode ( GetSafeHwnd(), IME_CMODE_FULLSHAPE, 0, FALSE ); }

public:
	void SetIMEMode ( HWND hWnd, DWORD dwNewConvMode, DWORD dwNewSentMode, BOOL fFlag );

	// Operations
public:
	BOOL OnImeComposition(WPARAM wParam,LPARAM lParam);
	void OnImeStartComposition(WPARAM wParam,LPARAM lParam);	
	void OnImeEndComposition(WPARAM wParam,LPARAM lParam);
	int	 InsertCompStr();
	void ShowCaretOnView();
	int  GetWidthOfString(wchar_t*);
	void SetFont(HKL hKeyboardLayout, LPCTSTR szSelectedFont);
	void ClearBuffer();
	int  GetCombinedCharLength(int); 
	void SetCandiDateWindowPos();
	void SetCompositionWindowPos();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIMEEdit)
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CIMEEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIMEEdit)
	afx_msg void OnPaint();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMEEDIT_H__F425503C_0F3C_4C7D_A2E5_A9A7E35AC07B__INCLUDED_)