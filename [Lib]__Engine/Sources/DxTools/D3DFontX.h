#pragma once

#include "DxFontMan.h"

/**
 * \class	CD3DFontX
 * \date	2005/09/26
 * \author	Jun-Hyuk Choi
 */
class CD3DFontX : public CD3DFontPar
{
public:
	enum
	{
		EM_DC_SHADOW	= 0x01,
		EM_DC_OUTLINE	= 0x02,
		EM_DC_TEXT		= 0x04,

		EM_DC_EFF		= EM_DC_SHADOW | EM_DC_OUTLINE,
		EM_DC_ALL		= EM_DC_EFF | EM_DC_TEXT
	};

protected:
	// 공통
	static BOOL			m_bWindows98;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	INT					m_iHeightScreen;
	INT					m_iOutLine;

	// D3DFont 버전
	LPD3DXFONT			m_pd3dxFont;
	HDC					m_hd3dxDC;

	// HFont 버전
	HDC					m_hDC;
	HFONT				m_hFont;
	HFONT				m_hFontOld;

public:
	// 공통
	CD3DFontX( TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags = 0L, LANGFLAG emLang = DEFAULT );
	~CD3DFontX();

public:
	// 공통
	HRESULT Create( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT CleanUp();

public:
	// 공통
	HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags = 0L, BOOL bImmediately = TRUE );
	HRESULT GetTextExtent( const TCHAR* strText, SIZE &Size );
	VOID ConvWideAndTextExtent( const TCHAR* strText, INT iText, WCHAR* wstrText, INT &iWText, SIZE &Size, BOOL bConv );
	VOID ConvWideAndTextExtent98( const TCHAR* strText, INT iText, WCHAR* wstrText, INT &iWText, SIZE &Size, BOOL bConv );
	INT GetHeightScreen();

	// D3DFont 버전 ( 화면에 즉시 출력 )
	HRESULT DrawText( const TCHAR* strText, DWORD dwColor, DWORD dwFlags, FLOAT x, FLOAT y );

	// HFont 버전 ( 현 프레임에서 텍스쳐에 출력(Push), 다음 프레임에서 화면에 출력(Draw) )
	VOID PushText( const TCHAR* strText );
	VOID DrawText( const TCHAR* strText, DWORD dwFlags, RECT* rtBound, HDC hDC );

	VOID CheckVietnam( const TCHAR* strText, TCHAR* newStrText );
	BOOL CheckCharSunjo( TCHAR chText );
	BOOL CheckCharExAlphabet( TCHAR chText );
	BOOL CheckCharMoum( TCHAR chText );
};