#include "pch.h"

#include "GLCharacter.h"
#include "GLCharClient.h"
#include "GLPartyClient.h"
#include "GLCrowClient.h"
#include "GLItemMan.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "GLGaeaClient.h"
#include "GLLandManClient.h"

#include "glcrowrenlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool CROWREN::DIFFERENT ( const CROWREN &sINFO )
{
	//m_emCROW;	//	key 이므로 다른 경우가 없음.
	//m_dwID;

	if ( m_wSCHOOL!=sINFO.m_wSCHOOL )					return true;
	if ( m_dwCOLOR!=sINFO.m_dwCOLOR )					return true;
	if ( m_emPARTY!=sINFO.m_emPARTY )					return true;
	if ( m_dwTYPE!=sINFO.m_dwTYPE )						return true;
	if ( m_bPMARKET!=sINFO.m_bPMARKET )					return true;

	if ( strcmp(m_szNAME,sINFO.m_szNAME) )				return true;
	if ( strcmp(m_szPMARKET,sINFO.m_szPMARKET) )		return true;
	if ( strcmp(m_szNICK,sINFO.m_szNICK) )				return true;

	if ( m_dwCLUB!=sINFO.m_dwCLUB )						return true;
	if ( m_dwCLUB_MARK_VER!=sINFO.m_dwCLUB_MARK_VER )	return true;

	//m_vPOS;		//	항상 변하는 자료.
	//m_dwCOUNT;

	return false;
}

void CROWREN::INIT ( GLCharacter *pMYCHAR )
{
	m_emCROW = CROW_PC;
	m_dwID = pMYCHAR->GetCtrlID();
	m_wSCHOOL = pMYCHAR->m_wSchool;

	StringCchCopy ( m_szNAME, CHAR_SZNAME, pMYCHAR->m_szName );
}

void CROWREN::SETTYPEFLAG ( GLCharacter *pMYCHAR )
{
	m_emPARTY = OTHER_PLAYER;
	if ( pMYCHAR->IsPartyMaster() )				m_emPARTY = PARTY_MASTER;
	else if ( pMYCHAR->IsPartyMem() )			m_emPARTY = PARTY_MEMBER;

	m_dwTYPE = TYPE_NORMAL;
	if ( m_emPARTY==PARTY_MASTER || m_emPARTY==PARTY_MEMBER )		m_dwTYPE |= TYPE_OURS;

	PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap();
	if ( pLAND )
	{
		if ( pLAND->m_bClubBattle || pLAND->m_bClubBattleHall )
		{
			if ( pMYCHAR->IsCDCertify() )									m_dwTYPE |= TYPE_CLUBCD;
		}
	}

	m_bPMARKET = pMYCHAR->m_sPMarket.IsOpen();
	StringCchCopy ( m_szPMARKET, CHAT_MSG_SIZE+1, pMYCHAR->m_sPMarket.GetTitle().c_str() );
	StringCchCopy ( m_szNICK, CHAR_SZNAME, pMYCHAR->m_szNick );

	// Note : 이름표시 높이조절
	m_vPOS = pMYCHAR->GetPosBodyHeight();

	m_dwCLUB = pMYCHAR->m_sCLUB.m_dwID;
	m_dwCLUB_MARK_VER = pMYCHAR->m_sCLUB.m_dwMarkVER;

	//	Note : 이름 색.
	if ( m_dwTYPE&TYPE_OURS )
	{
		m_dwCOLOR = NS_UITEXTCOLOR::GREENYELLOW;
	}
	else
	{
		m_dwCOLOR = pMYCHAR->GET_PK_COLOR();
	}
}

void CROWREN::INIT ( GLCharClient *pCHAR )
{
	m_emCROW = CROW_PC;
	m_dwID = pCHAR->GetCtrlID();
	m_wSCHOOL = pCHAR->GetCharData().wSchool;

	StringCchCopy ( m_szNAME, CHAR_SZNAME, pCHAR->GetCharData().szName );
}

