#ifndef GLDBMAN_H_
#define GLDBMAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gassert.h"
#include <vector>

#include "../Dependency/NetGlobal/s_NetGlobal.h" // struct VIEWLOGITEMEXCHANGEMAX; struct SHOPPURCHASE;
// #include "../[Lib]__NetServer/Sources/s_CDbAction.h" // class CDbAction;

#include "./ByteStream.h" // class CByteStream;

#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLCharData.h" // struct SCHARDATA2;
#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLClubMan.h" // struct GLCLUBMEMBER; struct GLCLUBINFO;
#include "../[Lib]__RanClient/Sources/G-Logic/Server/GLGuidance.h" // struct GLGUID_DB;
#include "../[Lib]__RanClient/Sources/G-Logic/GLPet.h" //struct GLPET;


// struct VIEWLOGITEMEXCHANGEMAX;
// struct SHOPPURCHASE;
// struct SCHARDATA2;
// struct GLCLUBMEMBER;
// struct GLCLUBINFO;
// struct GLGUID_DB;
// struct GLPET;

// class CByteStream;
class CDbAction;

struct SLOGITEMEXCHANGE
{
	int			m_nNID_M;			// 아이템 주번호
	int			m_nNID_S;			// 아이템 부번호
	int			m_nSGNum;			// 서버그룹
	int			m_nSvrNum;			// 서버번호
	int			m_nFldNum;			// 필드번호
	int			m_nMakeType;		// 타입
	LONGLONG	m_lnMakeNum;		// 생성번호
	int			m_nFromType;		// from의 종류 ( 유저, 케릭터, 클럽 )
	int			m_nItemFrom;		// 아이템 출발지
	int			m_nToType;			// to의 종류 ( 유저, 케릭터, 클럽 )
	int			m_nItemTo;			// 아이템 목적지
	int			m_nExchangeFlag;	// 교환 타입
	int			m_nNum;

	//	코스툼 변환 정보.
	int			m_nCOSTUME_MID;
	int			m_nCOSTUME_SID;

	//	연마 정보.
	short		m_nDamage;
	short		m_nDefense;
	short		m_nFire;
	short		m_nIce;
	short		m_nPoison;
	short		m_nElectric;
	short		m_nSpirit;

	SLOGITEMEXCHANGE(
		int _nNID_M,
		int _nNID_S,
		int _nSGNum,
		int _nSvrNum,
		int _nFldNum,
		int _nMakeType,
		LONGLONG _lnMakeNum,
		int _nFromType,
		int _nItemFrom,
		int _nToType,
		int _nItemTo,
		int _nExchangeFlag,
		int _nNum,

		int _nCOSTUME_MID,
		int _nCOSTUME_SID,

		short _nDamage,
		short _nDefense,
		short _nFire,
		short _nIce,
		short _nPoison,
		short _nElectric,
		short _nSpirit	) 
		: m_nNID_M(_nNID_M)
		, m_nNID_S(_nNID_S)
		, m_nSGNum(_nSGNum)
		, m_nSvrNum(_nSvrNum)
		, m_nFldNum(_nFldNum)
		, m_nMakeType(_nMakeType)
		, m_lnMakeNum(_lnMakeNum)
		, m_nFromType(_nFromType)
		, m_nItemFrom(_nItemFrom)
		, m_nToType(_nToType)
		, m_nItemTo(_nItemTo)
		, m_nExchangeFlag(_nExchangeFlag)
		, m_nNum(_nNum)
		, m_nCOSTUME_MID(_nCOSTUME_MID)
		, m_nCOSTUME_SID(_nCOSTUME_SID)
		, m_nDamage(_nDamage)
		, m_nDefense(_nDefense)
		, m_nFire(_nFire)
		, m_nIce(_nIce)
		, m_nPoison(_nPoison)
		, m_nElectric(_nElectric)
		, m_nSpirit(_nSpirit)
	{
	}

	SLOGITEMEXCHANGE () 
		: m_nNID_M(0)
		, m_nNID_S(0)
		, m_nSGNum(0)
		, m_nSvrNum(0)
		, m_nFldNum(0)
		, m_nMakeType(0)
		, m_lnMakeNum(0)
		, m_nFromType(0)
		, m_nItemFrom(0)
		, m_nToType(0)
		, m_nItemTo(0)
		, m_nExchangeFlag(0)
		, m_nNum(0)
		, m_nCOSTUME_MID(0)
		, m_nCOSTUME_SID(0)
		, m_nDamage(0)
		, m_nDefense(0)
		, m_nFire(0)
		, m_nIce(0)
		, m_nPoison(0)
		, m_nElectric(0)
		, m_nSpirit(0)
	{
	}
};

struct SLOGMONEYEXCHANGE
{
	int			m_nSGNum;
	int			m_nSvrNum;
	int			m_nFldNum;
	int			m_nFromType;
	int			m_nItemFrom;
	int			m_nToType;
	int			m_nItemTo;
	LONGLONG	m_lnPrice;
	int			m_nExchangeFlag;

