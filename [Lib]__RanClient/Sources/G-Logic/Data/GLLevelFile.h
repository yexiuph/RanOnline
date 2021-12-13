#pragma once
#include <string>

#include "./GLMapAxisInfo.h"
#include "./GLMobSchedule.h"
#include "./GLogicEx.h"
#include "./GLCharDefine.h"

#include "../[Lib]__Engine/Sources/Common/BaseStream.h"
#include "../[Lib]__Engine/Sources/DxTools/DxCustomTypes.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandDef.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandGateMan.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLQuadTree.h"
#include "../[Lib]__Engine/Sources/Common/StringFile.h"
#include "../[Lib]__Engine/Sources/Common/SerialFile.h"
#include "../[Lib]__Engine/Sources/Common/STRINGUTILS.h"

struct DXLANDEFF;
typedef DXLANDEFF* PLANDEFF;

struct GLLANDMARK;
typedef GLLANDMARK* PLANDMARK;
typedef std::vector<PLANDMARK>	VECLANDMARK;

const int MAX_CREATE_INSTANT_MAP_NUM	= 100;



enum EMCDT_SIGN
{
	EMSIGN_SAME			= 0,	//	=
	EMSIGN_HIGHER		= 1,	//	<
	EMSIGN_LOWER		= 2,	//	>
	EMSIGN_SAMEHIGHER	= 3,	//	<=
	EMSIGN_SAMELOWER	= 4,	//	>=

	EMSIGN_ALWAYS		= 5,	//	항상.
	EMSIGN_FROMTO		= 6,	//	~

    EMSIGN_SIZE			= 7
};

enum EMREQFAIL
{
	EMREQUIRE_COMPLETE	= 0,
	EMREQUIRE_LEVEL		= 1,
	EMREQUIRE_ITEM		= 2,
	EMREQUIRE_SKILL		= 3,
	EMREQUIRE_QUEST_COM	= 4,
	EMREQUIRE_QUEST_ACT	= 5,

	EMREQUIRE_LIVING	= 6,
	EMREQUIRE_BRIGHT	= 7,

	EMREQUIRE_SIZE		= 8,
};

struct SLEVEL_REQUIRE
{
	enum { VERSION = 0x0102, };

	bool		m_bPartyMbr;

	EMCDT_SIGN	m_signLevel;
	WORD		m_wLevel;
	WORD		m_wLevel2;
	SNATIVEID	m_sItemID;
	SNATIVEID	m_sSkillID;
	
	SNATIVEID	m_sComQuestID;
	SNATIVEID	m_sActQuestID;

	EMCDT_SIGN	m_signLiving;
	int			m_nLiving;
	EMCDT_SIGN	m_signBright;
	int			m_nBright;

	SLEVEL_REQUIRE() :
		m_bPartyMbr(false),

		m_signLevel(EMSIGN_ALWAYS),
		m_wLevel(0),
		m_wLevel2(0),

		m_sItemID(false),
		m_sSkillID(false),
	
		m_sComQuestID(false),
		m_sActQuestID(false),

		m_signLiving(EMSIGN_ALWAYS),
		m_nLiving(0),
		
		m_signBright(EMSIGN_ALWAYS),
		m_nBright(0)
	{
	}

	BOOL SAVE ( CSerialFile &SFile );
	BOOL LOAD ( basestream &SFile );

	static VOID SaveCsvHead( std::fstream &SFile );
	VOID SaveCsv( std::fstream &SFile );
	VOID LoadCsv( CStringArray &StrArray );

	EMREQFAIL ISCOMPLETE ( GLCHARLOGIC * pCHARLOGIC ) const;
};

struct SLEVEL_HEAD_100
{
	char					m_szWldFile[MAX_PATH];
	char					m_szMapName[MAXLANDNAME];
	SNATIVEID				m_MapID;
	EMDIVISION				m_eDivision;
};

struct SLEVEL_HEAD
{
	enum { VERSION = 0x0101, };

	std::string				m_strWldFile;
	std::string				m_strMapName;
	EMBRIGHT				m_emBright;

	EMDIVISION				m_eDivision;

	SLEVEL_HEAD () :
		m_emBright(BRIGHT_BOTH),
		m_eDivision(QUAD_DEFAULT_DIV)
	{
	}

