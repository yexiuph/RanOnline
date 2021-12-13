#include "pch.h"
#include "./GLItemLMT.h"
#include "./GLGaeaServer.h"
#include "./GLChar.h"
#include "./DbActionLogic.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"
#include "../[Lib]__Engine/Sources/Common/StringFile.h"
#include "../[Lib]__Engine/Sources/Common/GLTexFile.h"
#include "../[Lib]__Engine/Sources/Common/StringUTILs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLITEMLMT& GLITEMLMT::GetInstance()
{
	static GLITEMLMT Instance;
	return Instance;
}

GLITEMLMT::GLITEMLMT(void) :
	m_nSGNum(0),
	m_nSvrNum(0),
	m_dwFieldID(0),

	m_pDBMan(NULL),
	m_pMsgServer(NULL)
{
}

GLITEMLMT::~GLITEMLMT(void)
{
}

void GLITEMLMT::ReadMaxKey ()
{
	if ( m_pMsgServer )
	{
		m_nSGNum = m_pMsgServer->GetServerGroup();
		m_nSvrNum = m_pMsgServer->GetServerNum();
	}

	if ( !m_pDBMan )	return;

	const int nTYPE[] =
	{
		EMGEN_DEFAULT,
		EMGEN_INIT,
		EMGEN_SHOP,
		EMGEN_NPC,
		EMGEN_QUEST,
		EMGEN_MOB,
		EMGEN_GMEDIT,
		EMGEN_OLD,
		EMGEN_BILLING,
		EMGEN_GATHERING,
		EMGEN_SYSTEM 
	};
	const int nNUM = sizeof(nTYPE)/sizeof(int);

	std::vector<VIEWLOGITEMEXCHANGEMAX> vecMAX;
	m_pDBMan->GetItemMaxNum ( m_nSGNum, m_nSvrNum, m_dwFieldID, vecMAX );
	DWORD dwSIZE = (DWORD) vecMAX.size();
	for ( DWORD i=0; i<dwSIZE; ++i )
	{
		const VIEWLOGITEMEXCHANGEMAX &sMAX = vecMAX[i];
		SetItemGenNum ( SNATIVEID(sMAX.wItemMain,sMAX.wItemSub), (EMITEMGEN)sMAX.nMakeType, sMAX.llMaxNum );
	}

	//for ( WORD m=0; m<GLItemMan::MAX_MID; ++m )
	//for ( WORD s=0; s<GLItemMan::MAX_SID; ++s )
	//{
	//	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( m, s );
	//	if ( !pITEM )		continue;

	//	for ( int i=0; i<nNUM; ++i )
	//	{
	//		EMITEMGEN emTYPE = (EMITEMGEN) nTYPE[i];

	//		LONGLONG lnMax = m_pDBMan->GetItemMaxNum ( m_nSGNum, m_nSvrNum, m_dwFieldID, emTYPE, m, s );
	//		if ( lnMax == DB_ERROR )		lnMax = 0;

	//		SetItemGenNum ( SNATIVEID(m,s), emTYPE, lnMax );
	//	}
	//}
}

void GLITEMLMT::SetItemGenNum ( const SNATIVEID &sNID, EMITEMGEN emTYPE, LONGLONG lnMax )
{
	SITEMLMT sLMT;
	GLGENITEM_ITER iter = m_mapGENITEM.find ( sNID.dwID );
	if ( iter!=m_mapGENITEM.end() )
	{
		sLMT = (*iter).second;
	}

	switch ( emTYPE )
	{
	case EMGEN_DEFAULT:		sLMT.lnDEFAULT = lnMax;
	case EMGEN_INIT:		sLMT.lnINIT = lnMax;
	case EMGEN_SHOP:		sLMT.lnSHOP = lnMax;
	case EMGEN_NPC:			sLMT.lnNPC = lnMax;
	case EMGEN_QUEST:		sLMT.lnQUEST = lnMax;
	case EMGEN_MOB:			sLMT.lnMOB = lnMax;
	case EMGEN_GMEDIT:		sLMT.lnGM = lnMax;
	case EMGEN_OLD:			sLMT.lnOLD = lnMax;
	case EMGEN_BILLING:		sLMT.lnBILLING = lnMax;
	case EMGEN_GATHERING:	sLMT.lnGather = lnMax;
	case EMGEN_SYSTEM:		sLMT.lnSystem = lnMax;
	};

	m_mapGENITEM[sNID.dwID] = sLMT;
}

