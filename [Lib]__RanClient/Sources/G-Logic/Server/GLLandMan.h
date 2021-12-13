#ifndef GLLANDMAN_H_
#define GLLANDMAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <stack>

#include "./GLSkill.h"
#include "./GLCrow.h"
#include "./GLItem.h"
#include "./GLMaterial.h"
#include "./GLPetField.h"
#include "./GLSummonField.h"
#include "./GLLevelFile.h"

#include "../[Lib]__Engine/Sources/Common/CList.h"
#include "../[Lib]__Engine/Sources/Common/CMemPool.h"
#include "../[Lib]__Engine/Sources/Common/SerialFile.h"
#include "../[Lib]__Engine/Sources/NaviMesh/NavigationMesh.h"
#include "../[Lib]__Engine/Sources/DxTools/DxWeatherMan.h"
#include "../[Lib]__Engine/Sources/DxLand/CollisionMap.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandDef.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandMan.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandGateMan.h"
#include "../[Lib]__Engine/Sources/DxTools/DxViewPort.h"
#include "../[Lib]__Engine/Sources/DxTools/EditMeshs.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLOGIC.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLPeriod.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLQuadTree.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSimpleMeshMan.h"
#include "../[Lib]__Engine/Sources/NaviMesh/NavigationMesh.h"

#include "../[Lib]__MfcEx/Sources/RANPARAM.h"

#include "../[Lib]__NetServer/Sources/s_CConsoleMessage.h"

#ifndef GLChar
	class GLChar;
	typedef GLChar* PGLCHAR;

	typedef CGLLIST<PGLCHAR>	GLCHARLIST;
	typedef SGLNODE<PGLCHAR>	GLCHARNODE;

	typedef std::map<std::string,PGLCHAR>	GLCHAR_MAP;
	typedef std::pair<std::string,PGLCHAR>	GLCHAR_MAP_PAIR;
	typedef GLCHAR_MAP::iterator			GLCHAR_MAP_ITER;
#endif //GLChar

#ifndef GLPetField
	class GLPetField;
	typedef GLPetField* PGLPETFIELD;

	typedef CGLLIST<PGLPETFIELD>	GLPETLIST;
	typedef SGLNODE<PGLPETFIELD>	GLPETNODE;
#endif

#ifndef std_afunc
	namespace std_afunc
	{
		struct CHARDIST;
		struct CROWDIST;
	};
#endif

typedef std::vector<std_afunc::CHARDIST>	ARRAY_CHAR;
typedef ARRAY_CHAR::iterator				ARRAY_CHAR_ITER;

typedef std::vector<std_afunc::CROWDIST>	ARRAY_CROW;
typedef ARRAY_CROW::iterator				ARRAY_CROW_ITER;


class NavigationMesh;

struct SSKILLACT
{
	STARGETID		sID;
	STARGETID		sID_TAR;
	float			fDELAY;
	SKILL::EMAPPLY	emAPPLY;

	DWORD			dwDamageFlag;
	int				nVAR_HP, nVAR_MP, nVAR_SP;
	int				nGATHER_HP, nGATHER_MP, nGATHER_SP;

	SSKILLACT () :
		fDELAY(0.0f),
		emAPPLY(SKILL::EMAPPLY_PHY_SHORT),

		dwDamageFlag( DAMAGE_TYPE_NONE ),

		nVAR_HP(0),
		nVAR_MP(0),
		nVAR_SP(0),

		nGATHER_HP(0),
		nGATHER_MP(0),
		nGATHER_SP(0)
	{
	}

	bool IsDELAY () const
	{
		return fDELAY!=0.0f;
	}

	bool IsATTACK () const
	{
		return nVAR_HP<0 || nVAR_MP<0 || nVAR_SP<0;
	}

	bool VALID_VAR () const
	{
		return (nVAR_HP|nVAR_MP|nVAR_SP)!=NULL;
	}

	bool VALID_GATHER () const
	{
		return (nGATHER_HP|nGATHER_MP|nGATHER_SP)!=NULL;
	}

	bool VALID () const
	{
		return VALID_VAR() | VALID_GATHER();
	}
};

struct SSKILLACTEX
{
	STARGETID	sID;
	STARGETID	sID_TAR;
	float		fDELAY;

	SNATIVEID	idSKILL;		//	��ų ���� ����.
	WORD		wSKILL_LVL;		//

	float		fPUSH_PULL;		//	��ų �о�� Ư�� ���.

	DWORD		dwCUREFLAG;		//	���� �̻� ġ��.
	SSTATEBLOW	sSTATEBLOW;		//	���� �̻� �߻�.

