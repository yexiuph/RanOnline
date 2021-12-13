#pragma once

#include "s_NetGlobal.h"
#include "GLCharData.h"
#include "GLCrowData.h"
#include "GLItem.h"
#include "GLMoney.h"
#include "GLParty.h"
#include "GLPeriod.h"
#include "GLAutoLevel.h"
#include "GLClubMan.h"
#include "GLCDMDefine.h"

//	#include "GLContrlMsg.h"
//	메세지 정보의 크기를 (NET_DATA_BUFSIZE) byte 이내로 제한 하여 설계.
//
#define NET_DELAY		(0.2f)
#define NET_MSGDELAY	(0.3f)

#define MAX_SEARCH_RESULT 10

inline float APPLY_MSGDELAY ( float &fDelay )
{
	fDelay -= NET_MSGDELAY;
	if ( fDelay < 0.0f )	fDelay = 0.0f;
	return fDelay;
}

enum EMGLMSG
{
	EMGLMSG_SKILLMAX	= 100,
	EMGLMSG_COOLTIMEMAX	= 30,
};

enum EMREQUEST_MSG
{
	EMREQMSG_OBJ_DROP	= 0,
};

enum EMTAKE_FB
{
	EMTAKE_FB_OFF				= 0,	//	아이탬이 사라진 경우.
	EMTAKE_FB_RESERVED			= 1,	//	예약 소유자가 지정되어 있을 경우.
	EMTAKE_FB_INVEN_ERROR		= 2,	//	인벤토리에 넣을수 없음.
	EMTAKE_FB_TOO_EVENT_ITEM	= 3,	//	이미 이벤트 아이템을 가지고 있습니다.
	EMTAKE_FB_DISTANCE			= 4,	//	거리가 너무 멉니다.
};

enum EMHOLD_FB
{
	EMHOLD_FB_OFF				= 0,	//	동일한 아이탬이 존제하지 않습니다.
	EMHOLD_FB_OK				= 1,	//	아이탬을 HOLD합니다.
	EMHOLD_FB_NONKEEPSTORAGE	= 2,	//	락커에 물품을 넣을 수 없음.
};

enum EMCHAR_JOIN_FB
{
	EMCJOIN_FB_ERROR		= 0,
	EMCJOIN_FB_NOWLOGIN		= 1,	//	현제 접속중인 상태라서 접속이 안됩니다.
	EMCJOIN_FB_PKPOINT		= 2,	//	pk 성향이 높아서 해당 채널에 접속 불가.
};

enum EMDROP_OUT_FORCED
{
	EMDROPOUT_ERROR			= 0,	//	오류로 인한 접속 종료.
	EMDROPOUT_REQLOGIN		= 1,	//	동일 유저의 접속 시도로 인한 접속 종료.
};

enum EMCHAR_GATEOUT_FB
{
	EMCHAR_GATEOUT_OK		= 0,
	EMCHAR_GATEOUT_FAIL		= 1,
	EMCHAR_GATEOUT_MAPID	= 2,
	EMCHAR_GATEOUT_GATEID	= 3,
	EMCHAR_GATEOUT_TARMAPID	= 4,
	EMCHAR_GATEOUT_TARGATEID= 5,
	EMCHAR_GATEOUT_FIELDSVR	= 6,
	EMCHAR_GATEOUT_HOLD		= 7,
	EMCHAR_GATEOUT_CONDITION = 8,
	EMCHAR_GATEOUT_CLUBBATTLE = 9,		//	선도전맵시 클럽배틀
	EMCHAR_GATEOUT_CLUBBATTLE2 = 10,	//	CDM맵시 클럽배틀
};


enum EMSKILL_LEARNCHECK
{
	EMSKILL_LEARN_OK		= 0,	//	배우기 성공.
	EMSKILL_LEARN_MAX		= 1,	//	더 이상 렙업을 할 수 없음.
	EMSKILL_LEARN_NOTTERM	= 2,	//	요구 조건이 충족되지 않아서 렙업을 할수 없음.
	EMSKILL_LEARN_NOTCLASS	= 3,	//	부서가 틀려서 배울수 없습니다.
	EMSKILL_LEARN_NOTBRIGHT	= 4,	//	빛/암 조건이 충족되지 않습니다.
	EMSKILL_LEARN_ALREADY	= 5,	//	이미 배운 스킬입니다.

	EMSKILL_LEARN_UNKNOWN	= 999
};

enum EMPARTY_LURE_FB
{
	EMPARTY_LURE_OK			= 0,	//	파티 모집에 수락.
	EMPARTY_LURE_REFUSE		= 1,	//	파티 모집에 거부.
	
	EMPARTY_LURE_FULL		= 2,	//	파티가 가득차서 더이상 오을수 없음.
	EMPARTY_LURE_OTHER		= 3,	//	다른 파티에 가입되어 있어서 거절됨.
	EMPARTY_LURE_OPT		= 4,	//	파티 옵션 오류.
	EMPARTY_LURE_OTHERSCHOOL= 5,	//	다른 학원생과의 파티 불가능.
	EMPARTY_LURE_CLUBBATTLE	= 6,	//	클럽배틀 중일때는 파티 불가
};

