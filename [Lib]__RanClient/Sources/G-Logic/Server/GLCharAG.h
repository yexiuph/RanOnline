#pragma once

#include "./GLContrlCharJoinMsg.h"

class GLCharAG : public GLCHARAG_DATA
{
protected:
	typedef std::map<DWORD,SCONFT_LOG>	CONFT_MAP;
	typedef CONFT_MAP::iterator			CONFT_MAP_ITER;

public:
	DWORD				m_dwClientID;
	int					m_nChannel;


	DWORD				m_dwGaeaID;
	DWORD				m_dwPartyID;

	DWORD				m_dwActState;							//	���� �ൿ ����.

	SNATIVEID			m_sCurMapID;							//	���� ��.
	DWORD				m_dwCurFieldSvr;						//	���� �ʵ� ����.

	SNATIVEID			m_sNextMapID;							//	�ʵ� ���� ����� ���� ������ �� ID.
	DWORD				m_dwNextGateID;							//	�ʵ� ���� ����� ���� ������ ����Ʈ ID.
	D3DXVECTOR3			m_vNextPos;								//	�ʵ� ���� ����� ���ο� ��ġ.
	DWORD				m_dwNextFieldSvr;						//	�ʵ� ���� ���� ID.

	SGLNODE<GLCharAG*>*	m_pPCNode;								//	PC ����Ʈ ���.

	vector<SGETWHISPERLIST> m_vecGetWhisperList;			    // �ӼӸ��� ����� GM����Ʈ

	SGETWHISPERLIST			m_GmWhisperList;				    // �ڽ��� GM�� ��� ����� ĳ���� ����
	bool					m_bViewWhisperMSG;


	bool				m_bFIELDCK_OK;							//	�̹� üũ�� �Ϸ��Ͽ��°�?
	DWORD				m_dwFIELDCK_NUM;						//	�ʵ� ���� üũ ��.
	DWORD				m_dwFIELDCK_BEING;						//	�ʵ� ������ ���� �ִ� ������.
	bool				m_bFIELD_CK[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX];//	�ʵ� ���� üũ ����.

	SPARTY_OPT			m_sReqPartyOpt;							//	��û�� ��Ƽ �ɼ�.


	CONFT_MAP			m_mapCONFT;								//	�ֱ� ��� ����.
	SCONFTING			m_sCONFTING;							//	���� ��� ����.
	SCONFTING			m_sREQCONFTING;							//	��û�� ��� ����.

	float				m_fFRIEND_TIMER;

	bool				m_bReciveEventTime;						// �̺�Ʈ �ð��� �޾Ҵ��� �ƴ���.
	bool				m_bTracingUser;							// ���� �������� �������� �ƴ���

	DWORD				m_dwComboAttend;						// �⼮ ���� ����.

	float				m_fCyberClassCheck;					// ���̹� ī�� üũŸ��

	__time64_t			m_tLoginTime;

protected:
	bool m_bFriendWindowOpen;			// ģ��â ���� ����
	bool m_bClubWindowOpen;				// Ŭ��â ���� ����

protected:
	BOOL IsSTATE ( DWORD dwState )					{ return m_dwActState&dwState; }
	
public:
	void SetSTATE ( DWORD dwState )					{ m_dwActState |= dwState; }
	void ReSetSTATE ( DWORD dwState )				{ m_dwActState &= ~dwState; }

public:
	void SetPartyID ( DWORD dwPartyID )		{ m_dwPartyID = dwPartyID; }
	void ReSetPartyID ()					{ m_dwPartyID = (DWORD) PARTY_NULL; }
	DWORD GetPartyID ()						{ return m_dwPartyID; }

public:
	HRESULT CreateChar ( GLCHARAG_DATA *pchar_data );

public:
	//	�ʵ忡 ĳ���͸� �������� �ʵ��� char ���Ἲ ���� �׽�Ʈ �Ϸ� ���� ����.
	//	( �Ϸ��Ĵ� ���ѹ��� ȣ��Ǿ���. )
	bool IsFieldCheckComplete ( bool *pFieldSvr );

	//	������ ���� ĳ�������� �˻�.
	//	(������ ���� ĳ���ʹ� �ʵ弭������ ������ ��ȿ�ؾ��Ѵ�.)
	bool IsDoneFieldCheck ();

public:
	bool IsCHATBLOCK ();

public:
	void SetCurrentField ( DWORD dwSvr, SNATIVEID &sNID );

