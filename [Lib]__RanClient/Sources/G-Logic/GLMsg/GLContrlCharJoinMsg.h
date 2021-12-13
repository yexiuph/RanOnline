#pragma once
#include <map>
#include <string>

#include "GLContrlBaseMsg.h"

enum EMREQ_RECALL_FB
{
	EMREQ_RECALL_FB_OK			= 0,	//	성공.
	EMREQ_RECALL_FB_FAIL		= 1,	//	실패.
	EMREQ_RECALL_FB_ITEM		= 2,	//	아이템 불충족.
	EMREQ_RECALL_FB_CONDITION	= 3,	//	여건이 안됨. ( 케릭 상태 이상, 대련, 거래 ).
	EMREQ_RECALL_FB_NOTLASTCALL	= 4,	//	직전귀환 위치가 저장되어 있지 않음.
	EMREQ_RECALL_FB_PK_LEVEL	= 5,	//	pk 성향이 낮아서 귀환 카드 사용 불가능.
	EMREQ_RECALL_FB_JOINCON		= 6,	// 진입조건 불충분 ( 레벨, 스킬, 퀘스트, 아이템, 성향, 생활점수 )
	EMREQ_RECALL_FB_IMMOVABLE	= 7,	// 진입불가 ( 이벤트 지역 )
};

enum EMREQ_TELEPORT_FB
{
	EMREQ_TELEPORT_FB_OK			= 0,	//	성공.
	EMREQ_TELEPORT_FB_FAIL			= 1,	//	실패.
	EMREQ_TELEPORT_FB_ITEM			= 2,	//	아이템 불충족.
	EMREQ_TELEPORT_FB_CONDITION		= 3,	//	여건이 안됨. ( 케릭 상태 이상, 대련, 거래 ).
	EMREQ_TELEPORT_FB_PK_LEVEL		= 4,	//	pk 성향이 낮아서 귀환 카드 사용 불가능.
	EMREQ_TELEPORT_FB_JOINCON		= 5,	//  진입조건 불충분 ( 레벨, 스킬, 퀘스트, 아이템, 성향, 생활점수 )
	EMREQ_TELEPORT_FB_IMMOVABLE		= 6,	//  진입불가 ( 이벤트 지역 )
};

enum EMREQ_MUST_LEAVE_MAP_FB
{
	EMREQ_MUST_LEAVE_MAP_FB_OK			= 0,	//	성공.
	EMREQ_MUST_LEAVE_MAP_FB_FAIL			= 1,	//	실패.
};

enum EMFRIEND_FLAGS
{
	EMFRIEND_BLOCK	= -1,	//	블럭 ( 귓말 차단 상태. )
	EMFRIEND_OFF	= 0,	//	친구가 아닌 상태.
	EMFRIEND_VALID	= 1,	//	서로 유효한 친구.
	EMFRIEND_BETRAY = 2,	//	상대가 나를 차단하거나 삭제한 상태.
};

enum EMSAVECHARPOS_FB
{
	EMSAVECHARPOS_FB_FAIL				= 0,	//	일반 오류.
	EMSAVECHARPOS_FB_OK					= 1,	//	성공.
};

struct SFRIEND
{
	int nCharID;
	int nCharFlag;
	TCHAR szCharName[CHR_ID_LENGTH+1];
	TCHAR szPhoneNumber[SMS_RECEIVER];

	bool bONLINE;
	int nCHANNEL;

	SFRIEND () 
		: nCharID(0)
		, nCharFlag(EMFRIEND_OFF)
		, bONLINE(false)
		, nCHANNEL(0)
	{
		memset(szCharName, 0, sizeof(szCharName));
		memset(szPhoneNumber, 0, sizeof(szPhoneNumber));
	}

	bool IsVALID()	{ return nCharFlag==EMFRIEND_VALID; }
	bool IsBLOCK()	{ return nCharFlag==EMFRIEND_BLOCK; }
	bool IsOFF()	{ return nCharFlag==EMFRIEND_OFF;	}
	bool IsBETRAY()	{ return nCharFlag==EMFRIEND_BETRAY;}

	SFRIEND& operator= ( const SFRIEND &cData );
};

