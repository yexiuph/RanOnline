#ifndef GLCHARDATA_H_
#define GLCHARDATA_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

#include "./GLInventory.h"
#include "./GLItem.h"
#include "./GLQuestPlay.h"
#include "./GLSkill.h"
#include "./GLVEHICLE.h"

#include "../[Lib]__Engine/Sources/Common/ByteStream.h"

enum EMCHARDATA
{
	EMSKILLQUICK_VERSION	= 0x0101,
	EMSKILLQUICK_SIZE		= 30,
	EMACTIONQUICK_SIZE		= 6,

	SKILLREALFACT_SIZE		= 14,
	SKILLFACT_SIZE			= 14,

	EMMAX_CLUB_NUM			= 100,

	EMSTORAGE_CHANNEL				= 5,
	
	EMSTORAGE_CHANNEL_DEF			= 0,
	EMSTORAGE_CHANNEL_DEF_SIZE		= 1,

	EMSTORAGE_CHANNEL_SPAN			= 1,
	EMSTORAGE_CHANNEL_SPAN_SIZE		= 3,

	EMSTORAGE_CHANNEL_PREMIUM		= 4,
	EMSTORAGE_CHANNEL_PREMIUM_SIZE	= 1,
};

enum EMGM_EVENT_TYPE
{
	EMGM_EVENT_NONE		= 0x00,	//	없음.
	EMGM_EVENT_SPEED	= 0x01,	//	이동속도.
	EMGM_EVENT_ASPEED	= 0x02,	//	공격속도.
	EMGM_EVENT_ATTACK	= 0x04,	//	공격력.
};

struct SCHARSKILL
{
	static DWORD VERSION;
	static DWORD SIZE;

	SNATIVEID	sNativeID;
	WORD		wLevel;

	SCHARSKILL () :
		sNativeID(SNATIVEID::ID_NULL,SNATIVEID::ID_NULL),
		wLevel(0)
	{
	}

	SCHARSKILL ( const SNATIVEID &_sNID, const WORD _wLvl ) :
		sNativeID(_sNID),
		wLevel(_wLvl)
	{
	}
};

struct DAMAGE_SPEC
{
	float		m_fPsyDamageReduce;				// 물리 데미지 흡수율
	float		m_fMagicDamageReduce;			// 마법 데미지 흡수율
	float		m_fPsyDamageReflection;			// 물리 데미지 반사율
	float		m_fPsyDamageReflectionRate;		// 물리 데미지 반사확률
	float		m_fMagicDamageReflection;		// 마법 데미지 반사율
	float		m_fMagicDamageReflectionRate;	// 마법 데미지 반사확률

	DAMAGE_SPEC()	:
		m_fPsyDamageReduce(0.0f),
		m_fMagicDamageReduce(0.0f),
		m_fPsyDamageReflection(0.0f),
		m_fPsyDamageReflectionRate(0.0f),
		m_fMagicDamageReflection(0.0f),
		m_fMagicDamageReflectionRate(0.0f)
	{
	}


	void RESET()
	{
		*this = DAMAGE_SPEC();
	}
};

struct DEFENSE_SKILL
{

	SNATIVEID	m_dwSkillID;					//	발동형 스킬 MID/SID
	WORD		m_wLevel;						//	발동형 스킬 레벨
	float		m_fRate;						//	발동형 스킬 MID/SID 확률
	bool		m_bActive;						//	발동형 스킬 구동상태

	DEFENSE_SKILL() 
		: m_dwSkillID ( NATIVEID_NULL() )
		, m_wLevel ( 0 )
		, m_fRate ( 0.0f )
		, m_bActive(false)
	{
	}

	void RESET()
	{
		m_dwSkillID = NATIVEID_NULL();
		m_wLevel = 0;
		m_fRate = 0.0f;
	}
};

enum EMACTION_SLOT
{
	EMACT_SLOT_NONE	= 0,
	EMACT_SLOT_DRUG	= 1,
};

struct SACTION_SLOT
{
	enum { VERSION = 0x0100, };

	WORD		wACT;
	SNATIVEID	sNID;

	SACTION_SLOT () :
		wACT(EMACT_SLOT_NONE),
		sNID(false)
	{
	}

	bool VALID () const
	{
		return wACT!=EMACT_SLOT_NONE && sNID!=SNATIVEID(false);
	}

