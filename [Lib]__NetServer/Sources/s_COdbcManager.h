#ifndef _CODBCMANAGER_H_
#define _CODBCMANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>
#include <stdio.h>
#include <odbcinst.h>
#include <sqlext.h> // ODBC library

#include <vector>
//#include <strstream>
#include <sstream>
#include <map>

#include "s_NetGlobal.h"
#include "s_CCfg.h"
#include "s_COdbcSupervisor.h"
#include "s_CLock.h"
#include "s_CConsoleMessage.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLDBMan.h"

#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLCharData.h"
#include "../[Lib]__RanClient/Sources/G-Logic/GLMsg/GLContrlMsg.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Server/GLCharAG.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLClubMan.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Server/GLGuidance.h"
#include "../[Lib]__RanClient/Sources/G-Logic/GLPet.h" // Pet Define

/**
 * \ingroup NetServerLib
 *
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2002-05-28
 *
 * \author jgkim
 *
 * \par license
 * Copyright 2002-2005 (c) Mincoms. All rights reserved. 
 * \todo 
 * 
 * \n Board Database 관련된 함수는 현재 없다. (구현일정 미정)
 *
 * \bug 
 *
 */
class COdbcManager : public GLDBMan, public CLock
{
public:	
	static COdbcManager* GetInstance();
	static void ReleaseInstance();

private:
// public:
	COdbcManager(void);
	~COdbcManager(void);
	static COdbcManager* SelfInstance;

protected:
   	COdbcSupervisor* m_pGameDB;
	COdbcSupervisor* m_pUserDB;
	COdbcSupervisor* m_pBoardDB;
	COdbcSupervisor* m_pLogDB;
	COdbcSupervisor* m_pShopDB;
	COdbcSupervisor* m_pTerraDB;
	COdbcSupervisor* m_pGspDB;
	COdbcSupervisor* m_pThaiDB;
	COdbcSupervisor* m_pKorDB;
	COdbcSupervisor* m_pMyDB;			 // 말레이시아 PC방 이벤트
	int              m_nServiceProvider; ///< 서비스 제공업자

public:
	void SetServiceProvider(int nServiceProvider);

	int	OpenGameDB(
			TCHAR* szOdbcName, 
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseGameDB();

	int	OpenUserDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID,
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseUserDB();

	int	OpenBoardDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseBoardDB();

	int	OpenLogDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID,
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseLogDB();

	int OpenShopDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID,
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseShopDB();
	
	int	OpenTerraDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseTerraDB();

	int	OpenGspDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseGspDB();

	int	OpenThaiDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseThaiDB();

	int	OpenKorDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseKorDB();

	// 말레이시아 PC방 이벤트
	int	OpenMyDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID,
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize = DB_POOL_SIZE,
			int nDBTimeOut = DB_RESPONSE_TIME );
	void CloseMyDB();


	void Print(const CString & strMsg);
	CString	GetErrorString(SQLHSTMT hStmt);
	void CheckConnection();

	/**************************************************************************
	* Game Database
    */
	//! 사용자 인벤토리에서 데이타를 읽어온다.
	int	ReadUserInven(
			int SGNum,
			DWORD dwUserNum,
			CByteStream &ByteStream );

	//! 유저인벤토리가 있는지 체크한다.
	bool CheckInven( int SGNum, DWORD dwUserNum );
	int	MakeUserInven( int SGNum, DWORD dwUserNum );
	//! 사용자 인벤토리에 데이타를 저장한다.
	int	WriteUserInven( 
			int SGNum, 
			LONGLONG llMoney /* LONGLONG __int64 */,
			DWORD dwUserNum,
			BYTE* pData,
			int nSize);
	
	// 새로운 캐릭터를 생성한다.
	int	CreateNewCharacter( SCHARDATA2* pCharData2 );
	// 대만,홍콩
	// 캐릭터 생성시 Temp Table에 캐릭터 레벨과 금액 정보를 암호화 해서 저장해 둔다.
	int	InsertCharInfoTemp( int nChaNum );

	virtual int ReadUserInven( SCHARDATA2* pChaData2 );

	//! 캐릭터를 저장한다
	virtual int	SaveCharacter( LPVOID _pbuffer );
	virtual void AddJob    ( CDbAction *pDbActJob );
	virtual void AddLogJob ( CDbAction *pDbActJob );
	virtual void AddUserJob( CDbAction *pDbActJob );
	virtual void AddWebJob ( CDbAction *pDbActJob );

	//! 캐릭터의 위치정보를 저장한다.
	//! \param dwChaNum 캐릭터 번호
	//! \param dwStartmapID 시작하는 맵 번호
	//! \param dwStartGate 시작하는 게이트 번호
	//! \param fStartPosX 시작하는 X 좌표
	//! \param fStartPosY 시작하는 Y 좌표
	//! \param fStartPosZ 시작하는 Z 좌표
	//! \param dwSaveMapID 마지막으로 플레이한 맵 번호
	//! \param fSavePosX 마지막으로 플레이한 X 좌표
	//! \param fSavePosY 마지막으로 플레이한 Y 좌표
	//! \param fSavePosZ 마지막으로 플레이한 Z 좌표
	//! \param dwLastCallMapID 시작 귀환카드에 저장된 맵 번호
	//! \param fLastCallPosX 시작 귀환카드에 저장된 X 좌표
	//! \param fLastCallPosY 시작 귀환카드에 저장된 Y 좌표
	//! \param fLastCallPosZ 시작 귀환카드에 저장된 Z 좌표
	//! \return DB_OK, DB_ERROR
	int	SaveChaPos(
			DWORD dwChaNum,
			DWORD dwStartMapID,
			DWORD dwStartGate,
			FLOAT fStartPosX,
			FLOAT fStartPosY,
			FLOAT fStartPosZ,
			DWORD dwSaveMapID,
			FLOAT fSavePosX,
			FLOAT fSavePosY,
			FLOAT fSavePosZ,
			DWORD dwLastCallMapID,
			FLOAT fLastCallPosX,
			FLOAT fLastCallPosY,
			FLOAT fLastCallPosZ );

