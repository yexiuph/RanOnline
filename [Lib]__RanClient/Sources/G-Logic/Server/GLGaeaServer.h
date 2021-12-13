#pragma once

#include "./GLACTOR.h"
#include "./GLARoundSlot.h"
#include "./GLChar.h"
#include "./GLClubMan.h"
#include "./GLEventProc.h"
#include "./GLFreePK.h"
#include "./GLITEMLMT.h"
#include "./GLogicData.h"
#include "./GLLandMan.h"
#include "./GLMapList.h"
#include "./GLPartyFieldMan.h"
#include "./GLPetField.h"
#include "./GLSummonField.h"
#include "./DbActionLogic.h"

#include "../[Lib]__Engine/Sources/Common/CMemPool.h"
#include "../[Lib]__Engine/Sources/Common/StringFile.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandGateMan.h"
#include "../[Lib]__Engine/Sources/G-Logic/DxConsoleMsg.h"
#include "../[Lib]__Engine/Sources/G-Logic/DxMsgServer.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLDBMan.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLPeriod.h"

#include "../[Lib]__NetServer/Sources/s_CDbAction.h"
#include "../[Lib]__NetServer/Sources/s_CConsoleMessage.h"
#include "../[Lib]__NetServer/Sources/s_CSMsgList.h"

#ifndef GLARoundSlot
	class GLARoundSlot;
#endif

class GLCrow;

struct SFIELDCROW
{
	EMCROW					emCROW;
	DWORD					dwID;
	DWORD					dwFRAME;
	SGLNODE<SFIELDCROW*>*	pNODE;

	SFIELDCROW () :
		emCROW(CROW_PC),
		dwID(0),
		dwFRAME(0),
		pNODE(NULL)
	{
	}

	void RESET ()
	{
		emCROW = CROW_PC;
		dwID = 0;

		dwFRAME = 0;
		pNODE = NULL;
	}
};
typedef SFIELDCROW* PSFIELDCROW;

class CDbActToAgentMsg;

struct SDROPOUTINFO
{
	DWORD				m_dwGaeaID;
	DWORD				m_dwUserID;
	CDbActToAgentMsg*	m_pMsg;

	SDROPOUTINFO () :
		m_dwGaeaID(0),
		m_dwUserID(0),
		m_pMsg(NULL)
	{
	}

	SDROPOUTINFO ( DWORD dwGaeaID, DWORD dwUserID, CDbActToAgentMsg* pMsg ) :
		m_dwGaeaID(dwGaeaID),
		m_dwUserID(dwUserID),
		m_pMsg(pMsg)
	{
	}
};

struct SDROPOUTPETINFO
{
	DWORD dwPetGuid;
	bool  bLeaveFieldServer;
	bool  bMoveMap;

	SDROPOUTPETINFO () :
		dwPetGuid(UINT_MAX),
		bLeaveFieldServer(false),
		bMoveMap(false)
	{
	}

	SDROPOUTPETINFO ( DWORD _dwPetGuid, bool _bLeaveFieldServer, bool _bMoveMap ) :
		dwPetGuid(_dwPetGuid),
		bLeaveFieldServer(_bLeaveFieldServer),
		bMoveMap(_bMoveMap)
	{
	}
};

struct SDROPOUTSUMMONINFO
{
	DWORD dwSummonGuid;
	bool  bLeaveFieldServer;

	SDROPOUTSUMMONINFO () :
		dwSummonGuid(UINT_MAX),
		bLeaveFieldServer(false)
	{
	}

	SDROPOUTSUMMONINFO ( DWORD _dwSummonGuid, bool _bLeaveFieldServer ) :
		dwSummonGuid(_dwSummonGuid),
		bLeaveFieldServer(_bLeaveFieldServer)
	{
	}
};

struct SCLASSEVENTDATA
{
	float fItemGainRate[GLCI_NUM_NEWSEX];
	float fExpGainRate[GLCI_NUM_NEWSEX];
	float fMoneyGainRate[GLCI_NUM_NEWSEX];

	DWORD dwExpMinLevel[GLCI_NUM_NEWSEX];
	DWORD dwExpMaxLevel[GLCI_NUM_NEWSEX];
	DWORD dwItemMinLevel[GLCI_NUM_NEWSEX];
	DWORD dwItemMaxLevel[GLCI_NUM_NEWSEX];
	DWORD dwMoneyMinLevel[GLCI_NUM_NEWSEX];
	DWORD dwMoneyMaxLevel[GLCI_NUM_NEWSEX];

	SCLASSEVENTDATA()
	{
		AllInitExpRate();
		AllInitItemRate();
		AllInitMoneyRate();
	}

	void AllInitExpRate()
	{
		for( BYTE i = 0; i < GLCI_NUM_NEWSEX; i++ )
		{
			InitExpRate(i);
		}
	}
	void AllInitItemRate()
	{
		for( BYTE i = 0; i < GLCI_NUM_NEWSEX; i++ )
		{
			InitItemRate(i);
		}
	}
	void AllInitMoneyRate()
	{
		for( BYTE i = 0; i < GLCI_NUM_NEWSEX; i++ )
		{
			InitMoneyRate(i);
		}
	}