	bool		bCrushBlow;		//	����Ÿ�� / �о�� ����

	DWORD		dwRemoveFlag;	//	����/����� ����		

	SSKILLACTEX () :
		fDELAY(0.0f),

		idSKILL(false),
		wSKILL_LVL(0),

		fPUSH_PULL(0.0f),
		dwCUREFLAG(NULL),
		bCrushBlow (false),
		dwRemoveFlag(0)
	{
	}

	bool IsDELAY () const
	{
		return fDELAY!=0.0f;
	}

	bool VALID() const
	{
		if ( idSKILL!=SNATIVEID(false) )		return true;
		if ( fPUSH_PULL!=0.0f )					return true;

		if ( dwCUREFLAG!=NULL )					return true;
		if ( sSTATEBLOW.emBLOW!=EMBLOW_NONE )	return true;
		if ( dwRemoveFlag!=0)					return true;

		return false;
	}
};

typedef std::list<SSKILLACT>		LISTSACT;
typedef LISTSACT::iterator			LISTSACT_ITER;

typedef std::list<SSKILLACTEX>		LISTSACTEX;
typedef LISTSACTEX::iterator		LISTSACTEX_ITER;

typedef std::list<DWORD>			LISTDELCROWID;
typedef LISTDELCROWID::iterator		LISTDELCROWID_ITER;

typedef std::list<DWORD>			LISTDELMATERIALID;
typedef LISTDELMATERIALID::iterator	LISTDELMATERIALID_ITER;

struct SGROUPINFO
{
	DWORD	  dwLeaderID;
	STARGETID sTargetID;

	SGROUPINFO()
		: dwLeaderID(0)
	{
		sTargetID.RESET();
	}
};
typedef std::set<DWORD>				SETGROUPMEM;
typedef SETGROUPMEM::iterator		SETGROUPMEM_ITER;

typedef std::map< std::string,SETGROUPMEM> MAPMOBGROUP;
typedef MAPMOBGROUP::iterator			   MAPMOBGROUP_ITER;

typedef std::map<std::string,SGROUPINFO> MAPGROUPINFO;
typedef MAPMOBGROUP::iterator			 MAPGROUPINFO_ITER;

class GLLandNode
{
public:
	ITEMDROPLIST	m_ItemList;			//	SUIT ������ ����Ʈ.
	MONEYDROPLIST	m_MoneyList;		//	�� ����Ʈ.

	GLCHARLIST		m_PCList;			//	PC ����Ʈ.
	GLCROWLIST		m_CROWList;			//	CROW ����Ʈ.
	GLMATERIALLIST	m_MaterialList;		//	Materail ����Ʈ
	
	// PET
	GLPETLIST		m_PETList;			//  PET ����Ʈ

	// SUMMON
	GLSUMMONLIST    m_SummonList;		//	Summon ����Ʈ

public:
	GLLandNode ()
	{
	}
	~GLLandNode ()
	{
	}
};
typedef SQUADNODE<GLLandNode>	LANDQUADNODE;
typedef GLQuadTree<GLLandNode>	LANDQUADTREE;

class GLGuidance;

class GLLandMan : public GLLevelFile
{
	enum{ INSTANTMAP_DELETE_TIME = 60 }; //30��

protected:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

public:
	void SetD3dDevice ( LPDIRECT3DDEVICEQ pd3dDevice )	{ m_pd3dDevice = pd3dDevice; }

protected:
	BOOL					m_bInit;
	bool					m_bEmulator;

protected:
	D3DXVECTOR3				m_vMax;
	D3DXVECTOR3				m_vMin;
	NavigationMesh*			m_pNaviMesh;			//	�濡 ���� ������ ���� �޽�
	CCollisionMap			m_CollisionMap;			//	�浹 ��.
	WEATHER_PROPERTY		m_sWeatherProp;			//	���� �Ӽ�.
	LANDQUADTREE			m_LandTree;				//	Cell Tree.
	GLMobScheduleMan		m_MobSchManEx;			//	����ȯ by ���

public:
	DWORD					m_dwClubMapID;			//	���� �� CDM ID
	GLGuidance*				m_pGuidance;			//	���� ����.
	bool					m_bGuidBattleMap;		//	���� Ŭ�� ���� ��.
	bool					m_bGuidBattleMapHall;	//	���� Ŭ�� ���� ��.

	bool					m_bClubDeathMatchMap;			//	Ŭ��������ġ ��
	bool					m_bClubDeathMatchMapHall;		//	Ŭ��������ġ ��


	DWORD					m_dwGuidClubID;			//	���� Ŭ�� ID.
	float					m_fCommissionRate;		//	��� Ȱ�� ������.