	void RESET ()
	{
		wACT = EMACT_SLOT_NONE;
		sNID = SNATIVEID(false);
	}
};

//	Note : 케릭터 1차 항목.
//
struct SCHARDATA
{
protected:
	DWORD				m_dwUserID;
public:
	void SetUserID( DWORD dwUserID) { m_dwUserID = dwUserID; }
	DWORD GetUserID() { return m_dwUserID; }

public:
	DWORD				m_dwUserLvl;				//	사용자 권한 Level.
	__time64_t			m_tPREMIUM;					//	프리미엄 기한.
	bool				m_bPREMIUM;					//	현제 프리미엄 상태.
	__time64_t			m_tCHATBLOCK;				//	귓말 차단.

	__time64_t			m_tSTORAGE[EMSTORAGE_CHANNEL_SPAN_SIZE];	//	창고 기한.
	bool				m_bSTORAGE[EMSTORAGE_CHANNEL_SPAN_SIZE];	//	창고 2 사용.

	WORD				m_wINVENLINE;				//	추가된 인벤토리 줄수.

	DWORD				m_dwServerID;			
	DWORD				m_dwCharID;

	char				m_szName[CHAR_SZNAME];		//	이름. (고정)

	EMTRIBE				m_emTribe;					//	종족. (고정)
	EMCHARCLASS			m_emClass;					//	직업. (고정)
	WORD				m_wSchool;					//	학원.
	WORD				m_wSex;						//	성별.
	WORD				m_wHair;					//	머리스타일.
	WORD				m_wHairColor;				//	머리색상
	WORD				m_wFace;					//	얼굴모양.

	int					m_nBright;					//	속성.
	int					m_nLiving;					//	생활.
	
	WORD				m_wLevel;					//	레벨.
	LONGLONG			m_lnMoney;					//	소지 금액.

	bool				m_bMoneyUpdate;				// 돈이 업데이트 되었는지 아닌지
	bool				m_bStorageMoneyUpdate;		// 창고 돈이 업데이트 되었는지 아닌지
	WORD				m_wTempLevel;				// 비교용 레벨 임시 변수
	LONGLONG			m_lnTempMoney;				// 비교용 소지 금액 임시 변수
	LONGLONG			m_lnTempStorageMoney;		// 비교용 창고  금액 임시 변수

	LONGLONG			m_lVNGainSysMoney;			//  베트남 탐닉 방지 저장 금액

	DWORD				m_dwGuild;					//	길드 번호.
	char				m_szNick[CHAR_SZNAME];		//	별명.
	__time64_t			m_tSECEDE;					//	탈퇴시간.

	SCHARSTATS			m_sStats;					//	Stats.
	WORD				m_wStatsPoint;				//	가용 stats 포인트.

	WORD				m_wAP;						//	기본 공격력.
	WORD				m_wDP;						//	기본 방어력.

	WORD				m_wPA;						//	격투치.
	WORD				m_wSA;						//	사격치.
	WORD				m_wMA;						//	마력치.

	GLLLDATA			m_sExperience;				//	경험치. ( 현제/다음레밸도달값 )
	LONGLONG			m_lnReExp;					//  회복할수 있는 경험치

	DWORD				m_dwSkillPoint;				//	Skill Point.

	LONGLONG			m_lVNGainSysExp;			// 베트남 탐닉 방지 저장 경험치

	GLPADATA			m_sHP;						//	생명량. ( 현제/최대량 )
	GLPADATA			m_sMP;						//	정신량. ( 현제/최대량 )
	GLPADATA			m_sSP;						//	근력량.	( 현제/최대량 )

	WORD				m_wPK;						//	총 PK 횟수.

	bool				m_bEventBuster;				//	이벤트 중인지 아닌지

	__time64_t			m_tLoginTime;				//  로긴 시간이나 이벤트 시작시간

	int					m_EventStartLv;				//  이벤트 최소 레벨
	int					m_EventEndLv;				//  이벤트 최대 레벨

	int					m_RemainEventTime;			//  이벤트 적용까지 남은 시간
	int					m_RemainBusterTime;			//  부스터 남은시간

	bool				m_bEventApply;				//  이벤트 적용대사 여부
	bool				m_bEventStart;				//  이벤트 시작했는지 아닌지, 이벤트 대상인지

	int					m_EventStartTime;			//  이벤트 적용 시작 시간
	int					m_EventBusterTime;			//  이벤트 부스터 타임


