#pragma once

#include <list>
#include <map>

class CD3DFontX;
class CTextGroup;
class CTextTexture;

/**
 * \class	CTextUtil
 * \date	2005/10/06
 * \author	Jun-Hyuk Choi
 */
class CTextUtil
{
protected:
	typedef std::list< CTextGroup* >	GROUP_LIST;

	typedef std::multimap< INT, CTextTexture* >		TEXTURE_MAP;
	typedef TEXTURE_MAP::iterator					TEXTURE_ITOR;
	typedef std::pair< TEXTURE_ITOR, TEXTURE_ITOR >	TEXTURE_RANGE;

public:
	static BOOL				m_bUsage;
	static BOOL				m_bUsageRenderQueue;
	static BOOL				m_bBlurFilter;
	static INT				m_iSpaceSize;
	static INT				m_iSpaceSizeHalf;

protected:
	static CTextUtil*		m_cpTextUtil;

	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	LPDIRECT3DSTATEBLOCK9	m_pTextureSB;

	INT						m_iTextureSizeSmall;
	INT						m_iTextureSizeBig;

	DWORD					m_dwLastTick;

	GROUP_LIST				m_listGroup;
	TEXTURE_MAP				m_mmapTexture;

public:
	CTextUtil();
	virtual ~CTextUtil();

	VOID OneTimeSceneInit();
	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice, SIZE* sizeTexture );
	VOID RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID FrameMove();
	VOID Render( BOOL bPrev );
	VOID InvalidateDeviceObjects();
	VOID DeleteDeviceObjects();

	VOID ClearData();

	VOID PushText( const TCHAR* strText, CD3DFontX* pFont );
	BOOL DrawText( const TCHAR* strText, CD3DFontX* pFont, D3DCOLOR dwColor, FLOAT fX, FLOAT fY );

	CTextGroup* CreateTextGroup( CD3DFontX* pFont );
	CTextGroup* GetTextGroup( CD3DFontX* pFont );

	CTextTexture* CreateTextTexture( INT iHeight );

	BOOL IsTextureBig( INT iFontSize );
	INT GetTextureSize( INT iFontSize );
	VOID SaveTextureToFile( TCHAR* pszPath );

public:
	LPDIRECT3DDEVICEQ GetDevice()	{ return m_pd3dDevice; }
	DWORD GetLastTick()				{ return m_dwLastTick; }

public:
	static CTextUtil* Get();
	static VOID CleanUp();
};