#pragma once

#include <list>
#include <map>
#include <string>

#include "bytestream.h"
#include "SerialFile.h"
#include "DxEffChar.h"
#include "DxSkinChar.h"

class DxEffCharData
{
public:
	struct SEFFDATA
	{
		CByteStream	m_byteStream;

		SEFFDATA ()
		{
		}
	};

private:
	typedef std::list<SEFFDATA*>			EFFDATALIST;
	typedef std::list<SEFFDATA*>::iterator	EFFDATALIST_ITER;

protected:
	std::string		m_strFileName;
	EMPIECECHAR		m_emPieceType;
	EFFDATALIST		m_listEffData;

public:
	EMPIECECHAR GetPieceType ()								{ return m_emPieceType; }
	void SetFileName ( std::string strFileName )			{ m_strFileName = strFileName; }

public:
	BOOL SetPart ( DxCharPart *pCharPart, LPDIRECT3DDEVICEQ	pd3dDevice, D3DXVECTOR3 *pDir=NULL, float fTarDir=0.0f );
	BOOL SetPiece ( DxSkinPiece *pSkinPiece, LPDIRECT3DDEVICEQ	pd3dDevice, D3DXVECTOR3 *pDir=NULL, float fTarDir=0.0f );

public:
	void ConvertTPoint( LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL Import ( EMPIECECHAR _emType, GLEFFCHAR_VEC &vecEFFECT );
	void CleanUp ();

public:
	BOOL LoadFile ( basestream &SFile );
	BOOL SaveFile ( CSerialFile &SFile );

	BOOL LoadFile ( const char* szFile );
	BOOL SaveFile ( const char* szFile );

public:
	DxEffCharData(void);
	~DxEffCharData(void);
};
typedef DxEffCharData* PEFFCHARDATA;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


class DxEffCharDataArray
{
protected:
	std::string		m_strFileName;
	PEFFCHARDATA	m_pEffCharDataArray[PIECE_SIZE];

public:
	BOOL SetSkinPart ( PDXCHARPART pCharPart, LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 *pDir=NULL, float fTarDir=0.0f );
	BOOL SetSkinChar ( DxSkinChar *pSkinChar, LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 *pDir=NULL, float fTarDir=0.0f );
	BOOL SetSkinCharData ( DxSkinCharData *pSkinCharData, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void ConvertTPoint( LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL Import ( DxSkinCharData *pSkinCharData );
	void CleanUp ();

public:
	BOOL LoadFile ( const char* szFile );
	BOOL SaveFile ( const char* szFile );

public:
	DxEffCharDataArray(void)
	{
		memset ( m_pEffCharDataArray, 0x00, sizeof(PEFFCHARDATA)*PIECE_SIZE );
	}

	~DxEffCharDataArray(void)
	{
		CleanUp ();
	}
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

class DxEffcharDataMan
{
protected:
	std::string			m_strPath;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	void SetPath ( char *szPath )	{ m_strPath = szPath; }
	const char* GetPath ()			{ return m_strPath.c_str(); }
	
private:
	typedef std::map<std::string,DxEffCharData*>				EFFDATA_MAP;
	typedef std::map<std::string,DxEffCharData*>::iterator		EFFDATA_MAP_ITER;

	typedef std::map<std::string,DxEffCharDataArray*>			EFFDATA_ARRAYMAP;
	typedef std::map<std::string,DxEffCharDataArray*>::iterator	EFFDATA_ARRAYMAP_ITER;

protected:
	EFFDATA_MAP			m_mapEffData;
	EFFDATA_ARRAYMAP	m_mapEffDataArray;

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	BOOL PutEffect ( DxCharPart *pCharPart, const char* szEffFile, D3DXVECTOR3 *pDir, float fTarDir=0.0f );
	BOOL PutEffect ( DxSkinChar *pSkinChar, const char* szEffFile, D3DXVECTOR3 *pDir, float fTarDir=0.0f );

	BOOL PutPassiveEffect ( DxSkinChar *pSkinChar, const char* szEffFile, D3DXVECTOR3 *pDir, float fTarDir=0.0f );
	void OutEffect ( DxSkinChar *pSkinChar, std::string strEffFile );

	void CleanUp ();

private:
	DxEffcharDataMan ()
	{
	}

public:
	~DxEffcharDataMan ()
	{
		CleanUp ();
	}

public:
	static DxEffcharDataMan& GetInstance();
};