	void InitExpRate(BYTE dwClass)
	{
		fExpGainRate[dwClass]   = 1.0f;
		dwExpMinLevel[dwClass] = dwExpMaxLevel[dwClass] = 0;
	}
	void InitItemRate(BYTE dwClass)
	{
		fItemGainRate[dwClass]  = 1.0f;
		dwItemMinLevel[dwClass] = dwItemMaxLevel[dwClass] = 0;
	}
	void InitMoneyRate(BYTE dwClass)
	{
		fMoneyGainRate[dwClass] = 1.0f;
		dwMoneyMinLevel[dwClass] = dwMoneyMaxLevel[dwClass] = 0;
	}
};


struct SReserveMSG
{
	CTime  sendTime;
	BYTE   sendMsg[NET_DATA_BUFSIZE];
	DWORD  dwClientID;
	DWORD  dwGaeaID;
	
};


class GLGaeaServer : public GLMapList
{
public:
	typedef std::map<DWORD,DWORD>			CLIENTMAP;
	typedef CLIENTMAP::iterator				CLIENTMAP_ITER;

	typedef std::vector<SDROPOUTINFO>		VPCID;
	typedef VPCID::iterator					VPCID_ITER;

	typedef std::set<DWORD>					PCID;
	typedef PCID::iterator					PCID_ITER;

	typedef std::vector<SDROPOUTPETINFO>	VPETID;
	typedef VPETID::iterator				VPETID_ITER;


	typedef std::vector<SDROPOUTSUMMONINFO>	VSUMMONID;
	typedef VSUMMONID::iterator				VSUMMONID_ITER;

	typedef std::vector<GLLandMan*> VEC_LANDMAN;
	typedef VEC_LANDMAN::iterator	VEC_LANDMAN_ITER;

	typedef std::list<DWORD>			LISTSEARCHSHOP;
	typedef LISTSEARCHSHOP::iterator	LISTSEARCHSHOP_ITER;

	typedef std::vector<DWORD>				VEC_INSTANT_MAPID;
	typedef VEC_INSTANT_MAPID::iterator		VEC_INSTANT_MAPID_ITER;


	typedef std::list<SReserveMSG>		RESERVEMSGLIST;
	typedef RESERVEMSGLIST::iterator	RESERVEMSGLIST_ITER;


protected:
	CString		m_strPath;

public:
	CString& GetPath ()				{ return m_strPath; }
	void SetPath ( char* szPath )	{ m_strPath = szPath; }

public:
	bool					m_bBigHead;
	bool					m_bBigHand;
	bool					m_bBrightEvent;

	int						m_nServiceProvider;
	SEVENT_FACT				m_sEVENTFACT;
	SEventState				m_sEventState;

	bool					m_bEmptyMsg;

protected:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	DxMsgServer*			m_pMsgServer;
	DxConsoleMsg*			m_pConsoleMsg;
	GLDBMan*				m_pDBMan;

	GLEventProc				m_cEventProc;

protected:
	int						m_nServerChannel;
	DWORD					m_dwFieldSvrID;
	DWORD					m_dwAgentSlot;
	BOOL					m_bUseIntelTBB;

protected:
	BOOL					m_bUpdate;
	bool					m_bEmulator;
	bool					m_bReservedStop;		// 서버 정지 예약

	bool					m_bGenItemHold;
	bool					m_bClubBattleStarted;	// 선도전 시작했는지 유무
	bool					m_bClubDMStarted;		// 클럽데스매치 시작여부

	//float					m_fMaxDelayMsgProc;		//	메시지 처리의 최대 지연 시간.  ( 서버 Flip Time )

	DWORD					m_dwMaxClient;
	PGLCHAR*				m_PCArray;				//	PC 배열.
	
	GLCHARLIST				m_GaeaPCList;			//	PC 리스트.
	GLCHAR_MAP				m_PCNameMap;			//	PC Name map.
	CLIENTMAP				m_PCClientIDMAP;		//	PC ClientID map.
	CLIENTMAP				m_mapCHARID;			//	CID map.

	// PET
	PGLPETFIELD*			m_PETArray;
	CMemPool<GLPetField>	m_poolPET;
	CMList<DWORD>			m_FreePETGIDs;			//	미사용된 PET GlobID 들.
	VPETID					m_reqDropOutPet;		//  DropOutPet PET GUID

	// Summon
	PGLSUMMONFIELD*			m_SummonArray;
	CMemPool<GLSummonField>	m_poolSummon;
	CMList<DWORD>			m_FreeSummonGIDs;			//	미사용된 SUMMON GlobID 들.
	VSUMMONID				m_reqDropOutSummon;		//  DropOutSummon SUMMON GUID

	GLLandMan*				m_pLandMan[MAXLANDMID][MAXLANDSID];	// 랜드 포인터.
	VEC_LANDMAN				m_vecLandMan;

	VEC_INSTANT_MAPID		m_vecInstantMapId;						// 인던 맵 아이디 벡터
	VEC_LANDMAN				m_vecInstantMapSrcLandMan;				// 인던 복사 대상 맵 벡터

	GLPartyFieldMan			m_cPartyFieldMan;
	GLClubMan				m_cClubMan;

	VPCID					m_reqDropOutChar;		//	dropout 요청.
	
	PCID					m_reqSaveDBUserID;		//	저장요청한 userid.

	float					m_fTIMER_CLUB;

	CRITICAL_SECTION		m_CSPCLock;

