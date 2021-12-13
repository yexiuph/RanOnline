#pragma once

#include "./GLItem.h"
#include "./GLCharData.h"
#include "./GLCrowData.h"
//#include "./GLogicData.h"
//#include "./GLContrlMsg.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"



enum EMSUMMONACTIONTYPE
{
	SUMMONAT_IDLE		= 0,
	SUMMONAT_WALK		= 1,
	SUMMONAT_RUN		= 2,
	SUMMONAT_ATTACK		= 3,

	SUMMONAT_NONE		= 4,
};

enum EMSUMMONACTSTATE
{
	EM_SUMMONACT_MOVE			  = 0x00000001,		// 움직임
	EM_SUMMONACT_WAIT			  = 0x00000002,		// 잠시대기
	EM_SUMMONACT_STOP			  = 0x00000004,		// 완전정지
	EM_SUMMONACT_RUN			  = 0x00000010,		// 뛰기모드
	EM_SUMMONACT_WALK			  = 0x00000020,		// 걷기모드
	EM_SUMMONACT_RUN_CLOSELY	  = 0x00000040,		// 근접거리까지 뛰기모드
	EM_SUMMONACT_ATTACK			  = 0x00000100,		// Attack모드
	EM_SUMMONACT_TRACING		  = 0x00000200,		// 추적모드
};


struct GLSUMMON
{
	SUMMON_TYPE		m_emTYPE;
	DWORD			m_dwGUID;
	SNATIVEID		m_sSummonID;
	DWORD			m_dwOwner;					// 주인 GaeaID
	SNATIVEID		m_sMapID;
	DWORD			m_dwCellID;
	DWORD			m_dwSummonID;
	PCROWDATA		m_pSummonCrowData;					//	Crow 기본 정보.
	DWORD			m_dwSummon_AType;

	float			m_fRunArea; // 달려야만 하는 거리
	float			m_fWalkArea; // 걸어야만 하는 거리
	float			m_fOwnerDistance; // 주인과의 유지 거리
	float			m_fPetDistance;  // 펫과의 유지 거리

	float				m_fACTIONDELAY[SCROWDATA::EMMAXATTACK];	//	동작 cool time;

	SSKILLFACT			m_sSKILLFACT[SKILLFACT_SIZE];	//	스킬효과들.

	DWORD				m_dwHOLDBLOW;					//	상태이상이 방지됨.

	SSTATEBLOW			m_sSTATEBLOWS[EMBLOW_MULTI];	//	상태이상.
	float				m_fSTATE_MOVE;					//	상태 이상 이동속도. ( 1.0f == 100% )
	float				m_fSTATE_DELAY;					//	상태 이상 딜래이. ( 1.0f == 100% ) (+)
	float				m_fSTATE_DAMAGE;				//	상태 이상 대미지 증가.

	bool				m_bSTATE_PANT;					//	상태이상 헐떡임.
	bool				m_bSTATE_STUN;					//	상태이상 기절.
	bool				m_bINVISIBLE;					//	투명.
	bool				m_bRECVISIBLE;					//	투명 확인.

	float				m_fATTVELO;						//	공격속도.
	float				m_fSKILL_MOVE;					//	스킬 이동속도.


	int					m_nSUM_HIT;						//	명중율 + 스킬효과.
	int					m_nSUM_AVOID;					//	회피율 + 스킬효과.

	int					m_nSUM_DEFENSE;					//	방어력 ( DP + POW*계수 + ITEM_DEF ) * 공간적응 보정 + 스킬.
	int					m_nDX_DAMAGE;					//	기본 공격력. + 스킬효과.

	int					m_nSUM_PIERCE;					//	추가 관통 정도.
	float				m_fSUM_TARRANGE;				//	공격 가능거리 합산.

	float				m_fDamageRate;					//	공격력 변화율.
	float				m_fDefenseRate;					//	방어력 변화율.

	SRESIST				m_sSUMRESIST;					//	저항값.

	DAMAGE_SPEC			m_sDamageSpec;					//	데미지 흡수 및 반사
	DEFENSE_SKILL		m_sDefenseSkill;				//  발동형 스킬

	DWORD				m_dwAType;						//	공격 속성 순번.

	DWORD				m_dwNowHP;						//	생명량.
	WORD				m_wNowMP;						//	정신량.
	WORD				m_wNowSP;						//	정신량.

	SNATIVEID			m_idACTIVESKILL;				//	활성 스킬.
	WORD				m_wACTIVESKILL_LVL;				//	활성 스킬  래벨.

	//	Note : 회복되는 수치 산출용 임시 변수.
	float				m_fIncHP;
	float				m_fIncMP;
	float				m_fIncSP;


	// 자동 물약 아이템 아이디
	SNATIVEID			m_sPosionID;


	D3DXVECTOR3			m_RandPos[8];

	// constructor & operator
	GLSUMMON () :
	m_emTYPE(SUMMON_TYPE_NONE),

		m_pSummonCrowData(NULL),

