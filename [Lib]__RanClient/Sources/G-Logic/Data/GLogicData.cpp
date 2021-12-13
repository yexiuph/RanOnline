#include "pch.h"
#include "StringFile.h"
#include "StringUtils.h"
#include "GLogic.h"
#include "GLCharData.h"
#include "GLCrowData.h"
#include "GLItemMan.h"
#include "DxSkinDataDummy.h"
#include "GLQuestMan.h"
#include "GLBusStation.h"
#include "GLTaxiStation.h"
#include "GLPeriod.h"
#include "GLogicData.h"
#include "GLCommentFile.h"
#include "GLStringTable.h"
#include "Unzipper.h"
#include "GLTexfile.h"
#include "GLItemMixMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// note : zip 파일 패스 설정
//
namespace GLOGIC
{
	std::string	strGLOGIC_ZIPFILE;
	std::string	strQUEST_ZIPFILE;
	std::string	strLEVEL_ZIPFILE;
	std::string	strNPCTALK_ZIPFILE;

	void SetFullPath( BOOL bPack )
	{
		if( bPack )
		{
			strGLOGIC_ZIPFILE	= "\\data\\glogic\\";
			strQUEST_ZIPFILE	= "\\data\\glogic\\quest\\";
			strLEVEL_ZIPFILE	= "\\data\\glogic\\level\\";
			strNPCTALK_ZIPFILE	= "\\data\\glogic\\npctalk\\";
		}
		else
		{
			strGLOGIC_ZIPFILE = GLOGIC::GetPath();
			strGLOGIC_ZIPFILE += "GLogic.rcc";

			strQUEST_ZIPFILE = GLOGIC::GetPath();
			strQUEST_ZIPFILE += "Quest\\Quest.rcc";

			strLEVEL_ZIPFILE = GLOGIC::GetPath();
			strLEVEL_ZIPFILE += "Level.rcc";

			strNPCTALK_ZIPFILE = GLOGIC::GetPath();
			strNPCTALK_ZIPFILE += "NpcTalk\\NpcTalk.rcc";
			
			CUnzipper::LOADFILE_RCC( strGLOGIC_ZIPFILE );
			CUnzipper::LOADFILE_RCC( strQUEST_ZIPFILE );
			CUnzipper::LOADFILE_RCC( strLEVEL_ZIPFILE );
			CUnzipper::LOADFILE_RCC( strNPCTALK_ZIPFILE );
		}
	}
};

//----------------------------------------------------------------------------------------
void GLCONST_CHARCLASS::LoadAniSet ( char* szSkinObj )
{
	if ( szSkinObj[0]=='\0' )	return;

	DxSkinDataDummy DataDummy;
	BOOL bOK = DataDummy.LoadFile ( szSkinObj );
	if ( !bOK )
	{
		CString StrMsg;
		StrMsg.Format ( "[%s] Skin File Load Fail.", szSkinObj );
		MessageBox ( NULL, StrMsg.GetString(), "ERROR", MB_OK );
		return;
	}

	size_t nSIZE = DataDummy.m_vecANIMINFO.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		const SANIMCONINFO *pAnimConInfo = DataDummy.m_vecANIMINFO[n];

		SANIATTACK sAniAttack;
		sAniAttack.Assign ( pAnimConInfo );
		m_ANIMATION[pAnimConInfo->m_MainType][pAnimConInfo->m_SubType].push_back ( sAniAttack );
	}
}

void GLCONST_CHARCLASS::ClearAniSet ()
{
	for ( int i=0; i<AN_SUB_00_SIZE; ++i )
	for ( int j=0; j<AN_TYPE_SIZE; ++j )
	{
		m_ANIMATION[j][i].clear ();
	}
}


//----------------------------------------------------------------------------------------
namespace GLCONST_CHAR
{
	BOOL		bTESTSERVER			= FALSE;			//	테스트 서버에만 적용.
	INT			nUI_KEYBOARD		= 0;				// 인터페이스 키보드 설정. 기본 값은 키보드, UI 둘다 사용
	BOOL		bBATTLEROYAL		= FALSE;			// 배틀로얄
	INT			nMAX_FRIEND_NUMBER	= 100;				// 최대 친구, 차단 목록 개수
	WORD		wLEVEL_FOR_EXTREME	= 192;				// 극강부를 생성하기 위해 만족되어야 하는 레벨(lv.192)

	float		fFIELD_NEXTPICK		= 0.4f;
	WORD		wSERVER_NORMAL		= 40;				//	서버 보통.
	WORD		wSERVER_CONGEST		= 70;				//	서버 혼잡.
	WORD		wSERVER_NOVACANCY	= 95;				//  서버 풀, 접속 불가

//	float		fDISPRICE			= 0.3f;				//	아이템 재판매 가격.
	WORD		wMAXITEM_AGE		= 120;
	WORD		wMAXITEM_HOLD		= 10;

	float		fCHAR_HEIGHT[GLCI_NUM_NEWSEX]	= { 20.0f, 20.0f, 17.0f, 17.0f, 20.0f, 17.0f, 17.0f, 17.0f, 20.0f, 20.0f };

	//	Note : 캐릭터 기본 상수.
	//
	WORD		wSCHOOLNUM = 1;
	std::string	strSCHOOLNAME[MAX_SCHOOL]	= { "성문" };
	SNATIVEID	nidSTARTMAP[MAX_SCHOOL]		= { SNATIVEID(0,0) };
	DWORD		dwSTARTGATE[MAX_SCHOOL]		= { 0 };

	BOOL		bPARTY_2OTHERSCHOOL = TRUE;
	BOOL		bCLUB_2OTHERSCHOOL = TRUE;

	WORD		wMAX_LEVEL			= 255;			//	레밸 최대 제한.
	WORD		wMAX_EXTREME_LEVEL	= 255;			//	극강부 레밸 최대 제한.
	WORD		wLVL_STATS_P		= 3;			//	렙업당 스탯 포인트 부여값.
	WORD		wLVL_2ndSTATS_P		= 4;			//	렙업당 스탯 포인트 부여값. 극강부
	float		fLVL_EXP_G			= 25.0f;		//	렙업 경험치 산출에 필요한 환산 팩터.
	float		fLVL_EXP_S			= 1.3f;			//	렙업 경험치 산출에 필요한 환산 팩터.
	float		fREC_EXP_RATE		= 2.0f;			//	상대방을 회복시켜 줄때 획득하는 경험치의 배율.
	float		fKILL_EXP_RATE		= 0.5f;			//	"총획득 가능 경험치" * 죽일때 경험치.