	/**
	* 캐릭터 이름을 변경한다.
	* \param dwChaNum 캐릭터번호
	* \param szCharName 변경할 캐릭터이름
	* \return DB_OK, DB_ERROR
	*/
	int RenameCharacter(
			DWORD dwChaNum, 
			TCHAR* szCharName );

	/**
	* 캐릭터 정보를 가져온다 (바이너리 제거 데이타)
	* \param nUserNum 사용자번호
	* \param nChaNum 캐릭터번호
	* \return 캐릭터정보
	*/
	/*
	SCHARDATA2*	GetCharacter(
					int nUserNumber,
					int nChaNum );
	*/

	int GetChaPhoneNumber(
			int nChaNum,
			TCHAR* szPhoneNumber);
	/**	
	* 캐릭터 정보를 가져온다. 캐릭터의 모든정보 (바이너리 포함)
	* \param nUserNum 사용자번호
	* \param nChaNum 캐릭터번호
	* \param pChaData2 캐릭터 데이터를 받을 포인트
	* \return DB_OK, DB_ERROR
	*/
	int GetCharacterInfo(
			int nUserNumber,
		    int nChaNum,
			SCHARDATA2* pChaData2 );
	/**
	* 캐릭터가 시작할 맵, 게이트, 위치를 가져온다
	* \param nUserNum 사용자번호
	* \param nChaNum 캐릭터번호
	* \param pChaData 캐릭터 데이터를 받을 포인트
	* \return DB_OK, DB_ERROR
	*/
	int	GetCharacterInfo(
			int nUserNumber,
		    int nChaNum,
			GLCHARAG_DATA* pChaData );
	
	//! 해당 사용자의 캐릭터 정보를 가져온다.
	int	GetChaAllInfo( int nUsrNum );
	int	GetChaBAInfo( int nUsrNum, int nSvrGrp, NET_CHA_BBA_INFO* ncbi );
	int	GetChaBInfo( int nUserNum, int nChaNum, SCHARINFO_LOBBY* sci );
	
	// 캐릭터 삭제	
	int	DelCharacter( int nUsrNum, int nChaNum, const TCHAR* szPass2 );
	// 캐릭터 삭제
	int	DaumDelCharacter( int nUsrNum, int nChaNum );
	// 캐릭터 삭제
	int	TerraDelCharacter( int nUsrNum, int nChaNum );
	// GSP 캐릭터 삭제
	int	GspDelCharacter( int nUsrNum, int nChaNum );
	// 캐릭터 삭제
	int	ExciteDelCharacter( int nUsrNum, int nChaNum );
	// Japan 캐릭터 삭제
	int	JapanDelCharacter( int nUsrNum, int nChaNum );
	// 캐릭터 삭제	
	int	GsDelCharacter( int nUsrNum, int nChaNum );
	// 실제 캐릭터 삭제 함수
	int	DeleteCharacter( int nUsrNum, int nChaNum );


	// 베트남 탐직방지 시스템의 추가 정보 불러오는 부분 추가
	int COdbcManager::GetVTCharInfo( int nChaNum, SCHARDATA2* pChaData2 );
	// 대만 해킹 문제 관련해서 최근 로그아웃 했을때의 정보를 DB에서 읽어옴.
	int COdbcManager::GetLastCharInfo( int nUserNum, int nChaNum,SCHARDATA2* pChaData2 );

	/**
	* 캐릭터의 친구정보를 가져온다.
	*/
	int	GetChaFriend(
			int nChaNum,
			std::vector<CHA_FRIEND> &vFriend );

	int	DelChaFriend(
			int nChaP,
			int nChaS );
	int AddChaFriend(
			int nChaP,
			int nChaS );

	int SetChaFriend(
			int nChaP,
			int nChaS,
			int nFlag );

	int	LogPartyMatch(
			int nSGNum,
			int nSvrNum,
			WORD wWin,
			WORD wLost ); // 학교vs학교 파티대련결과저장
        
    /**
    * 해당 캐릭터의 경험치를 세팅한다.    
    * 주의 
    * 캐릭터번호는 0 보다 커야한다.
    * 경험치는 0 보다 커야한다. 
	* \param nChaNum 캐릭터번호 
	* \param llExp 경험치
	* \return 
	*/
    int SetChaExp(int nChaNum, LONGLONG llExp);
    
    /**
    * 해당 캐릭터의 경험치를 가져온다.    
	* 주의
	* 캐릭터번호는 0 보다 커야한다.
	* 경험치가 0 보다 작으면 에러이다.
    * \param nChaNum  캐릭터번호
    * \return (LONGLONG) 경험치
    */
    LONGLONG GetChaExp(int nChaNum);

    /**
    * 해당 락커 만료시간을 정한다.
    * 1 번 5 번 락커는 정할 수 없음.
    * 2, 3, 4 번 락커만 만료시간을 정할 수 있다.
    * 입력
    * \param nUserNum 사용자고유번호
    * \param nStorageNum 스토리지 번호
    * \param tTime 만료시간
    * \return -1 DB_ERROR 0 : DB_OK
	*/
	int SetChaStorageDate(int nUserNum, int nStorageNum, __time64_t tTime);

    /**
    * 해당 캐릭터의 인벤토리 추가 줄수를 세팅한다.
    * \n 주의 
    * \n 최초 0 에서 한줄 추가시 1 을 입력 
    * \n 현재 한줄 상태에서 또 한줄 추가시 2 를 입력 (최종 줄수를 입력한다)
    * \n 현재 두줄 상태에서 한줄 추가시 3 을 입력 
    * \n 최대 3까지만 입력가능.
	* \param nChaNum 캐릭터번호
    * \param wInvenLine 추가줄수 	
	* \return 
	*/
	int SetChaInvenNum(int nChaNum, WORD wInvenLine=0);

	/**
	* 직전귀환 카드 사용을 위한 포지션을 저장한다.
	* \param nChaNum 캐릭터번호
	* \param dwMapID 맵 ID
	* \param vPos    캐릭터 위치
	* \return 
	*/
    int SetLastCallPos(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos);
 
