#pragma once
#include <string>
#include "./GLCharData.h"
#include "./GLCharDefine.h"
#include "./GLInventory.h"
#include "./NpcDialogueSet.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"
#include "../[Lib]__Engine/Sources/DxMeshs/SAnimation.h"
#include "../[Lib]__Engine/Sources/NaviMesh/NavigationPath.h"
#define MOBNPCEDIT_VERSION 0x0110


//---------------------------------------------------------------------- OLD VER
struct SCROWATTACK_100
{
	enum { VERSION = 0x0100, };

	BOOL			bUsed;						//	사용가능.
	EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
	WORD			wRange;						//	공격가능 거리.
	WORD			wSphere;					//	공격영향 범위.

	//	Skill 의 영향 받는 목표.
	EMIMPACT_TAR	emImpact_Tar;				//	타겟. ( 자신, 대상, 위치 ) &&
	EMIMPACT_REALM	emImpact_Realm;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&
	EMIMPACT_SIDE	emImpact_Side;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )

	EMELEMENT		emElement;					//	공격 속성.
	GLPADATA		sDamage;					//	타격값.
	float			fDelay;						//	딜래이.
	float			fLife;						//	지속 시간.

	WORD			wUse_MP;					//	사용시 MP 소진량.
	WORD			wUse_SP;					//	사용시 SP 소진량.

	EMSTATE_BLOW	emBlow;						//	상태이상.
	float			fBlowRATE;					//	발생 확율.
	float			fBlowVALUE;					//	적용 값.

	EMIMPACT_ADDON	emImpact;					//	부가효과 종류.
	int				nImpactVAR;					//	변화 수치.

	char			szAniFile[ACF_SZNAME];		//	에니메이션 파일 이름.
	SANIATTACK		sAniAttack;					//	에니메이션 스텝.

	char			szSelfBodyEffect[ACF_SZNAME];	//	자기 자신의 몸에 붓는 이팩트.
	char			szTargBodyEffect[ACF_SZNAME];	//	목표 유닛의 몸에 붓는 이팩트.
	char			szTargetEffect[ACF_SZNAME];		//	목표 지향 이팩트.
};

//----------------------------------------------------------------------
struct SCROWATTACK_101
{
	BOOL			bUsed;						//	사용가능.
	EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
	WORD			wRange;						//	공격가능 거리.

	GLPADATA		sDamage;					//	타격값.
	float			fDelay;						//	딜래이.

	WORD			wUse_SP;					//	사용시 SP 소진량.

	char			szAniFile[ACF_SZNAME];		//	에니메이션 파일 이름.
	SANIATTACK		sAniAttack;					//	에니메이션 스텝.

	SNATIVEID		skill_id;					//	설정된 스킬 종류.

	char			szSelfBodyEffect[ACF_SZNAME];	//	자기 자신의 몸에 붓는 이팩트.
	char			szTargBodyEffect[ACF_SZNAME];	//	목표 유닛의 몸에 붓는 이팩트.
	char			szTargetEffect[ACF_SZNAME];		//	목표 지향 이팩트.
};

struct SCROWATTACK_102
{
	BOOL			bUsed;						//	사용가능.
	EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
	WORD			wRange;						//	공격가능 거리.

	GLPADATA		sDamage;					//	타격값.
	float			fDelay;						//	딜래이.

	WORD			wUse_SP;					//	사용시 SP 소진량.

	char			szAniFile[ACF_SZNAME];		//	에니메이션 파일 이름.
	SANIATTACK		sAniAttack;					//	에니메이션 스텝.

	SNATIVEID		skill_id;					//	설정된 스킬 종류.

	EMSTATE_BLOW	emBLOW_TYPE;				//	'상태이상' 적용 타입.
	float			fBLOW_RATE;					//	발생 확율.
	float			fBLOW_LIFE;					//	지속 시간.
	float			fBLOW_VAR1;					//	적용 값 1.
	float			fBLOW_VAR2;					//	적용 값 2.

	char			szSelfBodyEffect[ACF_SZNAME];	//	자기 자신의 몸에 붓는 이팩트.
	char			szTargBodyEffect[ACF_SZNAME];	//	목표 유닛의 몸에 붓는 이팩트.
	char			szTargetEffect[ACF_SZNAME];		//	목표 지향 이팩트.
};

enum EMCROWSKTAR
{
	CROWSKTAR_OUR	= 0,
	CROWSKTAR_ENEMY	= 1,
};

struct SCROWATTACK_103
{
	BOOL			bUsed;						//	사용가능.
	EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
	WORD			wRange;						//	공격가능 거리.

	GLPADATA		sDamage;					//	타격값.
	float			fDelay;						//	딜래이.

	WORD			wUse_SP;					//	사용시 SP 소진량.

	char			szAniFile[ACF_SZNAME];		//	에니메이션 파일 이름.
	SANIATTACK_100	sAniAttack;					//	에니메이션 스텝.

	SNATIVEID		skill_id;					//	스킬 종류.
	WORD			skill_lev;					//	스킬 래벨.
	EMCROWSKTAR		skill_tar;					//	스킬 타겟.

	EMSTATE_BLOW	emBLOW_TYPE;				//	'상태이상' 적용 타입.
	float			fBLOW_RATE;					//	발생 확율.
	float			fBLOW_LIFE;					//	지속 시간.
	float			fBLOW_VAR1;					//	적용 값 1.
	float			fBLOW_VAR2;					//	적용 값 2.

	char			szSelfBodyEffect[ACF_SZNAME];	//	자기 자신의 몸에 붓는 이팩트.
	char			szTargBodyEffect[ACF_SZNAME];	//	목표 유닛의 몸에 붓는 이팩트.
	char			szTargetEffect[ACF_SZNAME];		//	목표 지향 이팩트.
};

struct SCROWATTACK_104
{
	BOOL			bUsed;						//	사용가능.
	EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
	WORD			wRange;						//	공격가능 거리.