LONGLONG GLITEMLMT::GetItemGenNum ( const SNATIVEID &sNID, EMITEMGEN emTYPE )
{
	GLGENITEM_ITER iter = m_mapGENITEM.find ( sNID.dwID );
	if ( iter!=m_mapGENITEM.end() )
	{
		const SITEMLMT &sLMT = (*iter).second;

		switch ( emTYPE )
		{
		case EMGEN_DEFAULT:		return sLMT.lnDEFAULT;
		case EMGEN_INIT:		return sLMT.lnINIT;
		case EMGEN_SHOP:		return sLMT.lnSHOP;
		case EMGEN_NPC:			return sLMT.lnNPC;
		case EMGEN_QUEST:		return sLMT.lnQUEST;
		case EMGEN_MOB:			return sLMT.lnMOB;
		case EMGEN_GMEDIT:		return sLMT.lnGM;
		case EMGEN_OLD:			return sLMT.lnOLD;
		case EMGEN_BILLING:		return sLMT.lnBILLING;
		case EMGEN_ALL:			return sLMT.GETTOTAL();
		case EMGEN_GATHERING:	return sLMT.lnGather;
		case EMGEN_SYSTEM:		return sLMT.lnSystem;
		};
	}

	return 0;
}

//	Note : 생성 가능한지 검사하는 함수.
//
bool GLITEMLMT::DoCheckItemGen ( const SNATIVEID &sNID, EMITEMGEN emTYPE /*=EMGEN_DEFAULT*/ )
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sNID );
	if ( !pITEM )											return false;

	//	이벤트 아이템이 아닐 경우 생성 무조건 가능.
	if ( !pITEM->sBasicOp.IsEVENT() )	return true;

	LONGLONG lnNum = GetItemGenNum ( sNID, emTYPE );

	return ( lnNum < pITEM->sGenerateOp.dwLimitTimeGen );
}

LONGLONG GLITEMLMT::RegItemGen ( const SNATIVEID &sNID, EMITEMGEN emTYPE /*=EMGEN_DEFAULT*/ )
{
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( sNID );
	if ( !pITEM )		return 0;

	SITEMLMT sLMT;
	GLGENITEM_ITER iter = m_mapGENITEM.find ( sNID.dwID );
	if ( iter!=m_mapGENITEM.end() )
	{
		sLMT = (*iter).second;
	}

	LONGLONG lnCOUNT(0);
	switch ( emTYPE )
	{
	case EMGEN_DEFAULT:
		lnCOUNT = ++sLMT.lnDEFAULT;
		break;

	case EMGEN_INIT:
		lnCOUNT = ++sLMT.lnINIT;
		break;

	case EMGEN_SHOP:
		lnCOUNT = ++sLMT.lnSHOP;
		break;

	case EMGEN_NPC:
		lnCOUNT = ++sLMT.lnNPC;
		break;

	case EMGEN_QUEST:
		lnCOUNT = ++sLMT.lnQUEST;
		break;

	case EMGEN_MOB:
		lnCOUNT = ++sLMT.lnMOB;
		break;

	case EMGEN_GMEDIT:
		lnCOUNT = ++sLMT.lnGM;
		break;

	case EMGEN_OLD:
		lnCOUNT = ++sLMT.lnOLD;
		break;

	case EMGEN_BILLING:
		lnCOUNT = ++sLMT.lnBILLING;
		break;

	case EMGEN_GATHERING:
		lnCOUNT = ++sLMT.lnGather;
		break;
	case EMGEN_SYSTEM:
		lnCOUNT = ++sLMT.lnSystem;
		break;
	};

	m_mapGENITEM[sNID.dwID] = sLMT;
	
	return lnCOUNT;
}

