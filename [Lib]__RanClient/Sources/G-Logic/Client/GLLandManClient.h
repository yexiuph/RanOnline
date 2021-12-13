#ifndef GLLANDMANCLIENT_H_
#define GLLANDMANCLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>

#include "./GLCharClient.h"
#include "./GLCrowClient.h"
#include "./GLMaterialClient.h"
#include "./GLLevelFile.h"
#include "./GLMoney.h"
#include "./GLAnyPet.h"
#include "./GLAnySummon.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandMan.h"

//#include "GLContrlMsg.h"
//#include "GLogicData.h"
//#include "GLMobSchedule.h"
//#include "GLItem.h"

//#include "GLCharClient.h"

//#include "GLLandMan.h"
//#include "GLAnyPet.h"

class GLClientNode
{
public:
	ITEMCLIENTDROPLIST		m_ItemList;			//	SUIT ������ ����Ʈ.
	MONEYCLIENTDROPLIST		m_MoneyList;		//	�� ����Ʈ.

	GLCHARCLIENTLIST		m_PCList;			//	PC ����Ʈ.
	GLCROWCLIENTLIST		m_CROWList;			//	CROW ����Ʈ.
	GLMATERIALCLIENTLIST	m_MaterailList;		//	Material ����Ʈ

	// PET
	GLANYPETLIST			m_PETList;			//  PET ����Ʈ.

	// Summon
	GLANYSUMMONLIST			m_SummonList;			//  Summon ����Ʈ.

public:
	GLClientNode ()
	{
	}
	~GLClientNode ()
	{
	}
};
typedef SQUADNODE<GLClientNode>		CLIENTQUADNODE;
typedef GLQuadTree<GLClientNode>	CLIENTQUADTREE;

class GLLandManClient : public GLLevelFile
{
public:
	typedef std::map<std::string,GLCharClient*>		CHARMAP;
	typedef CHARMAP::iterator						CHARMAP_ITER;

	typedef std::map<DWORD,GLCharClient*>			CHARMAPID;
	typedef CHARMAPID::iterator						CHARMAPID_ITER;

	typedef std::map<DWORD,GLAnyPet*>				PETMAPID;
	typedef PETMAPID::iterator						PETMAPID_ITER;


	typedef std::map<DWORD,GLAnySummon*>				SUMMONMAPID;
	typedef SUMMONMAPID::iterator						SUMMONMAPID_ITER;

public:
	BOOL					m_bPeaceZone;				//	��ȭ���� ����.
	float					m_fAge;

	bool					m_bClubBattle;				//	Ŭ���� ����.
	bool					m_bClubBattleHall;			//	Ŭ���� ����.
	float					m_fCommission;				//	��ŷ� ������.

	bool					m_bClubDeathMatch;			//	CDM����
	bool					m_bClubDeathMatchHall;		//	CDM���� 

	DWORD					m_dwGuidClub;					//	����Ŭ�� ���̵�.
	DWORD					m_dwGuidClubMarkVer;
	char					m_szGuidClubName[CHAR_SZNAME];	//	����Ŭ�� ��ũ.

protected:
	D3DXVECTOR3				m_vMax;
	D3DXVECTOR3				m_vMin;

	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	DxLandMan				m_LandMan;
	CLIENTQUADTREE			m_LandTree;

protected:
	DWORD					m_dwCOUNT;
	PITEMCLIENTDROP			m_ItemArray[MAXITEM];		//	������ �迭.
	ITEMCLIENTDROPLIST		m_GlobItemList;				//	������ ����Ʈ.

	PMONEYCLIENTDROP		m_MoneyArray[MAXMONEY];		//	���� �迭.
	MONEYCLIENTDROPLIST		m_GlobMoneyList;			//	���� ����Ʈ.

	PGLCROWCLIENT			m_CROWArray[MAXCROW];		//	CROW �迭.
	GLCROWCLIENTLIST		m_GlobCROWList;				//	CROW ����Ʈ.

	PGLMATERIALCLIENT		m_MaterialArray[MAXCROW];	//	CROW �迭.
	GLMATERIALCLIENTLIST	m_GlobMaterialList;			//	CROW ����Ʈ.

	GLCHARCLIENTLIST		m_GlobPCList;				//	PC ����Ʈ.

	// PET
	GLANYPETLIST			m_GlobAnyPetList;
	PETMAPID				m_PETArray;

	// SUMMON
	GLANYSUMMONLIST			m_GlobAnySummonList;
	SUMMONMAPID				m_SummonArray;

	CHARMAPID				m_PCArray;
	CHARMAP					m_mapPC;

public:
	BOOL DropItem ( SDROP_CLIENT_ITEM *pItemDrop );
	BOOL DropMoney ( LONGLONG lnAmount, D3DXVECTOR3 vPos, DWORD dwGlobID, float fAge );
	
	BOOL DropCrow ( SDROP_CROW *pCrowDrop );
	BOOL DropChar ( SDROP_CHAR *pCharDrop );
	BOOL DropMaterial ( SDROP_MATERIAL *pCrowDrop );

public:
	BOOL DropOutItem ( DWORD dwGlobID );
	BOOL DropOutMoney ( DWORD dwGlobID );

	BOOL DropOutCrow ( DWORD dwGlobID );
	BOOL DropOutChar ( DWORD dwGlobID );
	BOOL DropOutMaterial ( DWORD dwGlobID );