		m_dwGUID(UINT_MAX),
		m_sSummonID(NATIVEID_NULL()),

		m_sPosionID(NATIVEID_NULL()),

		m_dwOwner(0),

		m_sMapID(NATIVEID_NULL()),
		m_dwCellID(0),

		m_dwSummonID(0),
		m_dwSummon_AType(0),

		m_dwHOLDBLOW(NULL),
		m_fSTATE_MOVE(1.0f),
		m_fSTATE_DELAY(1.0f),
		m_fSTATE_DAMAGE(1.0f),

		m_bSTATE_PANT(false),
		m_bSTATE_STUN(false),
		m_bINVISIBLE(false),
		m_bRECVISIBLE(false),

		m_fATTVELO(0.0f),
		m_fSKILL_MOVE(0.0f),

		m_nSUM_HIT(0),
		m_nSUM_AVOID(0),

		m_nSUM_DEFENSE(0),
		m_nDX_DAMAGE(0),

		m_fSUM_TARRANGE(0),
		m_nSUM_PIERCE(0),

		m_fDamageRate(1.0f),
		m_fDefenseRate(1.0f),

		m_dwAType(0),

		m_dwNowHP(0),
		m_wNowMP(0),
		m_wNowSP(0),

		m_idACTIVESKILL(false),
		m_wACTIVESKILL_LVL(0),

		m_fIncHP(0.0f),
		m_fIncMP(0.0f),
		m_fIncSP(0.0f),


		m_fRunArea(60.0f),
		m_fWalkArea(40.0f),
		m_fOwnerDistance(20.0f),
		m_fPetDistance(3.0f)

	{
		for ( int i=0; i< SCROWDATA::EMMAXATTACK; ++i )
			m_fACTIONDELAY[i] = 0.0f;

		m_RandPos[0] = D3DXVECTOR3(0,0,m_fOwnerDistance);			
		m_RandPos[1] = D3DXVECTOR3(0,0,-m_fOwnerDistance);
		m_RandPos[2] = D3DXVECTOR3(m_fOwnerDistance,0,0);
		m_RandPos[3] = D3DXVECTOR3(-m_fOwnerDistance,0,0);
		m_RandPos[4] = D3DXVECTOR3(cos(45.0f)*m_fOwnerDistance,0,cos(45.0f)*m_fOwnerDistance);
		m_RandPos[5] = D3DXVECTOR3(-cos(45.0f)*m_fOwnerDistance,0,cos(45.0f)*m_fOwnerDistance);
		m_RandPos[6] = D3DXVECTOR3(cos(45.0f)*m_fOwnerDistance,0,-cos(45.0f)*m_fOwnerDistance);
		m_RandPos[7] = D3DXVECTOR3(-cos(45.0f)*m_fOwnerDistance,0,-cos(45.0f)*m_fOwnerDistance);
	}
	GLSUMMON ( const GLSUMMON& value ) { operator=( value ); }
	GLSUMMON& operator= ( const GLSUMMON& rvalue );

	void RESET_DATA ();


	bool VALIDACTION ()
	{
		const SCROWATTACK *pAttack = m_pSummonCrowData->m_sCrowAttack;
		for ( int i=0; i<SCROWDATA::EMMAXATTACK; ++i )
		{
			if ( pAttack[i].bUsed )		return true;
		}

		return true;
	}

	bool VALIDSKILL ()
	{
		const SCROWATTACK *pAttack = m_pSummonCrowData->m_sCrowAttack;
		for ( int i=0; i<SCROWDATA::EMMAXATTACK; ++i )
		{
			if ( pAttack[i].bUsed && pAttack[i].skill_id!=SNATIVEID(false) )		return true;
		}

		return true;
	}

	bool VALIDATTACK ()
	{
		const SCROWATTACK *pAttack = m_pSummonCrowData->m_sCrowAttack;
		for ( int i=0; i<SCROWDATA::EMMAXATTACK; ++i )
		{
			if ( pAttack[i].bUsed && pAttack[i].skill_id==SNATIVEID(false) )		return true;
		}

		return true;
	}

	void ASSIGN ( const GLSUMMON& sSummonData );
	void RESET ();

	WORD GETSUMMON_BODYRADIUS () const		{ return m_pSummonCrowData->m_sAction.m_wBodyRadius; }
	WORD GETSUMMON_ATTACKRANGE () const	{ return m_pSummonCrowData->m_sCrowAttack[m_dwSummon_AType].wRange; }

	EMCROW GETCROW () const 	{ return CROW_SUMMON; }
	WORD GETLEVEL () const		{ return m_pSummonCrowData->m_wLevel; }
	const char* GETNAME ();

	DWORD GETHP () const		{ return m_dwNowHP; }
	DWORD GETMAXHP () const		{ return m_pSummonCrowData->m_dwHP; }

	int GETMP () const			{ return m_wNowMP; }
	int GETMAXMP () const		{ return m_pSummonCrowData->m_wMP; }

	int GETSP () const			{ return m_wNowSP; }
	int GETMAXSP () const		{ return m_pSummonCrowData->m_wSP; }

