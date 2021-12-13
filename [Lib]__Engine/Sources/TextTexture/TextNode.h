#pragma once

#include "../[Lib]__EngineUI/Sources/UIDataType.h"

class CD3DFontX;

/**
 * \class	CTextPart
 * \date	2005/10/06
 * \author	Jun-Hyuk Choi
 */
class CTextPart
{
public:
	BOOL		m_bValid;
	INT			m_iLength;
	TCHAR*		m_strText;
	INT			m_iWidth;

	BOOL		m_bTemp;
	BOOL		m_bDelete;
	BOOL		m_bCreate;

	RECT		m_rtTBound;
	UIVERTEX	m_stVertex[ 4 ];

	CD3DFontX*	m_pFont;

public:
	CTextPart*	m_pLeft;
	CTextPart*	m_pRight;

public:
	CTextPart();
	virtual ~CTextPart();

	VOID CreateText( const TCHAR* strText, CD3DFontX* pFont );
	VOID DrawText( HDC hDC, DWORD dwFlags );
	VOID DeleteText();
	VOID SetTextureUV();

	// 렌더큐 이용해서 출력
	VOID DrawText( LPDIRECT3DTEXTUREQ pTexture, D3DCOLOR dwColor, FLOAT& fX, FLOAT fY );

	// 디바이스로 직접 출력
	VOID DrawText( D3DCOLOR dwColor, FLOAT& fX, FLOAT fY );

	DWORD GetFontFlags();
};