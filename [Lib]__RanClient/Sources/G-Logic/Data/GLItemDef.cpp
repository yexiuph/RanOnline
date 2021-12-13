#include "pch.h"
#include "./GLItemDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

EMGRINDING_RS_INDEX GrindRsClassToIndex ( const EMGRINDING_RS_FLAGS emClass )
{
	switch (emClass)
	{
	case EMGRINDING_RS_FIRE:	return EMGRINDING_RS_FIRE_INDEX;
	case EMGRINDING_RS_ICE:		return EMGRINDING_RS_ICE_INDEX;
	case EMGRINDING_RS_ELECTRIC:return EMGRINDING_RS_ELECTRIC_INDEX;
	case EMGRINDING_RS_POISON:	return EMGRINDING_RS_POISON_INDEX;
	case EMGRINDING_RS_SPIRIT:	return EMGRINDING_RS_SPIRIT_INDEX;
	};

	GASSERT(0&&"�߸��� ���׾����� Ŭ�����Դϴ�.");
	return EMGRINDING_RS_FIRE_INDEX;
}

EMGRINDING_RS_FLAGS GrindIndexToRsClass ( const EMGRINDING_RS_INDEX emIndex )
{
	switch (emIndex)
	{
	case EMGRINDING_RS_FIRE_INDEX:		return EMGRINDING_RS_FIRE;
	case EMGRINDING_RS_ICE_INDEX:		return EMGRINDING_RS_ICE;
	case EMGRINDING_RS_ELECTRIC_INDEX:	return EMGRINDING_RS_ELECTRIC;
	case EMGRINDING_RS_POISON_INDEX:	return EMGRINDING_RS_POISON;
	case EMGRINDING_RS_SPIRIT_INDEX:	return EMGRINDING_RS_SPIRIT;
	};

	GASSERT(0&&"�߸��� ���׾����� Ŭ�����Դϴ�.");
	return EMGRINDING_RS_FIRE;
}

namespace COMMENT
{
	std::string ITEMLEVEL[LEVEL_NSIZE] =
	{
		"�Ϲ�",
		"������",
		"������",
		"����",
		"�鿭",
	};

	std::string ITEMTYPE[ITEM_NSIZE] =
	{
		"����,����,����",
		"ȭ��",
		"��ǰ",
		"��ų��",
		"��ȯ��",
		"������",
		"������",
		"����",
		"������",
		"��ų����Ʈ ����",
		"��������Ʈ ����",
		"��ų, ���� ����",
		"��������",
		"������ ����",
		"Ȯ����",
		"����",
		"ĳ���� �߰� ī��",
		"�κ� Ȯ�� ī��",
		"â�� Ȯ�� ī��",
		"â�� ���� ī��",
		"�����̾� ��Ʈ",
		"���λ��� �㰡��",
		"���� ������",
		"�ڽ��� �и�",
		"��Ÿ�� ����",
		"�� ����",
		"? ������",
		"CD",
		"ģ������",
		"Ŭ��ȣ��",
		"���� �̿��",
		"�̸����� ī��",
		"��Ÿ��",
		"����÷�",
		"��ȥ��",
		"�� ī��",	// PetData
		"�� ����",
		"�� �̸�����",
		"�� �÷�����",
		"�� ��Ÿ�Ϻ���"
		"������ ȹ��ī��"
	};

	std::string ITEMSUIT[SUIT_NSIZE] =
	{
		"���ڷ�",
		"����",
		"����",
		"�尩��",
		"�Ź߷�",

		"�����",

		"�����",
		"����",

		"����",

		"�� A",	// PetData
		"�� B"
		"S.R.C",
		"S.R.C Skin",
		"S.R.C Parts_A",
		"S.R.C Parts_B",
		"S.R.C Parts_C",

	};

	std::string ITEMDRUG[ITEM_DRUG_SIZE] =
	{
		"NUNE",
		"ü��ȸ��",
		"���ȸ��",
		"�ٷ�ȸ��",
		"ü��+��� ȸ��",
		"ü��+Ȱ�� ȸ��",
		"ü��+���+Ȱ�� ȸ��",
		"�̻�ġ��",

		"�б���ȯ",
		"���۱�ȯ",
		"������ȯ",

		"��Ȱ���",
		"ü��ȸ��+�̻�ġ��",
		"ü��+���+Ȱ��ȸ��+�̻�ġ��"
	};