	/// 베트남 탐닉 방지 시스템 변수
	LONGLONG			m_VietnamGameTime;			//	저장된 누적 시간
	BYTE				m_dwVietnamGainType;		//  베트남 탐닉 방지 시스템 
	DWORD				m_dwVietnamInvenCount;		// 옮길수 있는 아이템의 갯수

    TCHAR				m_szPhoneNumber[SMS_RECEIVER]; // 캐릭터 폰 번호

	SCHARDATA () 
		: m_dwUserID(0)
		, m_dwUserLvl(0)
		, m_tPREMIUM(0)
		, m_bPREMIUM(false)
		, m_tCHATBLOCK(0)
		, m_wINVENLINE(0)
		, m_dwServerID(0)
		, m_dwCharID(0)
		, m_emTribe(TRIBE_HUMAN)
		, m_emClass(GLCC_FIGHTER_M)
		, m_wSchool(0)
		, m_wSex(0)
		, m_wHair(0)
		, m_wHairColor(0)
		, m_wFace(0)
		, m_nBright(1)
		, m_nLiving(0)
		, m_wLevel(0)
		, m_lnMoney(0)
		, m_lVNGainSysMoney(0)
		, m_bMoneyUpdate(FALSE)
		, m_bStorageMoneyUpdate(FALSE)
		, m_wTempLevel(0)
		, m_lnTempMoney(0)
		, m_lnTempStorageMoney(0)
		, m_dwGuild(CLUB_NULL)
		, m_tSECEDE(0)
		, m_wStatsPoint(0)
		, m_wAP(0)
		, m_wDP(0)
		, m_wPA(0)
		, m_wSA(0)
		, m_wMA(0)
		, m_dwSkillPoint(0)
		, m_wPK(0)
		, m_EventStartTime(0)
		, m_EventBusterTime(0)
		, m_bEventBuster(FALSE)
		, m_bEventStart(FALSE)
		, m_tLoginTime(0)
		, m_EventStartLv(0)
		, m_EventEndLv(0)
		, m_bEventApply(FALSE)
		, m_RemainEventTime(0)
		, m_RemainBusterTime(0)
		, m_VietnamGameTime(0)
		, m_dwVietnamGainType(0)
		, m_lVNGainSysExp(0)
		, m_dwVietnamInvenCount(0)
		, m_lnReExp ( 0 )
	{
		

		memset(m_szName, 0, sizeof(char) * CHAR_SZNAME);
		memset(m_szNick, 0, sizeof(char) * CHAR_SZNAME);
		memset(m_szPhoneNumber, 0, sizeof(TCHAR) * SMS_RECEIVER);

		for ( int i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i )	m_tSTORAGE[i] = 0;
		for ( int i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i )	m_bSTORAGE[i] = false;
	}

	SCHARDATA &GETCHARDATA ()			{ return *this; }
	EMCHARINDEX GETCHARINDEX () const	{ return CharClassToIndex ( m_emClass ); }

	bool IsKEEP_STORAGE ( DWORD dwCHANNEL );	//	해당 번호의 락커에 물건을 맡길수 있는지 검사.
	CTime GetStorageTime (  DWORD dwCHANNEL );
	WORD GetOnINVENLINE ();
	void CalcPREMIUM ();
};
typedef SCHARDATA* PCHARDATA;

typedef std::vector<SHOPPURCHASE>			VECSHOP;

typedef std::map<std::string,SHOPPURCHASE>	MAPSHOP;
typedef MAPSHOP::iterator					MAPSHOP_ITER;

typedef std::map<DWORD,std::string>			MAPSHOP_KEY;
typedef MAPSHOP_KEY::iterator				MAPSHOP_KEY_ITER;

struct SCHARDATA2 : public SCHARDATA
{
	typedef std::map<DWORD,SCHARSKILL>				SKILL_MAP;
	typedef SKILL_MAP::iterator						SKILL_MAP_ITER;
	typedef SKILL_MAP::const_iterator				SKILL_MAP_CITER;

	char					m_szUID[USR_ID_LENGTH+1];

	SKILL_MAP				m_ExpSkills;						//	수련 스킬 속성.
	SITEMCUSTOM				m_PutOnItems[SLOT_TSIZE];			//	착용 Item.