	GLPADATA		sDamage;					//	타격값.
	float			fDelay;						//	딜래이.

	WORD			wUse_SP;					//	사용시 SP 소진량.

	char			szAniFile[ACF_SZNAME];		//	에니메이션 파일 이름.
	SANIATTACK		sAniAttack;					//	에니메이션 스텝.

	SNATIVEID		skill_id;					//	스킬 종류.
	WORD			skill_lev;					//	스킬 래벨.
	EMCROWSKTAR		skill_tar;					//	스킬 타겟.

	EMSTATE_BLOW	emBLOW_TYPE;				//	'상태이상' 적용 타입.
	float			fBLOW_RATE;					//	발생 확율.
	float			fBLOW_LIFE;					//	지속 시간.
	float			fBLOW_VAR1;					//	적용 값 1.
	float			fBLOW_VAR2;					//	적용 값 2.

	char			szSelfBodyEffect[ACF_SZNAME];	//	자기 자신의 몸에 붓는 이팩트.
	char			szTargBodyEffect[ACF_SZNAME];	//	목표 유닛의 몸에 붓는 이팩트.
	char			szTargetEffect[ACF_SZNAME];		//	목표 지향 이팩트.
};

//----------------------------------------------------------------------
struct SCROWATTACK
{
	enum { VERSION = 0x0105, };

	BOOL			bUsed;						//	사용가능.
	EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
	WORD			wRange;						//	공격가능 거리.

	GLPADATA		sDamage;					//	타격값.
	float			fDelay;						//	딜래이.

	WORD			wUse_SP;					//	사용시 SP 소진량.

	std::string		strAniFile;					//	에니메이션 파일 이름.
	SANIATTACK		sAniAttack;					//	에니메이션 스텝.

	SNATIVEID		skill_id;					//	스킬 종류.
	WORD			skill_lev;					//	스킬 래벨.
	EMCROWSKTAR		skill_tar;					//	스킬 타겟.

	EMSTATE_BLOW	emBLOW_TYPE;				//	'상태이상' 적용 타입.
	float			fBLOW_RATE;					//	발생 확율.
	float			fBLOW_LIFE;					//	지속 시간.
	float			fBLOW_VAR1;					//	적용 값 1.
	float			fBLOW_VAR2;					//	적용 값 2.

	std::string		strSelfBodyEffect;			//	자기 자신의 몸에 붓는 이팩트.
	std::string		strTargBodyEffect;			//	목표 유닛의 몸에 붓는 이팩트.
	std::string		strTargetEffect;			//	목표 지향 이팩트.

	SCROWATTACK () :
		bUsed(FALSE),
		emAttRgType(EMATT_SHORT),
		wRange(4),

		sDamage(4,6),
		fDelay(0.2f),

		wUse_SP(0),
		skill_id(NATIVEID_NULL()),
		skill_lev(0),
		skill_tar(CROWSKTAR_ENEMY),

		emBLOW_TYPE(EMBLOW_NONE),
		fBLOW_RATE(0.0f),
		fBLOW_LIFE(0.0f),
		fBLOW_VAR1(0.0f),
		fBLOW_VAR2(0.0f)
	{
	}

	SCROWATTACK (const SCROWATTACK& value )
	{
		operator= ( value );
	}

	SCROWATTACK& operator= ( const SCROWATTACK_100 &OldData );
	SCROWATTACK& operator= ( const SCROWATTACK_101 &OldData );
	SCROWATTACK& operator= ( const SCROWATTACK_102 &OldData );
	SCROWATTACK& operator= ( const SCROWATTACK_103 &OldData );
	SCROWATTACK& operator= ( const SCROWATTACK_104 &OldData );

	SCROWATTACK& operator= ( const SCROWATTACK &value );

	bool LOAD ( basestream &SFile );
	bool SAVE ( CSerialFile &SFile );

	static VOID SaveCsvHead ( std::fstream &SFile, int iIndex );
	VOID SaveCsv ( std::fstream &SFile );
	VOID LoadCsv ( CStringArray &StrArray );
};

struct SCROWBASIC_100
{
	SNATIVEID			sNativeID;					//	고유 ID.
	char				m_szName[CHAR_SZNAME];		//	이름.
	EMTRIBE				m_emTribe;					//	종족.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	WORD				m_wLevel;					//	레벨.

	WORD				m_wHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wDefense;					//	방어력.

	SRESIST_101			m_sResist;					//	저항값.
};

struct SCROWBASIC_101
{
	SNATIVEID			sNativeID;					//	고유 ID.
	char				m_szName[CHAR_SZNAME];		//	이름.
	EMTRIBE				m_emTribe;					//	종족.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	WORD				m_wLevel;					//	레벨.

	WORD				m_wHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wDefense;					//	방어력.

	SRESIST				m_sResist;					//	저항값.
};

struct SCROWBASIC_102
{
	SNATIVEID			sNativeID;					//	고유 ID.
	char				m_szName[CHAR_SZNAME];		//	이름.
	EMTRIBE				m_emTribe;					//	종족.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	WORD				m_wLevel;					//	레벨.

	WORD				m_wHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wDefense;					//	방어력.

	SRESIST				m_sResist;					//	저항값.

	WORD				m_wBonusExp;				//	추가 경험치.
};

struct SCROWBASIC_103
{
	SNATIVEID			sNativeID;					//	고유 ID.

	char				m_szName[CHAR_SZNAME];		//	이름.

	EMTRIBE				m_emTribe;					//	종족.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	WORD				m_wLevel;					//	레벨.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wDefense;					//	방어력.

	SRESIST				m_sResist;					//	저항값.

	WORD				m_wBonusExp;				//	추가 경험치.
};

struct SCROWBASIC_104
{
	SNATIVEID			sNativeID;					//	고유 ID.

	char				m_szName[CHAR_SZNAME];		//	이름.

	EMTRIBE				m_emTribe;					//	종족.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	// 팻타입 추가
	PETTYPE				m_emPetType;				// 팻타입