inline bool IsLOG_PILE_ITEM ( EMITEM_TYPE emTYPE )
{
	switch ( emTYPE )
	{
	case ITEM_TICKET:
	case ITEM_RECALL:
	case ITEM_KEY:
	case ITEM_LOUDSPEAKER:
	case ITEM_FIRECRACKER:
	case ITEM_DISJUNCTION:
	case ITEM_GRINDING:

	case ITEM_CHARACTER_CARD:
	case ITEM_INVEN_CARD:
	case ITEM_STORAGE_CARD:
	case ITEM_STORAGE_CONNECT:
	case ITEM_PREMIUMSET:
	case ITEM_PRIVATEMARKET:
	case ITEM_HAIR:
	case ITEM_FACE:
	case ITEM_CD:
	case ITEM_2FRIEND:
	case ITEM_CLUBCALL:
	case ITEM_HAIRSHOP:
	case ITEM_REVIVE:
	case ITEM_ANTI_DISAPPEAR:
	case ITEM_REMODEL:
	case ITEM_GARBAGE_CARD:
	case ITEM_TELEPORT_CARD:
		return true;

	default:
		return false;
	};

	return false;
}

void GLITEMLMT::ReqItemRoute ( const SITEMCUSTOM &sITEM, EMIDTYPE emFROME, DWORD dwFROMID, EMIDTYPE emTO, DWORD dwTOID, EMITEM_ROUTE emROUTE, int nNum )
{
	//	Note : 캐릭터 생성시 부여되는 아이템은 item ROUTE 정보 저장하지 않음.
	if ( sITEM.cGenType == EMGEN_INIT )		return;

	//	Note : 아이템 정보를 가져옴.
	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sITEM.sNativeID );
	if ( !pITEM )							return;

	LONGLONG lnGenNum = sITEM.lnGenNum;

	if ( !IsLOG_PILE_ITEM(pITEM->sBasicOp.emItemType) )
	{
		//	Note : 아이템 설정이 겹침가능일 경우 Item ROUTE 정보 저장하지 않음.
		if ( pITEM->ISPILE() )					return;

		lnGenNum = sITEM.lnGenNum;
	}

	if ( m_pDBMan )
	{
		//	Note : 정보 설정.
		SLOGITEMEXCHANGE sEXCHANGE
		(
			sITEM.sNativeID.wMainID,
			sITEM.sNativeID.wSubID,
			m_nSGNum,
			sITEM.cChnID,
			sITEM.cFieldID,
			sITEM.cGenType,
			lnGenNum,

			emFROME,
			dwFROMID,
			emTO,
			dwTOID,
			emROUTE,
			nNum,

			sITEM.nidDISGUISE.wMainID,
			sITEM.nidDISGUISE.wSubID,

			sITEM.grade_damage(),
			sITEM.grade_defense(),
		
			sITEM.grade_elec(),
			sITEM.grade_fire(),
			sITEM.grade_ice(),
			sITEM.grade_poison(),
			sITEM.grade_spirit()
		);

		CLogItemExchange *pDbAction = new CLogItemExchange ( sEXCHANGE );
		m_pDBMan->AddLogJob ( pDbAction );
	}
}

void GLITEMLMT::ReqMoneyExc(
	EMIDTYPE emFROM,
	DWORD dwFrom,
	EMIDTYPE emTO,
	DWORD dwTo,
	LONGLONG lnPrice,
	DWORD nFlag )
{
	if ( m_pDBMan )
	{
		SLOGMONEYEXCHANGE sLOG;
		sLOG.m_nSGNum = m_nSGNum;
		sLOG.m_nSvrNum = m_nSvrNum;
		sLOG.m_nFldNum = m_dwFieldID;
		
		sLOG.m_nFromType = emFROM;
		sLOG.m_nItemFrom = dwFrom;

		sLOG.m_nToType = emTO;
		sLOG.m_nItemTo = dwTo;
		
		sLOG.m_lnPrice = lnPrice;
		sLOG.m_nExchangeFlag = nFlag;

		CLogMoneyExchange *pDbAction = new CLogMoneyExchange( sLOG );
		m_pDBMan->AddLogJob( pDbAction );
	}
}

void GLITEMLMT::ReqItemConversion ( const SITEMCUSTOM &sITEM, EMIDTYPE emFROM, DWORD dwFrom )
{
	if ( m_pDBMan )
	{
		SLOGITEMCONVERSION sLOG
		(
			sITEM.sNativeID.wMainID,
			sITEM.sNativeID.wSubID,
			m_nSGNum,
			sITEM.cChnID,
			sITEM.cFieldID,
			sITEM.cGenType,
			sITEM.lnGenNum,

			emFROM,
			dwFrom,

			sITEM.nidDISGUISE.wMainID,
			sITEM.nidDISGUISE.wSubID,

			sITEM.grade_damage(),
			sITEM.grade_defense(),
		
			sITEM.grade_elec(),
			sITEM.grade_fire(),
			sITEM.grade_ice(),
			sITEM.grade_poison(),
			sITEM.grade_spirit()
		);

		CLogItemConversion *pDbAction = new CLogItemConversion( sLOG );
		m_pDBMan->AddLogJob( pDbAction );
	}
}

