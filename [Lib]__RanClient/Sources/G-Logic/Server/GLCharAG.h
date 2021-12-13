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

	DWORD				m_dwActState;							//	현제 행동 설정.

	SNATIVEID			m_sCurMapID;							//	현재 맵.
	DWORD				m_dwCurFieldSvr;						//	현제 필드 서버.

	SNATIVEID			m_sNextMapID;							//	필드 서버 변경시 새로 진입할 맵 ID.
	DWORD				m_dwNextGateID;							//	필드 서버 변경시 새로 진입할 게이트 ID.
	D3DXVECTOR3			m_vNextPos;								//	필드 서버 변경시 새로운 위치.
	DWORD				m_dwNextFieldSvr;						//	필드 서버 변경 ID.

	SGLNODE<GLCharAG*>*	m_pPCNode;								//	PC 리스트 노드.

	vector<SGETWHISPERLIST> m_vecGetWhisperList;			    // 귓속말을 엿듣는 GM리스트

	SGETWHISPERLIST			m_GmWhisperList;				    // 자신이 GM일 경우 엿듣는 캐릭터 정보
	bool					m_bViewWhisperMSG;


	bool				m_bFIELDCK_OK;							//	이미 체크를 완료하였는가?
	DWORD				m_dwFIELDCK_NUM;						//	필드 서버 체크 수.
	DWORD				m_dwFIELDCK_BEING;						//	필드 서버에 남아 있던 잔존량.
	bool				m_bFIELD_CK[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX];//	필드 서버 체크 여부.

	SPARTY_OPT			m_sReqPartyOpt;							//	요청한 파티 옵션.


	CONFT_MAP			m_mapCONFT;								//	최근 대련 정보.
	SCONFTING			m_sCONFTING;							//	현제 대련 정보.
	SCONFTING			m_sREQCONFTING;							//	요청한 대련 정보.

	float				m_fFRIEND_TIMER;

	bool				m_bReciveEventTime;						// 이벤트 시간을 받았는지 아닌지.
	bool				m_bTracingUser;							// 현재 추적중인 유저인지 아닌지

	DWORD				m_dwComboAttend;						// 출석 연속 일자.

	float				m_fCyberClassCheck;					// 사이버 카페 체크타임

	__time64_t			m_tLoginTime;

protected:
	bool m_bFriendWindowOpen;			// 친구창 갱신 여부
	bool m_bClubWindowOpen;				// 클럽창 갱신 여부

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
	//	필드에 캐릭터를 생성전에 필드의 char 무결성 점검 테스트 완료 여부 점검.
	//	( 완료후는 단한번만 호출되야함. )
	bool IsFieldCheckComplete ( bool *pFieldSvr );

	//	점검이 끝난 캐릭터인지 검사.
	//	(점검이 끝난 캐릭터는 필드서버와의 연결이 유효해야한다.)
	bool IsDoneFieldCheck ();

public:
	bool IsCHATBLOCK ();

public:
	void SetCurrentField ( DWORD dwSvr, SNATIVEID &sNID );

	void ResetNextFieldSvr ();
	void SetNextFieldSvr ( SNATIVEID sMID, DWORD dwGateID, D3DXVECTOR3 vPos, DWORD dwFieldSvr );

public:
	//	최근 대련 정보 요청.
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

	//	Memo :	주/보조 무기 변경(Field->Agent)
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

