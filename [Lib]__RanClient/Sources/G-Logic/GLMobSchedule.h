#ifndef GLMOBSCHEDULE_H_
#define GLMOBSCHEDULE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <set>

#include "../[Lib]__Engine/Sources/Common/BaseStream.h"
#include "../[Lib]__Engine/Sources/DxTools/DxMethods.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLList.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"
#include "../[Lib]__Engine/Sources/NaviMesh/NavigationMesh.h"

enum EMMOBACTIONS
{
	EMACTION_MOVE		= 0,
	EMACTION_ACTSTAY	= 1,

	EMACTION_SIZE		= 2,
};

struct SMOBACTION
{
	const static DWORD	VERSION;

	EMMOBACTIONS	emAction;

	union
	{
		struct { D3DXVECTOR3 vPos; };
		struct { float fLife; };
	};

	SMOBACTION () :
		emAction(EMACTION_MOVE),
		vPos(0,0,0)
	{
	}
};
typedef CGLLIST<SMOBACTION>	MOBACTIONLIST;
typedef SGLNODE<SMOBACTION>	MOBACTIONNODE;


class GLMobMan;
class CSerialFile;

class GLMobSchedule : public DXAFFINEMATRIX
{
public:
	enum EMFLAGS
	{
		EM_EDIT			= 1,
		EM_MAXSZNAME	= 65,
		EM_MAXRENDPOS	= 12,
		
		EM_MINDISTFACTOR	= 10,
		EM_MAXDISTFACTOR	= 100,
		
		EM_RENDDISTSTART	= 30,
		EM_MAXRENDDISTRANGE	= 14,
	};

	enum EMVOLATFLAGS
	{
		EM_ONCE		= 0x0001,
		EM_EXMODE	= 0x0002,
		//0x0004;
		//0x0008;
	};

	static const D3DXVECTOR3	m_vHIGHER;
	static const float			m_fSPHERE_S1;
	static const float			m_fSPHERE_S2;
	static const float			m_fSPHERE_S3;

	typedef std::vector<D3DXVECTOR3>	VEC3ARRAY;
	typedef VEC3ARRAY::iterator			VEC3ARRAY_ITER;


	// Save/Load 필요 자료.
public:
	enum { VERSION = 0x0105, };
	
	static BOOL	m_bRENDACT;

	char				m_szName[EM_MAXSZNAME];
	SNATIVEID			m_CrowID;
	float				m_fReGenTime;

	int					m_nRegenMin;
	int					m_nRegenHour;
	bool				m_bDayOfWeek[7];
	CTime				m_RegenTime;

	MOBACTIONLIST		m_sMobActList;

	DWORD				m_dwPC_REGEN_GATEID;
	std::string			m_strBUSLOCATION;

	BOOL				m_bLeaderMonster;
	std::string			m_strGroupName;
	
	BOOL				m_bRendGenPos;
	VEC3ARRAY			m_vectorRendGenPos;
	WORD				m_wRendGenPosNum;
	WORD				m_wRendGenPosDist;

public:
	void SetObjRotate90();
	void ModifierSchList( D3DXVECTOR3* pOffset, NavigationMesh* pNaviMesh );

	bool GetUseRegenTime();
	bool IsRegen();

public:
	BOOL Load ( basestream &SFile );
	BOOL Save ( CSerialFile &SFile );

	BOOL Load_0100( basestream &SFile );
	BOOL Load_0101( basestream &SFile );
	BOOL Load_0102( basestream &SFile );
	BOOL Load_0103( basestream &SFile );
	BOOL Load_0104( basestream &SFile );
	BOOL Load_VERSION( basestream &SFile );

public:
	void SetExMode() { m_dwVolatFlags |= EM_EXMODE; }
	BOOL GenerateRendGenPos ( NavigationMesh* pNaviMesh );
	BOOL IsCollision ( D3DXVECTOR3 vPoint1, D3DXVECTOR3 vPoint2 );

	HRESULT RenderGenPos ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );

protected:
	void CalculateBox ();

	// 휘발성 자료.
public:
	DWORD				m_dwVolatFlags;
	float				m_fTimer;

	BOOL				m_bALive;
	BOOL				m_bGroub;
	DWORD				m_dwGlobID; // Note : 몹소환 ID추가 by 경대

	DWORD				m_dwRendGenIndex;

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	GLMobSchedule*		m_pNext;

public:
	GLMobSchedule(void);
	~GLMobSchedule(void);

public:
	void Reset ()
	{
		m_fTimer = 0.0f;
		m_bALive = FALSE;
		m_dwGlobID = UINT_MAX;		
		m_RegenTime	= 0;
	}

	void Clear ()
	{
		memset(m_szName, 0, sizeof(char) * (EM_MAXSZNAME));
		m_fReGenTime = 4.0f;


		m_nRegenMin		= 0;
		m_nRegenHour	= 0;

		m_bDayOfWeek[0]	= FALSE;
		m_bDayOfWeek[1]	= FALSE;
		m_bDayOfWeek[2]	= FALSE;
		m_bDayOfWeek[3]	= FALSE;
		m_bDayOfWeek[4]	= FALSE;
		m_bDayOfWeek[5]	= FALSE;
		m_bDayOfWeek[6]	= FALSE;

		m_sMobActList.DELALL ();

		m_strGroupName = "";
		m_bLeaderMonster = FALSE;
	}

	void operator= ( const GLMobSchedule &MobSch );

	D3DXVECTOR3 GetRendGenPos ()
	{
		D3DXVECTOR3 vGenPos;
		if ( m_vectorRendGenPos.empty() )	vGenPos = m_pAffineParts->vTrans;
		else
		{
			vGenPos = m_vectorRendGenPos[m_dwRendGenIndex++];
			if ( m_dwRendGenIndex >= m_vectorRendGenPos.size() )	m_dwRendGenIndex = 0;
		}

		return vGenPos;
	}
};

