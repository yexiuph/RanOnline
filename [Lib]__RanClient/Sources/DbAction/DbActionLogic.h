#ifndef DBACTIONLOGIC_H_
#define DBACTIONLOGIC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../[Lib]__NetServer/Sources/s_CDbAction.h"
#include "./GLPet.h"
#include "./GLVEHICLE.h"

/**
 * \ingroup RanClientLib
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
 * \date 2005-05-31
 *
 * \author 
 *
 * \par license
 * 
 * \todo 
 *
 * \bug 
 *
 */

class CDbActSaveChar : public CDbAction
{
public:
	CDbActSaveChar ();
	virtual ~CDbActSaveChar();
	
	bool SetInfo ( DWORD dwClient, DWORD dwGaea, SCHARDATA2 *pCHARDATA );

	virtual int Execute ( CServer* pServer );

protected:
	DWORD		m_dwClient;
	DWORD		m_dwGaea;
	SCHARDATA2	m_sCHARDATA;
};

class CDbActToAgentMsg : public CDbAction
{
public:
	CDbActToAgentMsg ();
	virtual ~CDbActToAgentMsg();
	
	bool SetMsg ( DWORD dwClient, NET_MSG_GENERIC *nmg );

	virtual int Execute ( CServer* pServer );

protected:
	DWORD	m_dwClientID;
	BYTE	m_aryMsg[NET_DATA_BUFSIZE];
};

class CDelChaFriend : public CDbAction
{
public:
	CDelChaFriend(int nChaP, int nChaS);
	virtual ~CDelChaFriend() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaP;
	int m_nChaS;
};

class CAddChaFriend : public CDbAction
{
public:
	CAddChaFriend(int nChaP, int nChaS);
	virtual ~CAddChaFriend() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaP;
	int m_nChaS;
};

class CSetChaFriend : public CDbAction
{
public:
	CSetChaFriend(int nChaP, int nChaS, int nFlag);
	virtual ~CSetChaFriend() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaP;
	int m_nChaS;
	int m_nFlag;
};

class CLogPartyMatch : public CDbAction
{
public:
	CLogPartyMatch(int nSGNum, int nSvrNum, WORD wWin, WORD wLost);
	virtual ~CLogPartyMatch() {};
	virtual int Execute (CServer* pServer);

protected:
	int		m_nSGNum;
	int		m_nSvrNum;
	WORD	m_wWin;
	WORD	m_wLost;	
};

class CLogItemExchange : public CDbAction
{
public:
	CLogItemExchange ();
	CLogItemExchange ( const SLOGITEMEXCHANGE &sEXCHANGE );
	virtual ~CLogItemExchange() {};
    virtual int Execute (CServer* pServer);

protected:
	SLOGITEMEXCHANGE m_sEXCHANGE;
};

class CLogMoneyExchange : public CDbAction
{
public:
	CLogMoneyExchange ();
	CLogMoneyExchange ( const SLOGMONEYEXCHANGE &_sLOG );
	virtual ~CLogMoneyExchange() {};
    virtual int Execute (CServer* pServer);	

protected:
	SLOGMONEYEXCHANGE m_sLOG;
};

class CLogItemConversion : public CDbAction
{
public:
	CLogItemConversion(
		const SLOGITEMCONVERSION &_sLOG );
	virtual ~CLogItemConversion() {};
    virtual int Execute( CServer* pServer );

protected:
	SLOGITEMCONVERSION m_sLOG;
};

/**
* 아이템의 랜덤옵션 로그를 남긴다.
* \param _sLOG 랜덤옵션 구조체
*/
class CLogRandomItem : public CDbAction
{
public:
	CLogRandomItem(
		const SLOGRANDOMOPTION& sLOG );
	virtual ~CLogRandomItem() {};
	virtual int Execute( CServer* pServer );

protected:
	SLOGRANDOMOPTION m_sLOG;
};

/**
* 펫의 액션 로그를 남긴다.
* \param _sLOG 펫액션로그 구조체
*/
class CLogPetAction : public CDbAction
{
public:
	CLogPetAction( const SLOGPETACTION& sLOG );
	virtual ~CLogPetAction() {};
	virtual int Execute( CServer* pServer );

protected:
	SLOGPETACTION m_sLOG;
};

/**
 * Shop 에서 구입한 아이템을 가져온다.
 */
class CGetPurchaseItem : public CDbAction
{
public:
    CGetPurchaseItem(CString strUID, DWORD dwCharID); 
    virtual ~CGetPurchaseItem() {};
    virtual int Execute(CServer* pServer);

protected:
	DWORD	m_dwCharID;
    CString m_strUID;
    std::vector<SHOPPURCHASE> m_vItem;
};

/**
 * 실제로 가져갈 수 있는 상품인지 확인한다.
 * 입력값
 * nFlag (0 : 구매, 1 : 구매완료, 2 : 구매취소신청, 3 : 구매취소처리)
 * 출력값
 * 1 : 가져갈수 있음
 * 이외의값 : 가져갈수 없음
 */
class CSetPurchaseItem : public CDbAction
{
public:
    CSetPurchaseItem(CString strPurKey, int nFlag=1);
    virtual ~CSetPurchaseItem() {};
    virtual int Execute(CServer* pServer);

protected:
    CString m_strPurKey;
    int m_nFlag;
};

/**
 * 구입한 아이템을 인벤토리로 가져옵니다.
 */
class CPurchaseItem_Get : public CDbAction
{
public:
    CPurchaseItem_Get ( DWORD dwClient, DWORD dwUserID, CString strPurKey, SNATIVEID nidITEM, DWORD dwInvenPos );
    virtual ~CPurchaseItem_Get() {};
    virtual int Execute(CServer* pServer);

protected:
	DWORD		m_dwClient;
	DWORD		m_dwUserID;
    CString		m_strPurKey;
	SNATIVEID	m_nidITEM;
	DWORD		m_dwInvenPos;
};