	CMemPool<GLChar>		m_poolCHAR;
	CMemPool<GLCrow>		m_poolCROW;
	CMemPool<GLMaterial>	m_poolMATERIAL;
	CMemPool<SFIELDCROW>	m_poolFIELDCROW;
	CMemPool<GLLandMan>		m_poolGLLandMan;

	SCLASSEVENTDATA			m_ClassEventData;

	LISTSEARCHSHOP			m_listSearchShop;

	RESERVEMSGLIST			m_listReserveMsg;

protected:
	double					m_fCrashTimer;
	double					m_fDelayMilliSec;
	DWORD					m_dwServerCrashTime;
	WORD					m_wSkipPacketNum;


public:
	void					SetServerCrashTime ( DWORD dwSec )	{ m_dwServerCrashTime = dwSec; }
	void					SetServerCrashTime ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_SET_SERVER_CRASHTIME_FLD* pNetMsgFld );
	void					SetDelayMilliSec ( DWORD dwSec )	{ m_fDelayMilliSec = (double)dwSec; }
	void					SetDelayMilliSec ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_SET_SERVER_DELAYTIME_FLD* pNetMsgFld );
	void					SetSkipPacketNum ( WORD wNum )		{ m_wSkipPacketNum = wNum; }
	void					SetSkipPacketNum ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_SET_SERVER_SKIPPACKET_FLD* pNetMsgFld );
	double					GetLimitFPS() const					{ return (m_fDelayMilliSec*0.001f); }

	float					GetExpGainRate( EMCHARINDEX CLASS, DWORD dwLevel );
	float					GetItemGainRate( EMCHARINDEX CLASS, DWORD dwLevel );
	float					GetMoneyGainRate( EMCHARINDEX CLASS, DWORD dwLevel );


public:
	GLCrow* NEW_CROW ();
	void RELEASE_CROW ( GLCrow* pCROW );

	GLChar* NEW_CHAR ();
	void RELEASE_CHAR ( GLChar* pCHAR );

	GLMaterial* NEW_MATERIAL ();
	void RELEASE_MATERIAL ( GLMaterial* pMaterial );

	SFIELDCROW* NEW_FIELDCROW ();
	void RELEASE_FIELDCROW ( SFIELDCROW* pFIELDCROW );

	PGLPETFIELD NEW_PET ();
	void RELEASE_PET ( PGLPETFIELD pPet );

	PGLSUMMONFIELD NEW_SUMMON ();
	void RELEASE_SUMMON ( PGLSUMMONFIELD pSummon );


	PGLLANDMAN NEW_GLLANDMAN ();
	void RELEASE_GLLANDMAN ( PGLLANDMAN pSummon );

public:
	void SetAgentSlot ( DWORD dwClient ) { m_dwAgentSlot = dwClient; }
	DWORD GetAgentSlot () { return m_dwAgentSlot; }
	void SetReserveServerStop () { m_bReservedStop = true; }
	bool IsReserveServerStop () { return m_bReservedStop; }

public:
	bool IsGenItemHold ()				{ return m_bGenItemHold; }
	bool IsBRIGHTEVENT ()				{ return m_bBrightEvent; }
	bool IsClubBattleStarted ()			{ return m_bClubBattleStarted; }
	bool IsClubDMStarted()				{ return m_bClubDMStarted; }

public:
	DxMsgServer* GetMsgServer ()		{ return m_pMsgServer; }
	DxConsoleMsg* GetConsoleMsg ()		{ return m_pConsoleMsg; }
	GLDBMan* GetDBMan ()				{ return m_pDBMan; }

	DWORD GetFieldSvrID ()				{ return m_dwFieldSvrID; }
	int GetServerChannel ()				{ return m_nServerChannel; }

	BOOL GetUseIntelTBB()				{ return m_bUseIntelTBB; }

	GLPARTY_FIELD* GetParty ( DWORD dwPartyID )		{ return m_cPartyFieldMan.GetParty(dwPartyID); }
	GLPartyFieldMan& GetPartyMan ()					{ return m_cPartyFieldMan; }
	GLClubMan& GetClubMan ()						{ return m_cClubMan; }

public:
	PGLCHAR GetChar ( DWORD dwID ) const;
	PGLCHAR GetChar ( std::string strName );
	PGLCHAR GetCharID ( DWORD dwCharID );

	DWORD GetNumPC ()					{ return m_GaeaPCList.m_dwAmount; }
	DWORD GetMaxClient ()				{ return m_dwMaxClient; }

//public:
//	void SetMaxDelayMsgProc ( float fmaxdelay )		{ m_fMaxDelayMsgProc = fmaxdelay; }
//	float GetMaxDelayMsgProc ()						{ return m_fMaxDelayMsgProc; }

public:
	void ChangeNameMap ( PGLCHAR pChar, const char* pszOldName, const char* pszNewName );
	void ChangeNameMap ( PGLCHAR pChar, const TCHAR* pszPhoneNumber );

protected:
	BOOL DropPC ( SNATIVEID MapID, D3DXVECTOR3 vPos, PGLCHAR pPC );
	BOOL DropOutPC ( DWORD dwGaeaID );

public:
	BOOL SaveCharDB ( DWORD dwGaeaID );

public:
	BOOL ClearReservedDropOutPC ();

