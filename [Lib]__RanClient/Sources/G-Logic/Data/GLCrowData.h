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

	BOOL			bUsed;						//	��밡��.
	EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
	WORD			wRange;						//	���ݰ��� �Ÿ�.
	WORD			wSphere;					//	���ݿ��� ����.

	//	Skill �� ���� �޴� ��ǥ.
	EMIMPACT_TAR	emImpact_Tar;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
	EMIMPACT_REALM	emImpact_Realm;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
	EMIMPACT_SIDE	emImpact_Side;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )

	EMELEMENT		emElement;					//	���� �Ӽ�.
	GLPADATA		sDamage;					//	Ÿ�ݰ�.
	float			fDelay;						//	������.
	float			fLife;						//	���� �ð�.

	WORD			wUse_MP;					//	���� MP ������.
	WORD			wUse_SP;					//	���� SP ������.

	EMSTATE_BLOW	emBlow;						//	�����̻�.
	float			fBlowRATE;					//	�߻� Ȯ��.
	float			fBlowVALUE;					//	���� ��.

	EMIMPACT_ADDON	emImpact;					//	�ΰ�ȿ�� ����.
	int				nImpactVAR;					//	��ȭ ��ġ.

	char			szAniFile[ACF_SZNAME];		//	���ϸ��̼� ���� �̸�.
	SANIATTACK		sAniAttack;					//	���ϸ��̼� ����.

	char			szSelfBodyEffect[ACF_SZNAME];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
	char			szTargBodyEffect[ACF_SZNAME];	//	��ǥ ������ ���� �״� ����Ʈ.
	char			szTargetEffect[ACF_SZNAME];		//	��ǥ ���� ����Ʈ.
};

//----------------------------------------------------------------------
struct SCROWATTACK_101
{
	BOOL			bUsed;						//	��밡��.
	EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
	WORD			wRange;						//	���ݰ��� �Ÿ�.

	GLPADATA		sDamage;					//	Ÿ�ݰ�.
	float			fDelay;						//	������.

	WORD			wUse_SP;					//	���� SP ������.

	char			szAniFile[ACF_SZNAME];		//	���ϸ��̼� ���� �̸�.
	SANIATTACK		sAniAttack;					//	���ϸ��̼� ����.

	SNATIVEID		skill_id;					//	������ ��ų ����.

	char			szSelfBodyEffect[ACF_SZNAME];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
	char			szTargBodyEffect[ACF_SZNAME];	//	��ǥ ������ ���� �״� ����Ʈ.
	char			szTargetEffect[ACF_SZNAME];		//	��ǥ ���� ����Ʈ.
};

struct SCROWATTACK_102
{
	BOOL			bUsed;						//	��밡��.
	EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
	WORD			wRange;						//	���ݰ��� �Ÿ�.

	GLPADATA		sDamage;					//	Ÿ�ݰ�.
	float			fDelay;						//	������.

	WORD			wUse_SP;					//	���� SP ������.

	char			szAniFile[ACF_SZNAME];		//	���ϸ��̼� ���� �̸�.
	SANIATTACK		sAniAttack;					//	���ϸ��̼� ����.

	SNATIVEID		skill_id;					//	������ ��ų ����.

	EMSTATE_BLOW	emBLOW_TYPE;				//	'�����̻�' ���� Ÿ��.
	float			fBLOW_RATE;					//	�߻� Ȯ��.
	float			fBLOW_LIFE;					//	���� �ð�.
	float			fBLOW_VAR1;					//	���� �� 1.
	float			fBLOW_VAR2;					//	���� �� 2.

	char			szSelfBodyEffect[ACF_SZNAME];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
	char			szTargBodyEffect[ACF_SZNAME];	//	��ǥ ������ ���� �״� ����Ʈ.
	char			szTargetEffect[ACF_SZNAME];		//	��ǥ ���� ����Ʈ.
};

enum EMCROWSKTAR
{
	CROWSKTAR_OUR	= 0,
	CROWSKTAR_ENEMY	= 1,
};

struct SCROWATTACK_103
{
	BOOL			bUsed;						//	��밡��.
	EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
	WORD			wRange;						//	���ݰ��� �Ÿ�.

