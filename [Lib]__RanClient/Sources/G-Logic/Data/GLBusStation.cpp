#include "pch.h"
#include "./GLBusStation.h"
#include "./GLogicData.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"
#include "../[Lib]__Engine/Sources/Common/IniLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLBusStation& GLBusStation::GetInstance ()
{
	static GLBusStation _instance;
	return _instance;
}

GLBusStation::GLBusStation(void)
{
}

GLBusStation::~GLBusStation(void)
{
}

void GLBusStation::insert ( SSTATION &sSTATION )
{
	//	Note : 정차역을 등록.
	//
	m_vecSTATION.push_back ( sSTATION );
}

bool GLBusStation::LOAD ( std::string strFILE )
{
	//	Note : 이전 정보 삭제.
	m_vecSTATION.clear ();

	std::string strLIST_FILE = m_strPATH + strFILE;

	CIniLoader cFILE;

	if( GLOGIC::bGLOGIC_ZIPFILE )
		cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	std::string strSep( ",[]\t" );
	cFILE.reg_sep( strSep );

	if( !cFILE.open( strLIST_FILE, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile ( "ERROR : GLBusStation::LOAD(), File Open %s", strFILE.c_str() );
		return false;
	}

	INT nStationKeySize(0);

	cFILE.getflag( "STATION_LIST_INFO", "StationKeySize", 0, 1, nStationKeySize );

	DWORD dwNUM = cFILE.GetKeySize( "STATION_LIST", "STATION" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		bool bUsed(false);
		cFILE.getflag( i, "STATION_LIST", "STATION", 0, nStationKeySize, bUsed );

		if( !bUsed ) continue;

		SNATIVEID sMAPID;
		SSTATION sSTATION;

		cFILE.getflag( i, "STATION_LIST", "STATION", 1, nStationKeySize, sSTATION.dwLINKID );

		cFILE.getflag( i, "STATION_LIST", "STATION", 2, nStationKeySize, sMAPID.wMainID );
		cFILE.getflag( i, "STATION_LIST", "STATION", 3, nStationKeySize, sMAPID.wSubID );
		sSTATION.dwMAPID = sMAPID.dwID;

		cFILE.getflag( i, "STATION_LIST", "STATION", 4, nStationKeySize, sSTATION.dwGATEID );
		cFILE.getflag( i, "STATION_LIST", "STATION", 5, nStationKeySize, sSTATION.strMAP );
		cFILE.getflag( i, "STATION_LIST", "STATION", 6, nStationKeySize, sSTATION.strSTATION );

		insert( sSTATION );
	}

	return true;
}

SSTATION* GLBusStation::GetStation ( DWORD dwID )
{
	int nSize = (int) m_vecSTATION.size();

	for( int i=0; i<nSize; ++i )
	{
		if( m_vecSTATION[i].dwLINKID == dwID )
		{
			return &m_vecSTATION[i];
		}
	}

	return NULL;
}