#pragma once

#include <string>

#include "./GLCharData.h"
#include "./GLParty.h"
#include "./GLInventory.h"
#include "./GLPet.h"
#include "./GLVEHICLE.h"
#include "./GLSUMMON.h"

#include "../[Lib]__Engine/Sources/Common/SFileSystem.h"
#include "../[Lib]__Engine/Sources/DxMeshs/DxSkinAniMan.h"
#include "../Dependency/NetGlobal/s_NetGlobal.h"

namespace GLOGIC
{
	extern BOOL			bGLOGIC_ZIPFILE;
	extern std::string	strGLOGIC_ZIPFILE;
	extern std::string	strQUEST_ZIPFILE;
	extern std::string	strLEVEL_ZIPFILE;
	extern std::string	strNPCTALK_ZIPFILE;

	void SetFullPath( BOOL bPack );
};

enum GLCONST
{
	MAX_SERVERCHAR		= MAX_ONESERVERCHAR_NUM,
	MAX_SELECTCHAR		= 4, // 캐릭터 선택 화면에서 보이는 최대 캐릭터의 수, 현재 4개
	MAX_VIEWRANGE		= 300, // 최대 범위 (파티원 끼리 인접한가?...)

	MAX_HEAD			= 20,
	MAX_HAIR			= 20,

	MAX_SCHOOL			= 10,
	MAX_LEVEL			= 300,

	MAX_CLUBSTORAGE		= 5,
	MAX_CLUBRANK		= 10,

	GRADE_NORMAL		= 4,
	GRADE_HIGH			= 9,
	GRADE_LIMIT_MAX		= 15,
};

//	Note : 캐릭터 종류별 기본 상수.
//
struct GLCONST_CHARCLASS
{
	float				fWALKVELO;		//	캐릭의 걷기 속도.
	float				fRUNVELO;		//	캐릭의 뛰기 속도.

	float				fHP_STR;		//	str(체력) -> HP 환산 비율.
	float				fMP_SPI;		//	spi(정신) -> MP 환산 비율.
	float				fSP_STA;		//	sta(근력) -> SP 환산 비율.
	float				fHIT_DEX;		//	dex(민첩) -> HIT rate 환산 비율.
	float				fAVOID_DEX;		//	dex(민첩) -> AVOID rate 환산 비율.
	float				fDEFENSE_DEX;	//	dex(민첩) -> DEFENSE 환산 비율.

	float				fPA_POW;		//	pow(힘) -> PA(격투치) 환산 비율.
	float				fPA_DEX;		//	dex(민첩) -> PA(격투치) 환산 비율.

	float				fSA_POW;		//	pow(힘) -> SA(사격치) 환산 비율.
	float				fSA_DEX;		//	dex(민첩) -> SA(사격치) 환산 비율.

	float				fMA_DEX;		//	dex(민첩) -> MA(마법치) 환산 비율.
	float				fMA_SPI;		//	spi(정신) -> MA(마법치) 환산 비율.
	float				fMA_INT;		//	int(지력) -> MA(마법치) 환산 비율.

	float				fCONV_AP;		//	환산 기본 공격력.
	float				fCONV_DP;		//	환산 기본 방어력.
	float				fCONV_PA;		//	환산 기본 격투치.
	float				fCONV_SA;		//	환산 기본 사격치.

	SCHARSTATS			sBEGIN_STATS;	//	STATS 초기 수치.
	FCHARSTATS			sLVLUP_STATS;	//	랩업당 STATS 기본 증가 수치.

	WORD				wBEGIN_AP;		//	초기 공격력.
	WORD				wBEGIN_DP;		//	초기 방어력.
	WORD				wBEGIN_PA;		//	초기 격투치.
	WORD				wBEGIN_SA;		//	초기 사격치.

	float				fLVLUP_AP;		//	랩업당 공격력 증가.
	float				fLVLUP_DP;		//	랩업당 방어력 증가.
	float				fLVLUP_PA;		//	랩업당 격투치 증가.
	float				fLVLUP_SA;		//	랩업당 사격치 증가.

	//	에니메이션 정보. ( 시간/스탭 )
	//	server 에서만 필요한 정보.
	VECANIATTACK		m_ANIMATION[AN_TYPE_SIZE][AN_SUB_00_SIZE];

	DWORD				dwHEADNUM;
	DWORD				dwHEADNUM_SELECT;
	std::string			strHEAD_CPS[MAX_HEAD];

	DWORD				dwHAIRNUM;
	DWORD				dwHAIRNUM_SELECT;
	std::string			strHAIR_CPS[MAX_HAIR];

	std::string			strCLASS_EFFECT;

	void LoadAniSet ( char* szSkinObj );
	void ClearAniSet ();

	BOOL LOADFILE ( const char* szFileName );

	GLCONST_CHARCLASS () :
		fWALKVELO(12),
		fRUNVELO(34),

		fHP_STR(0),
		fMP_SPI(0),
		fSP_STA(0),
		fHIT_DEX(0),
		fAVOID_DEX(0),
		fPA_POW(0),
		fSA_DEX(0),
		
