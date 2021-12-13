#pragma once

#include <map>
#include "./DxBoneCollector.h"
#include "./SerialFile.h"

//------------------------------------------------------------------------------------------------------------
//						D	x		A	n	i		S	c	a	l	e		M	A	I	N
//------------------------------------------------------------------------------------------------------------
struct DxAniScaleMAIN
{
	typedef std::map<DWORD,float>		MAP_ANISCALEDATA;		//  KEY, SCALE
	typedef MAP_ANISCALEDATA::iterator	MAP_ANISCALEDATA_ITER;	//  KEY, SCALE

	MAP_ANISCALEDATA		m_mapAniScaleData;

	void UpdateBoneScale( DxBoneTrans* pBoneTrans, float fWeight, BOOL bFirst, float fTime, float fSTime, float fETime, float UnitTime );

	void InsertData( DWORD dwKey, float fScale );
	void ModifyData( int nIndex, float fScale );
	void DeleteData( int nIndex );

	void Save( CSerialFile& SFile );
	void Load( basestream& SFile );

	DxAniScaleMAIN& operator = ( DxAniScaleMAIN &value );
};

//------------------------------------------------------------------------------------------------------------
//								D	x		A	n	i		S	c	a	l	e
//------------------------------------------------------------------------------------------------------------
class DxAniScale
{
public:
	typedef std::map<TSTRING,DxAniScaleMAIN*>	MAP_ANISCALEMAIN;
	typedef MAP_ANISCALEMAIN::iterator			MAP_ANISCALEMAIN_ITER;

	MAP_ANISCALEMAIN	m_mapAniScaleMain;

public:
	void UpdateBoneScale( DxSkeleton* pSkeleton, float fWeight, BOOL bFirst, float fTime, float fSTime, float fETime, float UnitTime );

public:
	void InsertAniScale( const TCHAR* pName );
	void DeleteAniScale( int nIndex );

	DxAniScaleMAIN* GetAniScaleMAIN( int nIndex );

public:
	void CleanUp();

	void Save( CSerialFile& SFile );
	void Load( basestream& SFile );

	DxAniScale& operator = ( DxAniScale &value );

public:
	DxAniScale();
	~DxAniScale();
};