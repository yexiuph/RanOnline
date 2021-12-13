#if !defined(AFX_IMEEDIT_H__F425503C_0F3C_4C7D_A2E5_A9A7E35AC07B__INCLUDED_)
#define AFX_IMEEDIT_H__F425503C_0F3C_4C7D_A2E5_A9A7E35AC07B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IMEEdit.h : ��� �����Դϴ�.
//

// CIMEEdit�� ���� ����� �����մϴ�.
#define X_INIT					0		
#define Y_INIT					0
#define FONT_HEIGHT				9
#define FONT_WIDTH				9
#define BUFFERSIZE				1024
#define WCHARSIZE				2

// ��� ID�� �����մϴ�.
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
	bool	m_bEnterKeyDown;			// �Է� ���� ���� �Ǵܿ� ���

	int		m_xEndPos;					// ������ �� ��ġ�Դϴ�. 
	int		m_xInsertPos;				// ������ ���� ��ġ�Դϴ�.
	int		m_xCaretPos;				// ������ ĳ�� ��ġ�Դϴ�.
	int		m_nComSize;					// �������� ���ڿ� ũ���Դϴ�.
	int		m_nComCursorPos;			// �������� ���ڿ��� ���� Ŀ�� ��ġ�Դϴ�.
	DWORD	m_nComAttrSize;				// �������� ���ڿ��� Ư�� ũ���Դϴ�.
	int		m_nInvertStart;				// ���� ���ڿ��� ���� ��ġ�Դϴ�.
	int		m_nInvertEnd;				// ���� ���ڿ��� �� ��ġ�Դϴ�.
	DWORD	m_dwProperty;				// IME �Ӽ��Դϴ�.
	UINT	m_nCodePage;				// �ڵ� �������Դϴ�.
	BYTE	m_nCharSet;					// ���� �����Դϴ�.
	HFONT	m_hFont;					// �۲��Դϴ�.
	BOOL	m_fShowInvert;				// true�̸� ���ÿ��� ���� ���ڿ��� ǥ���մϴ�. 
	BOOL	m_fStat;					// true�̸� IME �ռ� ���ڿ��Դϴ�.
	BYTE	m_bComAttr[BUFFERSIZE+1];	// �������� ���ڿ��� Ư���Դϴ�.
	DWORD	m_dwCompCls[BUFFERSIZE+1];	// ���������Դϴ�. 
	wchar_t	m_szBuffer[BUFFERSIZE+1];	// �Է� ���ڿ��� �����Դϴ�.
	wchar_t	m_szComStr[BUFFERSIZE+1];	// �������� ���ڿ��� �����Դϴ�.
	wchar_t	m_szBackup[BUFFERSIZE+1];	// ������ ���ڿ� ����� ���� �����Դϴ�.

	char	m_szBufferMbs[BUFFERSIZE*2+1];
	BOOL	m_bKeyUse;					// Ű���� ��뿩��
	bool	m_bWideCaret;

public:
	wchar_t m_szWFontName[50];			// �۲� �̸�(�����ڵ�)�Դϴ�.
	char	m_szMBFontName[50];			// �۲� �̸�(��Ƽ����Ʈ)�Դϴ�.
	int		m_nLanguage;				// ����� ���� �ε����Դϴ�.
	BOOL	m_fIsNT;					// true�̸� NT�̰�, �׷��� ������ Win9X�Դϴ�.
	BOOL	m_fIsXP;					// true�̸� Windows XP �̻� �����Դϴ�.

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