	GLPADATA		sDamage;					//	Ÿ�ݰ�.
	float			fDelay;						//	������.

	WORD			wUse_SP;					//	���� SP ������.

	char			szAniFile[ACF_SZNAME];		//	���ϸ��̼� ���� �̸�.
	SANIATTACK_100	sAniAttack;					//	���ϸ��̼� ����.

	SNATIVEID		skill_id;					//	��ų ����.
	WORD			skill_lev;					//	��ų ����.
	EMCROWSKTAR		skill_tar;					//	��ų Ÿ��.

	EMSTATE_BLOW	emBLOW_TYPE;				//	'�����̻�' ���� Ÿ��.
	float			fBLOW_RATE;					//	�߻� Ȯ��.
	float			fBLOW_LIFE;					//	���� �ð�.
	float			fBLOW_VAR1;					//	���� �� 1.
	float			fBLOW_VAR2;					//	���� �� 2.

	char			szSelfBodyEffect[ACF_SZNAME];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
	char			szTargBodyEffect[ACF_SZNAME];	//	��ǥ ������ ���� �״� ����Ʈ.
	char			szTargetEffect[ACF_SZNAME];		//	��ǥ ���� ����Ʈ.
};

struct SCROWATTACK_104
{
	BOOL			bUsed;						//	��밡��.
	EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
	WORD			wRange;						//	���ݰ��� �Ÿ�.

	GLPADATA		sDamage;					//	Ÿ�ݰ�.
	float			fDelay;						//	������.

	WORD			wUse_SP;					//	���� SP ������.

	char			szAniFile[ACF_SZNAME];		//	���ϸ��̼� ���� �̸�.
	SANIATTACK		sAniAttack;					//	���ϸ��̼� ����.

	SNATIVEID		skill_id;					//	��ų ����.
	WORD			skill_lev;					//	��ų ����.
	EMCROWSKTAR		skill_tar;					//	��ų Ÿ��.

	EMSTATE_BLOW	emBLOW_TYPE;				//	'�����̻�' ���� Ÿ��.
	float			fBLOW_RATE;					//	�߻� Ȯ��.
	float			fBLOW_LIFE;					//	���� �ð�.
	float			fBLOW_VAR1;					//	���� �� 1.
	float			fBLOW_VAR2;					//	���� �� 2.

	char			szSelfBodyEffect[ACF_SZNAME];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
	char			szTargBodyEffect[ACF_SZNAME];	//	��ǥ ������ ���� �״� ����Ʈ.
	char			szTargetEffect[ACF_SZNAME];		//	��ǥ ���� ����Ʈ.
};

//----------------------------------------------------------------------
struct SCROWATTACK
{
	enum { VERSION = 0x0105, };

	BOOL			bUsed;						//	��밡��.
	EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
	WORD			wRange;						//	���ݰ��� �Ÿ�.

	GLPADATA		sDamage;					//	Ÿ�ݰ�.
	float			fDelay;						//	������.

	WORD			wUse_SP;					//	���� SP ������.

	std::string		strAniFile;					//	���ϸ��̼� ���� �̸�.
	SANIATTACK		sAniAttack;					//	���ϸ��̼� ����.

	SNATIVEID		skill_id;					//	��ų ����.
	WORD			skill_lev;					//	��ų ����.
	EMCROWSKTAR		skill_tar;					//	��ų Ÿ��.

	EMSTATE_BLOW	emBLOW_TYPE;				//	'�����̻�' ���� Ÿ��.
	float			fBLOW_RATE;					//	�߻� Ȯ��.
	float			fBLOW_LIFE;					//	���� �ð�.
	float			fBLOW_VAR1;					//	���� �� 1.
	float			fBLOW_VAR2;					//	���� �� 2.

	std::string		strSelfBodyEffect;			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
	std::string		strTargBodyEffect;			//	��ǥ ������ ���� �״� ����Ʈ.
	std::string		strTargetEffect;			//	��ǥ ���� ����Ʈ.

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
	SNATIVEID			sNativeID;					//	���� ID.
	char				m_szName[CHAR_SZNAME];		//	�̸�.
	EMTRIBE				m_emTribe;					//	����.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	WORD				m_wLevel;					//	����.