	float					m_fElapsedLimitTime;	//  ���ѽð� ������Ʈ.
	int						m_nLastSendRemainTime;	//  ���������� ���� �����ð�.

protected:
	DWORD						m_dwOldWeather;
	std::vector<SONEMAPWEATHER> m_vecMapOldWeather;
	bool					m_bServerStop;			// ������ �����ϴ���

	float					m_fInstantMapDeleteTime; // �δ� ���� �ð�


public:
	LISTDELCROWID			m_listDelGlobIDEx;		// ���� �ֱٿ� mob_gen_ex ���� ������ ���� ID
	LISTDELCROWID			m_listDelGlobID;		// ���� �ֱٿ� mob_gen���� ������ ���� ID, ������ �ʿ�
	LISTDELMATERIALID		m_listDelGlobIDExMat;	// ���� �ֱٿ� mob_gen_ex ���� ������ Meterial�� ID
	LISTDELMATERIALID		m_listDelGlobIDMat;		// ���� �ֱٿ� mob_gen���� ������ Material�� ID, ������ �ʿ�

public:
	NavigationMesh* GetNavi () 						{ return m_pNaviMesh; }
	LANDQUADTREE* GetLandTree ()					{ return &m_LandTree; }
	CCollisionMap& GetCollisionMap ()				{ return m_CollisionMap; }
	
	// ���� �ð� �ʰ� ���� �Ǵ�
	BOOL IsExcessiveLimitTime ();

	BOOL IsWeatherActive () const					{ return m_sWeatherProp.m_dwFlag&ISACTIVE; }

public:
	const BOOL IsInit () const						{ return m_bInit; }
	void SetEmulator ( bool bEMUL )					{ m_bEmulator = bEMUL; }

	void SetServerStop ()							{ m_bServerStop = true; }

public:
	BOOL LoadWldFile ( const char* szWldFile, bool bLandGateLoad );
	HRESULT CreateLandTree ();

	void CleanUp ();

protected:
	PITEMDROP				m_ItemArray[MAXITEM];		//	������ �迭.
	CMemPool<CItemDrop>		m_ItemMemPool;
	CMList<DWORD>			m_FreeItemGIDs;				//	�̻��� Glob ID ��.
	ITEMDROPLIST			m_GlobItemList;				//	������ ����Ʈ.

	PMONEYDROP				m_MoneyArray[MAXMONEY];		//	���� �迭.
	CMemPool<CMoneyDrop>	m_MoneyMemPool;
	CMList<DWORD>			m_FreeMoneyGIDs;			//	�̻��� Glob ID ��.
	MONEYDROPLIST			m_GlobMoneyList;			//	���� ����Ʈ.

	PGLCROW					m_CROWArray[MAXCROW];		//	CROW �迭.
	CMList<DWORD>			m_FreeCROWGIDs;				//	�̻��� CROW GlobID ��.
	GLCROWLIST				m_GlobCROWList;				//	CROW ����Ʈ.

	PGLMATERIAL				m_MaterialArray[MAXCROW];	//	CROW �迭.
	CMList<DWORD>			m_FreeMaterialGIDs;			//	�̻��� CROW GlobID ��.
	GLMATERIALLIST			m_GlobMaterialList;			//	CROW ����Ʈ.

	GLCROWLIST				m_GlobBOSSCROWList;			//  BossCROW ����Ʈ[2006.12.27 �߰�]

	MAPMOBGROUP				m_mapMobGroup;				//  ���� �׷� ����
	MAPGROUPINFO			m_mapGroupInfo;				//  ���� �׷� ���� ����

public:
	
	GLCHARLIST				m_GlobPCList;				//	PC ����Ʈ.

	// PET
	GLPETLIST				m_GlobPETList;

	// Summon
	GLSUMMONLIST			m_GlobSummonList;

    /*HANDLE					m_hWeatherThread;
	HANDLE					m_hWeatherEvent;

	void CreateWeatherThread();
	void CloseWeatherThread();*/

	void UpdateWeather();

protected:
	ARRAY_CHAR				m_vTempPC;
	ARRAY_CHAR				m_vDetectPC;