	void ResetNextFieldSvr ();
	void SetNextFieldSvr ( SNATIVEID sMID, DWORD dwGateID, D3DXVECTOR3 vPos, DWORD dwFieldSvr );

public:
	//	�ֱ� ��� ���� ��û.
	const SCONFT_LOG* GETCONFRONT_LOG ( DWORD dwCHARID );
	const SCONFTING& GETCONFRONTING ()		{ return m_sCONFTING; }

	void SaveLogConfront ();

	void StartConfront ( EMCONFT_TYPE emType, DWORD dwTAR_ID, const SCONFT_OPTION &sOption );
	void ResetConfront ( EMCONFRONT_END emEND );
	void ResetConfrontOk ();

public:
	void AddFriend ( char *szFriend );
	bool DelFriend ( char* szFriend );
	bool BlockFriend ( char* szFriend, bool bBLOCK );
	void SetStateFriend ( char *szFriend, DWORD dwFlag );
	int  GetStateFriend ( char *szFriend );


	void CarcAttendCombo();

protected:
	HRESULT MsgActState ( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqFriendList ( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqFriendAdd ( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqFriendAddAns ( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqFriendDel ( NET_MSG_GENERIC* nmg );
	HRESULT MsgReqFriendBlock ( NET_MSG_GENERIC* nmg );

	HRESULT MsgClubMemberReqAgt ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubMemberDel ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubMemberSecede ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubAuthorityReq ( NET_MSG_GENERIC* nmg );	
	HRESULT MsgClubAuthorityReqAns ( NET_MSG_GENERIC* nmg );	
	HRESULT MsgClubMarkInfo ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubMarkChange ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubRank2Agt ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubGuidCommission ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubNoticeReq ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubSubMaster ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubAllianceReq ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubAllianceReqAns ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubAllianceDelReq ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubAllianceSecReq ( NET_MSG_GENERIC* nmg );
	HRESULT MsgClubAllianceDisReq ( NET_MSG_GENERIC* nmg );

	HRESULT	MsgClubBattleReq ( NET_MSG_GENERIC* nmg );
	HRESULT	MsgClubBattleReqAns ( NET_MSG_GENERIC* nmg );
	HRESULT	MsgClubBattleReqAnsAlliance ( NET_MSG_GENERIC* nmg );
	HRESULT	MsgClubBattleArmisticeReq ( NET_MSG_GENERIC* nmg );
	HRESULT	MsgClubBattleArmisticeReqAns ( NET_MSG_GENERIC* nmg );
	HRESULT	MsgClubBattleSubmissionReq ( NET_MSG_GENERIC* nmg );	

	HRESULT	MsgAllianceBattleReq ( NET_MSG_GENERIC* nmg );
	HRESULT	MsgAllianceBattleReqAns ( NET_MSG_GENERIC* nmg );
	HRESULT	MsgAllianceBattleArmisticeReq ( NET_MSG_GENERIC* nmg );
	HRESULT	MsgAllianceBattleArmisticeReqAns ( NET_MSG_GENERIC* nmg );
	HRESULT	MsgAllianceBattleSubmissionReq ( NET_MSG_GENERIC* nmg );
	

	HRESULT MsgRenameBrd ( NET_MSG_GENERIC* nmg );
	HRESULT MsgPhoneNumberBrd ( NET_MSG_GENERIC* nmg );
	HRESULT	MsgUpdateStartMap ( NET_MSG_GENERIC* nmg );

	HRESULT MsgSetServerDelayTime ( NET_MSG_GENERIC* nmg );
	HRESULT MsgSetSkipPacketNum ( NET_MSG_GENERIC* nmg );
	HRESULT MsgSetServerCrashTime ( NET_MSG_GENERIC* nmg );

	//	Memo :	��/���� ���� ����(Field->Agent)
	HRESULT MsgChangeArm( NET_MSG_GENERIC* nmg );

	HRESULT MsgFriendClubWindowOpen( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqQboxOption( NET_MSG_GENERIC* nmg );

	HRESULT	MsgReqAttendList( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqAttendance( NET_MSG_GENERIC* nmg );

public:
	HRESULT MsgProcess ( NET_MSG_GENERIC* nmg );
	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	GLCharAG(void);
	~GLCharAG(void);
};

typedef GLCharAG* PGLCHARAG;
typedef CGLLIST<PGLCHARAG>	AGCHARLIST;
typedef SGLNODE<PGLCHARAG>	AGCHARNODE;

