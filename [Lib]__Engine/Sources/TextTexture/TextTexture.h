#pragma once

#include <list>

class CD3DFontX;
class CTextObject;
class CTextPart;

/**
 * \class	CTextTexture
 * \date	2005/10/06
 * \author	Jun-Hyuk Choi
 */
class CTextTexture
{
protected:
	typedef std::list< CTextPart* >	PART_LIST;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	INT					m_iHeight;
	INT					m_iTextureSize;

	LPDIRECT3DTEXTUREQ	m_pTexture;
	VOID*				m_pTexData;
	DWORD				m_dwTexSize;

	HDC					m_hDC;
	HBITMAP				m_hBitmap;
	HBITMAP				m_hBitmapOld;
	DWORD*				m_pBits;

	INT					m_iLine;
	INT*				m_pWidth;
	INT*				m_pWidthTemp;
	INT					m_iTopWidth;
	PART_LIST*			m_pListPart;

	BOOL				m_bDelete;
	BOOL				m_bCreate;
	BOOL				m_bCopy;

public:
	CTextTexture();
	virtual ~CTextTexture();

public:
	virtual BOOL InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID FrameMove();
	VOID InvalidateDeviceObjects();
	virtual VOID DeleteDeviceObjects();

public:
	BOOL MarkTextPart( CTextObject* pObject );
	VOID CheckTopLength( BOOL& bSucceed, CTextPart* pPart );
	VOID MarkLength( BOOL& bSucceed, CTextPart* pPart );

	BOOL IsEmpty();

public:
	virtual VOID CalcDelete() = 0;
	virtual VOID CalcTexData( CTextPart* pPart ) = 0;
	virtual VOID FilterTexture( CTextPart* pPart ) = 0;
	VOID CalcCreate();
	VOID CopyTexture();

public:
	VOID SetTexture();

public:
	VOID SaveTextureToFile( const TCHAR* pszPath, D3DXIMAGE_FILEFORMAT eType = D3DXIFF_BMP );

public:
	VOID SetHeight( INT iHeight )	{ m_iHeight = iHeight; }

	VOID SetDelete()				{ m_bDelete = TRUE; }

public:
	INT GetHeight()					{ return m_iHeight; }

	LPDIRECT3DTEXTUREQ GetTexture()	{ return m_pTexture; }
};