	WORD				m_wLevel;					//	레벨.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wDefense;					//	방어력.

	SRESIST				m_sResist;					//	저항값.

	WORD				m_wBonusExp;				//	추가 경험치.
};

struct SCROWBASIC_105
{
	SNATIVEID			sNativeID;					//	고유 ID.

	char				m_szName[CHAR_SZNAME];		//	이름.

	EMTRIBE				m_emTribe;					//	종족.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	WORD				m_wLevel;					//	레벨.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wDefense;					//	방어력.

	SRESIST				m_sResist;					//	저항값.

	WORD				m_wBonusExp;				//	추가 경험치.

	// 팻타입 추가
	PETTYPE				m_emPetType;				// 팻타입
};

struct SCROWBASIC_106
{
	SNATIVEID			sNativeID;					//	고유 ID.

	char				m_szName[CHAR_SZNAME];		//	이름.

	EMTRIBE				m_emTribe;					//	종족.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	WORD				m_wLevel;					//	레벨.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wDefense;					//	방어력.

	SRESIST				m_sResist;					//	저항값.

	WORD				m_wBonusExp;				//	추가 경험치.

	bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

	// 팻타입 추가
	PETTYPE				m_emPetType;				// 팻타입
};

struct SCROWBASIC_108
{
	SNATIVEID			sNativeID;					//	고유 ID.

	char				m_szName[CHAR_SZNAME];		//	이름.

	EMTRIBE				m_emTribe;					//	종족.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	WORD				m_wLevel;					//	레벨.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wDefense;					//	방어력.

	SRESIST				m_sResist;					//	저항값.

	DWORD				m_wBonusExp;				//	추가 경험치.

	bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

	// 팻타입 추가
	PETTYPE				m_emPetType;				// 팻타입
};

struct SCROWBASIC_109
{
	SNATIVEID			sNativeID;					//	고유 ID.

	char				m_szName[CHAR_SZNAME];		//	이름.

	EMTRIBE				m_emTribe;					//	종족.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	WORD				m_wLevel;					//	레벨.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wDefense;					//	방어력.

	SRESIST				m_sResist;					//	저항값.

	DWORD				m_wBonusExp;				//	추가 경험치.

	bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

	// 팻타입 추가
	PETTYPE				m_emPetType;				// 팻타입
	
	DWORD				m_dwGenTime;				// 호출 시간
};

struct SCROWBASIC
{
	enum { VERSION = MOBNPCEDIT_VERSION, };

	SNATIVEID			sNativeID;					//	고유 ID.

	char				m_szName[CHAR_SZNAME];		//	이름.

	EMTRIBE				m_emTribe;					//	종족.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	WORD				m_wLevel;					//	레벨.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wDefense;					//	방어력.

	SRESIST				m_sResist;					//	저항값.

	DWORD				m_wBonusExp;				//	추가 경험치.

	bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

	// 팻타입 추가
	PETTYPE				m_emPetType;				// 팻타입

	DWORD				m_dwGenTime;				// 호출 시간
	WORD				m_wGatherTimeLow;			// 채집시간(Low)
	WORD				m_wGatherTimeHigh;			// 채집시간(High)
	float				m_fGatherRate;				// 채집확률
	WORD				m_wGatherAnimation;			// 애니메이션

	SCROWBASIC (void) :
		m_emTribe(TRIBE_HUMAN),
		m_emCrow(CROW_MOB),
		m_emBright(BRIGHT_LIGHT),

		m_wLevel(1),

		m_dwHP(70),
		m_wMP(40),
		m_wSP(20),

		m_fIncHP(0.3f),
		m_fIncMP(0.3f),
		m_fIncSP(0.5f),
	
		m_wAvoidRate(2),
		m_wHitRate(4),

		m_wViewRange(80),
		m_wDefense(0),

		m_wBonusExp(0),

		m_bOverlapAttack(FALSE),

		m_emPetType(PETTYPE_A),
		m_dwGenTime(0),
		m_wGatherTimeLow(0),
		m_wGatherTimeHigh(0),
		m_fGatherRate(0.0f),
		m_wGatherAnimation(0)
	{
		SecureZeroMemory ( m_szName, sizeof(m_szName) );
	}

	SCROWBASIC& operator = ( const SCROWBASIC_100 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_101 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_102 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_103 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_104 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_105 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_106 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_108 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_109 &sCrowBasic );

	static VOID SaveCsvHead ( std::fstream &SFile );
	VOID SaveCsv ( std::fstream &SFile );
	VOID LoadCsv ( CStringArray &StrArray );
};

enum EMCROW_NPCACT
{
	EMCROWACT_TARSHORT	= 0x00004,
	EMCROWACT_CDCERTIFY	= 0x00008,

	EMCROWACT_INVISIBLE	= 0x00010,
	EMCROWACT_RECVISIBLE= 0x00020,
	EMCROWACT_BARRIER	= 0x00040,
	EMCROWACT_POSHOLD	= 0x00080,
	EMCROWACT_DIRHOLD	= 0x00100,

	EMCROWACT_KNOCK		= 0x00200,
	EMCROWACT_BOSS		= 0x00400,
	EMCROWACT_BUSUNIT	= 0x01000,

	EMCROWACT_IGNORE_SHOCK	= 0x10000,

	EMCROWACT_AUTODROP	= 0x20000,
};

enum EMCROWACT_UP
{
	EMCROWACT_UP_IDLE			= 0,
	EMCROWACT_UP_FIRSTSTRIKE	= 1,
	EMCROWACT_UP_LOWLEVEL		= 2,
	EMCROWACT_UP_LOWHP			= 3,
	EMCROWACT_UP_BRIGHT			= 4,
	EMCROWACT_UP_DARK			= 5,
	EMCROWACT_UP_BLOW			= 6,
	EMCROWACT_UP_ARMER			= 7,
	EMCROWACT_UP_RUNNER			= 8,
	EMCROWACT_UP_ESCAPE			= 9,

