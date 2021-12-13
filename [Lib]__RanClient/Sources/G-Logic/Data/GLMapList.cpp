#include "pch.h"
#include "./GLMapList.h"
#include "./GLGaeaServer.h"

#include "../[Lib]__Engine/Sources/Common/IniLoader.h"
#include "../[Lib]__Engine/Sources/Common/StringUtils.h"
#include "../[Lib]__Engine/Sources/Common/BaseString.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SMAPNODE& SMAPNODE::operator = ( const SMAPNODE& value )
{
	if( this == &value )
		return *this;

	strFile = value.strFile;
	sNativeID = value.sNativeID;
	dwFieldSID = value.dwFieldSID;

	bPeaceZone	= value.bPeaceZone;
	bCommission = value.bCommission;
	bBattleZone = value.bBattleZone;
	bPKZone		= value.bPKZone;
	bFreePK		= value.bFreePK;
	bItemDrop	= value.bItemDrop;
	bMove		= value.bMove;
	bRestart	= value.bRestart;
	bPetActivity= value.bPetActivity;
	bDECEXP		= value.bDECEXP;
	bVehicleActivity = value.bVehicleActivity;
	bClubBattleZone = value.bClubBattleZone;
	bCDMZone	= value.bCDMZone;

	strMapName = value.strMapName;
	strBGM = value.strBGM;
	strLoadingImageName = value.strLoadingImageName;

	sLEVEL_REQUIRE  = value.sLEVEL_REQUIRE;
	sLEVEL_ETC_FUNC = value.sLEVEL_ETC_FUNC;
	bInstantMap	   = value.bInstantMap;

	return *this;
}

GLMapList::GLMapList(void)
{
}

GLMapList::~GLMapList(void)
{
}

BOOL GLMapList::IsRegMapNode ( SMAPNODE &MapCur )
{
	FIELDMAP_ITER iter = m_MapList.begin ();
	FIELDMAP_ITER iter_end = m_MapList.end ();

	for ( ; iter!=iter_end; ++iter )
	{
		const SMAPNODE &sMapNode = (*iter).second;

		if ( sMapNode.sNativeID==MapCur.sNativeID )		return TRUE;
		if ( MapCur.strFile==sMapNode.strFile )			return TRUE;
	}

	return FALSE;
}