	WORD				m_wHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wDefense;					//	����.

	SRESIST_101			m_sResist;					//	���װ�.
};

struct SCROWBASIC_101
{
	SNATIVEID			sNativeID;					//	���� ID.
	char				m_szName[CHAR_SZNAME];		//	�̸�.
	EMTRIBE				m_emTribe;					//	����.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	WORD				m_wLevel;					//	����.

	WORD				m_wHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wDefense;					//	����.

	SRESIST				m_sResist;					//	���װ�.
};

struct SCROWBASIC_102
{
	SNATIVEID			sNativeID;					//	���� ID.
	char				m_szName[CHAR_SZNAME];		//	�̸�.
	EMTRIBE				m_emTribe;					//	����.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	WORD				m_wLevel;					//	����.

	WORD				m_wHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wDefense;					//	����.

	SRESIST				m_sResist;					//	���װ�.

	WORD				m_wBonusExp;				//	�߰� ����ġ.
};

struct SCROWBASIC_103
{
	SNATIVEID			sNativeID;					//	���� ID.

	char				m_szName[CHAR_SZNAME];		//	�̸�.

	EMTRIBE				m_emTribe;					//	����.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	WORD				m_wLevel;					//	����.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wDefense;					//	����.

	SRESIST				m_sResist;					//	���װ�.

	WORD				m_wBonusExp;				//	�߰� ����ġ.
};

struct SCROWBASIC_104
{
	SNATIVEID			sNativeID;					//	���� ID.

	char				m_szName[CHAR_SZNAME];		//	�̸�.

	EMTRIBE				m_emTribe;					//	����.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	// ��Ÿ�� �߰�
	PETTYPE				m_emPetType;				// ��Ÿ��

	WORD				m_wLevel;					//	����.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wDefense;					//	����.

	SRESIST				m_sResist;					//	���װ�.

	WORD				m_wBonusExp;				//	�߰� ����ġ.
};

struct SCROWBASIC_105
{
	SNATIVEID			sNativeID;					//	���� ID.

	char				m_szName[CHAR_SZNAME];		//	�̸�.

	EMTRIBE				m_emTribe;					//	����.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	WORD				m_wLevel;					//	����.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wDefense;					//	����.

	SRESIST				m_sResist;					//	���װ�.

	WORD				m_wBonusExp;				//	�߰� ����ġ.

	// ��Ÿ�� �߰�
	PETTYPE				m_emPetType;				// ��Ÿ��
};

struct SCROWBASIC_106
{
	SNATIVEID			sNativeID;					//	���� ID.

	char				m_szName[CHAR_SZNAME];		//	�̸�.

	EMTRIBE				m_emTribe;					//	����.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	WORD				m_wLevel;					//	����.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wDefense;					//	����.

	SRESIST				m_sResist;					//	���װ�.

	WORD				m_wBonusExp;				//	�߰� ����ġ.

	bool				m_bOverlapAttack;			//  �������� ���� ���� ����

	// ��Ÿ�� �߰�
	PETTYPE				m_emPetType;				// ��Ÿ��
};

struct SCROWBASIC_108
{
	SNATIVEID			sNativeID;					//	���� ID.

	char				m_szName[CHAR_SZNAME];		//	�̸�.

	EMTRIBE				m_emTribe;					//	����.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	WORD				m_wLevel;					//	����.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wDefense;					//	����.

	SRESIST				m_sResist;					//	���װ�.

	DWORD				m_wBonusExp;				//	�߰� ����ġ.

	bool				m_bOverlapAttack;			//  �������� ���� ���� ����

	// ��Ÿ�� �߰�
	PETTYPE				m_emPetType;				// ��Ÿ��
};

struct SCROWBASIC_109
{
	SNATIVEID			sNativeID;					//	���� ID.

	char				m_szName[CHAR_SZNAME];		//	�̸�.

	EMTRIBE				m_emTribe;					//	����.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	WORD				m_wLevel;					//	����.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wDefense;					//	����.

	SRESIST				m_sResist;					//	���װ�.