	WORD					m_wSKILLQUICK_ACT;					//	스킬 퀵슬롯중 액티브된 스킬.
	SNATIVEID				m_sSKILLQUICK[EMSKILLQUICK_SIZE];	//	스킬 퀵슬롯.
	SACTION_SLOT			m_sACTIONQUICK[EMACTIONQUICK_SIZE];	//	액션 퀵슬롯.

	GLInventory				m_cInventory;						//	인벤토리.

	BOOL					m_bServerStorage;					//	창고 유효함. ( 서버측. ) ( 새로 생성되는 캐릭터는 무조건 유효하게 생성시 체크함. - GLCHARLOGIC::INIT_DATA() )
	LONGLONG				m_lnStorageMoney;					//	창고 소지 금액.
	BOOL					m_bStorage[EMSTORAGE_CHANNEL];		//	창고 유효함. ( 클라이언트측. )
	GLInventory				m_cStorage[EMSTORAGE_CHANNEL];		//	창고.

	GLQuestPlay				m_cQuestPlay;						//	퀘스트.

	//	서버 전용.
	MAPSHOP					m_mapCharged;						//	구입한 아이템 목록.
	
	//	클라이언트 전용.
	GLInventory				m_cInvenCharged;					//	구입한 아이템 인벤.
	MAPSHOP_KEY				m_mapChargedKey;					//	구입한 아이템 인벤의 해당 위치 purkey 찾기.

	//	Note : 초기 시작될 맵, 맵 위치값.
	//
	SNATIVEID			m_sStartMapID;				//	초기 시작 맵.
	DWORD				m_dwStartGate;				//	초기 시작 개이트.
	D3DXVECTOR3			m_vStartPos;				//	초기 시작 위치.

	SNATIVEID			m_sSaveMapID;				//	종료 맵.
	D3DXVECTOR3			m_vSavePos;					//	종료 위치.

	SNATIVEID			m_sLastCallMapID;			//	직전귀환 맵.
	D3DXVECTOR3			m_vLastCallPos;				//	직전귀환 위치.

	INT					m_dwThaiCCafeClass;			// 태국 사이버 카페 
	INT					m_nMyCCafeClass;			// 말레이시아 PC방 이벤트 
	SChinaTime			m_sChinaTime;				// 중국 시간별 수익
	SEventTime			m_sEventTime;				// 이벤트 시간

	SVietnamGainSystem  m_sVietnamSystem;			// 베트남 탐닉방지 시스템
	GLInventory			m_cVietnamInventory;		// 베트남 저장한 탐닉 인벤토리
	bool				m_bVietnamLevelUp;			// 베트남 경험치 획득 아이템 사용시 여러단계의 레벨을 증가시킬 수 있다.

	// 아이템 개조 정보	// ITEMREBUILD_MARK
	BOOL				m_bRebuildOpen;					// 개조창 오픈 유무
	SINVEN_POS			m_sRebuildItem;					// 개조창 등록된 인벤토리 아이템
	SINVEN_POS			m_sPreInventoryItem;			// 개조용 인벤토리에서 들어올린 임시 아이템
	LONGLONG			m_i64RebuildCost;				// 개조시 필요한 금액
	LONGLONG			m_i64RebuildInput;				// 개조창 등록한 금액

	bool				m_bTracingUser;			// 현재 추적중인 유저인지 아닌지


	SNATIVEID			m_sSummonPosionID;		// 소환수 포션 아이템 아이디

	typedef std::map<DWORD,ITEM_COOLTIME>	COOLTIME_MAP;
	typedef COOLTIME_MAP::iterator			COOLTIME_MAP_ITER;
	typedef COOLTIME_MAP::const_iterator	COOLTIME_MAP_CITER;

	COOLTIME_MAP		m_mapCoolTimeType;		//	아이템 타입별 쿨타임
    COOLTIME_MAP		m_mapCoolTimeID;		//	아이템 MID/SID별 쿨타임

	
	SCHARDATA2();
	void Assign ( SCHARDATA2 &CharData );

	EMCHARINDEX GETCHARINDEX () const { return CharClassToIndex ( m_emClass ); }

	BOOL SETEXPSKILLS_BYBUF ( CByteStream &ByteStream );
	BOOL GETEXPSKILLS_BYBUF ( CByteStream &ByteStream ) const;

	BOOL GETPUTONITEMS_BYBUF ( CByteStream &ByteStream ) const;
	
