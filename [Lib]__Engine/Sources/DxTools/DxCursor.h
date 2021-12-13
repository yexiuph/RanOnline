#pragma once

enum	CURSOR_TYPE
{
	EMCS_NORMAL = 0,
	EMCS_ATTACK = 1,
	EMCS_SELECT = 2,
	EMCS_SCROLL = 3,
	EMCS_TALK	= 4,

	EMCS_SIZE	= 5,
};
class DxCursor
{
private:
	HWND	m_hWnd;
	float	m_fTime;
	DWORD	m_dwCurIndex;

private:
	struct TexData
	{
		DWORD		m_dwIndexX;
		DWORD		m_dwIndexY;
		D3DXVECTOR2	m_vTexUV;
	};

	struct ANICURSOR
	{
		BOOL		m_bAni;
		DWORD		m_dwSize;
		TexData*	m_pTexData;

		ANICURSOR();
		~ANICURSOR();
	};

private:
	CURSOR_TYPE	m_emCursorType;
	ANICURSOR	m_sAniCursor[EMCS_SIZE];
	const DWORD m_dwWidth;
	const DWORD m_dwHeight;
	const DWORD m_dwGridW;
	const DWORD m_dwTexSizeW;
	const DWORD m_dwTexSizeH;
	D3DXVECTOR2 m_vTexSizeUV;

private:
	BOOL	m_bWindowCursor;

private:
	LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	LPDIRECT3DSTATEBLOCK9	m_pDrawSB;
	LPDIRECT3DTEXTUREQ		m_pTexture;

private:
	D3DXVECTOR2 GetTexUV( DWORD i, DWORD j );

public:
	void SetCursorType( CURSOR_TYPE emCursor );
	BOOL IsWindowCursor()						{ return m_bWindowCursor; }

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice, HWND hWnd );
	void OnDestroyDevice();
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

public:
	void FrameMove( const float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxCursor();
	~DxCursor();

public:
	static DxCursor& GetInstance();
};