	EMCROWACT_UP_NSIZE			= 10
};

enum EMCROWACT_DN
{
	EMCROWACT_DN_CONTINUE		= 0,
	EMCROWACT_DN_LOWHP			= 1,
	EMCROWACT_DN_ESCAPE			= 2,

	EMCROWACT_DN_NSIZE			= 3,
};

struct SCROWPATTERN
{
	enum { RANDOMPATTERNNUM = 3 };
	float				m_fPatternDNRate;
	EMCROWACT_UP		m_emActPattern;
	DWORD				m_dwPatternAttackSet;        

	SCROWPATTERN (void) :
		m_fPatternDNRate( 0.0f ),
		m_emActPattern( EMCROWACT_UP_IDLE ),
		m_dwPatternAttackSet( 0 )
		{

		}
};

struct SCROWACTION_100
{
	WORD				m_wBodyRadius;					//	몸체원통 반경.
	char				m_szSkinObj[ACF_SZNAME];		//	스킨 파일.
	char				m_szTalkFile[ACF_SZNAME];		//	대화 파일.

	BOOL				m_bTrade;						//	거래가능한지.
	char				m_szSaleFile[ACF_SZNAME];		//	Item 판매 목록.

	BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
	char				m_szFallingEffect[ACF_SZNAME];	//	죽는 순간에 나오는 효과.
	char				m_szBlowEffect[ACF_SZNAME];		//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

	DWORD				m_dwActionUP;					//	행동 성향 HP 50%이상.
	DWORD				m_dwActionDN;					//	행동 성향 HP 50%이하.

	EMMOVETYPE			m_emMoveType;					//	이동 타입.
	float				m_fDriftHeight;					//	부양 높이.

	float				m_fWalkVelo;					//	이동 속도.
	BOOL				m_bRun;							//	뛰기 가능.
	float				m_fRunVelo;						//	뛰기 속도.
};

struct SCROWACTION_101
{
	WORD				m_wBodyRadius;					//	몸체원통 반경.
	char				m_szSkinObj[ACF_SZNAME];		//	스킨 파일.
	char				m_szTalkFile[ACF_SZNAME];		//	대화 파일.

	DWORD				m_dwActFlag;					//	각종 행동.
	char				m_szSaleFile[ACF_SZNAME];		//	Item 판매 목록.

	BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
	char				m_szBirthEffect[ACF_SZNAME];	//	탄생시 나오는 효과.
	char				m_szFallingEffect[ACF_SZNAME];	//	죽는 순간에 나오는 효과.
	char				m_szBlowEffect[ACF_SZNAME];		//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

	EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
	EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

	EMMOVETYPE			m_emMoveType;					//	이동 타입.
	float				m_fDriftHeight;					//	부양 높이.

	float				m_fWalkVelo;					//	이동 속도.
	BOOL				m_bRun;							//	뛰기 가능.
	float				m_fRunVelo;						//	뛰기 속도.
};

struct SCROWACTION_102
{
	enum { SALENUM = 3 };
	WORD				m_wBodyRadius;					//	몸체원통 반경.
	char				m_szSkinObj[ACF_SZNAME];		//	스킨 파일.
	char				m_szTalkFile[ACF_SZNAME];		//	대화 파일.

	DWORD				m_dwActFlag;					//	각종 행동.
	char				m_szSaleFile[SALENUM][ACF_SZNAME];		//	Item 판매 목록.

	BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
	char				m_szBirthEffect[ACF_SZNAME];	//	탄생시 나오는 효과.
	char				m_szFallingEffect[ACF_SZNAME];	//	죽는 순간에 나오는 효과.
	char				m_szBlowEffect[ACF_SZNAME];		//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

	EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
	EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

	EMMOVETYPE			m_emMoveType;					//	이동 타입.
	float				m_fDriftHeight;					//	부양 높이.

	float				m_fWalkVelo;					//	이동 속도.
	BOOL				m_bRun;							//	뛰기 가능.
	float				m_fRunVelo;						//	뛰기 속도.
};

struct SCROWACTION_103
{
	enum { VERSION = 0x0103, SALENUM = 3 };

	WORD				m_wBodyRadius;					//	몸체원통 반경.
	std::string			m_strSkinObj;					//	스킨 파일.
	std::string			m_strTalkFile;					//	대화 파일.

	DWORD				m_dwActFlag;					//	각종 행동.
	std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

	BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
	std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
	std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
	std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

	EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
	EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

	EMMOVETYPE			m_emMoveType;					//	이동 타입.
	float				m_fDriftHeight;					//	부양 높이.

	float				m_fWalkVelo;					//	이동 속도.
	BOOL				m_bRun;							//	뛰기 가능.
	float				m_fRunVelo;						//	뛰기 속도.
};

struct SCROWACTION_104
{
	enum { VERSION = 0x0104, SALENUM = 3 };

	WORD				m_wBodyRadius;					//	몸체원통 반경.
	std::string			m_strSkinObj;					//	스킨 파일.
	std::string			m_strTalkFile;					//	대화 파일.

	DWORD				m_dwActFlag;					//	각종 행동.
	std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

	BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
	std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
	std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
	std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

	EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
	EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

	EMMOVETYPE			m_emMoveType;					//	이동 타입.
	float				m_fDriftHeight;					//	부양 높이.

	float				m_fWalkVelo;					//	이동 속도.
	BOOL				m_bRun;							//	뛰기 가능.
	float				m_fRunVelo;						//	뛰기 속도.

	float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)
};

struct SCROWACTION_105
{
	enum { VERSION = 0x0105, SALENUM = 3 };

	WORD				m_wBodyRadius;					//	몸체원통 반경.
	std::string			m_strSkinObj;					//	스킨 파일.
	std::string			m_strTalkFile;					//	대화 파일.

	DWORD				m_dwActFlag;					//	각종 행동.
	std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

	BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
	std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
	std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
	std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

	EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
	EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