	BOOL SETSKILL_QUICKSLOT ( CByteStream &ByteStream );
	BOOL GETSKILL_QUICKSLOT ( CByteStream &ByteStream ) const;

	BOOL SETACTION_QUICKSLOT ( CByteStream &ByteStream );
	BOOL GETACTION_QUICKSLOT ( CByteStream &ByteStream ) const;


	BOOL SETINVENTORY_BYBUF ( CByteStream &ByteStream );
	BOOL GETINVENTORYE_BYBUF ( CByteStream &ByteStream ) const;

	BOOL SETSTORAGE_BYBUF ( CByteStream &ByteStream );
	BOOL GETSTORAGE_BYBUF ( CByteStream &ByteStream ) const;

	BOOL SETQUESTPLAY ( CByteStream &ByteStream );
	BOOL GETQUESTPLAY ( CByteStream &ByteStream ) const;

	BOOL SETSHOPPURCHASE ( VECSHOP &vecSHOP );

	// 베트남 탐닉방지 시스템 추가에 따른 캐릭터 추가 인벤토리 확장
	BOOL SETVTADDINVENTORY_BYBUF ( CByteStream &ByteStream );
	BOOL GETVTADDINVENTORYE_BYBUF ( CByteStream &ByteStream ) const;

	BOOL SETITEMCOOLTIME_BYBUF( CByteStream &ByteStream ); 
	BOOL GETITEMCOOLTIME_BYBUF( CByteStream &ByteStream ) const; 

public:
	BOOL LOADFILE ( const char* szFileName );

public:
	//	클라이언트 전용.
	BOOL ADDSHOPPURCHASE ( const char* szPurKey, SNATIVEID nidITEM );
	BOOL DELSHOPPURCHASE ( const DWORD dwID );

public:	// ITEMREBUILD_MARK
	const SITEMCUSTOM& GET_REBUILD_ITEM();
	const SITEMCUSTOM& GET_PREHOLD_ITEM();	// 임시로 들어올린 아이템

public:	// ITEMREBUILD_MARK
	VOID InitRebuildData();
	VOID OpenRebuild()							{ m_bRebuildOpen = TRUE; }
	VOID CloseRebuild()							{ m_bRebuildOpen = FALSE; }
	const BOOL ValidRebuildOpen()				{ return m_bRebuildOpen; }
	const LONGLONG GetRebuildCost()				{ return m_i64RebuildCost; }
	const LONGLONG GetRebuildInput()			{ return m_i64RebuildInput; }

private:
	SCHARDATA2(const SCHARDATA2 &Inven )		{ GASSERT(0&&"묵시적 복사 불허!"); }
	SCHARDATA2& operator= ( SCHARDATA2 &Inven )	{ GASSERT(0&&"묵시적 복사 불허!"); return *this; }
};
typedef SCHARDATA2* PCHARDATA2;


struct SSTATEBLOW
{
	EMSTATE_BLOW	emBLOW;			//	상태 이상 종류.
	float			fAGE;			//	적용 시간.
	float			fSTATE_VAR1;	//	적용 값 1.
	float			fSTATE_VAR2;	//	적용 값 2.

	SSTATEBLOW () :
		emBLOW(EMBLOW_NONE),
		fAGE(0),
		fSTATE_VAR1(0),
		fSTATE_VAR2(0)
	{

	}
};

struct SSKILLFACT
{
	SNATIVEID		sNATIVEID;		//	스킬 ID.
	WORD			wLEVEL;			//	스킬 레벨.
	float			fAGE;			//	생성후 남은 시간.

	SKILL::EMTYPES	emTYPE;			//	기본 종류.
	float			fMVAR;			//	기본 수치.

	EMIMPACT_ADDON	emADDON;		//	부가 종류.
	float			fADDON_VAR;		//	부가 수치.

	EMSPEC_ADDON	emSPEC;
	float			fSPECVAR1;
	float			fSPECVAR2;
	DWORD			dwSPECFLAG;
	SNATIVEID		dwNativeID;

	DWORD			dwSpecialSkill;		  // 특수스킬
	bool			bRanderSpecialEffect; // 특수 스킬 사용시 이펙트가 발동되었는지 여부

	SSKILLFACT () :
		sNATIVEID(NATIVEID_NULL()),
		wLEVEL(0),
		fAGE(0),