	DWORD				m_wBonusExp;				//	�߰� ����ġ.

	bool				m_bOverlapAttack;			//  �������� ���� ���� ����

	// ��Ÿ�� �߰�
	PETTYPE				m_emPetType;				// ��Ÿ��
	
	DWORD				m_dwGenTime;				// ȣ�� �ð�
};

struct SCROWBASIC
{
	enum { VERSION = MOBNPCEDIT_VERSION, };

	SNATIVEID			sNativeID;					//	���� ID.

	char				m_szName[CHAR_SZNAME];		//	�̸�.

	EMTRIBE				m_emTribe;					//	����.
	EMCROW				m_emCrow;					//	Npc, Mob.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	WORD				m_wLevel;					//	����.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wDefense;					//	����.

	SRESIST				m_sResist;					//	���װ�.

	DWORD				m_wBonusExp;				//	�߰� ����ġ.

	bool				m_bOverlapAttack;			//  �������� ���� ���� ����

	// ��Ÿ�� �߰�
	PETTYPE				m_emPetType;				// ��Ÿ��

	DWORD				m_dwGenTime;				// ȣ�� �ð�
	WORD				m_wGatherTimeLow;			// ä���ð�(Low)
	WORD				m_wGatherTimeHigh;			// ä���ð�(High)
	float				m_fGatherRate;				// ä��Ȯ��
	WORD				m_wGatherAnimation;			// �ִϸ��̼�

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
	WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
	char				m_szSkinObj[ACF_SZNAME];		//	��Ų ����.
	char				m_szTalkFile[ACF_SZNAME];		//	��ȭ ����.

	BOOL				m_bTrade;						//	�ŷ���������.
	char				m_szSaleFile[ACF_SZNAME];		//	Item �Ǹ� ���.

	BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
	char				m_szFallingEffect[ACF_SZNAME];	//	�״� ������ ������ ȿ��.
	char				m_szBlowEffect[ACF_SZNAME];		//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

	DWORD				m_dwActionUP;					//	�ൿ ���� HP 50%�̻�.
	DWORD				m_dwActionDN;					//	�ൿ ���� HP 50%����.

	EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
	float				m_fDriftHeight;					//	�ξ� ����.

	float				m_fWalkVelo;					//	�̵� �ӵ�.
	BOOL				m_bRun;							//	�ٱ� ����.
	float				m_fRunVelo;						//	�ٱ� �ӵ�.
};

struct SCROWACTION_101
{
	WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
	char				m_szSkinObj[ACF_SZNAME];		//	��Ų ����.
	char				m_szTalkFile[ACF_SZNAME];		//	��ȭ ����.

	DWORD				m_dwActFlag;					//	���� �ൿ.
	char				m_szSaleFile[ACF_SZNAME];		//	Item �Ǹ� ���.

	BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
	char				m_szBirthEffect[ACF_SZNAME];	//	ź���� ������ ȿ��.
	char				m_szFallingEffect[ACF_SZNAME];	//	�״� ������ ������ ȿ��.
	char				m_szBlowEffect[ACF_SZNAME];		//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

	EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
	EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

	EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
	float				m_fDriftHeight;					//	�ξ� ����.

	float				m_fWalkVelo;					//	�̵� �ӵ�.
	BOOL				m_bRun;							//	�ٱ� ����.
	float				m_fRunVelo;						//	�ٱ� �ӵ�.
};

struct SCROWACTION_102
{
	enum { SALENUM = 3 };
	WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
	char				m_szSkinObj[ACF_SZNAME];		//	��Ų ����.
	char				m_szTalkFile[ACF_SZNAME];		//	��ȭ ����.

	DWORD				m_dwActFlag;					//	���� �ൿ.
	char				m_szSaleFile[SALENUM][ACF_SZNAME];		//	Item �Ǹ� ���.

	BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
	char				m_szBirthEffect[ACF_SZNAME];	//	ź���� ������ ȿ��.
	char				m_szFallingEffect[ACF_SZNAME];	//	�״� ������ ������ ȿ��.
	char				m_szBlowEffect[ACF_SZNAME];		//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

	EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
	EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

	EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
	float				m_fDriftHeight;					//	�ξ� ����.

	float				m_fWalkVelo;					//	�̵� �ӵ�.
	BOOL				m_bRun;							//	�ٱ� ����.
	float				m_fRunVelo;						//	�ٱ� �ӵ�.
};

struct SCROWACTION_103
{
	enum { VERSION = 0x0103, SALENUM = 3 };

	WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
	std::string			m_strSkinObj;					//	��Ų ����.
	std::string			m_strTalkFile;					//	��ȭ ����.

	DWORD				m_dwActFlag;					//	���� �ൿ.
	std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

	BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
	std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
	std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
	std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

	EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
	EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

	EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
	float				m_fDriftHeight;					//	�ξ� ����.

	float				m_fWalkVelo;					//	�̵� �ӵ�.
	BOOL				m_bRun;							//	�ٱ� ����.
	float				m_fRunVelo;						//	�ٱ� �ӵ�.
};

struct SCROWACTION_104
{
	enum { VERSION = 0x0104, SALENUM = 3 };

	WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
	std::string			m_strSkinObj;					//	��Ų ����.
	std::string			m_strTalkFile;					//	��ȭ ����.

	DWORD				m_dwActFlag;					//	���� �ൿ.
	std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

	BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
	std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
	std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
	std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

	EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
	EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

	EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
	float				m_fDriftHeight;					//	�ξ� ����.

	float				m_fWalkVelo;					//	�̵� �ӵ�.
	BOOL				m_bRun;							//	�ٱ� ����.
	float				m_fRunVelo;						//	�ٱ� �ӵ�.

	float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)
};

struct SCROWACTION_105
{
	enum { VERSION = 0x0105, SALENUM = 3 };

	WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
	std::string			m_strSkinObj;					//	��Ų ����.
	std::string			m_strTalkFile;					//	��ȭ ����.

	DWORD				m_dwActFlag;					//	���� �ൿ.
	std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

	BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
	std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
	std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
	std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

	EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
	EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

	EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
	float				m_fDriftHeight;					//	�ξ� ����.

	float				m_fWalkVelo;					//	�̵� �ӵ�.
	BOOL				m_bRun;							//	�ٱ� ����.
	float				m_fRunVelo;						//	�ٱ� �ӵ�.

	float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)
	float				m_fActionDNRate;				// �ൿ ���� ���� %
};


struct SCROWACTION_106
{
	enum { VERSION = 0x0106, SALENUM = 3 };

	WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
	std::string			m_strSkinObj;					//	��Ų ����.
	std::string			m_strTalkFile;					//	��ȭ ����.

	DWORD				m_dwActFlag;					//	���� �ൿ.
	std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

	BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
	std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
	std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
	std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

	EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
	EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

	EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
	float				m_fDriftHeight;					//	�ξ� ����.

	float				m_fWalkVelo;					//	�̵� �ӵ�.
	BOOL				m_bRun;							//	�ٱ� ����.
	float				m_fRunVelo;						//	�ٱ� �ӵ�.

	float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)
	float				m_fActionDNRate;				// �ൿ ���� ���� %

	BOOL				m_bMobLink;						// �ش���Ͱ� ������ ����Ǿ��ִ� ���͸� ȣ������ ����.
	SNATIVEID			m_sMobLinkID;					// ����� ������ ID
};

struct SCROWACTION_107
{
	enum { VERSION = 0x0107, SALENUM = 3, PATTERNNUM = 10, ATTACKSETNUM = 4 };

	WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
	std::string			m_strSkinObj;					//	��Ų ����.
	std::string			m_strTalkFile;					//	��ȭ ����.

	DWORD				m_dwActFlag;					//	���� �ൿ.
	std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

	BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
	std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
	std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
	std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

	EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
	EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

	EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
	float				m_fDriftHeight;					//	�ξ� ����.

	float				m_fWalkVelo;					//	�̵� �ӵ�.
	BOOL				m_bRun;							//	�ٱ� ����.
	float				m_fRunVelo;						//	�ٱ� �ӵ�.