	EMMOVETYPE			m_emMoveType;					//	이동 타입.
	float				m_fDriftHeight;					//	부양 높이.

	float				m_fWalkVelo;					//	이동 속도.
	BOOL				m_bRun;							//	뛰기 가능.
	float				m_fRunVelo;						//	뛰기 속도.

	float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)
	float				m_fActionDNRate;				// 행동 성향 이하 %
};


struct SCROWACTION_106
{
	enum { VERSION = 0x0106, SALENUM = 3 };

	WORD				m_wBodyRadius;					//	몸체원통 반경.
	std::string			m_strSkinObj;					//	스킨 파일.
	std::string			m_strTalkFile;					//	대화 파일.

	DWORD				m_dwActFlag;					//	각종 행동.
	std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

	BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
	std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
	std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
	std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

	EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
	EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

	EMMOVETYPE			m_emMoveType;					//	이동 타입.
	float				m_fDriftHeight;					//	부양 높이.

	float				m_fWalkVelo;					//	이동 속도.
	BOOL				m_bRun;							//	뛰기 가능.
	float				m_fRunVelo;						//	뛰기 속도.

	float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)
	float				m_fActionDNRate;				// 행동 성향 이하 %

	BOOL				m_bMobLink;						// 해당몬스터가 죽으면 연결되어있는 몬스터를 호출할지 여부.
	SNATIVEID			m_sMobLinkID;					// 연결된 몬스터의 ID
};

struct SCROWACTION_107
{
	enum { VERSION = 0x0107, SALENUM = 3, PATTERNNUM = 10, ATTACKSETNUM = 4 };

	WORD				m_wBodyRadius;					//	몸체원통 반경.
	std::string			m_strSkinObj;					//	스킨 파일.
	std::string			m_strTalkFile;					//	대화 파일.

	DWORD				m_dwActFlag;					//	각종 행동.
	std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

	BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
	std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
	std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
	std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

	EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
	EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

	EMMOVETYPE			m_emMoveType;					//	이동 타입.
	float				m_fDriftHeight;					//	부양 높이.

	float				m_fWalkVelo;					//	이동 속도.
	BOOL				m_bRun;							//	뛰기 가능.
	float				m_fRunVelo;						//	뛰기 속도.

	float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)
	float				m_fActionDNRate;				// 행동 성향 이하 %

	BOOL				m_bMobLink;						// 해당몬스터가 죽으면 연결되어있는 몬스터를 호출할지 여부.
	SNATIVEID			m_sMobLinkID;					// 연결된 몬스터의 ID

	SCROWPATTERN		m_Pattern;						
	std::vector< SCROWPATTERN >	m_vecPatternList;
};

struct SCROWACTION
{
	enum { VERSION = 0x0108, SALENUM = 3, PATTERNNUM = 10, ATTACKSETNUM = 4 };

	WORD				m_wBodyRadius;					//	몸체원통 반경.
	std::string			m_strSkinObj;					//	스킨 파일.
	std::string			m_strTalkFile;					//	대화 파일.

	DWORD				m_dwActFlag;					//	각종 행동.
	std::string			m_strSaleFile[SALENUM];			//	Item 판매 목록.

	BOOL				m_bAfterFall_NoBody;			//	죽는 순간에 몸자체 사라짐. ( 효과로 대채시에 유용 )
	std::string			m_strBirthEffect;				//	탄생시 나오는 효과.
	std::string			m_strFallingEffect;				//	죽는 순간에 나오는 효과.
	std::string			m_strBlowEffect;					//	타격시 이팩트, CROW 종류별로 지정할 수 있게.

	EMCROWACT_UP		m_emActionUP;					//	행동 성향 HP 50%이상.
	EMCROWACT_DN		m_emActionDN;					//	행동 성향 HP 50%이하.

	EMMOVETYPE			m_emMoveType;					//	이동 타입.
	float				m_fDriftHeight;					//	부양 높이.

	float				m_fWalkVelo;					//	이동 속도.
	BOOL				m_bRun;							//	뛰기 가능.
	float				m_fRunVelo;						//	뛰기 속도.

	float				m_fLiveTime;					//  활동가능한 시간(EMCROWACT_AUTODROP 플래그일 경우)
	float				m_fActionDNRate;				// 행동 성향 이하 %

	BOOL				m_bMobLink;						// 해당몬스터가 죽으면 연결되어있는 몬스터를 호출할지 여부.
	SNATIVEID			m_sMobLinkID;					// 연결된 몬스터의 ID
	float				m_fMobLinkScale;				// 연결된 몬스터 몸체 크기
	float				m_fMobLinkDelay;				// 연결된 몬스터 리젠 딜레이

    SCROWPATTERN		m_Pattern;						
	std::vector< SCROWPATTERN >	m_vecPatternList;


	SCROWACTION ( void ) :
		m_dwActFlag( NULL ),

		m_bAfterFall_NoBody( FALSE ),

		m_emActionUP( EMCROWACT_UP_IDLE ),
		m_emActionDN( EMCROWACT_DN_CONTINUE ),

		m_bRun( FALSE ),
		m_emMoveType( MOVE_LAND ),
		m_fDriftHeight( 0.0f ),

		m_wBodyRadius( 4 ),
		m_fWalkVelo( 12.0f ),
		m_fRunVelo( 34.0f ),
		m_fLiveTime( 0.0f ),
		
		m_fActionDNRate( 50.0f ),

		m_bMobLink( FALSE ),
		m_sMobLinkID( NATIVEID_NULL() ),
		m_fMobLinkScale( 0.0f ),
		m_fMobLinkDelay( 0.0f )
	{
	}

	SCROWACTION ( const SCROWACTION &value )
	{
		operator = ( value );
	}

