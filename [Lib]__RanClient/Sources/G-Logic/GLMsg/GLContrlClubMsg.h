#pragma once

#include "GLContrlBaseMsg.h"

enum EMCLUB_NEW_FB
{
	EMCLUB_NEW_FB_FAIL			= 0,	// �Ϲݿ���.
	EMCLUB_NEW_FB_OK			= 1,	// ����.
	EMCLUB_NEW_FB_ALREADY		= 2,	// �̹� �ٸ� Ŭ���� ���ԵǾ� ����.
	EMCLUB_NEW_FB_NOTMASTER		= 3,	// ��Ƽ �����͸� ��û ����.
	EMCLUB_NEW_FB_NOTMEMBER		= 4,	// ��Ƽ�� ����.
	EMCLUB_NEW_FB_NOTEXIST_MEM	= 5,	// ��Ƽ���� ��ó�� ����.
	EMCLUB_NEW_FB_MEM_ALREADY	= 6,	// �̹� �ٸ� Ŭ���� ���ԵǾ� ����.
	EMCLUB_NEW_FB_MASTER_LVL	= 7,	// �������� ���� ����.
	EMCLUB_NEW_FB_AVER_LIVING	= 8,	// ��� ��Ȱ���� ����.
	EMCLUB_NEW_FB_NOTMONEY		= 9,	// ��û��� ����.
	EMCLUB_NEW_FB_NAME			= 10,	// Ŭ�� �̸� ����.
	EMCLUB_NEW_FB_BADNAME		= 11,	// Ŭ�� �̸� ����.
	EMCLUB_NEW_FB_OTHERSCHOOL	= 12,	// �ٸ� �п����� Ŭ�� �Ἲ ����.
	EMCLUB_NEW_FB_THAICHAR_ERROR = 13,  // �±��� ���� ���� ����.
	EMCLUB_NEW_FB_VNCHAR_ERROR = 14,	// ��Ʈ�� ���� ���� ����.
};

enum EMCLUB_DIS_FB
{
	EMCLUB_DIS_FB_FAIL			= 0,	// �Ϲݿ���.
	EMCLUB_DIS_FB_OK			= 1,	// �ػ� ����.
	EMCLUB_DIS_FB_CANCEL		= 2,	// �ػ� ���.
	EMCLUB_DIS_FB_CLUBBATTLE	= 3,	// Ŭ����Ʋ���̶� ���� ����
};

enum EMCLUB_MEMBER_REQ_FB
{
	EMCLUB_MEMBER_REQ_FB_FAIL			= 0,	// �Ϲݿ���.
	EMCLUB_MEMBER_REQ_FB_OK				= 1,	// ����.
	EMCLUB_MEMBER_REQ_FB_REFUSE			= 2,	// ������ ����.
	EMCLUB_MEMBER_REQ_FB_ALREADY		= 3,	// �̹� ���ԵǾ� �ִ� ���.
	EMCLUB_MEMBER_REQ_FB_MAXNUM			= 4,	// �ִ� ���Լ�.
	EMCLUB_MEMBER_REQ_FB_SECEDED		= 5,	// ����Ż���� ����̿��� ���� ���� �ð� ������.
	EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL	= 6,	// �ٸ� �п����� Ŭ���� ���� ����.
	EMCLUB_MEMBER_REQ_FB_CLUBBATTLE		= 7,	// ��Ʋ�߿��� ����� ������ ����.
};

enum EMCLUB_MEMBER_DEL_FB
{
	EMCLUB_MEMBER_DEL_FB_FAIL	= 0,	//	�Ϲݿ���.
	EMCLUB_MEMBER_DEL_FB_OK		= 1,	//	����.
};

enum EMCLUB_AUTHORITY_REQ_FB
{
	EMCLUB_AUTHORITY_REQ_FB_FAIL		= 0,	//	�Ϲݿ���
	EMCLUB_AUTHORITY_REQ_FB_OK			= 1,	//	����
	EMCLUB_AUTHORITY_REQ_FB_REFUSE		= 2,	//	����
	EMCLUB_AUTHORITY_REQ_FB_NOMASTER	= 3,	//	�����Ͱ� �ƴ�
	EMCLUB_AUTHORITY_REQ_FB_NOONLINE	= 4,	//	��� ���� ���� ������
	EMCLUB_AUTHORITY_REQ_FB_ALLIANCE	= 5,	//	���� ������
	EMCLUB_AUTHORITY_REQ_FB_CLUBBATTLE	= 6,	//	Ŭ����Ʋ ��
	EMCLUB_AUTHORITY_REQ_FB_CONFING		= 7,	//	�����
	EMCLUB_AUTHORITY_REQ_FB_TIME		= 8,	//	�ð�����
	EMCLUB_AUTHORITY_REQ_FB_GUIDNBATTLE	= 9,	//	��������
};

enum EMCLUB_MARK_INFO_FB
{
	EMCLUB_MARK_INFO_FB_FAIL	= 0,	//	�Ϲݿ���.
	EMCLUB_MARK_INFO_FB_OK		= 1,	//	����.
};

enum EMCLUB_MARK_CHANGE_FB
{
	EMCLUB_MARK_CHANGE_FB_FAIL	= 0,	//	�Ϲ� ����.
	EMCLUB_MARK_CHANGE_FB_OK	= 1,	//	����.
};

enum EMCLUB_RANK_FB
{
	EMCLUB_RANK_FB_FAIL			= 0,	// �Ϲ� ����.
	EMCLUB_RANK_FB_OK			= 1,	// ����.
	EMCLUB_RANK_FB_MAX			= 2,	// �ְ� ��ũ ����.
	EMCLUB_RANK_FB_MASTER_LVL	= 3,	// �������� ���� ����.
	EMCLUB_RANK_FB_AVER_LIVING	= 4,	// ��� ��Ȱ���� ����.
	EMCLUB_RANK_FB_NOTMONEY		= 5,	// ��û��� ����.
};

enum EMCLUB_MEMBER_NICK_FB
{
	EMCLUB_MEMBER_NICK_FB_FAIL			= 0,	// �Ϲ� ����.
	EMCLUB_MEMBER_NICK_FB_OK			= 1,	// ����.
	EMCLUB_MEMBER_NICK_FB_BADNAME		= 2,	// �������� �̸�.
	EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR = 3,	// �±���� ���� ����
	EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR	= 4,
};

enum EMCLUB_MEMBER_SECEDE_FB
{
	EMCLUB_MEMBER_SECEDE_FB_FAIL	= 0,	//	�Ϲ� ����.
	EMCLUB_MEMBER_SECEDE_FB_OK		= 1,	//	����.
};

enum
{
	EMMAXCLUBMEMBER_NEW = 8
};

enum EMCLUB_NOTICE_FB
{
	EMCLUB_NOTICE_FB_FAIL			= 0,	//	�Ϲݿ���.
	EMCLUB_NOTICE_FB_OK				= 1,	//	����.
	EMCLUB_NOTICE_FB_NOTMATER		= 2,	//	Ŭ�� �����Ͱ� �ƴ�.
};

enum EMCDCERTIFY
{
	EMCDCERTIFY_FAIL			= 0,	//	�Ϲݿ���.
	EMCDCERTIFY_OK				= 1,	//	���� ����.
	EMCDCERTIFY_BADNPC			= 2,	//	npc ����.
	EMCDCERTIFY_NOTMASTER		= 3,	//	Ŭ�� �����Ͱ� �ƴ�.
	EMCDCERTIFY_AREADYCERTIFY	= 4,	//	�̹� ������.
	EMCDCERTIFY_OTHERCERTIFING	= 5,	//	�̹� �ٸ� ����� ������.
	EMCDCERTIFY_AREADYGUID		= 6,	//	�̹� �ٸ� ���� ���� ����.
	EMCDCERTIFY_NOTTIME			= 7,	//	������ ������ �ð��� �ƴ�.
	EMCDCERTIFY_DISTANCE		= 8,	//	�Ÿ��� �ָ� ����������.
	EMCDCERTIFY_NOTITEM			= 9,	//	������ CD�� ����.
	EMCDCERTIFY_NOTCONDITION	= 10,	//	npc ����.
	EMCDCERTIFY_AREADY			= 11,	//	�̹� ���� �Ǿ� �ֽ��ϴ�.
	EMCDCERTIFY_HAVEGUIDCLUB	= 12,	// ����Ŭ���߿� ����Ŭ���� ����

	EMCDCERTIFY_ING_TIMEOUT		= 13,	//	������ ���Ÿ� �ʰ�.
	EMCDCERTIFY_ING_DISTANCE	= 14,	//	������ ���Ÿ� �ʰ�.
	EMCDCERTIFY_ING_DIE			= 15,	//	���.

	EMCDCERTIFY_COMPLETE		= 16,	//	���� �Ϸ�.
};

enum EMGUIDCOMMISSION_FB
{
	EMGUIDCOMMISSION_FB_FAIL	= 0,	//	�Ϲݿ���.
	EMGUIDCOMMISSION_FB_OK		= 1,	//	����.
	EMGUIDCOMMISSION_FB_RANGE	= 2,	//	��뿵�� �ʰ�.
	EMGUIDCOMMISSION_FB_NOTCLUB	= 3,	//	���� Ŭ�� �����Ͱ� �ƴ�.
	EMGUIDCOMMISSION_FB_ALREADY	= 4,	//	�̹� ����Ǿ� ����.
	EMGUIDCOMMISSION_FB_BATTLE	= 5,	//	������ �����߿��� ������ ������ �� �����ϴ�.
};


enum EMCLUBSUBMASTER_FB
{
	EMCLUBSUBMASTER_FB_FAIL		= 0,
	EMCLUBSUBMASTER_FB_OK		= 1,
	EMCLUBSUBMASTER_FB_NOTMASTER= 2,
	EMCLUBSUBMASTER_FB_RESET	= 3,
	EMCLUBSUBMASTER_FB_BATTLE	= 4,
	EMCLUBSUBMASTER_FB_CDM		= 5,
	EMCLUBSUBMASTER_FB_CDM_MEM	= 6,	//	CDM ����� �ʰ�
};