	float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)
	float				m_fActionDNRate;				// �ൿ ���� ���� %

	BOOL				m_bMobLink;						// �ش���Ͱ� ������ ����Ǿ��ִ� ���͸� ȣ������ ����.
	SNATIVEID			m_sMobLinkID;					// ����� ������ ID

	SCROWPATTERN		m_Pattern;						
	std::vector< SCROWPATTERN >	m_vecPatternList;
};

struct SCROWACTION
{
	enum { VERSION = 0x0108, SALENUM = 3, PATTERNNUM = 10, ATTACKSETNUM = 4 };

	WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
	std::string			m_strSkinObj;					//	��Ų ����.
	std::string			m_strTalkFile;					//	��ȭ ����.

	DWORD				m_dwActFlag;					//	���� �ൿ.
	std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

	BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
	std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
	std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
	std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

	EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
	EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

	EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
	float				m_fDriftHeight;					//	�ξ� ����.

	float				m_fWalkVelo;					//	�̵� �ӵ�.
	BOOL				m_bRun;							//	�ٱ� ����.
	float				m_fRunVelo;						//	�ٱ� �ӵ�.

	float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)
	float				m_fActionDNRate;				// �ൿ ���� ���� %

	BOOL				m_bMobLink;						// �ش���Ͱ� ������ ����Ǿ��ִ� ���͸� ȣ������ ����.
	SNATIVEID			m_sMobLinkID;					// ����� ������ ID
	float				m_fMobLinkScale;				// ����� ���� ��ü ũ��
	float				m_fMobLinkDelay;				// ����� ���� ���� ������

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
	DWORD				m_dwGenMoney;				//	�߻� �ݾ�.
	SNATIVEID			m_sGenItemID;				//	�߻� ������.
	WORD				m_wGenMoney_Rate;			//	�߻� �ݾ� Ȯ��.
	WORD				m_wGenItem_Rate;			//	�߻� ������ Ȯ��.
};

struct SCROWGEN_101
{
	DWORD				m_dwGenMoney;				//	�߻� �ݾ�.
	SNATIVEID			m_sGenItemID;				//	�߻� ������.
	WORD				m_wGenMoney_Rate;			//	�߻� �ݾ� Ȯ��.
	WORD				m_wGenItem_Rate;			//	�߻� ������ Ȯ��.

	char				m_szGenItem[ACF_SZNAME];	//	�߻� ������ ���� ����.
};

struct SCROWGEN
{
	enum { VERSION = 0x0102, };

	//	�߻�������.
	DWORD				m_dwGenMoney;				//	�߻� �ݾ�.
	SNATIVEID			m_sGenItemID;				//	�߻� ������.
	WORD				m_wGenMoney_Rate;			//	�߻� �ݾ� Ȯ��.
	WORD				m_wGenItem_Rate;			//	�߻� ������ Ȯ��.

	std::string			m_strGenItem;				//	�߻� ������ ���� ����.
	std::string			m_strQtGenItem;				//	Question Item ���� ����.

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

//	Note : �ټ� ������ �߻��� ó�� ����.
//
struct SGENITEM
{
	enum
	{
		EMGNUM		= 5,
		EMGSPEC		= 30,
		EMSPEC_NULL	= -1
	};

	struct SGNUM	//	�߻� ������ Ȯ��.
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

	struct SGITEMSPEC	//	�߻� �����۱��� Ȯ��.
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

	float		m_fGenRate;				//	�߻� Ȯ��.
	SGNUM		m_sGenNum[EMGNUM];		//	�߻� ������ Ȯ�� ����.
	SGITEMSPEC	m_sItemSpec[EMGSPEC];	//	�߻� ������ ��.

	SGENITEM() :
		m_fGenRate(0)
	{
	}

	BOOL LOADFILE ( const char* szFileName );

	BOOL ISGEN ( float fGenRate = 1.0f );							//	�߻� Ȯ��.
	WORD SELECTNUM ();						//	�߻� ����.
	DWORD SELECTSPECID ( float fGEN_RATE, bool& bSPECID );	//	�߻� ��.
};

//	Note : Question ������ �߻��� ó�� ���� by ���
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