/**
 * 구입한 아이템을 인벤에 넣기에 실패했을 경우 flag 리셋.
 */
class CPurchaseItem_Restore : public CDbAction
{
public:
    CPurchaseItem_Restore ( CString strPurKey );
    virtual ~CPurchaseItem_Restore() {};
    virtual int Execute(CServer* pServer);

protected:
    CString m_strPurKey;
};

/**
 * 캐릭터추가카드 아이템
 * 영구적으로 생성가능한 캐릭터 숫자를 1 증가시킨다.
 */
class CItemChaCreateNumIncrease : public CDbAction
{
public:
    CItemChaCreateNumIncrease(int nUserNum);
    virtual ~CItemChaCreateNumIncrease() {};
    virtual int Execute(CServer* pServer);

protected:
    int m_nUserNum;
};

/**
 * 해당 락커 만료시간을 정한다.
 * 1 번 5 번 락커는 정할 수 없음.
 * 2, 3, 4 번 락커만 만료시간을 정할 수 있다.
 */
class CSetChaStorageDate : public CDbAction
{
public:
    CSetChaStorageDate(int nUserNum, int nStorageNum, __time64_t tTime);
    virtual ~CSetChaStorageDate() {} ;
    virtual int Execute(CServer* pServer);

protected:
    int m_nUserNum;
    int m_nStorageNum;
    __time64_t m_tTime;
};

/**
 * 해당 캐릭터의 인벤토리 추가 줄수를 세팅한다.
 * 주의 
 * 최초 0 에서 한줄 추가시 1 을 입력
 * 현재 한줄 상태에서 또 한줄 추가시 2 를 입력 (최종 줄수를 입력한다)
 * 현재 두줄 상태에서 한줄 추가시 3 을 입력
 * 최대 3까지만 입력가능.
 */
class CSetChaInvenNum : public CDbAction
{
public :
    CSetChaInvenNum(int nChaNum, WORD wInvenLine);
    virtual ~CSetChaInvenNum() {} ;
    virtual int Execute(CServer* pServer);

protected:
    int m_nChaNum;
    WORD m_wInvenLine;
};


/**
 * 해당 사용자의 프리미엄 기간을 세팅한다.
 */
class CSetPremiumTime : public CDbAction
{
public:
    CSetPremiumTime(int nUserNum, __time64_t tPremiumTime);
    virtual ~CSetPremiumTime() {} ;
    virtual int Execute(CServer* pServer);
protected:
    int m_nUserNum;
    __time64_t m_tPremiumTime;
};

/**
 * 직전귀환 카드 사용을 위한 포지션을 저장한다.
 */
class CSetLastCallPos : public CDbAction
{
public:
    CSetLastCallPos(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos);
    virtual ~CSetLastCallPos() {} ;
    virtual int Execute(CServer* pServer);

protected:
    int m_nChaNum;
    DWORD m_dwMapID;
    D3DXVECTOR3 m_vPos;
};

/**
 * 새로운 클럽을 생성한다.
 */
class CCreateClub : public CDbAction
{
public:
	/**
	* 새로운 클럽을 생성한다.
	* \param dwClientID Client 번호
	* \param strClubName 클럽이름
	* \param dwChaNum 클럽을 생성하려는 캐릭터 번호 (이 캐릭터가 클럽의 마스터가 된다)
	* \return -1:이미 클럽마스터로 등록되어 있음 생성불가.
	* \n      -2:클럽 생성중 에러발생 (중복된길드이름)
	* \n       0이상:생성된 클럽번호임
	*/
    CCreateClub ( DWORD dwClientID, CString strClubName, DWORD dwChaNum);
    virtual ~CCreateClub() {} ;
    virtual int Execute(CServer* pServer);

protected:
    CString m_strClubName;
    DWORD m_dwChaNum;
};

/**
 * 클럽을 삭제한다.
 */
class CDeleteClub : public CDbAction
{
public:
	/**
	* 클럽을 삭제한다.
	* \param dwClub 클럽번호
	* \param dwChaNum 클럽을 삭제하려는 캐릭터번호(캐릭터 번호가 클럽 마스터가 아니면 삭제가 되지 않는다)
	* \return 0:클럽 삭제 실패
    * \n      1:클럽 삭제 성공
	*/
    CDeleteClub(DWORD dwClub, DWORD dwChaNum);
    virtual ~CDeleteClub() {};
    virtual int Execute(CServer* pServer);

protected:
    DWORD m_dwClub;
    DWORD m_dwChaNum;
};

/**
 * 클럽랭크를 세팅한다.
 */
class CSetClubRank : public CDbAction
{
public:
	/**
	* 클럽랭크를 세팅한다.
	* \param dwClub 클럽번호
	* \param dwRank 랭킹
	* \return 
	*/
    CSetClubRank(DWORD dwClub, DWORD dwRank);
    virtual ~CSetClubRank() {};
    virtual int Execute(CServer* pServer);

protected:
    DWORD m_dwClub;
    DWORD m_dwRank;
};

class CWriteClubMarkImage : public CDbAction
{
public:
	/**
	* 클럽 마크 이미지를 저장한다.
	* \param dwClub 클럽번호
	* \param dwMarkVer 마크버전
	* \param pData 이미지데이터 포인터
	* \param nSize 이미지데이터 사이즈
	* \return 
	*/
    CWriteClubMarkImage(DWORD dwClub, DWORD dwMarkVer, BYTE* pData, int nSize);
    virtual ~CWriteClubMarkImage();
    virtual int Execute(CServer* pServer);

protected:
    DWORD m_dwClub;
    DWORD m_dwMarkVer;
    BYTE  m_aryMark[sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY];
    int   m_nSize;
};

/**
 * 클럽 해체시간을 설정한다.
 */
