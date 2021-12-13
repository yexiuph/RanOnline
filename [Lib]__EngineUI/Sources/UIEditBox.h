#pragma	once

#include "UIGroup.h"

class CDCFontPar;
class CBasicCarrat;

enum
{
	EDIT_LEFT_ALIGN,
	EDIT_CENTER_ALIGN,
	EDIT_RIGHT_ALIGN
};

class CUIEditBox : public CUIGroup
{
	static const int	nDEFAULT_LIMIT_INPUT;
	static const float	fLINE_END_GAP;
	static const int	nNOT_USE_MONEY_FORMAT;

	static UIGUID		DEBUG_BEGIN_ID;
	static UIGUID		DEBUG_END_ID;

private:
	bool				m_bModeNative;
	bool				m_bBeginEdit;
	bool				m_bLimited;
	int					m_nLimitInput;

	CD3DFontPar*		m_pFont;
	CBasicCarrat*		m_pCarrat;
	DWORD				m_dwCarratDiffuse;

	CString				m_strText;
	CString				m_strRenderText;
	CString				m_strRenderTextR;
	D3DXVECTOR2			m_vRenderPos;

	int					m_nTabIndex;
	BOOL				m_bHide;
	bool				m_bUseNumber;
	int					m_nMoneyUNIT;
	CString				m_strUNIT_SYMBOL;
	int					m_nAlign;
	D3DCOLOR			m_dwTextColor;
	bool				m_bVALID_INPUT;
	bool				m_bWideCarret;

protected:
	BOOL				m_bMinusSign;
	BOOL				m_bKeyInput;

protected:
	int					m_xViewPos;
	int					m_xInsertPos;
	int					m_xInsertPosR;
	float				m_fCarratPos;
	float				m_fCaretSizeX;

public:
	CUIEditBox ();
	virtual	~CUIEditBox ();

public:
	void CreateCarrat ( char* szCarrat, BOOL bUseRender, DWORD dwDiffuse );
	void SetFont ( CD3DFontPar* pFont );

	virtual void SetGlobalPos ( const UIRECT& rcPos );
	virtual void SetGlobalPos ( const D3DXVECTOR2& vPos );

	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	void UpdateRenderText();
	void UpdatePosition ();

	void GetInsertPos();
	void SetInsertPos();

	int MultiToWidePos( CString* strMulti, int iMultiPos );
	int WideToMultiPos( CStringW* strWide, int iWidePos );

private:
	void FLUSH_STRING_BUFFER ();
	void SET_STRING_BUFFER ( const char* pStr );

public:
	bool BeginEdit();
	bool IsBegin()							{ return m_bBeginEdit; }
	bool EndEdit();
	void ClearEdit();
	bool IsLimited()						{ return m_bLimited; } // 자동 개행 ( 아직 구현 안됨 )
	bool IsEmpty()							{ return m_strText.IsEmpty(); }

public:
	bool IsVALID_INPUT()					{ return m_bVALID_INPUT; }

	void SetLimitInput( int nLimitInput )	{ m_nLimitInput = nLimitInput; }
	int	GetLimitInput()						{ return m_nLimitInput; }

	void DoMODE_TOGGLE();
	bool IsMODE_NATIVE()					{ return m_bModeNative; }
	void SetMODE_NATIVE( bool bNative )		{ m_bModeNative = bNative; }

	const CString& GetEditString() const	{ return m_strText; }
	int GetEditLength()						{ return m_strText.GetLength (); }

	void SetEditString ( const CString& strInput );

	void DisableKeyInput()					{ m_bKeyInput = FALSE; }
	void UsableKeyInput()					{ m_bKeyInput = TRUE; }

public:
	void SetMinusSign( BOOL bUse )			{ m_bMinusSign = bUse; }

	void SetTabIndex( int nTabIndex )		{ m_nTabIndex = nTabIndex; }
	int	GetTabIndex( void )					{ return m_nTabIndex; }

	void SetHide( BOOL bHide )				{ m_bHide = bHide; }
	BOOL IsHide()							{ return m_bHide; }

	void DoUSE_NUMBER( bool bUse )			{ m_bUseNumber = bUse; }
	bool IsUSE_NUMBER()						{ return m_bUseNumber; }

	bool SET_MONEY_FORMAT( int nMoneyUNIT, const CString& strUNIT_SYMBOL = "," );
	int	GET_MONEY_FORMAT()					{ return m_nMoneyUNIT; }
	CString	GET_MONEY_SYMBOL();

	void SetTextColor( const D3DCOLOR& dwTextColor )	{ m_dwTextColor = dwTextColor; }
	const D3DCOLOR&	GetTextColor () const	{ return m_dwTextColor; }

	void SetAlign( int nAlign )				{ m_nAlign = nAlign; }
	int GetAlign()							{ return m_nAlign; }
};