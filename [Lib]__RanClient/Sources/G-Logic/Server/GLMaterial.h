#ifndef GLMETERIAL_H_
#define GLMETERIAL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <deque>

#include "./GLACTOR.h"

#include "./GLMobSchedule.h"
#include "./GLCrowData.h"

#include "../[Lib]__Engine/Sources/NaviMesh/Actor.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSkinChar.h"

#ifndef GLLandMan
	class GLLandMan;
	class GLLandNode;
	typedef SQUADNODE<GLLandNode> LANDQUADNODE;
#endif //GLLandMan

#ifndef GLChar
	class GLChar;
	typedef GLChar* PGLCHAR;
#endif //GLChar

class GLMaterial : public GLACTOR
{	
public:
	PCROWDATA			m_pCrowData;					//	Crow 기본 정보.
	SNATIVEID			m_sNativeID;					//	Crow NativeID.

public:
	
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	Actor				m_actorMove;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vDir;
	D3DXMATRIX			m_matTrans;

	//	Note : AABB
protected:
	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	//	Note : 제어 코드.
	//
public:
	DWORD				m_dwGlobID;				//	생성 메모리 인덱스용 ( 해당 GLLandMan내에서 유니크함 )

	SNATIVEID			m_sMapID;				//	맵 ID.
	DWORD				m_dwCeID;				//	셀 ID.
	
	GLLandMan*				m_pLandMan;				//	존재하는 Land.
	SGLNODE<GLMaterial*>*	m_pGlobNode;			//	맵 리스트 노드.
	LANDQUADNODE*			m_pQuadNode;			//	쿼드 트리 노드.
	SGLNODE<GLMaterial*>*		m_pCellNode;			//	셀 리스트 노드.

	DWORD				m_dwGatherCharID;		//	채집중인 캐릭터
	float				m_fGatherTime;			//	채집중 남은시간

	//	Note : 행동.
protected:
	float				m_fAge;					//	경과 시간.
	EMACTIONTYPE		m_Action;				//	행동.
	DWORD				m_dwActState;			//	행동 상태.
	DWORD				m_dwCFlag;				//	콘트롤 플래그.
	D3DXVECTOR3			m_vPos;					//	현제 위치.
	D3DXVECTOR3			m_vGenPos;				//	최초 생성위치.
	D3DXVECTOR3			m_vStartPos;			// 이동메시지 보낼때 시작위치
	float				m_fIdleTime;			//	휴식 시간.

//	Note : 스케쥴.
protected:
	GLMobSchedule*		m_pMobSchedule;

public:
	GLMobSchedule* GetMobSchedule ()			{ return m_pMobSchedule; }

public:
	BOOL IsACTION ( EMACTIONTYPE emCur ) const	{ return m_Action == emCur; }
	BOOL ISDIE () const							{ return m_Action==GLAT_DIE; }
	BOOL IsSTATE ( DWORD dwState ) const		{ return m_dwActState&dwState; }

protected:
	void SetSTATE ( DWORD dwState )				{ m_dwActState |= dwState; }
	void ReSetSTATE ( DWORD dwState )			{ m_dwActState &= ~dwState; }

public:
	DWORD GetGlobID ()								{ return m_dwGlobID; }




public:
	virtual EMCROW GetCrow () const					{ return CROW_MATERIAL; }
	virtual DWORD GetCtrlID () const				{ return m_dwGlobID; }

	virtual DWORD GetPartyID () const				{ return 0; }

	virtual BOOL IsAction ( EMACTIONTYPE emCur ) const	{ return IsACTION(emCur); }
	virtual BOOL IsDie () const;
	virtual BOOL IsValidBody () const;

	virtual const D3DXVECTOR3& GetPosition () const	{ return m_vPos; }
	virtual WORD GetBodyRadius () const				{ return m_pCrowData->m_sAction.m_wBodyRadius; }