	//std::string ITEMSLOT[SLOT_NSIZE_S] =
	//{
	//	"���ڷ�",
	//	"��ü",
	//	"��ü",
	//	"��",
	//	"��",

	//	"������ ����",
	//	"������ ���� ����",
	//	"�޼� ����",
	//	"�޼� ���� ����",

	//	"�����",
	//	"�ո�",

	//	"������ �հ���",
	//	"�޼� �հ���"
	//};

	std::string ITEMATTACK[ITEMATT_NSIZE+1] =
	{
		"'��'����",		//	0

		"��",			//	1
		"��",			//	2
		"��/��",		//	3
		"�ܰ�",			//	4
		"â",			//	5
		"������",		//	6
		"��",			//	7
		"Ȱ",			//	8
		"��ô",			//	9
		"Ÿ�԰˻����",	//	10
	};

	std::string ITEMADDON[EMADD_SIZE] =
	{
		"����",
		"������",
		"ȸ����",
	
		"�����",
		"����",

		"ä��",
		"����",
		"���׹̳�",

		"STATS ��",
		"STATS ä��",
		"STATS ���",
		"STATS ��ø",
		"STATS ����",
		"STATS �ٷ�",

		"����ġ",
		"���ġ",
		"���ġ",
	};

	std::string ITEMVAR[EMVAR_SIZE] =
	{
		"����",
		"HP ������",
		"MP ������",
		"SP ������",
		"HP+MP+SP ������"
	};

	std::string ITEMVOL[EMVAR_SIZE] = // by ���
	{
		"����",
		"HP ������",
		"MP ������",
		"SP ������",
		"HP+MP+SP ������"
	};

	std::string ITEM_QUE_TYPE[QUESTION_SIZE] =
	{
		"none",
		"Speed Up",
		"Crazy",
		"Attack Up",
		"Exp Up",
		"Exp Get",
		"Lucky",
		"Bomb!!",
		"Mob Gen",

		"Speed Up Max",
		"Madness",
		"Attack Up Max",
		"Heal",
	};

	std::string ITEM_QUE_VAR1[QUESTION_SIZE] =
	{
		"����",
		"�̵��ӵ�",
		"�̵��ӵ�",
		"Damage",
		"����ġ",
		"����",
		"�߻���",
		"Damage",
		"MID",

		"�̵��ӵ�",
		"�̵��ӵ�",
		"Damage",

		"hp"
	};

	std::string ITEM_QUE_VAR2[QUESTION_SIZE] =
	{
		"����",
		"����",
		"���ݼӵ�",
		"����",
		"����",
		"����",
		"����",
		"����",
		"SID",

		"����",
		"���ݼӵ�",
		"����",
		"����"
	};

	float ITEMVAR_SCALE[EMVAR_SIZE] =
	{
		1.0f,
		100.0f,
		100.0f,
		100.0f,
		100.0f
	};

	bool IsITEMVAR_SCALE ( EMITEM_VAR emITEM_VAR )
	{
		return ITEMVAR_SCALE[emITEM_VAR]==100.0f;
	};

	std::string GRINDING_TYPE[EMGRINDING_NSIZE] =
	{
		"����",

		"���ݷ�",
		"����",

		"����(ȭ)",
		"����(��)",
		"����(��)",
		"����(��)",
		"����(��)",
	};

	std::string GRINDING_LEVEL[EMGRINDER_SIZE] =
	{
		"����",
		"����",
		"�ֻ���",
	};

	std::string GRINDING_RESIST[EMGRINDING_RS_NSIZE] =
	{
		"ȭ��",
		"����",
		"����",
		"��",
		"��",
	};

	std::string ITEM_RANDOM_OPT[EMR_OPT_SIZE] =
	{
		"NULL",
		"DAMAGE(%)",
		"DEFENSE(%)",

		"HITRATE(+%)",
		"AVOIDRATE(+%)",

		"HP",
		"MP",
		"SP",

		"HP_INC",
		"MP_INC",
		"SP_INC",
		"HMS_INC",

		"GRIND_DAMAGE",
		"GRIND_DEFENSE",

		"ATTACK_RANGE",
		"DIS_SP",
		"RESIST"
	};


	DWORD	ITEMCOLOR[LEVEL_NSIZE] = 
	{
		0xffff0000,
		0xff00ff00,
		0xff0000ff,
		0xffffff00,
		0xffff00ff
	};


	std::string COOLTYPE[EMCOOL_SIZE] =
	{
		"������Ÿ��",
		"������MID/SID"
	};
};