		emTYPE(SKILL::EMFOR_VARHP),
		fMVAR(0),

		emADDON(EMIMPACTA_NONE),
		fADDON_VAR(0),

		emSPEC(EMSPECA_NULL),
		fSPECVAR1(0),
		fSPECVAR2(0),
		dwSPECFLAG(NULL),
		dwNativeID( NATIVEID_NULL() ),

		dwSpecialSkill(0),
		bRanderSpecialEffect(FALSE)

	{
	}

	BOOL IsSpecialSkill ( DWORD dwState )		 		{ return dwSpecialSkill == dwState ? TRUE : FALSE ; }
	void SetSpecialSkill ( DWORD dwState )				{ dwSpecialSkill = dwState; }


	void RESET ();
};


struct SLANDEFFECT
{
	D3DXVECTOR2			vMinPos;
	D3DXVECTOR2			vMaxPos;
	EMLANDEFFECT_TYPE	emLandEffectType;
	float				fValue;

	SLANDEFFECT() :
		vMinPos( 0.0f, 0.0f ),
		vMaxPos( 0.0f, 0.0f ),
		emLandEffectType(EMLANDEFFECT_ATK_SPEED),
		fValue( 0.0f )
	{
	}

	void Init()
	{
		vMinPos			 = D3DXVECTOR2( 0.0f, 0.0f );
		vMaxPos			 = D3DXVECTOR2( 0.0f, 0.0f );
		emLandEffectType = EMLANDEFFECT_ATK_SPEED;
		fValue			 = 0.0f;
	}

	bool IsUse()
	{
		if( vMinPos		 	 == D3DXVECTOR2( 0.0f, 0.0f ) &&
			vMaxPos			 == D3DXVECTOR2( 0.0f, 0.0f ) &&
			emLandEffectType == EMLANDEFFECT_ATK_SPEED &&
			fValue			 == 0.0f ) return FALSE;
		return TRUE;
	}

	bool operator == ( const SLANDEFFECT &value )
	{
		if( vMaxPos			 != value.vMaxPos )			 return FALSE;
		if( vMinPos			 != value.vMinPos )			 return FALSE;
		if( emLandEffectType != value.emLandEffectType ) return FALSE;
		if( fValue		     != value.fValue )			 return FALSE;

		return TRUE;
	}

};

typedef std::vector<SLANDEFFECT> VEC_LANDEFF;
typedef VEC_LANDEFF::iterator	 VEC_LANDEFF_ITER;

struct SPASSIVE_SKILL_DATA
{
	short	m_nHP;
	short	m_nMP;
	short	m_nSP;

	short	m_nDAMAGE;
	short	m_nDEFENSE;

	short	m_nHIT;
	short	m_nAVOID;

	short	m_nPIERCE;
	float	m_fTARRANGE;

	float	m_fMOVEVELO;
	float	m_fATTVELO;
	float	m_fSKILLDELAY;

	float	m_fINCR_HP;
	float	m_fINCR_MP;
	float	m_fINCR_SP;

	float	m_fDAMAGE_RATE;
	float	m_fDEFENSE_RATE;

	DAMAGE_SPEC m_sDamageSpec;

	short	m_nPA;
	short	m_nSA;
	short	m_nMA;

	float	m_fHP_RATE;
	float	m_fMP_RATE;
	float	m_fSP_RATE;

	SRESIST	m_sSUMRESIST;					//	저항값.

	SPASSIVE_SKILL_DATA () :
		m_nHP(0),
		m_nMP(0),
		m_nSP(0),

		m_fINCR_HP(0),
		m_fINCR_MP(0),
		m_fINCR_SP(0),

		m_nDAMAGE(0),
		m_nDEFENSE(0),

		m_nHIT(0),
		m_nAVOID(0),

		m_nPIERCE(0),
		m_fTARRANGE(0),

		m_fMOVEVELO(0),
		m_fATTVELO(0),
		m_fSKILLDELAY(0),

		m_fDAMAGE_RATE(0),
		m_fDEFENSE_RATE(0),

		m_nPA(0),
		m_nSA(0),
		m_nMA(0),

		m_fHP_RATE(0),
		m_fMP_RATE(0),
		m_fSP_RATE(0)	
	{
	}
};

struct SQITEM_FACT
{
	EMITEM_QUESTION	emType;
	float			fTime;
	WORD			wParam1;
	WORD			wParam2;

