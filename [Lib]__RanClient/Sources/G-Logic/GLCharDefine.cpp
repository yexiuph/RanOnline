#include "pch.h"

#include "./G-Logic/GLCharDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

WORD school2index ( const EMSCHOOLFLAG emSchool )
{
	switch ( emSchool )
	{
	case GLSCHOOL_00:	return 0;
	case GLSCHOOL_01:	return 1;
	case GLSCHOOL_02:	return 2;
	};

	return 0;
}

EMSCHOOLFLAG index2school ( const WORD wSchool )
{
	switch ( wSchool )
	{
	case 0:	return GLSCHOOL_00;
	case 1:	return GLSCHOOL_01;
	case 2:	return GLSCHOOL_02;
	};

	return GLSCHOOL_00;
}

EMCHARINDEX CharClassToIndex ( const EMCHARCLASS emClass )
{
	switch (emClass)
	{
	case GLCC_FIGHTER_M:		return GLCI_FIGHTER_M;
	case GLCC_ARMS_M:			return GLCI_ARMS_M;
	case GLCC_ARCHER_W:		return GLCI_ARCHER_W;
	case GLCC_SPIRIT_W:		return GLCI_SPIRIT_W;
	case GLCC_EXTREME_M:	return GLCI_EXTREME_M;
	case GLCC_EXTREME_W:	return GLCI_EXTREME_W;
	
	case GLCC_FIGHTER_W:	return GLCI_FIGHTER_W;
	case GLCC_ARMS_W:		return GLCI_ARMS_W;
	case GLCC_ARCHER_M:		return GLCI_ARCHER_M;
	case GLCC_SPIRIT_M:		return GLCI_SPIRIT_M;
	};

	GASSERT(0&&"�߸��� �ɸ��� Ŭ�����Դϴ�.");
	return GLCI_FIGHTER_M;
}

EMCHARCLASS CharIndexToClass ( const EMCHARINDEX emIndex )
{
	switch (emIndex)
	{
	case GLCI_FIGHTER_M:	return GLCC_FIGHTER_M;
	case GLCI_ARMS_M:		return GLCC_ARMS_M;
	case GLCI_ARCHER_W:		return GLCC_ARCHER_W;
	case GLCI_SPIRIT_W:		return GLCC_SPIRIT_W;

	case GLCI_EXTREME_M:	return GLCC_EXTREME_M;
	case GLCI_EXTREME_W:	return GLCC_EXTREME_W;

	case GLCI_FIGHTER_W:	return GLCC_FIGHTER_W;
	case GLCI_ARMS_W:		return GLCC_ARMS_W;
	case GLCI_ARCHER_M:		return GLCC_ARCHER_M;
	case GLCI_SPIRIT_M:		return GLCC_SPIRIT_M;
	};

	GASSERT(0&&"�߸��� �ɸ��� �ε����Դϴ�.");
	return GLCC_FIGHTER_M;
}


EMCHARCLASS CharClassToSex ( const EMCHARCLASS emClass )
{
	switch (emClass)
	{
	case GLCC_FIGHTER_M:		return GLCC_FIGHTER_W;
	case GLCC_ARMS_M:			return GLCC_ARMS_W;
	case GLCC_ARCHER_W:		return GLCC_ARCHER_M;
	case GLCC_SPIRIT_W:		return GLCC_SPIRIT_M;
	case GLCC_EXTREME_M:	return GLCC_EXTREME_W;
	case GLCC_EXTREME_W:	return GLCC_EXTREME_M;
	
	case GLCC_FIGHTER_W:	return GLCC_FIGHTER_M;
	case GLCC_ARMS_W:		return GLCC_ARMS_M;
	case GLCC_ARCHER_M:		return GLCC_ARCHER_W;
	case GLCC_SPIRIT_M:		return GLCC_SPIRIT_W;
	};

	GASSERT(0&&"�߸��� �ɸ��� Ŭ�����Դϴ�.");
	return GLCC_FIGHTER_M;
}

EMCHARINDEX CharIndexToSex ( const EMCHARINDEX emIndex )
{
	switch (emIndex)
	{
	case GLCI_FIGHTER_M:	return GLCI_FIGHTER_W;
	case GLCI_ARMS_M:		return GLCI_ARMS_W;
	case GLCI_ARCHER_W:		return GLCI_ARCHER_M;
	case GLCI_SPIRIT_W:		return GLCI_SPIRIT_M;

	case GLCI_EXTREME_M:	return GLCI_EXTREME_W;
	case GLCI_EXTREME_W:	return GLCI_EXTREME_M;

	case GLCI_FIGHTER_W:	return GLCI_SPIRIT_W;
	case GLCI_ARMS_W:		return GLCI_ARCHER_W;
	case GLCI_ARCHER_M:		return GLCI_ARMS_M;
	case GLCI_SPIRIT_M:		return GLCI_FIGHTER_M;
	};

	GASSERT(0&&"�߸��� �ɸ��� �ε����Դϴ�.");
	return GLCI_FIGHTER_M;
}