	BOOL SAVE ( CSerialFile &SFile );
	BOOL LOAD ( basestream &SFile );

	void Assign ( const SLEVEL_HEAD_100 &sOldData );
};

struct SLIMITTIME
{
	int nLimitMinute;
	int nTellTermMinute;

	SLIMITTIME()
		: nLimitMinute(0)
		, nTellTermMinute(0)

	{
	}

	void Init()
	{
		nLimitMinute	= 0;
		nTellTermMinute = 0;
	}
};

struct SCONTROLCAM
{
	float		fUp;
	float		fDown;
	float		fLeft;
	float		fRight;
	float		fFar;
	float		fNear;
	D3DXVECTOR3 vCamPos;

	SCONTROLCAM()
		: fUp(0.0f)
		, fDown(0.0f)
		, fLeft(0.0f)
		, fRight(0.0f)
		, fNear(0.0f)
		, fFar(0.0f)
		, vCamPos( 0.0f, 0.0f, 0.0f )

	{
	}

	void Init()
	{
		vCamPos	   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		fUp		   = 0.0f;
		fDown	   = 0.0f;
		fLeft	   = 0.0f;
		fRight	   = 0.0f;
		fFar	   = 0.0f;
		fNear	   = 0.0f;

	}
};

struct SLEVEL_ETC_FUNC
{
	enum { VERSION = 0x0101, };

	bool		m_bUseFunction[EMETCFUNC_SIZE];
	SLIMITTIME  m_sLimitTime;
	SCONTROLCAM m_sControlCam;
	VEC_LANDEFF m_vecLandEffect;

	SLANDEFFECT FindLandEffect( DWORD dwNum )
	{
		SLANDEFFECT landEffect;
		if( dwNum >= m_vecLandEffect.size() ) return landEffect;

		landEffect = m_vecLandEffect[dwNum];

		return landEffect;
	};



	SLEVEL_ETC_FUNC()
	{
		ZeroMemory( m_bUseFunction, sizeof(m_bUseFunction) );
		m_vecLandEffect.clear();
	}


	BOOL SAVE ( CSerialFile &SFile );
	BOOL LOAD ( basestream &SFile );
};

class GLLevelFile
{
public:
	enum
	{
		VERSION = 0x0104,

		VERSION_BEFORE_ENCODE	= 0x0100,
		VERSION_BEFORE_GATE		= 0x0101,
	};

public:
	const static char*		FILE_EXT;
	static char				szPATH[MAX_PATH];

public:
	static void SetPath ( const char*szPath )	{ StringCchCopy(szPATH,MAX_PATH,szPath); }
	static char* GetPath ()						{ return szPATH; }

protected:
	std::string				m_strFileName;
	DWORD					m_dwFileVer;
	SNATIVEID				m_sMapID;
	bool					m_bPeaceZone; // true:무조건 PK 안됨
	bool					m_bPKZone;    // true:pk 할수 있다 없다를 나타냄/적대관계 설정됨
	bool					m_bFreePK;    // true:성향치가 감소되지 않는다/적대관계 설정됨
	bool					m_bItemDrop;  // true:아이템 드랍이 가능
	bool					m_bPetActivity; // true:팻활동 가능
	bool					m_bDECEXP; // 경험치 드랍 유무
	bool					m_bVehicleActivity;	// true:탈것 활성화 가능
	bool					m_bClubBattleZone;

private:
	bool					m_bInstantMap;
	DWORD					m_dwInstantHostID;  // 인던주인의 GAEA ID
	bool					m_bPartyInstantMap; // 파티 인던인지 아닌지. 

protected:
	SLEVEL_HEAD				m_sLevelHead;
	SLEVEL_REQUIRE			m_sLevelRequire;
	GLMapAxisInfo			m_sLevelAxisInfo;
	SLEVEL_ETC_FUNC			m_sLevelEtcFunc;

protected:
	DxLandGateMan			m_cLandGateMan;
	GLMobScheduleMan		m_MobSchMan;

protected:
	//	Note : Single Effect
	//
	DWORD			m_dwNumLandEff;
	PLANDEFF		m_pLandEffList;
	OBJAABBNode*	m_pLandEffTree;

protected:
	DWORD			m_dwNumLandMark;
	PLANDMARK		m_pLandMarkList;
	OBJAABBNode*	m_pLandMarkTree;

protected:
	VECLANDMARK		m_vecLANDMARK_DISP;

public:
	void SetObjRotate90();

public:
	DWORD GetNumLandEff ()				{ return m_dwNumLandEff; }
	PLANDEFF GetLandEffList ()			{ return m_pLandEffList; }