	ARRAY_CROW				m_vTempCROW;
	ARRAY_CROW				m_vDetectCROW;

protected:
	LISTSACT				m_listSkillAct;
	LISTSACTEX				m_listSkillActEx;

protected:
	void UpdateSkillAct ( float fElapsedTime );

public:
	HRESULT SetMap ();
	HRESULT ReSetMap ();

public:
	BOOL LoadFile ( const char *szFile );
	BOOL LoadFileForInstantMap( GLLevelFile *pDestData, GLLevelFile *pSrcData );
	BOOL LoadMobSchManEx ( const char *szFile ); // ����ȯ �ε� by ���
	BOOL ClearMobSchManEx (); // ����ȯ Ŭ����

public:
	void ADD_GROUPMEMBER ( const std::string strGroupName, const DWORD dwGlobID );
	void ADD_GROUPLEADER ( const std::string strGroupName, const DWORD dwGlobID );
	void DEL_GROUPMEMBER ( const std::string strGroupName, const DWORD dwGlobID );
	void DEL_GROUPLEADER ( const std::string strGroupName );
	SETGROUPMEM GET_GROUPMEMBER ( const std::string strGroupName );
	PGLCROW		GET_GROUPLEADER  ( const std::string strGroupName );
	BOOL		IS_GROUPMEMBER ( const std::string strGroupName, const DWORD dwGlobID );
	STARGETID	GET_GROUPTARGET ( const std::string strGroupName );
	void		SET_GROUPTARGET ( const std::string strGroupName, const STARGETID sTargetID );



public:
	BOOL DropGenItem ( D3DXVECTOR3 &vPos, const SNATIVEID &sNID, EMITEMGEN emGENTYPE, EMGROUP emGroup=EMGROUP_ONE, DWORD dwHoldGID=GAEAID_NULL );
	BOOL DropItem ( D3DXVECTOR3 &vPos, SITEMCUSTOM *pItemCustom, EMGROUP emGroup=EMGROUP_ONE, DWORD dwHoldGID=GAEAID_NULL );
	BOOL DropMoney ( D3DXVECTOR3 &vPos, LONGLONG lnAmount, bool bDropMonster = FALSE, EMGROUP emGroup=EMGROUP_ONE, DWORD dwHoldGID=GAEAID_NULL );

	DWORD DropCrow ( GLMobSchedule *pMobSch );
	DWORD DropCrow ( GLMobSchedule *pMobSch, SNATIVEID sNID, FLOAT fPosX, FLOAT fPosY, FLOAT fScale = 0.0f );
	DWORD DropCrow ( SNATIVEID sNID, WORD wPosX, WORD wPosY );
	DWORD DropCrow ( SNATIVEID sNID, float fPosX, float fPosY );

	DWORD DropMaterial( GLMobSchedule *pMobSch );
	DWORD DropMaterial( SNATIVEID sNID, WORD wPosX, WORD wPosY );
	DWORD DropMaterial( SNATIVEID sNID, float fPosX, float fPosY );

	BOOL DropOutItem ( DWORD dwGlobID );
	BOOL DropOutMoney ( DWORD dwGlobID );
	BOOL DropOutCrow ( DWORD dwGlobID = MAXCROW );
	BOOL DropOutMaterial ( DWORD dwGlobID = MAXCROW );
	BOOL DropSaveCommission();
	
	BOOL RegistChar ( PGLCHAR pChar );
	BOOL RemoveChar ( PGLCHAR pChar );

	// PET
	BOOL RegistPet ( PGLPETFIELD pPet );
	BOOL RemovePet ( PGLPETFIELD pPet );
	BOOL MovePet ( DWORD dwGUID, const D3DXVECTOR3 &vPos );

	// Summon
	BOOL RegistSummon ( PGLSUMMONFIELD pSummon );
	BOOL RemoveSummon ( PGLSUMMONFIELD pSummon );
	BOOL MoveSummon ( DWORD dwGUID, const D3DXVECTOR3 &vPos );

	BOOL MoveCrow ( DWORD dwGlobID, const D3DXVECTOR3 &vPos );
	BOOL MoveChar ( DWORD dwGaeaID, const D3DXVECTOR3 &vPos );

	PITEMDROP GetItem ( DWORD dwGlobID ) const;
	PMONEYDROP GetMoney ( DWORD dwGlobID ) const;
	PGLCROW GetCrow ( DWORD dwID ) const;
	PGLCROW GetCrowID ( SNATIVEID sID ) const;
	PGLCROW GetCrowID ( SNATIVEID sID, WORD wPosX, WORD wPosY ) const;
	PGLMATERIAL GetMaterial ( DWORD dwID ) const;

	HRESULT ClearDropObj ();
	// �ɸ��͸� ������ ������Ʈ ����
	HRESULT ClearExptChaObj ();

public:
	BOOL RequestGenItemFieldFB ( GLMSG::SNET_REQ_GENITEM_FLD_FB * pNetMsg );

public:
	void DoSkillAct ( const SSKILLACT &sACT );
	void DoSkillActEx ( const SSKILLACTEX &sACTEX );

public:
	void RegSkillAct ( const SSKILLACT &sACT );
	void RegSkillActEx ( const SSKILLACTEX &sACTEX );

public:
	void PushPullAct( const STARGETID dwActorID, const STARGETID dwReActorID, const float fPUSH_PULL );

public:
	PGLCHAR FindEnemyChar ( const PGLCROW pFinder );