struct GLCHARAG_DATA
{
	typedef std::map<std::string,SFRIEND>		MAPFRIEND;
	typedef MAPFRIEND::iterator					MAPFRIEND_ITER;
	typedef std::vector<USER_ATTEND_INFO>		VECATTEND;

	DWORD						m_dwUserID;					//	사용자 고유 ID.
	DWORD						m_dwUserLvl;				//	사용자 권한 Level.
	__time64_t					m_tPREMIUM;					//	프리미엄 기한.
	__time64_t					m_tCHATBLOCK;				//	귓말 차단.


	DWORD						m_dwServerID;				//	서버군 ID.
	DWORD						m_dwCharID;					//	케릭터 고유 ID.
	char						m_szName[CHAR_SZNAME];		//	케릭터 이름. User ID Name
	TCHAR						m_szPhoneNumber[SMS_RECEIVER];
	char						m_szUserName[USR_ID_LENGTH+1]; // User Account Name
	TCHAR						m_szIp[MAX_IP_LENGTH+1];	//  캐릭터 접속 IP

	int							m_dwThaiCCafeClass;			// 태국 사이버 카페
	int							m_nMyCCafeClass;			// 말레이시아 PC방 이벤트

	EMCHARCLASS					m_emClass;					//	종족.
	WORD						m_wSchool;					//	학원.

	DWORD						m_dwGuild;					//	길드(클럽).
	__time64_t					m_tSECEDE;					//	탈퇴시간.

	int							m_nBright;					//	속성.
	BOOL						m_bUseArmSub;				// 극강부를 위한 보조 무기 사용여부

	//	Note : 초기 시작될 맵, 맵 위치값.
	//
	SNATIVEID					m_sStartMapID;				//	초기 시작 맵.
	DWORD						m_dwStartGate;				//	초기 시작 개이트.
	D3DXVECTOR3					m_vStartPos;				//	초기 시작 위치.

	SChinaTime					m_sChinaTime;				//  중국 시간.
	SVietnamGainSystem			m_sVietnamSystem;			//  베트남 탐닉방지 시스템

	SEventTime					m_sEventTime;				//  이벤트 적용 시간.

	SNATIVEID					m_sSaveMapID;				//	종료 맵.
	D3DXVECTOR3					m_vSavePos;					//	종료 위치.

	MAPFRIEND					m_mapFriend;				//	친구 정보.
	VECATTEND					m_vecAttend;				//	출석 정보.

	bool						m_bTracingUser;				//  현재 추적중인 유저인지 아닌지

	GLCHARAG_DATA () :
		m_dwUserID(0),
		m_dwUserLvl(0),
		m_tPREMIUM(0),
		m_tCHATBLOCK(0),

		m_dwServerID(0),
		m_dwCharID(0),
		
		m_emClass(GLCC_FIGHTER_M),
		m_wSchool(0),
		
		m_dwGuild(CLUB_NULL),
		m_tSECEDE(0),

		m_nBright(0),
		m_bUseArmSub(FALSE),

		m_dwStartGate(0),
		m_vStartPos(0,0,0),

		m_sSaveMapID(NATIVEID_NULL()),
		m_vSavePos(0,0,0),

		m_dwThaiCCafeClass(0),
		m_nMyCCafeClass(0),

		m_bTracingUser(FALSE)
		
	{
		memset(m_szName, 0, sizeof(m_szName));
		memset(m_szPhoneNumber, 0, sizeof(m_szPhoneNumber));
		memset(m_szUserName, 0, sizeof(m_szUserName));
		memset(m_szIp, 0, sizeof(m_szIp));
	}

	GLCHARAG_DATA& operator= ( const GLCHARAG_DATA &cData );
};

struct SCHARDATA2;

namespace GLMSG
{
	#pragma pack(1)

	//	내부 메시지용. 실제로 socket 을 거치지 않는다.
	struct SNETJOIN2AGENTSERVER
	{
		NET_MSG_GENERIC		nmg;