	SLOGMONEYEXCHANGE () 
		: m_nSGNum(0)
		, m_nSvrNum(0)
		, m_nFldNum(0)
		, m_nFromType(0)
		, m_nItemFrom(0)
		, m_nToType(0)
		, m_nItemTo(0)
		, m_lnPrice(0)
		, m_nExchangeFlag(0)
	{
	}

	SLOGMONEYEXCHANGE (
		int _nSGNum,
		int _nSvrNum,
		int _nFldNum,

		int _nFromType,
		int _nItemFrom,
		
		int _nToType,
		int _nItemTo,

		LONGLONG _lnPrice,
		int _nExchangeFlag
		) 
		: m_nSGNum(_nSGNum)
		, m_nSvrNum(_nSvrNum)
		, m_nFldNum(_nFldNum)
		, m_nFromType(_nFromType)
		, m_nItemFrom(_nItemFrom)
		, m_nToType(_nToType)
		, m_nItemTo(_nItemTo)
		, m_lnPrice(_lnPrice)
		, m_nExchangeFlag(_nExchangeFlag)
	{
	}
};

struct SLOGITEMCONVERSION
{
	int			m_nNIDMain;
	int			m_nNIDSub;
	int			m_nSGNum;
	int			m_nSvrNum;
	int			m_nFldNum;
	int			m_nMakeType;
	LONGLONG	m_lnMakeNum;

	int			m_nFromType;
	int			m_nItemFrom;

	int			m_nCOSTUME_MID;
	int			m_nCOSTUME_SID;

	short		m_nDamage;
	short		m_nDefense;
	short		m_nFire;
	short		m_nIce;
	short		m_nPoison;
	short		m_nElectric;
	short		m_nSpirit;

	SLOGITEMCONVERSION () 
		: m_nNIDMain(0)
		, m_nNIDSub(0)
		, m_nSGNum(0)
		, m_nSvrNum(0)
		, m_nFldNum(0)
		, m_nMakeType(0)
		, m_lnMakeNum(0)
		, m_nFromType(0)
		, m_nItemFrom(0)
		, m_nCOSTUME_MID(0)
		, m_nCOSTUME_SID(0)
		, m_nDamage(0)
		, m_nDefense(0)
		, m_nFire(0)
		, m_nIce(0)
		, m_nPoison(0)
		, m_nElectric(0)
		, m_nSpirit(0)
	{
	}

	SLOGITEMCONVERSION (
		int _nNIDMain,
		int _nNIDSub,
		int _nSGNum,
		int _nSvrNum,
		int _nFldNum,
		int _nMakeType,
		LONGLONG _lnMakeNum,

		int _nFromType,
		int _nItemFrom,

		int _nCOSTUME_MID,
		int _nCOSTUME_SID,

		short _nDamage,
		short _nDefense,
		short _nFire,
		short _nIce,
		short _nPoison,
		short _nElectric,
		short _nSpirit ) 
		: m_nNIDMain(_nNIDMain)
		, m_nNIDSub(_nNIDSub)
		, m_nSGNum(_nSGNum)
		, m_nSvrNum(_nSvrNum)
		, m_nFldNum(_nFldNum)
		, m_nMakeType(_nMakeType)
		, m_lnMakeNum(_lnMakeNum)
		, m_nFromType(_nFromType)
		, m_nItemFrom(_nItemFrom)
		, m_nCOSTUME_MID(_nCOSTUME_MID)
		, m_nCOSTUME_SID(_nCOSTUME_SID)
		, m_nDamage(_nDamage)
		, m_nDefense(_nDefense)
		, m_nFire(_nFire)
		, m_nIce(_nIce)
		, m_nPoison(_nPoison)
		, m_nElectric(_nElectric)
		, m_nSpirit(_nSpirit)
	{
	}
};

struct SLOGACTION
{
	int			m_nCHARID;
	int			m_nTYPE;

	int			m_nTARTYPE;
	int			m_nTARID;

	__int64		m_nEXP;
	int			m_nBRIGHT_POINT;
	int			m_nLIFE_POINT;
	int			m_nMONEY;

	SLOGACTION () 
		: m_nCHARID(0)
		, m_nTYPE(0)
		, m_nTARTYPE(0)
		, m_nTARID(0)
		, m_nEXP(0)
		, m_nBRIGHT_POINT(0)
		, m_nLIFE_POINT(0)
		, m_nMONEY(0)
	{
	}
};

/**
* 아이템의 랜덤옵션 로그를 남기기 위한 구조체.
*/
struct SLOGRANDOMOPTION
{
	// 아이템 고유값
	int nNIDMain; // 아이템고유번호 MID
	int nNIDSub;  // 아이템고유번호 SID
	int nSGNum;   // 서버그룹
	int nSvrNum;  
	int nFldNum;  // 필드번호
	int nMakeType; // 발생타입
	LONGLONG lnMakeNum; // 아이템고유번호

	//	랜덤 옵션 종류
	BYTE cOptTYPE1; // 랜덤 옵션 종류 #1
	BYTE cOptTYPE2; // 랜덤 옵션 종류 #2
	BYTE cOptTYPE3; // 랜덤 옵션 종류 #3
	BYTE cOptTYPE4; // 랜덤 옵션 종류 #4

	// 랜덤 옵션 값
	short nOptVALUE1; // % #1
	short nOptVALUE2; // % #2
	short nOptVALUE3; // % #3
	short nOptVALUE4; // % #4