	float	fDIE_DECEXP[DIE_DECEXP_NUM] =		// 죽을때 깍이는 경험치 비율
	{
		0, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	float	fDIE_RECOVERYEXP[DIE_DECEXP_NUM] =	// 죽었을때 회복할수 있는 경험치 비율
	{
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	float	fEXP_RATE_MONEY[DIE_DECEXP_NUM]	=	// 경험치당 금액 비율
	{
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	WORD		wGRADE_MAX			= 9;			//	연마 할수 있는 등급 제한치 
	WORD		wGRADE_MAX_REGI		= 9;

	WORD		wDAMAGE_GRADE		= 3;			//	한 등급당 증가 공격력 수치.
	WORD		wDEFENSE_GRADE		= 2;			//	한 등급당 증가 방어력 수치.

	float		fDAMAGE_GRADE		= 10;			//	한 등급당 증가율 공격력 수치.
	float		fDEFENSE_GRADE		= 5;			//	한 등급당 증가율 방어력 수치.

	float		fDAMAGE_GRADE_TOP[GRADE_LIMIT_MAX-GRADE_HIGH]	=	// 최상위 등급 공격력 증가율
	{
		1.0f, 1.0f, 1.0f, 1.0f,	1.0f, 1.0f
	};

	float		fDEFENSE_GRADE_TOP[GRADE_LIMIT_MAX-GRADE_HIGH]	=	// 최상위 등급 공격력 증가율
	{
		1.0f, 1.0f, 1.0f, 1.0f,	1.0f, 1.0f
	};

	WORD		wUSE_GRADE_NUM[GRADE_LIMIT_MAX-GRADE_HIGH]	=	// 최상위 등급시 필요한 연마제 갯수
	{
		1, 2, 3, 4, 5, 6	
	};

	WORD		wRESIST_FIRE_GRADE	= 2;			//	한 등급당 증가 저항(화) 수치.
	WORD		wRESIST_ICE_GRADE	= 2;			//	한 등급당 증가 저항(빙) 수치.
	WORD		wRESIST_ELEC_GRADE	= 2;			//	한 등급당 증가 저항(전) 수치.
	WORD		wRESIST_POISON_GRADE= 2;			//	한 등급당 증가 저항(독) 수치.
	WORD		wRESIST_SPIRIT_GRADE= 2;			//	한 등급당 증가 저항(정) 수치.

	WORD		wLVL_SKILL_P		= 1;			//	렙업당 스킬 포인트 부여값.
	WORD		wLVL_2ndSKILL_P		= 2;			//	렙업당 스킬 포인트 부여값. 극강부

	WORD		wMAXATRANGE_SHORT	= 2;			//	근접 공격거리.
	float		fPUSHPULL_VELO		= 40.0f;		//	밀거나 당길때 이동 속도.
	WORD		wMAXMOB_TRACKING	= 3;			//	최대 몹 몰이 수.

	float		fMOB_TRACING		= 90.0f;		//	몹 추적 거리.
	float		fLOW_SEED_DAMAGE	= 0.05f;		//	damage 들어갈때 최저 수용 damage 비율.
	float		fCONFT_SPTY_EXP		= 0.005f;		//	학교 대련 승리시에 전체 경험치 분배.

	WORD		wBODYRADIUS			= 4;			//	캐릭의 몸체 반경.
	float		fWALKVELO			= 10.0f;		//	캐릭의 걷기 속도.
	float		fRUNVELO			= 36.0f;		//	캐릭의 뛰기 속도.

	float		fREACT_VALID_SCALE	= 4.0f;			//	액션의 유효 거리 팩터.

	float		fUNIT_TIME			= 1.0f;			//	회복 단위 시간.
	float		fHP_INC_PER			= 0.3f*0.01f;	//	단위시간당 HP 회복율(%)
	float		fMP_INC_PER			= 0.3f*0.01f;	//	단위시간당 MP 회복율(%)
	float		fSP_INC_PER			= 0.5f*0.01f;	//	단위시간당 SP 회복량.

	float		fHP_INC				= 0;			//	단위시간당 HP 회복량.
	float		fMP_INC				= 0;			//	단위시간당 MP 회복량.
	float		fSP_INC				= 0;			//	단위시간당 SP 회복량.

	float		fRESIST_PHYSIC_G	= 0.5f;			//	물리 공격시 속성 저항치 적용 비율.
	float		fRESIST_G			= 0.5f;			//	마법 공격시 속성 저항치 적용 비율.
	WORD		wBASIC_DIS_SP		= 1;			//	기본 공격시 소모 SP 량.

	float		fLOWSP_MOTION		= 0.20f;		//	스태미나 (%) 보다 작을때 모션 처리.
	float		fLOWSP_DAMAGE		= 0.50f;		//	스태미나 부족시 Damage 감소.
	float		fLOWSP_HIT_DROP		= 0.25f;		//	스태미나 부족시 명중율 감소.
	float		fLOWSP_AVOID_DROP	= 0.50f;		//	스태미나 부족시 회피율 감소.
	DWORD		dwACCEPT_LOWERSTATS	= 5;			//	stats 부족분을 스태미나로 커버할 수 있는 가용 한도. ( 공격 무기에 한함. )

	float		fDAMAGE_DEC_RATE	= 40000.0f;		//	대미지 흡수율(반비례)
	float		fDAMAGE_GRADE_K		= 10.0f;		//	(기본공격+연마등급) 대미지 반영율.(반비례)

	DWORD		dwCRITICAL_DAMAGE	= 120;			//	크리티컬 데미지 비율
	DWORD		dwCRITICAL_MAX		= 40;			//	크리티컬 확률 MAX

	DWORD		dwCRUSHING_BLOW_DAMAGE	= 150;		//  강한타격 데미지 비율
	DWORD		dwCRUSHING_BLOW_MAX		= 20;		//  강한타격 확률 max
	float		fCRUSH_BLOW_RANGE		= 10.0f;	//  강한타격시 밀려나는 거리 ( 아이템 ) 

	float		fCONFRONT_TIME		= 180.0f;		//	대련 제한 시간.
	float		fCONFRONT_ELAP		= 60.0f;		//	연속 대련 방지 지연 시간.
	DWORD		dwCONFRONT_MAX		= 3;			//	연속 대련 총 가능 횟수.
	float		fCONFRONT_RESET		= 3600.0f;		//	연속 대련 방지 리셋 시간.
	float		fCONFRONT_ONE_DIST	= 0;			//	대련 거리 제한. ( 0일 경우 무한대. )
	float		fCONFRONT_PY_DIST	= 0;			//	대련 거리 제한. ( 0일 경우 무한대. )
	float		fCONFRONT_CLB_DIST	= 0;			//	대련 거리 제한. ( 0일 경우 무한대. )

	int			nCONFRONT_WIN_LP	= 1;			//	대련 승리시 living point ( 생활 점수 ).
	int			nCONFRONT_LOSS_LP	= -1;			//	대련 패배시 living point ( 생활 점수 ).

	int			nCONFRONT_CLB_WIN_LP	= 5;
	int			nCONFRONT_CLB_LOSS_LP	= -5;

	WORD		wCONFRONT_SCHOOL_LIMIT_NUM		= 4;	//	학교간 대련 최소 인원.
	WORD		wCONFRONT_SCHOOL_LIMIT_LEVEL	= 20;	//	학교간 대련 최소 레벨.
	float		fCONFRONT_SCHOOL_EXP_SCALE		= 1.5f;	//	학교간 대련시 습득 경험치 증가율.

	WORD		wCONFRONT_SCHOOL_WIN_NUM		= 200;	//	최고 승수 학교 선발 최소 승수.
	WORD		wCONFRONT_SCHOOL_WIN_RATE		= 30;	//	최고 승수 학교 선발 최소 승율.
	float		fCONFRONT_SCHOOL_GENITEM		= 1.1f;	//	최고 승수 학교에게 아이템 드롭 증가율.

    WORD		wCONFT_RC_TYPE[EMCONFT_RC_TYPENUM] = { 0, 4, 6, 8, 10, 12, 16, 20, 30, 0xffff };			//	대련 회복약 사용가능 횟수.
	float		fCONFT_STATE_TYPE[EMCONFT_STATE_TYPENUM] = { 1.0f, 1.2f, 1.5f, 1.6f, 2.0f, 2.5f, 3.0f, 4.0f };	//	대련 체력 확대 비율.

	int			nSTATEBLOW_LEVEL[EMSTATEBLOW_LEVEL_SIZE] = { +10,  +8,  +6, +3, 0,  -2,  -4,  -6,  -8,  -10 };

	BOOL		bPK_MODE = TRUE;
	BOOL		bPKLESS = FALSE;
	int			nPK_LIMIT_LEVEL = 20;			//	pk 제한 레벨.
	int			nPK_LIMIT_DX_LEVEL = 10;		//	pk 레벨에 따른 제한.
	int			nPK_TRY_BRIGHT_POINT = -1;		//	pk 시도시 포인트.
	int			nPK_KILL_BRIGHT_POINT = -9;		//	pk 죽였을 경우 포인트.
	int			nPK_TRY_LIVING_POINT = -1;		//	pk 시도시 포인트.
	int			nPK_KILL_LIVING_POINT = -10;	//	pk 죽였을 경우 포인트.
	float		fNONPK_ITEM_DROP = 0;			//	non pk 시 아이템 드롭율.
	float		fPK_ITEM_DROP = 0;				//	pk 시 아이템 드롭율.

	DWORD		dwPK_RECALL_ENABLE_LEVEL = 1;	//	귀환 사용 가능 레벨 설정.
	DWORD		dwPK_DRUG_ENABLE_LEVEL = 2;		//	회복약 사용 가능 레벨 설정.

	float		fPK_JUSTNESS_TIME = 60.0f;		//	반격 가능한 정당방위 시간.
	float		fPK_SAFE_TIME = 10.0f;			//	pk 안전 시간.

	DWORD		dwPK_NORMAL_NAME_COLOR = 0xffffffff;	//	pk 수치가 없을때 이름 색깔.
	float		fPK_POINT_DEC_RATE = 4.0f*3600.0f/GLPeriod::REALTIME_TO_VBR;	//	pk 수치 감소율. ( 시간당. )
	float		fPK_POINT_DEC_PHY = 0.5f;		//  pk 데미지 감소율 ( 물리 ).
	float		fPK_POINT_DEC_MAGIC = 0.5f;		//  pk 데미지 감소율 ( 마법 ).


	SPLAYERKILL	sPK_STATE[EMPK_STATE_LEVEL] =
	{
		SPLAYERKILL(5,0xFFFF00,-20,"일반학생",120.0f,1,10.0f,130.0f,90.0f),
		SPLAYERKILL(4,0xFFE400,-80,"불량학생",140.0f,2,10.0f,0.0f,0.0f),
		SPLAYERKILL(3,0xFFBC00,-160,"살인자",180.0f,3,30.0f,0.0f,0.0f),
		SPLAYERKILL(2,0xFF9700,-300,"살인마",240.0f,4,60.0f,0.0f,0.0f),
		SPLAYERKILL(1,0xFF0000,-INT_MAX,"살인귀",350.0f,5,90.0f,0.0f,0.0f),
	};

	//	Note : 경험치 Table
	//
	//
	WORD		wEXPTABLE[EXPTABLE_SIZE][EXPTABLE_LVL] = 
	{
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 36, 44, 55, 70, 90, 116, 149, 190 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 37, 46, 58, 74, 95, 122, 156, 198 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 38, 48, 61, 78, 100, 128, 163, 206 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 39, 50, 64, 82, 105, 134, 170, 214 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 40, 52, 67, 86, 110, 140, 177, 222 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 41, 54, 70, 90, 115, 146, 184, 230 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 30, 42, 56, 73, 94, 120, 152, 191, 238 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 15, 24, 31, 44, 59, 77, 99, 126, 159, 199, 247 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 16, 25, 32, 46, 62, 81, 104, 132, 166, 207, 256 },
		{ 0, 0, 1, 1, 2, 3, 4, 9, 16, 26, 33, 48, 65, 85, 109, 138, 173, 215, 265 },
		{ 0, 0, 1, 2, 2, 3, 5, 10, 17, 27, 34, 50, 68, 89, 114, 144, 180, 223, 274 },
		{ 0, 0, 1, 2, 2, 3, 5, 10, 17, 28, 35, 52, 71, 93, 119, 150, 187, 231, 283 },
		{ 0, 0, 1, 2, 2, 3, 5, 10, 18, 28, 36, 54, 74, 97, 124, 156, 194, 239, 292 },
		{ 0, 0, 1, 2, 2, 3, 5, 11, 18, 29, 37, 56, 77, 101, 129, 162, 201, 247, 301 },
		{ 0, 0, 1, 2, 3, 3, 5, 11, 19, 30, 38, 58, 80, 105, 134, 168, 208, 255, 310 },
		{ 0, 0, 1, 2, 3, 3, 5, 11, 19, 31, 39, 60, 83, 109, 139, 174, 215, 263, 319 },
		{ 0, 0, 1, 2, 3, 4, 6, 12, 20, 32, 40, 62, 86, 113, 144, 180, 222, 271, 328 },
		{ 0, 0, 1, 2, 3, 4, 6, 12, 20, 32, 41, 64, 89, 117, 149, 186, 229, 279, 337 },
		{ 0, 0, 1, 2, 3, 4, 6, 12, 21, 33, 42, 66, 92, 121, 154, 192, 236, 287, 346 },
		{ 0, 0, 1, 2, 3, 4, 6, 13, 22, 35, 44, 69, 96, 126, 160, 199, 244, 296, 356 },
		{ 0, 0, 1, 2, 3, 4, 6, 13, 23, 36, 46, 72, 100, 131, 166, 206, 252, 305, 366 },
		{ 0, 0, 1, 2, 3, 4, 7, 14, 24, 38, 48, 75, 104, 136, 172, 213, 260, 314, 376 },
		{ 0, 1, 2, 3, 4, 5, 7, 15, 25, 40, 50, 78, 108, 141, 178, 220, 268, 323, 386 },
		{ 0, 1, 2, 3, 4, 5, 7, 15, 26, 41, 52, 81, 112, 146, 184, 227, 276, 332, 396 },
		{ 0, 1, 2, 3, 4, 5, 8, 16, 27, 43, 54, 84, 116, 151, 190, 234, 284, 341, 406 },
		{ 0, 1, 2, 3, 4, 5, 8, 16, 28, 44, 56, 87, 120, 156, 196, 241, 292, 350, 416 },
		{ 0, 1, 2, 3, 4, 5, 8, 17, 29, 46, 58, 90, 124, 161, 202, 248, 300, 359, 426 },
		{ 0, 1, 2, 3, 4, 6, 9, 18, 30, 48, 60, 93, 128, 166, 208, 255, 308, 368, 436 },
		{ 0, 1, 2, 3, 4, 6, 9, 18, 31, 49, 62, 96, 132, 171, 214, 262, 316, 377, 446 },
		{ 0, 1, 2, 3, 5, 6, 9, 19, 32, 51, 64, 99, 136, 176, 220, 269, 324, 386, 456 },
		{ 0, 1, 2, 3, 5, 6, 9, 19, 33, 52, 66, 102, 140, 181, 226, 276, 332, 395, 466 },
		{ 0, 1, 2, 4, 5, 6, 10, 20, 34, 54, 68, 105, 144, 186, 232, 283, 340, 404, 476 },
		{ 0, 1, 2, 4, 5, 7, 10, 21, 35, 56, 71, 109, 149, 192, 239, 291, 349, 414, 487 },
		{ 0, 1, 2, 4, 5, 7, 11, 22, 37, 59, 74, 113, 154, 198, 246, 299, 358, 424, 498 },
		{ 0, 1, 3, 4, 6, 7, 11, 23, 38, 61, 77, 117, 159, 204, 253, 307, 367, 434, 509 },
		{ 0, 1, 3, 4, 6, 8, 12, 24, 40, 64, 80, 121, 164, 210, 260, 315, 376, 444, 520 },
		{ 0, 1, 3, 4, 6, 8, 12, 24, 41, 66, 83, 125, 169, 216, 267, 323, 385, 454, 531 },
		{ 0, 1, 3, 5, 6, 8, 12, 25, 43, 68, 86, 129, 174, 222, 274, 331, 394, 464, 542 },
		{ 0, 1, 3, 5, 7, 8, 13, 26, 44, 71, 89, 133, 179, 228, 281, 339, 403, 474, 553 },
		{ 0, 1, 3, 5, 7, 9, 13, 27, 46, 73, 92, 137, 184, 234, 288, 347, 412, 484, 564 },
		{ 0, 1, 3, 5, 7, 9, 14, 28, 47, 76, 95, 141, 189, 240, 295, 355, 421, 494, 575 },
		{ 0, 1, 3, 5, 7, 9, 14, 29, 49, 78, 98, 145, 194, 246, 302, 363, 430, 504, 586 },
		{ 1, 2, 4, 6, 8, 10, 15, 30, 50, 80, 101, 149, 199, 252, 309, 371, 439, 514, 597 },
		{ 1, 2, 4, 6, 8, 10, 15, 31, 52, 83, 104, 153, 204, 258, 316, 379, 448, 524, 608 },
		{ 1, 2, 4, 6, 8, 10, 16, 32, 54, 86, 108, 158, 210, 265, 324, 388, 458, 535, 620 },
		{ 1, 2, 4, 6, 8, 11, 16, 33, 56, 89, 112, 163, 216, 272, 332, 397, 468, 546, 632 },
		{ 1, 2, 4, 6, 9, 11, 17, 34, 58, 92, 116, 168, 222, 279, 340, 406, 478, 557, 644 },
		{ 1, 2, 4, 7, 9, 12, 18, 36, 60, 96, 120, 173, 228, 286, 348, 415, 488, 568, 656 },
		{ 1, 2, 4, 7, 9, 12, 18, 37, 62, 99, 124, 178, 234, 293, 356, 424, 498, 579, 668 },
		{ 1, 2, 5, 7, 10, 12, 19, 38, 64, 102, 128, 183, 240, 300, 364, 433, 508, 590, 680 },
		{ 1, 2, 5, 7, 10, 13, 19, 39, 66, 105, 132, 188, 246, 307, 372, 442, 518, 601, 692 },
		{ 1, 2, 5, 8, 10, 13, 20, 40, 68, 108, 136, 193, 252, 314, 380, 451, 528, 612, 704 },
	};

	float		fEXP_RATE_TABLE[EXPTABLE_RANGE] = 
	{
		//-30 -29   -28   -27   -26   -25   -24   -23   -22   -21
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		//-20 -19   -18   -17   -16   -15   -14   -13   -12   -11   -10   -9    -8    -7    -6    -5    -4    -3    -2    -1    0
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		//+1  +2   +3     +4    +5    +6    +7    +8    +9    +10   +11   +12   +13   +14   +15   +16   +17   +18   +19   +20
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		//+21 +22   +23   +24   +25   +26   +27   +28   +29   +30
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	LONGLONG	lnEXP_MAX_TABLE[MAX_LEVEL];
	LONGLONG	lnEXP_MAX_TABLE_2nd[MAX_LEVEL];
	DWORD		dwEXP_PC_TABLE[MAX_LEVEL];

	float		fPARTYEXPRANGE		= 80.0f;			//	경험치 수신이 가능한 최대 거리.
	float		fPARTYEXP_S			= 0.006f;			//	파티 경험치 분배시 레벨차이에 따른 감소비율.
	//											  1    2    3    4    5    6    7    8   
	WORD		aPARTYEXP_ONE[MAXPARTY]		= {   100, 100, 100, 100, 100, 100, 100, 100 };
	WORD		aPARTYKILLEXP_ONE[MAXPARTY]	= {   100, 100, 100, 100, 100, 100, 100, 100 };

	DWORD		dwCLUB_PARTYNUM				= 5;		//	클럽 창설시 최소 파티(초기) 인원.
	DWORD		dwCLUB_DISSOLUTION_DAY		= 7;		//	클럽 해체 유보 일.
	DWORD		dwCLUB_JOINBLOCK_DAY		= 5;		//	클럽 탈퇴시 가입 제한 일.
	DWORD		dwCLUB_AUTHORITY			= 24;		//	클럽마스터 위임 제한 시간			
	DWORD		dwCLUB_ALLIANCE_SEC			= 24;		//	클럽 동맹 탈퇴, 제명시 재가입 재한시간.
	DWORD		dwCLUB_ALLIANCE_DIS			= 24;		//	클럽 동맹 해체시 결성 재한시간.
	DWORD		dwCLUB_ALLIANCE_NUM			= 3;		//	클럽 동맹 최대 숫자.

    DWORD		bCLUB_BATTLE				= FALSE;	//	클럽 배틀 가능 여부
	DWORD		dwCLUB_BATTLE_RANK			= 1;		//	클럽 랭크 확인
	DWORD		dwCLUB_BATTLE_MAX			= 5;		//  클럽 배틀 동시 진행 갯수
	DWORD		dwCLUB_BATTLE_TIME			= 20;		//  클럽 배틀 시간(분) - 입력 방식으로 변경함
	DWORD		dwCLUB_BATTLE_TIMEMIN		= 10;		//	클럽 배틀 최소시간(분)
	DWORD		dwCLUB_BATTLE_TIMEMAX		= 1200;		//	클럽 배틀 최대시간(분)
	DWORD		dwCLUB_BATTLE_GUID_TIME		= 24;		//  클럽 배틀 선도전 제약 시간(분)
	DWORD		dwCLUB_BATTLE_DIS_TIME		= 1;		//  클럽 배틀 휴전 및 항복 가능한 시간(분)
	BOOL		bCLUB_BATTLE_ALLIANCE		= FALSE;	//  동맹 클럽 동시 진행

	BOOL		bCLUB_DEATHMATCH			= FALSE;	//	클럽 데스매치 가능 여부
	DWORD		dwCLUB_DEATHMATCH_MEM		= 7;		//	클럽 데스매치 가능 멤버수

	DWORD		dwMAX_CLUBRANK				= 5;		//  최대 클럽 랭크

	GLCLUBRANK	sCLUBRANK[MAX_CLUBRANK]		=			//	클럽 랭크별 상수.
	{
		GLCLUBRANK(30,0,1000000,15),
		GLCLUBRANK(45,80,2000000,20),
		GLCLUBRANK(60,100,3500000,30),
		GLCLUBRANK(80,140,5000000,40),
		GLCLUBRANK(100,180,7000000,50),
		GLCLUBRANK(120,220,10000000,60),
		GLCLUBRANK(130,230,20000000,70),
		GLCLUBRANK(140,240,30000000,80),
		GLCLUBRANK(150,250,40000000,90),
		GLCLUBRANK(160,260,50000000,100),
	};

	float fMAX_COMMISSION				= 50;		//	최대 선도 지역 수수료 제한.
	float fDEFAULT_COMMISSION			= 5;		//	비 선도 지역의 수수료.
	float fEARNING_RATE					= 0.2f;		//	클럽 수수료 수익율.
	float fEARNING_RATE_NPC				= 0.2f;		//	NPC 호출 수수료 수익율 

	float fCDCERTIFY_DIST				= 30;		//	인증 가능 거리.
	float fCDCERTIFY_DIST2				= 5;		//	인증 가능 거리.
	float fCDCERTIFY_TIME				= 60;		//	인증 소요 시간.

	//	Note : 경험치 배율.
	float fEXP_SCALE = 1.0f;

	//	Note : 아이탬 드롭율.
	float fITEM_DROP_SCALE = 1.0f;

	//	Note : 돈 드롭율.
	float fMONEY_DROP_SCALE = 1.0f;

	//	Note : 희귀 아이탬 드롭율.
	float fITEM_DROP_LOWER_SCALE = 1.0f;

	//	Note : ----------------------------------------------------------------
	bool bSCHOOL_FREE_PK_ALWAYS = false;	//	학원간 프리 pk 항상 가능 여부.
	bool bSCHOOL_FREE_PK = false;			//	학원간 프리 pk on/off.
	bool bSCHOOL_FREE_PK_TIME_REAL = false;	//	학원간 프리 pk 설정 시간 ( on : 실제 시간, off : 게임시간 )
	bool bSCHOOL_FREE_PK_Z_FORCED = true;	//	학원간 프리 pk 강제 공격.
	DWORD dwSCHOOL_FREE_PK_TIME_START = 21;	//	학원간 프리 pk 시작 시간.
	DWORD dwSCHOOL_FREE_PK_TIME_TERM = 7;	//	학원간 프리 pk 지속 시간.
	float fSCHOOL_FREE_PK_ITEM_DROP = 3;	//	학원간 프리 pk 아이템 드롭율 가산값. ( 기본 드롭율 + 가산값. )
	//	Note : ----------------------------------------------------------------

	bool bENCHANT_TERMINATE_ITEM = false;	//	인첸트시 아이템 파손 가능.

	//	Note : ----------------------------------------------------------------기능 설정.
	bool bMONEY_DROP2FIELD = true;			//	돈 바닥에 버리기 가능 여부.

	//	Note : ----------------------------------------------------------------연마 확율.
	//													  1 -15
	float fGRADE_RATE[GRADE_LIMIT_MAX]				= {    100,    100,    100,    100, 33.33f, 
													    25.00f, 20.00f, 16.67f, 15.00f, 15.00f,
														15.00f, 15.00f, 15.00f, 15.00f, 15.00f };
	float fGRADE_RESET_RATE[GRADE_LIMIT_MAX]		= {      0,      0,      0,      0, 10.00f, 
														20.00f, 30.00f, 40.00f, 50.00f, 50.00f, 
														50.00f, 50.00f, 50.00f, 50.00f, 50.00f };
	float fGRADE_TERMINATE_RATE[GRADE_LIMIT_MAX]	= {      0,      0,      0,      0, 40.00f, 
														50.00f, 60.00f, 70.00f, 70.00f, 70.00f,
														70.00f,	70.00f,	70.00f,	70.00f,	70.00f };

	//	Note : ----------------------------------------------------------------사운드 설정.
	std::string	strGRINDING_SUCCEED			= "";
	std::string	strGRINDING_FAIL			= "";
	std::string	strGRINDING_RESET			= "";
	std::string	strGRINDING_BROKEN			= "";

	std::string	strGAMBLING_SHUFFLE			= "";		// Monster7j
	std::string	strGAMBLING_WIN				= "";
	std::string	strGAMBLING_LOSE			= "";

	std::string	strITEMDROP_SUIT			= "";
	std::string	strITEMDROP_WAPON			= "";
	std::string	strITEMDROP_SHOES			= "";
	std::string	strITEMDROP_RING			= "";
	std::string	strITEMDROP_QBOX			= "";

	std::string	strITEMDROP_SCROLL			= "";
	std::string	strITEMDROP_COIN			= "";
	std::string	strITEMDROP_DRUGS			= "";

	std::string	strPICKUP_ITEM				= "";
	std::string	strQITEM_FACT				= "";
	std::string	strQITEM_BGM[QUESTION_SIZE];

	std::string	strSELECT_CHAR				= "";

	std::string	strREBIRTH_EFFECT			= "";
	std::string	strLEVELUP_EFFECT			= "";
	std::string	strSRIKE_EFFECT				= "";
	std::string	strSRIKE_ELMT_EFFECT[EMELEMENT_MAXNUM]	= { "", "", "", "", "", "", "", "", "" };

	std::string	strAMBIENT_EFFECT			= "";
	std::string	strERASE_EFFECT				= "";

	std::string	strSKILL_LEARN_EFFECT		= "";
	std::string	strSKILL_UP_EFFECT			= "";

	std::string strQUEST_START				= "";
	std::string strQUEST_END_SUCCEED		= "";
	std::string strQUEST_END_FAIL			= "";

	std::string	strMOB_GEM_EFFECT			= "";
	std::string	strMOB_DIE_BLOOD			= "";

	// PET
	std::string	strPET_GEN_EFFECT           = "";
	
	// vehicle
	std::string strVEHICLE_GEN_EFFECT		= "";

	std::string	strCONFRONT_BOUND			= "";
	std::string	strHALFALPHA_EFFECT			= "";

	std::string	strBLOW_NUMB_EFFECT			= "";
	std::string	strBLOW_STUN_EFFECT			= "";
	std::string	strBLOW_STONE_EFFECT		= "";
	std::string	strBLOW_BURN_EFFECT			= "";
	std::string	strBLOW_FROZEN_EFFECT		= "";

	std::string	strBLOW_MAD_EFFECT			= "";
	std::string	strBLOW_POISON_EFFECT		= "";
	std::string	strBLOW_CURSE_EFFECT		= "";

	std::string	strBLOW_BODY_NUMB_EFFECT	= "";
	std::string	strBLOW_BODY_STUN_EFFECT	= "";
	std::string	strBLOW_BODY_STONE_EFFECT	= "";
	std::string	strBLOW_BODY_BURN_EFFECT	= "";
	std::string	strBLOW_BODY_FROZEN_EFFECT	= "";

	std::string	strBLOW_BODY_MAD_EFFECT		= "";
	std::string	strBLOW_BODY_POISON_EFFECT	= "";
	std::string	strBLOW_BODY_CURSE_EFFECT	= "";

	std::string	strBLOW_EFFECTS[EMBLOW_SIZE];
	std::string	strBLOW_BODY_EFFECTS[EMBLOW_SIZE];

	std::string strCRUSHING_BLOW_EFFECT;		// 강한타격 이펙트

	GLCONST_CHARCLASS	cCONSTCLASS[GLCI_NUM_NEWSEX] = 
	{
		//	FighterM
		GLCONST_CHARCLASS ( 10.0f, 0.9f, 1.00f, 0.08f,
							0.08f, 0.4f, 0.4f, 0.2f,
							SCHARSTATS(15,13,20,10,0,20), FCHARSTATS(2,3,1,1,1,3),
							35, 15, 5, 4,
							2, 2, 3, 1 ),
		//	ArmsM
		GLCONST_CHARCLASS ( 12.0f, 0.8f, 1.00f, 0.07f,
							0.07f, 0.5f, 0.5f, 0.1f,
							SCHARSTATS(20,15,10,8,0,15), FCHARSTATS(3,2,1,1,1,2),
							40, 25, 5, 3,
							1, 3, 2, 1 ),
		//	ArcherW
		GLCONST_CHARCLASS ( 9.0f, 1.0f, 1.00f, 0.09f,
							0.09f, 0.4f, 0.4f, 0.3f,
							SCHARSTATS(10,11,15,15,0,10), FCHARSTATS(2,1,1,1,1,1),
							25, 20, 3, 7,
							2, 2, 1, 3 ),
		//	SpiritW
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),
		//	ExtremeM
		GLCONST_CHARCLASS ( 1.7f, 0.84f, 1.1f, 0,
							0, 0.1f, 0.22f, 0.1f,
							SCHARSTATS(7,34,18,10,0,7), FCHARSTATS(0.3f,4.3f,0.66f,0.4f,0,2.7f),
							4, 5, 4, 3,
							0.9f, 0.1f, 0.4f, 0.3f ),
		//	ExtremeW
		GLCONST_CHARCLASS ( 1.7f, 0.8f, 1.1f, 0,
							0, 0.1f, 0.22f, 0.1f,
							SCHARSTATS(7,34,18,10,0,7), FCHARSTATS(0.3f,4.3f,0.66f,0.38f,0,2.7f),
							4, 5, 4, 3,
							1.0f, 0.1f, 0.4f, 0.2f ),
		//	FighterW
		GLCONST_CHARCLASS ( 10.0f, 0.9f, 1.00f, 0.08f,
							0.08f, 0.4f, 0.4f, 0.2f,
							SCHARSTATS(15,13,20,10,0,20), FCHARSTATS(2,3,1,1,1,3),
							35, 15, 5, 4,
							2, 2, 3, 1 ),
		//	ArmsW
		GLCONST_CHARCLASS ( 12.0f, 0.8f, 1.00f, 0.07f,
							0.07f, 0.5f, 0.5f, 0.1f,
							SCHARSTATS(20,15,10,8,0,15), FCHARSTATS(3,2,1,1,1,2),
							40, 25, 5, 3,
							1, 3, 2, 1 ),
		//	ArcherM
		GLCONST_CHARCLASS ( 9.0f, 1.0f, 1.00f, 0.09f,
							0.09f, 0.4f, 0.4f, 0.3f,
							SCHARSTATS(10,11,15,15,0,10), FCHARSTATS(2,1,1,1,1,1),
							25, 20, 3, 7,
							2, 2, 1, 3 ),
		//	SpiritM
		GLCONST_CHARCLASS ( 8.0f, 1.2f, 1.00f, 0.10f,
							0.10f, 0.3f, 0.3f, 0.3f,
							SCHARSTATS(10,10,30,12,0,15), FCHARSTATS(1,1,1,1,1,1),
							20, 30, 3, 5,
							1, 3, 1, 2 ),
	};

	std::vector<std::string>	vecGUIDANCE_FILE;
	std::vector<std::string>	vecClubDM_FILE;

	char		szCharSkin[GLCI_NUM_NEWSEX][MAX_PATH] =
	{
				"o_m.chf",		//	FighterM
				"o_m.chf",		//	ArmsM
				"o_w.chf",		//	ArcherW
				"o_w.chf",		//	SpiritW
				"o_m.chf",		//	Extreme M
				"o_w.chf",		//	Extreme W
				"o_w.chf",		//	FighterW
				"o_w.chf",		//	ArmsW
				"o_m.chf",		//	ArcherM
				"o_m.chf",		//	SpiritM
	};

	std::string strCLASS_INIT[MAX_SCHOOL][GLCI_NUM_NEWSEX] =
	{
		{ "class00.charset", "class01.charset", "class02.charset", "class03.charset", "class04.charset", "class05.charset", "class06.charset", "class07.charset", "class08.charset", "class09.charset" },
		{ "class10.charset", "class11.charset", "class12.charset", "class13.charset", "class14.charset", "class15.charset", "class16.charset", "class17.charset", "class18.charset", "class19.charset" },
		{ "class20.charset", "class21.charset", "class22.charset", "class23.charset", "class24.charset", "class25.charset", "class26.charset", "class27.charset", "class28.charset", "class29.charset" },
	};

	PCHARDATA2	pCharData2[MAX_SCHOOL][GLCI_NUM_NEWSEX] =
	{
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	};

	BOOL VAID_CHAR_DATA2 ( WORD wSchool, EMCHARINDEX emIndex )	{ return pCharData2[wSchool][emIndex] != NULL; }
	SCHARDATA2& GET_CHAR_DATA2 ( WORD wSchool, EMCHARINDEX emIndex )
	{
		GASSERT(wSchool<MAX_SCHOOL);
		GASSERT(emIndex<GLCI_NUM_NEWSEX);

		return *pCharData2[wSchool][emIndex];
	}

	void SETEXP_SCALE ( float fScale )
	{
		if ( fScale < 1.0f )	fScale = 1.0f;
		if ( fScale > 4.0f )	fScale = 4.0f;

		fEXP_SCALE = fScale;
	}

	float GETEXP_SCALE ()
	{
		return fEXP_SCALE;
	}

	const char* GETSCHOOLNAME ( WORD wSCHOOL )
	{
		if ( wSCHOOL >= MAX_SCHOOL )	return NULL;
		return strSCHOOLNAME[wSCHOOL].c_str();
	}

	DWORD GETEXP ( int nAttackerLev, int nDefenserLev )
	{
		//	Note : 경험치 테이블 참조식.  (GLCONST_CHAR::EXPBASEINDEX+1) 에서 +1은 정상적이면 있으면 안되지만
		//			초반 오류로 +1 값으로 정착되어 계속 유지되고 있는 상황임.
		int	nDiffLev = nDefenserLev - nAttackerLev;
		int nResultIndex = nDiffLev + GLCONST_CHAR::EXPBASEINDEX+1;
		
		if ( nResultIndex > (GLCONST_CHAR::EXPTABLE_LVL - 1) )		nResultIndex = GLCONST_CHAR::EXPTABLE_LVL - 1;
		else if ( nResultIndex < 0 )								nResultIndex = 0;
		GASSERT ( 0 <= nResultIndex && nResultIndex < GLCONST_CHAR::EXPTABLE_LVL );
		
		int nexp_lvl = nAttackerLev / EXP_LVL_STEP;
		if ( nexp_lvl < 0 )						nexp_lvl = 0;
		else if ( nexp_lvl >= EXPTABLE_SIZE )	nexp_lvl = EXPTABLE_SIZE-1;

		return (DWORD)(GLCONST_CHAR::wEXPTABLE[nexp_lvl][nResultIndex]*GETEXP_SCALE());
	}

	DWORD GETEXP_PC ( WORD wLev )
	{
		WORD wLEVEL = wLev-1;
		if ( wLEVEL >= MAX_LEVEL )		return 0;
		
		return dwEXP_PC_TABLE[wLEVEL];
	}

	float GETEXP_RATE ( int nAttackerLev, int nDefenserLev )
	{
		int	nDiffLev = nDefenserLev - nAttackerLev;
		int nResultIndex = nDiffLev + GLCONST_CHAR::EXPTABLE_RANGE_BASE;

		if ( nResultIndex >= GLCONST_CHAR::EXPTABLE_RANGE )		nResultIndex = GLCONST_CHAR::EXPTABLE_RANGE-1;
		else if ( nResultIndex < 0 )							nResultIndex = 0;

		return GLCONST_CHAR::fEXP_RATE_TABLE[nResultIndex];
	}
};
	
namespace GLCONST_PET
{
	PGLPET			pGLPET[PETTYPE_SIZE] = { NULL, NULL, NULL };
	char			szPetInit[PETTYPE_SIZE][MAX_PATH] = { "pet_01.charset" 
														 ,"pet_02.charset"
														 ,"pet_03.charset"
														 ,"pet_04.charset"};

	PETSTYLE		sPETSTYLE[PETTYPE_SIZE];
	BOOL			bCHANGEABLECOLOR[PETTYPE_SIZE] = { FALSE,FALSE,FALSE,FALSE };
	int				nFullDecrement[PETTYPE_SIZE]; // 포만감 감소치
	
	float			fRunArea	   = 60.0f; // 달려야만 하는 거리
	float			fWalkArea	   = 40.0f; // 걸어야만 하는 거리
	float			fOwnerDistance = 20.0f; // 주인과의 유지 거리
	int				nMAXVIEWRANGE  = 50;    // 주변 아이템을 탐색할 수 있는 거리
	float			fMaginotLineHP  = 20.0f; // 자동물약 스킬시 무조건 물약을 먹는 수치(%) - HP
	float			fMaginotLineMP  = 20.0f; // 자동물약 스킬시 무조건 물약을 먹는 수치(%) - MP
	float			fMaginotLineSP  = 20.0f; // 자동물약 스킬시 무조건 물약을 먹는 수치(%) - SP

	PGLPET GetPetData ( PETTYPE emTYPE )
	{
		for ( WORD i = 0; i < PETTYPE_SIZE; ++i )
		{
			if ( pGLPET[i]->m_emTYPE == emTYPE ) return pGLPET[i];
		}

		return NULL;
	}

	PETSTYLE GetPetStyle ( PETTYPE emTYPE )
	{
		for ( WORD i = 0; i < PETTYPE_SIZE; ++i )
		{
			if ( pGLPET[i]->m_emTYPE == emTYPE ) return sPETSTYLE[i];
		}

		return PETSTYLE();
	}
	BOOL IsChangeablePetColor ( PETTYPE emTYPE ) { return bCHANGEABLECOLOR[emTYPE]; }
};

namespace MINIGAME_ODDEVEN // 미니 게임, 홀짝 게임
{
	float	fReturnRate[MAX_ROUND]	= {1.5f,2.0f,3.0f,4.0f,5.0f};	// 배당금 배율 인수(최종배당금 = 원금*1회차인수*2회차인수...)
	float	fSuccessRate[MAX_ROUND]	= {0.5f,0.4f,0.2f,0.1f,0.05f};	// 성공 활률
	float	fTimeLimit				= 60.0f;						// 제한 시간(초)
	float	fShuffleTime			= 2.0f;							// 섞임 시간(초)
	UINT	uiMaxBattingMoney		= 1000000000;					// 최대 배팅 가능 금액
};

namespace GLCONST_SUMMON
{
	PGLSUMMON		pGLSUMMON[SUMMON_TYPE_SIZE] = { NULL };
	char			szSummonInit[SUMMON_TYPE_SIZE][MAX_PATH] = { "Summon_01.charset" };

	PGLSUMMON GetSummonData ( SUMMON_TYPE emTYPE )
	{	
		for ( WORD i = 0; i < SUMMON_TYPE_SIZE; ++i )
		{
			if ( pGLSUMMON[i]->m_emTYPE == emTYPE ) return pGLSUMMON[i];
		}

		return NULL;
	}

}

namespace GLCONST_VEHICLE
{
	PGLVEHICLE		pGLVEHICLE[VEHICLE_TYPE_SIZE] = { NULL };
	char			szVehicleInit[VEHICLE_TYPE_SIZE][MAX_PATH] = { "Vehicle_01.charset" };

	int				nFullDecrVehicle[VEHICLE_TYPE_SIZE]; // 포만감 감소치
	
	PGLVEHICLE GetVehicleData ( VEHICLE_TYPE emTYPE )
	{
		for ( WORD i = 0; i < VEHICLE_TYPE_SIZE; ++i )
		{
			if ( pGLVEHICLE[i]->m_emTYPE == emTYPE ) return pGLVEHICLE[i];
		}

		return NULL;
	}
};

namespace GLCONST_ATTENDANCE
{	
	ATTEND_REWARD	sATEEND_REWARD[MAX_REWARD];
	DWORD			dwMAXREWARD = 0;
	DWORD			dwAttendTime = 0;
};

namespace HAIRCOLOR
{
	WORD wHairColor[GLCI_NUM_NEWSEX][MAXHAIRCOLOR] = 
	{ 
		22923, 30719, 32486, 24447, 32258, 32594, 31743, 32767,	// 격투부남
		30719, 22923, 32486, 24447, 32258, 32594, 31743, 32767,	// 검도부남
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// 양궁부여
		30278, 18008, 31503, 24004, 32488, 27245, 28364, 0,		// 기예부여
		22923, 30719, 32486, 24447, 32258, 32594, 31743, 32767,	// 극강부남
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// 극강부여
		30278, 18008, 31503, 24004, 32488, 27245, 28364, 0,		// 격투부여
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// 검도부여
		30719, 22923, 32486, 24447, 32258, 32594, 31743, 32767,	// 양궁부남
		22923, 30719, 32486, 24447, 32258, 32594, 31743, 32767,	// 기예부남
	};

	WORD GetHairColor ( WORD wClass, WORD wStyle ) { return wHairColor[wClass][wStyle]; }
};


//----------------------------------------------------------------------------------------
GLogicData& GLogicData::GetInstance()
{
	static GLogicData Instance;
	return Instance;
}

GLogicData::GLogicData(void)
{
}

GLogicData::~GLogicData(void)
{
	ClearData ();
}

HRESULT GLogicData::LoadData ( BOOL bServer, bool bPastLoad /*= FALSE */ )
{
	CGLCommentFile glCommentFile;
	if( glCommentFile.LOADFILE("comment.ini") == FALSE ) return E_FAIL;

	// Note : 스트링테이블을 로드한다. by 경대
	//
	GLStringTable::GetInstance().CLEAR();
	GLStringTable::GetInstance().LOADFILE( GLItemMan::GetInstance()._STRINGTABLE, GLStringTable::ITEM );
	GLStringTable::GetInstance().LOADFILE( GLSkillMan::GetInstance()._STRINGTABLE, GLStringTable::SKILL );
	GLStringTable::GetInstance().LOADFILE( GLCrowDataMan::GetInstance()._STRINGTABLE, GLStringTable::CROW );

	//if( GLItemMan::GetInstance().LoadFile ( "item.isf", bServer, bPastLoad ) == E_FAIL ) return E_FAIL;
	//if( GLCrowDataMan::GetInstance().LoadFile ( "Crow.mnsf", bServer, bPastLoad ) == E_FAIL ) return E_FAIL;
	//if( GLSkillMan::GetInstance().LoadFile ( "skill.ssf", bPastLoad ) == E_FAIL ) return E_FAIL;
	if( GLItemMan::GetInstance().LoadFile ( "item.isf", bServer, TRUE ) == E_FAIL ) return E_FAIL;
	if( GLCrowDataMan::GetInstance().LoadFile ( "Crow.mnsf", bServer, TRUE ) == E_FAIL ) return E_FAIL;
	if( GLSkillMan::GetInstance().LoadFile ( "skill.ssf", TRUE ) == E_FAIL ) return E_FAIL;
	GLQuestMan::GetInstance().LoadFile ( "quest.lst", bServer==TRUE );

	GLCONST_CHAR::LOADFILE ( "default.charclass", bServer );
	GLCONST_ATTENDANCE::LoadFile ( "Attendance.ini", bServer );

	GLBusStation::GetInstance().LOAD( "busstation.ini" );
	GLTaxiStation::GetInstance().LOAD( "taxistation.ini" );
	GLItemMixMan::GetInstance().LoadFile( "ItemMix.ini", bServer );

	for ( WORD i=0; i<MAX_SCHOOL; i++ )
	{
		for ( WORD j=0; j<GLCI_NUM_NEWSEX; j++ )
		{
			PCHARDATA2 pNEW_CHARDATA = new SCHARDATA2;

       			pNEW_CHARDATA->LOADFILE ( GLCONST_CHAR::strCLASS_INIT[i][j].c_str() );

			SAFE_DELETE(GLCONST_CHAR::pCharData2[i][j]);
			GLCONST_CHAR::pCharData2[i][j] = pNEW_CHARDATA;
		}
	}

	if ( bServer )
	{
		GLCONST_CHAR::cCONSTCLASS[GLCI_FIGHTER_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_FIGHTER_M] );
		GLCONST_CHAR::cCONSTCLASS[GLCI_FIGHTER_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_FIGHTER_W] );
		GLCONST_CHAR::cCONSTCLASS[GLCI_ARMS_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARMS_M] );
		GLCONST_CHAR::cCONSTCLASS[GLCI_ARMS_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARMS_W] );
		GLCONST_CHAR::cCONSTCLASS[GLCI_ARCHER_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARCHER_M] );
		GLCONST_CHAR::cCONSTCLASS[GLCI_ARCHER_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_ARCHER_W] );
		GLCONST_CHAR::cCONSTCLASS[GLCI_SPIRIT_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_SPIRIT_M] );
		GLCONST_CHAR::cCONSTCLASS[GLCI_SPIRIT_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_SPIRIT_W] );
		GLCONST_CHAR::cCONSTCLASS[GLCI_EXTREME_M].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_EXTREME_M] );
		GLCONST_CHAR::cCONSTCLASS[GLCI_EXTREME_W].LoadAniSet ( GLCONST_CHAR::szCharSkin[GLCI_EXTREME_W] );
	}

	// PET Data 로딩
	for ( WORD i = PETTYPE_A; i < PETTYPE_SIZE; ++i )
	{
		PGLPET pPet = new GLPET;
		if( !pPet->LoadFile ( GLCONST_PET::szPetInit[i], (PETTYPE)i ) )
		{
			return E_FAIL;
		}
	}

	// 탈것 data 로딩 ( 안에 대입 있음 )
	for ( WORD i = VEHICLE_TYPE_BOARD; i < VEHICLE_TYPE_SIZE; ++i )
	{
		PGLVEHICLE pVehicle = new GLVEHICLE;
		if ( !pVehicle->LoadFile( GLCONST_VEHICLE::szVehicleInit[i], (VEHICLE_TYPE) i ))
		{
			return E_FAIL;
		}
	}