		fMA_DEX(0),
		fMA_SPI(0),
		fMA_INT(0),

		fCONV_AP(0),
		fCONV_DP(0),
		fCONV_PA(0),
		fCONV_SA(0),

		wBEGIN_AP(0),
		wBEGIN_DP(0),
		wBEGIN_PA(0),
		wBEGIN_SA(0),

		fLVLUP_AP(0),
		fLVLUP_DP(0),
		fLVLUP_PA(0),
		fLVLUP_SA(0),

		dwHEADNUM(0),
		dwHEADNUM_SELECT(0),

		dwHAIRNUM(0),
		dwHAIRNUM_SELECT(0)
	{
	}
	
	GLCONST_CHARCLASS ( float _fHP_STR, float _fMP_SPI, float _fSP_STA, float _fHIT_DEX,
		float _fAVOID_DEX, float _fDEFENSE_DEX, float _fPA_POW, float _fSA_DEX,
		SCHARSTATS _fBEGIN_STATS, FCHARSTATS _fLVLUP_STATS,
		WORD _wBEGIN_AP, WORD _wBEGIN_DP, WORD _wBEGIN_PA, WORD _wBEGIN_SA,
		float _fLVLUP_AP, float _fLVLUP_DP, float _fLVLUP_PA, float _fLVLUP_SA ) :
		fWALKVELO(12),
		fRUNVELO(34),

		fHP_STR(_fHP_STR),
		fMP_SPI(_fMP_SPI),
		fSP_STA(_fSP_STA),
		fHIT_DEX(_fHIT_DEX),
		fAVOID_DEX(_fAVOID_DEX),
		fDEFENSE_DEX(_fDEFENSE_DEX),
		fPA_POW(_fPA_POW),
		fSA_DEX(_fSA_DEX),

		sBEGIN_STATS(_fBEGIN_STATS),
		sLVLUP_STATS(_fLVLUP_STATS),
		wBEGIN_AP(_wBEGIN_AP),
		wBEGIN_DP(_wBEGIN_DP),
		wBEGIN_PA(_wBEGIN_PA),
		wBEGIN_SA(_wBEGIN_SA),

		fLVLUP_AP(_fLVLUP_AP),
		fLVLUP_DP(_fLVLUP_DP),
		fLVLUP_PA(_fLVLUP_PA),
		fLVLUP_SA(_fLVLUP_SA)
	{
	}

	~GLCONST_CHARCLASS()
	{
	}

private:
	//	대입 연산이 못일어나게 원천적으로 막혀있음.
	GLCONST_CHARCLASS& operator= ( GLCONST_CHARCLASS &Input )	{ GASSERT(0); };	
};

struct GLCLUBRANK
{
	DWORD	m_dwMasterLvl;
	DWORD	m_dwLivingPoint;
	DWORD	m_dwPay;
	DWORD	m_dwMember;

	GLCLUBRANK () :
		m_dwMasterLvl(0),
		m_dwLivingPoint(0),
		m_dwPay(0),
		m_dwMember(0)
	{
	}

	GLCLUBRANK ( DWORD dwMLvl, DWORD dwLP, DWORD dwPY, DWORD dwMem ) :
		m_dwMasterLvl(dwMLvl),
		m_dwLivingPoint(dwLP),
		m_dwPay(dwPY),
		m_dwMember(dwMem)
	{
	}
};

struct SPLAYERKILL
{
	DWORD		dwLEVEL;
	DWORD		dwNAME_COLOR;
	int			nPKPOINT;
	std::string	strNAME;
	float		fPK_EXP_RATE;

	DWORD		dwITEM_DROP_NUM;
	float		fITEM_DROP_RATE;
	
	float		fSHOP_2BUY_RATE;
	float		fSHOP_2SALE_RATE;

	SPLAYERKILL () :
		dwLEVEL(0),
		dwNAME_COLOR(0),
		nPKPOINT(0),
		fPK_EXP_RATE(0),

		dwITEM_DROP_NUM(0),
		fITEM_DROP_RATE(0),
		
		fSHOP_2BUY_RATE(0),
		fSHOP_2SALE_RATE(0)
	{
	}

	SPLAYERKILL ( DWORD _dwLEVEL, DWORD _dwNAME_COLOR, int _nPKPOINT, std::string _strNAME, float _fPK_EXP_RATE,
		DWORD _dwITEM_DROP_NUM, float _fITEM_DROP_RATE, float _fSHOP_2BUY_RATE, float _fSHOP_2SALE_RATE ) :
		dwLEVEL(_dwLEVEL),
		dwNAME_COLOR(_dwNAME_COLOR),
		nPKPOINT(_nPKPOINT),
		strNAME(_strNAME),
		fPK_EXP_RATE(_fPK_EXP_RATE),

		dwITEM_DROP_NUM(_dwITEM_DROP_NUM),
		fITEM_DROP_RATE(_fITEM_DROP_RATE),
		
