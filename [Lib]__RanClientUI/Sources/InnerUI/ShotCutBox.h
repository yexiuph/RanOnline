
#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CDCFontPar;
class CBasicCarrat;

#define MAX_NOTSHOTKUT_KEY 23

class CShotCutBox : public CUIGroup
{
	static const int	nDEFAULT_LIMIT_INPUT;
	static const float	fLINE_END_GAP;

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
	D3DCOLOR			m_dwTextColor;
	bool				m_bVALID_INPUT;

protected:
	BOOL				m_bKeyInput;

protected:
	int					m_xViewPos;
	int					m_xInsertPos;
	int					m_xInsertPosR;
	float				m_fCarratPos;
	float				m_fCaretSizeX;
	
	BYTE				m_NotSetKey[MAX_NOTSHOTKUT_KEY];

public:
	CShotCutBox ();
	virtual	~CShotCutBox ();

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

	void SetImpossableKey();

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

	bool IsMODE_NATIVE()					{ return m_bModeNative; }
	void SetMODE_NATIVE( bool bNative )		{ m_bModeNative = bNative; }

	const CString& GetEditString() const	{ return m_strText; }
	int GetEditLength()						{ return m_strText.GetLength (); }

	void SetEditString ( const CString& strInput );
public:
	void SetTextColor( const D3DCOLOR& dwTextColor )	{ m_dwTextColor = dwTextColor; }
	const D3DCOLOR&	GetTextColor () const	{ return m_dwTextColor; }
};