	/**
	* 새로운 클럽을 생성한다.
	* \param strClubName 클럽이름
	* \param dwChaNum    클럽을 생성하려는 캐릭터 번호 (이 캐릭터가 클럽의 마스터가 된다)
	* \return 
	*/
    int CreateClub(CString strClubName, DWORD dwChaNum);

	/**
	* 클럽을 삭제한다.
	* \param dwClub 클럽번호
	* \param dwChaNum 클럽을 삭제하려는 캐릭터번호(캐릭터 번호가 클럽 마스터가 아니면 삭제가 되지 않는다)
	* \return 
	*/
    int DeleteClub(DWORD dwClub, DWORD dwChaNum);    

	/**
	* 클럽랭킹을 세팅한다.
	* \param dwClub 클럽번호
	* \param dwRank 랭킹
	* \return 
	*/
    int SetClubRank(DWORD dwClub, DWORD dwRank);    

	/**
	* 클럽마크 이미지를 가져온다.
	* \param dwClub 클럽번호
	* \param &ByteStream 버퍼
	* \return 
	*/
    int ReadClubMarkImage(DWORD dwClub, CByteStream &ByteStream);    

	/**
	* 클럽마크 이미지를 저장한다.
	* \param dwClub 클럽번호
	* \param dwMarkVer 클럽마크버전
	* \param pData 클럽이미지 주소
	* \param nSize 클럽이미지 사이즈
	* \return 
	*/
    int	WriteClubMarkImage(
			DWORD dwClub,
			DWORD dwMarkVer,
			BYTE* pData,
			int nSize );
	
	/**
	* 해당클럽의 보유금액을 세팅한다. (Storage)
	* \param dwClub 클럽번호
	* \param llMoney 보유금액 (금액은 >= 0)
	* \return 
	*/
    int	SetClubMoney(DWORD dwClub, LONGLONG llMoney);    

	/**
	* 해당클럽의 보유금액을 가져온다. (Storage)
	* \param dwClub 클럽번호
	* \return 리턴값은 >= 0, 0 보다 작으면 ERROR
	*/
    LONGLONG GetClubMoney(DWORD dwClub);