class CSetClubDissolutionTime : public CDbAction
{
public:
	/**
	* 클럽 해체시간을 설정한다.
	* \param dwClub 클럽번호
	* \param tDiss 해체시간
	* \return 
	*/
    CSetClubDissolutionTime(DWORD dwClub, __time64_t tDiss);
    virtual ~CSetClubDissolutionTime() {} ;
    virtual int Execute(CServer* pServer);

protected:
    DWORD m_dwClub;
    __time64_t m_tDiss;
};

/**
* 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
*/
class CSetClubAllianceSec : public CDbAction
{
public:
	/**
	* 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
	* \param dwClub 클럽번호
	* \param tSec 시간
	* \return 
	*/
    CSetClubAllianceSec(DWORD dwClub, __time64_t tSec);
    virtual ~CSetClubAllianceSec() {} ;
    virtual int Execute(CServer* pServer);

protected:
    DWORD m_dwClub;
    __time64_t m_tSec;
};

/**
* 클럽의 동맹해산 시간을 정한다.
*/
class CSetClubAllianceDis : public CDbAction
{
public:
	/**
	* 클럽의 동맹해산 시간을 정한다.
	* \param dwClub 클럽번호
	* \param tDis 해산시간
	* \return 
	*/
    CSetClubAllianceDis(DWORD dwClub, __time64_t tDis);
    virtual ~CSetClubAllianceDis() {} ;
    virtual int Execute(CServer* pServer);

protected:
    DWORD m_dwClub;
    __time64_t m_tDis;
};

/**
* 클럽의 마스터 위임 시간을 설정한다.
*/
class CSetClubAuthorityTime : public CDbAction
{
public:
	/**
	* 클럽의 마스터 위임 시간을 설정한다.
	* \param dwClub 클럽번호
	* \param tDis 마스터 위임 시간
	* \return 
	*/
    CSetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority);
    virtual ~CSetClubAuthorityTime() {} ;
    virtual int Execute(CServer* pServer);

protected:
    DWORD m_dwClub;
    __time64_t m_tAuthority;
};




/**
 * 새로운 캐릭터를 클럽에 가입시킨다
 */ 
class CAddClubMember : public CDbAction
{
public:
	/**
	* 새로운 캐릭터를 클럽에 가입시킨다
	* \param dwClub 클럽번호
	* \param dwChaNum 가입시키려는 캐릭터번호
	* \return 
	*/
    CAddClubMember(DWORD dwClub, DWORD dwChaNum);
    virtual ~CAddClubMember() {} ;
    virtual int Execute(CServer* pServer);

protected:
    DWORD m_dwClub;
    DWORD m_dwChaNum;
};

/**
 * 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
 */
class CDeleteClubMember : public CDbAction
{
public:
	/**
	* 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
	* \param dwChaNum 캐릭터번호
	* \return 
	*/
    CDeleteClubMember(DWORD dwChaNum);
    virtual ~CDeleteClubMember() {} ;
    virtual int Execute(CServer* pServer);

protected:
    DWORD m_dwChaNum;
};

/**
 * 해당 캐릭터의 경험치를 세팅한다.
 */
class CSetChaExp : public CDbAction
{
public:
	/**
	* 해당 캐릭터의 경험치를 세팅한다.
	* \n 주의
    * \n 캐릭터번호는 0 보다 커야한다.
    * \n 경험치는 0 보다 커야한다.
	* \param nChaNum 캐릭터번호
	* \param llExp 경험치
	* \return 
	*/
    CSetChaExp(int nChaNum, LONGLONG llExp);
    virtual ~CSetChaExp() {};
    virtual int Execute(CServer* pServer);

protected:
    int m_nChaNum;
    LONGLONG m_llExp;
};

/**
 * 해당 캐릭터의 경험치를 가져온다.
 */
class CGetChaExp : public CDbAction
{
public:

	/**
	* 해당 캐릭터의 경험치를 가져온다.
	* \n 주의 
    * \n 캐릭터번호는 0 보다 커야한다.
    * \n 경험치가 0 보다 작으면 에러이다.
	* \param nChaNum 캐릭터번호
	* \return 경험치
	*/
    CGetChaExp(int nChaNum);
    virtual ~CGetChaExp() {};
    virtual int Execute(CServer* pServer);

protected:
    int m_nChaNum;
};

/**
 * 게임내에서 진정이나 신고로 신고한 내용을 DB에 기록한다.
 */
class CLogAppeal : public CDbAction
{
public:
	/**
	* 게임내에서 진정이나 신고로 신고한 내용을 DB에 기록한다.
	* \param nSGNum 서버그룹
	* \param nChaNum 신고한 캐릭터번호
	* \param strMsg 신고내용 (최대크기는 CHAT_MSG_SIZE)
	* \return 
	*/
    CLogAppeal(int nSGNum, int nChaNum, CString strMsg);
    virtual ~CLogAppeal() {};
    virtual int Execute(CServer* pServer);

protected:
    int m_nSGNum;
    int m_nChaNum;
    CString m_strMsg;
};

/**
 * 지역과 그지역을 소유한 클럽 세율을 정한다.
 */ 
class CSetClubRegion : public CDbAction
{
public:
	/**
	* 지역과 그지역을 소유한 클럽 세율을 정한다.
	* \param dwRegionID 지역 ID
	* \param dwClub 클럽 ID
	* \param fTax 세율
	* \return 
	*/
	CSetClubRegion(DWORD dwRegionID, DWORD dwClub, float fTax);
	virtual ~CSetClubRegion() {} ;
	virtual int Execute(CServer* pServer);
	
protected:
	DWORD m_dwRegionID;
	DWORD m_dwClub;
	float m_fTax;
};

/**
 * 해당 지역의 소유 상태를 리셋한다.
 */
class CDelClubRegion : public CDbAction
{
public:
	/**
	* 해당 지역의 소유 상태를 리셋한다.
	* \n 주의
	* \n 해당지역 ID 와 클럽 ID 가 DB 정보와 정확히 일치하지 않으면 취소된다.
	* \param dwRegionID 지역 ID
	* \param dwClub 클럽 ID
	* \return 
	*/
	CDelClubRegion(DWORD dwRegionID, DWORD dwClub);
	virtual ~CDelClubRegion() {} ;
	virtual int Execute(CServer* pServer);
	
protected:
	DWORD m_dwRegionID;
	DWORD m_dwClub;
};