	virtual DWORD GETHOLDBLOW () const				{ return 0; }
	virtual const SRESIST& GETRESIST () const		{ return m_pCrowData->m_sResist; }
	virtual WORD GetLevel () const					{ return m_pCrowData->m_wLevel; }

	virtual DWORD GETHP () const					{ return 0; }	
	virtual DWORD GetNowHP () const					{ return 0; }
	virtual DWORD GetMaxHP () const					{ return 0; }

	virtual WORD GetNowMP () const					{ return 0; };
	virtual WORD GetMaxMP () const					{ return 0; }

	virtual WORD GetNowSP () const					{ return 0; }
	virtual WORD GetMaxSP () const					{ return 0; }

	virtual int GetDefense () const					{ return 0; }
	virtual DWORD GetBonusExp () const				{ return 0; };

	virtual int GetAvoid () const					{ return 0; }
	virtual EMBRIGHT GetBright () const				{ return BRIGHT_LIGHT; }
	virtual BOOL IsRunning () const					{ return false; }

	virtual DWORD ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const BOOL bShock ) { return 0;}
	virtual void ReceivePushPull ( const D3DXVECTOR3 &vMovePos ) {}

	virtual void STATEBLOW ( const SSTATEBLOW &sStateBlow ) {}
	virtual void CURE_STATEBLOW ( DWORD dwCUREFLAG ) {}
	virtual void BUFF_REMOVE( DWORD dwBuffFlag ){}
	virtual void VAR_BODY_POINT ( const EMCROW emACrow, const DWORD dwAID, const BOOL bPartySkill, int nvar_hp, int nvar_mp, int nvar_sp ){}
	virtual BOOL RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, DWORD &dwSELECT ) { return false; }

	virtual float GETSTATE_DAMAGE () const			{ return 0.0f; }

	virtual DAMAGE_SPEC GetDamageSpec() const		{ return DAMAGE_SPEC(); }
	virtual DEFENSE_SKILL GetDefenseSkill() const	{ return DEFENSE_SKILL(); }
	virtual void DamageReflectionProc ( int nDAMAGE, STARGETID sACTOR ) {}
	virtual void DefenseSkill( SNATIVEID sNativeID, WORD wLevel, STARGETID sACTOR ) {}

protected:
	void TurnAction ( EMACTIONTYPE toAction, bool bMsg=true );

public:
	float GetAge ()						{ return m_fAge; }

protected:
	DWORD GenerateGathering( DWORD dwGaeaID, bool& bDrop );
	float CalculateItemGenRate( float fGenRate, PGLCHAR pChar, bool bParty );

public:
	NET_MSG_GENERIC* ReqNetMsg_Drop ();

protected:
	typedef std::vector<DWORD>		VECPC;
	typedef VECPC::iterator			VECPC_ITER;

	DWORD	m_dwPC_AMOUNT;
	VECPC	m_vecPC_VIEW;

	void INSERT_PC_VIEW ( DWORD dwGAEAID );

public:
	HRESULT UpdateViewAround ();
	HRESULT SendMsgViewAround ( NET_MSG_GENERIC* nmg );
	virtual HRESULT SNDMSGAROUND ( NET_MSG_GENERIC *pnet )		{ return SendMsgViewAround ( pnet ); }

public:
	HRESULT UpateAnimation ( float fTime, float fElapsedTime );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT UpdateGathering( float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );

public:
	void RESET_DATA ();
	HRESULT CreateMaterial ( GLMobSchedule *pMobSchedule, GLLandMan* pLandMan, SNATIVEID sCROWID, LPDIRECT3DDEVICEQ pd3dDevice=NULL, LPD3DXVECTOR3 pPos=NULL );

public:
	GLMaterial(void);
	~GLMaterial(void);
};

typedef GLMaterial* PGLMATERIAL;

typedef CGLLIST<PGLMATERIAL>	GLMATERIALLIST;
typedef SGLNODE<PGLMATERIAL>	GLMATERIALNODE;

#endif // GLCROW_H_