	SLOGRANDOMOPTION()
		: nNIDMain(0)
		, nNIDSub(0)
		, nSGNum(0)
		, nSvrNum(0)
		, nFldNum(0)
		, nMakeType(0)
		, lnMakeNum(0)
		, cOptTYPE1(0) 
		, cOptTYPE2(0) 
		, cOptTYPE3(0) 
		, cOptTYPE4(0) 
		, nOptVALUE1(0)
		, nOptVALUE2(0)
		, nOptVALUE3(0)
		, nOptVALUE4(0)
	{
	}
};

/**
* 펫 액션 로그를 남기기 위한 구조체.
*/
struct SLOGPETACTION 
{
	int nPetNum;		// 펫 넘버
	int nItemMID;		// 아이템 MID
	int nItemSID;		// 아이템 SID
	int nActionType;	// 펫 액션 타입
	int nPetFull;		// 펫 포만도

	SLOGPETACTION()
		: nPetNum(0)
		, nItemMID(0)
		, nItemSID(0)
		, nActionType(0)
		, nPetFull(0)
	{
	}
};

struct SLOGVEHICLEACTION 
{
	int nVehicleNum;		// 펫 넘버
	int nItemMID;		// 아이템 MID
	int nItemSID;		// 아이템 SID
	int nActionType;	// 펫 액션 타입
	int nVehicleFull;		// 펫 포만도

	SLOGVEHICLEACTION()
		: nVehicleNum(0)
		, nItemMID(0)
		, nItemSID(0)
		, nActionType(0)
		, nVehicleFull(0)
	{
	}
};

class GLDBMan
{
public:
	virtual int SaveCharacter( LPVOID _pbuffer ) = 0;
	virtual int	ReadUserInven( SCHARDATA2* pChaData2 ) = 0;

	virtual int DelChaFriend( int nChaP, int nChaS )				{ return 0; };
	virtual int AddChaFriend( int nChaP, int nChaS )				{ return 0; };
	virtual int SetChaFriend( int nChaP, int nChaS, int nFlag )	{ return 0; };

	virtual int LogPartyMatch( int nSGNum, int nSvrNum, WORD wWin, WORD wLost )			{ return 0; }

    virtual LONGLONG GetItemMaxNum(
						int nSGNum,
						int nSvrNum,
						int nFldNum, 
                        std::vector<VIEWLOGITEMEXCHANGEMAX> &v )             { return 0; }

	virtual int LogItemExchange( const SLOGITEMEXCHANGE &sEXCHANGE )					{ return 0; }
	virtual int LogMoneyExchange( const SLOGMONEYEXCHANGE &_sLOG )						{ return 0; }
    virtual int LogItemConversion( const SLOGITEMCONVERSION &_sLOG )					{ return 0; }
    virtual int LogAction( const SLOGACTION &_sLOG )									{ return 0; }
    
	/**
	* Shop 에서 구입한 아이템을 가져온다.
	* \param strUID 유저ID
	* \param &v 상품을 담을 벡터
	* \return 
	*/
    virtual int GetPurchaseItem( CString strUID, std::vector<SHOPPURCHASE> &v ) { return 0; } 

	/**
	* 실제로 가져갈 수 있는 상품인지 확인한다.
	* \param strPurKey 구입코드
	* \param nFlag (0 : 구매, 1 : 구매완료, 2 : 구매취소신청, 3 : 구매취소처리)
	* \return 1:가져갈수 있음 이외의값:가져갈수 없음
	*/
    virtual int SetPurchaseItem( CString strPurKey, int nFlag=1 ) { return 0; }

	/**
	* 캐릭터 삭제, 캐릭터생성 갯수 증가
	*/
	virtual int UserChaNumIncrease( int nUserNum ) { return 0; }

	/**
	* 해당 락커 만료시간을 정한다.
	* \n 1 번 5 번 락커는 정할 수 없음.
	* \n 2, 3, 4 번 락커만 만료시간을 정할 수 있다.
	* \param nUserNum 사용자번호
	* \param nStorageNum 스토리지번호
	* \param tTime 만료시간
	* \return -1:DB_ERROR  0:DB_OK
	*/
    virtual int SetChaStorageDate(
					int nUserNum,
					int nStorageNum,
					__time64_t tTime ) { return 0; }
    
	/**
	* 해당 캐릭터의 인벤토리 추가 줄수를 세팅한다.
	* \n 최초 0 에서 한줄 추가시 1 을 입력
    * \n 현재 한줄 상태에서 또 한줄 추가시 2 를 입력 (최종 줄수를 입력한다)
    * \n 현재 두줄 상태에서 한줄 추가시 3 을 입력
    * \n 최대 3까지만 입력가능.
	* \param nChaNum 캐릭터번호
	* \param wInvenLine 추가줄수
	* \return 
	*/
    virtual int SetChaInvenNum(
					int nChaNum,
					WORD wInvenLine ) { return 0; }
    
	/**
	* 해당 사용자의 프리미엄 기간을 세팅한다.
	*/
    virtual int SetPremiumTime(
					int nUserNum,
					__time64_t tPremiumTime ) { return 0; }
    
