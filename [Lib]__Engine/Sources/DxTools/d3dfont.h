//-----------------------------------------------------------------------------
// File: D3DFont.h
//
// Desc: Texture-based font class
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//
//	Note : 한글이 지원되게 변경된 것이다...
//	Note : pNextFont 변수가 추가 되었다... 리스트를 만들수 있도록.
//
//-----------------------------------------------------------------------------
#ifndef D3DFONT_H
#define D3DFONT_H
#include <tchar.h>

#include <vector>
#include <map>

#include "DxFontMan.h"

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_SHADOW      0x0004
#define D3DFONT_SHADOW_EX	0x0008

#define D3DFONT_ASCII		0x0010	//	아스키 코드 영역. ( 숫자, 영어, 일반기호 )
#define D3DFONT_KSC5601		0x0020	//	표준 완성형.
#define D3DFONT_KS10646		0x0040	//	확장 완성형.
#define D3DFONT_TCBIGFIVE	0x0080	//	대만 번체 BIGFIVE ( 13494 문자 )
#define D3DFONT_SCGB		0x0100	//	중국 간체 GB
#define D3DFONT_SHIFTJIS	0x0200	//	일본 Shift-JIS
#define D3DFONT_THAI		0x0400	//	태국
#define D3DFONT_VIETNAM		0x0800	//	베트남

#define D3DFONT_BLUR		0x1000	//	D3DFontX 테두리 필터링

// Font rendering flags
#define D3DFONT_CENTERED    0x0001
#define D3DFONT_TWOSIDED    0x0002
#define D3DFONT_FILTERED    0x0004


#define MAX_NUM_VERTICES	80*6

//-----------------------------------------------------------------------------
// Name: class CD3DFont
// Desc: Texture-based font class for doing text in a 3D scene.
//-----------------------------------------------------------------------------
class CD3DFont : public CD3DFontPar
{
public:
	class FONT2DVERTEX
	{
	public:
		enum { FVF = (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1), };

		D3DXVECTOR4	vec;
		DWORD		color;
		FLOAT		tu, tv;

		FONT2DVERTEX ( const D3DXVECTOR4& _vec, D3DCOLOR _color, FLOAT _tu, FLOAT _tv )
		{
			vec = _vec; color = _color; tu = _tu; tv = _tv;
		}

		FONT2DVERTEX() :
			vec(0,0,0,0),
			color(0),
			tu(0),
			tv(0)
		{
		}
	};

public:
	CD3DFont( TCHAR* strFontName, DWORD dwHeight, DWORD dwBufferMode, DWORD dwFlags=0L, LANGFLAG emLang=DEFAULT );
	~CD3DFont();

public:
	virtual HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags = 0L, BOOL bImmediately = TRUE );
	virtual HRESULT GetTextExtent( const TCHAR* strText, SIZE &Size );

protected:
	HRESULT CreateFont ();
	HRESULT ReleaseFont ();

public:
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

public:
	virtual HRESULT Create ( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )
			return E_FAIL;

		if ( FAILED(RestoreDeviceObjects()) )
			return E_FAIL;

		return S_OK;
	}

	virtual HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();

		DeleteDeviceObjects();

		return S_OK;
	}

private:
	LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	LPDIRECT3DSTATEBLOCK9	m_pDrawTextStateBlock;
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

public:
	struct STEXUV
	{
		short	ntexINDEX;

		short	nX1;
		short	nY1;
		short	nX2;
		short	nY2;

		STEXUV () :
			ntexINDEX(0),
			nX1(0),
			nY1(0),
			nX2(0),
			nY2(0)
		{
		}
	};
	
	enum { _MAXVERTEX = 3072 };
	typedef std::map<short,STEXUV>			MAPTEXUV;
	typedef MAPTEXUV::iterator				MAPTEXUV_ITER;

	typedef std::vector<LPDIRECT3DTEXTUREQ>	VECTEX;
	typedef VECTEX::iterator				VECTEX_ITER;

	typedef std::vector<DWORD>				VERTNUM;
	typedef VECTEX::iterator				VERTNUM_ITER;

	typedef std::vector<FONT2DVERTEX*>		ARRAYVERTEX;
	typedef ARRAYVERTEX::iterator			ARRAYVERTEX_ITER;

private:
	DWORD		m_dwBufferMode;
	DWORD		m_dwTexWidth;                 // Texture dimensions
	DWORD		m_dwTexHeight;
	short		m_ntexNum;

	float		m_fFontHeight;
	float		m_fFontBlinkWidth;

	MAPTEXUV	m_mapTEXUV;
	
	VECTEX		m_vecTEX;
	VERTNUM		m_vecVERTNUM;

private:
	ARRAYVERTEX	m_vecVERTEX;

private:
	void DoFontReady();

private:
	bool LoadFontCache ();
	bool SaveFontCache ();
};

#endif