public:
	BOOL ReserveServerStop ();

	// 종료시 모든 클럽 배틀 진행상황을 저장한다.
public:
	BOOL SaveClubBattle();
	void DelPlayHostileClubBattle( DWORD dwClub_P, DWORD dwClub_S );
	void DelPlayHostileAllianceBattle( DWORD dwClub_P, DWORD dwClub_S );

public:
	bool IsEmulatorMode () { return m_bEmulator; }

	void InsertSearchShop( DWORD dwGaeaID );
	void EraseSearchShop( DWORD dwGaeaID );
	bool FindSearchShop( DWORD dwGaeaID );

public:
	BOOL ReserveDropOutPC ( DWORD dwGaeaID, CDbActToAgentMsg *pDbActToAgentMsg=NULL );
	
	BOOL FindSaveDBUserID ( DWORD dwUserID );
	void SetSaveDBUserID ( DWORD dwUserID );
	BOOL ResetSaveDBUserID ( DWORD dwUserID );

	BOOL SaveNpcCommission( DWORD dwCharID, DWORD dwUserID, LONGLONG lnCommission );
	BOOL SaveNpcCommissionDB( DWORD dwCharID, DWORD dwUserID, LONGLONG lnCommission );

public:
	PGLCHAR CreatePC ( PCHARDATA2 pCharData, DWORD dwClientID, DWORD dwGaeaID, BOOL bNEW=FALSE,
		SNATIVEID *_pStartMap=NULL, DWORD _dwStartGate=0, D3DXVECTOR3 vPos=D3DXVECTOR3(0,0,0),
		EMGAME_JOINTYPE emJOINTYPE=EMJOINTYPE_FIRST,
		DWORD dwThaiCCafeClass = 0, __time64_t loginTime = 0, INT nMyCCafeClass = 0 );

	BOOL EntryLand( DWORD dwGaeaID, DWORD dwGateID, BOOL bInstantMap, SNATIVEID sMapID = NATIVEID_NULL() );

	// PET
	PGLPETFIELD CreatePET ( PGLPET pPetData, DWORD dwOwner, DWORD dwPetID, bool bValid = true );
	void		CreatePETOnDB ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_CREATEPET_FROMDB_FB* pNetMsg );
	void		GetPETInfoFromDB ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_GETPET_FROMDB_FB* pNetMsg );
	void		GetPETInfoFromDBError( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_GETPET_FROMDB_ERROR* pNetMsg );
	BOOL		DropPET ( PGLPETFIELD pPet, SNATIVEID sMapID );
	BOOL		DropOutPET ( DWORD dwGUID, bool bLeaveFieldServer, bool bMoveMap );
	PGLPETFIELD GetPET ( DWORD dwGUID ) const;
	void		ReserveDropOutPet ( SDROPOUTPETINFO sPetInfo ) { m_reqDropOutPet.push_back (sPetInfo); }
	void		ClearReserveDropOutPet ();

	BOOL		RequestUsePETCARD ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_REQ_USEPETCARD* pNetMsg );
	BOOL		RequestRevivePet ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_REQ_REVIVE* pNetMsg );
	BOOL		RevivePet ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPET_REQ_REVIVE_FROMDB_FB* pNetMsg );

	BOOL		ReqActiveVehicle ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_ACTIVE_VEHICLE* pNetMsg );
	BOOL		ReqGetVehicle( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_GET_VEHICLE* pNetMsg );
	BOOL		CreateVehicle( DWORD dwClientID, DWORD dwGaeaID, DWORD dwVehicleID );
	void		CreateVehicleOnDB ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_VEHICLE_CREATE_FROMDB_FB* pNetMsg );
	void		GetVehicleInfoFromDB ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_VEHICLE_GET_FROMDB_FB* pNetMsg );
	void		GetVehicleInfoFromDBError( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_VEHICLE_GET_FROMDB_ERROR* pNetMsg );
	void		SaveVehicle( DWORD dwClientID, DWORD dwGaeaID, bool bLeaveFieldServer );
	void		SetActiveVehicle ( DWORD dwClientID, DWORD dwGaeaID, bool bActive );
	void		GetVehicleItemInfo ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_VEHICLE_REQ_ITEM_INFO* pNetMsg );

	//	예약 메시지 처리
	void		ReserveMessage( DWORD dwClientID, DWORD dwGaeaID, CTime time, LPVOID nmg );
	void		ReserveMessage( DWORD dwClientID, DWORD dwGaeaID, DWORD dwLatterSec, LPVOID nmg );
	void		ReserveMessageProcess();

	// 소환수
	BOOL		RequestSummon ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_USE_SUMMON* pNetMsg );

	PGLSUMMONFIELD CreateSummon ( PGLSUMMON pSummonData, DWORD dwOwner, DWORD dwSummonID, bool bValid = true );
	BOOL		DropSummon ( PGLSUMMONFIELD pSummon, SNATIVEID sMapID );
	BOOL		DropOutSummon ( DWORD dwGUID, bool bLeaveFieldServer );
	PGLSUMMONFIELD GetSummon ( DWORD dwGUID ) const;
	void		ReserveDropOutSummon ( SDROPOUTSUMMONINFO sSummonInfo ) { m_reqDropOutSummon.push_back (sSummonInfo); }
	void		ClearReserveDropOutSummon ();

	//	부활 스킬 사용 여부
	void		SetNonRebirth ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_NON_REBIRTH_REQ* pNetMsg );
	// QBox On/Off 옵션
	void		ReqQBoxEnable ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_QBOX_OPTION_REQ_FLD* pNetMsg );

	//	클럽데스매치 랭킹 요청
	BOOL	ReqClubDeathMatchRanking ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_CLUB_DEATHMATCH_RANKING_REQ* pNetMsg );