	SNATIVEID SELECTITEM(); // ������ �߻�
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
		SNATIVEID			sNativeID;					//	���� ID.
		char				m_szName[CHAR_SZNAME];		//	�̸�.
		EMTRIBE				m_emTribe;					//	����.
		EMCROW				m_emCrow;					//	Npc, Mob.
		EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

		WORD				m_wLevel;					//	����.

		DWORD				m_dwHP;						//	HP
		WORD				m_wMP;						//	MP
		WORD				m_wSP;						//	SP

		//	ȸ����.
		float				m_fIncHP;					//	HP ȸ����.
		float				m_fIncMP;					//	MP ȸ����.
		float				m_fIncSP;					//	SP ȸ����.

		WORD				m_wAvoidRate;				//	ȸ����.
		WORD				m_wHitRate;					//	������.

		WORD				m_wViewRange;				//	�þ� �Ÿ�.
		WORD				m_wDefense;					//	����.

		SRESIST				m_sResist;					//	���װ�.

		DWORD				m_wBonusExp;				//	�߰�����ġ.

		bool				m_bOverlapAttack;			//  �������� ���� ���� ����

		// PET
		PETTYPE				m_emPetType;
		DWORD				m_dwGenTime;

		WORD				m_wGatherTimeLow;			// ä���ð�(Low)
		WORD				m_wGatherTimeHigh;			// ä���ð�(High)
		WORD				m_fGatherRate;				// ä�� Ȯ��
		WORD				m_wGatherAnimation;			// �ִϸ��̼�

		};
	};

	SCROWACTION				m_sAction;
	SCROWGEN				m_sGenerate;

	//	npc talk ����.
	CNpcDialogueSet			m_sNpcTalkDlg;

	//	����, Skill ����.
	SCROWATTACK				m_sCrowAttack[EMMAXATTACK];

	//	���� �����۱� ���� [server ������ �ʿ��� ����]
	SGENITEM				m_sGenItem;
	SQTGENITEM				m_sQtGenItem; // Question Item ���� by ���

	//	�Ǹ� ���.
	std::string				m_strSaleType[SCROWACTION::SALENUM];
	GLInventory				m_sSaleItems[SCROWACTION::SALENUM];
	//  �Ǹ� NPC���� �Էµ� ����
	NPCSELL_PRICE_MAP		m_mapNpcSellPrice;

	//	���ϸ��̼� ����. ( �ð�/���� ) [server ������ �ʿ��� ����]
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

	SNATIVEID			sNativeID;	//	Crow ���� ID.

	SNATIVEID			sMapID;		//	�� ID.
	DWORD				dwCeID;		//	�� ID.
	DWORD				dwGlobID;	//	���� �޸� �ε�����.
	D3DXVECTOR3			vPos;		//	��ġ.
	D3DXVECTOR3			vDir;		//	����.
	FLOAT				fScale;		//  ũ��.

	// ���� Ŭ���̾�Ʈ�� ���� ��ġ ����ȭ�� ����
	D3DXVECTOR3			vStartPos;  // �̵��� ������ġ. �߰��Ȱ�
	NavigationPath::WAYPOINT sNextWaypoint; // ���� ��������Ʈ. �߰��Ȱ�

	DWORD				dwNowHP;	//	����.
	WORD				wNowMP;		//	���ŷ�.

	DWORD				dwFLAGS;	//	��Ÿ �Ӽ�.
	EMACTIONTYPE		emAction;	//	�ൿ.
	DWORD				dwActState;	//	�ൿ ����.

	STARGETID			TargetID;	//	��ǥ ����, ��ġ.

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

	SNATIVEID			sNativeID;	//	Crow ���� ID.

	SNATIVEID			sMapID;		//	�� ID.
	DWORD				dwCeID;		//	�� ID.
	DWORD				dwGlobID;	//	���� �޸� �ε�����.
	D3DXVECTOR3			vPos;		//	��ġ.
	D3DXVECTOR3			vDir;		//	����.

	DWORD				dwFLAGS;	//	��Ÿ �Ӽ�.
	EMACTIONTYPE		emAction;	//	�ൿ.
	DWORD				dwActState;	//	�ൿ ����.

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