enum EMTRADE_ANS
{
	EMTRADE_OK				= 0,
	EMTRADE_CANCEL			= 1,
};

enum EMTRADE_AGREE_FB
{
	EMTRADE_AGREE_OK		= 0,
	EMTRADE_AGREE_CANCEL	= 1,
	EMTRADE_AGREE_TIME		= 2,
};

enum EMTRADE_CANCEL_TAR
{
	EMTRADE_CANCEL_NORMAL	= 0,	//	일반적인 취소.
	EMTRADE_CANCEL_OTHER	= 1,	//	상대편이 취소함.
	EMTRADE_CANCEL_MYINVEN	= 2,	//	개래 도중 인밴 부족.
	EMTRADE_CANCEL_TARINVEN	= 3,	//	개래 도중 인밴 부족.
};

enum EMGRINDING_FB
{
	EMGRINDING_FAIL			= 0,
	EMGRINDING_SUCCEED		= 1
};

enum EMREGEN_GATE_FB
{
	EMREGEN_GATE_FAIL		= 0,
	EMREGEN_GATE_SUCCEED	= 1,
};

enum EMREGEN_CURE_FB
{
	EMREGEN_CURE_FAIL		= 0,
	EMREGEN_CURE_SUCCEED	= 1,
};

enum EMREGEN_CHARRESET_FB
{
	EMREGEN_CHARRESET_SUCCEED	= 0,
	EMREGEN_CHARRESET_FAIL		= 1,
	EMREGEN_CHARRESET_ITEM_FAIL	= 2,
};

enum EMNPC_ITEM_TRADE_FB
{
	EMNPC_ITEM_TRADE_SUCCEED		= 0,
	EMNPC_ITEM_TRADE_FAIL			= 1,
	EMNPC_ITEM_TRADE_ITEM_FAIL		= 2,
	EMNPC_ITEM_TRADE_INSERT_FAIL	= 3,
};

enum EMCONFRONT_FB
{
	EMCONFRONT_FAIL			= 0,	//	일반 오류.
	EMCONFRONT_AGREE		= 1,	//	대련 수락.	(ans용)
	EMCONFRONT_REFUSE		= 2,	//	대련 거부.
	EMCONFRONT_OPTION		= 3,	//	옵션 오류.
	EMCONFRONT_PEACE		= 4,	//	평화 지역 대련 불가.
	EMCONFRONT_MAX			= 5,	//	n 번 이상 연속 대련으로 time 이후 대련 가능.
	EMCONFRONT_ELAP			= 6,	//	종전 대련후 time 이후 대련 가능.
	EMCONFRONT_ALREADY_ME	= 7,	//	자신이 이미 대련중.
	EMCONFRONT_ALREADY_TAR	= 8,	//	상대방이 이미 대련중.
	EMCONFRONT_PARTY		= 9,	//	같은 파티원 끼리는 대련 불가.
	EMCONFRONT_DIE_YOU		= 10,	//	상대 캐릭터가 죽어있어서 대련 불가.
	EMCONFRONT_DIE_ME		= 11,	//	자신의 캐릭터가 죽어있어서 대련 불가.

	EMCONFRONT_MENOTPARTY	= 12,	//	파티원이 아니여서 파티 대련이 불가능.
	EMCONFRONT_MENOTMASTER	= 13,	//	파티장이 아니여서 '파티대련'을 신청하실수 없습니다.
	EMCONFRONT_TARMASTER	= 14,	//	파티장이 아니여서 '파티대련'을 신청하실수 없습니다.
};

enum EMCONFRONT_END
{
	EMCONFRONT_END_FAIL		= 0,	//	일반 오류.
	EMCONFRONT_END_WIN		= 1,	//	승리.
	EMCONFRONT_END_LOSS		= 2,	//	패배.
	EMCONFRONT_END_TIME		= 3,	//	시간 초과.
	EMCONFRONT_END_MOVEMAP	= 4,	//	맵 이동.
	EMCONFRONT_END_DISWIN	= 5,	//	거리가 너무 떨어짐.
	EMCONFRONT_END_DISLOSS	= 6,	//	거리가 너무 떨어짐.

	EMCONFRONT_END_PARTY	= 7,	//	대련 도중 서로 파티원이 되어서 종료됨.

	EMCONFRONT_END_NOTWIN	= 10,	//	승리에 기여하지 못함.
	EMCONFRONT_END_PWIN		= 11,	//	파티대련에 승리.
	EMCONFRONT_END_PLOSS	= 12,	//	파티대련에 패배.
	EMCONFRONT_END_PTIME	= 13,	//	파티대련 시간초과.

	EMCONFRONT_END_CWIN		= 14,	//	클럽대련에 승리.
	EMCONFRONT_END_CLOSS	= 15,	//	클럽대련에 패배.
	EMCONFRONT_END_CTIME	= 16,	//	클럽대련 시간초과.
};