	/**
	* 해당클럽의 수입금액을 세팅한다. (Income money)
	* \param dwClub 클럽번호
	* \param llMoney 수입금액 (금액은 >= 0)
	* \return 
	*/
    int SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney);    
    
	/**
	* 해당클럽의 수입금액을 가져온다. (Income money)
	* \param dwClub 클럽번호
	* \return >= 0, 0 보다 작으면 ERROR
	*/
    LONGLONG GetClubIncomeMoney(DWORD dwClub);    

	/**
	* 해당클럽의 클럽창고를 읽어온다.
	* \param dwClub 클럽번호
	* \param &ByteStream 버퍼
	* \return 
	*/
    int ReadClubStorage(DWORD dwClub, CByteStream &ByteStream);    

	/**
	* 해당클럽의 클럽창고를 저장한다.
	* \param dwClub 클럽번호
	* \param pData 창고위치
	* \param nSize 버퍼 전체크기
	* \return 
	*/
    int	WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize);

	/**
	* 클럽 해체시간을 설정한다.
	* \param dwClub 클럽번호
	* \param tDiss 해체시간
	* \return 
	*/
    int SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss);

	/**
	* 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
	* \param dwClub 클럽번호
	* \param tSec 시간
	* \return 
	*/
	int SetClubAllianceSec(DWORD dwClub, __time64_t tSec);

	/**
	* 클럽의 동맹해산 시간을 정한다.
	* \param dwClub 클럽번호
	* \param tDis 해산시간
	* \return 
	*/
	int SetClubAllianceDis(DWORD dwClub, __time64_t tDis);

	/**
	* 클럽의 마스터위임 시간을 정한다.
	* \param dwClub 클럽번호
	* \param tDis 마스터 위임시간
	* \return 
	*/
	int SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority);

	/**
	* 서버의 모든 클럽정보를 가져온다.
	* \param &vClub 클럽정보
	* \return 
	*/
    int GetClubInfo(std::vector<GLCLUBINFO> &vClub);
	
	/**
	* 클럽의 공지사항을 세팅한다.
	* \param dwClub 클럽번호
	* \param szClubNotice 공지사항
	* \return 
	*/
	int SetClubNotice(DWORD dwClub, const TCHAR* szClubNotice);

	/**
	* 클럽의 선도클럽 인증 대리자를 세팅한다.
	* \param dwClub 클럽번호
	* \param dwDeputy 대리인증자 캐릭터번호
	* \return 
	*/
	int SetClubDeputy(DWORD dwClub, DWORD dwDeputy);

	/**
	* 클럽의 마스터 권한을 위임한다.
	* \param dwClub 클럽번호
	* \param dwMasterID 마스터 권한 위임할 케릭터
	* \return 
	*/

	int SetClubAuthority (DWORD dwClub, DWORD dwMasterID);

	/**
	* 해당 클럽의 멤버를 가져온다
	* \param dwClub 클럽번호
	* \param &vMember 클럽멤버
	* \return 
	*/
    int GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER> &vMember);
    
	/**
	* 새로운 캐릭터를 클럽에 가입시킨다
	* \param dwClub 클럽번호
	* \param dwChaNum 가입시키려는 캐릭터번호
	* \return 
	*/
    int AddClubMember(DWORD dwClub, DWORD dwChaNum);

	/**
	* 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
	* \param dwChaNum 캐릭터번호
	* \return 
	*/
    int DeleteClubMember(DWORD dwChaNum);

	/**
	* 캐릭터의 클럽번호를 가져온다.
	* \param dwChaNum 캐릭터번호
	* \return 
	*/
    int GetChaGuildNum(DWORD dwChaNum);    

	/**
	* 지역과 그지역을 소유한 클럽 세율을 정한다.
	* \param dwRegionID 지역 ID
	* \param dwClub 클럽 ID
	* \param fTax 세율
	* \return 
	*/
    int SetClubRegion(DWORD dwRegionID, DWORD dwClub=0, float fTax=0);

	/**
	* 해당 지역의 소유 상태를 리셋한다.
	* \param dwRegionID 지역 ID
	* \param dwClub 클럽 ID (기본 0 이다)
	* \return 
	*/
    int DelClubRegion(DWORD dwRegionID, DWORD dwClub=0);    
    
	/**
	* 현재 모든지역의 선도 클럽데이터를 가져온다.
	* \n 주의:최초에는 가져올 값이 없다.
	* \param &vGUID_DB 
	* \return 
	*/
    int GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB);

	/**
	* 부 클럽장을 세팅한다
	* \param dwClub 클럽번호
	* \param dwChaNum 캐릭터번호
	* \param dwSubMasterFlags 플래그 (0 으로 넣으면 일반길드원이 된다)
	* \return 
	*/
	int	SetClubMasterFlags(DWORD dwClub, DWORD dwChaNum, DWORD dwSubMasterFlags);

	/**
	* 동맹 클럽을 결성한다.
    * \n 주의 
	* \n db 는 클럽의 무결성을 체크하지 않는다.
	* \n A 가 B 의 보조인 상태에서 
	* \n B 가 C 의 보조로 들어가고
	* \n C 가 A 의 보조로 들어가면 소유권 체인 문제가 발생한다.
	* \param dwClubP 주 클럽번호 (동맹 클럽의 주)
	* \param dwClubS 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
	* \return 
	*/
	int	SetClubAlliance(DWORD dwClubP, DWORD dwClubS);

	/**
	* 동맹 클럽을 해체한다.
	* \param dwClubP 주 클럽번호 (동맹 클럽의 주)
	* \param dwClubS 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
	* \return 
	*/
	int	DelClubAlliance(DWORD dwClubP, DWORD dwClubS);

	/**
	* 동맹 클럽정보를 가져온다.
	* \n 아직 게임 Logic쪽에서 구현이 안된관계로 구현되어 있지 않다.
	* \return 
	*/
	int	GetClubAlliance ( std::vector< std::pair<DWORD,DWORD> > &vecAlliance );
	
	/**
	* 클럽 배틀을 신청한다.
	* \param dwClubP 주클럽번호( 클럽 배틀의 신청을 받은 주 )
	* \param dwClubS 보조 클럽번호( 클럽 배틀의 신청을 한 클럽 )
	* \return
	*/
	int SetClubBattle( DWORD dwClubP, DWORD dwClubS, DWORD dwEndTime, int nAlliance );

	/**
	* 클럽 배틀을 종료한다.
	* \param dwClubP 주클럽 번호( 클럽 배틀의 신청을 받은 주 )
	* \param dwClubS 보조 클럽 번호( 클럽 배틀의 신청을 한 클럽 )
	* \param nGuFlag 승패여부
	* \param nGuKillNum 클럽 배틀 킬수
	* \return
	*/
	int EndClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance );

	/**
	* 클럽 배틀정보를 저장한다,( 클럽 배틀이 종료되기전 서버재시작이 필요한경우 배틀 정보의 저장이 필요함 )
	* \param dwClubP 주클럽 번호( 클럽 배틀의 신청을 받은 주)
	* \param dwClubS 보조 클럽 번호( 클럽 배틀의 신청을 한 클럽 )
	* \param nGuKillNum 클럽 배틀 킬수
	* \return
	*/
	int SaveClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum );

	/**
	* 동맹배틀 전적을 초기화 시키긴다.
	* \param dwClub 클럽 번호
	* \return
	*/
	int ReSetAllianceBattle( DWORD dwClub );

	/**
	* 해당 클럽의 배틀중인 클럽 정보를 가져온다.
	* \param dwClub 클럽번호
	* \param &vMember 클럽배틀 정보
	* \return 
	*/
    int GetClubBattleInfo( DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo );
	
	/**
	* 캐릭터를 온라인 상태로 만든다.
	* \param nChaNum 캐릭터번호
	* \return 
	*/
	int SetCharacterOnline(int nChaNum);

	/**
	* 캐릭터를 오프라인 상태로 만든다.
	* \param nChaNum 캐릭터번호
	* \return 
	*/
	int SetCharacterOffline(int nChaNum);

	/**
	* 모든 캐릭터를 오프라인 상태로 만든다.
	* \return 
	*/
	int SetAllCharacterOffline();

	/**
	* 캐릭터의 머리색을 변경한다.
	* \return DB_OK, DB_ERROR
	*/
	int SetChaHairColor(DWORD dwChaNum, int nHairColor);

	/**
	* 캐릭터의 헤어스타일을 변경한다.
	* \return DB_OK, DB_ERROR
	*/
	int SetChaHairStyle(DWORD dwChaNum, int nHairStyle);

	/**
	* 캐릭터의 얼굴스타일을 변경한다.
	* \return DB_OK, DB_ERROR
	*/
	int SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle);



	/**
	* 캐릭터의 성별을 변경한다.
	* \return DB_OK, DB_ERROR
	*/
	int SetChaGenderChange( DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor);

	//----------------------------------------------------------------------------
	// PET
	//----------------------------------------------------------------------------
	enum DB_PET_INVEN_TYPE
	{
		TYPEA = 1,
		TYPEB = 2,
		SKILL = 3,
	};

	// typedef std::pair < DWORD, PETSKILL> PETSKILL_PAIR;

	/**
	* 새로운 팻을 생성한다.
	* \param nChaNum  팻 소유자 캐릭터 번호
	* \param szPetName 팻이름
	* \param nPetType 팻의 타입
	* \param nPetMID 팻 MID
	* \param nPetSid 팻 SID
	* \param nPetStyle 팻의 스타일
	* \param nPetColor 팻의 컬러
	* \param m_nPetCardMID 팻 카드의 MID
	* \param m_nPetCardSID 팻 카드의 SID
	* \return DB_ERROR 또는 성공했을때는 Pet 의 고유번호
	*/
	int CreatePet(
		int nChaNum,
		const TCHAR* szPetName,
		int nPetType,
		int nPetMID,
		int nPetSID,
		int nPetStyle,
		int nPetColor,
		int nPetCardMID,
		int nPetCardSID
		);

	/**
	* 새로운 팻을 생성한다.
	* \param nPetNum 팻의 고유번호
	* \param szPetName 팻이름
	* \return DB_ERROR, DB_OK
	*/
	int RenamePet(int nChaNum, int nPetNum, const TCHAR* szPetName);

	/**
	* 팻의 소유권을 이전한다.
	* \param nChaNum 팻의 새로운 소유자
	* \param nPetNum 소유권을 이전할 팻 번호
	*/
	int ExchangePet(int nChaNum, int nPetNum);

	/**
	* 팻의 컬러를 변경한다.
	* \param nPetNum 컬러를 변경하려는 팻의 번호
	* \param nPetColor 팻의 컬러
	*/
	int SetPetColor(int nChaNum, int nPetNum, int nPetColor);

	/**
	* 팻의 스타일을 변경한다.
	* \param nPetNum 스타일을 변경하려는 팻의 번호
	* \param nPetStyle 팻의 스타일
	*
	*/
	int SetPetStyle(int nChaNum, int nPetNum, int nPetStyle);

	/**
	* 팻의 포만도를 설정한다. 포만도는 0-100 사이
	* \param nPetNum 포만도를 설정할 팻의 번호
	* \param nPetFull 포만도
	*/
	int SetPetFull( int nChaNum, int nPetNum, int nPetFull );

	/**
	* 팻의 인벤토리 업데이트
	*/
	int SetPetSkill(
		int nChaNum,
		int nPetNum,
		int nPetInvenType,
		int nPetInvenMID,
		int nPetInvenSID,
		int nPetInvenCMID,
		int nPetInvenCSID,
		int nPetInvenAvailable);

	/**
	* 팻의 인벤토리 업데이트
	*/
	int SetPetInven(
		int nPetChaNum,
		int nPetNum,
		CByteStream &ByteStream );

	/**
	* 팻을 삭제한다.
	* \param nPetNum 삭제할 팻의 번호
	*/
	int	DeletePet(int nChaNum, int nPetNum);
	
	/**
	* 팻의 스킬을 가져온다.
	* \param nPetNum 팻의 번호
	* \param vSkill 팻 스킬을 넣을 vector
	*/
	int GetPetSkill( int nChaNum, GLPET* pPet, DWORD dwPetNum );

	/**
	* 팻의 인벤토리 정보를 가져온다
	*/
	int GetPetInven( int nChaNum, GLPET* pPet, DWORD dwPetNum );

	/**
	* 팻의 정보를 가져온다.
	*/
	int GetPet( GLPET* pPet, DWORD dwPetNum, DWORD dwClientID );

	/**
	* 팻의 포만감을 가져온다
	* \param dwPetNum 팻 번호
	* \return 팻의 포만감 or DB_ERROR
	*/
	int GetPetFull( int nChaNum, DWORD dwPetNum );

	/**
	* 소멸된 팻의 리스트를 가져온다.
	* \param nChaNum 소멸된 펫 리스트를 출력하려는 캐릭터 번호
	* \param vecPetReList 소멸된 펫 리스트를 넣을 벡터
	*/
	int GetRestorePetList( int nChaNum, std::vector<PET_RELIST> &vecPetReList );

	/**
	* 팻을 부활시킨다.
	* \param nPetNum 부활할 팻 번호
	* \param nChaNum 부활을 요구한 캐릭터 번호
	*/
	int RestorePet( DWORD dwPetNum, int nChaNum );

	/**
	* 팻의 스킨팩을 업데이트 한다.
	* \param nChaNum		팻소유 캐릭터 번호
	* \param dwPetNum		펫 번호
	* \param nPetSkinMID	펫 스킨 MID
	* \param nPetSkinSID	펫 스킨 SID
	* \param nPetSkinScale	펫 스킨 Scale
	* \param nPetSkinTime	펫 스킨 적용시간
	*/
	int UpdatePetSkin( int nChaNum,
					   DWORD dwPetNum,
					   int nPetSkinMID,
					   int nPetSkinSID,
					   int nPetSkinScale,
					   int nPetSkinTime );

	/**************************************************************************
	* User Database
	*/

	/**
	* Taiwan / HongKong 
	* 로그인하려는 사용자를 체크한다.	
	* \param szUsrID 사용자 ID
	* \param szPasswd 사용자 패스워드
	* \param szUsrIP 사용자의 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return DB 인증후 결과값
	*/
	int	UserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		const TCHAR* szRandomPasswd,
		int nRandomNum,
		int nSvrGrp,
		int nSvrNum);

	/**
	* Thailand
	* 로그인하려는 사용자를 체크한다.	
	* \param szUsrID 사용자 ID
	* \param szPasswd 사용자 패스워드
	* \param szUsrIP 사용자의 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return DB 인증후 결과값
	*/
	int	ThaiUserCheck(
			const TCHAR* szUsrID,
		    const TCHAR* szPasswd,
			const TCHAR* szUsrIP,
			int nSvrGrp,
			int nSvrNum);

	/**
	* Daum Game (Korea)
	* \param szDaumGID 사용자 GID
	* \param szDaumUID 사용자 UID
	* \param szDaumSSNHEAD 주민번호 앞자리
	* \param szDaumSEX 성별
	* \param szUsrIP IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return 
	*/
	/*
	int	DaumUserCheck(
			const TCHAR* szDaumGID, 
			const TCHAR* szDaumUID,
			const TCHAR* szDaumSSNHEAD,
			const TCHAR* szDaumSEX,
			const TCHAR* szUsrIP, 
			int nSvrGrp, 
			int nSvrNum);
	*/

	/**
	* Daum Game (Korea)	
	* \param 
	* \param 
	* \param szUsrIP 로그인하려는 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return
	*/
	int DaumUserCheck(
			TCHAR* szDaumGID,
		    const TCHAR* szUUID,
			const TCHAR* szUserIP,
			int nSvrGrp,
			int nSvrNum );

	/**
	* GSP
	* \n 로그인하려는 사용자를 체크한다.
	* \param 
	* \param 
	* \param szUsrIP 로그인하려는 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return 
	*/
	int GspUserCheck(
			TCHAR* szGspUserID,
		    const TCHAR* szUUID,
			const TCHAR* szUserIP,
			int nSvrGrp,
			int nSvrNum );
	/**
	* Terra (Malaysia)
	* \n 로그인하려는 사용자를 체크한다.
	* \param szTerraLoginName Terra TLoginName varchar(15)
	* \param szTerraTID Web 에서 넘어온 문자열 (ENCODING 문자열)
	* \param szUsrIP 로그인하려는 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return 
	*/
	int TerraUserCheck(
			TCHAR* szTerraLoginName,
		    const TCHAR* szTerraDecodedTID,
			const TCHAR* szUsrIP,
			int nSvrGrp,
			int nSvrNum );
	/**
	* Excite (Japan)
	* \param szUserID 사용자ID
	* \param szUsrIP IP
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return 
	*/
	int ExciteUserCheck(
			const TCHAR* szUserID,
			const TCHAR* szUserIP,
			int nSvrGrp, 
			int nSvrNum );

	/**
	* Gonzo (Japan)
	* \param szUserID 사용자ID
	* \param szUsrIP IP
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return 
	*/
	int JapanUserCheck(
			const TCHAR* szUserID,
			const TCHAR* szUserIP,
			int nSvrGrp, 
			int nSvrNum,
			int nUserNum);


	/**
	* 글로벌 서비스
	* 로그인하려는 사용자를 체크한다.	
	* \param szUsrID 사용자 ID
	* \param szPasswd 사용자 패스워드
	* \param szUsrIP 사용자의 IP Address
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return DB 인증후 결과값
	*/
	int	GsUserCheck(
		const TCHAR* szUsrID,
		const TCHAR* szPasswd,
		const TCHAR* szUsrIP,
		int nSvrGrp,
		int nSvrNum);


	/**
	* 해당사용자를 Logout 한다
	* \param szUsrID 사용자 ID
	* \param nUsrNum 사용자번호
	* \param nGameTime 게임플레이시간 
	* \param nChaNum 캐릭터번호
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return 
	*/
	int	UserLogout(
			const TCHAR* szUsrID,
			int nUsrNum=0,
			int nGameTime=0,
			int nChaNum=0,
			int nSvrGrp=0,
			int nSvrNum=0,
			int nTotalGameTime=0,
			int nOfflineTime=0 );

	/**
	* Terra (Malaysia)
	* 해당사용자를 Logout 한다
	* \param szTID Terra User ID
	* \param nUsrNum 사용자번호
	* \param nGameTime 게임플레이시간
	* \param nChaNum 캐릭터번호
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return 
	*/
	int	TerraUserLogout(
			const TCHAR* szTID,
			int nUsrNum=0,
			int nGameTime=0,
			int nChaNum=0,
			int nSvrGrp=0,
			int nSvrNum=0 );

	/**
	* Maya (Taiwan/Hongkong)
	* \n 사용자를 로그아웃 시킨다
	* \param szUsrID 사용자 ID
	* \return 
	*/
	int	UserLogoutSimple( const TCHAR* szUsrID );

	/**
	* Maya (Taiwan/Hongkong)
	* \n 사용자를 로그아웃 시킨다.
	* \param nUserNum 사용자번호
	* \return 
	*/
	int UserLogoutSimple2( int nUserNum );
	
	/**
	* Feya (Taiwan/Hongkong)
	* 해당 게임서버의 전체 유저를 로그아웃 시킨다.
	* \param nSvrGrp 서버그룹
	* \param nSvrNum 서버번호
	* \return 
	*/
	int	UserLogoutSvr(int nSvrGrp, int nSvrNum);
	
	/**
	* Feya (Taiwan/Hongkong)
	* \n DB 사용자 정보의 마지막 플레이한 캐릭터 이름을 세팅한다.
	* \param nUsrNum 사용자번호
	* \param szChaName 캐릭터이름
	* \return 
	*/
	int	UserUpdateCha(int nUsrNum, const TCHAR* szChaName);		
	
	/**	
	* 해당 사용자의 사용자 번호를 가져온다.
	* \param szUsrId 사용자 ID
	* \return 사용자번호
	*/
	int	GetUserNum(const TCHAR* szUsrId);

	int GetUserChaRemain(int nUserNum);
	int	GetUserChaTestRemain(int nUserNum);
	int	UserChaNumDecrease(int nUserNum);
	/*
	* TEST 서버 캐릭터 생성갯수 감소
	*/
	int	UserTestChaNumDecrease(int nUserNum);

	/**
	* 캐릭터 삭제, 캐릭터생성 갯수 증가
	*/
	int UserChaNumIncrease(int nUserNum);

	/**
	* TEST 서버 캐릭터 삭제, 갯수 증가
	*/
	int UserTestChaNumIncrease(int nUserNum);
	
	/**
	* 해당 사용자의 타입을 가져온다.
	* \param nUserNum 사용자번호
	* \return 사용자타입
	*/
	int	GetUserType(int nUserNum);

	/**	
	* 해당 사용자의 프리미엄 기간을 가져온다.
	* \param nUserNum 사용자번호
	* \return 프리미엄기간(종료일)
	*/
    __time64_t GetPremiumTime(int nUserNum);	
    
	/**	
	* 해당 사용자의 프리미엄 기간을 세팅한다.
	* \param nUserNum 사용자번호
	* \param tPremiumTime 프리미엄 종료시간
	* \return 
	*/
    int SetPremiumTime(int nUserNum, __time64_t tPremiumTime);	

	/**
	* 해당 사용자의 채팅 블록시간을 가져온다
	* \param nUserNum 사용자번호
	* \return 채팅블록 만료시간
	*/
	__time64_t GetChatBlockTime(int nUserNum);

	/**
	* 해당 사용자의 채팅 블록시간을 설정한다
	* \param nUserNum 사용자번호
	* \param tBlockTime 채팅블록 만료시간
	* \return 
	*/
	int SetChatBlockTime(int nUserNum, __time64_t tBlockTime);

	int AddBlockIPList( TCHAR* szIP, TCHAR* szReason );
	int	AddLogServerState(int nSGNum=0, int nSvrNum=0, int nUserNum=0, int nUserMax=1);

	USER_INFO_BASIC GetUserInfo( const TCHAR* szUsrId, const TCHAR* szPasswd );
	int DaumGetUserInfo( const TCHAR* szGID, DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg );
	
	/**
	* 사용자 정보를 가져온다
	* Taiwan / HongKong
	* \param szUserID 
	* \param pMsg 
	* \return 
	*/
	int GetUserInfo( const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg );

	/**
	* 사용자 정보를 가져온다
	* Thailand
	* \param szUserID 
	* \param pMsg 
	* \return 
	*/
	int ThaiGetUserInfo( const TCHAR* szUserID, NET_LOGIN_FEEDBACK_DATA2* pMsg );

	/**
	* Terra 사용자 정보를 가져온다.
	* \param szTLoginName TLoginName varchar(15)
	* \param pMsg
	* \return 
	*/
	int TerraGetUserInfo( const TCHAR* szTLoginName, TERRA_NET_LOGIN_FEEDBACK_DATA2* pMsg );

	/**
	* GSP 사용자 정보를 가져온다.
	* \param szUserID UserID varchar(50)
	* \param pMsg
	* \return 
	*/
	int GspGetUserInfo( const TCHAR* szUserID, GSP_NET_LOGIN_FEEDBACK_DATA2* pMsg );

	/**
	* Excite 사용자 정보를 가져온다.
	* \param szUserID UserID varchar(20)
	* \param pMsg
	* \return 
	*/
	int ExciteGetUserInfo( const TCHAR* szUserID, EXCITE_NET_LOGIN_FEEDBACK_DATA2* pMsg );

	/**
	* Japan(Gonzo) 사용자 정보를 가져온다.
	* \param szUserID UserID varchar(16)
	* \param pMsg
	* \return 
	*/
	int JapanGetUserInfo( const TCHAR* szUserID, JAPAN_NET_LOGIN_FEEDBACK_DATA2* pMsg );


	/**
	* 글로벌서비스 사용자 정보를 가져온다.
	* \param szUserID UserID varchar(16)
	* \param pMsg
	* \return 
	*/
	int GsGetUserInfo( const TCHAR* szUserID, GS_NET_LOGIN_FEEDBACK_DATA2* pMsg );
	
	/**
	* 해당사용자를 블록시킨다.
	* \param nUserNum 사용자번호
	* \param nDay 블럭일수 1 - 15 까지
	* \return 
	*/
    int UserBlock( int nUserNum, int nDay );

	/**
	* 해당사용자를 블록시킨다.
	* \param nUserNum 사용자번호
	* \param nHour 시간
	* \return 
	*/
    int UserBlockDate( int nUserNum, __time64_t tBlockTime );
	
	/**
	* 해당 게임서버의 전체 유저를 로그아웃 시킨다.
	* \param nSvrGrp 
	* \return 
	*/
	int AllServerUserLogout(int nSvrGrp);

	/**
	* 전체 유저를 로그아웃 시킨다.
	* \return 
	*/
	int	AllUserLogout();

	/**
	* TERRA
	* 유저의 패스워드를 넣거나 체크한다.
	* \param szTID Terra UserID
	* \param szPasswd 사용자 패스워드
	* \return 
	*/
	int TerraUserPassCheck(
				const TCHAR* szTID,
				const TCHAR* szPasswd,
				int	nCheckFlag );

	/**
	* Daum
	* 유저의 패스워드를 넣거나 체크한다.
	* \param szDaumGID Daum UserGID
	* \param szDaumPass 사용자 패스워드
	* \return 
	*/
	int DaumUserPassCheck(
				const TCHAR* szDaumGID,
				const TCHAR* szDaumPasswd,
				int	nCheckFlag );


	/**
	* Excite
	* 유저의 패스워드를 넣거나 체크한다.
	* \param szUID Excite UserID
	* \param szUserID Excite 2차 UserID
	* \param szPasswd 사용자 패스워드
	* \return 
	*/
	int ExciteUserPassCheck(
					const TCHAR* szUID,
					const TCHAR* szUserID,
					const TCHAR* szPasswd,
					int	nCheckFlag );


	/**
	* Thailand
	* 사용자의 PC방 클래스를 체크한다.	
	* \param szUsrIP 사용자의 IP Address
	* \return DB 인증후 결과값
	*/
	int	ThaiUserClassCheck(			
			const TCHAR* szUsrIP );

	/**
	* Malaysia
	* 사용자의 PC방 클래스를 체크한다.	
	* \param szUsrIP 사용자의 IP Address
	* \return DB 인증후 결과값
	*/
	int	MyUserClassCheck(
			const TCHAR* szUsrIP );
	
	/**
	* 해당 사용자의 오늘 게임플레이 시간(분)을 가져온다.
	* Thailand
	* \param nUserNum 
	* \return 
	*/
	WORD ThaiGetTodayPlayTime (int nUserNum);

	/**************************************************************************
	* Log Database
    */

	// int LogServerState(int nSGNum=0, int nSvrNum=0, int nUserNum=0, int nUserMax=0);
	int	LogChaDeleted(
			int nSGNum=0,
			int nSvrNum=0,
			int nUserNum=0,
			int nChaNum=0 );

	int	LogItemExchange(
			const SLOGITEMEXCHANGE &sEXCHANGE );

	int	LogMoneyExchange(
			const SLOGMONEYEXCHANGE &_sLOG );

	int LogItemConversion(
			const SLOGITEMCONVERSION &_sLOG );

    int	LogAction(
			const SLOGACTION &_sLOG );

	/**
	* 아이템의 랜덤옵션 로그를 남긴다.
	* \param _sLOG 랜덤옵션 구조체
	*/
	int LogRandomItem( const SLOGRANDOMOPTION &_sLOG );

	/**
	* 펫의 액션로그를 남긴다.
	* \param _sLOG 펫액션로그 구조체
	*/
	int LogPetAction( const SLOGPETACTION &_sLOG );

	// 여름방학 PC 방 프로모션 복권관련'선물펑펑!!' 이벤트
	int	LogLottery(
			CString strLottery, 
		    CString strIP, 
			CString strUserUID, 
			int nUserNum );

	// 여름방학 PC 프로모션 아이템 '펑펑' 이벤트
	int LogPungPungCheck(
			CString strIP, 
		    int nUserNum );
    
	// 여름방학 PC 프로모션 아이템 '펑펑' 이벤트
	int LogPungPungWrite(
			CString strIP, 
		    CString strUserUID, 
			int nUserNum, 
			int nPrize );

    LONGLONG GetItemMaxNum(
				int nSGNum, 
		        int nSvrNum, 
				int nFldNum, 
				std::vector<VIEWLOGITEMEXCHANGEMAX> &v );

	/**
	* 게임내에서 진정이나 신고로 신고한 내용을 DB에 기록한다.
	* \param nSGNum 서버그룹
	* \param nChaNum 신고한 캐릭터번호
	* \param strMsg 신고내용 (최대크기는 CHAT_MSG_SIZE)
	* \return 
	*/
    int LogAppeal(int nSGNum, int nChaNum, CString strMsg);

	/**
	* 불법프로그램 사용내역을 기록한다.
	* \param nSGNum 서버그룹
	* \param nSvrNum 서버번호
	* \param nUserNum 유저번호
	* \param nChaNum 캐릭터번호
	* \param nHackProgramNum 해킹프로그램번호
	* \param szComment 콤멘트
	* \return 
	*/
	int	LogHackProgram(
		int nSGNum, 
		int nSvrNum,
		int nUserNum,
		int nChaNum,
		int nHackProgramNum,
		const TCHAR* szComment );

	/**
	* SMS 문자메시지를 발송한다.
	*
	* \param nSendChaNum SMS 를 발송하는 캐릭터 번호	
	* \param nReceiveChaNum SMS 를 받을 캐릭터 번호
	* \param szReceivePhone SMS 를 받을 캐릭터의 전화번호
	* \param szSmsMsg SMS 메시지 내용
	*/
	int LogSMS(
		int nSendChaNum,		
		int nReceiveChaNum,
		const TCHAR* szReceivePhone,
		const TCHAR* szSmsMsg );

	/**
	* 캐릭터의 휴대폰 번호를 저장한다.
	* \param nChaNum 캐릭터 번호
	* \param szPhoneNumber 전화번호 NULL 문자 포함 14자 SMS_RECEIVER
	*/
	int SetChaPhoneNumber(
			int nChaNum,
			const TCHAR* szPhoneNumber );

	/**
	* 캐릭터의 친구정보를 가져온다.
	* \param nChaNum 친구정보를 가져올 캐릭터 번호
	* \param vFriend 친구정보를 담을 vector
	*/
	int	GetChaFriendSMS(
			int nChaNum,
			std::vector<CHA_FRIEND_SMS> &vFriend );

	/**************************************************************************
	* Shop Database
    */

	/**
	* Item Shop 에서 구입한 아이템을 가져온다.
	*/
	int GetPurchaseItem(
			CString strUID,
			std::vector<SHOPPURCHASE> &v );
    
	/**
	* 실제로 가져갈 수 있는 상품인지 확인한다.
	* \param strPurKey 구매키
	* \param nFlag  (0:구매, 1:구매완료, 2:구매취소신청, 3:구매취소처리)
	* \return 1:가져갈수 있음 \n 이외의값:가져갈수 없음
	*/
    int SetPurchaseItem(
			CString strPurKey,
		    int nFlag=1 );

	/**************************************************************************
	* Vehicle Database
    */

	// VEHICLE
	//----------------------------------------------------------------------------
	/**
	* 새로운 탈것을 생성한다.
	* \param nChaNum  탈것 소유자 캐릭터 번호
	* \param szVehicleName 탈것 이름
	* \param nVehicleType 탈것의 타입
	* \param nVehicleMID 탈것 MID
	* \param nVehicleSID 탈것 SID
	* \param nVehicleCardMID 탈것의 아이템 MID
	* \param nVehicleCardSID 탈것의 아이템 SID
	* \return DB_ERROR 또는 성공했을때는 탈것의 고유번호
	*/
	int CreateVehicle(
		int nCharNum,
		const TCHAR* szVehicleName,
		int nVehicleType,
		int nVehicleCardMID,
		int nVehicleCardSID
		);


	/**
	* 탈것의 배터리를 설정한다. 배터리는 0-1000 사이
	* \param nVehicleBttery 배터리를 설정할 탈것의 번호
	* \param nVehicleBttery 배터리
	*/
	int SetVehicleBattery(
		int nVehicleNum,
		int nCharNum,
		int nVehicleBttery
		);


	int GetVehicleBattery(
		int nVehicleNum,
		int nCharNum
		);

	
	/**
	* 탈것의 액션로그를 남긴다.
	* \param _sLOG 탈것액션로그 구조체
	*/
	int LogVehicleAction( const SLOGVEHICLEACTION &_sLOG );




	/**
	* 탈것을 삭제한다.
	* \param nVehicle 삭제할 탈것의 번호
	*/
	int DeleteVehicle(
		int nVehicle, 
		int nCharNum);

	/**
	* 탈것의 정보를 가져온다.
	*/
	int GetVehicle(
		GLVEHICLE* pVehicle,
		int nVehicleNum,
		int nCharNum);


	/**
	* 탈것의 인벤토리 정보를 가져온다
	*/
	int GetVehicleInven( 
		GLVEHICLE* pVehicle,
		int nVehicleNum,
		int nCharNum );

	/**
	*탈것 인벤토리 업데이트
	*/
	int SetVehicleInven(
		int nVehicle,
		int nCharNum,
		CByteStream &ByteStream );	


	// 출석부 이벤트 
	int LogUserAttend( int nUserNum, std::vector<USER_ATTEND_INFO> &vecUserAttend );

	int InsertUserAttend( int nUserNum, int nCount, int nAttendReward );

	int InsertAttendItem( CString strPurKey, CString strUserID, int nItemMID, int nItemSID );

	//	UserMoney Add
	int UpdateUserMoneyAdd( DWORD dwUserID, LONGLONG lnUserMoney );
	
	//	UserLastInfo Add
	int UpdateUserLastInfoAdd( DWORD dwUserID, LONGLONG lnUserMoney );



	/**************************************************************************
	* etc
    **************************************************************************/
	int	Command( TCHAR* szCmdStr );
};

#endif // _CODBCMANAGER_H_