	/**    
	* 직전귀환 카드 사용을 위한 포지션을 저장한다.
	*/
    virtual int SetLastCallPos(
					int nChaNum,
					DWORD dwMapID,
					D3DXVECTOR3 vPos ) { return 0; }
    
	/**
	* 새로운 캐릭터를 생성한다.
	*/
    virtual int	CreateNewCharacter(SCHARDATA2* pCharData2) { return 0; }

    /**
    * 새로운 클럽을 생성한다.
    * 입력
    *     strClubName : 클럽이름
    *     dwChaNum : 클럽을 생성하려는 캐릭터 번호 (이 캐릭터가 클럽의 마스터가 된다)
    * 출력
    *     -1 : 이미 길드마스터로 등록되어 있음 생성불가.
    *     -2 : 길드생성중 에러발생 (중복된길드이름)
    *      0 보다 클때 : 생성된 길드번호임
	*/
    virtual int CreateClub(CString strClubName, DWORD dwChaNum) { return 0; }
    
    /**
    * 클럽을 삭제한다.
    * dwClub : 클럽번호
    * dwChaNum : 클럽을 삭제하려는 캐릭터번호(캐릭터 번호가 클럽 마스터가 아니면 삭제가 되지 않는다)
	*/
    virtual int DeleteClub(DWORD dwClub, DWORD dwChaNum)                                  { return 0; }
    
    /**
    * 클럽랭크를 세팅한다.
    * dwClub : 클럽번호
    * dwRank : 랭킹
	*/
    virtual int SetClubRank(DWORD dwClub, DWORD dwRank)                                   { return 0; }

    virtual int ReadClubMarkImage(
					DWORD dwClub, 
					CByteStream &ByteStream ) { return 0; }

    virtual int WriteClubMarkImage(
					DWORD dwClub,
					DWORD dwMarkVer,
					BYTE* pData,
					int nSize ) { return 0; }
    
	/**
	* 클럽 해체시간을 설정한다.
	* \param dwClub 클럽번호
	* \param tDiss 해체시간
	* \return 
	*/
    virtual int SetClubDissolutionTime(DWORD dwClub, __time64_t tDiss)                    { return 0; }

	/**
	* 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
	* \param dwClub 클럽번호
	* \param tSec 시간
	* \return 
	*/
	virtual int SetClubAllianceSec(DWORD dwClub, __time64_t tSec) { return 0; }

	/**
	* 클럽의 동맹해산 시간을 정한다.
	* \param dwClub 클럽번호
	* \param tDis 해산시간
	* \return 
	*/
	virtual int SetClubAllianceDis(DWORD dwClub, __time64_t tDis) { return 0; }

	/**
	* 클럽의 마스터 위임 시간을 정한다.
	* \param dwClub 클럽번호
	* \param tDis 마스터 위임시간
	* \return 
	*/
	virtual int SetClubAuthorityTime(DWORD dwClub, __time64_t tAuthority) { return 0; }
	

    /**
    * 서버의 모든 클럽정보를 가져온다    
    * \param vClub : 클럽정보
	*/
    virtual int GetClubInfo(std::vector<GLCLUBINFO> &vClub)                               { return 0; }
    
    /**
    * 해당 클럽의 멤버를 가져온다
    * \param dwGuild : 클럽번호
    * \param vMember : 클럽멤버
	*/
    virtual int GetClubMember(DWORD dwClub, std::vector<GLCLUBMEMBER> &vMember)           { return 0; }
    
    /**
    * 새로운 캐릭터를 클럽에 가입시킨다
    * \param dwClub : 클럽번호
    * \param dwChaNum : 가입시키려는 캐릭터번호
	*/
    virtual int AddClubMember(DWORD dwClub, DWORD dwChaNum)                               { return 0; }
    
    /**
    * 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
    * \param dwChaNum : 캐릭터번호
	*/
    virtual int DeleteClubMember(DWORD dwChaNum)                                          { return 0; }

	/**
	* 클럽 배틀을 신청한다.
	* \param dwClubP 주클럽번호( 클럽 배틀의 신청을 받은 주 )
	* \param dwClubS 보조 클럽번호( 클럽 배틀의 신청을 한 클럽 )
	* \return
	*/
	virtual int SetClubBattle( DWORD dwClubP, DWORD dwClub, DWORD dwEndTime, int nAlliance )	{ return 0; }

