#include "pch.h"
#include "./DbActionLogic.h"

#include "../[Lib]__NetServer/Sources/s_CSessionServer.h"
#include "../[Lib]__NetServer/Sources/s_CFieldServer.h"
#include "../[Lib]__NetServer/Sources/s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 새로운 팻을 생성한다.
* \param nChaNum  팻 소유자 캐릭터 번호
* \param szPetName 팻이름
* \param nPetType 팻의 타입
* \param nPetMID 팻 MID
* \param nPetSid 팻 SID
* \param nPetStyle 팻의 스타일
* \param nPetColor 팻의 컬러
* \return DB_ERROR 또는 성공했을때는 Pet 의 고유번호
*/
CCreatePet::CCreatePet(
    DWORD dwClientID,
	int nChaNum,
	const TCHAR* szPetName,
	int nPetType,
	int nPetMID,
	int nPetSID,
	int nPetStyle,
	int nPetColor,
	WORD wPosX,
	WORD wPosY,
	int nPetCardMID,
	int nPetCardSID)
	: m_nChaNum(nChaNum)
	, m_nPetType(nPetType)
	, m_nPetMID(nPetMID)
	, m_nPetSID(nPetSID)
	, m_nPetStyle(nPetStyle)
	, m_nPetColor(nPetColor)
	, m_wPosX(wPosX)
	, m_wPosY(wPosY)
	, m_nPetCardMID(nPetCardMID)
	, m_nPetCardSID(nPetCardSID)
{
	m_dwClient = dwClientID;
	m_strPetName = szPetName;	
	m_strPetName.Trim(_T(" ")); // 앞뒤 공백제거
}

int CCreatePet::Execute(CServer* pServer)
{
	int nResult =  m_pDbManager->CreatePet( m_nChaNum,
											m_strPetName.GetString(),
											m_nPetType,
											m_nPetMID,
											m_nPetSID,
											m_nPetStyle,
											m_nPetColor,
											m_nPetCardMID, // PetCard의 MID 추가
											m_nPetCardSID  // PetCard의 SID 추가
											);

	GLMSG::SNETPET_CREATEPET_FROMDB_FB InternalMsg;

	// 성공했을때만 내부 메시지 발송
	if ( nResult != DB_ERROR )
	{
		InternalMsg.dwPetID = nResult;
		InternalMsg.wPosX = m_wPosX;
		InternalMsg.wPosY = m_wPosY;

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_dwClient, (char*) &InternalMsg );
	}
	else
	{
		// 실패하면 팻카드의 팻번호를 다시 0으로 되돌려 놓는다. (재생성 가능하게 하기 위해)
		PGLCHAR pOwner = GLGaeaServer::GetInstance().GetCharID ( m_nChaNum );
		if ( pOwner )
		{
			SINVENITEM* pInvenItem = pOwner->m_cInventory.FindPosItem ( m_wPosX, m_wPosY );
			if ( pInvenItem ) pInvenItem->sItemCustom.dwPetID = 0;

			pOwner->m_bGetPetFromDB = false;
		}
	}

	return nResult;
}

/**
* 팻의 이름을 변경한다.
* \param nPetNum 팻의 고유번호
* \param szPetName 팻이름	
*/
CRenamePet::CRenamePet(DWORD dwClientID, int nChaNum, int nPetNum, const TCHAR* szPetName, WORD wPosX, WORD wPosY)
	: m_nChaNum ( nChaNum )
	, m_nPetNum(nPetNum)
	, m_wPosX(wPosX)
	, m_wPosY(wPosY)
{	
	m_dwClient = dwClientID;
	m_strPetName = szPetName;
	m_strPetName.Trim(_T(" ")); // 앞뒤 공백제거
}

int CRenamePet::Execute(CServer* pServer)
{
	if (pServer == NULL)
	{
		return DB_ERROR;
	}
	else
	{
		int nRESULT = m_pDbManager->RenamePet(m_nChaNum, m_nPetNum, m_strPetName.GetString());

		GLMSG::SNETPET_REQ_RENAME_FROMDB_FB NetMsgDB2Fld;

		if ( nRESULT == DB_OK )
		{
			NetMsgDB2Fld.emFB = EMPET_REQ_RENAME_FB_FROM_DB_OK;
			NetMsgDB2Fld.wPosX = m_wPosX;
			NetMsgDB2Fld.wPosY = m_wPosY;
			StringCchCopy ( NetMsgDB2Fld.szName, PETNAMESIZE+1, m_strPetName.GetString() );
		}
		else
		{
			NetMsgDB2Fld.emFB = EMPET_REQ_RENAME_FB_FROM_DB_FAIL;
			StringCchCopy ( NetMsgDB2Fld.szName, PETNAMESIZE+1, m_strPetName.GetString() );
		}

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg(m_dwClient, (char*) &NetMsgDB2Fld );
		return nRESULT;
	}
}