	SCROWACTION& operator= ( const SCROWACTION_100 &OldData );
	SCROWACTION& operator= ( const SCROWACTION_101 &OldData );
	SCROWACTION& operator= ( const SCROWACTION_102 &OldData );
	SCROWACTION& operator= ( const SCROWACTION_103 &OldData );
	SCROWACTION& operator= ( const SCROWACTION_104 &OldData );
	SCROWACTION& operator= ( const SCROWACTION_105 &OldData );
	SCROWACTION& operator= ( const SCROWACTION_106 &OldData );
	SCROWACTION& operator= ( const SCROWACTION_107 &OldData );

	SCROWACTION& operator= ( const SCROWACTION &value );

	bool LOAD103 ( basestream &SFile );
	bool LOAD104 ( basestream &SFile );
	bool LOAD105 ( basestream &SFile );
	bool LOAD106 ( basestream &SFile );
	bool LOAD107 ( basestream &SFile );

	bool LOAD ( basestream &SFile );
	bool SAVE ( CSerialFile &SFile );

	static VOID SaveCsvHead ( std::fstream &SFile );
	VOID SaveCsv ( std::fstream &SFile );
	VOID LoadCsv ( CStringArray &StrArray );
};

struct SCROWGEN_100
{
	DWORD				m_dwGenMoney;				//	발생 금액.
	SNATIVEID			m_sGenItemID;				//	발생 아이템.
	WORD				m_wGenMoney_Rate;			//	발생 금액 확율.
	WORD				m_wGenItem_Rate;			//	발생 아이템 확율.
};

struct SCROWGEN_101
{
	DWORD				m_dwGenMoney;				//	발생 금액.
	SNATIVEID			m_sGenItemID;				//	발생 아이템.
	WORD				m_wGenMoney_Rate;			//	발생 금액 확율.
	WORD				m_wGenItem_Rate;			//	발생 아이템 확율.

	char				m_szGenItem[ACF_SZNAME];	//	발생 아이템 설정 파일.
};

struct SCROWGEN
{
	enum { VERSION = 0x0102, };

	//	발생아이템.
	DWORD				m_dwGenMoney;				//	발생 금액.
	SNATIVEID			m_sGenItemID;				//	발생 아이템.
	WORD				m_wGenMoney_Rate;			//	발생 금액 확율.
	WORD				m_wGenItem_Rate;			//	발생 아이템 확율.

	std::string			m_strGenItem;				//	발생 아이템 설정 파일.
	std::string			m_strQtGenItem;				//	Question Item 설정 파일.

	SCROWGEN (void) :
		m_dwGenMoney(0),
		m_sGenItemID(NATIVEID_NULL()),
		m_wGenMoney_Rate(0),
		m_wGenItem_Rate(0)
	{
	}

	SCROWGEN ( const SCROWGEN &value )
	{
		operator = ( value );
	}

	void Assign ( SCROWGEN_100 &OldCrowGen )
	{
		m_dwGenMoney = OldCrowGen.m_dwGenMoney;
		m_wGenMoney_Rate = OldCrowGen.m_wGenMoney_Rate;
		
		m_sGenItemID = OldCrowGen.m_sGenItemID;
		m_wGenItem_Rate = OldCrowGen.m_wGenItem_Rate;
	}

	void Assign ( SCROWGEN_101 &OldCrowGen )
	{
		m_dwGenMoney = OldCrowGen.m_dwGenMoney;
		m_wGenMoney_Rate = OldCrowGen.m_wGenMoney_Rate;

		m_sGenItemID = OldCrowGen.m_sGenItemID;
		m_wGenItem_Rate = OldCrowGen.m_wGenItem_Rate;
	
		m_strGenItem = OldCrowGen.m_szGenItem;
	}

	SCROWGEN& operator = ( const SCROWGEN &value )
	{
		m_dwGenMoney = value.m_dwGenMoney;
		m_wGenMoney_Rate = value.m_wGenMoney_Rate;

		m_sGenItemID = value.m_sGenItemID;
		m_wGenItem_Rate = value.m_wGenItem_Rate;
	
		m_strGenItem = value.m_strGenItem;
		m_strQtGenItem = value.m_strQtGenItem;
	
		return *this;
	}

	bool LOAD ( basestream &SFile );
	bool SAVE ( CSerialFile &SFile );

	static VOID SaveCsvHead ( std::fstream &SFile );
	VOID SaveCsv ( std::fstream &SFile );
	VOID LoadCsv ( CStringArray &StrArray );
};

//	Note : 다수 아이템 발생시 처리 구조.
//
struct SGENITEM
{
	enum
	{
		EMGNUM		= 5,
		EMGSPEC		= 30,
		EMSPEC_NULL	= -1
	};

	struct SGNUM	//	발생 갯수별 확율.
	{
		WORD	wNum;
		float	fRateL;
		float	fRateH;

		SGNUM () :
			wNum(0),
			fRateL(100.0f),
			fRateH(100.0f)
		{
		}
	};

	struct SGITEMSPEC	//	발생 아이템군별 확율.
	{
		DWORD	dwSPECID;
		float	fRateL;
		float	fRateH;
		bool	bSPECID;

		SGITEMSPEC () :
			dwSPECID(EMSPEC_NULL),
			fRateL(100.0f),
			fRateH(100.0f),
			bSPECID( false )
		{
		}
	};

	float		m_fGenRate;				//	발생 확율.
	SGNUM		m_sGenNum[EMGNUM];		//	발생 갯수별 확율 분포.
	SGITEMSPEC	m_sItemSpec[EMGSPEC];	//	발생 아이템 군.

	SGENITEM() :
		m_fGenRate(0)
	{
	}

	BOOL LOADFILE ( const char* szFileName );

	BOOL ISGEN ( float fGenRate = 1.0f );							//	발생 확율.
	WORD SELECTNUM ();						//	발생 갯수.
	DWORD SELECTSPECID ( float fGEN_RATE, bool& bSPECID );	//	발생 군.
};

//	Note : Question 아이템 발생시 처리 구조 by 경대
//
struct SQTGENITEM
{
	enum { EMQTMAX = 20 };

	struct ITEM
	{
		SNATIVEID	m_sGenItemID;
		float		m_fGenRate;

