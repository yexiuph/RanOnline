#pragma once

#include "GLContrlBaseMsg.h"

#include "GLContrlServerMsg.h"
#include "GLContrlConflictMsg.h"
#include "GLContrlCrowMsg.h"
#include "GLContrlSummonMsg.h"
#include "GLContrlPartyMsg.h"
#include "GLContrlTradeMsg.h"
#include "GLContrlCharJoinMsg.h"
#include "GLContrlPcMsg.h"
#include "GLContrlInvenMsg.h"
#include "GLContrlSkillMsg.h"
#include "GLContrlNpcMsg.h"
#include "GLContrlQuestMsg.h"
#include "GLContrlFriendMsg.h"
#include "GLContrlPrivateMarket.h"
#include "GLContrlClubMsg.h"
#include "GLContrlPetMsg.h"
#include "GLContrlInDunMsg.h"

enum EMCHAT_LOUDSPEAKER
{
	EMCHAT_LOUDSPEAKER_FAIL		= 0,
	EMCHAT_LOUDSPEAKER_OK		= 1,
	EMCHAT_LOUDSPEAKER_NOITEM	= 2,
	EMCHAT_LOUDSPEAKER_BLOCK	= 3,
};

namespace GLMSG
{
	#pragma pack(1)

	struct SNETPC_CHAT_BLOCK
	{
		NET_MSG_GENERIC		nmg;
		CHAR				szName[CHR_ID_LENGTH+1];	// 특정 캐릭터명 or 사용자명
		__time64_t			tChatBlock;					// 채팅 차단 시간
		
		SNETPC_CHAT_BLOCK() 
			: tChatBlock(0)
		{
			nmg.dwSize = sizeof(SNETPC_CHAT_BLOCK);
			nmg.nType = NET_MSG_CHAT_BLOCK;
			memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
			GASSERT( nmg.dwSize <= NET_DATA_BUFSIZE );			
		}
	};

	struct SNETPC_CHAT_PRIVATE_FAIL
	{
		NET_MSG_GENERIC		nmg;
		CHAR				szName[CHR_ID_LENGTH+1];	// 특정 캐릭터명 or 사용자명

