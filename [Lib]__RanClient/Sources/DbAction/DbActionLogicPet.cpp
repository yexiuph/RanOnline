#include "pch.h"
#include "./DbActionLogic.h"

#include "../[Lib]__NetServer/Sources/s_CSessionServer.h"
#include "../[Lib]__NetServer/Sources/s_CFieldServer.h"
#include "../[Lib]__NetServer/Sources/s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* ���ο� ���� �����Ѵ�.
* \param nChaNum  �� ������ ĳ���� ��ȣ
* \param szPetName ���̸�
* \param nPetType ���� Ÿ��
* \param nPetMID �� MID
* \param nPetSid �� SID
* \param nPetStyle ���� ��Ÿ��
* \param nPetColor ���� �÷�
* \return DB_ERROR �Ǵ� ������������ Pet �� ������ȣ
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
	m_strPetName.Trim(_T(" ")); // �յ� ��������
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
											m_nPetCardMID, // PetCard�� MID �߰�
											m_nPetCardSID  // PetCard�� SID �߰�
											);

	GLMSG::SNETPET_CREATEPET_FROMDB_FB InternalMsg;

	// ������������ ���� �޽��� �߼�
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
		// �����ϸ� ��ī���� �ֹ�ȣ�� �ٽ� 0���� �ǵ��� ���´�. (����� �����ϰ� �ϱ� ����)
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
* ���� �̸��� �����Ѵ�.
* \param nPetNum ���� ������ȣ
* \param szPetName ���̸�	
*/
CRenamePet::CRenamePet(DWORD dwClientID, int nChaNum, int nPetNum, const TCHAR* szPetName, WORD wPosX, WORD wPosY)
	: m_nChaNum ( nChaNum )
	, m_nPetNum(nPetNum)
	, m_wPosX(wPosX)
	, m_wPosY(wPosY)
{	
	m_dwClient = dwClientID;
	m_strPetName = szPetName;
	m_strPetName.Trim(_T(" ")); // �յ� ��������
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
* ���� �������� �����Ѵ�.
* \param nChaNum ���� ���ο� ������
* \param nPetNum �������� ������ �� ��ȣ
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
* ���� �÷��� �����Ѵ�.
* \param nPetNum �÷��� �����Ϸ��� ���� ��ȣ
* \param nPetColor ���� �÷�
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
* ���� ��Ÿ���� �����Ѵ�.
* \param nPetNum ��Ÿ���� �����Ϸ��� ���� ��ȣ
* \param nPetStyle ���� ��Ÿ��
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
* ���� �������� �����Ѵ�. �������� 0-100 ����
* \param nPetNum �������� ������ ���� ��ȣ
* \param fPetFull ������
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
* ���� ��ų ������Ʈ
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
	// PET ��ų ������Ʈ
	if (m_bActive)
	{
		// Ȱ��ȭ �� ��ų�϶�		
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
		// Ȱ��ȭ ���� ���� ��ų�϶�
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
* ���� �κ��丮 ������Ʈ
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
* ���� �����Ѵ�.
* \param nPetNum ������ ���� ��ȣ
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
* ���� ������ �����´�.
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

	// ������ �ҷ����⸦ ������������ �޽��� ó��
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
	// �����ϸ� �޸� ����
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
* ���� �������� �����´�
* \param dwPetNum �� ��ȣ
* \return ���� ������ or DB_ERROR
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
* �Ҹ�� ���� ����Ʈ�� �����´�.
* \param nChaNum �Ҹ�� �� ����Ʈ�� ����Ϸ��� ĳ���� ��ȣ
* \param vecPetReList �Ҹ�� �� ����Ʈ�� ���� ����
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
        // ���θ޽����� �߻����Ѽ� ���������
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
		// ����ó��
	}

	return nResult;
}

/**
* ���� ��Ȱ��Ų��.
* \param nPetNum ��Ȱ�� �� ��ȣ
* \param nChaNum ��Ȱ�� �䱸�� ĳ���� ��ȣ
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
		// ��Ȱ�� ���� ���θ޽����� �������ش�.
		GLMSG::SNETPET_REQ_REVIVE_FROMDB_FB InternalMsg;
		InternalMsg.wPosX   = m_wPosX;
		InternalMsg.wPosY   = m_wPosY;
		InternalMsg.dwPetID = m_dwPetNum;

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_dwClient, (char*) &InternalMsg );
	}
	else
	{
		// ����ó��
		GLMSG::SNETPET_REQ_REVIVE_FB NetMsg;
		NetMsg.emFB = EMPET_REQ_REVIVE_FB_DBFAIL;
		NetMsg.dwPetID = m_dwPetNum;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClient, (LPVOID)&NetMsg );
	}
	return nResult;
}

/**
* �� ��Ų ���� ī�带 �����Ų��.
* \param nPetNum ��Ȱ�� �� ��ȣ
* \param nChaNum ��Ȱ�� �䱸�� ĳ���� ��ȣ
* \return �� ��ȣ or DB_ERROR
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