		fSHOP_2BUY_RATE(_fSHOP_2BUY_RATE),
		fSHOP_2SALE_RATE(_fSHOP_2SALE_RATE)
	{
	}

	SPLAYERKILL ( const SPLAYERKILL &value )
	{
		operator= ( value );
	}

	SPLAYERKILL& operator= ( const SPLAYERKILL &value )
	{
		dwLEVEL = value.dwLEVEL;
		dwNAME_COLOR = value.dwNAME_COLOR;
		nPKPOINT = value.nPKPOINT;
		strNAME = value.strNAME;
		fPK_EXP_RATE = value.fPK_EXP_RATE;

		dwITEM_DROP_NUM = value.dwITEM_DROP_NUM;
		fITEM_DROP_RATE = value.fITEM_DROP_RATE;
		
		fSHOP_2BUY_RATE = value.fSHOP_2BUY_RATE;
		fSHOP_2SALE_RATE = value.fSHOP_2SALE_RATE;

		return *this;
	}
};

//	Note : 캐릭터 공통 기본 상수.
//
namespace GLCONST_CHAR
{
	enum
	{
		DIE_DECEXP_NUM	= 30,
	};

	extern BOOL			bTESTSERVER;			// 테스트 서버에만 적용.
	extern INT			nUI_KEYBOARD;			// 인터페이스 키보드 설정
	extern BOOL			bBATTLEROYAL;			// 배틀로얄
	extern INT			nMAX_FRIEND_NUMBER;		// 최대 친구, 차단 목록 개수
	extern WORD			wLEVEL_FOR_EXTREME;		// 극강부를 생성하기 위해 만족되어야 하는 레벨(lv.192)

	//	Note : 일반 상수.
	//
	extern float		fFIELD_NEXTPICK;		//	멥에 있는 아이템(돈) 주을때 처음 메시지 전송후 다음 가능 시간.
	extern WORD			wSERVER_NORMAL;
	extern WORD			wSERVER_CONGEST;
	extern WORD			wSERVER_NOVACANCY;

	//---------------------------------------------------------------------------------------[케릭터 기본]
	extern WORD			wSCHOOLNUM;
	extern std::string	strSCHOOLNAME[MAX_SCHOOL];
	extern SNATIVEID	nidSTARTMAP[MAX_SCHOOL];
	extern DWORD		dwSTARTGATE[MAX_SCHOOL];
	extern float		fCHAR_HEIGHT[GLCI_NUM_NEWSEX];

	//---------------------------------------------------------------------------------------[]
	extern BOOL			bPARTY_2OTHERSCHOOL;
	extern BOOL			bCLUB_2OTHERSCHOOL;

//	extern float		fDISPRICE;				//	아이템 재판매 가격.
    extern WORD			wMAXITEM_AGE;			//	ITEM, 최대 지속시간.
	extern WORD			wMAXITEM_HOLD;			//	ITEM, 임시 소유자 보호 시간.
	extern WORD			wMAXMOB_TRACKING;		//	최대 몹 몰이 수.

	extern WORD			wMAX_LEVEL;				//	레밸 최대 제한.
	extern WORD			wMAX_EXTREME_LEVEL;		//	극강부 레밸 최대 제한.
	extern WORD			wLVL_STATS_P;			//	렙업당 스탯 포인트 부여값.
	extern WORD			wLVL_2ndSTATS_P;			//	렙업당 스탯 포인트 부여값.
	extern float		fLVL_EXP_G;				//	렙업 경험치 산출에 필요한 환산 팩터.
	extern float		fLVL_EXP_S;				//	렙업 경험치 산출에 필요한 환산 팩터.
	extern float		fKILL_EXP_RATE;			//	"총획득 가능 경험치" * 죽일때 경험치.


	extern float		fDIE_DECEXP[DIE_DECEXP_NUM];	//	사망시 경험치 감산 정도.

	extern float		fDIE_RECOVERYEXP[DIE_DECEXP_NUM];	// 경험치 복구 비율
	extern float		fEXP_RATE_MONEY[DIE_DECEXP_NUM];	// 경험치 복구 비용

	extern float		fREC_EXP_RATE;			//	상대방을 회복시켜 줄때 획득하는 경험치의 배율.

	//---------------------------------------------------------------------------------------[연마]

	extern WORD			wGRADE_MAX;				// 연마 할수 있는 등급 제한치 
	extern WORD			wGRADE_MAX_REGI;		// 연마 할수 있는 등급 제한치(저항) 
	
	extern WORD			wDAMAGE_GRADE;			//	한 등급당 증가 공격력 수치.
	extern WORD			wDEFENSE_GRADE;			//	한 등급당 증가 방어력 수치.

	extern float		fDAMAGE_GRADE;			//	한 등급당 증가율 공격력 수치.
	extern float		fDEFENSE_GRADE;			//	한 등급당 증가율 방어력 수치.

