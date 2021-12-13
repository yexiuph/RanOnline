#ifndef GLCROW_H_
#define GLCROW_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <deque>

#include "./GLACTOR.h"
#include "./GLogicEx.h"
#include "./GLMobSchedule.h"

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

typedef std::vector<STARGETID>	VEC_SK_TAR;
typedef VEC_SK_TAR::iterator	VEC_SK_ITER;

class GLCrow : public GLCROWLOGIC, public GLACTOR
{
public:
	enum EMCONTROL
	{
		EMTARGET		= 0x00000001,
		EMATTACKABLE	= 0x00000002,
		EMTRACING		= 0x00000004,
		EMSTAY			= 0x00000008,

		EMPREORDER		= 0x00000100,
	};

	enum EMREACTION
	{
		EMREACT_NUNE	= 0,
		EMREACT_ATTACK	= 1,
		EMREACT_SKILL	= 2,
	};


private:
	//	데미지 큐
	struct SSTRIKE
	{
		WORD wDivKey;
		bool bHit;

		SSTRIKE () : wDivKey(0), bHit(false) { }
		SSTRIKE ( WORD _wDivKey, bool _bHit ) : wDivKey(_wDivKey), bHit(_bHit) { }
	};
	typedef std::deque<SSTRIKE>	HITLIST;
	typedef HITLIST::iterator	HITLIST_ITER;

private:
	void	PreStrikeProc ( BOOL bSkill, BOOL bLowSP );

	//	공격 프로세스.
	void	PreAttackProc ();
	BOOL	AttackProcess ( float fElapsedTime );
	void	AvoidProc ();
	void	DamageProc ( const int nDAMAGE, DWORD dwDamageFlag );
public:
	virtual void DamageReflectionProc ( int nDAMAGE, STARGETID sACTOR );
	virtual void DefenseSkill( SNATIVEID sNativeID, WORD wLevel, STARGETID sACTOR ) { return;}

	//	스킬 프로세스.
	BOOL	PreSkillProc ();
	BOOL	SkillProcess ( float fElapsedTime );
	void	StartSkillProc ();
	void	SkillProc ( const BOOL bLowSP=FALSE );

	//	쇼크 프로세스.
	BOOL	ShockProcess ( float fElapsedTime );

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
	int m_nSkillReActionError;

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	//	Note : 제어 코드.
	//
public:
	DWORD				m_dwGlobID;				//	생성 메모리 인덱스용 ( 해당 GLLandMan내에서 유니크함 )

	SNATIVEID			m_sMapID;				//	맵 ID.
	DWORD				m_dwCeID;				//	셀 ID.
	
	GLLandMan*			m_pLandMan;				//	존재하는 Land.
	SGLNODE<GLCrow*>*	m_pGlobNode;			//	맵 리스트 노드.
	LANDQUADNODE*		m_pQuadNode;			//	쿼드 트리 노드.
	SGLNODE<GLCrow*>*	m_pCellNode;			//	셀 리스트 노드.

	bool				m_bNpcRecall;			//	소환된 여부
	DWORD				m_dwCallCharID;			//	소환한 캐릭터 ID
	DWORD				m_dwCallUserID;			//	소환한 User ID
	__time64_t			m_tGenTime;				//	소환된 시간
	LONGLONG			m_lnCommission;			//	소환된 상황에서 생긴 세금

	//	Note : 행동.
protected:
	float				m_fAge;					//	경과 시간.
	EMACTIONTYPE		m_Action;				//	행동.
	DWORD				m_dwActState;			//	행동 상태.
	DWORD				m_dwCFlag;				//	콘트롤 플래그.
	D3DXVECTOR3			m_vPos;					//	현제 위치.
	D3DXVECTOR3			m_vGenPos;				//	최초 생성위치.
	D3DXVECTOR3			m_vOrgTarPos;			//	최초 타겟팅 위치.
	
	float				m_fScale;				//  캐릭터 스케일

	D3DXVECTOR3			m_vStartPos;			// 이동메시지 보낼때 시작위치

	DWORD				m_dwAttPosSlot;			//	공격 위치 슬롯.
	STARGETID			m_TargetID;				//	목표 유닛.

	EMREACTION			m_emReAction;			//	진행중인 행위.
	SCROWATTACK*		m_pAttackProp;			//	공격 속성.
	float				m_fattTIMER;			//	공격 프래임.
	HITLIST				m_sHITARRAY;

	WORD				m_wTARNUM;				//	타겟 숫자.
	STARID				m_sTARIDS[EMTARGET_NET];//	타겟들.
	D3DXVECTOR3			m_vTARPOS;				//	타겟 위치.

	STARGETID			m_sAssault;				//	가장 최근에 자신을 공격한자.
	DAMAGELOG			m_cDamageLog;			///< 데미지 로그(캐릭터)
	DAMAGELOG			m_cSummonDamageLog;		///< 데미지 로그(소환수)
	DAMAGELOGPARTY      m_cDamageLogParty;      ///< 데미지 로그(파티)

protected:
	float				m_fIdleTime;			//	휴식 시간.
	float				m_fStayTimer;			//	휴식 딜래이.

