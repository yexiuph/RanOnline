#pragma once

#include "GLContrlBaseMsg.h"

enum EMPMARKET_REGITEM_FB
{
	EMPMARKET_REGITEM_FB_FAIL		= 0,	//	일반 오류.
	EMPMARKET_REGITEM_FB_OK			= 1,	//	성공.
	
	EMPMARKET_REGITEM_FB_MAXNUM		= 2,	//	등록 한도 초과.
	EMPMARKET_REGITEM_FB_REGITEM	= 3,	//	이미 등록된 아이템.
	EMPMARKET_REGITEM_FB_NOSALE		= 4,	//	판매 금지.
};

enum EMPMARKET_OPEN_FB
{
	EMPMARKET_OPEN_FB_FAIL			= 0,	//	일반오류.
	EMPMARKET_OPEN_FB_OK			= 1,	//	성공.
	EMPMARKET_OPEN_FB_ALREADY		= 2,	//	이미 오픈되어 있음.
	EMPMARKET_OPEN_FB_EMPTY			= 3,	//	등록된 아이템이 없음.
	EMPMARKET_OPEN_FB_NOITEM		= 4,	//	허가권 없음.
};

enum EMPMARKET_BUY_FB
{
	EMPMARKET_BUY_FB_FAIL			= 0,	//	일반오류.
	EMPMARKET_BUY_FB_OK				= 1,	//	구입.
	EMPMARKET_BUY_FB_NUM			= 2,	//	개수 오류.
	EMPMARKET_BUY_FB_LOWMONEY		= 3,	//	돈 부족.
	EMPMARKET_BUY_FB_SOLD			= 4,	//	이미 판매종료된 물품.
	EMPMARKET_BUY_FB_NOINVEN		= 5,	//	인벤 공간 부족.
	EMPMARKET_BUY_FB_NOTIME			= 6		//	유효 기간 지남.
};

namespace GLMSG
{
	#pragma pack(1)

	struct SNETPC_PMARKET_TITLE
	{
		NET_MSG_GENERIC		nmg;
		char				szPMarketTitle[CHAT_MSG_SIZE+1];

		SNETPC_PMARKET_TITLE ()
		{
			nmg.dwSize = sizeof(SNETPC_PMARKET_TITLE);
			nmg.nType = NET_MSG_GCTRL_PMARKET_TITLE;
			memset(szPMarketTitle, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_TITLE_FB
	{
		NET_MSG_GENERIC		nmg;
		char				szPMarketTitle[CHAT_MSG_SIZE+1];

		SNETPC_PMARKET_TITLE_FB ()
		{
			nmg.dwSize = sizeof(SNETPC_PMARKET_TITLE_FB);
			nmg.nType = NET_MSG_GCTRL_PMARKET_TITLE_FB;
			memset(szPMarketTitle, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNETPC_PMARKET_REGITEM
	{
		NET_MSG_GENERIC		nmg;

		WORD				wPosX;
		WORD				wPosY;
		
		DWORD				dwMoney;
		DWORD				dwNum;

		SNETPC_PMARKET_REGITEM () 
			: wPosX(0)
			, wPosY(0)
			, dwMoney(0)
			, dwNum(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_REGITEM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_REGITEM_FB
	{
		NET_MSG_GENERIC		nmg;

		EMPMARKET_REGITEM_FB emFB;

		WORD				wPosX;
		WORD				wPosY;
		
		DWORD				dwMoney;
		DWORD				dwNum;

		SNATIVEID			sSALEPOS;

		SNETPC_PMARKET_REGITEM_FB () :
			emFB(EMPMARKET_REGITEM_FB_FAIL),
			wPosX(0),
			wPosY(0),

			dwMoney(0),
			dwNum(0),

			sSALEPOS(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_REGITEM_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_DISITEM
	{
		NET_MSG_GENERIC		nmg;

		SNATIVEID			sSALEPOS;

		SNETPC_PMARKET_DISITEM () :
			sSALEPOS(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_DISITEM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_DISITEM_FB
	{
		NET_MSG_GENERIC		nmg;

		SNATIVEID			sSALEPOS;

		SNETPC_PMARKET_DISITEM_FB () :
			sSALEPOS(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_DISITEM_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_OPEN
	{
		NET_MSG_GENERIC		nmg;
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_PMARKET_OPEN () :
			wPosX(0),
			wPosY(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_OPEN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_OPEN_FB
	{
		NET_MSG_GENERIC		nmg;

		EMPMARKET_OPEN_FB	emFB;

		SNETPC_PMARKET_OPEN_FB () :
			emFB(EMPMARKET_OPEN_FB_FAIL)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_OPEN_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_OPEN_BRD : public SNETPC_BROAD
	{
		char				szPMarketTitle[CHAT_MSG_SIZE+1];

		SNETPC_PMARKET_OPEN_BRD ()
		{
			nmg.dwSize = sizeof(SNETPC_PMARKET_OPEN_BRD);
			nmg.nType = NET_MSG_GCTRL_PMARKET_OPEN_BRD;
			memset(szPMarketTitle, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_CLOSE
	{
		NET_MSG_GENERIC		nmg;

		SNETPC_PMARKET_CLOSE ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_CLOSE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_CLOSE_BRD : public SNETPC_BROAD
	{

		SNETPC_PMARKET_CLOSE_BRD ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_CLOSE_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_BUY
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwGaeaID;
		SNATIVEID			sSALEPOS;
		DWORD				dwNum;

		SNETPC_PMARKET_BUY ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_BUY;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_BUY_FB
	{
		NET_MSG_GENERIC		nmg;

		EMPMARKET_BUY_FB	emFB;

		DWORD				dwGaeaID;
		SNATIVEID			sSALEPOS;
		DWORD				dwNum;

		SNETPC_PMARKET_BUY_FB () :
			emFB(EMPMARKET_BUY_FB_FAIL),
			dwGaeaID(0),
			sSALEPOS(false),
			dwNum(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_BUY_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_ITEM_INFO
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;

		SNETPC_PMARKET_ITEM_INFO () :
			dwGaeaID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_ITEM_INFO;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_ITEM_INFO_BRD : public SNETPC_BROAD
	{
		SNATIVEID	sSALEPOS;
		SNATIVEID	sINVENPOS;
		SITEMCUSTOM	sITEMCUSTOM;
		LONGLONG	llPRICE;
		DWORD		dwNUMBER;
		bool		bSOLD;

		SNETPC_PMARKET_ITEM_INFO_BRD () :
			llPRICE(0),
			dwNUMBER(0),
			bSOLD(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_ITEM_INFO_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_ITEM_UPDATE_BRD : public SNETPC_BROAD
	{
		SNATIVEID	sSALEPOS;
		DWORD		dwNUMBER;
		bool		bSOLD;

		SNETPC_PMARKET_ITEM_UPDATE_BRD () :
			dwNUMBER(0),
			bSOLD(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_SEARCH_ITEM
	{
		NET_MSG_GENERIC		nmg;
		SSEARCHITEMDATA		sSearchData;

		SNETPC_PMARKET_SEARCH_ITEM ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_SEARCH_ITEM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_SEARCH_ITEM_RESULT
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwSearchNum;
		SSEARCHITEMESULT	sSearchResult[MAX_SEARCH_RESULT];
		DWORD				dwPageNum;

		SNETPC_PMARKET_SEARCH_ITEM_RESULT () :
				dwSearchNum(0),
				dwPageNum(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwPageNum;

		SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ () :
			dwPageNum(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};


	// Revert to default structure packing
	#pragma pack()
};