// ���Ͱ���/�Ἲ
enum EMCLUB_ALLIANCE_REQ_FB
{
	EMCLUB_ALLIANCE_REQ_FB_FAIL			= 0,
	EMCLUB_ALLIANCE_REQ_FB_OK			= 1,
	EMCLUB_ALLIANCE_REQ_FB_NOTMASTER	= 2,
	EMCLUB_ALLIANCE_REQ_FB_NOTCHIEF		= 3,
	EMCLUB_ALLIANCE_REQ_FB_TARNOTMASTER	= 4,
	EMCLUB_ALLIANCE_REQ_FB_ALREADY		= 5,
	EMCLUB_ALLIANCE_REQ_FB_MAX			= 6,
	EMCLUB_ALLIANCE_REQ_FB_REFUSE		= 7,
	EMCLUB_ALLIANCE_REQ_FB_DISTIME		= 8,
	EMCLUB_ALLIANCE_REQ_FB_SECTIME		= 9,
	EMCLUB_ALLIANCE_REQ_FB_TOSECTIME	= 10,
	EMCLUB_ALLIANCE_REQ_FB_OTHERSCHOOL	= 11,
	EMCLUB_ALLIANCE_REQ_FB_BOTHGUID		= 12,
	EMCLUB_ALLIANCE_REQ_FB_GUIDMAP		= 13,
	EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION	= 14,	// ��û�ڰ� ��ü�����϶�
	EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION2	= 15,	// ������ ��ü�����϶�
	EMCLUB_ALLIANCE_REQ_FB_CLUBBATTLE		= 16,	// Ŭ�� ��Ʋ�� �������϶� ( �� )
	EMCLUB_ALLIANCE_REQ_FB_TARCLUBBATTLE	= 17,	// Ŭ�� ��Ʋ�� �������϶� ( ��� )
};

// Ŭ����Ʋ
enum EMCLUB_BATTLE_REQ_FB
{
	EMCLUB_BATTLE_REQ_FB_FAIL			= 0,	// �Ϲ����� ����
	EMCLUB_BATTLE_REQ_FB_OK				= 1,
	EMCLUB_BATTLE_REQ_FB_NOTMASTER		= 2,	// �����Ͱ� �ƴ�	
	EMCLUB_BATTLE_REQ_FB_TARNOTMASTER	= 3,	// ������ �����Ͱ� �ƴ�
	EMCLUB_BATTLE_REQ_FB_ALREADY		= 4,	// �̹� Ŭ����Ʋ ��	
	EMCLUB_BATTLE_REQ_FB_ALREADY2		= 5,	// �̹� Ŭ����Ʋ ��	
	EMCLUB_BATTLE_REQ_FB_MAX			= 6,	// �������� ���� �ʰ�
	EMCLUB_BATTLE_REQ_FB_TARMAX			= 7,	// ����� �������� ���� �ʰ�
	EMCLUB_BATTLE_REQ_FB_REFUSE			= 8,	// ����� ����
	EMCLUB_BATTLE_REQ_FB_GUIDMAP		= 9,	// ������ �ʿ��� �ȵ�
	EMCLUB_BATTLE_REQ_FB_DISSOLUTION	= 10,	// ��û�ڰ� ��ü�����϶�
	EMCLUB_BATTLE_REQ_FB_DISSOLUTION2	= 11,	// ������ ��ü�����϶�
	EMCLUB_BATTLE_REQ_FB_ALLIANCE		= 12,	// ���� ���� �Ҽ��϶�
	EMCLUB_BATTLE_REQ_FB_RANK			= 13,	// Ŭ����ũ �̴�
	EMCLUB_BATTLE_REQ_FB_TARRANK		= 14,	// ��� Ŭ����ũ �̴�
	EMCLUB_BATTLE_REQ_FB_CONFT			= 15,	// �����
	EMCLUB_BATTLE_REQ_FB_TARCONFT		= 16,	// ��� �����
	EMCLUB_BATTLE_REQ_FB_GUIDNBATTLE	= 17,	// ������ �ð��߿��� �����Ҽ� ����.
	EMCLUB_BATTLE_REQ_FB_TIMEMIN		= 18,	// ����ð��� ����
	EMCLUB_BATTLE_REQ_FB_TIMEMAX		= 19,	// ����ð��� ��
	EMCLUB_BATTLE_REQ_FB_CDMMAP			= 20,	// Ŭ��������ġ�������� �ȉ�
};

// Ŭ����Ʋ
enum EMALLIANCE_BATTLE_REQ_FB
{
	EMALLIANCE_BATTLE_REQ_FB_FAIL			= 0,	// �Ϲ����� ����
	EMALLIANCE_BATTLE_REQ_FB_OK				= 1,
	EMALLIANCE_BATTLE_REQ_FB_NOTMASTER		= 2,	// �����Ͱ� �ƴ�	
	EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER	= 3,	// ������ �����Ͱ� �ƴ�
	EMALLIANCE_BATTLE_REQ_FB_ALREADY		= 4,	// �̹� Ŭ����Ʋ ��	
	EMALLIANCE_BATTLE_REQ_FB_ALREADY2		= 5,	// �̹� ���͹�Ʋ ��	
	EMALLIANCE_BATTLE_REQ_FB_MAX			= 6,	// �������� ���� �ʰ�
	EMALLIANCE_BATTLE_REQ_FB_TARMAX			= 7,	// ����� �������� ���� �ʰ�
	EMALLIANCE_BATTLE_REQ_FB_REFUSE			= 8,	// ����� ����
	EMALLIANCE_BATTLE_REQ_FB_GUIDMAP		= 9,	// ������ �ʿ��� �ȵ�
	EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION	= 10,	// ��û�ڰ� ��ü�����϶�
	EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION2	= 11,	// ������ ��ü�����϶�
	EMALLIANCE_BATTLE_REQ_FB_ALLIANCE		= 12,	// ���� ���� �Ҽ��϶�
	EMALLIANCE_BATTLE_REQ_FB_RANK			= 13,	// Ŭ����ũ �̴�
	EMALLIANCE_BATTLE_REQ_FB_TARRANK		= 14,	// ��� Ŭ����ũ �̴�
	EMALLIANCE_BATTLE_REQ_FB_CONFT			= 15,	// �����
	EMALLIANCE_BATTLE_REQ_FB_TARCONFT		= 16,	// ��� �����
	EMALLIANCE_BATTLE_REQ_FB_GUIDNBATTLE	= 17,	// ������ �ð��߿��� �����Ҽ� ����.
	EMALLIANCE_BATTLE_REQ_FB_TIMEMIN		= 18,	// ����ð��� ����
	EMALLIANCE_BATTLE_REQ_FB_TIMEMAX		= 19,	// ����ð��� ��
	EMALLIANCE_BATTLE_REQ_FB_CDMMAP			= 20,	// ������ �ʿ��� �ȵ�
};

// Ŭ�� ��Ʋ ����
enum EMCLUB_BATTLE_ARMISTICE_REQ_FB
{
	EMCLUB_BATTLE_ARMISTICE_FB_FAIL			= 0,	// �Ϲ����� ����
	EMCLUB_BATTLE_ARMISTICE_FB_OK			= 1,	// ����
	EMCLUB_BATTLE_ARMISTICE_FB_NOTMASTER	= 2,	// �����Ͱ� �ƴ�
	EMCLUB_BATTLE_ARMISTICE_FB_OFFMASTER	= 3,	// ��� �����Ͱ� �� ������
	EMCLUB_BATTLE_ARMISTICE_FB_REFUSE		= 4,	// ����
	EMCLUB_BATTLE_ARMISTICE_FB_GUIDMAP		= 5,	// ������ ��
	EMCLUB_BATTLE_ARMISTICE_FB_NOBATTLE		= 6,	// ��Ʋ���� �ƴ�
	EMCLUB_BATTLE_ARMISTICE_FB_DISTIME		= 7,	// ��Ʋ ���� �Ұ�
	EMCLUB_BATTLE_ARMISTICE_FB_CDMMAP		= 8,	// Ŭ��������ġ ��
};

// ���� ��Ʋ ����
enum EMALLIANCE_BATTLE_ARMISTICE_REQ_FB
{
	EMALLIANCE_BATTLE_ARMISTICE_FB_FAIL			= 0,	// �Ϲ����� ����
	EMALLIANCE_BATTLE_ARMISTICE_FB_OK			= 1,	// ����
	EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER	= 2,	// �����Ͱ� �ƴ�
	EMALLIANCE_BATTLE_ARMISTICE_FB_OFFMASTER	= 3,	// ��� �����Ͱ� �� ������
	EMALLIANCE_BATTLE_ARMISTICE_FB_REFUSE		= 4,	// ����
	EMALLIANCE_BATTLE_ARMISTICE_FB_GUIDMAP		= 5,	// ������ ��
	EMALLIANCE_BATTLE_ARMISTICE_FB_NOBATTLE		= 6,	// ��Ʋ���� �ƴ�
	EMALLIANCE_BATTLE_ARMISTICE_FB_DISTIME		= 7,	// ��Ʋ ���� �Ұ�
	EMALLIANCE_BATTLE_ARMISTICE_FB_CDMMAP		= 8,	// Ŭ��������ġ ��
};



// Ŭ�� ��Ʋ �׺�
enum EMCLUB_BATTLE_SUBMISSION_REQ_FB
{
	EMCLUB_BATTLE_SUBMISSION_FB_FAIL		= 0,	// �Ϲ����� ����
	EMCLUB_BATTLE_SUBMISSION_FB_OK			= 1,	// ����
	EMCLUB_BATTLE_SUBMISSION_FB_NOTMASTER	= 2,	// �����Ͱ� �ƴ�
	EMCLUB_BATTLE_SUBMISSION_FB_GUIDMAP		= 3,	// ������ ��
	EMCLUB_BATTLE_SUBMISSION_FB_NOBATTLE	= 4,	// ��Ʋ���� �ƴ�
	EMCLUB_BATTLE_SUBMISSION_FB_DISTIME		= 5,	// ��Ʋ ���� �Ұ�
	EMCLUB_BATTLE_SUBMISSION_FB_CDMMAP		= 6,	// CDM��
};

// ���� ��Ʋ �׺�
enum EMALLIANCE_BATTLE_SUBMISSION_REQ_FB
{
	EMALLIANCE_BATTLE_SUBMISSION_FB_FAIL		= 0,	// �Ϲ����� ����
	EMALLIANCE_BATTLE_SUBMISSION_FB_OK			= 1,	// ����
	EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER	= 2,	// �����Ͱ� �ƴ�
	EMALLIANCE_BATTLE_SUBMISSION_FB_GUIDMAP		= 3,	// ������ ��
	EMALLIANCE_BATTLE_SUBMISSION_FB_NOBATTLE	= 4,	// ��Ʋ���� �ƴ�
	EMALLIANCE_BATTLE_SUBMISSION_FB_DISTIME		= 5,	// ��Ʋ ���� �Ұ�
	EMALLIANCE_BATTLE_SUBMISSION_FB_CDMMAP		= 6,	// CDM��
};