		ITEM() : m_sGenItemID(false), m_fGenRate(0)
		{
		}
	};

	ITEM	m_sItem[EMQTMAX];
	int		m_nItemNum;

	SQTGENITEM() : m_nItemNum(0)
	{
	}

	BOOL LOADFILE ( const char * szFileName );

	SNATIVEID SELECTITEM(); // 아이템 발생
};

typedef std::map<DWORD,LONGLONG>	NPCSELL_PRICE_MAP;
typedef NPCSELL_PRICE_MAP::iterator NPCSELL_PRICE_MAP_ITER;

struct SCROWDATA
{
	enum
	{
		VERSION = 0x0101,
		EMMAXATTACK = 3,

		FILE_SBASIC			= 1,
		FILE_SACTION		= 2,
		FILE_SGEN			= 3,
		FILE_SATTACK		= 4,
		FILE_END_DATA		= 0xEDEDEDED,
	};

	union
	{
		struct
		{
		SCROWBASIC			m_sBasic;
		};
		struct
		{
		SNATIVEID			sNativeID;					//	고유 ID.
		char				m_szName[CHAR_SZNAME];		//	이름.
		EMTRIBE				m_emTribe;					//	종족.
		EMCROW				m_emCrow;					//	Npc, Mob.
		EMBRIGHT			m_emBright;					//	속성 (광/암)

		WORD				m_wLevel;					//	레벨.

		DWORD				m_dwHP;						//	HP
		WORD				m_wMP;						//	MP
		WORD				m_wSP;						//	SP

		//	회복율.
		float				m_fIncHP;					//	HP 회복율.
		float				m_fIncMP;					//	MP 회복율.
		float				m_fIncSP;					//	SP 회복율.

		WORD				m_wAvoidRate;				//	회피율.
		WORD				m_wHitRate;					//	명중율.

		WORD				m_wViewRange;				//	시야 거리.
		WORD				m_wDefense;					//	방어력.

		SRESIST				m_sResist;					//	저항값.

		DWORD				m_wBonusExp;				//	추가경험치.

		bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

		// PET
		PETTYPE				m_emPetType;
		DWORD				m_dwGenTime;

		WORD				m_wGatherTimeLow;			// 채집시간(Low)
		WORD				m_wGatherTimeHigh;			// 채집시간(High)
		WORD				m_fGatherRate;				// 채집 확률
		WORD				m_wGatherAnimation;			// 애니메이션

		};
	};

	SCROWACTION				m_sAction;
	SCROWGEN				m_sGenerate;

	//	npc talk 정보.
	CNpcDialogueSet			m_sNpcTalkDlg;

	//	공격, Skill 설정.
	SCROWATTACK				m_sCrowAttack[EMMAXATTACK];

	//	생성 아이템군 설정 [server 에서만 필요한 정보]
	SGENITEM				m_sGenItem;
	SQTGENITEM				m_sQtGenItem; // Question Item 생성 by 경대

	//	판매 목록.
	std::string				m_strSaleType[SCROWACTION::SALENUM];
	GLInventory				m_sSaleItems[SCROWACTION::SALENUM];
	//  판매 NPC에게 입력된 가격
	NPCSELL_PRICE_MAP		m_mapNpcSellPrice;

	//	에니메이션 정보. ( 시간/스탭 ) [server 에서만 필요한 정보]
	//	
	VECANIATTACK*			m_pANIMATION;

	VECANIATTACK&			GetAnimation ( EMANI_MAINTYPE emMType, EMANI_SUBTYPE emSType );

	BOOL IsCdCertify ()		{ return m_sAction.m_dwActFlag&EMCROWACT_CDCERTIFY; }
	BOOL IsBusUnit ()		{ return m_sAction.m_dwActFlag&EMCROWACT_BUSUNIT; }
	BOOL IsTarShort ()		{ return m_sAction.m_dwActFlag&EMCROWACT_TARSHORT; }
	BOOL IsIgnoreShock()	{ return m_sAction.m_dwActFlag&EMCROWACT_IGNORE_SHOCK; }

	BOOL IsInVisible ()		{ return m_sAction.m_dwActFlag&EMCROWACT_INVISIBLE; }
	BOOL IsRecVisible ()	{ return m_sAction.m_dwActFlag&EMCROWACT_RECVISIBLE; }

	BOOL IsBarrier ()		{ return m_sAction.m_dwActFlag&EMCROWACT_BARRIER; }
	BOOL IsPosHold ()		{ return m_sAction.m_dwActFlag&EMCROWACT_POSHOLD; }
	BOOL IsDirHold ()		{ return m_sAction.m_dwActFlag&EMCROWACT_DIRHOLD; }

	DWORD GetSaleNum () { return SCROWACTION::SALENUM; }
	GLInventory* GetSaleInven ( DWORD dwIndex )
	{
		if ( GetSaleNum() <= dwIndex )	return NULL;
		return &m_sSaleItems[dwIndex];
	}
	std::string GetSaleType ( DWORD dwIndex )
	{
		if ( GetSaleNum() <= dwIndex )	return "";
		return m_strSaleType[dwIndex];
	}
	LONGLONG GetNpcSellPrice ( DWORD dwID )
	{
		NPCSELL_PRICE_MAP_ITER it = m_mapNpcSellPrice.find(dwID);
		if( it == m_mapNpcSellPrice.end() )
		{
			return 0;
		}
		return it->second;
	}

	const char* GetName();
	const char* GetTalkFile ()		{ return m_sAction.m_strTalkFile.c_str(); }
	const char* GetSkinObjFile ()	{ return m_sAction.m_strSkinObj.c_str(); }

	void LoadAniSet ( const char* szSkinObj );
	SCROWDATA& Assign ( SCROWDATA &Data );

	HRESULT LoadFile ( basestream &SFile, BOOL bServer, bool bPastLoad );
	HRESULT SaveFile ( CSerialFile &SFile );