VOID GetCharSkillClassIndex( const EMCHARCLASS emClass, WORD & wBeginIndex, WORD & wEndIndex )
{
	switch ( emClass )
	{
	case GLCC_FIGHTER_M:
	case GLCC_FIGHTER_W:
		{
			wBeginIndex = EMSKILL_FIGHTER_01;
			wEndIndex = EMSKILL_FIGHTER_04;
		}
		break;

	case GLCC_ARMS_M:
	case GLCC_ARMS_W:
		{
			wBeginIndex = EMSKILL_ARMS_01;
			wEndIndex = EMSKILL_ARMS_04;
		}
		break;

	case GLCC_ARCHER_M:
	case GLCC_ARCHER_W:
		{
			wBeginIndex = EMSKILL_ARCHER_01;
			wEndIndex = EMSKILL_ARCHER_04;
		}
		break;

	case GLCC_SPIRIT_M:
	case GLCC_SPIRIT_W:
		{
			wBeginIndex = EMSKILL_SPIRIT_01;
			wEndIndex = EMSKILL_SPIRIT_04;
		}
		break;	

	case GLCC_EXTREME_M:
	case GLCC_EXTREME_W:
		{
			wBeginIndex = EMSKILL_EXTREME_01;
			wEndIndex = EMSKILL_EXTREME_04;
		}
		break;

	default:
		GASSERT(0&&"�߸��� �ɸ��� �ε����Դϴ�.");
		break;
	};
}

namespace COMMENT
{
	std::string ATIONTYPE[GLAT_SIZE] =
	{
		"GLAT_IDLE",
		"GLAT_MOVE",
		"GLAT_ATTACK",
		"GLAT_SKILL",
		"GLAT_SHOCK",
		"GLAT_PUSHPULL",

		"GLAT_TALK",
		"GLAT_CONFT_END",

		"GLAT_FALLING",
		"GLAT_DIE",
	};

	std::string TRIBE[TRIBE_NSIZE] = 
	{
		"�ΰ�",
		"����",
		"õ��",
		"�Ǹ�",
		"����",
		"����",
		"�巡��"
	};

	std::string MOVETYPE[MOVETYPE_NSIZE] = 
	{
		"���� �̵�",
		"���� �̵�",
		"���� �̵�"
	};

	std::string ELEMENT[EMELEMENT_MAXNUM2] = 
	{
		"����",
		"��",
		"����",
		"����",
		"��",
		"��ȭ",
		"����",
		"����",
		"����",
		"������ ����"
	};

	std::string BRIGHT[BRIGHT_SIZE] =
	{
		"{��}",
		"{���}",
		"[��+���]"
	};

	std::string CHARCLASS[GLCI_NUM_NEWSEX] =
	{
		"�����γ�",	//"������",
		"�˵��γ�",	//"�˵���",
		"��úο�",	//"��ú�",
		"�⿹�ο�",	//"�⿹��",
		"�ذ��γ�",	//"�ذ��γ�",
		"�ذ��ο�",	//"�ذ��ο�",
		"�����ο�",	//"������",
		"�˵��ο�",	//"�˵���",
		"��úγ�",	//"��ú�",
		"�⿹�γ�",	//"�⿹��",
	};

	std::string EMSCHOOL[GLSCHOOL_NUM] =
	{
		"school 1",
		"school 2",
		"school 3",
	};

	std::string CHARCOMMENT[GLCI_NUM_NEWSEX] = 
	{
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	};

	std::string BLOW[EMBLOW_SIZE] =
	{
		"����",	// 0
		"����",	// 1
		"����",	// 2
		"��ȭ",	// 3
		"ȭ��",	// 4
		"�õ�",	// 5

		"����",	// 6
		"�ߵ�",	// 7
		"����",	// 8
	};

	std::string BLOW_VAR1[EMBLOW_SIZE] =
	{
		"����",
		"�̼Ӻ�ȭ��",	//"����"
		"����",			//"����"
		"�̼Ӻ�ȭ��",	//"��ȭ"
		"����",			//"ȭ��"
		"�̼Ӻ�ȭ��",	//"�õ�"

		"����",			//"����"
		"����",			//"�ߵ�"
		"����",			//"����"
	};

	float BLOW_VAR1_SCALE[EMBLOW_SIZE] =
	{
		1.0f,
		100.0f,			//"����"
		1.0f,			//"����"
		100.0f,			//"��ȭ"
		1.0f,			//"ȭ��"
		100.0f,			//"�õ�"

		1.0f,			//"����"
		1.0f,			//"�ߵ�"
		1.0f,			//"����"
	};