/**
* 팻의 소유권을 이전한다.
* \param nChaNum 팻의 새로운 소유자
* \param nPetNum 소유권을 이전할 팻 번호
*/
CExchangePet::CExchangePet(int nChaNum, int nPetNum)
	: m_nChaNum(nChaNum)
	, m_nPetNum(nPetNum)
{
}

int CExchangePet::Execute(CServer* pServer)
{
	return m_pDbManager->ExchangePet(m_nChaNum, m_nPetNum);
}

/**
* 팻의 컬러를 변경한다.
* \param nPetNum 컬러를 변경하려는 팻의 번호
* \param nPetColor 팻의 컬러
*/
CSetPetColor::CSetPetColor(int nChaNum, int nPetNum, int nPetColor)
	: m_nChaNum ( nChaNum )
	, m_nPetNum(nPetNum)
	, m_nPetColor(nPetColor)
{	
}
	
int CSetPetColor::Execute(CServer* pServer)
{
	return m_pDbManager->SetPetColor( m_nChaNum, m_nPetNum, m_nPetColor);
}

/**
* 팻의 스타일을 변경한다.
* \param nPetNum 스타일을 변경하려는 팻의 번호
* \param nPetStyle 팻의 스타일
*/
CSetPetStyle::CSetPetStyle(int nChaNum, int nPetNum, int nPetStyle)
	: m_nChaNum ( nChaNum )
	, m_nPetNum(nPetNum)
	, m_nPetStyle(nPetStyle)
{	
}

int CSetPetStyle::Execute(CServer* pServer)
{
	return m_pDbManager->SetPetStyle(m_nChaNum, m_nPetNum, m_nPetStyle);
}

/**
* 팻의 포만도를 설정한다. 포만도는 0-100 사이
* \param nPetNum 포만도를 설정할 팻의 번호
* \param fPetFull 포만도
*/
CSetPetFull::CSetPetFull(
    int	nChaNum,
	int nPetNum, 
	int nPetFull)
	: m_nChaNum ( nChaNum )
	, m_nPetNum(nPetNum)
	, m_nPetFull(nPetFull)

{
}

int CSetPetFull::Execute(CServer* pServer)
{
	return m_pDbManager->SetPetFull(m_nChaNum, m_nPetNum, m_nPetFull);
}

/**
* 팻의 스킬 업데이트
*/
CSetPetSkill::CSetPetSkill(
	int nChaNum,
	int nPetNum,
	PETSKILL& PetSkill,
	bool bActive)
	: m_nChaNum(nChaNum)
	, m_nPetNum(nPetNum)	
	, m_bActive(bActive)
{
	m_PetSkill = PetSkill;
}

int CSetPetSkill::Execute(CServer* pServer)
{
	// PET 스킬 업데이트
	if (m_bActive)
	{
		// 활성화 된 스킬일때		
		m_pDbManager->SetPetSkill(
			m_nChaNum,
			m_nPetNum,
			3,		
			m_PetSkill.sNativeID.wMainID,
			m_PetSkill.sNativeID.wSubID,
			0,
			0,
			1);
	}
	else
	{
		// 활성화 되지 않은 스킬일때
		m_pDbManager->SetPetSkill(
			m_nChaNum,
			m_nPetNum,
			3,		
			m_PetSkill.sNativeID.wMainID,
			m_PetSkill.sNativeID.wSubID,
			0,
			0,
			0);
	}
	return DB_OK;
}

/**
* 팻의 인벤토리 업데이트
*/
CSetPetInven::CSetPetInven(
    int nChaNum,
	int nPetNum,
	GLPET* pPet)
	: m_nChaNum(nChaNum)
	, m_nPetNum(nPetNum)
{	
    pPet->GETPUTONITEMS_BYBUF( m_ByteStream );
}

int CSetPetInven::Execute(CServer* pServer)
{	
	return m_pDbManager->SetPetInven(
				m_nChaNum,
				m_nPetNum,
				m_ByteStream);
}