	extern float		fDAMAGE_GRADE_TOP[GRADE_LIMIT_MAX-GRADE_HIGH];		// 최상위 등급 공격력 증가율
	extern float		fDEFENSE_GRADE_TOP[GRADE_LIMIT_MAX-GRADE_HIGH];	// 최상위 등급 방어력 증가율
	extern	WORD		wUSE_GRADE_NUM[GRADE_LIMIT_MAX-GRADE_HIGH];			// 최상위 등급 필요한 연마제 갯수

	extern WORD			wRESIST_FIRE_GRADE;		//	한 등급당 증가 저항(화) 수치.
	extern WORD			wRESIST_ICE_GRADE;		//	한 등급당 증가 저항(빙) 수치.
	extern WORD			wRESIST_ELEC_GRADE;		//	한 등급당 증가 저항(전) 수치.
	extern WORD			wRESIST_POISON_GRADE;	//	한 등급당 증가 저항(독) 수치.
	extern WORD			wRESIST_SPIRIT_GRADE;	//	한 등급당 증가 저항(정) 수치.

	//---------------------------------------------------------------------------------------
	extern WORD			wLVL_SKILL_P;			//	렙업당 스킬 포인트 부여값.
	extern WORD			wLVL_2ndSKILL_P;			//	렙업당 스킬 포인트 부여값.

	extern WORD			wMAXATRANGE_SHORT;		//	근접 공격거리.
	extern float		fPUSHPULL_VELO;			//	밀거나 당길때 이동 속도.

	extern float		fMOB_TRACING;			//	몹 추적 거리.
	extern float		fLOW_SEED_DAMAGE;		//	damage 들어갈때 최저 수용 damage.
	extern float		fCONFT_SPTY_EXP;		//	학교 대련 승리시에 전체 경험치 분배.

	extern WORD			wBODYRADIUS;			//	캐릭의 몸체 반경.

	extern float		fREACT_VALID_SCALE;		//	액션의 유효 거리 팩터.

	extern float		fUNIT_TIME;				//	회복 단위 시간.
	extern float		fHP_INC_PER;			//	단위시간당 HP 회복율(%)
	extern float		fMP_INC_PER;			//	단위시간당 MP 회복율(%)
	extern float		fSP_INC_PER;			//	단위시간당 SP 회복율(%)

	extern float		fHP_INC;				//	단위시간당 HP 회복량.
	extern float		fMP_INC;				//	단위시간당 MP 회복량.
	extern float		fSP_INC;				//	단위시간당 SP 회복량.

	extern float		fRESIST_PHYSIC_G;		//	물리 공격시 속성 저항치 적용 비율.
	extern float		fRESIST_G;				//	속성 저항치 적용 비율.
	extern WORD			wBASIC_DIS_SP;			//	기본 소모 SP.

	extern float		fLOWSP_MOTION;			//	스태미나 (%) 보다 작을때 모션 처리.
	extern float		fLOWSP_DAMAGE;			//	스태미나 부족시 Damage 감소.
	extern float		fLOWSP_HIT_DROP;		//	스태미나 부족시 명중율 감소.
	extern float		fLOWSP_AVOID_DROP;		//	스태미나 부족시 회피율 감소.
	extern DWORD		dwACCEPT_LOWERSTATS;	//	stats 부족분을 스태미나로 커버할 수 있는 가용 한도. ( 공격 무기에 한함. )

	extern float		fDAMAGE_DEC_RATE;		//	대미지 감소율.
	extern float		fDAMAGE_GRADE_K;		//	(기본공격+연마등급) 대미지 반영율.

	extern DWORD		dwCRITICAL_DAMAGE;		//	크리티컬 데미지 비율
	extern DWORD		dwCRITICAL_MAX;			//	크리티컬 확률 MAX
	extern DWORD		dwCRUSHING_BLOW_DAMAGE;	//  강한타격 데미지 비율
	extern DWORD		dwCRUSHING_BLOW_MAX;	//  강한타격 확률 max
	extern float		fCRUSH_BLOW_RANGE;		//  강한타격시 밀려나는 거리 ( 아이템 ) 
		

	//	Note : ----------------------------------------------------------------대련
	extern float		fCONFRONT_TIME;			//	대련 제한 시간.
	extern float		fCONFRONT_ELAP;			//	연속 대련 방지 지연 시간.
	extern DWORD		dwCONFRONT_MAX;			//	연속 대련 총 가능 횟수.
	extern float		fCONFRONT_RESET;		//	연속 대련 방지 리셋 시간.
	extern float		fCONFRONT_ONE_DIST;		//	대련 거리 제한. ( 0일 경우 무한대. )
	extern float		fCONFRONT_PY_DIST;		//	대련 거리 제한. ( 0일 경우 무한대. )
	extern float		fCONFRONT_CLB_DIST;		//	대련 거리 제한. ( 0일 경우 무한대. )

	extern int			nCONFRONT_WIN_LP;		//	대련 승리시 living point ( 생활 점수 ).
	extern int			nCONFRONT_LOSS_LP;		//	대련 패배시 living point ( 생활 점수 ).