	SQITEM_FACT () :
		emType(QUESTION_NONE),
		fTime(0),
		wParam1(0),
		wParam2(0)
	{
	}

	bool IsACTIVE ()
	{
		return emType!=QUESTION_NONE;
	}

	void RESET ()
	{
		emType = QUESTION_NONE;
		fTime = 0;
		wParam1 = 0;
		wParam2 = 0;
	};
};



struct SEventState
{
	float				fItemGainRate;		// 이벤트 시 얻게되는 아이템 드랍율
	float				fExpGainRate;		// 이벤트 시 얻게되는 경헙치 배율
	int					MinEventLevel;		// 이벤트 최소 레벨
	int					MaxEventLevel;		// 이벤트 최대 레벨
	bool				bEventStart;		// 이벤트가 시작됐는지 아닌지
	int					EventPlayTime;		// 이벤트가 적용 되는 플레이 시간
	int					EventBusterTime;	// 이벤트가 지속되는 시간
	CTime				EventStartTime;		// 이벤트가 최초 시작한 시간
	DWORD				dwEventEndMinute;	// 이벤트 적용 시간


	SEventState()
	{
		Init();
	}
	void Init()
	{
		fItemGainRate    = 1.0f;
		fExpGainRate     = 1.0f;
		MinEventLevel    = 0;		// 이벤트 최소 레벨
		MaxEventLevel    = 0;		// 이벤트 최대 레벨
		bEventStart      = FALSE;			// 이벤트가 시작됐는지 아닌지
		EventPlayTime    = 0;		// 이벤트가 시작 시간
		EventBusterTime  = 0;		// 이벤트가 지속되는 시간
		EventStartTime   = 0;
		dwEventEndMinute = 0;

	}
};

struct SEVENT_FACT
{
	INT				nType;
	WORD			wSpeed;
	WORD			wASpeed;
	WORD			wAttack;

	SEVENT_FACT () 
		: nType(EMGM_EVENT_NONE)
		, wSpeed(0)
		, wASpeed(0)
		, wAttack(0)
	{
	}

	bool IsACTIVE( EMGM_EVENT_TYPE emType )
	{
		return (nType&emType)!=EMGM_EVENT_NONE;
	}

	void SetEVENT( EMGM_EVENT_TYPE emType, WORD wValue )
	{
		nType |= emType;

		switch( emType )
		{
		case EMGM_EVENT_SPEED:	wSpeed=wValue;	break;
		case EMGM_EVENT_ASPEED:	wASpeed=wValue;	break;
		case EMGM_EVENT_ATTACK:	wAttack=wValue;	break;
		}
	}

	void ResetEVENT( EMGM_EVENT_TYPE emType )
	{
		nType &= ~emType;
	}
};

struct SDROP_STATEBLOW
{
	EMSTATE_BLOW	emBLOW;			//	상태 이상 종류.
	float			fAGE;			//	적용 시간.
	float			fSTATE_VAR1;	//	적용 값 1.
	float			fSTATE_VAR2;	//	적용 값 2.

	SDROP_STATEBLOW () 
		: emBLOW(EMBLOW_NONE)
		, fAGE(0)
		, fSTATE_VAR1(0)
		, fSTATE_VAR2(0)
	{
	}

	SDROP_STATEBLOW& operator= ( SSTATEBLOW &sblow )
	{
		emBLOW = sblow.emBLOW;
		fAGE = sblow.fAGE;
		fSTATE_VAR1 = sblow.fSTATE_VAR1;
		fSTATE_VAR2 = sblow.fSTATE_VAR2;

		return *this;
	}
};

struct SDROP_SKILLFACT
{
	SNATIVEID		sNATIVEID;		//	스킬 ID.
	WORD			wSLOT;			//	슬롯 위치.
	WORD			wLEVEL;			//	스킬 LEVEL.
	float			fAGE;			//	생성후 남은 시간.

	SDROP_SKILLFACT () 
		: sNATIVEID(NATIVEID_NULL())
		, wSLOT(0)
		, wLEVEL(0)
		, fAGE(0)
	{
	}

	void Assign ( SSKILLFACT &sfact, WORD _wSLOT )
	{
		wSLOT = _wSLOT;
		sNATIVEID = sfact.sNATIVEID;
		wLEVEL = sfact.wLEVEL;
		fAGE = sfact.fAGE;
	}
};

