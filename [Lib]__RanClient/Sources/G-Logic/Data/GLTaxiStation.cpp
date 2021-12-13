#include "pch.h"
#include "./GLTaxiStation.h"
#include "./GLogicData.h"

#include "../[Lib]__Engine/Sources/Common/IniLoader.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLTaxiStation& GLTaxiStation::GetInstance ()
{
	static GLTaxiStation _instance;
	return _instance;
}

GLTaxiStation::GLTaxiStation(void)
: m_dwBasicCharge ( 0 )
{
}

GLTaxiStation::~GLTaxiStation(void)
{
}



void GLTaxiStation::insert ( STAXI_STATION &sSTATION )
{
	//	Note : 기존에 등록된 맵 목록이 있는지 검사
	//
	DWORD dwSIZE = (DWORD) m_vecTaxiMap.size();
	for ( DWORD i=0; i<dwSIZE; ++i )
	{
		STAXI_MAP &sTaxiMap = m_vecTaxiMap[i];
		if ( sTaxiMap.dwMAPID == sSTATION.dwMAPID )
		{
			//	Note : 기존 마을에 등록.
			//
			sTaxiMap.vecTaxiStation.push_back ( sSTATION );
			return;
		}
	}

	//	Note : 새로운 마을 등록.
	//
	STAXI_MAP sTaxiMap_NEW;
	sTaxiMap_NEW.dwMAPID = sSTATION.dwMAPID;
	sTaxiMap_NEW.strMAP = sSTATION.strMAP;
	sTaxiMap_NEW.vecTaxiStation.push_back ( sSTATION );
	m_vecTaxiMap.push_back ( sTaxiMap_NEW );

	return;
}

bool GLTaxiStation::LOAD ( std::string strFILE )
{
	//	Note : 이전 정보 삭제.
	m_vecTaxiMap.clear ();

	std::string strLIST_FILE = m_strPATH + strFILE;

	CIniLoader cFILE;

	if( GLOGIC::bGLOGIC_ZIPFILE )
		cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	std::string strSep( ",[]\t" );
	cFILE.reg_sep( strSep );

	if( !cFILE.open( strLIST_FILE, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile ( "ERROR : GLTaxiStation::LOAD(), File Open %s", strFILE.c_str() );
		return false;
	}

	INT nStationKeySize(0);

	cFILE.getflag( "STATION_LIST_INFO", "StationKeySize", 0, 1, nStationKeySize );
	cFILE.getflag( "STATION_LIST_INFO", "StationBasicCharge", 0, 1, m_dwBasicCharge );
	

	DWORD dwNUM = cFILE.GetKeySize( "STATION_LIST", "STATION" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		bool bUsed(false);
		cFILE.getflag( i, "STATION_LIST", "STATION", 0, nStationKeySize, bUsed );

		if( !bUsed ) continue;

		SNATIVEID sMAPID;
		SNATIVEID sNPCID;
		STAXI_STATION sSTATION;

		cFILE.getflag( i, "STATION_LIST", "STATION", 1, nStationKeySize, sSTATION.dwLINKID );

		cFILE.getflag( i, "STATION_LIST", "STATION", 2, nStationKeySize, sMAPID.wMainID );
		cFILE.getflag( i, "STATION_LIST", "STATION", 3, nStationKeySize, sMAPID.wSubID );
		sSTATION.dwMAPID = sMAPID.dwID;

		cFILE.getflag( i, "STATION_LIST", "STATION", 4, nStationKeySize, sSTATION.wPosX );
		cFILE.getflag( i, "STATION_LIST", "STATION", 5, nStationKeySize, sSTATION.wPosY );

		cFILE.getflag( i, "STATION_LIST", "STATION", 6, nStationKeySize, sNPCID.wMainID );
		cFILE.getflag( i, "STATION_LIST", "STATION", 7, nStationKeySize, sNPCID.wSubID );
		sSTATION.dwNPCID = sNPCID.dwID;

		cFILE.getflag( i, "STATION_LIST", "STATION", 8, nStationKeySize, sSTATION.dwMapCharge );

		cFILE.getflag( i, "STATION_LIST", "STATION", 9, nStationKeySize, sSTATION.strMAP );
		cFILE.getflag( i, "STATION_LIST", "STATION", 10, nStationKeySize, sSTATION.strSTATION );

		insert( sSTATION );
	}

	return true;
}

DWORD GLTaxiStation::GetTaxiMapNum ()
{
	return (DWORD)m_vecTaxiMap.size();
}

STAXI_MAP* GLTaxiStation::GetTaxiMap ( int nIndex )
{
	if ( (int)m_vecTaxiMap.size() <= nIndex )	
		return NULL;

	return &m_vecTaxiMap[nIndex];
}

STAXI_STATION* GLTaxiStation::GetStation ( int nMapIndex, int nStationIndex )
{
	STAXI_MAP* pTaxiMap = GetTaxiMap( nMapIndex );
	
	if ( !pTaxiMap ) return NULL;

	if ( (int)pTaxiMap->vecTaxiStation.size() <= nStationIndex )	
		return NULL;

	return &pTaxiMap->vecTaxiStation[nStationIndex];
}