	std::string BLOW_VAR2[EMBLOW_SIZE] =
	{
		"����",
		"�����̺�ȭ��",		//"����"
		"����",				//"����"
		"����",				//"��ȭ"
		"����Ÿ��ġ",		//"ȭ��"
		"Ÿ�ݰ�����",		//"�õ�"

		"����",				//"����"
		"����Ÿ��ġ",		//"�ߵ�"
		"����Ÿ��ġ",		//"����"
	};

	float BLOW_VAR2_SCALE[EMBLOW_SIZE] =
	{
		1.0f,
		100.0f,				//"����"
		1.0f,				//"����"
		1.0f,				//"��ȭ"
		1.0f,				//"ȭ��"
		100.0f,				//"�õ�"

		1.0f,				//"����"
		1.0f,				//"�ߵ�"
		1.0f,				//"����"
	};

	std::string IMPACT_ADDON[EIMPACTA_SIZE] =
	{
		"����",
		"������ ��ȭ��",
		"ȸ���� ��ȭ��",
	
		"����ġ ��ȭ��",
		"���ġ ��ȭ��",

		"HP ��ȭ��",
		"MP ��ȭ��",
		"SP ��ȭ��",
		"HP+MP+SP ��ȭ��",

		"����ġ ��ȭ��",
		"���ġ ��ȭ��",

		"����ġ",
		"���ġ",
		"���ġ",

		"HP ������",
		"MP ������",
		"SP ������",
		"����ġ ��ȭ",
	};

	float IMPACT_ADDON_SCALE[EIMPACTA_SIZE] =
	{
		1.0f,
		1.0f,
		1.0f,
	
		1.0f,
		1.0f,

		100.0f,
		100.0f,
		100.0f,
		100.0f,

		100.0f,
		100.0f,

		1.0f,
		1.0f,
		1.0f,

		1.0f,
		1.0f,
		1.0f,
		1.0f,
	};

	std::string IMPACT_TAR[TAR_SIZE] =
	{
		"�ڽ�",
		"Ÿ��",
		"�ڽź��� Ÿ�ٱ���",
		"Ư����ġ"
	};

	std::string IMPACT_REALM[REALM_SIZE] =
	{
		"'��ǥ' �ڽ�",
		"'��ǥ' ����",
		"'��ǥ' ���� ����",
		"'��ǥ'�� ���� ��ä��"
	};

	std::string IMPACT_SIDE[SIDE_SIZE] =
	{
		"�ڱ�����",
		"������",
		"��ο���"
	};

	std::string SPEC_ADDON[EMSPECA_NSIZE] =
	{
		"��� ����",
		"����/�б�",
		
		"����� �ݻ�",
		"��Ȱ",

		"HP ������",
		"MP ������",
		"SP ������",

		"HP ����",
		"MP ����",
		"SP ����",

		"�����̻� ����",
		"�����̻� ȸ��",

		"��������",
		"�����Ÿ�",

		"�̵��ӵ�",

		"����",
		"����",
		"����Ȯ��",

		"���ݼӵ�",
		"��ų ������"

		"���� ������ �����",
		"���� ������ �����",
		"���� ������ �ݻ���",
		"���� ������ �ݻ���",
		"�̷ο� ����ȿ�� ����",
		"�طο� ����ȿ�� ����",
		"�ߵ��� ��ų"
	};

	std::string SPEC_ADDON_VAR1[EMSPECA_NSIZE] =
	{
		"����",		// ��ɾ���.
		"�Ÿ�",		// ����/�б�.

		"����",		// ������ݻ�.
		"ȸ����",	// ��Ȱ.

		"�����",	// HP ������.
		"�����",	// MP ������.
		"�����",	// SP ������.

		"����",		// HP ����.
		"����",		// MP ����.
		"����",		// SP ����.

		"����",		// �����̻� ����.
		"����",		// �����̻� ȸ��.

		"��������",	// ��������.
		"���ݰŸ�",	// �����Ÿ�.

		"�̼���",	// �̵��ӵ�.

		"�Ÿ�",		// ����
		"����",		// ����
		"����",		// ����Ȯ��

		"������",	// ������.
		"��ȭ��",	// ��ų ������ ��ȭ��
		
		"�����",	// ���� ������ �����
		"�����",	// ���� ������ �����
		"�ݻ���",	// ���� ������ �ݻ���
		"�ݻ���",	// ���� ������ �ݻ���
		"���밪",	// �̷ο� ����ȿ�� ����
		"���밪",	// �طο� ����ȿ�� ����
		"MID",		// �ߵ��� ��ų
	};