/**
 * 해당클럽의 보유금액을 세팅한다. (Storage)
 */
class CSetClubMoney : public CDbAction
{
public:
	/**
	* 해당클럽의 보유금액을 세팅한다. (Storage)
	* \param dwClub 클럽번호
	* \param llMoney 보유금액 (금액은 >= 0)
	* \return 
	*/
	CSetClubMoney(DWORD dwClub, LONGLONG llMoney);
	virtual ~CSetClubMoney() {} ;
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwClub;
	LONGLONG m_llMoney;
};

/**
 * 해당클럽의 수입금액을 세팅한다. (Income money)
 */
class CSetClubIncomeMoney : public CDbAction
{
public:
	/**
	* 해당클럽의 수입금액을 세팅한다. (Income money)
	* \param dwClub 클럽번호
	* \param llMoney 수입금액 (금액은 >= 0)
	* \return 
	*/
	CSetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney);
	virtual ~CSetClubIncomeMoney() {} ;
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwClub;
	LONGLONG m_llMoney;
};

/**
 * 해당클럽의 클럽창고를 저장한다. 
 */
class CWriteClubStorage : public CDbAction
{
public:
	/**
	* 해당클럽의 클럽창고를 저장한다.
	* \param dwClub 클럽번호
	* \param pData 창고데이터 포인터
	* \param nSize 버퍼크기
	* \return 
	*/
	CWriteClubStorage(DWORD dwClub, const BYTE* pData, int nSize);
	virtual ~CWriteClubStorage();
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwClub;
	BYTE* m_pData;
	int   m_nSize;
};

class CGetClubStorage : public CDbAction
{
public:
	CGetClubStorage( DWORD _dwClientID, DWORD _dwCharID, DWORD _dwClubID );
	virtual ~CGetClubStorage() {};
	virtual int Execute( CServer* pServer );

protected:
	DWORD m_dwClientID;
	DWORD m_dwCharID;
	DWORD m_dwClubID;
};

/**
* 불법프로그램 사용내역을 기록한다.
*/
class CLogHackProgram : public CDbAction
{
	enum { MAX_INFO = 512 };

public:
	/**
	* 불법프로그램 사용내역을 기록한다.
	* \param nUserNum 유저번호
	* \param nChaNum 캐릭터번호
	* \param nHackProgramNum 해킹프로그램 번호
	* \param *szINFO 콤멘트...문자열
	* \return 
	*/
	CLogHackProgram(
		int nUserNum,
		int nChaNum,
		int nHackProgramNum,
		TCHAR* szINFO );
	virtual ~CLogHackProgram() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	int m_nChaNum;
	int m_nHackProgramNum;
	TCHAR m_szINFO[CLogHackProgram::MAX_INFO];
};

/**
* 부 클럽장을 세팅한다
*/
class CSetClubMasterFlags : public CDbAction
{
public:
	/**
	*
	* dwClub : 클럽번호
    * dwChaNum : 캐릭터번호
    * dwSubMasterFlags : 플래그 (0 으로 넣으면 일반길드원이 된다)
	* \param dwClub 
	* \param dwChaNum 
	* \param dwSubMasterFlags 
	* \return 
	*/
	CSetClubMasterFlags(DWORD dwClub, DWORD dwChaNum, DWORD dwSubMasterFlags);
	virtual ~CSetClubMasterFlags() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwClub;
	DWORD m_dwChaNum;
	DWORD m_dwSubMasterFlags;
};

/**
* 동맹 클럽을 결성한다.
*/
class CSetClubAlliance : public CDbAction
{
public:
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
	CSetClubAlliance(DWORD dwClubP, DWORD dwClubS);
	virtual ~CSetClubAlliance() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwClubP;
	DWORD m_dwClubS;
};

/**
* 동맹 클럽을 해체한다.
*/
class CDelClubAlliance : public CDbAction
{
public:
	/**
	* 동맹 클럽을 해체한다.
	* \param dwClubP 주 클럽번호 (동맹 클럽의 주)
	* \param dwClubS 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
	* \return 
	*/
	CDelClubAlliance(DWORD dwClubP, DWORD dwClubS);
	virtual ~CDelClubAlliance() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwClubP;
	DWORD m_dwClubS;
};

class CSetClubBattle : public CDbAction
{
public:
	/**
	* 클럽배틀을 시작한다.
	* \param dwClubP 주 클럽번호 (자기클럽)
	* \param dwClubS 보조 클럽번호 (상대클럽)
	* \return 
	*/
	CSetClubBattle(DWORD dwClubP, DWORD dwClubS, DWORD dwEndTime, bool bAlliance = false );
	virtual ~CSetClubBattle() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwClubP;
	DWORD m_dwClubS;
	DWORD m_dwEndTime;
	bool m_bAlliance;
};

class CEndClubBattle : public CDbAction
{
public:
	/**
	* 클럽배틀을 종료한다.
	* \param dwClubP 주 클럽번호 (자기클럽)
	* \param dwClubS 보조 클럽번호 (상대클럽)
	* \param nFlag		종료 Flag
	* \param nGuKillNum	 킬수
	* \param nGuDeathNum 데스수
	* \return 
	*/
	CEndClubBattle(DWORD dwClubP, DWORD dwClubS, int nFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance = false );
	virtual ~CEndClubBattle() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD	m_dwClubP;
	DWORD	m_dwClubS;
	int		m_nFlag;
	int		m_nGuKillNum;
	int		m_nGuDeathNum;
	bool	m_bAlliance;
};