	extern int			nCONFRONT_CLB_WIN_LP;	//	클럽 대련 승리시 living point
	extern int			nCONFRONT_CLB_LOSS_LP;	//	클럽 대련 패배시 living point

	//	Note : ----------------------------------------------------------------대련
	enum { EMCONFT_RC_TYPENUM = 10, EMCONFT_STATE_TYPENUM = 8 };
	extern WORD			wCONFT_RC_TYPE[EMCONFT_RC_TYPENUM];			//	대련 회복약 사용가능 횟수.
	extern float		fCONFT_STATE_TYPE[EMCONFT_STATE_TYPENUM];	//	대련 체력 확대 비율.

	extern WORD			wCONFRONT_SCHOOL_LIMIT_NUM;					//	학교간 대련 최소 인원.
	extern WORD			wCONFRONT_SCHOOL_LIMIT_LEVEL;				//	학교간 대련 최소 레벨.
	extern float		fCONFRONT_SCHOOL_EXP_SCALE;					//	학교간 대련시 습득 경험치 증가율.

	extern WORD			wCONFRONT_SCHOOL_WIN_NUM;
	extern WORD			wCONFRONT_SCHOOL_WIN_RATE;
	extern float		fCONFRONT_SCHOOL_GENITEM;

	//	Note : ----------------------------------------------------------------pk
	enum { EMPK_STATE_LEVEL = 5, };
	extern BOOL			bPK_MODE;				//	pk on/off.
	extern BOOL			bPKLESS;				//  절대적인 PK on/off.
	extern int			nPK_LIMIT_LEVEL;		//	pk 제한 레벨.
	extern int			nPK_LIMIT_DX_LEVEL;		//	pk 레벨에 따른 제한.
	extern int			nPK_TRY_BRIGHT_POINT;	//	pk 시도시 포인트.
	extern int			nPK_KILL_BRIGHT_POINT;	//	pk 죽였을 경우 포인트.
	extern int			nPK_TRY_LIVING_POINT;	//	pk 시도시 포인트.
	extern int			nPK_KILL_LIVING_POINT;	//	pk 죽였을 경우 포인트.
	extern float		fNONPK_ITEM_DROP;		//	non pk 시 아이템 드롭율.
	extern float		fPK_ITEM_DROP;			//	pk 시 아이템 드롭율.

	//	 ( pk level : 0 ~ 4 )
	//	0 - 일반학생
	//	1 - 불량학생
	//	2 - 살인자
	//	3 - 살인마
	//	4 - 살인귀
	extern DWORD		dwPK_RECALL_ENABLE_LEVEL;	//	귀환 사용 가능 레벨 설정.
	extern DWORD		dwPK_DRUG_ENABLE_LEVEL;		//	회복약 사용 가능 레벨 설정.


	extern float		fPK_JUSTNESS_TIME;		//	반격 가능한 정당방위 시간.
	extern float		fPK_SAFE_TIME;			//	pk 안전 시간.

	extern DWORD		dwPK_NORMAL_NAME_COLOR;	//	pk 수치가 없을때 이름 색깔.
	extern float		fPK_POINT_DEC_RATE;		//	pk 수치 감소율. ( 시간당. )
	extern float		fPK_POINT_DEC_PHY;		//  pk 데미지 감소율 ( 물리 ).
	extern float		fPK_POINT_DEC_MAGIC;	//  pk 데미지 감소율 ( 마법 ).
	
	extern SPLAYERKILL	sPK_STATE[EMPK_STATE_LEVEL];

	//	Note : ----------------------------------------------------------------상태이상
	enum { EMSTATEBLOW_LEVEL_SIZE = 10, EMSTATEBLOW_LEVEL_BASE = 1, };
	extern int			nSTATEBLOW_LEVEL[EMSTATEBLOW_LEVEL_SIZE];

	//	Note : ----------------------------------------------------------------경험치 Table
	//
	enum { EXPTABLE_SIZE = 52, EXPTABLE_LVL = 19, EXPBASEINDEX = 10, EXP_LVL_STEP = 5,
	EXPTABLE_RANGE = 61,  EXPTABLE_RANGE_BASE = 30 };
	extern WORD			wEXPTABLE[EXPTABLE_SIZE][EXPTABLE_LVL];
	extern float		fEXP_RATE_TABLE[EXPTABLE_RANGE];

	extern LONGLONG		lnEXP_MAX_TABLE[MAX_LEVEL];
	extern LONGLONG		lnEXP_MAX_TABLE_2nd[MAX_LEVEL];
	extern DWORD		dwEXP_PC_TABLE[MAX_LEVEL];

	//	Note : ----------------------------------------------------------------파티 경험치.
	extern float		fPARTYEXPRANGE;				//	경험치 수신이 가능한 최대 거리.
	extern float		fPARTYEXP_S;				//	파티 경험치 분배시 레벨차이에 따른 감소비율.
	extern WORD			aPARTYEXP_ONE[MAXPARTY];
	extern WORD			aPARTYKILLEXP_ONE[MAXPARTY];

