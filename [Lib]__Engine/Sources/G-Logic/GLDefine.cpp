#include "pch.h"

#include "GLDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace COMMENT
{	
	std::string CROW[CROW_NUM] = 
	{
		"CROW_PC",
		"CROW_NPC",
		"CROW_MOB",
		"CROW_ITEM",
		"CROW_MONEY",
		"CROW_PET"	// PetData
		"CROW_MATERIAL"
	};

	std::string ATTACK_RGTYPE[EMATT_SIZE] =
	{
		"단거리",
		"장거리",
	};
};