public:
	BOOL RequestMustLeaveMap ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_MUST_LEAVE_MAP* pNetMsg );
	BOOL RequestGateOutReq ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETREQ_GATEOUT_REQ* pNetMsg );
	BOOL RequestReBirth ( const DWORD dwGaeaID, const SNATIVEID &sNID_Map, const DWORD dwGenGate, const D3DXVECTOR3 &_vPos );
	BOOL RequestMoveMapPC ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETREQ_GATEOUT *pNetMsg );
	BOOL RequestLandIn ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETREQ_LANDIN *pNetMsg );

	BOOL RequestFieldSvrOut ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_FIELDSVR_OUT *pNetMsg );
	BOOL RequestReBirthOut ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REBIRTH_OUT *pNetMsg );

	BOOL RequestTrade ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE *pNetMsg );
	BOOL RequestTradeTarAns ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_TAR_ANS *pNetMsg );

	BOOL RequestTradeMoney ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_MONEY *pNetMsg );
	BOOL RequestTradeItemResist ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_ITEM_REGIST *pNetMsg );
	BOOL RequestTradeItemReMove ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_ITEM_REMOVE *pNetMsg );

	BOOL RequestTradeAgree ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_AGREE *pNetMsg );
	BOOL RequestTradeCancel ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_TRADE_CANCEL *pNetMsg );

	BOOL RequestFieldSvrCharChk ( DWORD dwClientID, GLMSG::SNETPC_FIELDSVR_CHARCHK *pNetMsg );

	BOOL RequestConfrontPartyChkMbr ( GLMSG::SNETPC_CONFRONTPTY_CHECKMBR2_FLD *pNetMsg );
	BOOL RequestConfrontParty ( GLMSG::SNETPC_CONFRONTPTY_START2_FLD *pNetMsg );
	BOOL RequestConfrontPartyEnd ( GLMSG::SNETPC_CONFRONTPTY_END2_FLD *pNetMsg );

	BOOL RequestConfrontClubChkMbr ( GLMSG::SNET_CONFRONTCLB_CHECKMBR_FLD *pNetMsg );
	BOOL RequestConfrontClub ( GLMSG::SNETPC_CONFRONTCLB_START2_FLD *pNetMsg );
	BOOL RequestConfrontClubEnd ( GLMSG::SNETPC_CONFRONTCLB_END2_FLD *pNetMsg );

	BOOL RequestClubRank2Fld ( GLMSG::SNET_CLUB_RANK_2FLD *pNetMsg );
	BOOL RequestClubInfo ( GLMSG::SNET_CLUB_INFO_2FLD *pNetMsg );
	BOOL RequestClubDissolution2Fld ( GLMSG::SNET_CLUB_DISSOLUTION_2FLD *pNetMsg );
	BOOL RequestClubDel2Fld ( GLMSG::SNET_CLUB_DEL_2FLD *pNetMsg );
	
	BOOL RequestClubAdd2Fld ( GLMSG::SNET_CLUB_MEMBER_ADD_2FLD *pNetMsg );
	BOOL RequestClubDel ( GLMSG::SNET_CLUB_MEMBER_DEL_2FLD *pNetMsg );
	BOOL RequestClubMarkChange ( GLMSG::SNET_CLUB_MARK_CHANGE_2FLD *pNetMsg );
	BOOL RequestClubSubMaster ( GLMSG::SNET_CLUB_SUBMASTER_FLD *pNetMsg );
	BOOL RequestClubAuthority ( GLMSG::SNET_CLUB_AUTHORITY_FLD *pNetMsg );	

	BOOL RequestChargedItem2Inven ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_CHARGED_ITEM2_INVEN *pNetMsg );

	BOOL RequestConftSPtyExp ( GLMSG::SNET_CONFT_SPTY_EXP_FLD *pNetMsg );

	BOOL RequestGenItemFieldFB ( GLMSG::SNET_REQ_GENITEM_FLD_FB * pNetMsg );

	BOOL RequestInvenRecallThisSvr ( GLChar* pPC, SNATIVEID sMAPID, DWORD dwGATEID, D3DXVECTOR3 vPOS );
	BOOL RequestInvenRecall ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_INVEN_RECALL *pNetMsg );
	BOOL RequestBus ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_BUS *pNetMsg );
	BOOL RequestTaxi ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_TAXI *pNetMsg );
	BOOL RequestTaxiNpcPos ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_TAXI_NPCPOS *pNetMsg );
	BOOL Request2FriendCK ( GLMSG::SNETPC_2_FRIEND_CK *pNetMsg );
	BOOL RequestMove2CharPos ( DWORD dwClientID, GLMSG::SNETPC_GM_MOVE2CHAR_POS *pNetMsg );

	BOOL RequestInvenTeleport ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_REQ_INVEN_TELEPORT *pNetMsg );
	BOOL RequestInvenTeleportThisSvr ( GLChar* pPC, SNATIVEID sMAPID, D3DXVECTOR3 vPOS );

	BOOL RequestCreateInstantMapReq ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETREQ_CREATE_INSTANT_MAP_REQ *pNetMsg );
	BOOL RequestCreateInstantMap  ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETREQ_CREATE_INSTANT_MAP_FLD *pNetMsg );

	void DeleteInstantMap( const DWORD i );


	BOOL RequestNpcRecall( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_INVEN_NPC_RECALL *pNetMsg );
	BOOL RequestNpcCommission( GLMSG::SNET_INVEN_NPC_COMMISSION_FB* pNetMsg );
	
