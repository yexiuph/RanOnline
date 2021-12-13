#pragma once

class CD3DFontPar;

// 열거형 언어를 선언합니다.
enum LANGFLAG           
{
	DEFAULT				= 0,
	TRADITIONAL_CHINESE	= 1,
	JAPANESE			= 2,
	KOREAN				= 3,
	SIMPLIFIED_CHINESE	= 4,
	THAILAND			= 5,
	VIETNAM				= 6,

	LANG_NUM			= 7
};

extern char _DEFAULT_FONT[128];
extern DWORD _DEFAULT_FONT_FLAG;
extern DWORD _DEFAULT_FONT_SHADOW_FLAG;
extern DWORD _DEFAULT_FONT_SHADOW_EX_FLAG;

#define STRING_NUM80	80

class CD3DFontPar
{
public:
	static int			nCodePage[LANG_NUM];
	static int			nCharSet[LANG_NUM];
	static char			szDefaultFontName[LANG_NUM][50];
	static TCHAR		szEnglishFontName[LANG_NUM][20];

	CD3DFontPar*		pNextFont;

protected:
	int					m_nReference;

	LANGFLAG			m_emLangFlag;
	TCHAR				m_strFontName[STRING_NUM80];
	DWORD				m_dwFontHeight;
	DWORD				m_dwFontFlags;

	BOOL				m_bUsageCS;
	CRITICAL_SECTION	m_csLock;

public:
	static int GetCodePage ( int nLang );

public:
	int		AddRef()					{ return ++m_nReference; }
	int		SubRef()					{ return --m_nReference; }

	TCHAR*	GetFontName()				{ return m_strFontName; }
	DWORD	GetFontHeight()				{ return m_dwFontHeight; }
	DWORD	GetFontFlags()				{ return m_dwFontFlags; }

	VOID	UsageCS( BOOL bUsageCS )	{ m_bUsageCS = bUsageCS; }

	inline VOID AddFlags( DWORD dwFlags )	{ m_dwFontFlags |= dwFlags; }
	inline VOID SubFlags( DWORD dwFlags )	{ m_dwFontFlags &= ~dwFlags; }

public:
	virtual INT		MultiToWide( const TCHAR* strText, INT iText, WCHAR* wstrText, INT iWText );
	virtual INT		WideToMulti( const WCHAR* wstrText, INT iWText, TCHAR* strText, INT iText );

	virtual VOID	PushText( const TCHAR* strText )							{}
	virtual HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor,
		const TCHAR* strText, DWORD dwFlags = 0L, BOOL bImmediately = TRUE )	{ return S_OK; }
	virtual HRESULT GetTextExtent( const TCHAR* strText, SIZE &Size )			{ return S_OK; }

	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )			{ return S_OK; }
	virtual HRESULT RestoreDeviceObjects()										{ return S_OK; }
	virtual HRESULT InvalidateDeviceObjects()									{ return S_OK; }
	virtual HRESULT DeleteDeviceObjects()										{ return S_OK; }

public:
	virtual HRESULT Create( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual HRESULT CleanUp() = 0;

public:
	CD3DFontPar()
		: pNextFont( NULL )
		, m_bUsageCS( FALSE )
		, m_emLangFlag( DEFAULT )
		, m_dwFontHeight( 0 )
		, m_dwFontFlags( 0 )
		, m_nReference( 0 )
	{
		memset( m_strFontName, 0, sizeof(TCHAR)*STRING_NUM80 );

		InitializeCriticalSection( &m_csLock );
	}
	virtual ~CD3DFontPar()
	{
		DeleteCriticalSection( &m_csLock );

		SAFE_DELETE( pNextFont );
	}
};

class DxFontMan
{
protected:
	enum EMFONTSYS { EMFONT_D3D, EMFONT_D3DX };

public:
	static LPD3DXSPRITE	g_pSprite;

protected:
	std::string			m_strPATH;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	CD3DFontPar*		m_pFontListHead;

	EMFONTSYS			m_emFontSys;
	DWORD				m_dwBufferMode;

	LANGFLAG			m_emLang;

protected:
	DxFontMan(void);
	virtual ~DxFontMan(void);

protected:
	inline BOOL IsSameFont( CD3DFontPar *pD3dFontSrc, TCHAR* szFontName, DWORD dwHeight, DWORD dwFlags );

public:
	CD3DFontPar* LoadDxFont( TCHAR* szFontName, DWORD dwHeight, DWORD dwFlags=0L );
	void DeleteDxFont( TCHAR* szFontName, DWORD dwHeight, DWORD dwFlags=0L );
	void DeleteDxFontAll();

	CD3DFontPar* FindFont( TCHAR* szFontName, DWORD dwHeight, DWORD dwFlags );

	void AddRef( CD3DFontPar* pD3dFont );
	void ReleaseDxFont( TCHAR* szFontName, DWORD dwHeight, DWORD dwFlags=0L );
	void ReleaseDxFont( CD3DFontPar *pD3dFont );

public:
	HRESULT SetFontSys( CString strFontSys, DWORD dwLang );
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

public:
	void SetPath( const char *szPath )	{ m_strPATH = szPath; }
	const char* GetPath()				{ return m_strPATH.c_str(); }
	int GetCodePage()					{ return CD3DFontPar::GetCodePage(m_emLang); }
	bool IsXFont()						{ return ( m_emFontSys == EMFONT_D3DX ); }

public:
	static DxFontMan& GetInstance();
};

namespace COMMENT
{
	extern std::string FONTSYS[2];
};