#pragma once

#include "./GLContrlBaseMsg.h"

namespace GLMSG
{
	#pragma pack(1)

	struct SNET_PERIOD
	{
		NET_MSG_GENERIC		nmg;

		SPRERIODTIME		sPRERIODTIME;

		SNET_PERIOD ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_PERIOD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_FIELDINFO_RESET
	{
		NET_MSG_GENERIC		nmg;

		SNET_FIELDINFO_RESET ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_FIELDINFO_RESET;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_WEATHER
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwWeather;

		SNETPC_WEATHER () :
			dwWeather(NULL)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_WEATHER;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_WHIMSICALWEATHER
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwWhimsical;

		SNETPC_WHIMSICALWEATHER () :
			dwWhimsical(NULL)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_WHIMSICAL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_MAPWEATHER
	{
		enum { MAX_WEATHER = 32 };

		NET_MSG_GENERIC				nmg;

		SONEMAPWEATHER				MapWeather[MAX_WEATHER];
		DWORD						dwMapWeatherSize;


		SNETPC_MAPWEATHER ()  :
			dwMapWeatherSize(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_MAPWEATHER;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_MAPWHIMSICALWEATHER
	{
		NET_MSG_GENERIC				nmg;
		
		SONEMAPWEATHER				MapWeather;

		SNETPC_MAPWHIMSICALWEATHER () 
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_MAPWHIMSICAL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETSERVER_CTRL_WEATHER
	{
		NET_MSG_GENERIC		nmg;
		bool				bActive;
		DWORD				dwWeather;

		SNETSERVER_CTRL_WEATHER () :
			bActive(true),
			dwWeather(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_SERVER_CTRL_WEATHER;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETSERVER_CTRL_WEATHER2
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwWeather;
		WORD				map_mID;
		WORD				map_sID;
		DWORD				dwApplyTime;

		SNETSERVER_CTRL_WEATHER2 () :
			dwWeather(0),
			map_mID(0),
			map_sID(0),
			dwApplyTime(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_SERVER_CTRL_WEATHER2;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETSERVER_CTRL_TIME
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwTime;

		SNETSERVER_CTRL_TIME () :
			dwTime(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_SERVER_CTRL_TIME;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETSERVER_CTRL_MONTH
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwMonth;

		SNETSERVER_CTRL_MONTH () :
			dwMonth(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_SERVER_CTRL_MONTH;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETSERVER_CTRL_GENITEMHOLD
	{
		NET_MSG_GENERIC		nmg;
		bool				bHold;

		SNETSERVER_CTRL_GENITEMHOLD () :
			bHold(true)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_SERVER_CTRL_GENITEMHOLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_GM_MOVE2GATE
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGATE;

		SNET_GM_MOVE2GATE () :
			dwGATE(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOVE2GATE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOVE2GATE_FLD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGATE;
		DWORD				dwGaeaID;

		SNET_GM_MOVE2GATE_FLD () :
			dwGATE(0),
			dwGaeaID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOVE2GATE_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_GM_MOVE2GATE_FB
	{
		NET_MSG_GENERIC		nmg;
		D3DXVECTOR3			vPOS;

		SNET_GM_MOVE2GATE_FB () :
			vPOS(0,0,0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOVE2GATE_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};
	
	//---------------------------------------------------------------------------NET
	struct SNET_GM_MOVE2MAPPOS
	{
		NET_MSG_GENERIC		nmg;
		
		SNATIVEID			nidMAP;

		DWORD				dwPOSX;
		DWORD				dwPOSY;

		SNET_GM_MOVE2MAPPOS () :
			nidMAP(false),
			dwPOSX(0),
			dwPOSY(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOVE2MAPPOS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOVE2MAPPOS_FLD
	{
		NET_MSG_GENERIC		nmg;

		SNATIVEID			nidMAP;
		DWORD				dwGaeaID;
		DWORD				dwPOSX;
		DWORD				dwPOSY;

		SNET_GM_MOVE2MAPPOS_FLD () :
			nidMAP(false),
			dwGaeaID(0),
			dwPOSX(0),
			dwPOSY(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOVE2MAPPOS_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_BLOCK_DETECTED
	{
		enum { MAX_INFO = 512 };

		NET_MSG_GENERIC		nmg;

		DWORD				dwDETECTED;
		DWORD				dwCHARID;
		char				szINFO[MAX_INFO];

		SNET_BLOCK_DETECTED () :
			dwDETECTED(0),
			dwCHARID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_BLOCK_DETECTED;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			szINFO[0] = '\0';
		}
	};

	struct SNET_GM_SHOWMETHEMONEY
	{
		NET_MSG_GENERIC		nmg;

		LONGLONG			llMoney;

		SNET_GM_SHOWMETHEMONEY () :
			llMoney(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_SHOWMETHEMONEY;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_SHOWMETHEMONEY_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwGaeaID;
		LONGLONG			llMoney;

		SNET_GM_SHOWMETHEMONEY_FLD () :
			dwGaeaID(0),
			llMoney(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_SHOWMETHEMONEY_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_KICK_USER
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwID;

		SNET_GM_KICK_USER () :
			dwID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_KICK_USER;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_KICK_USER_PROC
	{
		NET_MSG_GENERIC		nmg;

		SNET_GM_KICK_USER_PROC ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_KICK_USER_PROC;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_KICK_USER_PROC_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwID;

		SNET_GM_KICK_USER_PROC_FLD () :
			dwID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_KICK_USER_PROC_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_GM_EVENT_EXP
	{
		NET_MSG_GENERIC		nmg;

		float				fSCALE;

		SNET_GM_EVENT_EXP () :
			fSCALE(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_EVENT_EXP;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_BEGIN
	{
		NET_MSG_GENERIC		nmg;

		int			start_Lv;
		int			end_Lv;
		int			play_Time;
		int			buster_Time;
		float		expGain_Rate;
		float		itemGain_Rate;
		DWORD		dwEventMinute;


		SNET_GM_LIMIT_EVENT_BEGIN () :
								start_Lv(0),
								end_Lv(0),
								play_Time(0),
								buster_Time(0),
								expGain_Rate(0.0f),
								itemGain_Rate(0.0f),
								dwEventMinute(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_LIMIT_EVENT_BEGIN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_END
	{
		NET_MSG_GENERIC		nmg;

		SNET_GM_LIMIT_EVENT_END () 
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_LIMIT_EVENT_END;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_CLASS_EVENT
	{
		NET_MSG_GENERIC		nmg;
		BYTE				dwEventType; // 0 : EXP 1 : ITEM 2 : MONEY
		DWORD				dwClassIndex;
		float				fEventRate;
		bool				bStart;
		int					start_Lv;
		int					end_Lv;

		SNET_GM_CLASS_EVENT () :
				dwEventType(0),
				dwClassIndex(GLCC_NONE),
				fEventRate(0.0f),
				bStart(TRUE),
				start_Lv(0),
				end_Lv(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_CLASS_EVENT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct  SNET_GM_LIMIT_EVENT_RESTART
	{
		NET_MSG_GENERIC nmg;
		__time64_t restartTime;

		SNET_GM_LIMIT_EVENT_RESTART() :
			restartTime(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_LIMIT_EVENT_RESTART;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_BEGIN_FB
	{
		NET_MSG_GENERIC		nmg;

		int start_Lv;
		int end_Lv;
		int play_Time;
		int buster_Time;
		float expGain_Rate;
		float itemGain_Rate;


		SNET_GM_LIMIT_EVENT_BEGIN_FB () :
			start_Lv(0),
			end_Lv(0),
			play_Time(0),
			buster_Time(0),
			expGain_Rate(0.0f),
			itemGain_Rate(0.0f)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_LIMIT_EVENT_BEGIN_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_END_FB
	{
		NET_MSG_GENERIC		nmg;

		SNET_GM_LIMIT_EVENT_END_FB ()		
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_LIMIT_EVENT_END_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct  SNET_GM_LIMIT_EVENT_APPLY_START
	{
		NET_MSG_GENERIC nmg;
		__time64_t loginTime;   // 로그인 시간이나 이벤트 시작시 시간

		SNET_GM_LIMIT_EVENT_APPLY_START() :
									loginTime( 0 )
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_LIMIT_EVENT_APPLY_START;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct  SNET_GM_LIMIT_EVENT_APPLY_END
	{
		NET_MSG_GENERIC nmg;
		__time64_t loginTime;   // 로그인 시간이나 이벤트 시작시 시간

		SNET_GM_LIMIT_EVENT_APPLY_END() :
									loginTime( 0 )
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_LIMIT_EVENT_APPLY_END;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_TIME_REQ
	{
		NET_MSG_GENERIC nmg;
		DWORD			dwGaeaID;

		SNET_GM_LIMIT_EVENT_TIME_REQ() :
										dwGaeaID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_LIMIT_EVENT_TIME_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_TIME_RESET
	{
		NET_MSG_GENERIC nmg;
		__time64_t		loginTime;
		DWORD			dwGaeaID;


		SNET_GM_LIMIT_EVENT_TIME_RESET() :
			dwGaeaID(0),
			loginTime(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_LIMIT_EVENT_TIME_RESET;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_TIME_REQ_FB
	{
		NET_MSG_GENERIC nmg;

		bool bEventStart;       // 이벤트 시작 여부나 적용 대상이 아니면 FALSE 리턴
		__time64_t loginTime;   // 로그인 시간이나 이벤트 시작시 시간
		int play_Time;			// 이벤트 적용 시작 시간
		int buster_Time;		// 이벤트 적용 시간

		int start_Lv;
		int end_Lv;


		SNET_GM_LIMIT_EVENT_TIME_REQ_FB() :
										bEventStart(FALSE),
										loginTime(0),
										play_Time(0),
										buster_Time(0),
										start_Lv(0),
										end_Lv(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_LIMIT_EVENT_TIME_REQ_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_EXP_FB
	{
		NET_MSG_GENERIC		nmg;

		WORD				wFIELDSVR;
		float				fSCALE;

		SNET_GM_EVENT_EXP_FB () :
			wFIELDSVR(0),
			fSCALE(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_EVENT_EXP_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_EXP_END
	{
		NET_MSG_GENERIC		nmg;

		SNET_GM_EVENT_EXP_END ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_EVENT_EXP_END;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_EXP_END_FB
	{
		NET_MSG_GENERIC		nmg;
		WORD				wFIELDSVR;

		SNET_GM_EVENT_EXP_END_FB () :
			wFIELDSVR(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_EVENT_EXP_END_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_EX
	{
		NET_MSG_GENERIC		nmg;
		EMGM_EVENT_TYPE		emType;
		WORD				wValue;

		SNET_GM_EVENT_EX () 
			: emType(EMGM_EVENT_NONE)
			, wValue(0)
		{
			nmg.dwSize = sizeof(SNET_GM_EVENT_EX);
			nmg.nType = NET_MSG_GM_EVENT_EX;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_EX_END
	{
		NET_MSG_GENERIC		nmg;
		EMGM_EVENT_TYPE		emType;

		SNET_GM_EVENT_EX_END ()
			: emType(EMGM_EVENT_NONE)
		{
			nmg.dwSize = sizeof(SNET_GM_EVENT_EX_END);
			nmg.nType = NET_MSG_GM_EVENT_EX_END;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_ITEM_GEN
	{
		NET_MSG_GENERIC		nmg;

		float				fRATE;

		SNET_GM_EVEN_ITEM_GEN () :
			fRATE(1)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_EVENT_ITEM_GEN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_ITEM_GEN_FB
	{
		NET_MSG_GENERIC		nmg;
		float				fRATE;

		SNET_GM_EVEN_ITEM_GEN_FB () :
			fRATE(1)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_EVENT_ITEM_GEN_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_ITEM_GEN_END
	{
		NET_MSG_GENERIC		nmg;

		SNET_GM_EVEN_ITEM_GEN_END ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_EVENT_ITEM_GEN_END;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_ITEM_GEN_END_FB
	{
		NET_MSG_GENERIC		nmg;

		SNET_GM_EVEN_ITEM_GEN_END_FB ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_EVENT_ITEM_GEN_END_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_MONEY_GEN
	{
		NET_MSG_GENERIC		nmg;

		float				fRATE;

		SNET_GM_EVEN_MONEY_GEN () :
			fRATE(1)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_EVENT_MONEY_GEN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_MONEY_GEN_END
	{
		NET_MSG_GENERIC		nmg;

		SNET_GM_EVEN_MONEY_GEN_END ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_EVENT_MONEY_GEN_END;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SERVER_GENERALCHAT
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwLENGTH;
		
		enum { EMMAX_TEXT = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)-sizeof(DWORD)-1 };
		char				szTEXT[EMMAX_TEXT];

		SNET_SERVER_GENERALCHAT () :
			dwLENGTH(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_SERVER_GENERALCHAT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szTEXT, 0, sizeof(char) * EMMAX_TEXT);
		}

		void SETTEXT ( const char *szTemp )
		{
			if( !szTemp )	return;

			dwLENGTH = (DWORD) ( strlen(szTemp)+1 );
			if( dwLENGTH > EMMAX_TEXT )		return;

			StringCbCopy ( szTEXT, dwLENGTH, szTemp );

			nmg.dwSize = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(char)*dwLENGTH;
		}
	};

	struct SNET_CYBERCAFECLASS_UPDATE
	{
		NET_MSG_GENERIC nmg;
		DWORD			dwCyberCafeClass;

		SNET_CYBERCAFECLASS_UPDATE()
			: dwCyberCafeClass(0)
		{
			nmg.dwSize = sizeof(SNET_CYBERCAFECLASS_UPDATE);
			nmg.nType = NET_MSG_CYBERCAFECLASS_UPDATE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

	};


	struct SNET_SERVER_PLAYERKILLING_MODE
	{
		NET_MSG_GENERIC		nmg;
		BOOL				bMODE;

		SNET_SERVER_PLAYERKILLING_MODE () :
			bMODE(FALSE)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_SERVER_PLAYERKILLING_MODE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_GM_PRINT_CROWLIST
	{
		NET_MSG_GENERIC		nmg;
		BYTE				dwFindMob;

		SNET_GM_PRINT_CROWLIST () :
							dwFindMob( 0 )
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_PRINT_CROWLIST;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_PRINT_CROWLIST_FLD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;
		BYTE				dwFindMob;

		SNET_GM_PRINT_CROWLIST_FLD () :
			dwGaeaID(0),
			dwFindMob( 0 )
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_PRINT_CROWLIST_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_PRINT_CROWLIST_FB
	{
		NET_MSG_GENERIC		nmg;
		// crow의 MID, SID
		WORD				mID;
		WORD				sID;
		WORD				wNum;
		// MOB인지 NPC인지
		EMCROW				emCrow;

		SNET_GM_PRINT_CROWLIST_FB ()
			: mID(0)
			, sID(0)
			, wNum(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_PRINT_CROWLIST_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_NPC
	{
		NET_MSG_GENERIC		nmg;
		SNATIVEID			nidNPC;

		SNET_GM_WHERE_NPC () :
			nidNPC(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_WHERE_NPC;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_NPC_FLD
	{
		NET_MSG_GENERIC		nmg;
		SNATIVEID			nidNPC;
		DWORD				dwGaeaID;

		SNET_GM_WHERE_NPC_FLD () :
			nidNPC(false),
			dwGaeaID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_WHERE_NPC_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_NPC_FB
	{
		NET_MSG_GENERIC		nmg;
		int					nPosX;
		int					nPosY;

		SNET_GM_WHERE_NPC_FB () 
			: nPosX(0)
			, nPosY(0)
		{
			nmg.dwSize = sizeof(SNET_GM_WHERE_NPC_FB);
			nmg.nType = NET_MSG_GM_WHERE_NPC_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_PC_MAP
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwCHARID;

		SNET_GM_WHERE_PC_MAP () :
			dwCHARID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_WHERE_PC_MAP;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_PC_MAP_FB
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwChannel;
		SNATIVEID			nidMAP;

		SNET_GM_WHERE_PC_MAP_FB () :
			dwChannel(0),
			nidMAP(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_WHERE_PC_MAP_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_PC_POS
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwCHARID;

		SNET_GM_WHERE_PC_POS () :
			dwCHARID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_WHERE_PC_POS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_PC_POS_FB
	{
		NET_MSG_GENERIC		nmg;
		bool				bFOUND;
		int					nPosX;
		int					nPosY;

		SNET_GM_WHERE_PC_POS_FB () :
			bFOUND(false),
			nPosX(0),
			nPosY(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_WHERE_PC_POS_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_BIGHEAD
	{
		NET_MSG_GENERIC		nmg;
		bool				bBIGHEAD;

		SNET_GM_BIGHEAD () :
			bBIGHEAD(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_BIGHEAD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_BIGHEAD_BRD
	{
		NET_MSG_GENERIC		nmg;
		bool				bBIGHEAD;

		SNET_GM_BIGHEAD_BRD () :
			bBIGHEAD(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_BIGHEAD_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_BIGHAND
	{
		NET_MSG_GENERIC		nmg;
		bool				bBIGHAND;

		SNET_GM_BIGHAND () :
			bBIGHAND(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_BIGHAND;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_BIGHAND_BRD
	{
		NET_MSG_GENERIC		nmg;
		bool				bBIGHAND;

		SNET_GM_BIGHAND_BRD () :
			bBIGHAND(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_BIGHAND_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_FREEPK
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwPKTIME;

		SNET_GM_FREEPK () :
			dwPKTIME(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_FREEPK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_FREEPK_BRD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwPKTIME;
		bool				bSTATEUPDATE;

		SNET_GM_FREEPK_BRD () :
			dwPKTIME(0),
			bSTATEUPDATE(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_FREEPK_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_VIEWALLPLAYER
	{
		NET_MSG_GENERIC		nmg;

		SNET_GM_VIEWALLPLAYER () 
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType  = NET_MSG_GM_VIEWALLPLAYER;
		}
	};

	struct SNET_GM_SHOP_INFO_REQ
	{
		NET_MSG_GENERIC		nmg;
		bool				bBasicInfo;
		SNATIVEID			sSearchItemID;

		SNET_GM_SHOP_INFO_REQ () 
			: sSearchItemID(NATIVEID_NULL())
			, bBasicInfo(TRUE)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType  = NET_MSG_GM_SHOP_INFO_REQ;
		}
	};


	struct SNET_GM_SHOP_INFO_FB
	{
		enum { FIRST, MIDDLE, END };

		NET_MSG_GENERIC		nmg;
		char				szBasicInfo[MAX_CHAR_LENGTH];
		bool				bBasicInfo;
		BYTE				bSTATE;

		SNET_GM_SHOP_INFO_FB () 
			: bBasicInfo(TRUE)
			, bSTATE(MIDDLE)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType  = NET_MSG_GM_SHOP_INFO_FB;
			memset(szBasicInfo, 0, sizeof(char) * (MAX_CHAR_LENGTH));
		}
	};

	
	struct SNET_GM_VIEWWORKEVENT
	{
		NET_MSG_GENERIC		nmg;

		SNET_GM_VIEWWORKEVENT () 
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType  = NET_MSG_GM_VIEWWORKEVENT;
		}
	};

	struct SNET_GM_VIEWWORKEVENT_FB
	{
		NET_MSG_GENERIC		nmg;
		char				szWorkEventInfo[256];

		SNET_GM_VIEWWORKEVENT_FB () 
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType  = NET_MSG_GM_VIEWWORKEVENT_FB;
			memset(szWorkEventInfo, 0, sizeof(CHAR) * 256);
		}
	};

	struct SNET_GM_VIEWALLPLAYER_FLD_REQ
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwID;

		SNET_GM_VIEWALLPLAYER_FLD_REQ ()
			: dwID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType  = NET_MSG_GM_VIEWALLPLAYER_FLD_REQ;
		}
	};

	struct SNET_GM_VIEWALLPLAYER_FLD_FB
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwCHARID;
		char				szCHARNAME[CHAR_SZNAME];
		DWORD				dwPlayerNum;


		SNET_GM_VIEWALLPLAYER_FLD_FB ()
			: dwCHARID(0),
			  dwPlayerNum(0)
		{
			nmg.dwSize = sizeof(SNET_GM_VIEWALLPLAYER_FLD_FB);
			nmg.nType  = NET_MSG_GM_VIEWALLPLAYER_FLD_FB;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));			
		}
	};

	struct SNET_GM_WARNING_MSG
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;
		bool				bOn;

		SNET_GM_WARNING_MSG () :
			dwGaeaID(0),
			bOn(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_WARNING_MSG;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WARNING_MSG_FLD
	{
		NET_MSG_GENERIC		nmg;
		
		DWORD				dwGaeaID;
		bool				bOn;

		SNET_GM_WARNING_MSG_FLD () :
			dwGaeaID(0),
			bOn(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_WARNING_MSG_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WARNING_MSG_BRD
	{
		NET_MSG_GENERIC		nmg;
		
		bool				bOn;

		SNET_GM_WARNING_MSG_BRD () :
			bOn(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_WARNING_MSG_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_COUNTDOWN_MSG_BRD
	{
		NET_MSG_GENERIC		nmg;

		INT					nCount;

		SNET_GM_COUNTDOWN_MSG_BRD () :
			nCount(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_COUNTDOWN_MSG_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_GEN
	{
		NET_MSG_GENERIC		nmg;

		WORD				wMainID;
		WORD				wSubID;

		WORD				wPosX;
		WORD				wPosY;

		SNET_GM_MOB_GEN () :
			wMainID(0),
			wSubID(0),

			wPosX(0),
			wPosY(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOB_GEN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_GEN_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwGaeaID;
		WORD				wMainID;
		WORD				wSubID;

		WORD				wPosX;
		WORD				wPosY;

		SNET_GM_MOB_GEN_FLD () :
			dwGaeaID(0),
			wMainID(0),
			wSubID(0),

			wPosX(0),
			wPosY(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOB_GEN_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_GEN_EX
	{
		NET_MSG_GENERIC		nmg;

		WORD				wMobMID;
		WORD				wMobSID;
		WORD				wMapMID;
		WORD				wMapSID;
		WORD				wPosX;
		WORD				wPosY;
		WORD				wRange;
		WORD				wNum;
		BOOL				bThisChannel;

		SNET_GM_MOB_GEN_EX () :
			wMobMID(0),
			wMobSID(0),
			wMapMID(0),
			wMapSID(0),
			wPosX(0),
			wPosY(0),
			wRange(0),
			wNum(0),
			bThisChannel(FALSE)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOB_GEN_EX;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_GEN_EX_FLD
	{
		NET_MSG_GENERIC		nmg;

		WORD				wMobMID;
		WORD				wMobSID;
		WORD				wMapMID;
		WORD				wMapSID;
		WORD				wPosX;
		WORD				wPosY;
		WORD				wRange;
		WORD				wNum;

		DWORD				dwGaeaID;

		SNET_GM_MOB_GEN_EX_FLD () :
			wMobMID(0),
			wMobSID(0),
			wMapMID(0),
			wMapSID(0),
			wPosX(0),
			wPosY(0),
			wRange(0),
			wNum(0),
			dwGaeaID(-1)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOB_GEN_EX_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_DEL_EX
	{
		NET_MSG_GENERIC		nmg;

		WORD				wMobMID;
		WORD				wMobSID;
		WORD				wMapMID;
		WORD				wMapSID;
		BOOL				bThisChannel;

		SNET_GM_MOB_DEL_EX () :
			wMobMID(0),
			wMobSID(0),
			wMapMID(0),
			wMapSID(0),
			bThisChannel(FALSE)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOB_DEL_EX;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_DEL_EX_FLD
	{
		NET_MSG_GENERIC		nmg;

		WORD				wMobMID;
		WORD				wMobSID;
		WORD				wMapMID;
		WORD				wMapSID;

		SNET_GM_MOB_DEL_EX_FLD () :
			wMobMID(0),
			wMobSID(0),
			wMapMID(0),
			wMapSID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOB_DEL_EX_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_DEL
	{
		NET_MSG_GENERIC		nmg;
		bool				bMaterial;
		SNATIVEID			sMobID;

		SNET_GM_MOB_DEL () :
			sMobID( false ),
			bMaterial(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOB_DEL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_DEL_FLD
	{
		NET_MSG_GENERIC		nmg;
		bool				bMaterial;
		SNATIVEID			sMobID;

		DWORD				dwGaeaID;

		SNET_GM_MOB_DEL_FLD () :
			dwGaeaID(0),
			sMobID( false ),
			bMaterial(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOB_DEL_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_LEV
	{
		enum { FILE_NAME = 128, };

		NET_MSG_GENERIC		nmg;

		WORD				wMAP_MID;
		WORD				wMAP_SID;

		char				szLevelFile[FILE_NAME];

		SNET_GM_MOB_LEV () :
			wMAP_MID(USHRT_MAX),
			wMAP_SID(USHRT_MAX)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOB_LEVEL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_LEV_CLEAR
	{
		NET_MSG_GENERIC		nmg;

		WORD				wMAP_MID;
		WORD				wMAP_SID;

		SNET_GM_MOB_LEV_CLEAR () :
			wMAP_MID(USHRT_MAX),
			wMAP_SID(USHRT_MAX)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_MOB_LEVEL_CLEAR;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_CHAT_BLOCK_UACCOUNT
	{
		NET_MSG_GENERIC		nmg;

		char				szUACCOUNT[USR_ID_LENGTH+1];
		DWORD				dwBLOCK_MINUTE;

		SNET_GM_CHAT_BLOCK_UACCOUNT () 
			: dwBLOCK_MINUTE(0)
		{
			nmg.dwSize = sizeof(SNET_GM_CHAT_BLOCK_UACCOUNT);
			nmg.nType = NET_MSG_GM_CHAT_BLOCK_UACCOUNT;
			memset(szUACCOUNT, 0, sizeof(char) * (USR_ID_LENGTH+1));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_CHAT_BLOCK_CHARNAME
	{
		NET_MSG_GENERIC		nmg;

		char				szCHARNAME[CHAR_SZNAME];
		DWORD				dwBLOCK_MINUTE;

		SNET_GM_CHAT_BLOCK_CHARNAME () :
			dwBLOCK_MINUTE(0)
		{
			nmg.dwSize = sizeof(SNET_GM_CHAT_BLOCK_CHARNAME);
			nmg.nType = NET_MSG_GM_CHAT_BLOCK_CHARNAME;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAT_BLOCK_CHARID
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwCHARID;
		DWORD				dwBLOCK_MINUTE;

		SNET_GM_CHAT_BLOCK_CHARID () :
			dwCHARID(0),
			dwBLOCK_MINUTE(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_CHAT_BLOCK_CHARID;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_CHAT_BLOCK_FB
	{
		NET_MSG_GENERIC		nmg;

		bool				bBLOCK;
		char				szUACCOUNT[USR_ID_LENGTH+1];
		char				szCHARNAME[CHAR_SZNAME];
		DWORD				dwBLOCK_MINUTE;

		SNET_GM_CHAT_BLOCK_FB () 
			: bBLOCK(false)
			, dwBLOCK_MINUTE(0)
		{
			nmg.dwSize = sizeof(SNET_GM_CHAT_BLOCK_FB);
			nmg.nType = NET_MSG_GM_CHAT_BLOCK_FB;
			memset(szUACCOUNT, 0, sizeof(char) * (USR_ID_LENGTH+1));
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAT_BLOCK_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwCHARID;
		DWORD				dwBLOCK_MINUTE;

		SNET_GM_CHAT_BLOCK_FLD () :
			dwCHARID(0),
			dwBLOCK_MINUTE(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_CHAT_BLOCK_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_USER_CHAR_INFO_4NAME
	{
		NET_MSG_GENERIC		nmg;
		char				szCHARNAME[CHAR_SZNAME];

		SNET_USER_CHAR_INFO_4NAME ()
		{
			nmg.dwSize = sizeof(SNET_USER_CHAR_INFO_4NAME);
			nmg.nType = NET_MSG_USER_CHAR_INFO_4NAME;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAR_INFO_4NAME
	{
		NET_MSG_GENERIC		nmg;
		char				szCHARNAME[CHAR_SZNAME];

		SNET_GM_CHAR_INFO_4NAME ()
		{
			nmg.dwSize = sizeof(SNET_GM_CHAR_INFO_4NAME);
			nmg.nType = NET_MSG_GM_CHAR_INFO_4NAME;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAR_INFO_4CHARID
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwCHARID;

		SNET_GM_CHAR_INFO_4CHARID () :
			dwCHARID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_CHAR_INFO_4CHARID;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_USER_CHAR_INFO_FLD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwCLIENTID_FB;
		DWORD				dwCHARID;

		SNET_USER_CHAR_INFO_FLD () :
			dwCLIENTID_FB(0),
			dwCHARID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_USER_CHAR_INFO_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_CHAR_INFO_FLD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwCLIENTID_FB;
		DWORD				dwCHARID;

		SNET_GM_CHAR_INFO_FLD () :
			dwCLIENTID_FB(0),
			dwCHARID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GM_CHAR_INFO_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_USER_CHAR_INFO_AGT_FB
	{
		NET_MSG_GENERIC		nmg;

		char				szCHARNAME[CHAR_SZNAME];

		SNET_USER_CHAR_INFO_AGT_FB () 
		{
			nmg.dwSize = sizeof(SNET_USER_CHAR_INFO_AGT_FB);
			nmg.nType = NET_MSG_USER_CHAR_INFO_AGT_FB;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAR_INFO_AGT_FB
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwCHARID;
		char				szUACCOUNT[USR_ID_LENGTH+1];
		char				szCHARNAME[CHAR_SZNAME];
		DWORD				dwSERVER;
		DWORD				dwCHANNEL;
		DWORD				dwGAEAID;

		SNET_GM_CHAR_INFO_AGT_FB () 
			: dwCHARID(0)
			, dwSERVER(0)
			, dwCHANNEL(0)
			, dwGAEAID(0)
		{
			nmg.dwSize = sizeof(SNET_GM_CHAR_INFO_AGT_FB);
			nmg.nType = NET_MSG_GM_CHAR_INFO_AGT_FB;
			memset(szUACCOUNT, 0, sizeof(char) * (USR_ID_LENGTH+1));
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_USER_CHAR_INFO_FLD_FB
	{
		NET_MSG_GENERIC		nmg;

		//	fld->agt--clientid_fb-->clt
		//	필드에서 에이젼트로 송신후 에이젼트에서 답변할 클라이언트 아이디.
		DWORD				dwCLIENTID_FB;
		char				szCLUB[CHAR_SZNAME];

		SNET_USER_CHAR_INFO_FLD_FB () 
			: dwCLIENTID_FB(0)
		{
			nmg.dwSize = sizeof(SNET_USER_CHAR_INFO_FLD_FB);
			nmg.nType = NET_MSG_USER_CHAR_INFO_FLD_FB;
			memset(szCLUB, 0, sizeof(char) * (CHAR_SZNAME));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAR_INFO_FLD_FB
	{
		NET_MSG_GENERIC		nmg;
		
		//	fld->agt--clientid_fb-->clt
		//	필드에서 에이젼트로 송신후 에이젼트에서 답변할 클라이언트 아이디.
		DWORD				dwCLIENTID_FB;

		DWORD				dwCHARID;
		SNATIVEID			nidMAP;
		WORD				wPOSX;
		WORD				wPOSY;

		WORD				wLEVEL;
		GLPADATA			sHP;
		GLPADATA			sMP;
		GLPADATA			sSP;
		GLLLDATA			sEXP;
		char				szCLUB[CHAR_SZNAME];

		SNET_GM_CHAR_INFO_FLD_FB () 
			: dwCLIENTID_FB(0)
			
			, dwCHARID(0)
			, wPOSX(0)
			, wPOSY(0)
			, wLEVEL(0)
		{
			nmg.dwSize = sizeof(SNET_GM_CHAR_INFO_FLD_FB);
			nmg.nType = NET_MSG_GM_CHAR_INFO_FLD_FB;
			memset(szCLUB, 0, sizeof(char) * (CHAR_SZNAME));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_LEVEL_EVENT_END_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwMapID;
		DWORD				dwGateID;
		DWORD				dwHallMapID;
		GLLEV_POS			levPos[MAX_EVENT_POS];

		SNET_LEVEL_EVENT_END_FLD () 
			: dwMapID(0)
			, dwGateID(0)
			, dwHallMapID(0)
		{
			nmg.dwSize = sizeof(SNET_LEVEL_EVENT_END_FLD);
			nmg.nType = NET_MSG_GCTRL_LEVEL_EVENT_END_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_LEVEL_EVENT_WARNING_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwMapID;
		bool				bOn;

		SNET_LEVEL_EVENT_WARNING_FLD ()
			: dwMapID(0)
			, bOn(false)
		{
			nmg.dwSize = sizeof(SNET_LEVEL_EVENT_WARNING_FLD);
			nmg.nType = NET_MSG_GCTRL_LEVEL_EVENT_WARNING_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_LEVEL_EVENT_COUNTDOWN_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwMapID;
		INT					nCount;

		SNET_LEVEL_EVENT_COUNTDOWN_FLD ()
			: dwMapID(0)
			, nCount(0)
		{
			nmg.dwSize = sizeof(SNET_LEVEL_EVENT_COUNTDOWN_FLD);
			nmg.nType = NET_MSG_GCTRL_LEVEL_EVENT_COUNTDOWN_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_DELAYTIME
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwMillisec;
		bool				bAllServer;
		
		SNET_SET_SERVER_DELAYTIME ()
			: dwMillisec(0)
			, bAllServer(false)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_DELAYTIME);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_DELAYTIME;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_DELAYTIME_FB
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwMillisec;
		bool				bAllServer;
		
		SNET_SET_SERVER_DELAYTIME_FB ()
			: dwMillisec(0)
			, bAllServer(false)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_DELAYTIME_FB);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_DELAYTIME_AGT
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwMillisec;
		
		SNET_SET_SERVER_DELAYTIME_AGT ()
			: dwMillisec(0)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_DELAYTIME_AGT);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_DELAYTIME_AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_DELAYTIME_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwMillisec;
		
		SNET_SET_SERVER_DELAYTIME_FLD ()
			: dwMillisec(0)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_DELAYTIME_FLD);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_CRASHTIME
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwSec;
		bool				bAllServer;
		
		SNET_SET_SERVER_CRASHTIME ()
			: dwSec(0)
			, bAllServer(false)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_CRASHTIME);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_CRASHTIME;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_CRASHTIME_FB
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwSec;
		bool				bAllServer;
		
		SNET_SET_SERVER_CRASHTIME_FB ()
			: dwSec(0)
			, bAllServer(false)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_CRASHTIME_FB);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_CRASHTIME_AGT
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwSec;
		
		SNET_SET_SERVER_CRASHTIME_AGT ()
			: dwSec(0)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_CRASHTIME_AGT);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_CRASHTIME_AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_CRASHTIME_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwSec;
		
		SNET_SET_SERVER_CRASHTIME_FLD ()
			: dwSec(0)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_CRASHTIME_FLD);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_SKIPPACKET
	{
		NET_MSG_GENERIC		nmg;

		WORD				wSkipPacketNum;
		bool				bAllServer;
		
		SNET_SET_SERVER_SKIPPACKET ()
			: wSkipPacketNum(0)
			, bAllServer(false)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_SKIPPACKET);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_SKIPPACKET;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_SKIPPACKET_FB
	{
		NET_MSG_GENERIC		nmg;

		WORD				wSkipPacketNum;
		bool				bAllServer;
		
		SNET_SET_SERVER_SKIPPACKET_FB ()
			: wSkipPacketNum(0)
			, bAllServer(false)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_SKIPPACKET_FB);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_SKIPPACKET_AGT
	{
		NET_MSG_GENERIC		nmg;

		WORD				wSkipPacketNum;
		
		SNET_SET_SERVER_SKIPPACKET_AGT ()
			: wSkipPacketNum(0)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_SKIPPACKET_AGT);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SET_SERVER_SKIPPACKET_FLD
	{
		NET_MSG_GENERIC		nmg;

		WORD				wSkipPacketNum;
		
		SNET_SET_SERVER_SKIPPACKET_FLD ()
			: wSkipPacketNum(0)
		{
			nmg.dwSize = sizeof(SNET_SET_SERVER_SKIPPACKET_FLD);
			nmg.nType = NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SERVER_LOGINMAINTENANCE_AGT
	{
		NET_MSG_GENERIC		nmg;
		bool				bOn;
		SNET_SERVER_LOGINMAINTENANCE_AGT ()
			: bOn( false )
		{
			nmg.dwSize = sizeof( SNET_SERVER_LOGINMAINTENANCE_AGT );
			nmg.nType = NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	// Revert to default structure packing
	#pragma pack()
};