	//	Note : ----------------------------------------------------------------클럽
	extern DWORD		dwCLUB_PARTYNUM;			//	클럽 창설시 최소 파티(초기) 인원.
	extern DWORD		dwCLUB_DISSOLUTION_DAY;		//	클럽 해체 유보 일.
	extern DWORD		dwCLUB_JOINBLOCK_DAY;		//	클럽 탈퇴시 가입 제한 일.
	extern DWORD		dwMAX_CLUBRANK;
	extern GLCLUBRANK	sCLUBRANK[MAX_CLUBRANK];	//	클럽 랭크별 상수.
	extern DWORD		dwCLUB_AUTHORITY;			//	클럽마스터 위임 제한 시간			
	extern DWORD		dwCLUB_ALLIANCE_SEC;		//	클럽 동맹 탈퇴, 제명시 재가입 재한시간.
	extern DWORD		dwCLUB_ALLIANCE_DIS;		//	클럽 동맹 해체시 결성 재한시간.
	extern DWORD		dwCLUB_ALLIANCE_NUM;		//	클럽 동맹 최대 숫자.

	extern DWORD		bCLUB_BATTLE;				//	클럽 배틀 가능 여부
	extern DWORD		dwCLUB_BATTLE_MAX;			//  클럽 배틀 동시 진행 갯수
	extern DWORD		dwCLUB_BATTLE_RANK;			//  클럽 배틀 랭크 제한
	extern DWORD		dwCLUB_BATTLE_TIME;			//  클럽 배틀 시간(분)
	extern DWORD		dwCLUB_BATTLE_TIMEMIN;		//	클럽 배틀 최소시간(분)
	extern DWORD		dwCLUB_BATTLE_TIMEMAX;		//	클럽 배틀 최대시간(분)
	extern DWORD		dwCLUB_BATTLE_GUID_TIME;	//  클럽 배틀 선도전 제약 시간(분)
	extern DWORD		dwCLUB_BATTLE_DIS_TIME;		//  클럽 배틀 휴전 및 항복 가능한 시간(분)
	extern BOOL			bCLUB_BATTLE_ALLIANCE;		//  클럽 배틀시 동맹 자동 참가 기능

	extern BOOL			bCLUB_DEATHMATCH;			//	클럽 데스매치 가능 여부
	extern DWORD		dwCLUB_DEATHMATCH_MEM;		//	클럽 데스매치 가능 멤버수
	

	//	Note : ----------------------------------------------------------------선도전
	extern float		fMAX_COMMISSION;			//	최대 선도 지역 수수료 제한.
	extern float		fDEFAULT_COMMISSION;		//	비 선도 지역의 수수료.
	extern float		fEARNING_RATE;				//	수수료 수익율.
	extern float		fEARNING_RATE_NPC;			//	NPC 소환 수수료 수익율.
	extern float		fCDCERTIFY_DIST;			//	인증 가능 거리.
	extern float		fCDCERTIFY_DIST2;			//	인증 가능 거리.
	extern float		fCDCERTIFY_TIME;			//	인증 소요 시간.

	//	Note : ----------------------------------------------------------------배율 조정
	extern float		fEXP_SCALE;					//	Note : 경험치 배율.
	extern float		fITEM_DROP_SCALE;			//	Note : 아이탬 드롭율.
	extern float		fMONEY_DROP_SCALE;			//	Note : 돈 드롭율.
	extern float		fITEM_DROP_LOWER_SCALE;		//	Note : 희귀 아이탬 드롭율.
	//	Note : ----------------------------------------------------------------

	//	Note : ----------------------------------------------------------------학원 프리 pk
	extern bool			bSCHOOL_FREE_PK_ALWAYS;		//	학원간 프리 pk 항상 가능 여부.
	extern bool			bSCHOOL_FREE_PK;			//	학원간 프리 pk on/off.
	extern bool			bSCHOOL_FREE_PK_TIME_REAL;	//	학원간 프리 pk 설정 시간 ( on : 실제 시간, off : 게임시간 )
	extern bool			bSCHOOL_FREE_PK_Z_FORCED;	//	학원간 프리 pk 강제 공격.
	extern DWORD		dwSCHOOL_FREE_PK_TIME_START;//	학원간 프리 pk 시작 시간.
	extern DWORD		dwSCHOOL_FREE_PK_TIME_TERM;	//	학원간 프리 pk 지속 시간.
	extern float		fSCHOOL_FREE_PK_ITEM_DROP;	//	학원간 프리 pk 아이템 드롭율 가산값. ( 기본 드롭율 + 가산값. )
	//	Note : ----------------------------------------------------------------

	//	Note : ----------------------------------------------------------------기능 설정.
	extern bool			bENCHANT_TERMINATE_ITEM;	//	인첸트시 아이템 파손 가능.
	extern bool			bMONEY_DROP2FIELD;			//	돈 바닥에 버리기 가능 여부.

