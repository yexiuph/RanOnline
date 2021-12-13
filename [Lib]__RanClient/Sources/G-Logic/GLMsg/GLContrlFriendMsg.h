#pragma once

#include "GLContrlBaseMsg.h"

enum EMFRIENDADD
{
	EMFRIENDADD_FAIL	= 0,	//	일반 오류.
	EMFRIENDADD_OK		= 1,	//	추가됨.
	EMFRIENDADD_REFUSE	= 2,	//	거절.
	EMFRIENDADD_ABSENCE	= 3,	//	부재.
};

enum EMFRIENDDEL
{
	EMFRIENDDEL_FAIL	= 0,	//	일반 오류.
	EMFRIENDDEL_OK		= 1,	//	삭제됨.
};

enum EMFRIENDBLOCK
{
	EMFRIENDBLOCK_FAIL	= 0,	//	일반 오류.
	EMFRIENDBLOCK_ON	= 1,	//	블럭조치.
	EMFRIENDBLOCK_OFF	= 2,	//	블럭취소.
};

enum EMWINDOW_TYPE
{
	EMFRIEND_WINDOW	= 0,	//	친구창
	EMCLUB_WINDOW	= 1,	//	클럽창
};

namespace GLMSG
{
	#pragma pack(1)

	struct SNETPC_REQ_FRIENDLIST
	{
		NET_MSG_GENERIC		nmg;

		SNETPC_REQ_FRIENDLIST ()
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDLIST);
			nmg.nType = NET_MSG_REQ_FRIENDLIST;
		}
	};


	struct SNETPC_REQ_FRIENDADD
	{
		NET_MSG_GENERIC		nmg;
		char				szADD_CHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDADD ()
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDADD);
			nmg.nType = NET_MSG_REQ_FRIENDADD;
			memset(szADD_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDADD_LURE
	{
		NET_MSG_GENERIC		nmg;
		char				szREQ_CHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDADD_LURE ()
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDADD_LURE);
			nmg.nType = NET_MSG_REQ_FRIENDADD_LURE;
            memset(szREQ_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDADD_ANS
	{
		NET_MSG_GENERIC		nmg;

		bool				bOK;
		char				szREQ_CHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDADD_ANS () 
			: bOK(false)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDADD_ANS);
			nmg.nType = NET_MSG_REQ_FRIENDADD_ANS;
			memset(szREQ_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDADD_FB
	{
		NET_MSG_GENERIC		nmg;

		EMFRIENDADD			emFB;
		char				szADD_CHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDADD_FB () :
			emFB(EMFRIENDADD_FAIL)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDADD_FB);
			nmg.nType = NET_MSG_REQ_FRIENDADD_FB;
			memset(szADD_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDDEL
	{
		NET_MSG_GENERIC		nmg;

		char				szDEL_CHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDDEL ()
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDDEL);
			nmg.nType = NET_MSG_REQ_FRIENDDEL;
			memset(szDEL_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDDEL_FB
	{
		NET_MSG_GENERIC		nmg;

		EMFRIENDDEL			emFB;
		char				szDEL_CHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDDEL_FB () 
			: emFB(EMFRIENDDEL_FAIL)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDDEL_FB);
			nmg.nType = NET_MSG_REQ_FRIENDDEL_FB;
            memset(szDEL_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDBLOCK
	{
		NET_MSG_GENERIC		nmg;

		bool				bBLOCK;
		char				szCHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDBLOCK () 
			: bBLOCK(false)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDBLOCK);
			nmg.nType = NET_MSG_REQ_FRIENDBLOCK;
			memset(szCHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDBLOCK_FB
	{
		NET_MSG_GENERIC		nmg;

		EMFRIENDBLOCK		emFB;
		char				szCHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDBLOCK_FB () :
			emFB(EMFRIENDBLOCK_FAIL)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDBLOCK_FB);
			nmg.nType = NET_MSG_REQ_FRIENDBLOCK_FB;
			memset(szCHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDINFO
	{
		NET_MSG_GENERIC		nmg;

		SFRIEND				sFRIEND;

		SNETPC_REQ_FRIENDINFO ()
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDINFO);
			nmg.nType = NET_MSG_FRIENDINFO;
		}
	};

	struct SNETPC_REQ_FRIENDSTATE
	{
		NET_MSG_GENERIC		nmg;

		char				szCHAR[CHR_ID_LENGTH+1];
		int					nChaFlag;

		bool				bONLINE;
		int					nCHANNEL;

		SNETPC_REQ_FRIENDSTATE () 
			: nChaFlag(0)
			, bONLINE(false)
			, nCHANNEL(0)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDSTATE);
			nmg.nType = NET_MSG_FRIENDSTATE;
			memset(szCHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDPHONENUMBER_CLT
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwGaeaID;
		char				szName[CHAR_SZNAME];
		char				szNewPhoneNumber[SMS_RECEIVER];

		SNETPC_REQ_FRIENDPHONENUMBER_CLT () 
			: dwGaeaID(0)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDPHONENUMBER_CLT);
			nmg.nType = NET_MSG_GCTRL_FRIEND_PHONENUMBER_CLT;
			memset(szName, 0, CHAR_SZNAME);
			memset(szNewPhoneNumber, 0, SMS_RECEIVER);
		}
	};

	struct SNETPC_REQ_FRIENDRENAME_CLT
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwGaeaID;
		char				szOldName[CHAR_SZNAME];
		char				szNewName[CHAR_SZNAME];

		SNETPC_REQ_FRIENDRENAME_CLT () 
			: dwGaeaID(0)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDRENAME_CLT);
			nmg.nType = NET_MSG_GCTRL_FRIEND_RENAME_CLT;
			memset(szOldName, 0, CHAR_SZNAME);
			memset(szNewName, 0, CHAR_SZNAME);
		}
	};

	struct SNETPC_REQ_FRIENDRENAME_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwGaeaID;
		char				szOldName[CHAR_SZNAME];
		char				szNewName[CHAR_SZNAME];

		SNETPC_REQ_FRIENDRENAME_FLD () 
			: dwGaeaID(0)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIENDRENAME_FLD);
			nmg.nType = NET_MSG_GCTRL_FRIEND_RENAME_FLD;
			memset(szOldName, 0, CHAR_SZNAME);
			memset(szNewName, 0, CHAR_SZNAME);
		}
	};

	struct SNETPC_REQ_FRIEND_CLUB_OPEN
	{
		NET_MSG_GENERIC		nmg;
		EMWINDOW_TYPE		emTYPE;
		bool				bOpen;

		SNETPC_REQ_FRIEND_CLUB_OPEN ()
			: emTYPE(EMFRIEND_WINDOW)
			, bOpen(false)
		{
			nmg.dwSize = sizeof(SNETPC_REQ_FRIEND_CLUB_OPEN);
			nmg.nType = NET_MSG_REQ_FRIEND_CLUB_OPEN;
		}
	};

	// Revert to default structure packing
	#pragma pack()
};