#pragma once

#include "GLContrlBaseMsg.h"

enum EMQUEST_COMPLETE_FB
{
	EMQUEST_COMPLETE_FB_FAIL	= 0,
	EMQUEST_COMPLETE_FB_OK		= 1,
	EMQUEST_COMPLETE_FB_NOINVEN	= 2,
};

namespace GLMSG
{
	#pragma pack(1)

	//---------------------------------------------------------------------------NET
	struct SNET_QUEST_PROG_GIVEUP
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwQUESTID;

		SNET_QUEST_PROG_GIVEUP () :
			dwQUESTID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_GIVEUP;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_QUEST_PROG_DEL
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwQUESTID;

		SNET_QUEST_PROG_DEL () :
			dwQUESTID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_DEL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_STREAM
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)-sizeof(DWORD), };

		NET_MSG_GENERIC		nmg;
		
		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNET_QUEST_PROG_STREAM () :
			dwLengthStream(0)
		{
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_STREAM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		bool SETSTREAM ( BYTE *pSTream, DWORD dwLength )
		{
			if ( EMSIZE_MAX <= dwLength )		return false;

			dwLengthStream = dwLength;
			memcpy ( arraySTREAM, pSTream, sizeof(BYTE)*dwLengthStream );

			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(BYTE)*dwLengthStream;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			return true;
		}
	};

	struct SNET_QUEST_END_DEL
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwQUESTID;

		SNET_QUEST_END_DEL () :
			dwQUESTID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_END_DEL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_END_STREAM
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)-sizeof(DWORD), };

		NET_MSG_GENERIC		nmg;
		
		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNET_QUEST_END_STREAM () :
			dwLengthStream(0)
		{
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD);
			nmg.nType = NET_MSG_GCTRL_QUEST_END_STREAM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		bool SETSTREAM ( BYTE *pSTream, DWORD dwLength )
		{
			if ( EMSIZE_MAX <= dwLength )		return false;

			dwLengthStream = dwLength;
			memcpy ( arraySTREAM, pSTream, sizeof(BYTE)*dwLengthStream );

			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(BYTE)*dwLengthStream;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			return true;
		}
	};

	struct SNET_QUEST_PROG_STEP_STREAM
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)
			-sizeof(DWORD)-sizeof(DWORD)-sizeof(DWORD)
			-sizeof(DWORD), };

		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		DWORD				dwQSTEP;
		DWORD				dwFlag;

		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNET_QUEST_PROG_STEP_STREAM () :
			dwQID(0),
			dwQSTEP(0),
			dwFlag(0),
			dwLengthStream(0)
		{
			nmg.dwSize = sizeof(NET_MSG_GENERIC)
				+sizeof(DWORD)+sizeof(DWORD)+sizeof(DWORD)
				+sizeof(DWORD);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		bool SETSTREAM ( BYTE *pSTream, DWORD dwLength )
		{
			if ( EMSIZE_MAX <= dwLength )		return false;

			dwLengthStream = dwLength;
			memcpy ( arraySTREAM, pSTream, sizeof(BYTE)*dwLengthStream );

			nmg.dwSize = sizeof(NET_MSG_GENERIC)
				+ sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD)
				+ sizeof(DWORD) + sizeof(BYTE)*dwLengthStream;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			return true;
		}
	};

	struct SNET_QUEST_PROG_INVEN
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)
			-sizeof(DWORD)
			-sizeof(DWORD), };

		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;

		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNET_QUEST_PROG_INVEN () :
			dwQID(0),
			dwLengthStream(0)
		{
			nmg.dwSize = sizeof(NET_MSG_GENERIC)
				+sizeof(DWORD)
				+sizeof(DWORD);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_INVEN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		bool SETSTREAM ( BYTE *pSTream, DWORD dwLength )
		{
			if ( EMSIZE_MAX <= dwLength )		return false;

			dwLengthStream = dwLength;
			memcpy ( arraySTREAM, pSTream, sizeof(BYTE)*dwLengthStream );

			nmg.dwSize = sizeof(NET_MSG_GENERIC)
				+ sizeof(DWORD)
				+ sizeof(DWORD) + sizeof(BYTE)*dwLengthStream;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			return true;
		}
	};

	struct SNET_QUEST_STEP_NPC_TALK
	{
		NET_MSG_GENERIC		nmg;
		
		DWORD				dwNpcID;
		DWORD				dwTalkID;

		DWORD				dwQUESTID;

		SNET_QUEST_STEP_NPC_TALK () :
			dwNpcID(0),
			dwTalkID(0),

			dwQUESTID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_NPCTALK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_STEP_NPC_TALK_FB
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		DWORD				dwQSTEP;

		SNET_QUEST_STEP_NPC_TALK_FB () :
			dwQID(0),
			dwQSTEP(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_MOBKILL
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bMOBKILL_ALL;
		DWORD				dwMOBKILL;

		SNET_QUEST_PROG_MOBKILL () :
			dwQID(0),
			dwQSTEP(0),
			bMOBKILL_ALL(0),
			dwMOBKILL(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_MOBKILL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};
	struct SNET_QUEST_PARTY_PROG_MOBKILL
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bMOBKILL_ALL;

		SNET_QUEST_PARTY_PROG_MOBKILL () :
		dwQID(0),
		dwQSTEP(0),
		bMOBKILL_ALL(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_QITEM
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bQITEM;

		SNET_QUEST_PROG_QITEM () :
			dwQID(0),
			dwQSTEP(0),

			bQITEM(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_QITEM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PARTY_PROG_QITEM
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bQITEM;

		SNET_QUEST_PARTY_PROG_QITEM () :
			dwQID(0),
			dwQSTEP(0),

			bQITEM(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_REACHZONE
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bREACHZONE;

		SNET_QUEST_PROG_REACHZONE () :
			dwQID(0),
			dwQSTEP(0),

			bREACHZONE(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_REACHZONE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_QUEST_PROG_INVEN_TURNNUM
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		WORD				wPosX;
		WORD				wPosY;

		WORD				wTurnNum;
		
		SNETPC_QUEST_PROG_INVEN_TURNNUM () :
			dwQID(0),
			wPosX(0),
			wPosY(0),

			wTurnNum(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_INVEN_PICKUP
	{
		NET_MSG_GENERIC		nmg;
		SNATIVEID			sNID_ITEM;
		
		SNETPC_QUEST_PROG_INVEN_PICKUP () :
			sNID_ITEM(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_QUEST_PROG_INVEN_INSERT
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		SINVENITEM			Data;

		SNETPC_QUEST_PROG_INVEN_INSERT () :
			dwQID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_QUEST_PROG_INVEN_DELETE
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_QUEST_PROG_INVEN_DELETE () :
			dwQID(0),

			wPosX(0),
			wPosY(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_READ
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;

		SNETPC_QUEST_PROG_READ () :
			dwQID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_READ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_TIME
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		float				fELAPS;

		SNETPC_QUEST_PROG_TIME () :
			dwQID(0),
			fELAPS(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_TIME;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_TIMEOVER
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;

		SNETPC_QUEST_PROG_TIMEOVER () :
			dwQID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_TIMEOVER;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_NONDIE
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;

		SNETPC_QUEST_PROG_NONDIE () :
			dwQID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_NONDIE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_LEAVEMAP
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;

		SNETPC_QUEST_PROG_LEAVEMAP () :
		dwQID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_LEVEL
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;

		SNETPC_QUEST_PROG_LEVEL () :
			dwQID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_PROG_LEVEL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_COMPLETE_FB
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;
		EMQUEST_COMPLETE_FB	emFB;

		SNETPC_QUEST_COMPLETE_FB () :
			dwQID(0),
			emFB(EMQUEST_COMPLETE_FB_FAIL)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_QUEST_COMPLETE_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_QUEST_COMPLETE
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwQID;

		SNETPC_REQ_QUEST_COMPLETE () :
			dwQID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_QUEST_COMPLETE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	// Revert to default structure packing
	#pragma pack()
};