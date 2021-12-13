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

	GASSERT(0&&"잘못된 저항아이템 클래스입니다.");
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

	GASSERT(0&&"잘못된 저항아이템 클래스입니다.");
	return EMGRINDING_RS_FIRE;
}

namespace COMMENT
{
	std::string ITEMLEVEL[LEVEL_NSIZE] =
	{
		"일반",
		"진귀한",
		"유일한",
		"무한",
		"백열",
	};

	std::string ITEMTYPE[ITEM_NSIZE] =
	{
		"착용,무기,도구",
		"화살",
		"약품",
		"스킬서",
		"귀환서",
		"인증서",
		"연마제",
		"부적",
		"승차권",
		"스킬포인트 리셋",
		"스텟포인트 리셋",
		"스킬, 스텟 리셋",
		"선물상자",
		"망각의 세제",
		"확성기",
		"폭죽",
		"캐릭터 추가 카드",
		"인벤 확장 카드",
		"창고 확장 카드",
		"창고 연결 카드",
		"프리미엄 세트",
		"개인상점 허가권",
		"랜덤 아이탬",
		"코스툼 분리",
		"헤어스타일 변경",
		"얼굴 변경",
		"? 아이템",
		"CD",
		"친구에게",
		"클럽호출",
		"헤어샾 이용권",
		"이름변경 카드",
		"헤어스타일",
		"헤어컬러",
		"귀혼주",
		"펫 카드",	// PetData
		"펫 먹이",
		"펫 이름변경",
		"펫 컬러변경",
		"펫 스타일변경"
		"아이템 획득카드"
	};

	std::string ITEMSUIT[SUIT_NSIZE] =
	{
		"모자류",
		"상의",
		"하의",
		"장갑류",
		"신발류",

		"무기류",

		"목걸이",
		"팔찌",

		"반지",

		"펫 A",	// PetData
		"펫 B"
		"S.R.C",
		"S.R.C Skin",
		"S.R.C Parts_A",
		"S.R.C Parts_B",
		"S.R.C Parts_C",

	};

	std::string ITEMDRUG[ITEM_DRUG_SIZE] =
	{
		"NUNE",
		"체력회복",
		"기력회복",
		"근력회복",
		"체력+기력 회복",
		"체력+활력 회복",
		"체력+기력+활력 회복",
		"이상치료",

		"학교귀환",
		"시작귀환",
		"직전귀환",

		"부활기능",
		"체력회복+이상치료",
		"체력+기력+활력회복+이상치료"
	};

	//std::string ITEMSLOT[SLOT_NSIZE_S] =
	//{
	//	"모자류",
	//	"상체",
	//	"하체",
	//	"손",
	//	"발",

	//	"오른손 도구",
	//	"오른손 도구 보조",
	//	"왼손 도구",
	//	"왼손 도구 보조",

	//	"목걸이",
	//	"손목",

	//	"오른손 손가락",
	//	"왼손 손가락"
	//};

	std::string ITEMATTACK[ITEMATT_NSIZE+1] =
	{
		"'비'착용",		//	0

		"검",			//	1
		"도",			//	2
		"검/도",		//	3
		"단검",			//	4
		"창",			//	5
		"몽둥이",		//	6
		"권",			//	7
		"활",			//	8
		"투척",			//	9
		"타입검사안함",	//	10
	};

	std::string ITEMADDON[EMADD_SIZE] =
	{
		"없음",
		"명중율",
		"회피율",
	
		"대미지",
		"방어력",

		"채력",
		"마나",
		"스테미나",

		"STATS 힘",
		"STATS 채력",
		"STATS 기력",
		"STATS 민첩",
		"STATS 지력",
		"STATS 근력",

		"격투치",
		"사격치",
		"기력치",
	};

	std::string ITEMVAR[EMVAR_SIZE] =
	{
		"없음",
		"HP 증가율",
		"MP 증가율",
		"SP 증가율",
		"HP+MP+SP 증가율"
	};

	std::string ITEMVOL[EMVAR_SIZE] = // by 경대
	{
		"없음",
		"HP 증가량",
		"MP 증가량",
		"SP 증가량",
		"HP+MP+SP 증가량"
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
		"없음",
		"이동속도",
		"이동속도",
		"Damage",
		"경험치",
		"없음",
		"발생율",
		"Damage",
		"MID",

		"이동속도",
		"이동속도",
		"Damage",

		"hp"
	};

	std::string ITEM_QUE_VAR2[QUESTION_SIZE] =
	{
		"없음",
		"없음",
		"공격속도",
		"없음",
		"없음",
		"없음",
		"없음",
		"없음",
		"SID",

		"없음",
		"공격속도",
		"없음",
		"없음"
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
		"없음",

		"공격력",
		"방어력",

		"저항(화)",
		"저항(빙)",
		"저항(전)",
		"저항(독)",
		"저항(정)",
	};

	std::string GRINDING_LEVEL[EMGRINDER_SIZE] =
	{
		"보통",
		"상위",
		"최상위",
	};

	std::string GRINDING_RESIST[EMGRINDING_RS_NSIZE] =
	{
		"화염",
		"얼음",
		"전기",
		"독",
		"기",
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
		"아이템타입",
		"아이템MID/SID"
	};
};