	float				m_fTargetTimer;			//	타갯 검색 타이머.

	//	Note : 스케쥴.
protected:
	MOBACTIONNODE*		m_pCurAct;
	GLMobSchedule*		m_pMobSchedule;

public:
	GLMobSchedule* GetMobSchedule ()			{ return m_pMobSchedule; }

public:
	BOOL IsACTION ( EMACTIONTYPE emCur ) const	{ return m_Action == emCur; }

	BOOL ISDIE () const							{ return m_Action==GLAT_DIE; }
	INT IsLowerHP ();

	BOOL IsSTATE ( DWORD dwState ) const		{ return m_dwActState&dwState; }
	BOOL IsRunning () const						{ return IsACTION(GLAT_MOVE) && IsSTATE(EM_ACT_RUN); };

	BOOL IsVisibleDetect ( const BOOL bRECVISIBLE );
	
protected:
	void SetSTATE ( DWORD dwState )				{ m_dwActState |= dwState; }
	void ReSetSTATE ( DWORD dwState )			{ m_dwActState &= ~dwState; }

public:
	BOOL IsFLAG ( EMCONTROL emFlag )			{ return m_dwCFlag&emFlag; }

protected:
	void SetFLAG ( EMCONTROL emFlag )			{ m_dwCFlag |= emFlag; }
	void ReSetFLAG ( EMCONTROL emFlag )			{ m_dwCFlag &= ~emFlag; }

public:
	EMCROWACT_UP GetActPattern ();

protected:
	bool FindLongTracePos ( const D3DXVECTOR3 &vCur, const D3DXVECTOR3 &vTar, const float fMin, const float fMax, D3DXVECTOR3 &vTracePos );
	bool DoShortTracing ( const WORD wTARRANGE, const float fElapsedTime );
	bool DoLongTracing ( const WORD wTARRANGE, const float fElapsedTime );

protected:
	void RunNextAct ();
	void RunSchedule ( float fTime, float fElapsedTime );

	void NewTarget ( const STARGETID &sTargetID, BOOL bCheckMember = FALSE );
	DWORD FindOptAType ( const STARGETID &sTargetID, bool branvar );

	void ReAction ( const STARGETID &sTargetID, float fElapsedTime=0.0f );
	void ResetAction ();

	bool AttackReAction ( float fTime, float fElapsedTime );
	bool SkillReAction ( float fTime, float fElapsedTime );

public:
	void SetPosition ( D3DXVECTOR3 &vPos );

	DWORD GetGlobID ()							{ return m_dwGlobID; }
	const STARGETID& GetTargetID ()				{ return m_TargetID; }
	EMATT_RGTYPE GetAttackRangeType ()			{ if ( !m_pAttackProp ) return EMATT_SHORT; return m_pAttackProp->emAttRgType; }

	BOOL  IsGroupMember()						{ return (!m_pMobSchedule || m_pMobSchedule->m_strGroupName == "" ) ? FALSE : TRUE; }
	BOOL  IsGroupLeader()						{ return (!m_pMobSchedule || !m_pMobSchedule->m_bLeaderMonster ) ? FALSE : TRUE; }
	std::string  GetGroupName()					{ return (!m_pMobSchedule ) ? "" : m_pMobSchedule->m_strGroupName; }

	void  GroupAttackProcess( const STARGETID sTargetID );
	BOOL  CheckGroupAttack();



protected:
	DWORD SELECT_SKILLSLOT ( SNATIVEID skill_id );

public:
	virtual EMCROW GetCrow () const					{ return GETCROW(); }
	virtual DWORD GetCtrlID () const				{ return m_dwGlobID; }

	// 크로우는 파티를 가질수 없지만 어쩔수 없이... ( 호출할 일 없음! )
	virtual DWORD GetPartyID () const				{ return 0; }

	virtual BOOL IsAction ( EMACTIONTYPE emCur ) const	{ return IsACTION(emCur); }
	virtual BOOL IsDie () const;
	virtual BOOL IsValidBody () const;
	virtual const D3DXVECTOR3& GetPosition () const	{ return m_vPos; }
	virtual WORD GetBodyRadius () const				{ return GETBODYRADIUS(); }

	virtual DWORD GETHOLDBLOW () const				{ return m_dwHOLDBLOW; }
	virtual const SRESIST& GETRESIST () const		{ return m_sSUMRESIST; }
	virtual WORD GetLevel () const					{ return GETLEVEL(); }
	
	virtual DWORD GetNowHP () const					{ return GETHP(); }
	virtual DWORD GetMaxHP () const					{ return GETMAXHP(); }

	virtual WORD GetNowMP () const					{ return GETMP(); };
	virtual WORD GetMaxMP () const					{ return GETMAXMP(); }

	virtual WORD GetNowSP () const					{ return GETMAXSP(); }
	virtual WORD GetMaxSP () const					{ return GETMAXSP(); }