	ARRAY_CHAR* DetectPC ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB, const DWORD dwMaxFind );
	ARRAY_CHAR* DetectPC ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind );
	ARRAY_CROW* DetectCROW ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind );

	LANDQUADNODE* FindCellNode ( int nx, int nz );
	HRESULT SendMsgPCViewAround ( int nX, int nZ, DWORD dwReqClientID, const LPNET_MSG_GENERIC &nmg );
	HRESULT SendMsgPC ( const LPNET_MSG_GENERIC &nmg );

	BOOL IsCollisionNavi ( D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vCollision );
	BOOL IsCollision ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, const BOOL bFrontColl );

	BOOL DoGateOutPCAll ( DWORD dwExceptClubID, DWORD dwGateID );
	BOOL DoGateOutPCAll ( DWORD dwGateID );

public:
	void GetNaviMeshAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin ) { vMax = m_vMax; vMin = m_vMin; }


protected:
	HRESULT UpdateItem ( float fTime, float fElapsedTime );
	HRESULT UpdateCrow ( float fTime, float fElapsedTime );

public:
	void CHECKVALID();

public:
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);

	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT	RendQuad ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	void	FrameMoveInstantMap ( float fElapsedTime );
	BOOL	MoveOutInstantMap( PGLCHAR pChar );

	// �δ� ���� ���� �Ǿ���ϴ����� üũ
	BOOL	IsDeleteInstantMap( float fElapsedTime );
	

public:
	GLLandMan(void);
	virtual ~GLLandMan(void);
	void	ResetLandMan();
};

typedef GLLandMan*				PGLLANDMAN;

typedef CGLLIST<PGLLANDMAN>		GLLANDMANLIST;
typedef SGLNODE<PGLLANDMAN>		GLLANDMANNODE;

//--------------------------------------------------------------------------------------------
inline PGLCROW GLLandMan::GetCrow ( DWORD dwID ) const
{
	if ( dwID >= MAXCROW )	return NULL;

	return m_CROWArray[dwID];
}

inline PGLCROW GLLandMan::GetCrowID ( SNATIVEID sID ) const
{
	if ( sID == NATIVEID_NULL() ) return NULL;

	for ( int i = 0; i < MAXCROW; ++i )
	{
		if ( m_CROWArray[i] && sID == m_CROWArray[i]->m_sNativeID )
		{
			return m_CROWArray[i];
		}	
	}

	return NULL;
}

//	Ư�� ��ǥ�� Crow�� ã�´�.
inline PGLCROW GLLandMan::GetCrowID ( SNATIVEID sID, WORD wPosX, WORD wPosY ) const
{
	if ( sID == NATIVEID_NULL() ) return NULL;

	for ( int i = 0; i < MAXCROW; ++i )
	{
		if ( m_CROWArray[i] && sID == m_CROWArray[i]->m_sNativeID )
		{
			int nPosX, nPosY;
			D3DXVECTOR3 vPos = m_CROWArray[i]->GetPosition();
			m_sLevelAxisInfo.Convert2MapPos( vPos.x, vPos.z, nPosX, nPosY );
            
			if ( wPosX == nPosX && wPosY == nPosY )
			{
				return m_CROWArray[i];
			}
		}	
	}

	return NULL;
}

inline PGLMATERIAL GLLandMan::GetMaterial ( DWORD dwID ) const
{
	if ( dwID >= MAXCROW )	return NULL;

	return m_MaterialArray[dwID];
}

inline PITEMDROP GLLandMan::GetItem ( DWORD dwGlobID ) const
{
	if ( dwGlobID >= MAXITEM )	return NULL;

	if ( m_ItemArray[dwGlobID] == NULL )	return NULL;

	return m_ItemArray[dwGlobID];
}

inline PMONEYDROP GLLandMan::GetMoney ( DWORD dwGlobID ) const
{
	if ( dwGlobID >= MAXMONEY )	return NULL;

	if ( m_MoneyArray[dwGlobID] == NULL )	return NULL;

	return m_MoneyArray[dwGlobID];
}

inline BOOL GLLandMan::IsCollision ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, const BOOL bFrontColl )
{
	return m_CollisionMap.IsCollision( vP1, vP2, FALSE, NULL, NULL, bFrontColl );
}

#endif // GLLANDMAN_H_