		DWORD				m_dwClientID;
		GLCHARAG_DATA*		m_pCharAgData;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(GLCHARAG_DATA*), };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETJOIN2AGENTSERVER () :
			m_dwClientID(0),
			m_pCharAgData(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_AGENT_REQ_JOIN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//	내부 메시지용. 실제로 socket 을 거치지 않는다.
	struct SNETJOIN2FIELDSERVER
	{
		struct SINFO
		{
			EMGAME_JOINTYPE		m_emType;
			SNATIVEID			m_sStartMap;
			DWORD				m_dwStartGate;
			D3DXVECTOR3			m_vStartPos;
			DWORD				m_dwActState;
			BOOL				m_bUseArmSub;
			INT					m_dwThaiCCafeClass;
			INT					m_nMyCCafeClass;				// 말레이시아 PC방 이벤트
			SChinaTime			m_sChinaTime;
			SVietnamGainSystem  m_sVietnamSystem;
			SEventTime			m_sEventTime;
		};

		NET_MSG_GENERIC		nmg;

		DWORD				m_dwClientID;
		SCHARDATA2*			m_pCharData;
		SINFO				m_sINFO;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(SCHARDATA2*)+sizeof(SINFO), };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETJOIN2FIELDSERVER () :
			m_dwClientID(0),
			m_pCharData(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_FIELD_REQ_JOIN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETLOBBY_REQ_GAME_JOIN
	{
		NET_MSG_GENERIC		nmg;

		SNETLOBBY_REQ_GAME_JOIN ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_LOBBY_REQ_GAME_JOIN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_CHARINFO
	{
		NET_MSG_GENERIC		nmg;

		SCHARINFO_LOBBY		Data;

		SNETLOBBY_CHARINFO ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_LOBBY_CHAR_SEL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_CHARJOIN
	{
		NET_MSG_GENERIC		nmg;

		char				szUserID[USR_ID_LENGTH+1];

		DWORD				dwClientID;
		DWORD				dwGaeaID;

		SNATIVEID			sMapID;								//	맵 ID.
		D3DXVECTOR3			vPos;								//	맵 상의 위치.

		SCHARDATA			Data;								//	기본 데이터.
		
		WORD				wSKILLQUICK_ACT;					//	스킬 퀵슬롯중 액티브된 스킬.
		SNATIVEID			sSKILLQUICK[EMSKILLQUICK_SIZE];		//	스킬 퀵슬롯.
		SACTION_SLOT		sACTIONQUICK[EMACTIONQUICK_SIZE];	//	엑션 퀵슬롯.

		DWORD				dwNumInvenItems;					//	인밴에 들어있는 아이템 갯수.
		DWORD				dwNumVNInvenItems;					//	베트남 인밴에 들어있는 아이템 갯수.
		DWORD				dwNumSkill;							//	습득한 스킬의 갯수.
		DWORD				dwNumQuestProc;
		DWORD				dwNumQuestEnd;
		DWORD				dwChargeItem;
		DWORD				dwItemCoolTime;

		bool				bBIGHEAD;
		bool				bBIGHAND;

		SNATIVEID			sStartMapID;				//	초기 시작 맵.
		DWORD				dwStartGate;				//	초기 시작 개이트.

		SNATIVEID			sLastCallMapID;				//	직전귀환 맵.
		D3DXVECTOR3			vLastCallPos;				//	직전귀환 위치.

		bool				bTracingChar;				//  추적 대상인지 아닌지

		DWORD				dwThaiCCafeClass;			//  태국 사이버 까페
		INT					nMyCCafeClass;				//  말레이시아 PC방 이벤트


		SNETLOBBY_CHARJOIN () :
			dwClientID(0),
			dwGaeaID(0),

			wSKILLQUICK_ACT(0),

			dwNumInvenItems(0),
			dwNumVNInvenItems(0),
			dwNumSkill(0),

			dwNumQuestProc(0),
			dwNumQuestEnd(0),

			dwChargeItem(0),
			dwItemCoolTime(0),

			bBIGHEAD(false),
			bBIGHAND(false),

			sStartMapID(false),
			dwStartGate(0),
			sLastCallMapID(false),
			vLastCallPos(0,0,0),
			bTracingChar(FALSE),
			dwThaiCCafeClass(0),
			nMyCCafeClass(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_LOBBY_CHAR_JOIN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
			memset ( szUserID, 0, sizeof( char ) * ( USR_ID_LENGTH+1) );
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_CHARPUTON
	{
		NET_MSG_GENERIC		nmg;

		SITEMCUSTOM			PutOnItems[SLOT_TSIZE];				//	착용 아이템.

		SNETLOBBY_CHARPUTON ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_LOBBY_CHAR_PUTON;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETENTRY_FAILED
	{
		NET_MSG_GENERIC		nmg;

		SNETENTRY_FAILED () 
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_ETNRY_FAILED;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETLIMITTIME_OVER
	{
		NET_MSG_GENERIC		nmg;
		int					nRemainTime;

		SNETLIMITTIME_OVER () :
				nRemainTime( 0 )
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_LIMITTIME_OVER;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETLOBBY_CHARPUTON_EX
	{
		NET_MSG_GENERIC		nmg;

		int					nSlot;
		SITEMCUSTOM			PutOnItem;							//	착용 아이템.

		SNETLOBBY_CHARPUTON_EX ()
		: nSlot ( 0 )
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_LOBBY_CHAR_PUTON_EX;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};	

	struct SNETLOBBY_CHARSKILL
	{
		NET_MSG_GENERIC		nmg;

		DWORD				wNum;
		SCHARSKILL			sSKILL[EMGLMSG_SKILLMAX];

		SNETLOBBY_CHARSKILL () :
			wNum(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_LOBBY_CHAR_SKILL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		BOOL ADD ( const SCHARSKILL &sSkill )
		{
			if ( wNum == EMGLMSG_SKILLMAX )	return FALSE;

			sSKILL[wNum] = sSkill;
			wNum++;

			nmg.dwSize = sizeof(*this) - sizeof(SCHARSKILL) * ( EMGLMSG_SKILLMAX - wNum );
			return TRUE;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_INVENITEM
	{
		NET_MSG_GENERIC		nmg;
		SINVENITEM			Data;

		SNETLOBBY_INVENITEM ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_LOBBY_CHAR_ITEM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_VIETNAM_INVENITEM
	{
		NET_MSG_GENERIC		nmg;
		SINVENITEM			Data;

		SNETLOBBY_VIETNAM_INVENITEM ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_LOBBY_CHAR_VIETNAM_ITEM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_QUEST_PROG
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)-sizeof(DWORD), };

		NET_MSG_GENERIC		nmg;
		
		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNETLOBBY_QUEST_PROG () :
			dwLengthStream(0)
		{
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD);
			nmg.nType = NET_MSG_LOBBY_QUEST_PROG;
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

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_QUEST_END
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)-sizeof(DWORD), };

		NET_MSG_GENERIC		nmg;
		
		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNETLOBBY_QUEST_END () :
			dwLengthStream(0)
		{
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD);
			nmg.nType = NET_MSG_LOBBY_QUEST_END;
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

	struct SNETLOBBY_CHARGE_ITEM
	{
		NET_MSG_GENERIC		nmg;

		char				szPurKey[PURKEY_LENGTH+1];
		SNATIVEID			nidITEM;

		SNETLOBBY_CHARGE_ITEM () 
			: nidITEM(false)
		{
			nmg.dwSize = sizeof(SNETLOBBY_CHARGE_ITEM);
			nmg.nType = NET_MSG_LOBBY_CHARGE_ITEM;
			memset(szPurKey, 0, sizeof(char) * (PURKEY_LENGTH+1));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETLOBBY_ITEM_COOLTIME
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwNum;
		EMCOOL_TYPE			emCoolType;
		ITEM_COOLTIME		sItemCoolTime[EMGLMSG_COOLTIMEMAX];

		SNETLOBBY_ITEM_COOLTIME ()
			: dwNum(0)
			, emCoolType( EMCOOL_ITEMID )
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_LOBBY_ITEM_COOLTIME;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		BOOL ADD ( const ITEM_COOLTIME &sCoolTime )
		{
			if ( dwNum == EMGLMSG_COOLTIMEMAX )	return FALSE;

			sItemCoolTime[dwNum] = sCoolTime;
			dwNum++;

			nmg.dwSize = sizeof(*this) - sizeof(ITEM_COOLTIME) * ( EMGLMSG_COOLTIMEMAX - dwNum );
			return TRUE;
		}
	};
	
	struct SNETLOBBY_CHARJOIN_FB
	{
		NET_MSG_GENERIC		nmg;
		EMCHAR_JOIN_FB		emCharJoinFB;

		SNETLOBBY_CHARJOIN_FB () 
			: emCharJoinFB(EMCJOIN_FB_NOWLOGIN)
		{
			nmg.dwSize = sizeof(SNETLOBBY_CHARJOIN_FB);
			nmg.nType = NET_MSG_LOBBY_CHAR_JOIN_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETLOBBY_CHARJOIN_CHINA_ERROR
	{
		NET_MSG_GENERIC		nmg;

		SNETLOBBY_CHARJOIN_CHINA_ERROR () 
		{
			nmg.dwSize = sizeof(SNETLOBBY_CHARJOIN_CHINA_ERROR);
			nmg.nType = NET_MSG_LOBBY_CHINA_ERROR;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_FIELDSVR_CHARCHK
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;
		DWORD				dwClientID;

		DWORD				dwUserID;
		DWORD				dwCharID;
		CHAR				szName[CHR_ID_LENGTH+1];

		SNETPC_FIELDSVR_CHARCHK	() 
			: dwGaeaID(0)
			, dwClientID(0)
			
			, dwUserID(0)
			, dwCharID(0)
		{
			nmg.dwSize = sizeof(SNETPC_FIELDSVR_CHARCHK);
			nmg.nType = NET_MSG_FIELDSVR_CHARCHK;
			memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_FIELDSVR_CHARCHK_FB
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;
		CHAR				szName[CHR_ID_LENGTH+1];

		int					nChannel;
		DWORD				dwFIELDID;
		bool				bExist;


		SNETPC_FIELDSVR_CHARCHK_FB () 
			: dwGaeaID(0)
			, nChannel(0)
			, dwFIELDID(0)
			, bExist(false)
		{
			nmg.dwSize = sizeof(SNETPC_FIELDSVR_CHARCHK_FB);
			nmg.nType = NET_MSG_FIELDSVR_CHARCHK_FB;
			memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_FIELDSVR_OUT
	{
		NET_MSG_GENERIC		nmg;

		SNETPC_FIELDSVR_OUT ()
		{
			nmg.dwSize = sizeof(SNETPC_FIELDSVR_OUT);
			nmg.nType = NET_MSG_GCTRL_FIELDSVR_OUT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_FIELDSVR_OUT_FB
	{
		NET_MSG_GENERIC		nmg;

		SNETPC_FIELDSVR_OUT_FB ()
		{
			nmg.dwSize = sizeof(SNETPC_FIELDSVR_OUT_FB);
			nmg.nType = NET_MSG_GCTRL_FIELDSVR_OUT_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REBIRTH_OUT
	{
		NET_MSG_GENERIC		nmg;

		SNETPC_REBIRTH_OUT ()
		{
			nmg.dwSize = sizeof(SNETPC_REBIRTH_OUT);
			nmg.nType = NET_MSG_GCTRL_REBIRTH_OUT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REBIRTH_OUT_FB
	{
		NET_MSG_GENERIC		nmg;

		SNETPC_REBIRTH_OUT_FB ()
		{
			nmg.dwSize = sizeof(SNETPC_REBIRTH_OUT_FB);
			nmg.nType = NET_MSG_GCTRL_REBIRTH_OUT_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETREQ_GATEOUT_REQ
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGateID;

		SNETREQ_GATEOUT_REQ () 
			: dwGateID(0)
		{
			nmg.dwSize = sizeof(SNETREQ_GATEOUT_REQ);
			nmg.nType = NET_MSG_GCTRL_REQ_GATEOUT_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETREQ_GATEOUT
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGateID;
		DWORD				dwGaeaID;

		SNETREQ_GATEOUT () 
			: dwGateID(0)
			, dwGaeaID(0)
		{
			nmg.dwSize = sizeof(SNETREQ_GATEOUT);
			nmg.nType = NET_MSG_GCTRL_REQ_GATEOUT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETREQ_GATEOUT_FB
	{
		NET_MSG_GENERIC		nmg;

		EMCHAR_GATEOUT_FB	emFB;

		SNATIVEID			sMapID;
		D3DXVECTOR3			vPos;

		SNETREQ_GATEOUT_FB () 
			: emFB(EMCHAR_GATEOUT_OK)
			, sMapID(0,0)
			, vPos(0,0,0)
		{
			nmg.dwSize = sizeof(SNETREQ_GATEOUT_FB);
			nmg.nType = NET_MSG_GCTRL_REQ_GATEOUT_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETREQ_LANDIN
	{
		NET_MSG_GENERIC		nmg;

		SNETREQ_LANDIN ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_LANDIN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETREQ_READY
	{
		NET_MSG_GENERIC		nmg;

		SNETREQ_READY ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_READY;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_REBIRTH
	{
		NET_MSG_GENERIC		nmg;

		// 진입조건이 맞는 경우를 위해 부활지점을 체크한다.
		bool				bRegenEntryFailed;

		SNETPC_REQ_REBIRTH () :
					bRegenEntryFailed(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_REBIRTH;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};
	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_REBIRTH_FB
	{
		NET_MSG_GENERIC		nmg;

		SNATIVEID			sMapID;
		D3DXVECTOR3			vPos;

		WORD				wNowHP;
		WORD				wNowMP;
		WORD				wNowSP;

		bool				bRegenEntryFailed;

		SNETPC_REQ_REBIRTH_FB () :
			vPos(0,0,0),
			wNowHP(0),
			wNowMP(0),
			wNowSP(0),
			bRegenEntryFailed(FALSE)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_REBIRTH_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_RECALL_AG
	{
		NET_MSG_GENERIC		nmg;

		SNATIVEID			sMAPID;
		DWORD				dwGATEID;
		D3DXVECTOR3			vPOS;

		SNETPC_REQ_RECALL_AG () :
			sMAPID(false),
			dwGATEID(UINT_MAX),
			vPOS(0,0,0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_RECALL_AG;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_RECALL_FB
	{
		NET_MSG_GENERIC		nmg;

		EMREQ_RECALL_FB		emFB;
		SNATIVEID			sMAPID;
		D3DXVECTOR3			vPOS;

		SNETPC_REQ_RECALL_FB () :
			emFB(EMREQ_RECALL_FB_OK),
			sMAPID(false),
			vPOS(0,0,0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_RECALL_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_MUST_LEAVE_MAP_AG
	{
		NET_MSG_GENERIC		nmg;

		SNATIVEID			sMAPID;
		DWORD				dwGATEID;
		D3DXVECTOR3			vPOS;

		SNETPC_REQ_MUST_LEAVE_MAP_AG () :
		sMAPID(false),
			dwGATEID(UINT_MAX),
			vPOS(0,0,0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_REQ_MUST_LEAVE_MAP_AG;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_MUST_LEAVE_MAP_FB
	{
		NET_MSG_GENERIC						nmg;

		EMREQ_MUST_LEAVE_MAP_FB		emFB;
		SNATIVEID							sMAPID;
		D3DXVECTOR3							vPOS;

		SNETPC_REQ_MUST_LEAVE_MAP_FB () :
		emFB(EMREQ_MUST_LEAVE_MAP_FB_FAIL),
			sMAPID(false),
			vPOS(0,0,0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_REQ_MUST_LEAVE_MAP_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_MUST_LEAVE_MAP
	{
		NET_MSG_GENERIC		nmg;
		SNATIVEID			sMAPID;
		DWORD				dwGATEID;

		SNETPC_REQ_MUST_LEAVE_MAP () :
			sMAPID(false),
			dwGATEID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_REQ_MUST_LEAVE_MAP;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPC_REQ_TELEPORT_AG
	{
		NET_MSG_GENERIC		nmg;
		SNATIVEID			sMAPID;
		D3DXVECTOR3			vPOS;

		SNETPC_REQ_TELEPORT_AG () :
			sMAPID(false),
			vPOS(0,0,0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_TELEPORT_AG;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_TELEPORT_FB
	{
		NET_MSG_GENERIC		nmg;

		EMREQ_TELEPORT_FB		emFB;
		SNATIVEID			sMAPID;
		D3DXVECTOR3			vPOS;

		SNETPC_REQ_TELEPORT_FB () :
			emFB(EMREQ_TELEPORT_FB_OK),
			sMAPID(false),
			vPOS(0,0,0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_TELEPORT_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_LAND_INFO
	{
		NET_MSG_GENERIC		nmg;

		SNATIVEID			nidMAP;
		bool				bClubBattle;
		bool				bClubBattleHall;
		bool				bClubDeathMatch;
		bool				bClubDeathMatchHall;
		bool				bPK;

		DWORD				dwGuidClub;
		DWORD				dwGuidClubMarkVer;
		char				szGuidClubName[CHAR_SZNAME];
		float				fCommission;

		SNETPC_LAND_INFO () 
			: bClubBattle(false)
			, bClubBattleHall(false)
			, bPK(false)
			, bClubDeathMatch(false)
			, bClubDeathMatchHall(false)
			, dwGuidClub(CLUB_NULL)
			, dwGuidClubMarkVer(0)
			, fCommission(0.0f)
		{
			nmg.dwSize = sizeof(SNETPC_LAND_INFO);
			nmg.nType = NET_MSG_GCTRL_LAND_INFO;
			memset(szGuidClubName, 0, sizeof(char) * (CHAR_SZNAME));
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNETPC_SERVER_INFO
	{
		NET_MSG_GENERIC		nmg;
		bool				bSCHOOL_FREEPK;

		SNETPC_SERVER_INFO () 
			: bSCHOOL_FREEPK(false)
		{
			nmg.dwSize = sizeof(SNETPC_SERVER_INFO);
			nmg.nType = NET_MSG_GCTRL_SERVER_INFO;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SERVER_BRIGHTEVENT_INFO
	{
		NET_MSG_GENERIC		nmg;
		bool				bBRIGHTEVENT;

		SNETPC_SERVER_BRIGHTEVENT_INFO () :
			bBRIGHTEVENT(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_SERVER_BRIGHTEVENT_INFO;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SERVER_CLUB_BATTLE_INFO
	{
		NET_MSG_GENERIC		nmg;
		bool				bClubBattle;

		SNETPC_SERVER_CLUB_BATTLE_INFO () :
			bClubBattle(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_SERVER_CLUB_BATTLE_INFO;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SERVER_CLUB_BATTLE_REMAIN_AG
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;

		SNETPC_SERVER_CLUB_BATTLE_REMAIN_AG ()
			: dwGaeaID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_SERVER_CLUB_BATTLE_REMAIN_AG;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPC_SERVER_CLUB_DEATHMATCH_INFO
	{
		NET_MSG_GENERIC		nmg;
		bool				bClubDeathMatch;

		SNETPC_SERVER_CLUB_DEATHMATCH_INFO () :
			bClubDeathMatch(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_SERVER_CLUB_DEATHMATCH_INFO;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SERVER_CLUB_DEATHMATCH_REMAIN_AG
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;

		SNETPC_SERVER_CLUB_DEATHMATCH_REMAIN_AG ()
			: dwGaeaID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_REQ_SERVER_CLUB_DEATHMATCH_REMAIN_AG;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SCHOOLFREEPK_FLD
	{
		NET_MSG_GENERIC		nmg;
		bool				bSCHOOL_FREEPK;

		SNETPC_SCHOOLFREEPK_FLD () :
			bSCHOOL_FREEPK(false)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_SCHOOLFREEPK_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SAVECHARPOSRST_FROMDB
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwCharID;
		DWORD				dwFieldServer;
		EMSAVECHARPOS_FB	emFB;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)*3 };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPC_SAVECHARPOSRST_FROMDB ()
			: emFB(EMSAVECHARPOS_FB_FAIL)
			, dwCharID(0)
			, dwFieldServer(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_CHARPOS_FROMDB2AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_MARKETCHECK
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;

		SNETPC_MARKETCHECK ()
			: dwGaeaID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_MARKET_CHECKTIME;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	// Revert to default structure packing
	#pragma pack()
};