class CSaveClubBattle : public CDbAction
{
public:
	/**
	* 클럽배틀을 진행상황을 저장한다.
	* \param dwClubP 주 클럽번호 (자기클럽)
	* \param dwClubS 보조 클럽번호 (상대클럽)
	* \param nGuKillNum	 킬수
	* \param nGuDeathNum 데스수
	* \return 
	*/
	CSaveClubBattle(DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum );
	virtual ~CSaveClubBattle() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD	m_dwClubP;
	DWORD	m_dwClubS;
	int		m_nGuKillNum;
	int		m_nGuDeathNum;
};


/**
* 동맹배틀 전적을 초기화 시킨다.
*/
class CReSetAllianceBattle : public CDbAction
{
public:
	/**
	* 동맹배틀 전적을 초기화 시킨다.	
	* \param dwClub 클럽번호
	* \return 
	*/
    CReSetAllianceBattle( DWORD dwClub );
    virtual ~CReSetAllianceBattle() {} ;
    virtual int Execute(CServer* pServer);

protected:
    DWORD m_dwClub;
};


/**
* 해당사용자의 채팅블록시간을 가져온다.
*/
class CGetChatBlockTime : public CDbAction
{
public:
	/**
	* 해당사용자의 채팅블록시간을 가져온다.
	* \param nUserNum 사용자번호
	* \return 
	*/
	CGetChatBlockTime(int nUserNum);
	virtual ~CGetChatBlockTime() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
};

/*
* 해당사용자의 채팅블록시간을 설정한다.
*/
class CSetChatBlockTime : public CDbAction
{
public:
	/**
	* 해당사용자의 채팅블록시간을 설정한다.
	* \param nUserNum 사용자번호
	* \param tBlockTime 채팅 블록 만료시간
	* \return 
	*/
	CSetChatBlockTime(int nUserNum, __time64_t tBlockTime);
	virtual ~CSetChatBlockTime() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	__time64_t m_tBlockTime;
};

/**
* 클럽의 공지사항을 세팅한다.
*/
class CSetClubNotice : public CDbAction
{
public:
	/**
	* 클럽의 공지사항을 세팅한다.
	* \param dwClub 클럽번호
	* \param szClubNotice 공지사항
	* \return 
	*/
	CSetClubNotice(DWORD dwClub, const char* szClubNotice);
	virtual ~CSetClubNotice() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwClub;
	char  m_szNotice[EMCLUB_NOTICE_LEN+1];
};

/**
* 클럽의 선도클럽 인증 대리자를 세팅한다.
*/
class CSetClubDeputy : public CDbAction
{
public:
	/**
	* 클럽의 선도클럽 인증 대리자를 세팅한다.
	* \param dwClub 클럽번호
	* \param dwDeputy 인증대리자 캐릭터번호
	* \return 
	*/
	CSetClubDeputy(DWORD dwClub, DWORD dwDeputy);
	virtual ~CSetClubDeputy() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwClub;
	DWORD m_dwDeputy;
};

/**
* 클럽의 마스터 권한을 위임한다.
*/
class CSetClubAuthority : public CDbAction
{
public:
	/**
	* 클럽의 선도클럽 인증 대리자를 세팅한다.
	* \param dwClub 클럽번호
	* \param dwDeputy 인증대리자 캐릭터번호
	* \return 
	*/
	CSetClubAuthority(DWORD dwClub, DWORD dwMasterID);
	virtual ~CSetClubAuthority() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwClub;
	DWORD m_dwMasterID;
};

/**
* 캐릭터 활동 로그를 기록한다.
*/
class CLogAction : public CDbAction
{
public:
	/**
	* 캐릭터 활동 로그를 기록한다.
	* \param &_sLOG 로그구조체
	* \return 
	*/
	CLogAction ( const SLOGACTION &_sLOG );
	virtual ~CLogAction() {};
    virtual int Execute (CServer* pServer);

protected:
	SLOGACTION m_sLOG;
};

/**
* 캐릭터 이름을 변경한다.
* m_dwChaNum 캐릭터번호
* m_szCharName 변경할 캐릭터이름
*/
class CRenameCharacter : public CDbAction
{
public:
	CRenameCharacter (DWORD dwClientID,
		              DWORD dwChaNum, 
					  const char* szCharName,
					  WORD wPosX,
					  WORD wPosY);
	virtual ~CRenameCharacter() {};
	virtual int Execute (CServer* pServer);	

protected:
	DWORD m_dwChaNum;	
	char  m_szCharName[CHR_ID_LENGTH];
	WORD  m_wPosX;
	WORD  m_wPosY;
};

//! 캐릭터의 머리색을 변경한다.
//! m_dwChaNum : 캐릭터번호
//! m_nHairColor : 머리색깔
class CSetChaHairColor : public CDbAction
{
public:
	CSetChaHairColor(DWORD dwChaNum, 
		                   int nHairColor);
	virtual ~CSetChaHairColor() {};
	virtual int Execute(CServer* pServer);	

protected:
	DWORD m_dwChaNum;
	int   m_nHairColor;
};

//! 캐릭터의 헤어스타일을 변경한다.
//! m_dwChaNum : 캐릭터 번호
//! m_nHairStyle : 헤어스타일
class CSetChaHairStyle : public CDbAction
{
public:
	CSetChaHairStyle(DWORD dwChaNum,
		                   int nHairStyle);
	virtual ~CSetChaHairStyle() {};
	virtual int Execute(CServer* pServer);	

protected:
	DWORD m_dwChaNum;
	int m_nHairStyle;
};

//! 캐릭터의 얼굴스타일을 변경한다.
//! m_dwChaNum : 캐릭터 번호
//! m_nFaceStyle : 얼굴스타일
class CSetChaFaceStyle : public CDbAction
{
public:
	CSetChaFaceStyle(DWORD dwChaNum,
		                   int nFaceStyle);
	virtual ~CSetChaFaceStyle() {};
	virtual int Execute(CServer* pServer);	

protected:
	DWORD m_dwChaNum;
	int m_nFaceStyle;
};