	int GETHIT () const			{ return m_nSUM_HIT; }
	int GETAVOID () const		{ return m_nSUM_AVOID; }

	EMBRIGHT GETBRIGHT () const	{ return m_pSummonCrowData->m_emBright; }

	void HP_INCREASE ( const WORD wValue, const BOOL bRate=FALSE );	

	int GETFORCE_LOW () const;
	int GETFORCE_HIGH () const;
	int GETDEFENSE () const			{ return m_nSUM_DEFENSE; }

	float GETSUM_TARRANGE () const	{ return m_fSUM_TARRANGE; }		//	장거리 무기(스킬)에만 가산.
	int GETSUM_PIERCE () const		{ return m_nSUM_PIERCE; }

	WORD GETBODYRADIUS () const		{ return m_pSummonCrowData->m_sAction.m_wBodyRadius; }
	WORD GETATTACKRANGE () const	{ return m_pSummonCrowData->m_sCrowAttack[m_dwAType].wRange; }

	WORD GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const;

	BOOL INIT_DATA ();

	BOOL CHECKHIT ( const STARGETID &cTargetID, const GLLandMan* pLandMan );
	/*bool CALCDAMAGE ( int& rResultDAMAGE, BOOL &bShock, const STARGETID &cTargetID, const GLLandMan* pLandMan,
	const GLSKILL* pSkill=NULL, DWORD dwskill_lev=0, DWORD dwWeatherFlag=NULL, DWORD dwDivCount=0 );*/
		DWORD CALCDAMAGE ( int& rResultDAMAGE, const DWORD dwGaeaID, const STARGETID &cTargetID, const GLLandMan* pLandMan,
		const GLSKILL* pSkill=NULL, DWORD dwskill_lev=0, DWORD dwWeatherFlag=NULL, DWORD dwDivCount=0 );

	DWORD RECEIVE_DAMAGE ( const WORD wDamage );

	void DISABLESKEFF ( int i )					{ m_sSKILLFACT[i].sNATIVEID = NATIVEID_NULL(); }
	void DISABLEBLOW ( int i )					{ m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE; }

	void UPDATE_DATA ( float fTime, float fElapsedTime, BOOL bClient=FALSE );

	/*void SETACTIVESKILL ( SNATIVEID skill_id, WORD wLevel );
	SNATIVEID GETACTIVESKILL ()						{ return m_idACTIVESKILL; }
	EMSKILLCHECK CHECHSKILL ( DWORD dwAType );
	void ACCOUNTSKILL ( WORD wStrikeNum );*/

		SNATIVEID GetPosionItem ();
	void	  SetPosionItem ( SNATIVEID sCustomItem );
	void	  ReSetPosionItem ();
	BOOL	  CheckPosionItem ( SNATIVEID sNativeID );

	EMSKILLCHECK CHECHSKILL ( DWORD dwAType, bool bNotLearn = false  );

	float GETATTVELO ();
	float GETMOVEVELO ();

	D3DXVECTOR3 GetRandomPostision ();
	const D3DXVECTOR3* GetRandPos();


	BOOL LoadFile ( const char* szFileName, SUMMON_TYPE emType );
};

typedef GLSUMMON* PGLSUMMON;

struct SDROPSUMMON
{
	DWORD			m_dwGUID;
	SNATIVEID		m_sSummonID;

	SUMMON_TYPE		m_emTYPE;

	DWORD			m_dwOwner;					// 주인 GaeaID

	SNATIVEID		m_sMapID;
	DWORD			m_dwCellID;

	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vDir;
	D3DXVECTOR3		m_vTarPos;

	DWORD			m_dwActionFlag;
	EMSUMMONACTIONTYPE m_emSUMMONACTYPE;

	WORD			m_wAniSub;					 // 애니메이션 관련
	DWORD			m_dwSummonID;

	DWORD			m_dwNowHP;						//	생명량.
	WORD			m_wNowMP;						//	정신량.

	//	SNATIVEID		m_sPosionID;					//  자동 사용 포션

	SDROPSUMMON () :
	m_dwGUID(UINT_MAX),
		m_sSummonID(NATIVEID_NULL()),

		//		m_sPosionID(NATIVEID_NULL()),

		m_dwOwner(0),

		m_sMapID(NATIVEID_NULL()),
		m_dwCellID(0),

		m_vPos (0,0,0),
		m_vDir (0,0,-1),
		m_vTarPos(0,0,0),

		m_dwActionFlag(0),
		m_emSUMMONACTYPE(SUMMONAT_IDLE),

		m_emTYPE(SUMMON_TYPE_NONE),

		m_wAniSub(1),
		m_dwSummonID(0),

		m_dwNowHP(0),
		m_wNowMP(0)
	{
	}

	SDROPSUMMON ( const SDROPSUMMON& value ) { operator=( value ); }
	SDROPSUMMON& operator= ( const SDROPSUMMON& rvalue );

	void RESET ();
};

typedef SDROPSUMMON* PSDROPSUMMON;