void GLITEMLMT::ReqAction ( DWORD dwCI, EMLOGACTION emACT, EMIDTYPE emTAR, DWORD dwTAR, __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY )
{
	if ( m_pDBMan )
	{
		SLOGACTION sACTION;
		sACTION.m_nCHARID = dwCI;
		sACTION.m_nTYPE = emACT;

		sACTION.m_nTARTYPE = emTAR;
		sACTION.m_nTARID = dwTAR;

		sACTION.m_nEXP = nEXP;
		sACTION.m_nBRIGHT_POINT = nBRIGHT;
		sACTION.m_nLIFE_POINT = nLIFE;
		sACTION.m_nMONEY = nMONEY;

		CLogAction *pDbAction = new CLogAction ( sACTION );
		m_pDBMan->AddLogJob ( pDbAction );
	}
}

void GLITEMLMT::ReqRandomItem ( const SITEMCUSTOM &sITEM )
{
	if( m_pDBMan )
	{
		SLOGRANDOMOPTION sRANDOMOPTION;

		sRANDOMOPTION.nNIDMain = sITEM.sNativeID.wMainID;
		sRANDOMOPTION.nNIDSub = sITEM.sNativeID.wSubID;
		sRANDOMOPTION.nSGNum = m_nSGNum;
		sRANDOMOPTION.nSvrNum = sITEM.cChnID;
		sRANDOMOPTION.nFldNum = sITEM.cFieldID;
		sRANDOMOPTION.nMakeType = sITEM.cGenType;
		sRANDOMOPTION.lnMakeNum = sITEM.lnGenNum;

		sRANDOMOPTION.cOptTYPE1 = sITEM.cOptTYPE1;
		sRANDOMOPTION.cOptTYPE2 = sITEM.cOptTYPE2;
		sRANDOMOPTION.cOptTYPE3 = sITEM.cOptTYPE3;
		sRANDOMOPTION.cOptTYPE4 = sITEM.cOptTYPE4;

		sRANDOMOPTION.nOptVALUE1 = sITEM.nOptVALUE1;
		sRANDOMOPTION.nOptVALUE2 = sITEM.nOptVALUE2;
		sRANDOMOPTION.nOptVALUE3 = sITEM.nOptVALUE3;
		sRANDOMOPTION.nOptVALUE4 = sITEM.nOptVALUE4;

		CLogRandomItem *pDbAction = new CLogRandomItem( sRANDOMOPTION );
		m_pDBMan->AddLogJob ( pDbAction );
	}
}

void GLITEMLMT::ReqPetAction( int nPetNum, SNATIVEID nItemID, EMPET_ACTION emAction, int nPetFull )
{
	if( m_pDBMan )
	{
		SLOGPETACTION sPETACTION;

		sPETACTION.nPetNum = nPetNum;
		sPETACTION.nItemMID = nItemID.wMainID;
		sPETACTION.nItemSID = nItemID.wSubID;
		sPETACTION.nActionType = emAction;
		sPETACTION.nPetFull = nPetFull;

		CLogPetAction *pDbAction = new CLogPetAction( sPETACTION );
		m_pDBMan->AddLogJob ( pDbAction );
	}
}

void GLITEMLMT::ReqVehicleAction( int nVehicleNum, SNATIVEID nItemID, EMVEHICLE_ACTION emAction, int nVehicleFull)
{
	if( m_pDBMan )
	{
		SLOGVEHICLEACTION sVEHICLEACTION;

		sVEHICLEACTION.nVehicleNum = nVehicleNum;
		sVEHICLEACTION.nItemMID = nItemID.wMainID;
		sVEHICLEACTION.nItemSID = nItemID.wSubID;
		sVEHICLEACTION.nActionType = emAction;
		sVEHICLEACTION.nVehicleFull = nVehicleFull;

		CLogVehicleAction *pDbAction = new CLogVehicleAction( sVEHICLEACTION );
		m_pDBMan->AddLogJob ( pDbAction );
	}
}