/**
* 팻을 삭제한다.
* \param nPetNum 삭제할 팻의 번호
*/
CDeletePet::CDeletePet(int nChaNum, int nPetNum)
	: m_nChaNum(nChaNum)
	, m_nPetNum(nPetNum)
{
}

int CDeletePet::Execute(CServer* pServer)
{
	return m_pDbManager->DeletePet(m_nChaNum,m_nPetNum);
}

/**
* 팻의 정보를 가져온다.
*/
CGetPet::CGetPet(
	GLPET* pPet,
	DWORD dwPetNum,
	DWORD dwClientID,
	DWORD dwCharNum,
	WORD  wPosX,
	WORD  wPosY,
	bool bLMTItemCheck,
	bool bCardInfo,
	bool bTrade)
	: m_pPet(pPet)
	, m_dwPetNum(dwPetNum)
	, m_wPosX ( wPosX )
	, m_wPosY ( wPosY )
	, m_bLMTItemCheck(bLMTItemCheck)
	, m_bCardInfo(bCardInfo)
	, m_bTrade(bTrade)
{
	m_dwClient = dwClientID;
	m_dwCharNum = dwCharNum;
}

int CGetPet::Execute(CServer* pServer)
{
	int nResult = m_pDbManager->GetPet(m_pPet, m_dwPetNum, m_dwCharNum);

	// 팻정보 불러오기를 성공했을때만 메시지 처리
	if ( nResult == DB_OK )
	{
		GLMSG::SNETPET_GETPET_FROMDB_FB InternalMsg;
		InternalMsg.pPet		  = m_pPet;
		InternalMsg.dwPetID		  = m_dwPetNum;
		InternalMsg.bLMTItemCheck = m_bLMTItemCheck;
		InternalMsg.bCardInfo	  = m_bCardInfo;
		InternalMsg.bTrade		  = m_bTrade;

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_dwClient, (char*) &InternalMsg );
	}
	// 실패하면 메모리 해제
	else if ( nResult == DB_ROWCNT_ERROR ) 
	{

		GLMSG::SNETPET_GETPET_FROMDB_ERROR InternalMsg;
		InternalMsg.bLMTItemCheck = m_bLMTItemCheck;
		InternalMsg.bCardInfo	  = m_bCardInfo;
		InternalMsg.bTrade		  = m_bTrade;
		InternalMsg.wPosX		  = m_wPosX;
		InternalMsg.wPosY		  = m_wPosY;

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_dwClient, (char*) &InternalMsg );

		SAFE_DELETE ( m_pPet );
	}
	else
	{
		PGLCHAR pOwner = GLGaeaServer::GetInstance().GetCharID ( m_dwCharNum );
		if ( pOwner )
		{
			pOwner->m_bGetPetFromDB = false;
		}

		SAFE_DELETE ( m_pPet );
	}

	return nResult;
}

/**
* 팻의 포만감을 가져온다
* \param dwPetNum 팻 번호
* \return 팻의 포만감 or DB_ERROR
*/
CGetPetFull::CGetPetFull(
    int   nChaNum,
	DWORD dwPetNum,
	DWORD dwClientID,
	WORD wCureVolume,
	BOOL bRatio,
	SNATIVEID sID,
	PETTYPE emType )
	: m_nChaNum( nChaNum )
	, CDbAction( dwClientID )
	, m_dwPetNum( dwPetNum )
	, m_wCureVolume(wCureVolume)
	, m_bRatio(bRatio)
	, m_sNativeID(sID)
	, m_emType(emType)
{
	m_dwClient = dwClientID;
}

int CGetPetFull::Execute( CServer* pServer )
{
	int nResult = m_pDbManager->GetPetFull( m_nChaNum, m_dwPetNum );

	GLMSG::SNETPET_REQ_GETFULL_FROMDB_FB InternalMsg;

	if ( nResult != DB_ERROR )
	{
		InternalMsg.dwPetID		 = m_dwPetNum;
		InternalMsg.nFull		 = nResult;
		InternalMsg.wCureVolume  = m_wCureVolume;
		InternalMsg.bRatio		 = m_bRatio;
		InternalMsg.sNativeID    = m_sNativeID;
		InternalMsg.emFB		 = EMPET_REQ_GETFULL_FROMDB_OK;
		InternalMsg.emType		 = m_emType;

	}
	else
	{
		InternalMsg.emFB		 = EMPET_REQ_GETFULL_FROMDB_FAIL;
	}

	CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
	pTemp->InsertMsg ( m_dwClient, (char*) &InternalMsg );

	return nResult;
}