// 캐릭터 성별을 변경한다.
class CSetChaGenderChange : public CDbAction
{
public:
	CSetChaGenderChange( DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor );
	virtual ~CSetChaGenderChange() {};
	virtual int Execute(CServer* pServer);	

protected:
	DWORD m_dwChaNum;
	int m_nClass;
	int	m_nSex;
	int m_nFace;
	int m_nHair;
	int m_nHairColor;
};

//! 캐릭터의 위치정보를 저장한다.
class CSaveChaPos : public CDbAction
{
public:
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
	CSaveChaPos(DWORD dwClientID,
				DWORD dwChaNum,
				DWORD dwFieldServer,
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
				FLOAT fLastCallPosZ);
	virtual ~CSaveChaPos() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwChaNum;
	DWORD m_dwStartMapID;
	DWORD m_dwFieldServer;
	DWORD m_dwStartGate;
	FLOAT m_fStartPosX;
	FLOAT m_fStartPosY;
	FLOAT m_fStartPosZ;
	DWORD m_dwSaveMapID;
	FLOAT m_fSavePosX;
	FLOAT m_fSavePosY;
	FLOAT m_fSavePosZ;
	DWORD m_dwLastCallMapID;
	FLOAT m_fLastCallPosX;
	FLOAT m_fLastCallPosY;
	FLOAT m_fLastCallPosZ;
};

/**
* 새로운 팻을 생성한다.
*/
class CCreatePet : public CDbAction
{
public:
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
	CCreatePet(
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
		int nPetCardSID
		);
	virtual ~CCreatePet() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
	CString m_strPetName;
	int m_nPetType;
	int m_nPetMID;
	int m_nPetSID;
	int m_nPetStyle;
	int m_nPetColor;
	
	WORD m_wPosX;
	WORD m_wPosY;

	int m_nPetCardMID;
	int m_nPetCardSID;
};

/**
* 팻의 이름을 변경한다.
*/
class CRenamePet : public CDbAction
{
public:
	/**
	* 팻의 이름을 변경한다.
	* \param nPetNum 팻의 고유번호
	* \param szPetName 팻이름	
	*/
	CRenamePet(DWORD dwClientID, int nChaNum, int nPetNum, const TCHAR* szPetName, WORD wPosX, WORD wPosY);
	virtual ~CRenamePet() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
	int m_nPetNum;
	CString m_strPetName;
	WORD m_wPosX;
	WORD m_wPosY;
};

/**
* 팻의 소유권을 이전한다.
*/
class CExchangePet : public CDbAction
{
public:
	/**
	* 팻의 소유권을 이전한다.
	* \param nChaNum 팻의 새로운 소유자
	* \param nPetNum 소유권을 이전할 팻 번호
	*/
	CExchangePet(int nChaNum, int nPetNum);
	virtual ~CExchangePet() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
	int m_nPetNum;
};


/**
* 팻의 컬러를 변경한다.
*/
class CSetPetColor : public CDbAction
{
public:
	/**
	* 팻의 컬러를 변경한다.
	* \param nPetNum 컬러를 변경하려는 팻의 번호
	* \param nPetColor 팻의 컬러
	*/
	CSetPetColor(int nChaNum, int nPetNum, int nPetColor);
	virtual ~CSetPetColor() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
	int m_nPetNum;
	int m_nPetColor;
};

/**
* 팻의 스타일을 변경한다.
*/
class CSetPetStyle : public CDbAction
{
public:
	/**
	* 팻의 스타일을 변경한다.
	* \param nPetNum 스타일을 변경하려는 팻의 번호
	* \param nPetStyle 팻의 스타일
	*/
	CSetPetStyle(int nChaNum, int nPetNum, int nPetStyle);
	virtual ~CSetPetStyle() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
	int m_nPetNum;
	int m_nPetStyle;
};

/**
* 팻의 포만도를 설정한다. 포만도는 0-100 사이
*/
class CSetPetFull : public CDbAction
{
public:
	/**
	* 팻의 포만도를 설정한다. 포만도는 0-100 사이
	* \param nPetNum 포만도를 설정할 팻의 번호
	* \param fPetFull 포만도
	*/
	CSetPetFull(int nChaNum, int nPetNum, int nPetFull);
	virtual ~CSetPetFull() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
	int m_nPetNum;
	int m_nPetFull;
};

/**
* 팻의 스킬 업데이트
*/
class CSetPetSkill : public CDbAction
{
public:
	CSetPetSkill(
		int nChaNum,
		int nPetNum,
		PETSKILL& PetSkill,
		bool bActive);
	virtual ~CSetPetSkill() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
	int m_nPetNum;
	PETSKILL m_PetSkill;
	bool m_bActive;
};

/**
* 팻의 인벤토리 업데이트
*  
*/
class CSetPetInven : public CDbAction
{
public:
	CSetPetInven(
		int nChaNum,
		int nPetNum,
		GLPET* pPet);
	virtual ~CSetPetInven() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
	int m_nPetNum;
	CByteStream m_ByteStream;
};



/**
* 팻을 삭제한다.
*/
class CDeletePet : public CDbAction
{
public:
	/**
	* 팻을 삭제한다.
	* \param nPetNum 삭제할 팻의 번호
	*/
    CDeletePet(int nChaNum, int nPetNum);
	virtual ~CDeletePet() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
	int m_nPetNum;
};

/**
* 팻의 정보를 가져온다.
*/
class CGetPet : public CDbAction
{
public:
    CGetPet(GLPET* pPet, DWORD dwPetNum, DWORD dwClientID, DWORD dwCharNum, WORD wPosX, WORD wPosY, bool bLMTItemCheck=false, bool bCardInfo=false, bool bTrade=false );
	virtual ~CGetPet() {};
	virtual int Execute(CServer* pServer);

protected:
	GLPET* m_pPet;
	DWORD  m_dwPetNum;
	bool   m_bLMTItemCheck;
	bool   m_bCardInfo;
	bool   m_bTrade;
	DWORD  m_dwCharNum;
	WORD   m_wPosX;
	WORD   m_wPosY;
};