#ifdef _RELEASED // 소환수 내부에서만 되게 막음
	// 소환수 data 로딩 ( 안에 대입 있음 )
	for ( WORD i = SUMMON_TYPE_A; i < SUMMON_TYPE_SIZE; ++i )
	{
		PGLSUMMON pSummon = new GLSUMMON;
		if ( !pSummon->LoadFile( GLCONST_SUMMON::szSummonInit[i], (SUMMON_TYPE) i ))
		{
			return E_FAIL;
		}
	}
#endif

	return S_OK;
}

HRESULT GLogicData::ClearData ()
{
	GLCrowDataMan::GetInstance().FinalCleanup ();
	GLSkillMan::GetInstance().FinalCleanup ();

	for ( int i=0; i<GLCI_NUM_NEWSEX; i++ )	GLCONST_CHAR::cCONSTCLASS[i].ClearAniSet();
	
	for ( int i=0; i<MAX_SCHOOL; i++ )
	for ( int j=0; j<GLCI_NUM_NEWSEX; j++ )
	{
		SAFE_DELETE(GLCONST_CHAR::pCharData2[i][j]);
	}

	for ( int i = 0; i < PETTYPE_SIZE; ++i )
	{
		SAFE_DELETE( GLCONST_PET::pGLPET[i] );
	}

	for ( WORD i = VEHICLE_TYPE_BOARD; i < VEHICLE_TYPE_SIZE; ++i )
	{
		SAFE_DELETE( GLCONST_VEHICLE::pGLVEHICLE[i] );
	}

	for ( WORD i = SUMMON_TYPE_A; i < SUMMON_TYPE_SIZE; ++i )
	{
		SAFE_DELETE( GLCONST_SUMMON::pGLSUMMON[i] );
	}

	return S_OK;
}

HRESULT GLogicData::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr=S_OK;

	hr = GLItemMan::GetInstance().InitDeviceObjects ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT GLogicData::DeleteDeviceObjects ()
{
	HRESULT hr=S_OK;

	hr = GLItemMan::GetInstance().DeleteDeviceObjects ();
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