/**
* 소멸된 팻의 리스트를 가져온다.
* \param nChaNum 소멸된 펫 리스트를 출력하려는 캐릭터 번호
* \param vecPetReList 소멸된 펫 리스트를 넣을 벡터
*/
CGetRestorePetList::CGetRestorePetList( int nChaNum, DWORD dwClientID ) : m_nChaNum(nChaNum)
{
	m_dwClient = dwClientID;
}

int CGetRestorePetList::Execute( CServer* pServer )
{
    std::vector<PET_RELIST> vecPetReList;
	int nResult = m_pDbManager->GetRestorePetList( m_nChaNum, vecPetReList );

	if( nResult != DB_ERROR )
	{
		GLMSG::SNETPET_REQ_PETREVIVEINFO_FB NetMsg;

		WORD wNum(0);
        // 내부메시지를 발생시켜서 전달해줘라
		for ( WORD i = 0; i < vecPetReList.size(); ++i )
		{
			NetMsg.arryPetReviveInfo[wNum].dwPetID	   = vecPetReList[i].nPetNum;
			NetMsg.arryPetReviveInfo[wNum].sPetCardID  = vecPetReList[i].sPetCardID;
			NetMsg.arryPetReviveInfo[wNum].emType	   = vecPetReList[i].emType;
			StringCchCopy ( NetMsg.arryPetReviveInfo[wNum].szPetName, PETNAMESIZE+1, vecPetReList[i].szPetName );
			if ( ++wNum >= MAXPETREVINFO )
			{
				NetMsg.wPetReviveNum = MAXPETREVINFO;
				GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClient, (LPVOID)&NetMsg );
				wNum = 0;
			}
		}

		if ( wNum > 0 )
		{
			NetMsg.wPetReviveNum = wNum;
			GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClient, (LPVOID)&NetMsg );
		}
	}
	else
	{
		// 실패처리
	}

	return nResult;
}

/**
* 팻을 부활시킨다.
* \param nPetNum 부활할 팻 번호
* \param nChaNum 부활을 요구한 캐릭터 번호
*/
CRestorePet::CRestorePet( DWORD dwPetNum, 
						  DWORD	dwClientID,
						  int nChaNum, 
						  WORD wPosX, 
						  WORD wPosY ) : 
						  m_dwPetNum(dwPetNum),
                          m_nChaNum(nChaNum),
                          m_wPosX(wPosX),
                          m_wPosY(wPosY)
{
	m_dwClient = dwClientID;
}

int CRestorePet::Execute( CServer* pServer)
{
	int nResult = m_pDbManager->RestorePet( m_dwPetNum, m_nChaNum );

	if( nResult != DB_ERROR )
	{
		// 부활된 팻을 내부메시지로 전달해준다.
		GLMSG::SNETPET_REQ_REVIVE_FROMDB_FB InternalMsg;
		InternalMsg.wPosX   = m_wPosX;
		InternalMsg.wPosY   = m_wPosY;
		InternalMsg.dwPetID = m_dwPetNum;

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_dwClient, (char*) &InternalMsg );
	}
	else
	{
		// 실패처리
		GLMSG::SNETPET_REQ_REVIVE_FB NetMsg;
		NetMsg.emFB = EMPET_REQ_REVIVE_FB_DBFAIL;
		NetMsg.dwPetID = m_dwPetNum;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClient, (LPVOID)&NetMsg );
	}
	return nResult;
}

/**
* 팻 스킨 변경 카드를 적용시킨다.
* \param nPetNum 부활할 팻 번호
* \param nChaNum 부활을 요구한 캐릭터 번호
* \return 팻 번호 or DB_ERROR
*/
CPetSkinPack::CPetSkinPack( int nChaNum, 
							DWORD dwPetNum, 
							int nPetSkinMID, 
							int nPetSkinSID, 
							int nPetSkinScale, 
							int nPetSkinTime )
{
	m_nChaNum		= nChaNum;
	m_dwPetNum		= dwPetNum;
	m_nPetSkinMID	= nPetSkinMID;
	m_nPetSkinSID	= nPetSkinSID;
	m_nPetSkinScale	= nPetSkinScale;
	m_nPetSkinTime	= nPetSkinTime;
}

int CPetSkinPack::Execute(CServer* pServer)
{
	return m_pDbManager->UpdatePetSkin( m_nChaNum, m_dwPetNum, m_nPetSkinMID, m_nPetSkinSID, m_nPetSkinScale, m_nPetSkinTime );
}