public:
	BOOL ServerClubBattleStart ( GLMSG::SNET_CLUB_BATTLE_START_FLD *pNetMsg );
	BOOL ServerClubBattleEnd ( GLMSG::SNET_CLUB_BATTLE_END_FLD *pNetMsg );
	
	BOOL ServerClubDeathMatchReady( GLMSG::SNET_CLUB_DEATHMATCH_READY_FLD *pNetMsg );
	BOOL ServerClubDeathMatchStart ( GLMSG::SNET_CLUB_DEATHMATCH_START_FLD *pNetMsg );
	BOOL ServerClubDeathMatchEnd ( GLMSG::SNET_CLUB_DEATHMATCH_END_FLD *pNetMsg );
	
	BOOL ServerClubCertify ( GLMSG::SNET_CLUB_CERTIFIED_FLD *pNetMsg );
	BOOL ServerClubGuidCommission ( GLMSG::SNET_CLUB_GUID_COMMISSION_FLD *pNetMsg );
	BOOL ServerClubNotice ( GLMSG::SNET_CLUB_NOTICE_FLD *pNetMsg );
	BOOL ServerClubStorageGetDB ( GLMSG::SNET_CLUB_STORAGE_GET_DB *pNetMsg );
	BOOL ServerClubInComeDn ( DWORD dwClientID, GLMSG::SNET_CLUB_INCOME_DN *pNetMsg );
	BOOL ServerClubAllianceAddFld ( GLMSG::SNET_CLUB_ALLIANCE_ADD_FLD *pNetMsg );
	BOOL ServerClubAllianceDel ( GLMSG::SNET_CLUB_ALLIANCE_DEL_FLD *pNetMsg );
	BOOL ServerClubAllianceDis ( GLMSG::SNET_CLUB_ALLIANCE_DIS_FLD *pNetMsg );

	BOOL ServerClubBattleBeginFld ( GLMSG::SNET_CLUB_BATTLE_BEGIN_FLD *pNetMsg );
	BOOL ServerClubBattleOverFld ( GLMSG::SNET_CLUB_BATTLE_OVER_FLD *pNetMsg );
	BOOL ServerClubBattleKillUpdate( GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_FLD *pNetMsg );
	BOOL ServerClubBattleLastKillUpdate( GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD *pNetMsg );

	BOOL ServerLevelEventEnd( GLMSG::SNET_LEVEL_EVENT_END_FLD *pNetMsg );
	BOOL ServerLevelEventWarning( GLMSG::SNET_LEVEL_EVENT_WARNING_FLD * pNetMsg );
	BOOL ServerLevelEventCountdown( GLMSG::SNET_LEVEL_EVENT_COUNTDOWN_FLD * pNetMsg );

	BOOL ServerFieldInfoReset ( GLMSG::SNET_FIELDINFO_RESET *pNetMsg );
	BOOL ServerSchoolFreePk ( GLMSG::SNETPC_SCHOOLFREEPK_FLD *pNetMsg );

	BOOL ServerCtrlPeriod ( GLMSG::SNET_PERIOD *pNetMsg );
	BOOL ServerCtrlWeather ( GLMSG::SNETSERVER_CTRL_WEATHER *pNetMsg );
	BOOL ServerCtrlWeather2 ( GLMSG::SNETSERVER_CTRL_WEATHER2 *pNetMsg );
	BOOL ServerCtrlTime ( GLMSG::SNETSERVER_CTRL_TIME *pNetMsg );
	BOOL ServerCtrlMonth ( GLMSG::SNETSERVER_CTRL_MONTH *pNetMsg );
	BOOL ServerCtrlGenItemHold ( GLMSG::SNETSERVER_CTRL_GENITEMHOLD *pNetMsg );
	BOOL ServerCtrlPlayerKillingMode ( GLMSG::SNET_SERVER_PLAYERKILLING_MODE *pNetMsg );
	BOOL ServerCtrlCharDropOutForced ( GLMSG::SNET_DROP_OUT_FORCED *pNetMsg );


