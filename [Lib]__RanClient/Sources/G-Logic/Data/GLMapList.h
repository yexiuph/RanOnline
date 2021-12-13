#pragma once
#include <map>
#include <string>

#include "./GLLevelFile.h"
#include "./GLogicData.h"

#include "../[Lib]__Engine/Sources/Common/CList.h"
#include "../Dependency/NetGlobal/s_NetGlobal.h"

enum
{
	MAP_NAME_MAX	= 16,
};

struct SMAPNODE
{
	std::string		strFile;
	SNATIVEID		sNativeID;
	DWORD			dwFieldSID;

	bool			bBattleZone;
	bool			bPeaceZone;
	bool			bCommission;
	bool			bPKZone;
	bool			bFreePK;
	bool			bItemDrop;
	bool			bMove;
	bool			bRestart;
	bool			bPetActivity;
	bool			bDECEXP;
	bool			bVehicleActivity;
	bool			bClubBattleZone;
	bool			bCDMZone;

	std::string		strMapName;
	std::string		strBGM;
	std::string		strLoadingImageName;

	bool			bInstantMap;

	SLEVEL_REQUIRE	sLEVEL_REQUIRE;
	SLEVEL_ETC_FUNC sLEVEL_ETC_FUNC;

	SMAPNODE () :
		dwFieldSID(0),
		bBattleZone(false),

		bPeaceZone(false),
		bCommission(false),
		bPKZone(false),
		bFreePK(false),
		bItemDrop(false),
		bMove(false),
		bRestart(false),
		bDECEXP(true),
		bPetActivity(false),
		bVehicleActivity(false),
		bClubBattleZone(false),
		bCDMZone(false),
		bInstantMap(FALSE)
	{
	}

	SMAPNODE ( const SMAPNODE& value )
	{
		operator = ( value );
	}

	SMAPNODE& operator = ( const SMAPNODE& value );
};

class GLMapList
{
public:
	typedef std::map<DWORD,SMAPNODE>		FIELDMAP;
	typedef FIELDMAP::iterator				FIELDMAP_ITER;

	typedef std::vector<DWORD>				VEC_INSTANT_MAPID;
	typedef VEC_INSTANT_MAPID::iterator		VEC_INSTANT_MAPID_ITER;


protected:
	FIELDMAP					m_MapList;

public:
	BOOL IsRegMapNode ( SMAPNODE &MapNode );
	HRESULT LoadMapList ( const TCHAR* szMapListFile, LPF_SERVER_INFO pServerInfo=NULL, int nServerNum=0 );
	HRESULT CleanUp ();

public:
	SMAPNODE* FindMapNode ( const SNATIVEID &sNativeID );
	const char* GetMapName ( const SNATIVEID &sNativeID );
	const SLEVEL_REQUIRE*  GetLevelRequire ( const SNATIVEID &sNativeID );
	const SLEVEL_ETC_FUNC* GetLevelEtcFunc ( const SNATIVEID &sNativeID );

	FIELDMAP& GetMapList ()						{ return m_MapList; }

public:
	GLMapList(void);
	~GLMapList(void);
};


