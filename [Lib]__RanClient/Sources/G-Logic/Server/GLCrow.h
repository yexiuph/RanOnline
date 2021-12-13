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
	//	������ ť
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

	//	���� ���μ���.
	void	PreAttackProc ();
	BOOL	AttackProcess ( float fElapsedTime );
	void	AvoidProc ();
	void	DamageProc ( const int nDAMAGE, DWORD dwDamageFlag );
public:
	virtual void DamageReflectionProc ( int nDAMAGE, STARGETID sACTOR );
	virtual void DefenseSkill( SNATIVEID sNativeID, WORD wLevel, STARGETID sACTOR ) { return;}

	//	��ų ���μ���.
	BOOL	PreSkillProc ();
	BOOL	SkillProcess ( float fElapsedTime );
	void	StartSkillProc ();
	void	SkillProc ( const BOOL bLowSP=FALSE );

	//	��ũ ���μ���.
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

	//	Note : ���� �ڵ�.
	//
public:
	DWORD				m_dwGlobID;				//	���� �޸� �ε����� ( �ش� GLLandMan������ ����ũ�� )

	SNATIVEID			m_sMapID;				//	�� ID.
	DWORD				m_dwCeID;				//	�� ID.
	
	GLLandMan*			m_pLandMan;				//	�����ϴ� Land.
	SGLNODE<GLCrow*>*	m_pGlobNode;			//	�� ����Ʈ ���.
	LANDQUADNODE*		m_pQuadNode;			//	���� Ʈ�� ���.
	SGLNODE<GLCrow*>*	m_pCellNode;			//	�� ����Ʈ ���.

	bool				m_bNpcRecall;			//	��ȯ�� ����
	DWORD				m_dwCallCharID;			//	��ȯ�� ĳ���� ID
	DWORD				m_dwCallUserID;			//	��ȯ�� User ID
	__time64_t			m_tGenTime;				//	��ȯ�� �ð�
	LONGLONG			m_lnCommission;			//	��ȯ�� ��Ȳ���� ���� ����

	//	Note : �ൿ.
protected:
	float				m_fAge;					//	��� �ð�.
	EMACTIONTYPE		m_Action;				//	�ൿ.
	DWORD				m_dwActState;			//	�ൿ ����.
	DWORD				m_dwCFlag;				//	��Ʈ�� �÷���.
	D3DXVECTOR3			m_vPos;					//	���� ��ġ.
	D3DXVECTOR3			m_vGenPos;				//	���� ������ġ.
	D3DXVECTOR3			m_vOrgTarPos;			//	���� Ÿ���� ��ġ.
	
	float				m_fScale;				//  ĳ���� ������

	D3DXVECTOR3			m_vStartPos;			// �̵��޽��� ������ ������ġ

	DWORD				m_dwAttPosSlot;			//	���� ��ġ ����.
	STARGETID			m_TargetID;				//	��ǥ ����.

	EMREACTION			m_emReAction;			//	�������� ����.
	SCROWATTACK*		m_pAttackProp;			//	���� �Ӽ�.
	float				m_fattTIMER;			//	���� ������.
	HITLIST				m_sHITARRAY;

	WORD				m_wTARNUM;				//	Ÿ�� ����.
	STARID				m_sTARIDS[EMTARGET_NET];//	Ÿ�ٵ�.
	D3DXVECTOR3			m_vTARPOS;				//	Ÿ�� ��ġ.

	STARGETID			m_sAssault;				//	���� �ֱٿ� �ڽ��� ��������.
	DAMAGELOG			m_cDamageLog;			///< ������ �α�(ĳ����)
	DAMAGELOG			m_cSummonDamageLog;		///< ������ �α�(��ȯ��)
	DAMAGELOGPARTY      m_cDamageLogParty;      ///< ������ �α�(��Ƽ)

protected:
	float				m_fIdleTime;			//	�޽� �ð�.
	float				m_fStayTimer;			//	�޽� ������.

	float				m_fTargetTimer;			//	Ÿ�� �˻� Ÿ�̸�.

	//	Note : ������.
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

	// ũ�ο�� ��Ƽ�� ������ ������ ��¿�� ����... ( ȣ���� �� ����! )
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
	//	�ڱ����� ��ų üũ ( �ڱ� ��ġ�� ���� )
	BOOL SKT_SELFZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//  ���鿡�� ��ų üũ ( �ڱ� ��ġ�� ���� )
	BOOL SKT_SELFZONEENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//  ��ο��� ��ų üũ ( �ڱ� ��ġ�� ���� )
	BOOL SKT_SELFZONANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	�ڱ����� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
	BOOL SKT_TARSPECOUR_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	���鿡�� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
	BOOL SKT_TARSPECENERMY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	��ο��� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
	BOOL SKT_TARSPECANYBODY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	�ڱ����� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
	BOOL SKT_TARSPECOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	���鿡�� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
	BOOL SKT_TARSPECENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	��ο��� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
	BOOL SKT_TARSPECANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	'��ǥ'�� ���� ��ä�� ���.
	BOOL SKT_TARSPECFANWIZE ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	// �����Ը� �ڱ� ��ġ���� ��� ��ġ����. ( ȭ�� ���� )
	BOOL SKT_TARSELFTOSPEC ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );

	//	�ڱ����� ��ų üũ ( ��ġ�� ���� )
	BOOL SKT_TARZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	���鿡�� ��ų üũ ( ��ġ�� ���� )
	BOOL SKT_TARZONEENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL );
	//	��ο��� ��ų üũ ( ��ġ�� ���� )
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

	//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
	void GenerateReward();
	void GenerateReward_200502();
	void GenerateReward_20060402();
	void GenerateReward_20060417();
	//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
	//! \param dwOWNER ������ ������
	void GenerateRewardItem(DWORD dwOWNER = GAEAID_NULL);
	// �̺�Ʈ�� Ž�� ���� �ý��ۿ� ���� ����� ������ ������� ���Ѵ�.
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