	// PET
	BOOL DropPet ( PSDROPPET pPet );
	BOOL CreateAnyPet ( PSDROPPET pPet );
	BOOL DropOutPet ( DWORD dwGUID );
	PGLANYPET GetPet ( DWORD dwGUID );

	// Summon
	BOOL DropSummon ( PSDROPSUMMON pSummon );
	BOOL CreateAnySummon ( PSDROPSUMMON pSummon );
	BOOL DropOutSummon ( DWORD dwGUID );
	PGLANYSUMMON GetSummon ( DWORD dwGUID );

	HRESULT ClearDropObj ();

	BOOL ResetClientObjects ();

public:
	BOOL ChangeCharMap( char* szOldName, char* szNewName );

public:
	PITEMCLIENTDROP GetItem ( DWORD dwGlobID );
	PMONEYCLIENTDROP GetMoney ( DWORD dwGlobID );

	PGLCROWCLIENT GetCrow ( DWORD dwID );
	PGLCHARCLIENT GetChar ( DWORD dwID );
	PGLMATERIALCLIENT GetMaterial ( DWORD dwID );

	PGLCHARCLIENT FindChar ( std::string strName );
	PGLCHARCLIENT FindChar ( DWORD dwID );

public:
	void SETITEM_PICKDELAY ( DWORD dwGlobID );
	void SETMONEY_PICKDELAY ( DWORD dwGlobID );

public:
	BOOL ISITEM_PICKDELAY ( DWORD dwGlobID );
	BOOL ISMONEY_PICKDELAY ( DWORD dwGlobID );

public:
	WORD GetTargetBodyRadius ( const STARGETID &sTargetID );

public:
	void DisplayAllUser ();

public:
	const char*				GetGLLandName ()		{ return m_strFileName.c_str(); }

public:
	DxLandMan*				GetLandMan ()			{ return &m_LandMan; }
	NavigationMesh*			GetNaviMesh ()			{ return m_LandMan.GetNaviMesh(); }
	CLIENTQUADTREE*			GetLandTree ()			{ return &m_LandTree; }
	GLCROWCLIENTLIST*		GetCrowList ()			{ return &m_GlobCROWList; }
	GLMATERIALCLIENTLIST*	GetMaterialList ()		{ return &m_GlobMaterialList; }
	GLCHARCLIENTLIST*		GetCharList ()			{ return &m_GlobPCList; }
	GLANYSUMMONLIST*		GetSummonList ()		{ return &m_GlobAnySummonList; }
	ITEMCLIENTDROPLIST*		GetItemList ()			{ return &m_GlobItemList; }
	MONEYCLIENTDROPLIST*	GetMoneyList ()			{ return &m_GlobMoneyList; }

	BOOL IsPeaceZone ()								{ return m_bPeaceZone; }

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	
	HRESULT Render ( CLIPVOLUME &CV );
	HRESULT Render_MobItem ( CLIPVOLUME &CV );
	HRESULT Render_EFF ( CLIPVOLUME &CV );
	void	RenderPickAlpha();

public:
	HRESULT Create ( const char* szSetFileName, LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vBasicPos, SNATIVEID* pMapID=NULL, BOOL bPeaceZone=FALSE );
	HRESULT ActiveMap ();
	void CleanUp ();

public:
	BOOL LoadFile ( const char *szFile );

public:
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects();

	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

public:
	GLLandManClient(void);
	~GLLandManClient(void);

public:
	static GLLandManClient& GetInstance();

};
typedef GLLandManClient* PLANDMANCLIENT;

//--------------------------------------------------------------------------------------------

inline PGLCHARCLIENT GLLandManClient::GetChar ( DWORD dwID )
{
	CHARMAPID_ITER iter = m_PCArray.find(dwID);
	if ( iter!=m_PCArray.end() )	return (*iter).second;

	return NULL;
}

inline PGLCROWCLIENT GLLandManClient::GetCrow ( DWORD dwID )
{
	if ( dwID>=MAXCROW )			return NULL;
	return m_CROWArray[dwID];
}

inline PGLMATERIALCLIENT GLLandManClient::GetMaterial ( DWORD dwID )
{
	if ( dwID>=MAXCROW )			return NULL;
	return m_MaterialArray[dwID];
}

inline PITEMCLIENTDROP GLLandManClient::GetItem ( DWORD dwGlobID )
{
	if ( dwGlobID>=MAXITEM )			return NULL;
	return m_ItemArray[dwGlobID];
}

inline PMONEYCLIENTDROP GLLandManClient::GetMoney ( DWORD dwGlobID )
{
	if ( dwGlobID>=MAXMONEY )	return NULL;
	return m_MoneyArray[dwGlobID];
}

inline PGLANYPET GLLandManClient::GetPet ( DWORD dwGUID )
{
	PETMAPID_ITER iter = m_PETArray.find( dwGUID );
	if ( iter != m_PETArray.end() )	return (*iter).second;

	return NULL;
}

inline PGLANYSUMMON GLLandManClient::GetSummon ( DWORD dwGUID )
{
	SUMMONMAPID_ITER iter = m_SummonArray.find( dwGUID );
	if ( iter != m_SummonArray.end() )	return (*iter).second;

	return NULL;
}

#endif // GLLANDMANCLIENT_H_