	void AddLandEff ( PLANDEFF pLandEff );
	void DelLandEff ( PLANDEFF pLandEff );
	PLANDEFF FindLandEff ( char* szName );

	void BuildSingleEffTree ();

public:
	DWORD GetNumLandMark ()					{ return m_dwNumLandMark; }
	PLANDMARK GetLandMarkList ()			{ return m_pLandMarkList; }

	void AddLandMark ( PLANDMARK pLandMark );
	void DelLandMark ( PLANDMARK pLandMark );
	PLANDMARK FindLandMark ( char* szName );

	void BuildLandMarkTree ();

public:
	void SetFileName ( const char* szFile );
	const char* GetFileName () const;
	const DWORD GetFileVer () const;
	const DWORD GetInstantMapHostID () const { return m_dwInstantHostID; }

public:
	void SetMapID ( const SNATIVEID &sMapID, bool bPeaceZone, bool bPKZone=true );
	void SetInstantMap( bool bInstantMap, DWORD dwHostID, DWORD dwPartyID );
	const SNATIVEID& GetMapID () const;

	const char* GetWldFileName () const;
	const EMBRIGHT GETBRIGHT () const;

	bool IsPeaceZone () const						{ return m_bPeaceZone; }
	bool IsPKZone () const							{ return m_bPKZone; }
	bool IsFreePK () const							{ return m_bFreePK; }
	bool IsItemDrop () const						{ return m_bItemDrop; }
	bool IsPetActivity () const						{ return m_bPetActivity; }
	bool IsDecreaseExpMap () const					{ return m_bDECEXP; }
	bool IsVehicleActivity () const					{ return m_bVehicleActivity; }
	bool IsClubBattleZone()	const					{ return m_bClubBattleZone; }
	bool IsInstantMap() const						{ return m_bInstantMap; }
	bool IsPartyInstantMap() const					{ return m_bPartyInstantMap; }
	void SetInstantMap( const bool bInstantMap )	{ m_bInstantMap = bInstantMap; }

	void SetPKZone ( bool bPK )						{ m_bPKZone = true; }
	void SetClubBattleZone ( bool bClubBattleZone )	{ m_bClubBattleZone = bClubBattleZone; }

public:
	EMDIVISION GetLandTreeDiv () const;
	void SetLandTreeDiv ( EMDIVISION eDivision );

public:
	SLEVEL_ETC_FUNC* GetLevelEtcFunc ()				{ return &m_sLevelEtcFunc; }
	SLEVEL_REQUIRE* GetLevelRequire ()				{ return &m_sLevelRequire; }
	GLMobScheduleMan* GetMobSchMan ()				{ return &m_MobSchMan; }
	GLMapAxisInfo& GetMapAxisInfo ()				{ return m_sLevelAxisInfo; }
	DxLandGateMan& GetLandGateMan ()				{ return m_cLandGateMan; }

public:
	BOOL LOAD_000 ( basestream &SFile, BOOL bCLIENT );
	BOOL LOAD_101 ( basestream &SFile, BOOL bCLIENT );
	BOOL LOAD_102 ( basestream &SFile, BOOL bCLIENT, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_103 ( basestream &SFile, BOOL bCLIENT, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_104 ( basestream &SFile, BOOL bCLIENT, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	BOOL SaveFile ( const char *szFile );
	BOOL LoadFile ( const char *szFile, BOOL bCLIENT, LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL SaveCsvFile ( std::fstream &SFile, bool bAllSave );
	BOOL LoadCsvFile ( const char *szFile );
	BOOL LoadAllCsvFile( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	HRESULT Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );

public:
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);

	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();	

public:
	GLLevelFile(void);
	~GLLevelFile(void);

public:
	GLLevelFile& operator= ( GLLevelFile &rvalue );
};

namespace COMMENT
{
	extern std::string CDT_SIGN[EMSIGN_SIZE];

	extern std::string CDT_SIGN_ID[EMSIGN_SIZE];
};