public:
	BOOL GMCtrolMove2Gate ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOVE2GATE_FLD *pNetMsg );
	//BOOL GMCtrolMove2Gate ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOVE2GATE *pNetMsg );
	BOOL GMCtrolMove2MapPos ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOVE2MAPPOS_FLD *pNetMsg );

	BOOL GMCtrlWhereNpc ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_WHERE_NPC_FLD *pNetMsg );
	BOOL GMCtrolWherePcPos ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_WHERE_PC_POS *pNetMsg );
	BOOL GMCtrlWarningMSG ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_WARNING_MSG_FLD* pNetMsg );

	BOOL GMCtrolBigHead ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_BIGHEAD *pNetMsg );
	BOOL GMCtrolBigHand ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_BIGHAND *pNetMsg );

	BOOL GMCtrolFreePK ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_FREEPK *pNetMsg );
	BOOL GMKicUser( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_KICK_USER_PROC_FLD* pNetMsgFld );
	BOOL GMCtrolShowMeTheMoney( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_SHOWMETHEMONEY_FLD* pNetMsgFld );

	BOOL GMCtrolMobGen ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_GEN_FLD *pNetMsg );
	BOOL GMCtrolMobDel ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_DEL_FLD *pNetMsg );

	BOOL GMCtrolMoPrintCrowList ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_PRINT_CROWLIST_FLD *pNetMsg );

	BOOL GMCtrolMobGenEx ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_GEN_EX_FLD *pNetMsg );
	BOOL GMCtrolMobDelEx ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_DEL_EX_FLD *pNetMsg );

	BOOL GMCtrolEventEx ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_EVENT_EX *pNetMsg );
	BOOL GMCtrolEventExEnd ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_EVENT_EX_END *pNetMsg );

	BOOL GMCtrolLimitEventBegin ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_LIMIT_EVENT_BEGIN *pNetMsg );
	BOOL GMCtrolLimitEventTimeReset ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_LIMIT_EVENT_TIME_RESET *pNetMsg );
	BOOL GMCtrolLimitEventEnd   ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_LIMIT_EVENT_END *pNetMsg );

	BOOL GMCtrolClassEvent   ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_CLASS_EVENT *pNetMsg );

	BOOL GMCtrolMobLevel ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_LEV *pNetMsg );
	BOOL GMCtrolMobLevelClear ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_MOB_LEV_CLEAR *pNetMsg );
	BOOL GMCtrolChatBlockFld ( GLMSG::SNET_GM_CHAT_BLOCK_FLD *pNetMsg );
	BOOL GMCtrolCharInfoFld ( GLMSG::SNET_GM_CHAR_INFO_FLD *pNetMsg );
	BOOL UserCtrolCharInfoFld ( GLMSG::SNET_USER_CHAR_INFO_FLD *pNetMsg );

	BOOL RequestViewAllPlayer ( GLMSG::SNET_GM_VIEWALLPLAYER_FLD_REQ *pNetMsg );

	BOOL ReqClubMemberRename ( GLMSG::SNET_CLUB_MEMBER_RENAME_FLD* pNetMsg );

	BOOL ReqSearchShopItem ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_PMARKET_SEARCH_ITEM* pNetMsg );
	BOOL ReqSearchResultShopItem ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ* pNetMsg );

	BOOL RequestShopInfo ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_GM_SHOP_INFO_REQ *pNetMsg );

	BOOL CyberCafeClassUpdate ( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNET_CYBERCAFECLASS_UPDATE *pNetMsg );

	void CheckMarketState( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_MARKETCHECK *pNetMsg );

	BOOL VietnamAllInitTime ( GLMSG::SNETPC_VIETNAM_ALLINITTIME *pNetMsg );

	BOOL MsgVietTimeReset( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_VIETNAM_TIME_REQ_FB *pNetMsg ); //vietnamtest%%% && vietnamtest2


public:
	BOOL ValidCheckTarget ( GLLandMan* pLandMan, STARGETID &sTargetID );

	GLACTOR* GetTarget ( const GLLandMan* pLandMan, const STARGETID &sTargetID );
	const D3DXVECTOR3& GetTargetPos ( const GLLandMan* pLandMan, const STARGETID &sTargetID );
	WORD GetTargetBodyRadius ( GLLandMan* pLandMan, STARGETID &sTargetID );

	GLARoundSlot* GetARoundSlot ( const STARGETID &sTargetID );

public:
	GLLandMan* GetByMapID ( const SNATIVEID &sMapID );
	GLLandMan* GetRootMap ();
	void	   SetMapState();
//	GLLANDMANLIST& GetLandManList ()	{ return m_LandManList; }
	VEC_LANDMAN GetLandMan() { return m_vecLandMan; }

protected:
	HRESULT InsertMap ( GLLandMan* pNewLandMan );
	HRESULT CreateInstantMap( SNATIVEID sDestMapID, SNATIVEID sInstantMapID, DWORD dwGaeaID, DWORD dwPartyID );
	GLLandMan* GetInstantMapByMapID ( const SNATIVEID &sMapID );

public:
	HRESULT Create ( DWORD dwMaxClient, DxMsgServer *pMsgServer, DxConsoleMsg* pConsoleMsg, GLDBMan* pDBMan, int nServiceProvider, const char* szMapList=NULL, DWORD dwFieldSID=FIELDSERVER_MAX, int nChannel=0, BOOL bUseIntelTBB = FALSE );
	HRESULT Create4Level ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT CleanUp ();

	void SetUpdate ( BOOL bUpdate )		{ m_bUpdate = bUpdate; }

public:
	HRESULT ClearDropObj ();

public:
	HRESULT OneTimeSceneInit();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	void	FrameMoveGLChar( float fElapsedTime );
	void	FrameMoveInstantMap( float fElapsedTime );
	void	FrameMoveLandMan( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );

protected:
	BOOL ChatMsgProc ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

public:
	HRESULT MsgProcess ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