void CROWREN::SETTYPEFLAG ( GLCharacter *pMYCHAR, GLCharClient *pCHAR )
{
	m_dwTYPE = TYPE_NORMAL;

	//	Note : 대련 검사.
	const EMCONFT_TYPE& emTYPE = pMYCHAR->GetConfting().emTYPE;
	BOOL bCONFT_TAR = pMYCHAR->ISCONFRONT_TAR ( STARGETID(CROW_PC,pCHAR->GetCtrlID()) );
	if ( bCONFT_TAR )
	{
		m_dwTYPE |= TYPE_TARGET;

		if ( pCHAR->IsPartyMaster () )		m_dwTYPE |= TYPE_PARTYMASTER;
		else if ( pCHAR->IsPartyMem () )	m_dwTYPE |= TYPE_PARTYMEMBER;

		if ( emTYPE==EMCONFT_GUILD )
		{
			if ( pCHAR->IsClubMaster () )	m_dwTYPE |= TYPE_CLUBMASTER;
			else							m_dwTYPE |= TYPE_CLUBMEMBER;
		}
	}
	else
	{
		//if ( emTYPE == EMCONFT_GUILD )
		//{
		//	const DWORD& dwCharID = pCHAR->GetCharID();
		//	if ( pMYCHAR->m_sCLUB.ISCONFT_MEMBER ( dwCharID ) )
		//	{
		//		m_dwTYPE |= TYPE_CLUBMEMBER;
		//	}
		//}

		//	Note : 같은 파티원인지 검사.
		//
		DWORD dwPARTYID = GLPartyClient::GetInstance().GetPartyID();
		if ( dwPARTYID!=PARTY_NULL && dwPARTYID==pCHAR->GETPARTYID() )
		{
			if ( pCHAR->IsPartyMaster() )		m_dwTYPE |= TYPE_OURS | TYPE_PARTYMASTER;
			else								m_dwTYPE |= TYPE_OURS | TYPE_PARTYMEMBER;
		}
		
		//	파티 마스터인 경우
		if ( pCHAR->IsPartyMaster () )			m_dwTYPE |= TYPE_PARTYMASTER;
		else if ( pCHAR->IsPartyMem() )			m_dwTYPE |= TYPE_PARTYMEMBER;

		//	클럽 마스터인 경우.
		if ( pCHAR->IsClubMaster () )			m_dwTYPE |= TYPE_CLUBMASTER;

		//	
		PLANDMANCLIENT pLAND = GLGaeaClient::GetInstance().GetActiveMap();
		if ( pLAND )
		{
			if ( pLAND->m_bClubBattle || pLAND->m_bClubBattleHall )
			{
				if ( pCHAR->IsCDCertify() )				m_dwTYPE |= TYPE_CLUBCD;
			}
		}
	}

	//	Note : 적대 행위자.
	if ( pMYCHAR->IsPK_TAR(pCHAR,false) )
	{
		if ( pMYCHAR->IsClubBattle_TAR(pCHAR) )	m_dwTYPE |= TYPE_CLUBBATTLE;			
		else m_dwTYPE |= TYPE_TARGET;
	}

	//	Note : PARTY 속성.
	m_emPARTY = OTHER_PLAYER;
	if ( m_dwTYPE&TYPE_PARTYMASTER )			m_emPARTY = PARTY_MASTER_GENERAL;
	if ( m_dwTYPE&TYPE_TARGET )
	{
		if ( m_dwTYPE&TYPE_PARTYMASTER )		m_emPARTY = PARTY_MASTER_TARGET;
		else if ( m_dwTYPE&TYPE_PARTYMEMBER )	m_emPARTY = PARTY_MEMBER_TARGET;
		else									m_emPARTY = SOLO_TARGET;
	}
	else  if ( m_dwTYPE&TYPE_OURS )
	{
		if ( m_dwTYPE&TYPE_PARTYMASTER )		m_emPARTY = PARTY_MASTER;
		if ( m_dwTYPE&TYPE_PARTYMEMBER )		m_emPARTY = PARTY_MEMBER;
	}

	//	Note : 이름 색.
	if ( m_dwTYPE&TYPE_OURS )
	{
		m_dwCOLOR = NS_UITEXTCOLOR::GREENYELLOW;
	}
	else if ( m_dwTYPE&TYPE_CLUBBATTLE )
	{
		DWORD dwClubCnt = pMYCHAR->GetClubColorIndex( pCHAR );
		m_dwCOLOR = NS_UITEXTCOLOR::CLUB_BATTLE_COLOR[dwClubCnt];
	}
	else if ( m_dwTYPE&TYPE_TARGET )
	{
		m_dwCOLOR = NS_UITEXTCOLOR::RED;
	}
	else
	{
		m_dwCOLOR = NS_UITEXTCOLOR::DEFAULT;
		m_dwCOLOR = pCHAR->GET_PK_COLOR();
	}

	//	Note : 이름 등등..
	m_bPMARKET = pCHAR->m_sPMarket.IsOpen();
	StringCchCopy ( m_szPMARKET, CHAT_MSG_SIZE+1, pCHAR->m_sPMarket.GetTitle().c_str() );

	StringCchCopy ( m_szNICK, CHAR_SZNAME, pCHAR->GetCharData().szNick );

	// Note : 이름표시 높이조절
	m_vPOS = pCHAR->GetPosBodyHeight();

	m_dwCLUB = pCHAR->GetCharData().dwGuild;
	m_dwCLUB_MARK_VER = pCHAR->GetCharData().dwGuildMarkVer;
}