// Ŭ�� ��Ʋ ����
enum EMCLUB_BATTLE_OVER_FB
{
	EMCLUB_BATTLE_OVER_DRAW					= 0,	// ���
	EMCLUB_BATTLE_OVER_WIN					= 1,	// �¸�
	EMCLUB_BATTLE_OVER_LOSE					= 2,	// �й�
	EMCLUB_BATTLE_OVER_ARMISTICE			= 3,	// ����
	EMCLUB_BATTLE_OVER_SUBMISSION			= 4,	// �׺�
	EMCLUB_BATTLE_OVER_TARSUBMISSION		= 5,	// �׺� �޴� ���
};


// ����Ŭ�� ����
enum EMCLUB_ALLIANCE_DEL_FB
{
	EMCLUB_ALLIANCE_DEL_FB_FAIL			= 0,
	EMCLUB_ALLIANCE_DEL_FB_OK			= 1,
	EMCLUB_ALLIANCE_DEL_FB_NOTMASTER	= 2,
	EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF		= 3,
	EMCLUB_ALLIANCE_DEL_FB_GUIDMAP		= 4,
	EMCLUB_ALLIANCE_DEL_FB_BATTLE		= 5,	//	��Ʋ���϶�.
};

// ���� Ż��
enum EMCLUB_ALLIANCE_SEC_FB
{
	EMCLUB_ALLIANCE_SEC_FB_FAIL			= 0,
	EMCLUB_ALLIANCE_SEC_FB_OK			= 1,
	EMCLUB_ALLIANCE_SEC_FB_NOTMASTER	= 2,
	EMCLUB_ALLIANCE_SEC_FB_ALLIANCE		= 3,
	EMCLUB_ALLIANCE_SEC_FB_GUIDMAP		= 4,
	EMCLUB_ALLIANCE_SEC_FB_BATTLE		= 5,	//	��Ʋ���϶�.
};

// ���� ��ü
enum EMCLUB_ALLIANCE_DIS_FB
{
	EMCLUB_ALLIANCE_DIS_FB_FAIL			= 0,
	EMCLUB_ALLIANCE_DIS_FB_OK			= 1,
	EMCLUB_ALLIANCE_DIS_FB_NOTMASTER	= 2,
	EMCLUB_ALLIANCE_DIS_FB_NOTCHIEF		= 3,
	EMCLUB_ALLIANCE_DIS_FB_GUIDMAP		= 4,
	EMCLUB_ALLIANCE_DIS_FB_BATTLE		= 5,	//	��Ʋ���϶�.
};

namespace GLMSG
{
	#pragma pack(1)

	struct SNET_CLUB_NEW
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwNpcID;
		char				szClubName[CHAR_SZNAME];