	//	Note : ----------------------------------------------------------------연마 확율.
	extern float		fGRADE_RATE[GRADE_LIMIT_MAX];
	extern float		fGRADE_RESET_RATE[GRADE_LIMIT_MAX];
	extern float		fGRADE_TERMINATE_RATE[GRADE_LIMIT_MAX];

	//	Note : ----------------------------------------------------------------사운드
	extern std::string	strGRINDING_SUCCEED;
	extern std::string	strGRINDING_FAIL;
	extern std::string	strGRINDING_RESET;
	extern std::string	strGRINDING_BROKEN;

	extern std::string	strGAMBLING_SHUFFLE;	// Monster7j
	extern std::string	strGAMBLING_WIN;		
	extern std::string	strGAMBLING_LOSE;	

	extern std::string	strITEMDROP_SUIT;
	extern std::string	strITEMDROP_WAPON;
	extern std::string	strITEMDROP_SHOES;
	extern std::string	strITEMDROP_RING;
	extern std::string	strITEMDROP_QBOX;

	extern std::string	strITEMDROP_SCROLL;
	extern std::string	strITEMDROP_COIN;
	extern std::string	strITEMDROP_DRUGS;

	extern std::string	strPICKUP_ITEM;
	extern std::string	strQITEM_FACT;
	extern std::string	strQITEM_BGM[QUESTION_SIZE];

	//	Note : ----------------------------------------------------------------효과
	extern std::string	strSELECT_CHAR;			//	로비에서 케릭터 선택시.

	extern std::string	strREBIRTH_EFFECT;		//	캐릭 부활 이팩트.
	extern std::string	strLEVELUP_EFFECT;		//	래밸 업 이팩트.
	
	extern std::string	strSRIKE_ELMT_EFFECT[EMELEMENT_MAXNUM]; //	타격시 이팩트.

	extern std::string	strAMBIENT_EFFECT;		//	타격시 Ambient 이펙트
	extern std::string	strERASE_EFFECT;		//	몹 사라지는 이팩트.

	extern std::string	strSKILL_LEARN_EFFECT;	//	스킬 습득시.
	extern std::string	strSKILL_UP_EFFECT;		//	스킬 업.

	extern std::string strQUEST_START;			//	퀘스트 시작.
	extern std::string strQUEST_END_SUCCEED;	//	퀘스트 성공.
	extern std::string strQUEST_END_FAIL;		//	퀘스트 실패.

	extern std::string	strMOB_GEM_EFFECT;		//	몹 생성시 이팩트.
	extern std::string	strMOB_DIE_BLOOD;		//	몹이 죽었는지.

	// PET
	extern std::string	strPET_GEN_EFFECT;		// 팻 생성시 이팩트

	// Vehicle
	extern std::string  strVEHICLE_GEN_EFFECT;	// 탑승 이펙트

	extern std::string	strCONFRONT_BOUND;		//	대련 경계선 표시 효과.

	extern std::string	strHALFALPHA_EFFECT;	//	투명 케릭터.

	extern std::string	strBLOW_NUMB_EFFECT;
	extern std::string	strBLOW_STUN_EFFECT;
	extern std::string	strBLOW_STONE_EFFECT;
	extern std::string	strBLOW_BURN_EFFECT;
	extern std::string	strBLOW_FROZEN_EFFECT;

	extern std::string	strBLOW_MAD_EFFECT;
	extern std::string	strBLOW_POISON_EFFECT;
	extern std::string	strBLOW_CURSE_EFFECT;

	extern std::string	strBLOW_BODY_NUMB_EFFECT;
	extern std::string	strBLOW_BODY_STUN_EFFECT;
	extern std::string	strBLOW_BODY_STONE_EFFECT;
	extern std::string	strBLOW_BODY_BURN_EFFECT;
	extern std::string	strBLOW_BODY_FROZEN_EFFECT;

	extern std::string	strBLOW_BODY_MAD_EFFECT;
	extern std::string	strBLOW_BODY_POISON_EFFECT;
	extern std::string	strBLOW_BODY_CURSE_EFFECT;

	extern std::string strBLOW_EFFECTS[EMBLOW_SIZE];
	extern std::string strBLOW_BODY_EFFECTS[EMBLOW_SIZE];

	extern std::string strCRUSHING_BLOW_EFFECT;		// 강한타격 이펙트

	extern GLCONST_CHARCLASS	cCONSTCLASS[GLCI_NUM_NEWSEX];	//	캐릭터 클래스별 상수.

	extern std::string strCLASS_INIT[MAX_SCHOOL][GLCI_NUM_NEWSEX];

	extern std::vector<std::string>	vecGUIDANCE_FILE;
	extern std::vector<std::string>	vecClubDM_FILE;

	//	Note : 캐릭터 클래스별 초기 설정값.
	//
	extern char					szCharSkin[GLCI_NUM_NEWSEX][MAX_PATH];	//	캐릭별 초기 형상.

	BOOL	LOADFILE ( char* szFileName, BOOL bServer=TRUE );
	BOOL	EXP_LOADFILE ( char* szFileName );