void CROWREN::INIT ( GLCrowClient *pCROW )
{
	m_emCROW = pCROW->GETCROW();
	m_dwID = pCROW->GetCtrlID();

	StringCchCopy ( m_szNAME, CHAR_SZNAME, pCROW->m_pCrowData->GetName() );
}

void CROWREN::SETTYPEFLAG ( GLCharacter *pMYCHAR, GLCrowClient *pCROW )
{
	EMCROW emCROW = pCROW->GETCROW();
	
	if ( emCROW == CROW_NPC )	m_dwCOLOR = NS_UITEXTCOLOR::ORANGE;
	else
	{
		WORD wMY_LEVEL = pMYCHAR->GETLEVEL();
		WORD wMOB_LEVEL = pCROW->m_pCrowData->m_wLevel;
		float fEXP_RATE = GLCONST_CHAR::GETEXP_RATE ( wMY_LEVEL, wMOB_LEVEL );

		if ( fEXP_RATE==0 )		m_dwCOLOR = CROWCOLOR::MOB_COLOR_GRAY;
		else					m_dwCOLOR = CROWCOLOR::MOB_COLOR_RED;
	}

	// Note : 이름표시 높이조절
	m_vPOS = pCROW->GetPosBodyHeight();
}

void CROWREN::INIT ( GLMaterialClient *pMaterial )
{
	m_emCROW = pMaterial->GetCrow();
	m_dwID = pMaterial->GetCtrlID();

	StringCchCopy ( m_szNAME, CHAR_SZNAME, pMaterial->m_pCrowData->GetName() );
}

void CROWREN::SETTYPEFLAG ( GLCharacter *pMYCHAR, GLMaterialClient *pMaterial )
{
	WORD wMY_LEVEL = pMYCHAR->GETLEVEL();
	WORD wMOB_LEVEL = pMaterial->m_pCrowData->m_wLevel;
	float fEXP_RATE = GLCONST_CHAR::GETEXP_RATE ( wMY_LEVEL, wMOB_LEVEL );

	if ( fEXP_RATE==0 )		m_dwCOLOR = CROWCOLOR::MOB_COLOR_GRAY;
	else					m_dwCOLOR = CROWCOLOR::MOB_COLOR_RED;

	// Note : 이름표시 높이조절
	m_vPOS = pMaterial->GetPosBodyHeight();
}

void CROWREN::INIT ( CItemClientDrop *pITEM )
{
	m_emCROW = CROW_ITEM;
	m_dwID = pITEM->dwGlobID;

	SITEM *pITEM_DATA = GLItemMan::GetInstance().GetItem ( pITEM->sItemClient.sNativeID );

	/*// 드랍된 아이템이 겹침 아이템일 경우 수량 표시 ( 준혁 )
	// 기획팀에서 할 필요 없다고 해서 주석 처리함 ㅡㅜ;
	#ifdef DAUMTESTPARAM
	{
		if( pITEM->sItemClient.wTurnNum > 1 )
		{
			CString strDescText = ID2GAMEWORD( "ITEM_TURN_INFO", 0 );
			CString strName;
			strName.Format( "%s [%s:%d]", pITEM_DATA->GetName(), strDescText.GetString(), pITEM->sItemClient.wTurnNum );
			StringCchCopy( m_szNAME, CHAR_SZNAME, strName.GetString() );
		}
		else
		{
			StringCchCopy( m_szNAME, CHAR_SZNAME, pITEM_DATA->GetName() );
		}
	}
	#else
	*/
	{
		StringCchCopy( m_szNAME, CHAR_SZNAME, pITEM_DATA->GetName() );
	}
	//#endif

	m_vPOS = pITEM->vPos;
}