		SNET_CLUB_NEW () 
			: dwNpcID(0)
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_NEW);
			nmg.nType = NET_MSG_GCTRL_CLUB_NEW;
			memset (szClubName, 0, sizeof(char) * CHAR_SZNAME);
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_NEW_2AGT
	{
		NET_MSG_GENERIC		nmg;
		char				szClubName[CHAR_SZNAME];

		SNET_CLUB_NEW_2AGT ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_NEW_2AGT);
			nmg.nType = NET_MSG_GCTRL_CLUB_NEW_2AGT;
			memset (szClubName, 0, sizeof(char) * CHAR_SZNAME);
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_NEW_DB2AGT
	{
		NET_MSG_GENERIC		nmg;
		EMCLUB_NEW_FB		emFB;

		DWORD				dwMasterID;
		DWORD				dwClubID;
		char				szClubName[CHAR_SZNAME];

		BYTE				buffer [ NET_DATA_BUFSIZE-(sizeof(NET_MSG_GENERIC)+sizeof(EMCLUB_NEW_FB)+sizeof(DWORD)+sizeof(DWORD)+sizeof(char)*CHAR_SZNAME) ];

		SNET_CLUB_NEW_DB2AGT () 
			: emFB(EMCLUB_NEW_FB_FAIL)
			, dwMasterID(0)
			, dwClubID(0)
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_NEW_DB2AGT);
			nmg.nType = NET_MSG_GCTRL_CLUB_NEW_DB2AGT;
			memset (szClubName, 0, sizeof(char) * CHAR_SZNAME);			
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_NEW_2FLD
	{
		NET_MSG_GENERIC		nmg;
		EMCLUB_NEW_FB		emFB;

		DWORD				dwCLUBID;
		char				szClubName[CHAR_SZNAME];

		SNET_CLUB_NEW_2FLD () 
			: emFB(EMCLUB_NEW_FB_FAIL)
			, dwCLUBID(0)
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_NEW_2FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_NEW_2FLD;
			memset (szClubName, 0, sizeof(char) * CHAR_SZNAME);
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_NEW_FB
	{
		NET_MSG_GENERIC		nmg;
		EMCLUB_NEW_FB		emFB;
		char				szClubName[CHAR_SZNAME];

		SNET_CLUB_NEW_FB () :
			emFB(EMCLUB_NEW_FB_FAIL)
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_NEW_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_NEW_FB;
			memset (szClubName, 0, sizeof(char) * CHAR_SZNAME);
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_INFO_2FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;
		char				szClubName[CHAR_SZNAME];

		DWORD				dwMasterID;
		char				szMasterName[CHAR_SZNAME];
		DWORD				dwCDCertifior;

		DWORD				dwRank;
		DWORD				dwMarkVER;

		__time64_t			tOrganize;
		__time64_t			tDissolution;

		DWORD				dwMemberNum;
		DWORD				aMembers[EMMAXCLUBMEMBER_NEW];
		char				szMemberName[EMMAXCLUBMEMBER_NEW][CHAR_SZNAME];

		SNET_CLUB_INFO_2FLD () 
			: dwClubID(0)
			, dwMasterID(0)
			, dwCDCertifior(0)
			, tOrganize(0)
			, tDissolution(0)
			, dwRank(0)
			, dwMarkVER(0)
			, dwMemberNum(0)
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_INFO_2FLD);			
			nmg.nType = NET_MSG_GCTRL_CLUB_INFO_2FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
			
			memset (szClubName, 0, sizeof(char) * CHAR_SZNAME);
			memset (szMasterName, 0, sizeof(char) * CHAR_SZNAME);
			memset (aMembers, 0, sizeof(DWORD) * EMMAXCLUBMEMBER_NEW );			
			
			for (int i=0; i<EMMAXCLUBMEMBER_NEW; ++i )
			{
				memset (szMemberName[i], 0, sizeof(char) * CHAR_SZNAME);
			}
		}
	};

	struct SNET_CLUB_INFO_2CLT
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;
		char				szClubName[CHAR_SZNAME];
		char				szNickName[CHAR_SZNAME];

		DWORD				dwMasterID;
		char				szMasterName[CHAR_SZNAME];
		DWORD				dwCDCertifior;

		DWORD				dwRank;
		DWORD				dwMarkVER;

		__time64_t			tOrganize;
		__time64_t			tDissolution;

		char				szNotice[EMCLUB_NOTICE_LEN+1];

		SNET_CLUB_INFO_2CLT () 
			: dwClubID(0)
			, dwMasterID(0)
			, dwCDCertifior(0)
			, dwRank(0)
			, dwMarkVER(0)
			, tOrganize(0)
			, tDissolution(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_INFO_2CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_INFO_2CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset (szClubName, 0, sizeof(char) * CHAR_SZNAME);
			memset (szNickName, 0, sizeof(char) * CHAR_SZNAME);
			memset (szMasterName, 0, sizeof(char) * CHAR_SZNAME);
			memset (szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN+1));
		}
	};

	struct SNET_CLUB_INFO_DISSOLUTION
	{
		NET_MSG_GENERIC		nmg;
		__time64_t			tDissolution;

		SNET_CLUB_INFO_DISSOLUTION () 
			: tDissolution( 0 ) 
		{
			nmg.dwSize = sizeof(SNET_CLUB_INFO_DISSOLUTION);
			nmg.nType = NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SCLUBMEMBER
	{
		DWORD dwID;
		DWORD dwFlag;
		char szMember[CHAR_SZNAME];

		SCLUBMEMBER () 
			: dwID(0)
			, dwFlag(NULL)
		{
			memset (szMember, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_MEMBER_2CLT
	{
		enum { EMMAXMEMBER = 10 };

		NET_MSG_GENERIC		nmg;
		DWORD				dwMemberNum;

		SCLUBMEMBER			sMEMBER[EMMAXMEMBER];

		SNET_CLUB_MEMBER_2CLT () 
			: dwMemberNum(0)
		{
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_2CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		bool ADDMEMBER ( DWORD dwCharID, DWORD dwFlag, const char *szMember )
		{
			if ( EMMAXMEMBER==dwMemberNum )		return false;

			sMEMBER[dwMemberNum].dwID = dwCharID;
			sMEMBER[dwMemberNum].dwFlag = dwFlag;
			StringCchCopy ( sMEMBER[dwMemberNum].szMember, CHAR_SZNAME, szMember );

			++dwMemberNum;

			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(SCLUBMEMBER)*dwMemberNum;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
			return true;
		}

		void RESET ()
		{
			dwMemberNum = 0;
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD);
		}
	};

	struct SNET_CLUB_INFO_BRD : public SNETPC_BROAD
	{
		DWORD				dwClubID;
		DWORD				dwMarkVer;
		char				szNickName[CHAR_SZNAME];
	
		SNET_CLUB_INFO_BRD () 
			: dwClubID(0)
			, dwMarkVer(0)
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_INFO_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_INFO_BRD;
			memset (szNickName, 0, sizeof(char) * CHAR_SZNAME);
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_INFO_MARK_BRD : public SNETPC_BROAD
	{
		DWORD				dwMarkVer;
	
		SNET_CLUB_INFO_MARK_BRD () 
			: dwMarkVer(0)
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_INFO_MARK_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_INFO_MARK_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_INFO_NICK_BRD : public SNETPC_BROAD
	{
		char				szNickName[CHAR_SZNAME];
	
		SNET_CLUB_INFO_NICK_BRD () 
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_INFO_NICK_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_INFO_NICK_BRD;
			memset (szNickName, 0, sizeof(char) * CHAR_SZNAME);
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DISSOLUTION
	{
		NET_MSG_GENERIC		nmg;
		bool				bCANCEL;

		SNET_CLUB_DISSOLUTION () 
			: bCANCEL(false)
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_DISSOLUTION);
			nmg.nType = NET_MSG_GCTRL_CLUB_DISSOLUTION;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DISSOLUTION_2FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;
		__time64_t			tDissolution;

		SNET_CLUB_DISSOLUTION_2FLD () 
			: dwClubID(0)
			, tDissolution(0)
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_DISSOLUTION_2FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_DISSOLUTION_2FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DISSOLUTION_FB
	{
		NET_MSG_GENERIC		nmg;

		EMCLUB_DIS_FB		emFB;
		__time64_t			tDissolution;

		SNET_CLUB_DISSOLUTION_FB () 
			: emFB(EMCLUB_DIS_FB_FAIL)
			, tDissolution(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_DISSOLUTION_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_DISSOLUTION_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEL_2FLD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwClubID;

		SNET_CLUB_DEL_2FLD () :
			dwClubID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEL_2FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEL_2CLT 
	{
		NET_MSG_GENERIC		nmg;

		SNET_CLUB_DEL_2CLT ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEL_2CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEL_BRD : public SNETPC_BROAD
	{
		SNET_CLUB_DEL_BRD ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEL_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETLOBBY_CLUB_INFO
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;
		char				szClubName[CHAR_SZNAME];
		DWORD				dwRank;

		DWORD				dwMarkVER;

		DWORD				dwMasterID;
		char				szMasterName[CHAR_SZNAME];
		DWORD				dwCDCertifior;

		__time64_t			tOrganize;
		__time64_t			tDissolution;

		DWORD				dwAlliance;

		DWORD				dwMEMBER_NUM;
		DWORD				dwALLIANCE_NUM;
		DWORD				dwBATTLE_NUM;

		char				szNotice[EMCLUB_NOTICE_LEN+1];

		DWORD				dwBattleWin;
		DWORD				dwBattleLose;
		DWORD				dwBattleDraw;
		
		DWORD				dwAllianceBattleWin;
		DWORD				dwAllianceBattleLose;
		DWORD				dwAllianceBattleDraw;

		SNETLOBBY_CLUB_INFO () 
			: dwClubID(0)
			, dwMasterID(0)
			, dwRank(0)
			
			, dwMarkVER(0)
			
			, dwCDCertifior(0)
			
			, tOrganize(0)
			, tDissolution(0)
			
			, dwAlliance(0)
			
			, dwMEMBER_NUM(0)
			, dwALLIANCE_NUM(0)
			, dwBATTLE_NUM(0)
			, dwBattleWin(0)
			, dwBattleLose(0)
			, dwBattleDraw(0)
			, dwAllianceBattleWin(0)
			, dwAllianceBattleLose(0)
			, dwAllianceBattleDraw(0)
		{
			nmg.dwSize = sizeof(SNETLOBBY_CLUB_INFO);
			nmg.nType = NET_MSG_LOBBY_CLUB_INFO;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szClubName, 0, sizeof(char) * (CHAR_SZNAME));
			memset(szMasterName, 0, sizeof(char) * (CHAR_SZNAME));
			memset(szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN+1));
		}
	};

	struct SNETLOBBY_CLUB_MEMBER
	{
		enum { EMMAXMEMBER = 10 };

		NET_MSG_GENERIC		nmg;
		DWORD				dwMemberNum;

		SCLUBMEMBER			sMEMBER[EMMAXMEMBER];

		SNETLOBBY_CLUB_MEMBER () :
			dwMemberNum(0)
		{
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD);
			nmg.nType = NET_MSG_LOBBY_CLUB_MEMBER;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		bool ADDMEMBER ( DWORD dwCharID, DWORD dwFlags, const char *szMember )
		{
			if ( EMMAXMEMBER==dwMemberNum )		return false;

			sMEMBER[dwMemberNum].dwID = dwCharID;
			sMEMBER[dwMemberNum].dwFlag = dwFlags;
			StringCchCopy ( sMEMBER[dwMemberNum].szMember, CHAR_SZNAME, szMember );

			++dwMemberNum;

			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(SCLUBMEMBER)*dwMemberNum;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
			return true;
		}

		void RESET ()
		{
			dwMemberNum = 0;
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD);
		}
	};

	struct GLALLIANCE
	{
		DWORD		m_dwID;
		char		m_szName[CHAR_SZNAME];

		GLALLIANCE() :
			m_dwID(CLUB_NULL)
		{
			memset( m_szName, 0, sizeof(char) * CHAR_SZNAME );			
		}

		GLALLIANCE ( DWORD dwID, const char *szName )
		{
			m_dwID = dwID;
			StringCchCopy ( m_szName, CHAR_SZNAME, szName );
		}
	};

	struct SNETLOBY_CLUB_ANCE_2CLT
	{
		enum { EMMAX = 20 };

		NET_MSG_GENERIC		nmg;
		DWORD				dwAllianceNum;

		GLALLIANCE			sALLIANCE[EMMAX];

		SNETLOBY_CLUB_ANCE_2CLT () :
			dwAllianceNum(0)
		{		
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(GLALLIANCE)*dwAllianceNum;
			nmg.nType = NET_MSG_LOBBY_CLUB_ALLIANCE;
			memset(sALLIANCE, 0, sizeof(GLALLIANCE) * SNETLOBY_CLUB_ANCE_2CLT::EMMAX);
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		bool ADDALLIANCE ( DWORD dwClubID, const char* szName )
		{
			if ( dwAllianceNum >= EMMAX || szName == NULL)
			{
				return false;
			}
			else
			{
				sALLIANCE[dwAllianceNum] = GLALLIANCE( dwClubID, szName );
				++dwAllianceNum;
				nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(GLALLIANCE)*dwAllianceNum;
				GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
				return true;
			}
		}

		void RESET ()
		{
			dwAllianceNum = 0;
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(GLALLIANCE)*dwAllianceNum;
			memset(sALLIANCE, 0, sizeof(GLALLIANCE) * SNETLOBY_CLUB_ANCE_2CLT::EMMAX);
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_REQ
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwGaeaID;

		SNET_CLUB_MEMBER_REQ () :
			dwGaeaID(GAEAID_NULL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_REQ_ASK
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwMaster;
		char				szClubName[CHAR_SZNAME];
		char				szMasterName[CHAR_SZNAME];

		SNET_CLUB_MEMBER_REQ_ASK () 
			: dwMaster(GAEAID_NULL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_REQ_ASK);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_REQ_ASK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(	szClubName,   0, sizeof(char) * CHAR_SZNAME );
			memset( szMasterName, 0, sizeof(char) * CHAR_SZNAME );
		}
	};

	struct SNET_CLUB_MEMBER_REQ_ANS
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwMaster;
		bool				bOK;

		SNET_CLUB_MEMBER_REQ_ANS () 
			: dwMaster(GAEAID_NULL)
			, bOK(false)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_REQ_ANS);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_REQ_ANS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_REQ_2AGT
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwReqGID;

		SNET_CLUB_MEMBER_REQ_2AGT () 
			: dwReqGID(GAEAID_NULL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_REQ_2AGT);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_REQ_2AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_REQ_FB
	{
		NET_MSG_GENERIC			nmg;
		char					szReqName[CHAR_SZNAME];
		EMCLUB_MEMBER_REQ_FB	emFB;

		
		SNET_CLUB_MEMBER_REQ_FB () 
			: emFB(EMCLUB_MEMBER_REQ_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_REQ_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_REQ_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szReqName, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_MEMBER_ADD_2FLD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwClubID;
		DWORD				dwMember;
		char				szMember[CHAR_SZNAME];		

		SNET_CLUB_MEMBER_ADD_2FLD () 
			: dwClubID(0)
			, dwMember(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_ADD_2FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_ADD_2FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szMember, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_MEMBER_DB2DEL
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwClub;
		DWORD				dwMember;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(DWORD), };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNET_CLUB_MEMBER_DB2DEL () :
			dwClub(0),
			dwMember(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_DB2DEL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_DEL
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwMember;

		SNET_CLUB_MEMBER_DEL () 
			: dwMember(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_DEL);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_DEL;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_DEL_2FLD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwClub;
		DWORD				dwMember;
		__time64_t			tSECEDE;


		SNET_CLUB_MEMBER_DEL_2FLD () 
			: dwClub(0)
			, dwMember(0)
			, tSECEDE(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_DEL_2FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_DEL_2FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_DEL_2CLT
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwClub;
		DWORD				dwMember;

		SNET_CLUB_MEMBER_DEL_2CLT () 
			: dwClub(0)
			, dwMember(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_DEL_2CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_DEL_2CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_DEL_FB
	{
		NET_MSG_GENERIC			nmg;
		EMCLUB_MEMBER_DEL_FB	emFB;

		SNET_CLUB_MEMBER_DEL_FB () 
			: emFB(EMCLUB_MEMBER_DEL_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_DEL_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_DEL_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_AUTHORITY_REQ
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwMember;

		SNET_CLUB_AUTHORITY_REQ () 
			: dwMember(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_AUTHORITY_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_AUTHORITY_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_AUTHORITY_REQ_FB
	{
		NET_MSG_GENERIC			nmg;
		EMCLUB_AUTHORITY_REQ_FB	emFB;
		DWORD					dwCharID;
		__time64_t				tAuthority;

		SNET_CLUB_AUTHORITY_REQ_FB () 
			: emFB(EMCLUB_AUTHORITY_REQ_FB_FAIL)
			, dwCharID( GAEAID_NULL ) 
			, tAuthority( 0 )
		{
			nmg.dwSize = sizeof(SNET_CLUB_AUTHORITY_REQ_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_AUTHORITY_REQ_ASK
	{
		NET_MSG_GENERIC		nmg;

		SNET_CLUB_AUTHORITY_REQ_ASK () 		
		{
			nmg.dwSize = sizeof(SNET_CLUB_AUTHORITY_REQ_ASK);
			nmg.nType = NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_AUTHORITY_REQ_ANS
	{
		NET_MSG_GENERIC		nmg;
		bool				bOK;

		SNET_CLUB_AUTHORITY_REQ_ANS () 
			: bOK(false)
		{
			nmg.dwSize = sizeof(SNET_CLUB_AUTHORITY_REQ_ANS);
			nmg.nType = NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ANS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};	

	struct SNET_CLUB_AUTHORITY_CLT
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwCharID;

		SNET_CLUB_AUTHORITY_CLT () 
			: dwCharID( GAEAID_NULL ) 
		{
			nmg.dwSize = sizeof(SNET_CLUB_AUTHORITY_CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_AUTHORITY_CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct 	SNET_CLUB_AUTHORITY_BRD : public SNETPC_BROAD
	{
		DWORD				dwCharID;

		SNET_CLUB_AUTHORITY_BRD () 
			: dwCharID( GAEAID_NULL ) 
		{
			nmg.dwSize = sizeof(SNET_CLUB_AUTHORITY_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_AUTHORITY_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};



	struct SNET_CLUB_AUTHORITY_FLD
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwClubID;
		DWORD				dwCharID;

		SNET_CLUB_AUTHORITY_FLD () 
			: dwClubID( CLUB_NULL )
			, dwCharID( GAEAID_NULL ) 
		{
			nmg.dwSize = sizeof(SNET_CLUB_AUTHORITY_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_AUTHORITY_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MARK_INFO
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwClubID;

		SNET_CLUB_MARK_INFO ()
			: dwClubID (0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MARK_INFO);
			nmg.nType = NET_MSG_GCTRL_CLUB_MARK_INFO;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MARK_INFO_FB
	{
		NET_MSG_GENERIC			nmg;

		EMCLUB_MARK_INFO_FB		emFB;
		DWORD					dwClubID;
		DWORD					dwMarkVER;
		DWORD					aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];

		SNET_CLUB_MARK_INFO_FB () 
			: emFB(EMCLUB_MARK_INFO_FB_FAIL)
			, dwClubID(0)
			, dwMarkVER(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MARK_INFO_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_MARK_INFO_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(aryMark, 0, sizeof(DWORD) * (EMCLUB_MARK_SX*EMCLUB_MARK_SY));
		}
	};

	struct SNET_CLUB_MARK_CHANGE
	{
		NET_MSG_GENERIC			nmg;
		DWORD					aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];

		SNET_CLUB_MARK_CHANGE ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_MARK_CHANGE);
			nmg.nType = NET_MSG_GCTRL_CLUB_MARK_CHANGE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(aryMark, 0, sizeof(DWORD) * (EMCLUB_MARK_SX*EMCLUB_MARK_SY));
		}
	};

	struct SNET_CLUB_MARK_CHANGE_2FLD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwClub;
		DWORD					dwMarkVER;
		DWORD					aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];

		SNET_CLUB_MARK_CHANGE_2FLD () 
			: dwClub(0)
			, dwMarkVER(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MARK_CHANGE_2FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_MARK_CHANGE_2FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(aryMark, 0, sizeof(DWORD) * (EMCLUB_MARK_SX*EMCLUB_MARK_SY));
		}
	};

	struct SNET_CLUB_MARK_CHANGE_2CLT
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwMarkVER;
		DWORD					aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];

		SNET_CLUB_MARK_CHANGE_2CLT () 
			: dwMarkVER(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MARK_CHANGE_2CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(aryMark, 0, sizeof(DWORD) * (EMCLUB_MARK_SX*EMCLUB_MARK_SY));
		}
	};

	struct SNET_CLUB_MARK_CHANGE_FB
	{
		NET_MSG_GENERIC			nmg;
		EMCLUB_MARK_CHANGE_FB	emFB;

		SNET_CLUB_MARK_CHANGE_FB () 
			: emFB(EMCLUB_MARK_CHANGE_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_MARK_CHANGE_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_MARK_CHANGE_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_RANK
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwNpcID;

		SNET_CLUB_RANK () 
			: dwNpcID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_RANK);
			nmg.nType = NET_MSG_GCTRL_CLUB_RANK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_RANK_2AGT
	{
		NET_MSG_GENERIC			nmg;

		SNET_CLUB_RANK_2AGT ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_RANK_2AGT);
			nmg.nType = NET_MSG_GCTRL_CLUB_RANK_2AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_RANK_2FLD
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwClubID;
		DWORD					dwRank;

		SNET_CLUB_RANK_2FLD () 
			: dwClubID(0)
			, dwRank(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_RANK_2FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_RANK_2CLT
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwRank;

		SNET_CLUB_RANK_2CLT () 
			: dwRank(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_RANK_2CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_RANK_2CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_RANK_FB
	{
		NET_MSG_GENERIC			nmg;
		EMCLUB_RANK_FB			emFB;
		DWORD					dwValue;

		SNET_CLUB_RANK_FB () 
			: emFB(EMCLUB_RANK_FB_FAIL)
			, dwValue(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_RANK_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_RANK_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_NICK
	{
		NET_MSG_GENERIC			nmg;
		char					szNick[CHAR_SZNAME];

		SNET_CLUB_MEMBER_NICK ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_NICK);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_NICK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szNick, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_MEMBER_NICK_FB
	{
		NET_MSG_GENERIC			nmg;
		EMCLUB_MEMBER_NICK_FB	emFB;
		char					szNick[CHAR_SZNAME];

		SNET_CLUB_MEMBER_NICK_FB () 
			: emFB(EMCLUB_MEMBER_NICK_FB_FAIL)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szNick, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SMEMSTATE
	{
		DWORD		dwCHARID;
		bool		bONLINE;
		int			nCHANNEL;
		SNATIVEID	nidMAP;

		SMEMSTATE () 
			: dwCHARID(0)
			, bONLINE(0)
			, nCHANNEL(0)
			, nidMAP(false)
		{
		}
	};

	struct SNET_CLUB_MEMBER_STATE
	{
		enum { EMMAX = 50, };
		NET_MSG_GENERIC			nmg;
		DWORD					dwNumber;
		SMEMSTATE				sSTATE[EMMAX];

		SNET_CLUB_MEMBER_STATE () 
			: dwNumber(0)
		{
			nmg.dwSize = GETSIZE();
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_STATE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(sSTATE, 0, sizeof(SMEMSTATE) * EMMAX);
		}

		int GETSIZE ()	{ return sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(SMEMSTATE)*dwNumber; }

		bool ADD ( DWORD dwCHARID, bool bONLINE, int nCHANNEL, SNATIVEID nidMAP )
		{
			if ( dwNumber>=EMMAX )	return false;

			sSTATE[dwNumber].dwCHARID = dwCHARID;
			sSTATE[dwNumber].bONLINE = bONLINE;
			sSTATE[dwNumber].nCHANNEL = nCHANNEL;
			sSTATE[dwNumber].nidMAP = nidMAP;
			++dwNumber;

			nmg.dwSize = GETSIZE();
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			return true;
		}

		void RESET ()
		{
			dwNumber = 0;
			nmg.dwSize = GETSIZE();
			memset(sSTATE, 0, sizeof(SMEMSTATE) * EMMAX);
		}
	};

	struct SMEMPOS
	{
		DWORD		dwCHARID;
		D3DXVECTOR2	vecPOS;

		SMEMPOS () 
			: dwCHARID(0)
			, vecPOS(0,0)
		{
		}
	};

	struct SNET_CLUB_MEMBER_POS
	{
		enum { EMMAX = 50, };
		NET_MSG_GENERIC			nmg;
		DWORD					dwNumber;
		SMEMPOS					sPOS[EMMAX];

		SNET_CLUB_MEMBER_POS () 
			: dwNumber(0)
		{
			nmg.dwSize = GETSIZE();
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_POS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(sPOS, 0, sizeof(SMEMPOS) * EMMAX);
		}

		int GETSIZE ()	{ return sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(SMEMPOS)*dwNumber; }

		bool ADD ( DWORD dwCHARID, D3DXVECTOR2 vecPOS )
		{
			if ( dwNumber>=EMMAX )	return false;

			sPOS[dwNumber].dwCHARID = dwCHARID;
			sPOS[dwNumber].vecPOS = vecPOS;
			++dwNumber;

			nmg.dwSize = GETSIZE();
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			return true;
		}

		void RESET ()
		{
			dwNumber = 0;
			nmg.dwSize = GETSIZE();
			memset(sPOS, 0, sizeof(SMEMPOS) * EMMAX);
		}
	};

	struct SNET_CLUB_MEMBER_SECEDE
	{
		NET_MSG_GENERIC			nmg;

		SNET_CLUB_MEMBER_SECEDE ()
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_SECEDE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_SECEDE_FB
	{
		NET_MSG_GENERIC			nmg;
		EMCLUB_MEMBER_SECEDE_FB	emFB;

		SNET_CLUB_MEMBER_SECEDE_FB () 
			: emFB(EMCLUB_MEMBER_SECEDE_FB_FAIL)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEATHMATCH_START_BRD
	{
		enum { TEXT_LEN = 33, };

		NET_MSG_GENERIC			nmg;
		
		//	0:��ý���, 10, 10���Ľ���, 30���Ľ���,60���Ŀ�����.
		int						nTIME;
		char					szName[TEXT_LEN];

		SNET_CLUB_DEATHMATCH_START_BRD () 
			: nTIME(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_DEATHMATCH_START_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_START_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szName, 0, sizeof(char) * TEXT_LEN);
		}
	};

	struct SNET_CLUB_DEATHMATCH_READY_FLD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwID;

		SNET_CLUB_DEATHMATCH_READY_FLD () 
			: dwID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_READY_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEATHMATCH_START_FLD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwID;

		SNET_CLUB_DEATHMATCH_START_FLD () 
			: dwID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_START_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEATHMATCH_END_FLD
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwID;

		SNET_CLUB_DEATHMATCH_END_FLD () 
			: dwID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_DEATHMATCH_END_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_END_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEATHMATCH_END_BRD
	{
		enum { TEXT_LEN = 33, };

		NET_MSG_GENERIC			nmg;
		char					szName[TEXT_LEN];
		char					szClubName[CHAR_SZNAME];

		SNET_CLUB_DEATHMATCH_END_BRD ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_DEATHMATCH_END_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_END_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szName, 0, sizeof(char) * TEXT_LEN);
			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_DEATHMATCH_REMAIN_BRD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwTime;

		SNET_CLUB_DEATHMATCH_REMAIN_BRD ()
			: dwTime(0)
		{
			nmg.dwSize = sizeof ( SNET_CLUB_DEATHMATCH_REMAIN_BRD );
			nmg.nType  = NET_MSG_GCTRL_CLUB_DEATHMATCH_REMAIN_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEATHMATCH_POINT_UPDATE
	{
		NET_MSG_GENERIC			nmg;
		
		bool					bKillPoint;

		SNET_CLUB_DEATHMATCH_POINT_UPDATE () 
			: bKillPoint(false)		
		{
			nmg.dwSize = sizeof(SNET_CLUB_DEATHMATCH_POINT_UPDATE);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_POINT_UPDATE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNET_CLUB_DEATHMATCH_MYRANK_UPDATE
	{
		NET_MSG_GENERIC			nmg;
		
		SCDM_RANK_EX			sMyCdmRank;		

		SNET_CLUB_DEATHMATCH_MYRANK_UPDATE () 
		{
			nmg.dwSize = sizeof(SNET_CLUB_DEATHMATCH_MYRANK_UPDATE);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_MYRANK_UPDATE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNET_CLUB_DEATHMATCH_RANKING_UPDATE
	{
//		enum { RANKING_MAX = 9 };

		NET_MSG_GENERIC		nmg;

        WORD				wRankNum;		
		SCDM_RANK			sCdmRank[RANKING_NUM];

		SNET_CLUB_DEATHMATCH_RANKING_UPDATE () 
			: wRankNum(0)
		{
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_UPDATE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		bool ADDCLUB ( const SCDM_RANK& sRank )
		{
			if ( RANKING_NUM==wRankNum )		return false;

			sCdmRank[wRankNum] = sRank;

			++wRankNum;

			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD) + sizeof(SCDM_RANK)*wRankNum;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
			return true;
		}

		void RESET ()
		{
			wRankNum = 0;
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD);
		}
	};

	struct SNET_CLUB_DEATHMATCH_RANKING_REQ
	{
		NET_MSG_GENERIC		nmg;    
		
		DWORD				dwMapID;		
		
		SNET_CLUB_DEATHMATCH_RANKING_REQ () 
			: dwMapID(UINT_MAX)		
		{
			nmg.dwSize = sizeof(SNET_CLUB_DEATHMATCH_RANKING_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}       

	};

	struct SNET_CLUB_BATTLE_START_BRD
	{
		enum { TEXT_LEN = 33, };

		NET_MSG_GENERIC			nmg;
		
		//	0:��ý���, 10, 10���Ľ���, 30���Ľ���,60���Ŀ�����.
		int						nTIME;
		char					szName[TEXT_LEN];

		SNET_CLUB_BATTLE_START_BRD () 
			: nTIME(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_START_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_START_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szName, 0, sizeof(char) * TEXT_LEN);
		}
	};

	struct SNET_CLUB_BATTLE_START_FLD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwID;

		SNET_CLUB_BATTLE_START_FLD () 
			: dwID(0)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_START_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_END_FLD
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwID;

		SNET_CLUB_BATTLE_END_FLD () 
			: dwID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_END_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_END_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_END_BRD
	{
		enum { TEXT_LEN = 33, };

		NET_MSG_GENERIC			nmg;
		char					szName[TEXT_LEN];
		char					szClubName[CHAR_SZNAME];

		SNET_CLUB_BATTLE_END_BRD ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_END_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_END_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szName, 0, sizeof(char) * TEXT_LEN);
			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_BATTLE_REMAIN_BRD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwTime;

		SNET_CLUB_BATTLE_REMAIN_BRD ()
			: dwTime(0)
		{
			nmg.dwSize = sizeof ( SNET_CLUB_BATTLE_REMAIN_BRD );
			nmg.nType  = NET_MSG_GCTRL_CLUB_BATTLE_REMAIN_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_CD_CERTIFY
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwNpcID;

		SNET_CLUB_CD_CERTIFY () 
			: dwNpcID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_CD_CERTIFY);
			nmg.nType = NET_MSG_GCTRL_CLUB_CD_CERTIFY;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_CD_CERTIFY_FB
	{
		enum { TEXT_LEN = 33, };

		NET_MSG_GENERIC			nmg;
		EMCDCERTIFY				emFB;

		SNET_CLUB_CD_CERTIFY_FB () 
			: emFB(EMCDCERTIFY_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_CD_CERTIFY_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_CD_CERTIFY_ING_BRD
	{
		enum { TEXT_LEN = 33, };

		NET_MSG_GENERIC			nmg;
		EMCDCERTIFY				emFB;

		char					szZone[TEXT_LEN];
		char					szName[CHAR_SZNAME];
		char					szClub[CHAR_SZNAME];
		char					szAlliance[CHAR_SZNAME];

		SNET_CLUB_CD_CERTIFY_ING_BRD () 
			: emFB(EMCDCERTIFY_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_CD_CERTIFY_ING_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_CD_CERTIFY_ING_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szZone, 0, sizeof(char) * TEXT_LEN);
			memset(szName, 0, sizeof(char) * CHAR_SZNAME);
			memset(szClub, 0, sizeof(char) * CHAR_SZNAME);
			memset(szAlliance, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_CD_CERTIFY_BRD
	{
		enum { TEXT_LEN = 33, };

		NET_MSG_GENERIC			nmg;
		char					szZone[TEXT_LEN];
		char					szName[CHAR_SZNAME];
		char					szClub[CHAR_SZNAME];
		char					szAlliance[CHAR_SZNAME];


		SNET_CLUB_CD_CERTIFY_BRD ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_CD_CERTIFY_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_CD_CERTIFY_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szZone, 0, sizeof(char) * TEXT_LEN);
			memset(szName, 0, sizeof(char) * CHAR_SZNAME);
			memset(szClub, 0, sizeof(char) * CHAR_SZNAME);
			memset(szAlliance, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_CERTIFIED_AGT
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwID;
		DWORD					dwCLUBID;

		SNET_CLUB_CERTIFIED_AGT () 
			: dwID(0)
			, dwCLUBID(CLUB_NULL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_CERTIFIED_AGT);
			nmg.nType = NET_MSG_GCTRL_CLUB_CERTIFIED_AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_CERTIFIED_FLD
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwID;
		DWORD					dwCLUBID;

		SNET_CLUB_CERTIFIED_FLD () 
			: dwID(0)
			, dwCLUBID(CLUB_NULL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_CERTIFIED_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_CERTIFIED_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_GUID_COMMISSION
	{
		NET_MSG_GENERIC			nmg;
		float					fCommission;

		SNET_CLUB_GUID_COMMISSION () 
			: fCommission(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_GUID_COMMISSION);
			nmg.nType = NET_MSG_GCTRL_CLUB_COMMISSION;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_GUID_COMMISSION_FB
	{
		NET_MSG_GENERIC			nmg;

		EMGUIDCOMMISSION_FB		emFB;
		float					fCommission;

		SNET_CLUB_GUID_COMMISSION_FB () 
			: emFB(EMGUIDCOMMISSION_FB_FAIL)
			, fCommission(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_GUID_COMMISSION_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_COMMISSION_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_GUID_COMMISSION_FLD
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwID;
		float					fCommission;

		SNET_CLUB_GUID_COMMISSION_FLD () 
			: dwID(0)
			, fCommission(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_GUID_COMMISSION_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_COMMISSION_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_GUID_COMMISSION_BRD
	{
		NET_MSG_GENERIC			nmg;
		float					fCommission;

		SNET_CLUB_GUID_COMMISSION_BRD () 
			: fCommission(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_GUID_COMMISSION_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_COMMISSION_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_GUID_COMMISSION_RESERVE_BRD
	{
		NET_MSG_GENERIC			nmg;
		float					fCommission;

		SNET_CLUB_GUID_COMMISSION_RESERVE_BRD () 
			: fCommission(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_GUID_COMMISSION_RESERVE_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_COMMISSION_RV_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_INCOME_MONEY_AGT
	{
		NET_MSG_GENERIC			nmg;
		DWORD					dwID;
		LONGLONG				lnMoney;

		SNET_CLUB_INCOME_MONEY_AGT () 
			: dwID(0)
			, lnMoney(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_INCOME_MONEY_AGT);
			nmg.nType = NET_MSG_GCTRL_CLUB_INCOME_MONEY_AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_GETSTORAGE
	{
		NET_MSG_GENERIC		nmg;

		SNET_CLUB_GETSTORAGE ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_GETSTORAGE);
			nmg.nType = NET_MSG_GCTRL_CLUB_GETSTORAGE;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_GETSTORAGE_ITEM
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwChannel;
		SINVENITEM			Data;

		SNET_CLUB_GETSTORAGE_ITEM () 
			: dwChannel(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_GETSTORAGE_ITEM);
			nmg.nType = NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_STORAGE_TO_HOLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwChannel;
		WORD				wPosX;
		WORD				wPosY;

		SNET_CLUB_STORAGE_TO_HOLD () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_TO_HOLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_TO_HOLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_STORAGE_EX_HOLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwChannel;
		WORD				wPosX;
		WORD				wPosY;

		SNET_CLUB_STORAGE_EX_HOLD () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_EX_HOLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_EX_HOLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_HOLD_TO_STORAGE
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwChannel;
		WORD				wPosX;
		WORD				wPosY;

		SNET_CLUB_HOLD_TO_STORAGE () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_HOLD_TO_STORAGE);
			nmg.nType = NET_MSG_GCTRL_CLUB_HOLD_TO_STORAGE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_STORAGE_SPLIT
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwChannel;
		WORD				wPosX;
		WORD				wPosY;
		WORD				wSplit;

		SNET_CLUB_STORAGE_SPLIT () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
			, wSplit(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_SPLIT);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_SPLIT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_STORAGE_SAVE_MONEY
	{
		NET_MSG_GENERIC		nmg;
		LONGLONG			lnMoney;

		SNET_CLUB_STORAGE_SAVE_MONEY () 
			: lnMoney(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_SAVE_MONEY);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_SAVE_MONEY;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_STORAGE_DRAW_MONEY
	{
		NET_MSG_GENERIC		nmg;
		LONGLONG			lnMoney;

		SNET_CLUB_STORAGE_DRAW_MONEY () 
			: lnMoney(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_DRAW_MONEY);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_DRAW_MONEY;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_STORAGE_INSERT
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwChannel;
		SINVENITEM			Data;

		SNET_CLUB_STORAGE_INSERT () 
			: dwChannel(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_INSERT);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_INSERT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_STORAGE_DELETE
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwChannel;
		WORD				wPosX;
		WORD				wPosY;

		SNET_CLUB_STORAGE_DELETE () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_DELETE);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_DELETE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_STORAGE_DEL_AND_INS
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwChannel;

		WORD				wDelX;
		WORD				wDelY;
        
		SINVENITEM			sInsert;

		SNET_CLUB_STORAGE_DEL_AND_INS () 
			: dwChannel(0)
			, wDelX(0)
			, wDelY(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_DEL_AND_INS);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_STORAGE_UPDATE_ITEM
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwChannel;

		WORD				wPosX;
		WORD				wPosY;

		WORD				wTurnNum;
		
		SNET_CLUB_STORAGE_UPDATE_ITEM () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
			, wTurnNum(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_UPDATE_ITEM);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_CLUB_STORAGE_UPDATE_MONEY
	{
		NET_MSG_GENERIC		nmg;
		LONGLONG			lnMoney;

		SNET_CLUB_STORAGE_UPDATE_MONEY ()
			: lnMoney(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_UPDATE_MONEY);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_STORAGE_RESET
	{
		NET_MSG_GENERIC		nmg;

		SNET_CLUB_STORAGE_RESET ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_STORAGE_RESET);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_RESET;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	//	���� �޽�����. ������ socket �� ��ġ�� �ʴ´�.
	struct SNET_CLUB_STORAGE_GET_DB
	{
		NET_MSG_GENERIC		nmg;

		DWORD				m_dwCharID;
		DWORD				m_dwClubID;

		LONGLONG			m_lnStorageMoney;
		CByteStream*		m_pStream;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(DWORD)+sizeof(LONGLONG)+sizeof(CByteStream*), };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNET_CLUB_STORAGE_GET_DB () 
			: m_dwCharID(0)
			, m_dwClubID(0)
			, m_lnStorageMoney(0)
			, m_pStream(NULL)
		{
			nmg.dwSize = sizeof(*this);
			nmg.nType = NET_MSG_GCTRL_CLUB_STORAGE_GET_DB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(m_cBUFFER, 0, sizeof(char) * (NET_DATA_BUFSIZE-EMMSG_SIZE));
		}
	};

	struct SNET_CLUB_INCOME_RENEW
	{
		NET_MSG_GENERIC		nmg;

		SNET_CLUB_INCOME_RENEW ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_INCOME_RENEW);
			nmg.nType = NET_MSG_GCTRL_CLUB_INCOME_RENEW;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_INCOME_UP
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwClubID;

		SNET_CLUB_INCOME_UP () 
			: dwClubID(CLUB_NULL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_INCOME_UP);
			nmg.nType = NET_MSG_GCTRL_CLUB_INCOME_UP;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_INCOME_DN
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwClubID;
		LONGLONG			lnInComeMoney;

		SNET_CLUB_INCOME_DN () 
			: dwClubID(CLUB_NULL)
			, lnInComeMoney(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_INCOME_DN);
			nmg.nType = NET_MSG_GCTRL_CLUB_INCOME_DN;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_INCOME_FB
	{
		NET_MSG_GENERIC		nmg;
		DWORD				dwClubID;
		LONGLONG			lnInComeMoney;

		SNET_CLUB_INCOME_FB () 
			: dwClubID(CLUB_NULL)
			, lnInComeMoney(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_INCOME_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_INCOME_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_NOTICE_REQ
	{
		NET_MSG_GENERIC		nmg;
		char				szNotice[EMCLUB_NOTICE_LEN+1];

		SNET_CLUB_NOTICE_REQ ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_NOTICE_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_NOTICE_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
			memset(szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN+1));
		}
	};

	struct SNET_CLUB_NOTICE_FB
	{
		NET_MSG_GENERIC		nmg;

		EMCLUB_NOTICE_FB	emFB;
		char				szNotice[EMCLUB_NOTICE_LEN+1];

		SNET_CLUB_NOTICE_FB () 
			: emFB(EMCLUB_NOTICE_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_NOTICE_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_NOTICE_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
			memset(szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN+1));
		}
	};

	struct SNET_CLUB_NOTICE_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwCLUB_ID;
		char				szNotice[EMCLUB_NOTICE_LEN+1];

		SNET_CLUB_NOTICE_FLD () 
			: dwCLUB_ID(CLUB_NULL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_NOTICE_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_NOTICE_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

            memset(szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN+1));
		}
	};

	struct SNET_CLUB_NOTICE_CLT
	{
		NET_MSG_GENERIC		nmg;

		char				szNotice[EMCLUB_NOTICE_LEN+1];

		SNET_CLUB_NOTICE_CLT ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_NOTICE_CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_NOTICE_CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN+1));
		}
	};

	struct SNET_CLUB_MEMBER_RENAME_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;
		DWORD				dwCharID;
		char				szName[CHAR_SZNAME];

		SNET_CLUB_MEMBER_RENAME_FLD () 
			: dwClubID(0),
			  dwCharID(0)
		{
			memset ( szName, 0, CHAR_SZNAME );
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_RENAME_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_MBR_RENAME_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_RENAME_CLT
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;
		DWORD				dwCharID;
		char				szName[CHAR_SZNAME];

		SNET_CLUB_MEMBER_RENAME_CLT () 
			: dwClubID(0),
			  dwCharID(0)
		{
			memset ( szName, 0, CHAR_SZNAME );
			nmg.dwSize = sizeof(SNET_CLUB_MEMBER_RENAME_CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_SUBMASTER
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwCharID;
		DWORD				dwFlags;

		SNET_CLUB_SUBMASTER () 
			: dwCharID(0)
			, dwFlags(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_SUBMASTER);
			nmg.nType = NET_MSG_GCTRL_CLUB_SUBMASTER;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_SUBMASTER_FB
	{
		NET_MSG_GENERIC		nmg;

		EMCLUBSUBMASTER_FB	emFB;
		DWORD				dwCharID;
		DWORD				dwFlags;

		SNET_CLUB_SUBMASTER_FB () 
			: emFB(EMCLUBSUBMASTER_FB_FAIL)
			, dwCharID(0)
			, dwFlags(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_SUBMASTER_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_SUBMASTER_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_SUBMASTER_BRD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwCharID;
		DWORD				dwFlags;
		DWORD				dwCDCertifior;

		SNET_CLUB_SUBMASTER_BRD () 
			: dwCharID(0)
			, dwFlags(0)
			, dwCDCertifior(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_SUBMASTER_BRD);
			nmg.nType = NET_MSG_GCTRL_CLUB_SUBMASTER_BRD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_SUBMASTER_FLD
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;
		DWORD				dwCharID;
		DWORD				dwFlags;
		DWORD				dwCDCertifior;

		SNET_CLUB_SUBMASTER_FLD () 
			: dwClubID(0)
			, dwCharID(0)
			, dwFlags(0)
			, dwCDCertifior(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_SUBMASTER_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_SUBMASTER_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_REQ
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwCharID;	// ��Ʋ�� Ŭ�������� ID.
		DWORD				dwBattleTime;

		SNET_CLUB_BATTLE_REQ () 
			: dwCharID(0)
			, dwBattleTime(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_REQ
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwCharID;	// ��Ʋ�� Ŭ�������� ID.
		DWORD				dwBattleTime;

		SNET_ALLIANCE_BATTLE_REQ () 
			: dwCharID(0)
			, dwBattleTime(0)
		{
			nmg.dwSize = sizeof(SNET_ALLIANCE_BATTLE_REQ);
			nmg.nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_REQ_FB
	{
		NET_MSG_GENERIC			nmg;

		EMCLUB_BATTLE_REQ_FB	emFB;
		
		SNET_CLUB_BATTLE_REQ_FB () 
			: emFB(EMCLUB_BATTLE_REQ_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_REQ_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_REQ_FB
	{
		NET_MSG_GENERIC			nmg;

		EMALLIANCE_BATTLE_REQ_FB	emFB;
		
		SNET_ALLIANCE_BATTLE_REQ_FB () 
			: emFB(EMALLIANCE_BATTLE_REQ_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_ALLIANCE_BATTLE_REQ_FB);
			nmg.nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_REQ_ASK
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubCharID;					//	����� Ŭ����
		char				szClubName[CHAR_SZNAME];		//	����� Ŭ���̸�.
		DWORD				dwBattleTime;

		SNET_CLUB_BATTLE_REQ_ASK () 
			: dwClubCharID(0)
			, dwBattleTime(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_REQ_ASK);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_ALLIANCE_BATTLE_REQ_ASK
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubCharID;					//	����� Ŭ����
		char				szClubName[CHAR_SZNAME];		//	����� Ŭ���̸�.
		DWORD				dwBattleTime;

		SNET_ALLIANCE_BATTLE_REQ_ASK () 
			: dwClubCharID(0)
			, dwBattleTime(0)
		{
			nmg.dwSize = sizeof(SNET_ALLIANCE_BATTLE_REQ_ASK);
			nmg.nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_BATTLE_REQ_ANS
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubCharID;
		bool				bOK;

		SNET_CLUB_BATTLE_REQ_ANS () 
			: dwClubCharID(0)
			, bOK(false)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_REQ_ANS);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_REQ_ANS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_REQ_ANS
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubCharID;
		bool				bOK;

		SNET_ALLIANCE_BATTLE_REQ_ANS () 
			: dwClubCharID(0)
			, bOK(false)
		{
			nmg.dwSize = sizeof(SNET_ALLIANCE_BATTLE_REQ_ANS);
			nmg.nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ANS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_BEGIN_FLD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwChiefClubID;
		DWORD					dwIndianClubID;

		__time64_t				tStartTime;
		__time64_t				tEndTime;

		bool					bAlliance;

		SNET_CLUB_BATTLE_BEGIN_FLD () 
			: dwChiefClubID(0)
			, dwIndianClubID(0)
			, tStartTime(0)
			, tEndTime(0)
			, bAlliance(false)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_BEGIN_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_BEGIN_CLT
	{
		NET_MSG_GENERIC			nmg;

		char					szClubName[CHAR_SZNAME];
		bool					bAlliance;

		SNET_CLUB_BATTLE_BEGIN_CLT () 
			: bAlliance(false)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_BEGIN_CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_BATTLE_BEGIN_CLT2
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwClubID;
		char					szClubName[CHAR_SZNAME];
		
		__time64_t				tStartTime;
		__time64_t				tEndTime;

		bool					bAlliance;

		SNET_CLUB_BATTLE_BEGIN_CLT2 () 
			: dwClubID(0)			
			, tStartTime(0)
			, tEndTime(0)
			, bAlliance(false)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_BEGIN_CLT2);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_BATTLE_ARMISTICE_REQ
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwClubID;

		SNET_CLUB_BATTLE_ARMISTICE_REQ () 
			: dwClubID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_ARMISTICE_REQ
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwClubID;

		SNET_ALLIANCE_BATTLE_ARMISTICE_REQ () 
			: dwClubID(0)
		{
			nmg.dwSize = sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ);
			nmg.nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_ARMISTICE_REQ_FB
	{
		NET_MSG_GENERIC			nmg;

		EMCLUB_BATTLE_ARMISTICE_REQ_FB	emFB;
		
		SNET_CLUB_BATTLE_ARMISTICE_REQ_FB () 
			: emFB(EMCLUB_BATTLE_ARMISTICE_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB
	{
		NET_MSG_GENERIC			nmg;

		EMALLIANCE_BATTLE_ARMISTICE_REQ_FB	emFB;
		
		SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB () 
			: emFB(EMALLIANCE_BATTLE_ARMISTICE_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB);
			nmg.nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;				//	����� Ŭ��ID

		SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK () 
			: dwClubID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;				//	����� Ŭ��ID

		SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK () 
			: dwClubID(0)
		{
			nmg.dwSize = sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK);
			nmg.nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_CLUB_BATTLE_ARMISTICE_REQ_ANS
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;
		bool				bOK;

		SNET_CLUB_BATTLE_ARMISTICE_REQ_ANS () 
			: dwClubID(0)
			, bOK(false)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ_ANS);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ANS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwClubID;
		bool				bOK;

		SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS () 
			: dwClubID(0)
			, bOK(false)
		{
			nmg.dwSize = sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS);
			nmg.nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_SUBMISSION_REQ
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwClubID;

		SNET_CLUB_BATTLE_SUBMISSION_REQ () 
			: dwClubID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_SUBMISSION_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_SUBMISSION_REQ
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwClubID;

		SNET_ALLIANCE_BATTLE_SUBMISSION_REQ () 
			: dwClubID(0)
		{
			nmg.dwSize = sizeof(SNET_ALLIANCE_BATTLE_SUBMISSION_REQ);
			nmg.nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_SUBMISSION_REQ_FB
	{
		NET_MSG_GENERIC			nmg;

		EMCLUB_BATTLE_SUBMISSION_REQ_FB	emFB;
		
		SNET_CLUB_BATTLE_SUBMISSION_REQ_FB () 
			: emFB(EMCLUB_BATTLE_SUBMISSION_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_SUBMISSION_REQ_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB
	{
		NET_MSG_GENERIC			nmg;

		EMALLIANCE_BATTLE_SUBMISSION_REQ_FB		emFB;
		
		SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB () 
			: emFB(EMALLIANCE_BATTLE_SUBMISSION_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB);
			nmg.nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwClubID_A;
		DWORD					dwClubID_B;

		SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD () 
			: dwClubID_A(0)
			, dwClubID_B(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwClubID_A;
		DWORD					dwClubID_B;
		WORD					wKillPoint;
		WORD					wDeathPoint;

		SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT () 
			: dwClubID_A(0)
			, dwClubID_B(0)
			, wKillPoint(0)
			, wDeathPoint(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_OVER_FLD
	{
		NET_MSG_GENERIC			nmg;

		EMCLUB_BATTLE_OVER_FB	emFB;

		DWORD					dwWinClubID;
		DWORD					dwLoseClubID;

		SNET_CLUB_BATTLE_OVER_FLD () 
			: emFB(EMCLUB_BATTLE_OVER_DRAW)
			, dwWinClubID(0)
			, dwLoseClubID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_OVER_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_OVER_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_OVER_CLT
	{
		NET_MSG_GENERIC			nmg;

		EMCLUB_BATTLE_OVER_FB	emFB;

		DWORD					dwClubID;

		WORD					wKillPoint;
		WORD					wDeathPoint;

		SNET_CLUB_BATTLE_OVER_CLT () 
			: emFB(EMCLUB_BATTLE_OVER_DRAW)
			, dwClubID(0)
			, wKillPoint(0)
			, wDeathPoint(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_OVER_CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_CLUB_BATTLE_KILL_UPDATE_AGT
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwClubID;
		DWORD					dwBattleClubID;
		WORD					wKillPoint;
		WORD					wDeathPoint;

		SNET_CLUB_BATTLE_KILL_UPDATE_AGT () 
			: dwClubID(0)
			, dwBattleClubID(0)
			, wKillPoint(0)
			, wDeathPoint(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_KILL_UPDATE_AGT);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_AGT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNET_CLUB_BATTLE_KILL_UPDATE_FLD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwClubID;
		DWORD					dwBattleClubID;
		WORD					wKillPoint;
		WORD					wDeathPoint;

		SNET_CLUB_BATTLE_KILL_UPDATE_FLD () 
			: dwClubID(0)
			, dwBattleClubID(0)
			, wKillPoint(0)
			, wDeathPoint(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_KILL_UPDATE_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNET_CLUB_BATTLE_KILL_UPDATE
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwBattleClubID;
		WORD					wKillPoint;
		WORD					wDeathPoint;

		SNET_CLUB_BATTLE_KILL_UPDATE () 
			: dwBattleClubID(0)
			, wKillPoint(0)
			, wDeathPoint(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_KILL_UPDATE);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNET_CLUB_BATTLE_POINT_UPDATE
	{
		NET_MSG_GENERIC			nmg;
		
		DWORD					dwBattleClubID;
		BOOL					bKillPoint;

		SNET_CLUB_BATTLE_POINT_UPDATE () 
			: dwBattleClubID(0)
			, bKillPoint(false)		
		{
			nmg.dwSize = sizeof(SNET_CLUB_BATTLE_POINT_UPDATE);
			nmg.nType = NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNETLOBBY_CLUB_BATTLE
	{
		enum { EMMAXBATTLE = 10 };

		NET_MSG_GENERIC		nmg;
		DWORD				dwBattleNum;

		GLCLUBBATTLE_LOBY	sBATTLE[EMMAXBATTLE];

		SNETLOBBY_CLUB_BATTLE () :
			dwBattleNum(0)
		{
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD);
			nmg.nType = NET_MSG_LOBBY_CLUB_BATTLE;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}

		bool ADDBATTLE ( const GLCLUBBATTLE_LOBY& sBattle )
		{
			if ( EMMAXBATTLE==dwBattleNum )		return false;

			sBATTLE[dwBattleNum] = sBattle;
			++dwBattleNum;

			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(GLCLUBBATTLE_LOBY)*dwBattleNum;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
			return true;
		}

		void RESET ()
		{
			dwBattleNum = 0;
			nmg.dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD);
		}
	};

	struct SNET_CLUB_ALLIANCE_REQ
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwCharID;	// ���Ϳ� ���Ե� Ŭ�������� ID.

		SNET_CLUB_ALLIANCE_REQ () 
			: dwCharID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_REQ_ASK
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwChiefCharID;					//	���� ��θӸ�.
		char				szChiefName[CHAR_SZNAME];		//	���� ��θӸ�.		

		SNET_CLUB_ALLIANCE_REQ_ASK () 
			: dwChiefCharID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_REQ_ASK);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ASK;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szChiefName, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_ALLIANCE_REQ_ANS
	{
		NET_MSG_GENERIC		nmg;

		DWORD				dwChiefCharID;
		bool				bOK;

		SNET_CLUB_ALLIANCE_REQ_ANS () 
			: dwChiefCharID(0)
			, bOK(false)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_REQ_ANS);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ANS;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_REQ_FB
	{
		NET_MSG_GENERIC			nmg;

		EMCLUB_ALLIANCE_REQ_FB	emFB;
		__time64_t				tBlock;

		SNET_CLUB_ALLIANCE_REQ_FB () 
			: tBlock(0)
			, emFB(EMCLUB_ALLIANCE_REQ_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_REQ_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_ADD_FLD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwChiefClubID;
		DWORD					dwIndianClubID;

		SNET_CLUB_ALLIANCE_ADD_FLD () 
			: dwChiefClubID(0)
			, dwIndianClubID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_ADD_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_ADD_CLT
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwChiefClubID;
		DWORD					dwIndianClubID;

		char					szChiefClub[CHAR_SZNAME];
		char					szIndianClub[CHAR_SZNAME];

		DWORD					dwAllianceBattleWin;
		DWORD					dwAllianceBattleLose;
		DWORD					dwAllianceBattleDraw;

		SNET_CLUB_ALLIANCE_ADD_CLT () 
			: dwChiefClubID(0)
			, dwIndianClubID(0)
			, dwAllianceBattleWin(0)
			, dwAllianceBattleLose(0)
			, dwAllianceBattleDraw(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_ADD_CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szChiefClub, 0, sizeof(char) * CHAR_SZNAME);
			memset(szIndianClub, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_ALLIANCE_DEL_REQ
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwDelClubID;

		SNET_CLUB_ALLIANCE_DEL_REQ () 
			: dwDelClubID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_DEL_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DEL_FB
	{
		NET_MSG_GENERIC			nmg;

		EMCLUB_ALLIANCE_DEL_FB	emFB;

		SNET_CLUB_ALLIANCE_DEL_FB () 
			: emFB(EMCLUB_ALLIANCE_DEL_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_DEL_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_SEC_REQ
	{
		NET_MSG_GENERIC			nmg;

		SNET_CLUB_ALLIANCE_SEC_REQ ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_SEC_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_SEC_FB
	{
		NET_MSG_GENERIC			nmg;

		EMCLUB_ALLIANCE_SEC_FB	emFB;

		SNET_CLUB_ALLIANCE_SEC_FB () 
			: emFB(EMCLUB_ALLIANCE_SEC_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_SEC_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DEL_FLD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwAlliance;
		DWORD					dwDelClubID;

		SNET_CLUB_ALLIANCE_DEL_FLD () 
			: dwAlliance(0)
			, dwDelClubID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_DEL_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DEL_CLT
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwDelClubID;
		char					szDelClub[CHAR_SZNAME];		

		SNET_CLUB_ALLIANCE_DEL_CLT () 
			: dwDelClubID(0)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_DEL_CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);

			memset(szDelClub, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_ALLIANCE_DIS_REQ
	{
		NET_MSG_GENERIC			nmg;

		SNET_CLUB_ALLIANCE_DIS_REQ ()
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_DIS_REQ);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_REQ;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DIS_FB
	{
		NET_MSG_GENERIC			nmg;
		EMCLUB_ALLIANCE_DIS_FB	emFB;

		SNET_CLUB_ALLIANCE_DIS_FB () 
			: emFB(EMCLUB_ALLIANCE_DIS_FB_FAIL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_DIS_FB);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DIS_FLD
	{
		NET_MSG_GENERIC			nmg;

		DWORD					dwChiefClubID;

		SNET_CLUB_ALLIANCE_DIS_FLD () 
			: dwChiefClubID(CLUB_NULL)
		{
			nmg.dwSize = sizeof(SNET_CLUB_ALLIANCE_DIS_FLD);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FLD;
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DIS_CLT
	{
		NET_MSG_GENERIC			nmg;

		char					szChiefClub[CHAR_SZNAME];

		SNET_CLUB_ALLIANCE_DIS_CLT ()
		{
			nmg.dwSize = (DWORD) sizeof(SNET_CLUB_ALLIANCE_DIS_CLT);
			nmg.nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT;
			memset (szChiefClub, 0, sizeof(char) * CHAR_SZNAME);
			GASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	// Revert to default structure packing
	#pragma pack()
};