struct SDROP_CHAR
{
	enum
	{
		CHAR_GEN	= 0x001,
		CLUB_CD		= 0x002
	};

	char			szName[CHAR_SZNAME];		//	이름.
	EMTRIBE			emTribe;					//	종족.
	EMCHARCLASS		emClass;					//	직업.
	WORD			wSchool;					//	학원.
	WORD			wHair;						//	머리카락.
	WORD			wHairColor;					//  머리카락 컬러
	WORD			wFace;						//	얼굴모양.
	WORD			wSex;						//  성별

	int				nBright;					//	속성.

	DWORD			dwCharID;					//	케릭터ID.
	WORD			wLevel;						//	레벨.
	DWORD			dwGuild;					//	길드 번호.
	DWORD			dwAlliance;					//	동맹 번호
	char			szClubName[CHAR_SZNAME];	//	클럽 이름.
	DWORD			dwGuildMarkVer;				//	길드 마크 번호.
	DWORD			dwGuildMaster;				//	길드 마스터.
	char			szNick[CHAR_SZNAME];		//	별명.
	
	DWORD			dwParty;					//	파티 번호.
	DWORD			dwPMasterID;				//	파티 마스터 ID.

	GLPADATA		sHP;						//	생명량. ( 현제/최대량 )

	DWORD			dwGaeaID;					//	생성 메모리 인덱스용.
	SNATIVEID		sMapID;						//	생성 맵 ID.
	DWORD			dwCeID;						//	셀 ID.
	D3DXVECTOR3		vPos;						//	위치.
	D3DXVECTOR3		vDir;						//	위치.

	EMACTIONTYPE	Action;						//	현제 액션.
	DWORD			dwActState;					//	현제 액션 플래그.
	D3DXVECTOR3		vTarPos;					//	현제 목표 위치.

	DWORD			dwSummonGUID;				//  소환수 ID

	SDROP_SKILLFACT	sSKILLFACT[SKILLFACT_SIZE];
	SDROP_STATEBLOW	sSTATEBLOWS[EMBLOW_MULTI];
	int				nLandEffect[EMLANDEFFECT_MULTI];	//  지형 이상 효과들

	SQITEM_FACT		sQITEMFACT;
	SEVENT_FACT		sEVENTFACT;
	SEventState		sEventState;
	
	SITEMCLIENT		m_PutOnItems[SLOT_NSIZE_S_2];	//	착용 Item.
	BOOL			m_bVehicle;
	CLIENT_VEHICLE	m_sVehicle;

	bool			m_bItemShopOpen;		// ItemShopOpen

	BOOL			m_bUseArmSub;				// 극강부를 위한 보조 무기 사용여부

	DWORD			dwFLAGS;					//	기타 속성.

	SPASSIVE_SKILL_DATA	sPASSIVE_SKILL;			//	passive skill data.
	
	DWORD			m_dwANISUBTYPE;				//	애니메이션 서브타입

	SDROP_CHAR () 
		: emTribe(TRIBE_HUMAN)
		, emClass(GLCC_FIGHTER_M)
		, wSchool(0)
		, wHairColor(0)
		, wSex(0)
		, wHair(0)
		, wFace(0)
		, nBright(0)
		, dwCharID(0)
		, wLevel(1)
		, dwGuild(CLUB_NULL)
		, dwGuildMarkVer(0)
		, dwGuildMaster(0)
		, dwAlliance(0)
		, dwParty(PARTY_NULL)
		, dwPMasterID(GAEAID_NULL)
		, dwGaeaID(0)
		, dwCeID(0)
		, vPos(0,0,0)
		, vDir(0,0,-1)
		, Action(GLAT_IDLE)
		, dwActState(NULL)
		, vTarPos(0,0,0)
		, dwFLAGS(NULL)
		, m_bUseArmSub(FALSE)
		, m_bVehicle ( FALSE )
		, m_bItemShopOpen( false )
		, dwSummonGUID(GAEAID_NULL)
		, m_dwANISUBTYPE( 0 )
	{
		for( int i=0; i < EMLANDEFFECT_MULTI; i++)
		{
			nLandEffect[i] = -1;
		}
		memset(szName, 0, sizeof(char) * CHAR_SZNAME);
		memset(szNick, 0, sizeof(char) * CHAR_SZNAME);
		memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
	}
};

#endif // GLCHARDATA_H_