	float SPEC_ADDON_VAR1_SCALE[EMSPECA_NSIZE] =
	{
		1.0f,		// ��ɾ���.
		1.0f,		// ����/�б�.

		1.0f,		// ������ݻ�.
		100.0f,		// ��Ȱ.

		100.0f,		// HP ������.
		100.0f,		// MP ������.
		100.0f,		// SP ������.

		1.0f,		// HP ����.
		1.0f,		// MP ����.
		1.0f,		// SP ����.

		1.0f,		// �����̻� ����.
		1.0f,		// �����̻� ȸ��.

		1.0f,		// ��������.
		1.0f,		// �����Ÿ�.

		100.0f,		// �̵��ӵ�.

		1.0f,		// ����
		1.0f,		// ����
		1.0f,		// ����Ȯ��

		100.0f,		// ������.
		1.0f,		// ��ȭ��.
		
		100.0f,		// ���� ������ �����
		100.0f,		// ���� ������ �����
		100.0f,		// ���� ������ �ݻ���
		100.0f,		// ���� ������ �ݻ���
		1.0f,		// �̷ο� ����ȿ�� ����
		1.0f,		// �طο� ����ȿ�� ����
		1.0f,		// �ߵ��� ��ų
	};

	std::string SPEC_ADDON_VAR2[EMSPECA_NSIZE] =
	{
		"����",		// ��ɾ���.
		"Ȯ��",		// ����/�б�.

		"����",		// ������ݻ�.
		"����",		// ��Ȱ.

		"����",		// HP ������.
		"����",		// MP ������.
		"����",		// SP ������.

		"����",		// HP ����.
		"����",		// MP ����.
		"����",		// SP ����.

		"����",		// �����̻� ����.
		"����",		// �����̻� ȸ��.

		"����",		// ��������.
		"����",		// �����Ÿ�.

		"����",		// �̵��ӵ�.

		"����",		// ����
		"����",		// ����
		"����",		// ����Ȯ��

		"����",		// ������
		"����",		// ��ų ������.

		"����",		// ���� ������ �����
		"����",		// ���� ������ �����
		"Ȯ��",		// ���� ������ �ݻ���
		"Ȯ��",		// ���� ������ �ݻ���
		"����",		// �̷ο� ����ȿ�� ����
		"����",		// �طο� ����ȿ�� ����
		"SID"		// �ߵ��� ��ų
	};

	float SPEC_ADDON_VAR2_SCALE[EMSPECA_NSIZE] =
	{
		1.0f,		// ��ɾ���.
		100.0f,		// ����/�б�.

		1.0f,		// ������ݻ�.
		1.0f,		// ��Ȱ.

		1.0f,		// HP ������.
		1.0f,		// MP ������.
		1.0f,		// SP ������.

		1.0f,		// HP ����.
		1.0f,		// MP ����.
		1.0f,		// SP ����.

		1.0f,		// �����̻� ����.
		1.0f,		// �����̻� ȸ��.

		1.0f,		// ��������.
		1.0f,		// �����Ÿ�.

		1.0f,		// �̵��ӵ�.

		1.0f,		// ����
		1.0f,		// ����
		1.0f,		// ����Ȯ��

		1.0f,		// ���ݼӵ�
		1.0f,		// ��ų ������

		1.0f,		// ���� ������ �����
		1.0f,		// ���� ������ �����
		100.0f,		// ���� ������ �ݻ���
		100.0f,		// ���� ������ �ݻ���
		1.0f,		// �̷ο� ����ȿ�� ����
		1.0f,		// �طο� ����ȿ�� ����
		1.0f,		// �ߵ��� ��ų
	};

	std::string SKILLCLASS[EMSKILLCLASS_NSIZE] =
	{
		"������-class1",
		"������-class2",
		"������-class3",
		"������-class4",

		"�˵���-class1",
		"�˵���-class2",
		"�˵���-class3",
		"�˵���-class4",

		"��ú�-class1",
		"��ú�-class2",
		"��ú�-class3",
		"��ú�-class4",

		"�⿹��-class1",
		"�⿹��-class2",
		"�⿹��-class3",
		"�⿹��-class4",

		"NPC_01",
		"NPC_02",
		"NPC_03",
		"NPC_04",
		"NPC_05",
		"NPC_06",
		"NPC_07",
		"NPC_08",
		"NPC_09",
		"NPC_10",
		"NPC_11",
		"NPC_12",
		"NPC_13",
		"NPC_14",
		"�ذ���-class1",
		"�ذ���-class2",
		"�ذ���-class3",
		"�ذ���-class4",
		"NPC_19",
		"NPC_20"
	};

	std::string LANDEFFECT_TYPE[EMLANDEFFECT_SIZE] =
	{
		"���ݼӵ�",
		"�̵��ӵ�",
		"HP ��ȭ��",
		"MP ��ȭ��",
		"ȸ����",
		"����ġ ��ȭ��",
		"���ġ ��ȭ��",
		"���׼�ġ",
		"��� �������"
	};
};