	static VOID SaveCsvHead ( std::fstream &SFile );
	VOID SaveCsv ( std::fstream &SFile );
	VOID LoadCsv ( CStringArray &StrArray );

	SCROWDATA () :
		m_pANIMATION(NULL)
	{
		for ( int i=0; i<SCROWACTION::SALENUM; ++i )
		{
			m_sSaleItems[i].SetState ( SALE_INVEN_X, SALE_INVEN_Y );
			m_mapNpcSellPrice.clear();
		}
	}

	~SCROWDATA ()
	{
		SAFE_DELETE_ARRAY(m_pANIMATION);
	}
};
typedef SCROWDATA*		PCROWDATA;

BOOL SaleInvenLoadFile ( const char* szFileName, GLInventory &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice );

class GLCrowDataMan
{
public:
	enum
	{
		ENCODE_VER = 0x0102,

		VERSION = 0x0102,
		MAX_CROW_MID = 1024,
		MAX_CROW_SID = 64
	};

public:
	static const char*	_FILEHEAD;
	static const char*	_LOGFILE;
	static const char*	_STRINGTABLE;

protected:
	char				m_szFileName[MAX_PATH];

	PCROWDATA**			m_ppPCROWDATA;
	bool				m_bModify;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	PCROWDATA GetMainCrowData ( WORD wMainID );

	PCROWDATA GetCrowData ( WORD wMID, WORD wSID );
	PCROWDATA GetCrowData ( SNATIVEID sNativeID );

	bool	ValidData ();
	bool	IsModify ()								{ return m_bModify; }

public:
	WORD FindFreeCrowDataMID ();
	WORD FindFreeCrowDataSID ( WORD wMainID );

public:
	BOOL InsertCrowData ( WORD wMID, WORD wSID, PCROWDATA pCrowData, bool binner=false );
	BOOL InsertCrowData ( SNATIVEID sNativeID, PCROWDATA pCrowData, bool binner=false )
	{
		return InsertCrowData ( sNativeID.wMainID, sNativeID.wSubID, pCrowData, binner );
	}

public:
	BOOL DeleteCrowData ( WORD wMID, WORD wSID );
	BOOL DeleteCrowData ( SNATIVEID sNativeID );

public:
	HRESULT OneTimeSceneInit ();
	HRESULT FinalCleanup ();

public:
	HRESULT	SyncStringTable();
	HRESULT SyncUpdateData ();
	HRESULT SaveCsvFile( CWnd* pWnd );
	HRESULT LoadCsvFile( CWnd* pWnd );

public:
	HRESULT LoadFile ( const char* szFile, BOOL bServer, bool bPastLoad );
	HRESULT SaveFile ( const char* szFile );

public:
	GLCrowDataMan ();

public:
	~GLCrowDataMan ();

public:
	static GLCrowDataMan& GetInstance();
};

struct SDROP_CROW
{
	enum { CROW_GEM=0x0001, };

	SNATIVEID			sNativeID;	//	Crow 고유 ID.

	SNATIVEID			sMapID;		//	맵 ID.
	DWORD				dwCeID;		//	셀 ID.
	DWORD				dwGlobID;	//	생성 메모리 인덱스용.
	D3DXVECTOR3			vPos;		//	위치.
	D3DXVECTOR3			vDir;		//	방향.
	FLOAT				fScale;		//  크기.

	// 서버 클라이언트간 몹의 위치 동기화를 위해
	D3DXVECTOR3			vStartPos;  // 이동시 시작위치. 추가된거
	NavigationPath::WAYPOINT sNextWaypoint; // 다음 웨이포인트. 추가된거

	DWORD				dwNowHP;	//	생명량.
	WORD				wNowMP;		//	정신량.

	DWORD				dwFLAGS;	//	기타 속성.
	EMACTIONTYPE		emAction;	//	행동.
	DWORD				dwActState;	//	행동 상태.

	STARGETID			TargetID;	//	목표 유닛, 위치.

	SDROP_SKILLFACT		sSKILLFACT[SKILLFACT_SIZE];
	SDROP_STATEBLOW		sSTATEBLOWS[EMBLOW_MULTI];

	SDROP_CROW () 
		: sMapID(NATIVEID_NULL())
		, dwCeID(0)
		, dwGlobID(0)
		, vPos(0,0,0)
		, vDir(1,0,0)
		, dwNowHP(0)
		, wNowMP(0)
		, dwFLAGS(NULL)
		, emAction(GLAT_IDLE)
		, dwActState(0)
	{
	}
};

struct SDROP_MATERIAL
{
	enum { CROW_GEM=0x0001, };

	SNATIVEID			sNativeID;	//	Crow 고유 ID.

	SNATIVEID			sMapID;		//	맵 ID.
	DWORD				dwCeID;		//	셀 ID.
	DWORD				dwGlobID;	//	생성 메모리 인덱스용.
	D3DXVECTOR3			vPos;		//	위치.
	D3DXVECTOR3			vDir;		//	방향.

	DWORD				dwFLAGS;	//	기타 속성.
	EMACTIONTYPE		emAction;	//	행동.
	DWORD				dwActState;	//	행동 상태.

	SDROP_MATERIAL () 
		: sMapID(NATIVEID_NULL())
		, dwCeID(0)
		, dwGlobID(0)
		, vPos(0,0,0)
		, vDir(1,0,0)
		, dwFLAGS(NULL)
		, emAction(GLAT_IDLE)
		, dwActState(0)
	{
	}
};



namespace COMMENT
{
	extern std::string szCROWACT_UP[EMCROWACT_UP_NSIZE];
	extern std::string szCROWACT_DN[EMCROWACT_DN_NSIZE];
};

class CROW_PATTERN_CMP
{
public:
	bool operator () ( const SCROWPATTERN &lvalue, const SCROWPATTERN &rvalue )
	{
		if( lvalue.m_fPatternDNRate > rvalue.m_fPatternDNRate ) return true;
		return false;
	}
};


