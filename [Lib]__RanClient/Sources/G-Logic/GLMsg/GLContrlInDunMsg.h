#pragma once

#include "GLContrlBaseMsg.h"

enum EMCHAR_CREATE_INSTANT_MAP_FB
{
	EMCHAR_CREATE_INSTANT_MAP_OK			  = 0,	// 인던 생성 완료
	EMCHAR_CREATE_INSTANT_MAP_FAIL			 = 1, // 인던 실패
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATE	 = 2, // 인던 생성 실패
	EMCHAR_CREATE_INSTANT_MAP_FAIL_MAPID	 = 3, // 인던 현재 맵 아이디를 찾을 수 없을경우
	EMCHAR_CREATE_INSTANT_MAP_FAIL_GATEID	 = 4, // 인던 현재 게이트 아이디를 찾을 수 없을경우
	EMCHAR_CREATE_INSTANT_MAP_FAIL_TARMAPID  = 5, // 인던 타겟 아이디를 찾을수 없을경우
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CONDITION = 6, // 인던 진입 조건 실패
	EMCHAR_CREATE_INSTANT_MAP_FAIL_FIELDSVR  = 7, // 인던 생성 필드서버 문제로 실패
	EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT		 = 8, // 인던이 아닌경우
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATEMAX = 9, // 인던을 생성할 수 있는 최대 개수 초과
};

namespace GLMSG
{
	#pragma pack(1)

	
	//---------------------------------------------------------------------------NET
	struct SNETREQ_CREATE_INSTANT_MAP_REQ
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGateID;
		DWORD				dwGaeaID;
		bool				bCreate;
		SNATIVEID			sInstantMapID;


		SNETREQ_CREATE_INSTANT_MAP_REQ () 
			: dwGateID(0)
			, dwGaeaID(0)
			, sInstantMapID(NATIVEID_NULL())
			, bCreate(TRUE)
		{
			nmg.dwSize = sizeof(SNETREQ_CREATE_INSTANT_MAP_REQ);
			nmg.nType = NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	
	struct SNETREQ_CREATE_INSTANT_MAP_FLD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGateID;
		DWORD				dwGaeaID;
		SNATIVEID			sInstantMapID;
		SNATIVEID			sBaseMapID;
		bool				bCreate;

		SNETREQ_CREATE_INSTANT_MAP_FLD () 
			: dwGateID(0)
			, dwGaeaID(0)
			, sInstantMapID(NATIVEID_NULL())
			, sBaseMapID(NATIVEID_NULL())
			, bCreate(FALSE)
		{
			nmg.dwSize = sizeof(SNETREQ_CREATE_INSTANT_MAP_FLD);
			nmg.nType = NET_MSG_GCTRL_CREATE_INSTANT_MAP_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETREQ_CREATE_INSTANT_MAP_FB
	{
		NET_MSG_GENERIC		nmg;

		EMCHAR_CREATE_INSTANT_MAP_FB	emFB;

		SNATIVEID			sBaseMapID;
		SNATIVEID			sInstantMapID;
		D3DXVECTOR3			vPos;

		SNETREQ_CREATE_INSTANT_MAP_FB () 
			: emFB(EMCHAR_CREATE_INSTANT_MAP_OK)
			, sInstantMapID(NATIVEID_NULL())
			, sBaseMapID(NATIVEID_NULL())
			, vPos(0,0,0)
		{
			nmg.dwSize = sizeof(SNETREQ_CREATE_INSTANT_MAP_FB);
			nmg.nType = NET_MSG_GCTRL_CREATE_INSTANT_MAP_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETREQ_CREATE_INSTANT_MAP_DEL
	{
		NET_MSG_GENERIC		nmg;
		SNATIVEID			sInstantMapID;

		SNETREQ_CREATE_INSTANT_MAP_DEL () 
			: sInstantMapID(NATIVEID_NULL())
		{
			nmg.dwSize = sizeof(SNETREQ_CREATE_INSTANT_MAP_DEL);
			nmg.nType = NET_MSG_GCTRL_CREATE_INSTANT_MAP_DEL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};



	// Revert to default structure packing
	#pragma pack()
};