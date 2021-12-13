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
	// ����
	static BOOL			m_bWindows98;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	INT					m_iHeightScreen;
	INT					m_iOutLine;

	// D3DFont ����
	LPD3DXFONT			m_pd3dxFont;
	HDC					m_hd3dxDC;

	// HFont ����
	HDC					m_hDC;
	HFONT				m_hFont;
	HFONT				m_hFontOld;

public:
	// ����
	CD3DFontX( TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags = 0L, LANGFLAG emLang = DEFAULT );
	~CD3DFontX();

public:
	// ����
	HRESULT Create( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT CleanUp();

public:
	// ����
	HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags = 0L, BOOL bImmediately = TRUE );
	HRESULT GetTextExtent( const TCHAR* strText, SIZE &Size );
	VOID ConvWideAndTextExtent( const TCHAR* strText, INT iText, WCHAR* wstrText, INT &iWText, SIZE &Size, BOOL bConv );
	VOID ConvWideAndTextExtent98( const TCHAR* strText, INT iText, WCHAR* wstrText, INT &iWText, SIZE &Size, BOOL bConv );
	INT GetHeightScreen();

	// D3DFont ���� ( ȭ�鿡 ��� ��� )
	HRESULT DrawText( const TCHAR* strText, DWORD dwColor, DWORD dwFlags, FLOAT x, FLOAT y );

	// HFont ���� ( �� �����ӿ��� �ؽ��Ŀ� ���(Push), ���� �����ӿ��� ȭ�鿡 ���(Draw) )
	VOID PushText( const TCHAR* strText );
	VOID DrawText( const TCHAR* strText, DWORD dwFlags, RECT* rtBound, HDC hDC );

	VOID CheckVietnam( const TCHAR* strText, TCHAR* newStrText );
	BOOL CheckCharSunjo( TCHAR chText );
	BOOL CheckCharExAlphabet( TCHAR chText );
	BOOL CheckCharMoum( TCHAR chText );
};