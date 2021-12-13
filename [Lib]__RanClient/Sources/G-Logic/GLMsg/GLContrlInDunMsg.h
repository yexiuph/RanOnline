#pragma once

#include "GLContrlBaseMsg.h"

enum EMCHAR_CREATE_INSTANT_MAP_FB
{
	EMCHAR_CREATE_INSTANT_MAP_OK			  = 0,	// �δ� ���� �Ϸ�
	EMCHAR_CREATE_INSTANT_MAP_FAIL			 = 1, // �δ� ����
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATE	 = 2, // �δ� ���� ����
	EMCHAR_CREATE_INSTANT_MAP_FAIL_MAPID	 = 3, // �δ� ���� �� ���̵� ã�� �� �������
	EMCHAR_CREATE_INSTANT_MAP_FAIL_GATEID	 = 4, // �δ� ���� ����Ʈ ���̵� ã�� �� �������
	EMCHAR_CREATE_INSTANT_MAP_FAIL_TARMAPID  = 5, // �δ� Ÿ�� ���̵� ã���� �������
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CONDITION = 6, // �δ� ���� ���� ����
	EMCHAR_CREATE_INSTANT_MAP_FAIL_FIELDSVR  = 7, // �δ� ���� �ʵ弭�� ������ ����
	EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT		 = 8, // �δ��� �ƴѰ��
	EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATEMAX = 9, // �δ��� ������ �� �ִ� �ִ� ���� �ʰ�
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