HRESULT GLMapList::LoadMapList ( const TCHAR* szMapListFile, LPF_SERVER_INFO pServerInfo, int nServerNum )
{
	GASSERT(szMapListFile);

	m_MapList.clear ();

	std::string strPath;
	strPath = GLOGIC::GetPath();
	strPath += szMapListFile;

	CIniLoader cFILE;

	if( GLOGIC::bGLOGIC_ZIPFILE )
		cFILE.SetZipFile( GLOGIC::strGLOGIC_ZIPFILE );

	std::string strSep( ",[]\t" );
	cFILE.reg_sep( strSep );

	if( !cFILE.open( strPath, true, GLOGIC::bGLOGIC_PACKFILE ) )
	{
		CDebugSet::ToLogFile ( "ERROR : GLMapList::LoadMapList(), File Open %s", szMapListFile );
		return false;
	}

	INT nMapKeySize(0);

	cFILE.getflag( "MAP_LIST_INFO", "MapKeySize", 0, 1, nMapKeySize );

	DWORD dwNUM = cFILE.GetKeySize( "MAP_LIST", "MAP" );
	for ( DWORD i=0; i<dwNUM; ++i )
	{
		bool bUsed(false);
		cFILE.getflag( i, "MAP_LIST", "MAP", 0, nMapKeySize, bUsed );

		if( !bUsed ) continue;

		SMAPNODE MapNode;

		cFILE.getflag( i, "MAP_LIST", "MAP", 1, nMapKeySize, MapNode.strFile );
		cFILE.getflag( i, "MAP_LIST", "MAP", 2, nMapKeySize, MapNode.sNativeID.wMainID );
		cFILE.getflag( i, "MAP_LIST", "MAP", 3, nMapKeySize, MapNode.sNativeID.wSubID );

		if ( !( MapNode.sNativeID.wMainID<MAXLANDMID || MapNode.sNativeID.wSubID<MAXLANDSID ) )
		{
			CString strTemp;
			strTemp.Format( "Map ID GuideLine  is 0~%d / 0~%d.", MAXLANDMID, MAXLANDSID );
			MessageBox( NULL, strTemp.GetString(), "ERROR", MB_OK );
			return E_FAIL;
		}

		cFILE.getflag( i, "MAP_LIST", "MAP", 4, nMapKeySize, MapNode.dwFieldSID );

		if ( pServerInfo && nServerNum )
		{
			if ( MapNode.dwFieldSID >= (DWORD)nServerNum )
			{
				MessageBox ( NULL, "FieldServer ID of Map is Overflow.", "ERROR", MB_OK );
			}

			if ( !pServerInfo[0*MAX_CHANNEL_NUMBER+MapNode.dwFieldSID].IsValid() )
			{
				MessageBox ( NULL, "Not valid FieldServer ID.", "ERROR", MB_OK );
			}
		}

		cFILE.getflag( i, "MAP_LIST", "MAP", 5, nMapKeySize, MapNode.bPeaceZone );
		
		bool bCommissionFree(false);
		cFILE.getflag( i, "MAP_LIST", "MAP", 6, nMapKeySize, bCommissionFree );
		MapNode.bCommission = !bCommissionFree;

		cFILE.getflag( i, "MAP_LIST", "MAP", 7, nMapKeySize, MapNode.bPKZone );
		cFILE.getflag( i, "MAP_LIST", "MAP", 8, nMapKeySize, MapNode.bFreePK );
		cFILE.getflag( i, "MAP_LIST", "MAP", 9, nMapKeySize, MapNode.bItemDrop );
		cFILE.getflag( i, "MAP_LIST", "MAP", 10, nMapKeySize, MapNode.bMove );
		cFILE.getflag( i, "MAP_LIST", "MAP", 11, nMapKeySize, MapNode.bRestart );
		cFILE.getflag( i, "MAP_LIST", "MAP", 12, nMapKeySize, MapNode.bPetActivity );
		cFILE.getflag( i, "MAP_LIST", "MAP", 13, nMapKeySize, MapNode.bDECEXP );
		cFILE.getflag( i, "MAP_LIST", "MAP", 14, nMapKeySize, MapNode.bVehicleActivity );
		cFILE.getflag( i, "MAP_LIST", "MAP", 15, nMapKeySize, MapNode.bClubBattleZone );				

		cFILE.getflag( i, "MAP_LIST", "MAP", 16, nMapKeySize, MapNode.strMapName );
		if ( MapNode.strMapName.length() > MAP_NAME_MAX )
		{
			MessageBox ( NULL, "Map Name Too Long.", MapNode.strMapName.c_str(), MB_OK );
		}

		cFILE.getflag( i, "MAP_LIST", "MAP", 17, nMapKeySize, MapNode.strBGM );
		cFILE.getflag( i, "MAP_LIST", "MAP", 18, nMapKeySize, MapNode.strLoadingImageName );

		cFILE.getflag( i, "MAP_LIST", "MAP", 19, nMapKeySize, MapNode.bInstantMap );

		if ( IsRegMapNode(MapNode) )
		{
			MessageBox ( NULL, "'GLGaeaClient::LoadMapList' Map Set Duplicate.", "ERROR", MB_OK );
			return E_FAIL;
		}

		if ( nServerNum )
		{
			GLLevelFile cLevelFile;
			BOOL bOK = cLevelFile.LoadFile ( MapNode.strFile.c_str(), TRUE, NULL );
			if ( !bOK )
			{
				CONSOLEMSG_WRITE ( "cLevelFile.LoadFile(), %s fail", MapNode.strFile.c_str() );
				continue;
			}
			MapNode.sLEVEL_REQUIRE  = *cLevelFile.GetLevelRequire();
			MapNode.sLEVEL_ETC_FUNC = *cLevelFile.GetLevelEtcFunc();
		}

		m_MapList.insert ( std::make_pair(MapNode.sNativeID.dwID,MapNode) );
	}
	
	return S_OK;
}

SMAPNODE* GLMapList::FindMapNode ( const SNATIVEID &sNativeID )
{
	FIELDMAP_ITER iter = m_MapList.find ( sNativeID.dwID );
	if ( iter!=m_MapList.end() )	return &(*iter).second;

	return NULL;
}

const char* GLMapList::GetMapName ( const SNATIVEID &sNativeID )
{
	SMAPNODE* pMap = FindMapNode ( sNativeID );
	if ( !pMap )	return NULL;

	return pMap->strMapName.c_str();
}

const SLEVEL_REQUIRE* GLMapList::GetLevelRequire ( const SNATIVEID &sNativeID )
{
	SMAPNODE* pMap = FindMapNode ( sNativeID );
	if ( !pMap )	return NULL;

	return &pMap->sLEVEL_REQUIRE;
}

const SLEVEL_ETC_FUNC* GLMapList::GetLevelEtcFunc ( const SNATIVEID &sNativeID )
{
	SMAPNODE* pMap = FindMapNode ( sNativeID );
	if ( !pMap )	return NULL;

	return &pMap->sLEVEL_ETC_FUNC;
}

HRESULT GLMapList::CleanUp ()
{
	m_MapList.clear ();

	return S_OK;
}