	/**
	* 클럽 배틀을 종료한다.
	* \param dwClubP 주클럽 번호( 클럽 배틀의 신청을 받은 주 )
	* \param dwClubS 보조 클럽 번호( 클럽 배틀의 신청을 한 클럽 )
	* \param nGuFlag 승패여부
	* \param nGuKillNum 클럽 배틀 킬수
	* \return
	*/
	virtual int EndClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance )	{ return 0; }

	/**
	* 클럽 배틀정보를 저장한다,( 클럽 배틀이 종료되기전 서버재시작이 필요한경우 배틀 정보의 저장이 필요함 )
	* \param dwClubP 주클럽 번호( 클럽 배틀의 신청을 받은 주)
	* \param dwClubS 보조 클럽 번호( 클럽 배틀의 신청을 한 클럽 )
	* \param nGuKillNum 클럽 배틀 킬수
	* \return
	*/
	virtual int SaveClubBattle( DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum )	{ return 0; }

	
	/**
	* 동맹배틀 전적을 초기화 시키긴다.
	* \param dwClub 클럽 번호
	* \return
	*/
	virtual int ReSetAllianceBattle( DWORD dwClub ) { return 0; }


	/**
	* 해당 클럽의 배틀중인 클럽 정보를 가져온다.
	* \param dwClub 클럽번호
	* \param &vMember 클럽배틀 정보
	* \return 
	*/
    virtual int GetClubBattleInfo( DWORD dwClub, std::vector<GLCLUBBATTLE> &vBattleInfo )	 { return 0; }

    /**
    * 해당 캐릭터의 경험치를 세팅한다.
	* 캐릭터번호는 0 보다 커야한다.
    * 경험치는 0 보다 커야한다.
    * \param  nChaNum : 캐릭터번호
    * \param   llExp : 경험치
	*/    
    virtual int SetChaExp(int nChaNum, LONGLONG llExp)                                      { return 0; }

    /**
    * 해당 캐릭터의 경험치를 가져온다.    
	* 캐릭터번호는 0 보다 커야한다.
    * 경험치가 0 보다 작으면 에러이다.
    * \param nChaNum 캐릭터번호
    *  LONGLONG : 경험치
	*/    
    virtual LONGLONG GetChaExp(int nChaNum)                                                 { return 0; }
    
    /**
    * 게임내에서 진정이나 신고로 신고한 내용을 DB에 기록한다.    
    * \param nSGNum 서버그룹
    * \param ChaNum 신고한 캐릭터번호
    * \param strMsg 신고내용 (최대크기는 CHAT_MSG_SIZE)
	*/
    virtual int LogAppeal(int nSGNum, int nChaNum, CString strMsg) { return 0; }
    
    /**
    * 현재 모든지역의 선도 클럽데이터를 가져온다.
	*/
    virtual int GetClubRegion(std::vector<GLGUID_DB> &vGUID_DB) { return 0; }
    
    /**
    * 지역과 그지역을 소유한 클럽 세율을 정한다.
    * \param dwRegionID : 지역 ID
    * \param dwClub : 클럽 ID
    * \param fTax : 세율
	*/
    virtual int SetClubRegion(DWORD dwRegionID, DWORD dwClub=0, float fTax=0) { return 0; }
    
    /**
    * 해당 지역의 소유 상태를 리셋한다.
    * \param dwRegionID 지역 ID
    * \param dwClub 클럽 ID (기본 0 이다)
	*/
    virtual int DelClubRegion(DWORD dwRegionID, DWORD dwClub=0) { return 0; }
	
	/**
	* 해당클럽의 보유금액을 가져온다. (Storage)
	* \param dwClub 클럽번호
	* \param 리턴값은 >= 0, 0 보다 작으면 ERROR
	*/
    virtual LONGLONG GetClubMoney(DWORD dwClub) { return 0; }

	/**
	* 해당클럽의 보유금액을 세팅한다. (Storage)
	* \param dwClub : 클럽번호
	* \param llMoney : 보유금액 (금액은 >= 0)
	*/
	virtual int	SetClubMoney(DWORD dwClub, LONGLONG llMoney) { return 0; }
	
	/**
	* 해당클럽의 수입금액을 세팅한다. (Income money)
	* \param dwClub : 클럽번호
	* \param llMoney : 수입금액 (금액은 >= 0)
	*/
	virtual int	SetClubIncomeMoney(DWORD dwClub, LONGLONG llMoney) { return 0; }
    
    /**
	* 해당클럽의 수입금액을 가져온다. (Income money)
	* \param dwClub : 클럽번호
	* \return 리턴값은 >= 0, 0 보다 작으면 ERROR
	*/
	virtual LONGLONG GetClubIncomeMoney(DWORD dwClub) { return 0; }
    
    /**
	* 해당클럽의 클럽창고를 읽어온다.
	* \param dwClub : 클럽번호
	* \param ByteStream : 버퍼
	*/
	virtual int ReadClubStorage(DWORD dwClub, CByteStream &ByteStream) { return 0; }
    
    /**
	* 해당클럽의 클럽창고를 저장한다.
	* \param dwClub : 클럽번호
	* \param pData : 창고위치
	* \param nSize : 버퍼 전체크기
	*/	
	virtual int	WriteClubStorage(DWORD dwClub, BYTE* pData, int nSize) { return 0; }
	
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
	virtual int LogHackProgram(
		int nSGNum, 
		int nSvrNum, 
		int nUserNum, 
		int nChaNum, 
		int nHackProgramNum,
		const TCHAR* szComment) { return 0 ; }
	
	/**
	* 부 클럽장을 세팅한다
	* \param dwClub : 클럽번호
	* \param dwChaNum : 캐릭터번호
	* \param dwSubMasterFlags : 플래그 (0 으로 넣으면 일반길드원이 된다)
	*/
	virtual int	SetClubMasterFlags(
		DWORD dwClub,
		DWORD dwChaNum,
		DWORD dwSubMasterFlags) { return 0 ; }

	/**
	* 동맹 클럽을 결성한다.
	* \ db 는 클럽의 무결성을 체크하지 않는다.
	* \ A 가 B 의 보조인 상태에서 
	* \ B 가 C 의 보조로 들어가고
	* \ C 가 A 의 보조로 들어가면 소유권 체인 문제가 발생한다.	
	* \param dwClubP : 주 클럽번호 (동맹 클럽의 주)
	* \param dwClubS : 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
	*/
	virtual int	SetClubAlliance(DWORD dwClubP, DWORD dwClubS) { return 0 ; }

	/**
	* 동맹 클럽을 해체한다.
	* \param dwClubP : 주 클럽번호 (동맹 클럽의 주)
	* \param dwClubS : 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
	*/
	virtual int	DelClubAlliance(DWORD dwClubP, DWORD dwClubS) { return 0; }
	
	/**
	* 동맹 클럽 리스트를 가져온다.
	* <주클럽,보조클럽>
	*/
	virtual int	GetClubAlliance ( std::vector< std::pair<DWORD,DWORD> > &vecAlliance )	{ return 0; }

	/**
	* 해당 사용자의 채팅 블록시간을 가져온다
	*/
	virtual __time64_t  GetChatBlockTime (int nUserNum) { return 0; }

	/**
	* 해당 사용자의 채팅 블록시간을 설정한다
	*/
	virtual int SetChatBlockTime (int nUserNum, __time64_t tBlockTime) { return 0; }
	
	/**
	* 클럽의 공지사항을 세팅한다.
	*/
	virtual int SetClubNotice (DWORD dwClub, const char* szClubNotice) { return 0; }
	
	/**
	* 클럽의 선도클럽 인증 대리자를 세팅한다.
	*/
	virtual int SetClubDeputy (DWORD dwClub, DWORD dwDeputy) { return 0; }

	/**
	* 클럽의 마스터 권한을 위임한다.
	*/
	virtual int SetClubAuthority (DWORD dwClub, DWORD dwMasterID) { return 0; }

	

	/**
	* 캐릭터의 이름을 변경한다.
	* \param dwChaNum 캐릭터번호
	* \param szCharName 변경할 캐릭터이름
	* \return DB_OK, DB_ERROR
	*/
	virtual int RenameCharacter (DWORD dwChaNum, char* szCharName) { return 0; }

	/**
	* 캐릭터의 머리색을 변경한다.
	* \return DB_OK, DB_ERROR
	*/
	virtual int SetChaHairColor(DWORD dwChaNum, int nHairColor) { return 0; }
	
	/**
	* 캐릭터의 헤어스타일을 변경한다.
	* \return DB_OK, DB_ERROR	
	*/
	virtual int SetChaHairStyle(DWORD dwChaNum, int nHairStyle) { return 0; }

	/**
	* 캐릭터의 얼굴스타일을 변경한다.
	* \return DB_OK, DB_ERROR	
	*/
	virtual int SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle) { return 0; }


	/**
	* 캐릭터의 성별을 변경한다.
	* \return DB_OK, DB_ERROR	
	*/
	virtual int SetChaGenderChange( DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor) { return 0; }

	/**
	* 캐릭터의 위치정보를 저장한다.
	* \param dwChaNum 캐릭터 번호
	* \param dwStartmapID 시작하는 맵 번호
	* \param dwStartGate 시작하는 게이트 번호
	* \param fStartPosX 시작하는 X 좌표
	* \param fStartPosY 시작하는 Y 좌표
	* \param fStartPosZ 시작하는 Z 좌표
	* \param dwSaveMapID 마지막으로 플레이한 맵 번호
	* \param fSavePosX 마지막으로 플레이한 X 좌표
	* \param fSavePosY 마지막으로 플레이한 Y 좌표
	* \param fSavePosZ 마지막으로 플레이한 Z 좌표
	* \param dwLastCallMapID 시작 귀환카드에 저장된 맵 번호
	* \param fLastCallPosX 시작 귀환카드에 저장된 X 좌표
	* \param fLastCallPosY 시작 귀환카드에 저장된 Y 좌표
	* \param fLastCallPosZ 시작 귀환카드에 저장된 Z 좌표
	* \return DB_OK, DB_ERROR
	*/
	virtual int	SaveChaPos(DWORD dwChaNum,
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
				 FLOAT fLastCallPosZ) { return 0; }

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
	virtual int CreatePet(
		int nChaNum,
		const TCHAR* szPetName,
		int nPetType,
		int nPetMID,
		int nPetSID,
		int nPetStyle,
		int nPetColor,
		int nPetCardMID,
	    int nPetCardSID
		) { return 0; }

	/**
	* 펫의 이름을 변경한다.
	* \param nChaNum 캐릭터 번호
	* \param nPetNum 팻의 고유번호
	* \param szPetName 팻이름
	* \return DB_ERROR, DB_OK
	*/
	virtual int RenamePet( int nChaNum, int nPetNum, const TCHAR* szPetName) { return 0; }

	/**
	* 팻의 소유권을 이전한다.
	* \param nChaNum 팻의 새로운 소유자
	* \param nPetNum 소유권을 이전할 팻 번호
	*/
	virtual int ExchangePet(int nChaNum, int nPetNum) { return 0; }

	/**
	* 팻의 컬러를 변경한다.
	* \param nChaNum 캐릭터 번호
	* \param nPetNum 컬러를 변경하려는 팻의 번호
	* \param nPetColor 팻의 컬러
	*/
	virtual int SetPetColor(int nChaNum, int nPetNum, int nPetColor) { return 0; }

	/**
	* 팻의 스타일을 변경한다.
	* \param nPetNum 스타일을 변경하려는 팻의 번호
	* \param nPetStyle 팻의 스타일
	*
	*/
	virtual int SetPetStyle( int nChaNum, int nPetNum, int nPetStyle) { return 0; }

	/**
	* 팻의 포만도를 설정한다. 포만도는 0-100 사이
	* \param nPetNum 포만도를 설정할 팻의 번호
	* \param fPetFull 포만도
	*/
	virtual int SetPetFull( int nChaNum, int nPetNum, int nPetFull) { return 0; }

	/**
	* 팻의 인벤토리 업데이트
	*/
	virtual int SetPetInven(
		int nChaNum,
		int nPetNum,
		CByteStream &ByteStream ) { return 0; }

	/**
	* 팻의 스킬 업데이트
	*  
	*/	
	virtual int SetPetSkill(
		int nChaNum,
		int nPetNum,
		int nPetInvenType,
		int nPetInvenMID,
		int nPetInvenSID,
		int nPetInvenCMID,
		int nPetInvenCSID,
		int nPetInvenAvailable) { return 0; }

	/**
	* 팻을 삭제한다.
	* \param nPetNum 삭제할 팻의 번호
	*/
	virtual int	DeletePet(int nChaNum, int nPetNum) { return 0; }
	
	/**
	* 팻의 스킬을 가져온다.
	* \param pPet 펫의 포인터	
	*/
	virtual int GetPetSkill(
				 int nChaNum,
				 GLPET* pPet,
				 DWORD dwPetNum ) { return 0; }
	
	/**
	* 팻의 인벤토리 정보를 가져온다
	*/
	virtual int GetPetInven( 
				 int nChaNum,
				 GLPET* pPet,
				 DWORD dwPetNum ) { return 0; }
	
	/**
	* 팻의 정보를 가져온다.
	*/
	virtual int GetPet(
				 GLPET* pPet,
				 DWORD dwPetNum,
				 DWORD dwClientID) { return 0; }

	/**
	* 팻의 포만감을 가져온다
	* \param dwPetNum 팻 번호
	* \return 팻의 포만감 or DB_ERROR
	*/
	virtual int GetPetFull( int nChaNum, DWORD dwPetNum ) { return 0; }

	/**
	* 소멸된 팻의 리스트를 가져온다.
	* \param nChaNum 소멸된 펫 리스트를 출력하려는 캐릭터 번호
	* \param vecPetReList 소멸된 펫 리스트를 넣을 벡터
	*/
	virtual int GetRestorePetList( int nChaNum, std::vector<PET_RELIST> &vecPetReList ) { return 0; }

	/**
	* 팻을 부활시킨다.
	* \param nPetNum 부활할 팻 번호
	* \param nChaNum 부활을 요구한 캐릭터 번호
	*/
	virtual int RestorePet( DWORD dwPetNum, int nChaNum ) { return 0; }

	/**
	* 팻의 스킨팩을 업데이트 한다.
	* \param nChaNum		팻소유 캐릭터 번호
	* \param dwPetNum		펫 번호
	* \param nPetSkinMID	펫 스킨 MID
	* \param nPetSkinSID	펫 스킨 SID
	* \param nPetSkinScale	펫 스킨 Scale
	* \param nPetSkinTime	펫 스킨 적용시간
	*/
	virtual int UpdatePetSkin( int nChaNum,
							   DWORD dwPetNum,
							   int nPetSkinMID,
							   int nPetSkinSID,
							   int nPetSkinScale,
							   int nPetSkinTime ) { return 0; }

	/**
	* SMS 문자메시지를 발송한다.
	*
	* \param nSendChaNum SMS 를 발송하는 캐릭터 번호	
	* \param nReceiveChaNum SMS 를 받을 캐릭터 번호
	* \param szReceivePhone SMS 를 받을 캐릭터의 전화번호
	* \param szSmsMsg SMS 메시지 내용
	*/
	virtual int LogSMS(
					int nSendChaNum,					
					int nReceiveChaNum,
					const TCHAR* szReceivePhone,
					const TCHAR* szSmsMsg ) { return 0; }

	/**
	* 아이템의 랜덤옵션 로그를 남긴다.
	* \param _sLOG 랜덤옵션 구조체
	*/
	virtual int LogRandomItem( const SLOGRANDOMOPTION &_sLOG ) { return 0; }

	/**
	* 캐릭터의 휴대폰 번호를 저장한다.
	* \param nChaNum 캐릭터 번호
	* \param szPhoneNumber 전화번호 NULL 문자 포함 14자 SMS_RECEIVER
	*/
	virtual int SetChaPhoneNumber(
					int nChaNum,
					const TCHAR* szPhoneNumber ) { return 0; }

	/**
	* 팻의 액션 로그를 남긴다.
	* \param _sLOG 펫액션로그 구조체
	*/
	virtual int LogPetAction( const SLOGPETACTION &_sLOG ) { return 0; }

	/**
	* Terra
	* 유저의 패스워드를 넣거나 체크한다.
	* \param szTID Terra UserID
	* \param szPasswd 사용자 패스워드
	* \return 
	*/
	virtual int TerraUserPassCheck(
					const TCHAR* szTID,
					const TCHAR* szPasswd,
					int	nCheckFlag ) { return 0; }
	
	/**
	* Daum
	* 유저의 패스워드를 넣거나 체크한다.
	* \param szDaumGID Daum UserGID
	* \param szDaumPass 사용자 패스워드
	* \return 
	*/
	virtual int DaumUserPassCheck(const TCHAR* szDaumGID, 
								  const TCHAR* szDaumPasswd,
								  int	nCheckFlag ) { return 0; }


	/**
	* Excite
	* 유저의 패스워드를 넣거나 체크한다.
	* \param szUID Excite UserID
	* \param szPasswd 사용자 패스워드
	* \return 
	*/
	virtual int ExciteUserPassCheck(const TCHAR* szUID,
								  const TCHAR* szUserID,
								  const TCHAR* szPasswd,
								  int	nCheckFlag ) { return 0; }


	/**
	* Thailand
	* 사용자의 PC방 클래스를 체크한다.	
	* \param szUsrIP 사용자의 IP Address
	* \return DB 인증후 결과값
	*/
	virtual int	ThaiUserClassCheck( const TCHAR* szUsrIP ) { return 0; };

	/** 
	* Malaysia
	* 사용자의 PC방 클래스를 체크한다.	
	* \param szUsrIP 사용자의 IP Address
	* \return DB 인증후 결과값
	*/
	virtual int	MyUserClassCheck( const TCHAR* szUsrIP ) { return 0; };

	
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
	virtual int CreateVehicle(
		int nCharNum,
		const TCHAR* szVehicleName,
		int nVehicleType,
		int nVehicleCardMID,
		int nVehicleCardSID
		)	{ return 0; }


	/**
	* 탈것의 배터리를 설정한다. 배터리는 0-1000 사이
	* \param nVehicleBttery 배터리를 설정할 탈것의 번호
	* \param nVehicleBttery 배터리
	*/
	virtual int SetVehicleBattery(
		int nVehicleNum,
		int nCharNum,
		int nVehicleBttery
		) { return 0; }


	/**
	* 탈것의 배터리를 가져온다.
	* \param nVehicleBttery 배터리를 설정할 탈것의 번호
	*/
	virtual  int GetVehicleBattery(
		int nVehicleNum,
		int nCharNum ) { return 0; }


	/**
	* 탈것을 삭제한다.
	* \param nVehicle 삭제할 탈것의 번호
	*/
	virtual int DeleteVehicle(
		int nVehicle, 
		int nCharNum) { return 0; }

	/**
	* 탈것의 정보를 가져온다.
	*/
	virtual int GetVehicle(
		GLVEHICLE* pVehicle,
		int nVehicleNum,
		int nCharNum) { return 0; }

	/**
	* 탈것의 인벤토리 정보를 가져온다
	*/
	virtual int GetVehicleInven( 
		GLVEHICLE* pVehicle,
		int nVehicleNum,
		int nCharNum ) { return 0; }

	/**
	*탈것 인벤토리 업데이트
	*/
	virtual int SetVehicleInven(
		int nVehicle,
		int nCharNum,
		CByteStream &ByteStream ) { return 0; }

	/**
	* 탈것의 액션 로그를 남긴다.
	* \param _sLOG 탈것액션로그 구조체
	*/
	virtual int LogVehicleAction( const SLOGVEHICLEACTION &_sLOG ) { return 0; }


	// 출석부 이벤트 
	virtual int LogUserAttend( int nUserNum, std::vector<USER_ATTEND_INFO> &vecUserAttend ) { return 0; }

	virtual int InsertUserAttend( int nUserNum, int nCount, int nAttendReward ) { return 0; }

	virtual int InsertAttendItem( CString strPurKey, CString strUserID, int nItemMID, int nItemSID ) { return 0; }

	//	UserMoney Add
	virtual int UpdateUserMoneyAdd( DWORD dwUserID, LONGLONG lnUserMoney ) { return 0; }
	virtual int UpdateUserLastInfoAdd( DWORD dwUserID, LONGLONG lnUserMoney ) { return 0; }

public:
	virtual void AddJob ( CDbAction *pDbActJob )
	{
		GASSERT ( 0&&"GLDBMan::AddJob()" );
	}

	virtual void AddLogJob ( CDbAction *pDbActJob )
	{
		GASSERT ( 0&&"GLDBMan::AddJob()" );
	}

	virtual void AddUserJob ( CDbAction *pDbActJob )
	{
		GASSERT ( 0&&"GLDBMan::AddJob()" );
	}

	virtual void AddWebJob ( CDbAction *pDbActJob )
	{
		GASSERT ( 0&&"GLDBMan::AddJob()" );
	}
};

#endif // GLDBMAN_H_