/**
* 팻의 포만감을 가져온다
* \param dwPetNum 팻 번호
* \return 팻의 포만감 or DB_ERROR
*/
class CGetPetFull : public CDbAction
{
public:
    CGetPetFull( int nChaNum, DWORD dwPetNum, DWORD dwClientID, WORD wCureVolume, BOOL bRatio, SNATIVEID sID, PETTYPE emType );
	virtual ~CGetPetFull() {};
	virtual int Execute( CServer* pServer );

protected:
	int	m_nChaNum;
	DWORD m_dwPetNum;
	int m_nFull;
	WORD m_wCureVolume;
	BOOL m_bRatio;
	SNATIVEID m_sNativeID;
	PETTYPE m_emType;
};

/**
* 소멸된 팻의 리스트를 가져온다.
* \param nChaNum 소멸된 펫 리스트를 출력하려는 캐릭터 번호
* \reutrn DB_OK or DB_ERROR
*/
class CGetRestorePetList : public CDbAction
{
public:
	CGetRestorePetList( int nChaNum, DWORD dwClientID );
	virtual ~CGetRestorePetList() {};
	virtual int Execute( CServer* pServer );
    
protected:
	int m_nChaNum;
};

/**
* 팻을 부활시킨다.
* \param nPetNum 부활할 팻 번호
* \param nChaNum 부활을 요구한 캐릭터 번호
* \return 팻 번호 or DB_ERROR
*/
class CRestorePet : public CDbAction
{
public:
	CRestorePet( DWORD dwPetNum, DWORD dwClientID, int nChaNum, WORD wPosX, WORD wPosY );
	virtual ~CRestorePet() {};
	virtual int Execute( CServer* pServer);

protected:
	int		m_dwPetNum;
	int		m_nChaNum;
	WORD    m_wPosX;
	WORD    m_wPosY;
};


/**
* 팻 스킨 변경 카드를 적용시킨다.
* \param nPetNum 부활할 팻 번호
* \param nChaNum 부활을 요구한 캐릭터 번호
* \return 팻 번호 or DB_ERROR
*/
class CPetSkinPack : public CDbAction
{
public:
	CPetSkinPack( int nChaNum, DWORD dwPetNum, int nPetSkinMID, int nPetSkinSID, int nPetSkinScale, int nPetSkinTime );
	virtual ~CPetSkinPack() {};
	virtual int Execute( CServer* pServer);

protected:
	int   m_nChaNum;
	DWORD m_dwPetNum;
	int   m_nPetSkinMID;
	int   m_nPetSkinSID;
	int   m_nPetSkinScale;
	int   m_nPetSkinTime;
};

/**
* SMS 문자메시지를 발송한다.
*/
class CSendSMS : public CDbAction
{
public:
	/**
	* SMS 문자메시지를 발송한다.
	* \param nSendChaNum SMS 를 발송하는 캐릭터 번호
	* \param szSendUserID SMS 를 발송하는 사용자 ID
	* \param nReceiveChaNum SMS 를 받을 캐릭터 번호
	* \param szReceivePhone SMS 를 받을 캐릭터의 전화번호
	* \param szSmsMsg SMS 메시지 내용
	*/
	CSendSMS(
		DWORD dwClientNum,
		DWORD dwSendChaNum,
		DWORD dwReceiveChaNum,
		TCHAR* szReceivePhone,
		TCHAR* szSendUserID,
		TCHAR* szSmsMsg,
		WORD wPosX,
		WORD wPosY );
	virtual ~CSendSMS() {};
	virtual int Execute( CServer* pServer );

protected:
	DWORD m_dwSendChaNum;
	DWORD m_dwReceiveChaNum;
	TCHAR m_szReceivePhone[SMS_RECEIVER];	
	TCHAR m_szSmsMsg[SMS_LENGTH];
	bool m_bSMS;
	WORD m_wPosX;
	WORD m_wPosY;
};

/**
* 캐릭터의 휴대폰 번호를 저장/업데이트한다.
*/
class CSetChaPhoneNumber : public CDbAction
{
public:
	/**
	* 캐릭터의 휴대폰 번호를 저장/업데이트한다.
	* \param nChaNum 캐릭터 번호
	* \param szPhoneNumber 전화번호 NULL 문자 포함 14자 SMS_RECEIVER
	*/
    CSetChaPhoneNumber(
		DWORD dwClientNum,
		DWORD dwChaNum,
		const TCHAR* szPhoneNumber );
	virtual ~CSetChaPhoneNumber() {};
	virtual int Execute( CServer* pServer );

protected:
	DWORD m_dwChaNum;
	TCHAR m_szPhoneNumber[SMS_RECEIVER];
	bool m_bSMS;
};


/**
* 새로운 탈것을 생성한다.
* \param nChaNum  탈것 소유자 캐릭터 번호
* \param szVehicleName 탈것 이름
* \param nVehicleType 탈것의 타입
* \param nVehicleCardMID 탈것의 아이템 MID
* \param nVehicleCardSID 탈것의 아이템 SID
* \return DB_ERROR 또는 성공했을때는 탈것의 고유번호
*/
class CCreateVehicle : public CDbAction
{
public:
	CCreateVehicle( 
		DWORD dwClientID,
		DWORD dwCharNum,
		const TCHAR* szVehicleName,
		int nVehicleType,
		int nVehicleCardMID,
		int nVehicleCardSID
		);
	virtual ~CCreateVehicle() {};
	virtual int Execute(CServer* pServer);

protected:

	DWORD m_dwCharNum;
	CString m_strVehicleName;
	int m_nVehicleType;
	int m_nVehicleCardMID;
	int m_nVehicleCardSID;
};