public:
	void SENDTOALLCLIENT ( LPVOID nmg );
	void SENDTOCLIENT ( DWORD dwClientID, LPVOID nmg );
	void SENDTOAGENT ( DWORD dwClientID, LPVOID nmg );
	void SENDTOAGENT ( LPVOID nmg );

	void SENDTOPARTYCLIENT ( DWORD dwPartyID, LPVOID nmg );
	void SENDTOCLUBCLIENT ( DWORD dwClubID, LPVOID nmg );
	
	void SENDTOCLIENT_ONMAP ( DWORD dwMapID, LPVOID nmg );
	void SENDTOCLUBCLIENT_ONMAP ( DWORD dwMapID, DWORD dwClubID, LPVOID nmg );

	void SendAgentQuestNpcPos();

protected:
	GLGaeaServer(void);

public:
	~GLGaeaServer(void);

public:
	static GLGaeaServer& GetInstance();

public:
	void CHECKVALID();
};

//-----------------------------------------------------------------------------------------
inline PGLPETFIELD GLGaeaServer::GetPET ( DWORD dwGUID ) const
{
	if ( dwGUID >= m_dwMaxClient )	return NULL;
	return m_PETArray[dwGUID];
}

inline PGLSUMMONFIELD GLGaeaServer::GetSummon ( DWORD dwGUID ) const
{
	if ( dwGUID >= m_dwMaxClient )	return NULL;
	return m_SummonArray[dwGUID];
}


inline PGLCHAR GLGaeaServer::GetChar ( DWORD dwID ) const
{
	if ( dwID >= m_dwMaxClient )	return NULL;
	return m_PCArray[dwID];
}

inline PGLCHAR GLGaeaServer::GetCharID ( DWORD dwCharID )
{
	CLIENTMAP_ITER pos = m_mapCHARID.find(dwCharID);
	if ( pos==m_mapCHARID.end() )		return NULL;

	return GetChar ( (*pos).second );
}

inline void GLGaeaServer::SENDTOAGENT ( DWORD dwClientID, LPVOID nmg )
{
	if ( m_pMsgServer )
		m_pMsgServer->SendAgent ( dwClientID, nmg );
}

inline void GLGaeaServer::SENDTOAGENT ( LPVOID nmg )
{
	if ( m_pMsgServer )
		m_pMsgServer->SendAgent ( nmg );
}

inline void GLGaeaServer::SENDTOPARTYCLIENT ( DWORD dwPartyID, LPVOID nmg )
{
	m_cPartyFieldMan.SendMsgToMember ( dwPartyID, (NET_MSG_GENERIC*) nmg );
}

inline void CONSOLEMSG_WRITE ( const char* msg, ... )
{
	GASSERT ( msg && "CONSOLEMSG_WRITE()" );

	DxConsoleMsg* pConsoleMsg = GLGaeaServer::GetInstance().GetConsoleMsg();
	if ( !pConsoleMsg )		return;

	char sbuf[C_BUFFER_SIZE];
	
	va_list ap;
	va_start(ap, msg);
	StringCbVPrintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	pConsoleMsg->Write( LOG_CONSOLE, sbuf );
}

inline void TEXTCONSOLEMSG_WRITE ( const char* msg, ... )
{
	GASSERT ( msg && "CONSOLEMSG_WRITE()" );

	DxConsoleMsg* pConsoleMsg = GLGaeaServer::GetInstance().GetConsoleMsg();
	if ( !pConsoleMsg )		return;

	char sbuf[C_BUFFER_SIZE];

	va_list ap;
	va_start(ap, msg);
	StringCbVPrintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	pConsoleMsg->Write( LOG_TEXT_CONSOLE, sbuf );
}

inline void HACKINGLOG_WRITE ( const char* msg, ... )
{
	GASSERT ( msg && "DEBUGMSG_WRITE()" );

	char sbuf[C_BUFFER_SIZE];

	va_list ap;
	va_start(ap, msg);
	StringCbVPrintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	CDebugSet::ToHackingFile( sbuf );

	DxConsoleMsg* pConsoleMsg = GLGaeaServer::GetInstance().GetConsoleMsg();
	if ( !pConsoleMsg )		return;

	pConsoleMsg->Write ( LOG_CONSOLE, sbuf );
}

inline void BILLIONUPDATE_WRITE ( const char* msg, ... )
{
	GASSERT ( msg && "DEBUGMSG_WRITE()" );

	char sbuf[C_BUFFER_SIZE];

	va_list ap;
	va_start(ap, msg);
	StringCbVPrintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

//	CDebugSet::ToBillionUpdateFile( sbuf );

	/*DxConsoleMsg* pConsoleMsg = GLGaeaServer::GetInstance().GetConsoleMsg();
	if ( !pConsoleMsg )		return;

	pConsoleMsg->Write ( LOG_CONSOLE, sbuf );*/
}


inline void DEBUGMSG_WRITE ( const char* msg, ... )
{
	GASSERT ( msg && "DEBUGMSG_WRITE()" );

	char sbuf[C_BUFFER_SIZE];
	
	va_list ap;
	va_start(ap, msg);
	StringCbVPrintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	CDebugSet::ToLogFile ( sbuf );

	DxConsoleMsg* pConsoleMsg = GLGaeaServer::GetInstance().GetConsoleMsg();
	if ( !pConsoleMsg )		return;

	pConsoleMsg->Write ( LOG_CONSOLE, sbuf );
}