	BOOL	EXP_PC_LOADFILE ( char* szFileName );
	BOOL	EXP_MAX_LOADFILE ( char* szFileName );
	BOOL	EXP_MAX_2nd_LOADFILE ( char* szFileName );

	BOOL VAID_CHAR_DATA2 ( WORD wSchool, EMCHARINDEX emIndex );
	SCHARDATA2& GET_CHAR_DATA2 ( WORD wSchool, EMCHARINDEX emIndex );

	inline const char* GETSTRIKE_EFFECT ( EMELEMENT emELEMENT=EMELEMENT_SPIRIT )
	{
		GASSERT(emELEMENT>=0);
		GASSERT(emELEMENT<EMELEMENT_MAXNUM);

		return strSRIKE_ELMT_EFFECT[emELEMENT].c_str();
	}

	const char* GETSCHOOLNAME ( WORD wSCHOOL );
	void SETEXP_SCALE ( float fScale );
	float GETEXP_SCALE ();
	DWORD GETEXP ( int nAttackerLev, int nDefenserLev );
	float GETEXP_RATE ( int nAttackerLev, int nDefenserLev );

	DWORD GETEXP_PC ( WORD wLev );
};

struct PETSTYLE
{
	std::string	strSTYLE_CPS[MAX_HAIR];
	WORD		wSTYLE_COLOR[MAX_HAIR];
	WORD		wSTYLENum;


	PETSTYLE () :
	wSTYLENum(0)
	{
		memset( wSTYLE_COLOR,0,sizeof(wSTYLE_COLOR));
	}
};

namespace GLCONST_PET
{
	extern PGLPET		pGLPET[PETTYPE_SIZE];
	extern char			szPetInit[PETTYPE_SIZE][MAX_PATH];
	extern PETSTYLE		sPETSTYLE[PETTYPE_SIZE];
	extern BOOL			bCHANGEABLECOLOR[PETTYPE_SIZE];

	extern float		fRunArea;
	extern float		fWalkArea;
	extern float		fOwnerDistance;
	extern int			nMAXVIEWRANGE;
	extern int			nFullDecrement[PETTYPE_SIZE];
	extern float		fMaginotLineHP;
	extern float		fMaginotLineMP;
	extern float		fMaginotLineSP;

	PGLPET   GetPetData ( PETTYPE emTYPE );
	PETSTYLE GetPetStyle ( PETTYPE emTYPE );
	BOOL     IsChangeablePetColor ( PETTYPE emTYPE );
};

namespace GLCONST_VEHICLE
{
	extern PGLVEHICLE		pGLVEHICLE[VEHICLE_TYPE_SIZE];
	extern char				szVehicleInit[VEHICLE_TYPE_SIZE][MAX_PATH];

	extern int				nFullDecrVehicle[VEHICLE_TYPE_SIZE]; // 포만감 감소치
	
	PGLVEHICLE		GetVehicleData ( VEHICLE_TYPE emTYPE );

};

namespace GLCONST_SUMMON
{
	extern PGLSUMMON		pGLSUMMON[SUMMON_TYPE_SIZE];
	extern char				szSummonInit[SUMMON_TYPE_SIZE][MAX_PATH];

	PGLSUMMON GetSummonData ( SUMMON_TYPE emTYPE );


};

namespace GLCONST_ATTENDANCE
{
	const int MAX_REWARD = 10;

	struct	ATTEND_REWARD
	{
		int				nComboDay;
		SNATIVEID		idReward;

		ATTEND_REWARD()
			: nComboDay ( 0 )
			, idReward( NATIVEID_NULL() )
		{
		}
	};


	extern ATTEND_REWARD	sATEEND_REWARD[MAX_REWARD];
	extern DWORD			dwMAXREWARD;
	extern DWORD			dwAttendTime;

	BOOL LoadFile( std::string strFileName, bool bServer );
};

namespace MINIGAME_ODDEVEN // 미니 게임, 홀짝 게임
{
	const unsigned int MAX_ROUND = 5;

	extern float	fReturnRate[MAX_ROUND];		// 배당금 배율 인수(최종배당금 = 원금*1회차인수*2회차인수...)
	extern float	fSuccessRate[MAX_ROUND];	// 성공 활률
	extern float	fTimeLimit;					// 제한 시간(초)
	extern float	fShuffleTime;				// 섞임 시간(초)
	extern UINT		uiMaxBattingMoney;			// 최대 배팅 가능 금액
};

namespace HAIRCOLOR
{
	enum { MAXHAIRCOLOR = 8 };

	extern WORD wHairColor[GLCI_NUM_NEWSEX][MAXHAIRCOLOR];

	extern WORD GetHairColor ( WORD wClass, WORD wStyle ); 
};


class GLogicData
{
public:
	HRESULT LoadData ( BOOL bServer, bool bPastLoad = FALSE );
	HRESULT ClearData ();

	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects ();

protected:
	GLogicData(void);

public:
	~GLogicData(void);

public:
	static GLogicData& GetInstance();
};