	virtual int GetDefense () const					{ return GETDEFENSE(); }
	virtual DWORD GetBonusExp () const				{ return DWORD(m_pCrowData->m_wBonusExp*GLCONST_CHAR::GETEXP_SCALE()); };

	virtual int GetAvoid () const					{ return GETAVOID(); }
	virtual EMBRIGHT GetBright () const				{ return GETBRIGHT(); }

	virtual DWORD ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const BOOL bShock );
	virtual void ReceivePushPull ( const D3DXVECTOR3 &vMovePos );

	virtual void STATEBLOW ( const SSTATEBLOW &sStateBlow );
	virtual void CURE_STATEBLOW ( DWORD dwCUREFLAG );
	virtual void BUFF_REMOVE( DWORD dwBuffFlag );
	virtual void VAR_BODY_POINT ( const EMCROW emACrow, const DWORD dwAID, const BOOL bPartySkill, int nvar_hp, int nvar_mp, int nvar_sp );
	virtual BOOL RECEIVE_SKILLFACT ( const SNATIVEID skill_id, const WORD wlevel, DWORD &dwSELECT );

	virtual float GETSTATE_DAMAGE () const			{ return m_fSTATE_DAMAGE; }

	virtual DAMAGE_SPEC GetDamageSpec() const		{ return m_sDamageSpec; }
	virtual DEFENSE_SKILL GetDefenseSkill() const	{ return m_sDefenseSkill; }

protected:
	//	자기편에게 스킬 체크 ( 자기 위치에 영향 )
	BOOL SKT_SELFZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//  적들에게 스킬 체크 ( 자기 위치에 영향 )
	BOOL SKT_SELFZONEENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//  모두에게 스킬 체크 ( 자기 위치에 영향 )
	BOOL SKT_SELFZONANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	자기편에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
	BOOL SKT_TARSPECOUR_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	적들에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
	BOOL SKT_TARSPECENERMY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	모두에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
	BOOL SKT_TARSPECANYBODY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	자기편에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
	BOOL SKT_TARSPECOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	적들에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
	BOOL SKT_TARSPECENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	모두에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
	BOOL SKT_TARSPECANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	'목표'를 향한 부채꼴 모양.
	BOOL SKT_TARSPECFANWIZE ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	// 적에게만 자기 위치에서 대상 위치까지. ( 화살 관통 )
	BOOL SKT_TARSELFTOSPEC ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	자기편에게 스킬 체크 ( 위치에 영향 )
	BOOL SKT_TARZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	적들에게 스킬 체크 ( 위치에 영향 )
	BOOL SKT_TARZONEENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	모두에게 스킬 체크 ( 위치에 영향 )
	BOOL SKT_TARZONEANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	BOOL SelectSkillTarget ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const SNATIVEID &sCheckSkill, const WORD wLevel );

protected:
	HRESULT MoveTo ( const D3DXVECTOR3 &vTarPos );
	HRESULT ESCAPE ( STARGETID &sTARID, const DWORD dwDamage );

protected:
	void ToRun ();
	void ToWalk ();
	float GetMoveVelo ();

	void TurnAction ( EMACTIONTYPE toAction, bool bMsg=true );

public:
	float GetAge ()						{ return m_fAge; }

protected:
	DWORD ToDamage ( const STARGETID &sTargetID, const int nDAMAGE, const BOOL bShock );

	//! 몹이 죽었을때 보상을 발생시킨다 (아이템, 게임머니, ?박스 등등...)
	void GenerateReward();
	void GenerateReward_200502();
	void GenerateReward_20060402();
	void GenerateReward_20060417();
	//! 몹이 죽었을때 보상을 발생시킨다 (아이템, 게임머니, ?박스 등등...)
	//! \param dwOWNER 아이템 소유자
	void GenerateRewardItem(DWORD dwOWNER = GAEAID_NULL);
	// 이벤트나 탐닉 방지 시스템에 의해 변경된 아이템 드랍율을 구한다.
	float CalculateItemGenRate( float fGenRate, PGLCHAR pChar, bool bParty );

protected:



	STARGETID FindEnemyChar ();
	VEC_SK_TAR DetectTarget ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB, const DWORD dwMaxFind );
	VEC_SK_TAR DetectTarget ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind );

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
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );

public:
	void RESET_DATA ();
	HRESULT CreateCrow ( GLMobSchedule *pMobSchedule, GLLandMan* pLandMan, SNATIVEID sCROWID, LPDIRECT3DDEVICEQ pd3dDevice=NULL, LPD3DXVECTOR3 pPos=NULL, FLOAT fScale = 0.0f );

public:
	GLCrow(void);
	~GLCrow(void);
};

typedef GLCrow* PGLCROW;

typedef CGLLIST<PGLCROW>	GLCROWLIST;
typedef SGLNODE<PGLCROW>	GLCROWNODE;

#endif // GLCROW_H_