void CROWREN::SETTYPEFLAG ( GLCharacter *pMYCHAR, CItemClientDrop *pITEM )
{
	m_dwCOLOR = NS_UITEXTCOLOR::ITEMMONEY;
}

void CROWREN::INIT ( CMoneyClientDrop *pMONEY )
{
	m_emCROW = CROW_MONEY;
	m_dwID = pMONEY->dwGlobID;

	// 드랍된 돈에 자릿수 문자 표시 ( 준혁 )
	//#if defined(KR_PARAM) || defined(KRT_PARAM)
	//{
		CString strMoney = NS_UITEXTUTIL::MAKE_MONEY_FORMAT( pMONEY->lnAmount, 3, "," );
		StringCchCopy( m_szNAME, CHAR_SZNAME, strMoney.GetString() );
	//}
	//#else
	//{
	//	//std::strstream strSTEAM;
	//	//strSTEAM << pMONEY->lnAmount << std::ends;

	//	TCHAR szMoney[128] = {0};
	//	_snprintf( szMoney, 128, "%I64d", pMONEY->lnAmount );

	//	StringCchCopy ( m_szNAME, CHAR_SZNAME, szMoney );
	//	//strSTEAM.freeze( false );
	//}
	//#endif
}

void CROWREN::SETTYPEFLAG ( GLCharacter *pMYCHAR, CMoneyClientDrop *pMONEY )
{
	m_dwCOLOR = NS_UITEXTCOLOR::ITEMMONEY;
	m_vPOS = pMONEY->vPos;
}

void CROWREN::UPDATE ( DWORD _COUNT, D3DXVECTOR3 _vPOS )
{
	m_vPOS = _vPOS;
	m_dwCOUNT = _COUNT;
}

void CROWREN::INIT ( PGLPETCLIENT pMyPet )
{
	m_emCROW = CROW_PET;
	m_dwID = pMyPet->m_dwGUID;

	StringCchCopy ( m_szNAME, PETNAMESIZE+1, pMyPet->m_szName );
}

void CROWREN::INIT ( PGLANYPET pAnyPet )
{
	m_emCROW = CROW_PET;
	m_dwID = pAnyPet->m_dwGUID;

	StringCchCopy ( m_szNAME, PETNAMESIZE+1, pAnyPet->m_szName );
}


void CROWREN::SETTYPEFLAG ( GLCharacter *pMYCHAR, PGLPETCLIENT pMyPet )
{
	// Note : 이름표시 높이조절
	m_vPOS = pMyPet->GetPosition();
	m_vPOS.y += pMyPet->m_fHeight;

	// 색깔
	m_dwCOLOR = NS_UITEXTCOLOR::GREENYELLOW;
}

void CROWREN::SETTYPEFLAG ( GLCharacter *pMYCHAR, PGLANYPET pAnyPet )
{
	// Note : 이름표시 높이조절
	m_vPOS = pAnyPet->GetPosition();
	m_vPOS.y += pAnyPet->m_fHeight;

	// 색깔
	m_dwCOLOR = NS_UITEXTCOLOR::GREENYELLOW;
}


void CROWREN::INIT ( PGLSUMMONCLIENT pMySummon )
{
	m_emCROW = CROW_SUMMON;
	m_dwID = pMySummon->m_dwGUID;

	StringCchCopy ( m_szNAME, CHAR_SZNAME+1, pMySummon->GETNAME() );
}
void CROWREN::INIT ( PGLANYSUMMON pAnySummon )
{
	m_emCROW = CROW_SUMMON;
	m_dwID = pAnySummon->m_dwGUID;

	StringCchCopy ( m_szNAME, CHAR_SZNAME+1, pAnySummon->GETNAME() );
}


void CROWREN::SETTYPEFLAG ( GLCharacter *pMYCHAR, PGLSUMMONCLIENT pMySummon )
{
	// Note : 이름표시 높이조절
	m_vPOS = pMySummon->GetPosition();
	m_vPOS.y += pMySummon->GetBodyHeight();

	// 색깔
	m_dwCOLOR = NS_UITEXTCOLOR::ORANGE;
}

void CROWREN::SETTYPEFLAG ( GLCharacter *pMYCHAR, PGLANYSUMMON pAnySummon )
{
	// Note : 이름표시 높이조절
	m_vPOS = pAnySummon->GetPosition();
	m_vPOS.y += pAnySummon->GetBodyHeight();

	// 색깔
	m_dwCOLOR = NS_UITEXTCOLOR::ORANGE;
}