/**
* 탈것의 배터리를 설정한다. 배터리는 0-1000 사이
* \param nVehicleBttery 배터리를 설정할 탈것의 번호
* \param nVehicleBttery 배터리
*/
class CSetVehicleBattery : public CDbAction
{
public:
	CSetVehicleBattery(
		DWORD dwClientID,
		DWORD dwCharNum,
		int nVehicleNum,
		int nVehicleBttery
		);
	virtual ~CSetVehicleBattery() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD m_dwCharNum;
	int m_nVehicleNum;
	int m_nVehicleBttery;
};



/**
* 탈것의 포만감을 가져온다
* \param nVehicleNum 탈것 번호
* \return 탈것 포만감 or DB_ERROR
*/

class CGetVehicleBattery : public CDbAction
{
public:
	CGetVehicleBattery( 
		DWORD dwClientID, 
		DWORD dwCharNum,
		int	  nVehicleNum, 
		SNATIVEID	sItemID,
		WORD  wCureVolume, 
		BOOL  bRatio, 
		SNATIVEID sBatteryID,
		VEHICLE_TYPE emType );

	virtual ~CGetVehicleBattery() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD		m_dwCharNum;
	int			m_nVehicleNum;
	SNATIVEID	m_sItemID;
	int			m_nVehicleBttery;
	WORD		m_wCureVolume;
	BOOL		m_bRatio;
	SNATIVEID	m_sBatteryID;
	VEHICLE_TYPE m_emType;
};

/**
* 탈것을 삭제한다.
* \param nVehicle 삭제할 탈것의 번호
*/
class CDeleteVehicle : public CDbAction
{
public:
	CDeleteVehicle( 
		DWORD dwClientID,
		DWORD dwCharNum,
		int nVehicleNum
		);

	virtual ~CDeleteVehicle() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD	m_dwCharNum;
	int		m_nVehicleNum;	
};

/**
* 탈것의 정보를 가져온다.
*/
class CGetVehicle : public CDbAction
{
public:
	CGetVehicle(
		GLVEHICLE* pVehicle,
		DWORD dwVehicleNum,
		DWORD dwClientID,
		DWORD dwCharNum,
		bool bLMTItemCheck=false, 
		bool bCardInfo=false, 
		bool bTrade=false
		);

	virtual ~CGetVehicle() {};
	virtual int Execute(CServer* pServer);

protected:
	GLVEHICLE* m_pVehicle;
	DWORD m_dwVehicleNum;
	DWORD	m_dwCharNum;
	bool  m_bLMTItemCheck;
	bool  m_bCardInfo;
	bool  m_bTrade;
};

/**
* 탈것 인벤토리 업데이트
*  
*/
class CSetVehicleInven : public CDbAction
{
public:
	CSetVehicleInven(
		DWORD dwClientID,
		DWORD dwCharNum,
		int nVehicleNum,		
		GLVEHICLE* pVehicle
		);
	virtual ~CSetVehicleInven() {};
	virtual int Execute(CServer* pServer);

protected:
	DWORD	m_dwCharNum;
	int m_nVehicleNum;
	CByteStream m_ByteStream;
};

/**
* 탈것의 액션 로그를 남긴다.
* \param _sLOG 펫액션로그 구조체
*/
class CLogVehicleAction : public CDbAction
{
public:
	CLogVehicleAction( const SLOGVEHICLEACTION& sLOG );
	virtual ~CLogVehicleAction() {};
	virtual int Execute( CServer* pServer );

protected:
	SLOGVEHICLEACTION m_sLOG;
};


class CInsertUserAttend : public CDbAction
{
public:
	CInsertUserAttend( DWORD dwUserID, int nComboAttend, int nAttendReward );
	virtual ~CInsertUserAttend() {};
	virtual int Execute( CServer* pServer );

protected:
	DWORD m_dwUserID;
	int	  m_nComboAttend;
	int	  m_nAttendReward;
};

class CInsertAttendItem : public CDbAction
{
public:
	CInsertAttendItem( const CString& strPurKey, const CString& strUserID, int nItemMID, int nItemSID );
	virtual ~CInsertAttendItem() {};
	virtual int Execute ( CServer* pServer );

protected:
	CString m_strPurKey;
	CString m_strUserID;
	int	  m_nItemMID;
	int	  m_nItemSID;
};

class CThaiCafeClassCheck : public CDbAction
{
public:
	CThaiCafeClassCheck ( const DWORD dwClientID, const DWORD dwCClassType, const TCHAR *pszIP );
	virtual ~CThaiCafeClassCheck() {};
	virtual int Execute ( CServer* pserver );
protected:
	DWORD m_dwClientID;
	DWORD m_dwCClassType;
	TCHAR m_szIp[MAX_IP_LENGTH+1];
};

class CMyCafeClassCheck : public CDbAction
{
public:
	CMyCafeClassCheck ( const DWORD dwClientID, const DWORD dwCClassType, const TCHAR *pszIP );
	virtual ~CMyCafeClassCheck() {};
	virtual int Execute ( CServer* pserver );
protected:
	DWORD m_dwClientID;
	DWORD m_dwCClassType;
	TCHAR m_szIp[MAX_IP_LENGTH+1];
};

class CUpdateUserMoneyAdd : public CDbAction
{
public:
	CUpdateUserMoneyAdd ( const DWORD dwUserID, const LONGLONG lnUserMoney );
	virtual ~CUpdateUserMoneyAdd() {};
	virtual int Execute ( CServer* pserver );
protected:
	DWORD m_dwUserID;
	LONGLONG m_lnUserMoney;
};

class CUpdateUserLastInfoAdd : public CDbAction
{
public:
	CUpdateUserLastInfoAdd ( const DWORD dwUserID, const LONGLONG lnUserMoney );
	virtual ~CUpdateUserLastInfoAdd() {};
	virtual int Execute ( CServer* pserver );
protected:
	DWORD m_dwUserID;
	LONGLONG m_lnUserMoney;
};

#endif // DBACTIONLOGIC_H_