enum EMANTIDISAPPEAR
{
	EMANTIDISAPPEAR_OFF	= 0,		// 소방주 없음
	EMANTIDISAPPEAR_ON	= 1,		// 소방주 착용
	EMANTIDISAPPEAR_USE	= 2,		// 소방주 사용
};

// 캐릭터 로비 디스플레이용 데이터
struct SCHARINFO_LOBBY
{
	DWORD				m_dwCharID;

	char				m_szName[CHAR_SZNAME];		// 이름. (고정)
	EMCHARCLASS			m_emClass;					// 직업. (고정)
	WORD				m_wSchool;					// 학원.
	WORD				m_wHair;					// 헤어스타일
	WORD				m_wFace;					// 얼굴모양
	WORD                m_wSex;                     // 성별 (추가:2005-12-29 Jgkim)
	WORD                m_wHairColor;               // 머리 색깔 (추가:2005-12-29 Jgkim)

	GLPADATA			m_sHP;						//	생명량. ( 현제/최대량 )
	GLLLDATA			m_sExperience;				//	경험치. ( 현제/다음레밸도달값 )
	int					m_nBright;					//	속성.

	WORD				m_wLevel;					//	레벨.
	SCHARSTATS			m_sStats;					//	Stats.

	SITEM_LOBY			m_PutOnItems[SLOT_TSIZE];	//	형상 정보.

	SNATIVEID			m_sSaveMapID;				//	종료 맵.

	SCHARINFO_LOBBY () 
		: m_dwCharID(0)
		, m_emClass(GLCC_ARMS_M)
		, m_wLevel(1)
		, m_wSchool(0)
		, m_wHair(0)
		, m_wFace(0)
		, m_wSex(0)
		, m_wHairColor(0)
		, m_nBright(0)
	{
		memset(m_szName, 0, sizeof(char) * CHAR_SZNAME);		
	}
};

namespace GLMSG
{
	#pragma pack(1)

	struct SNETPC_BROAD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;

		SNETPC_BROAD () 
			: dwGaeaID(0)
		{
		}
	};

	struct SNETCROW_BROAD
	{
		NET_MSG_GENERIC		nmg;
		EMCROW				emCrow;
		DWORD				dwID;

		SNETCROW_BROAD () 
			: emCrow(CROW_PC)
			, dwID(0)
		{
		}
	};

	struct SNETPET_BROAD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGUID;

		SNETPET_BROAD () :
			dwGUID(UINT_MAX)
		{
		}
	};

	struct SNET_COMBINE
	{
		enum
		{
			//				최대전송량    - ( 메시지 헤드. )
			EMHEAD_SIZE = ( sizeof(NET_MSG_GENERIC) + sizeof(WORD)*2 ),
			EMMAX_SIZE = NET_DATA_BUFSIZE - EMHEAD_SIZE,
		};

		NET_MSG_GENERIC		nmg;
		WORD				wMSG_SIZE;
		WORD				wMSG_CUR;

		BYTE				arrayMSG[EMMAX_SIZE];

		SNET_COMBINE () 
			: wMSG_SIZE(0)
			, wMSG_CUR(0)
		{
			nmg.dwSize = sizeof(SNET_COMBINE);
			nmg.nType = NET_MSG_COMBINE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(arrayMSG, 0, sizeof(BYTE) * EMMAX_SIZE);
		}

		bool IsVALID ()
		{
			return wMSG_SIZE!=0;
		}

		void RESET ()
		{
			wMSG_SIZE = 0;
			wMSG_CUR = 0;
			memset(arrayMSG, 0, sizeof(BYTE) * EMMAX_SIZE);
		}

		void RESET_CUR ()
		{
			wMSG_CUR = 0;
		}

		bool ADDMSG ( LPVOID _pMsg )
		{
			NET_MSG_GENERIC* pMsg = (NET_MSG_GENERIC*)_pMsg;
			
			//	Note : 아직 들어갈 공간이 있는지 점검.
			//
			WORD wSIZE = (WORD) pMsg->dwSize;
			if ( (wSIZE+wMSG_SIZE) >= EMMAX_SIZE )
			{
				return false;
			}
			else
			{
				//	Note : 메시지를 뒤에 넣는다.
				//
				memcpy ( arrayMSG+wMSG_SIZE, _pMsg, wSIZE );
				//	Note : 메시지가 차지한 공간을 계산.
				//
				wMSG_SIZE += wSIZE;
				//	메시지 사이즈 재 설정.
				nmg.dwSize = EMHEAD_SIZE + wMSG_SIZE;	
				return true;
			}
		}

		bool POPMSG ( LPNET_MSG_GENERIC &pMsg )
		{
			if ( wMSG_SIZE<=wMSG_CUR )	return false;

			pMsg = (NET_MSG_GENERIC*) ( arrayMSG + (wMSG_CUR) );
			wMSG_CUR += (WORD) pMsg->dwSize;

			GASSERT(wMSG_SIZE>=wMSG_CUR);
			if ( wMSG_SIZE<wMSG_CUR )	return false;

			return true;
		}
	};

	// Revert to default structure packing
	#pragma pack()
};
