#pragma once

#include "GLContrlBaseMsg.h"

namespace GLMSG
{
	#pragma pack(1)

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE
	{
		NET_MSG_GENERIC		nmg;
		
		DWORD				dwTargetID;

		SNET_TRADE () :
			dwTargetID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_FB
	{
		NET_MSG_GENERIC		nmg;
		
		DWORD				dwTargetID;
		EMTRADE_ANS			emAns;

		SNET_TRADE_FB () :
			dwTargetID(0),
			emAns(EMTRADE_OK)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_TAR
	{
		NET_MSG_GENERIC		nmg;
		
		DWORD				dwMasterID;

		SNET_TRADE_TAR () :
			dwMasterID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_TAR;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_TAR_ANS
	{
		NET_MSG_GENERIC		nmg;
		
		DWORD				dwMasterID;
		EMTRADE_ANS			emAns;

		SNET_TRADE_TAR_ANS () :
			dwMasterID(0),
			emAns(EMTRADE_OK)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_TAR_ANS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_AGREE
	{
		NET_MSG_GENERIC		nmg;
		
		SNET_TRADE_AGREE ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_AGREE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_AGREE_TAR
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;
		EMTRADE_AGREE_FB	emFB;
		
		SNET_TRADE_AGREE_TAR () :
			dwGaeaID(0),
			emFB(EMTRADE_AGREE_OK)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_AGREE_TAR;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_ITEM_REGIST
	{
		NET_MSG_GENERIC		nmg;

		WORD				wPosX;
		WORD				wPosY;

		WORD				wInvenX;
		WORD				wInvenY;

		SNET_TRADE_ITEM_REGIST () :
			wPosX(0),
			wPosY(0),
			wInvenX(0),
			wInvenY(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_ITEM_REGIST;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_ITEM_REGIST_TAR
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwGaeaID;
		SINVENITEM			sInsert;

		SNET_TRADE_ITEM_REGIST_TAR () :
			dwGaeaID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_ITEM_REMOVE
	{
		NET_MSG_GENERIC		nmg;

		WORD				wPosX;
		WORD				wPosY;

		SNET_TRADE_ITEM_REMOVE () :
			wPosX(0),
			wPosY(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_ITEM_REMOVE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_ITEM_REMOVE_TAR
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwGaeaID;
		WORD				wPosX;
		WORD				wPosY;

		SNET_TRADE_ITEM_REMOVE_TAR () :
			wPosX(0),
			wPosY(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_MONEY
	{
		NET_MSG_GENERIC		nmg;

		LONGLONG			lnMoney;

		SNET_TRADE_MONEY () :
			lnMoney(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_MONEY;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_MONEY_TAR
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwGaeaID;
		LONGLONG			lnMoney;

		SNET_TRADE_MONEY_TAR () :
			dwGaeaID(0),
			lnMoney(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_MONEY_TAR;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_COMPLETE_TAR
	{
		NET_MSG_GENERIC		nmg;

		SNET_TRADE_COMPLETE_TAR ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_COMPLETE_TAR;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_CANCEL
	{
		NET_MSG_GENERIC		nmg;

		SNET_TRADE_CANCEL ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_CANCEL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_TRADE_CANCEL_TAR
	{
		NET_MSG_GENERIC		nmg;
		EMTRADE_CANCEL_TAR	emCancel;

		SNET_TRADE_CANCEL_TAR () :
			emCancel(EMTRADE_CANCEL_NORMAL)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_TRADE_CANCEL_TAR;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	// Revert to default structure packing
	#pragma pack()
};