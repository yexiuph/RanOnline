#pragma once
#include <string>
#include "./CList.h"
#include "DxSkinCharData.h"

class DxSkinDataDummy
{
	typedef std::vector<SANIMCONINFO*>	VECANIMCONINFO;

public:
	std::string				m_strFileName;

	float					m_fScale;
	std::string				m_strPIECE[PIECE_SIZE];
	std::string				m_strSkeleton;

	VECANIMCONINFO			m_vecANIMINFO;
	D3DXVECTOR3				m_vMax, m_vMin;

protected:
	BOOL LOAD_0100 ( basestream &SFile );
	BOOL LOAD_0101 ( basestream &SFile );

public:
	BOOL LoadFile ( const char* szFile );
	void CleanUp ();

public:
	SANIMCONINFO* FindAniInfo ( const char* szName );

public:
	DxSkinDataDummy(void);
	~DxSkinDataDummy(void);
};
