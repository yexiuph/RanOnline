#pragma once

#include "GLContrlBaseMsg.h"

enum EMQUEST_START_FB
{
	EMQUEST_START_FB_OK			= 0,	//	성공.
	EMQUEST_START_FB_FAIL		= 1,	//	실패.
	EMQUEST_START_FB_ALREADY	= 2,	//	이미 수행중.
	EMQUEST_START_FB_DONAGAIN	= 3,	//	재시도 불가능.
	EMQUEST_START_FB_DONREPEAT	= 4,	//	재시도 불가능.
	EMQUEST_START_FB_CONDITION	= 5,	//	조건에 부적합.
	EMQUEST_START_FB_NEEDMONEY	= 6,	//	금액이 부족.
};

namespace GLMSG
{
	#pragma pack(1)

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_REGEN_GATE
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwNpcID;

		SNETPC_REQ_REGEN_GATE () 
			: dwNpcID(0)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_REGEN_GATE);
			nmg.nType = NET_MSG_GCTRL_REGEN_GATE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_REGEN_GATE_FB
	{
		NET_MSG_GENERIC		nmg;
		EMREGEN_GATE_FB		emFB;

		SNATIVEID			sMapID;
		DWORD				dwGateID;

		SNETPC_REQ_REGEN_GATE_FB () 
			: emFB(EMREGEN_GATE_FAIL)
			, sMapID(0,0)
			, dwGateID(0)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_REGEN_GATE_FB);
			nmg.nType = NET_MSG_GCTRL_REGEN_GATE_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CURE
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwGlobalID;
		DWORD				dwNpcID;

		SNETPC_REQ_CURE () 
			: dwNpcID(0)
			, dwGlobalID(0)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_CURE);
			nmg.nType = NET_MSG_GCTRL_CURE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CURE_FB
	{
		NET_MSG_GENERIC		nmg;

		EMREGEN_CURE_FB		emFB;
		DWORD				dwCUREFLAG;
		GLPADATA			sHP;

		SNETPC_REQ_CURE_FB () 
			: emFB(EMREGEN_CURE_FAIL)
			, dwCUREFLAG(NULL)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_CURE_FB);
			nmg.nType = NET_MSG_GCTRL_CURE_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CHARRESET
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwNpcID;

		SNETPC_REQ_CHARRESET () 
			: dwNpcID(0)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_CHARRESET);
			nmg.nType = (EMNET_MSG)NET_MSG_GCTRL_CHARRESET;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_CHARRESET_FB
	{
		NET_MSG_GENERIC		nmg;

		EMREGEN_CHARRESET_FB	emFB;
		DWORD					dwSTATS_P;
		DWORD					dwSKILL_P;

		SNETPC_REQ_CHARRESET_FB () 
			: emFB(EMREGEN_CHARRESET_FAIL)
			, dwSTATS_P(0)
			, dwSKILL_P(0)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_CHARRESET_FB);
			nmg.nType = (EMNET_MSG)NET_MSG_GCTRL_CHARRESET_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_NPC_ITEM_TRADE
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwNpcID;
		DWORD				dwA_NID[5];
		DWORD				dwB_NID;
		DWORD				dwGlobalID;

		SNETPC_REQ_NPC_ITEM_TRADE () 
			: dwNpcID(0)
			, dwB_NID(0)
			, dwGlobalID(0)
		{
			dwA_NID[0] = FALSE;
			dwA_NID[1] = FALSE;
			dwA_NID[2] = FALSE;
			dwA_NID[3] = FALSE;
			dwA_NID[4] = FALSE;
			nmg.dwSize = sizeof(SNETPC_REQ_NPC_ITEM_TRADE);
			nmg.nType = (EMNET_MSG)NET_MSG_GCTRL_NPC_ITEM_TRADE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_NPC_ITEM_TRADE_FB
	{
		NET_MSG_GENERIC		nmg;

		EMNPC_ITEM_TRADE_FB	emFB;

		SNETPC_REQ_NPC_ITEM_TRADE_FB () 
			: emFB(EMNPC_ITEM_TRADE_FAIL)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_NPC_ITEM_TRADE_FB);
			nmg.nType = (EMNET_MSG)NET_MSG_GCTRL_NPC_ITEM_TRADE_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_QUEST_START
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwNpcID;
		DWORD				dwTalkID;
		DWORD				dwQuestID;

		SNETPC_REQ_QUEST_START () 
			: dwNpcID(0)
			, dwTalkID(0)
			, dwQuestID(0)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_QUEST_START);
			nmg.nType = (EMNET_MSG)NET_MSG_GCTRL_REQ_QUEST_START;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_QUEST_START_FB
	{
		NET_MSG_GENERIC		nmg;

		EMQUEST_START_FB	emFB;

		SNETPC_REQ_QUEST_START_FB () 
			: emFB(EMQUEST_START_FB_OK)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_QUEST_START_FB);
			nmg.nType = (EMNET_MSG)NET_MSG_GCTRL_REQ_QUEST_START_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	// Revert to default structure packing
	#pragma pack()
};