typedef CGLLIST<GLMobSchedule*>		MOBSCHEDULELIST;
typedef SGLNODE<GLMobSchedule*>		MOBSCHEDULENODE;

typedef std::vector<GLMobSchedule*>	VECSCHEDULE;
typedef VECSCHEDULE::iterator		VECSCHEDULE_ITER;


struct GLMOBGROUB
{
	float			m_fTimer;
	DWORD			m_dwRendGenIndex;

	VECSCHEDULE		m_vecSchedule;

	GLMOBGROUB () :
		m_fTimer(0),
		m_dwRendGenIndex(UINT_MAX)
	{
	}

	GLMOBGROUB ( const GLMOBGROUB& value )
	{
		operator= ( value );
	}

	GLMOBGROUB& operator= ( const GLMOBGROUB& value );
	void ADD ( GLMobSchedule* pSch );
	void DEL ( GLMobSchedule* pSch );
};

typedef std::map<DWORD,GLMOBGROUB>			MAPMOBGROUB;
typedef MAPMOBGROUB::iterator				MAPMOBGROUB_ITER;

class GLLandMan;
class GLMobScheduleMan
{
public:
	typedef std::set<DWORD>					MOBDESC;
	typedef std::set<DWORD>::iterator		MOBDESC_ITER;

protected:
	const static DWORD		VERSION;

	char					m_szFileName[MAX_PATH];
	MOBSCHEDULELIST			m_GLMobSchList;			//	스케쥴 리스트.
	MOBDESC					m_setMobDesc;			//	설정된 몹 종류. ( 형상정보 사전 로딩을 위해서 )
	MAPMOBGROUB				m_mapMobGroub;			//	특정 종류의 몹(보스)을 랜덤 위치에 출현하기 위해서 여러개의 스케쥴을 쓸수 있게 만듬.

	//	휘발성 자료.
protected:
	GLLandMan*				m_pLandMan;

public:
	SGLNODE<GLMobSchedule*>* FindMobSch ( const char *szName );
	
	void AddMobSchClone ( GLMobSchedule MobSch )
	{
		GLMobSchedule* pNewMobSch = new GLMobSchedule;
		*pNewMobSch = MobSch;

		m_GLMobSchList.ADDTAIL ( pNewMobSch );
	}

	void AddMobSch ( GLMobSchedule *pMobSch )
	{
		m_GLMobSchList.ADDTAIL ( pMobSch );
	}

	void AddMobSch ( const GLMobSchedule &MobSch )
	{
		GLMobSchedule *pMobSch = new GLMobSchedule;
		*pMobSch = MobSch;
		m_GLMobSchList.ADDTAIL ( pMobSch );
	}


	void AddMobSchGroub ( GLMobSchedule *pMobSch );
	void DelMobSchGroub ( GLMobSchedule *pMobSch );

	BOOL DelMobSch ( const char *szName );
	CGLLIST<GLMobSchedule*>* GetMobSchList ()	{ return &m_GLMobSchList; }
	MOBDESC&				 GetMobDesc ()		{ return m_setMobDesc; }
	MAPMOBGROUB&			 GetMobGroubMap ()  { return m_mapMobGroub; }


public:
	GLMobSchedule* GetCollisionMobSchedule ( D3DXVECTOR3 vPoint1, D3DXVECTOR3 vPoint2 );

public:
	void SetObjRotate90();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );

public:
	void SetLandMan ( GLLandMan* pLandMan );
	void CleanUp ();

public:
	BOOL LoadFile ( basestream &SFile );
	BOOL SaveFile ( CSerialFile &SFile );

public:
	GLMobScheduleMan();
	~GLMobScheduleMan();

public:
	void Reset ()
	{
		SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;
		while ( pCur )
		{
			GLMobSchedule* pMObSch = pCur->Data;
			pMObSch->Reset();

			pCur = pCur->pNext;
		};
	}

public:
	void operator= ( GLMobScheduleMan &MobSchMan )
	{
		StringCchCopy ( m_szFileName, MAX_PATH, MobSchMan.m_szFileName );

		m_GLMobSchList.DELALL ();
		SGLNODE<GLMobSchedule*>* pCur = MobSchMan.GetMobSchList()->m_pHead;
		while ( pCur )
		{
			GLMobSchedule* pOldMObSch = pCur->Data;
		
			GLMobSchedule* pMobSch = new GLMobSchedule;
			*pMobSch = *pOldMObSch;

			m_GLMobSchList.ADDTAIL ( pMobSch );

			pCur = pCur->pNext;
		};
	}
};

namespace COMMENT
{
	extern std::string MOBACTIONS[EMACTION_SIZE];
};

#endif // GLMOBSCHEDULE_H_