		SNETPC_CHAT_PRIVATE_FAIL ()
		{
			nmg.dwSize = sizeof(SNETPC_CHAT_PRIVATE_FAIL);
			nmg.nType = NET_MSG_CHAT_PRIVATE_FAIL;
			memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CHAT_LOUDSPEAKER
	{
		NET_MSG_GENERIC		nmg;
		WORD				wPosX;
		WORD				wPosY;

		char				szMSG[CHAT_MSG_SIZE+1];

		SNETPC_CHAT_LOUDSPEAKER () 
			: wPosX(0)
			, wPosY(0)
		{
			nmg.dwSize = sizeof(SNETPC_CHAT_LOUDSPEAKER);
			nmg.nType = NET_MSG_CHAT_LOUDSPEAKER;
			memset(szMSG, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CHAT_LOUDSPEAKER_AGT
	{
		NET_MSG_GENERIC		nmg;
		bool				bchannel_all;
		char				szMSG[CHAT_MSG_SIZE+1];

		SNETPC_CHAT_LOUDSPEAKER_AGT () 
			: bchannel_all(false)
		{
			nmg.dwSize = sizeof(SNETPC_CHAT_LOUDSPEAKER_AGT);
			nmg.nType = NET_MSG_CHAT_LOUDSPEAKER_AGT;
			memset(szMSG, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CHAT_LOUDSPEAKER_FB
	{
		NET_MSG_GENERIC		nmg;
		EMCHAT_LOUDSPEAKER	emFB;

		SNETPC_CHAT_LOUDSPEAKER_FB () 
			: emFB(EMCHAT_LOUDSPEAKER_FAIL)
		{
			nmg.dwSize = sizeof(SNETPC_CHAT_LOUDSPEAKER_FB);
			nmg.nType = NET_MSG_CHAT_LOUDSPEAKER_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETDROP_ITEM
	{
		NET_MSG_GENERIC		nmg;

		SDROP_CLIENT_ITEM	Data;

		SNETDROP_ITEM ()
		{
			nmg.dwSize = sizeof(SNETDROP_ITEM);
			nmg.nType = NET_MSG_GCTRL_DROP_ITEM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETDROP_MONEY
	{
		NET_MSG_GENERIC		nmg;

		float				fAge;
		DWORD				dwGlobID;
		D3DXVECTOR3			vPos;
		LONGLONG			lnAmount;


		SNETDROP_MONEY () 
			: fAge(0)
			, dwGlobID(0)
			, vPos(0,0,0)
			, lnAmount(0)
		{
			nmg.dwSize = sizeof(SNETDROP_MONEY);
			nmg.nType = NET_MSG_GCTRL_DROP_MONEY;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETDROP_CROW
	{
		NET_MSG_GENERIC		nmg;

		SDROP_CROW			Data;

		SNETDROP_CROW ()
		{
			nmg.dwSize = sizeof(SNETDROP_CROW);
			nmg.nType = NET_MSG_GCTRL_DROP_CROW;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETDROP_PC
	{
		NET_MSG_GENERIC		nmg;

		SDROP_CHAR			Data;

		SNETDROP_PC ()
		{
			nmg.dwSize = sizeof(SNETDROP_PC);
			nmg.nType = NET_MSG_GCTRL_DROP_PC;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

//---------------------------------------------------------------------------NET
	struct SNETDROP_MATERIAL
	{
		NET_MSG_GENERIC		nmg;

		SDROP_MATERIAL		Data;

		SNETDROP_MATERIAL ()
		{
			nmg.dwSize = sizeof(SNETDROP_MATERIAL);
			nmg.nType = NET_MSG_GCTRL_DROP_MATERIAL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETDROP_OUT
	{
		enum { MAX_NUM = 200, };

		NET_MSG_GENERIC		nmg;
		BYTE				cNUM;
		BYTE				cCUR;
		STARID				sOUTID[MAX_NUM];

		SNETDROP_OUT () 
			: cNUM(0)
			, cCUR(0)
		{
			nmg.dwSize = sizeof(nmg) + sizeof(cNUM) + sizeof(cCUR);
			nmg.nType = NET_MSG_GCTRL_DROP_OUT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		void RESET_CROW ()
		{
			cNUM = 0;
			cCUR = 0;
		}

		BYTE GETAMOUNT ()
		{
			return cNUM;
		}

		bool ADD_CROW ( const STARID &cOUT )
		{
			if ( cNUM >= MAX_NUM )
			{
				return false;
			}
			else
			{
				sOUTID[cNUM++] = cOUT;
				nmg.dwSize = sizeof(nmg) + sizeof(cNUM) + sizeof(cCUR) + sizeof(STARID)*cNUM;
				return true;
			}
		}

		void RESET_CUR ()
		{
			cCUR = 0;
		}

		bool POP_CROW ( STARID &cOUT )
		{
			if ( cNUM == cCUR )
			{
				return false;
			}
			else
			{
				cOUT = sOUTID[cCUR++];
				return true;
			}
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_DROP_OUT_FORCED
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwCharID;
		EMDROP_OUT_FORCED	emForced;

		SNET_DROP_OUT_FORCED () 
			: dwCharID(0)
			, emForced(EMDROPOUT_REQLOGIN)
		{
			nmg.dwSize = sizeof(SNET_DROP_OUT_FORCED);
			nmg.nType = NET_MSG_GCTRL_DROP_OUT_FORCED;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_REQ_GENITEM_AGT
	{
		NET_MSG_GENERIC		nmg;

		int					nCHANNEL;
		SNATIVEID			sNID;

		SNATIVEID			sMAPID;
		EMITEMGEN			emGENTYPE;
		EMGROUP				emHoldGroup;
		DWORD				dwHoldGID;
		D3DXVECTOR3			vPos;

		SNET_REQ_GENITEM_AGT () 
			: nCHANNEL(0)
			, sNID(false)
			
			, sMAPID(false)
			, emGENTYPE(EMGEN_DEFAULT)
			, emHoldGroup(EMGROUP_ONE)
			, dwHoldGID(GAEAID_NULL)
			, vPos(0,0,0)
		{
			nmg.dwSize = sizeof(SNET_REQ_GENITEM_AGT);
			nmg.nType = NET_MSG_GCTRL_REQ_GENITEM_AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_REQ_GENITEM_FLD_FB
	{
		NET_MSG_GENERIC		nmg;

		SNATIVEID			sNID;

		SNATIVEID			sMAPID;
		EMITEMGEN			emGENTYPE;
		LONGLONG			lnGENCOUNT;
		EMGROUP				emHoldGroup;
		DWORD				dwHoldGID;
		D3DXVECTOR3			vPos;

		SNET_REQ_GENITEM_FLD_FB () 
			: sNID(false)
			
			, sMAPID(false)
			, emGENTYPE(EMGEN_DEFAULT)
			, lnGENCOUNT(0)
			, emHoldGroup(EMGROUP_ONE)
			, dwHoldGID(GAEAID_NULL)
			, vPos(0,0,0)
		{
			nmg.dwSize = sizeof(SNET_REQ_GENITEM_FLD_FB);
			nmg.nType = NET_MSG_GCTRL_REQ_GENITEM_FLD_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_REQ_SERVERTEST
	{
		NET_MSG_GENERIC		nmg;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC) };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNET_REQ_SERVERTEST () 
		{
			nmg.dwSize = sizeof(SNET_REQ_SERVERTEST);
			nmg.nType = NET_MSG_GCTRL_SERVERTEST;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_REQ_SERVERTEST_FB
	{
		NET_MSG_GENERIC		nmg;

		SNET_REQ_SERVERTEST_FB () 
		{
			nmg.dwSize = sizeof(SNET_REQ_SERVERTEST_FB);
			nmg.nType = NET_MSG_GCTRL_SERVERTEST_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_MSG_SERVERTIME_BRD
	{
		NET_MSG_GENERIC		nmg;

		__time64_t			t64Time;

		SNET_MSG_SERVERTIME_BRD () 
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_SERVERTIME_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	// Revert to default structure packing
	#pragma pack()
};

