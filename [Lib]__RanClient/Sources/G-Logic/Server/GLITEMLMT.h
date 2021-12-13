#pragma once
#include <map>
#include "./GLItemMan.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLDBMan.h"
#include "../[Lib]__Engine/Sources/G-Logic/DxMsgServer.h"

enum
{
	EMMONEY_LOG				= 10000,			// 로그를 남길만한 금액변화.
	EMCLUBINCOMEMONEY_LOG	= 100000000,	// 클럽 수입 로그
};

// 아이템의 교환타입
enum EMITEM_ROUTE
{
	EMITEM_ROUTE_CHAR		= 0, // 캐릭터간 거래
	EMITEM_ROUTE_GROUND		= 1, // 바닥에 떨굼
	EMITEM_ROUTE_SHOP		= 2, // 상점에 판매,구입
	EMITEM_ROUTE_USERINVEN	= 3, // 사용자 락커에 넣기, 빼기
	EMITEM_ROUTE_DELETE		= 4, // 아이템 삭제
	EMITEM_ROUTE_CLUB		= 5, // 클럽 락커에 넣기, 빼기
	EMITEM_ROUTE_CLUBINCOME	= 6, // 선도클럽 세금 수입
	EMITEM_ROUTE_NPCREMAKE  = 7, // 돈을 지불하고 NPC 에게서 아이템 랜덤수치 개조
	EMITEM_ROUTE_PETCARD	= 8, // 팻카드의 사용여부
	EMITEM_ROUTE_ODDEVEN	= 9, // 홀짝게임
	EMITEM_ROUTE_VEHICLE	= 10, // 보드 생성 로그
	EMITEM_ROUTE_VNINVEN	= 11, // 베트남 탐닉 방지 인벤토리에서 돈을 꺼냈을 경우
	EMITEM_ROUTE_GARBAGE	= 12, // 휴지통으로 아이템 삭제시
	EMITEM_ROUTE_NPCCOME	= 13, // NPC 소환으로 생긴 수수료
	EMITEM_ROUTE_ITEMMIX	= 14, // 아이템 조합
	EMITEM_ROUTE_GATHERING	= 15, // 채집
	EMITEM_ROUTE_SYSTEM		= 16, // 시스템으로 일어난 로그
};

enum EMVEHICLE_ACTION
{
	EMVEHICLE_ACTION_BATTERY_BEFORE	= 0, // 탈것 베터리 전
	EMVEHICLE_ACTION_BATTERY_AFTER	= 1, // 탈것 베터리 후
};

// 펫 액션 타입
enum EMPET_ACTION
{
	EMPET_ACTION_FOOD_BEFORE	= 0, // 펫 먹이전
	EMPET_ACTION_FOOD_AFTER		= 1, // 펫 먹이후
	EMPET_ACTION_RENAME			= 2, // 펫 이름변경
	EMPET_ACTION_COLOR			= 3, // 펫 컬러변경
	EMPET_ACTION_STYLE			= 4, // 펫 스타일변경
	EMPET_ACTION_SKILL			= 5, // 펫 스킬
	EMPET_ACTION_REVIVE			= 6, // 펫 부활카드
	EMPET_ACTION_PETSKINPACK	= 7  // 펫 스킨 팩
};

struct SITEMLMT
{
	enum { VERSION = 0x001, };

	LONGLONG lnDEFAULT;
	LONGLONG lnINIT;
	LONGLONG lnSHOP;
	LONGLONG lnNPC;
	LONGLONG lnQUEST;
	LONGLONG lnGM;
	LONGLONG lnMOB;
	LONGLONG lnOLD;
	LONGLONG lnBILLING;
	LONGLONG lnGather;
	LONGLONG lnSystem;

	SITEMLMT () :
		lnDEFAULT(0),
		lnINIT(0),
		lnSHOP(0),
		lnNPC(0),
		lnQUEST(0),
		lnMOB(0),
		lnGM(0),
		lnOLD(0),
		lnBILLING(0),
		lnGather(0),
		lnSystem(0)
	{
	}

	LONGLONG GETTOTAL () const
	{
		return lnDEFAULT+lnINIT+lnSHOP+lnNPC+lnQUEST+lnGM+lnMOB+lnOLD+lnBILLING+lnGather+lnSystem;
	}
};

class GLITEMLMT
{
public:
	enum { VERSION = 0x0001, };
	typedef std::map<DWORD,SITEMLMT>			GLGENITEM;
	typedef GLGENITEM::iterator					GLGENITEM_ITER;

protected:
	std::string		m_strPATH;
	GLDBMan*		m_pDBMan;
	DxMsgServer*	m_pMsgServer;
	
	int				m_nSGNum;
	int				m_nSvrNum;
	DWORD			m_dwFieldID;

public:
	void SetPath ( std::string strPATH )										{ m_strPATH = strPATH; }
	void SetDBMan ( GLDBMan* pDBMan )											{ m_pDBMan = pDBMan; }
	void SetServer ( DxMsgServer* pMsgServer, DWORD dwFieldID=UINT_MAX )		{ m_pMsgServer = pMsgServer; m_dwFieldID = dwFieldID; }

	void ReadMaxKey ();

protected:
	GLGENITEM	m_mapGENITEM;

protected:
	void SetItemGenNum ( const SNATIVEID &sNID, EMITEMGEN emTYPE, LONGLONG lnMax );
	LONGLONG GetItemGenNum ( const SNATIVEID &sNID, EMITEMGEN emTYPE );

public:
	//	발생 가능한지 점검. ( 발생 여유 갯수가 있는지 보는것. )
	bool DoCheckItemGen ( const SNATIVEID &sNID, EMITEMGEN emTYPE=EMGEN_DEFAULT );

	//	발생 갯수 등록. ( 1개 단위. )
	LONGLONG RegItemGen ( const SNATIVEID &sNID, EMITEMGEN emTYPE=EMGEN_DEFAULT );

	void RegPickUpInfo ( DWORD dwGaeaID, const SNATIVEID &sNID );

public:
	void ReqItemRoute ( const SITEMCUSTOM &sITEM, EMIDTYPE emFROME, DWORD dwFROMID, EMIDTYPE emTO, DWORD dwTOID, EMITEM_ROUTE emROUTE, int nNum );
	void ReqMoneyExc ( EMIDTYPE emFROM, DWORD dwFrom, EMIDTYPE emTO, DWORD dwTo, LONGLONG lnPrice, DWORD nFlag );
	void ReqItemConversion ( const SITEMCUSTOM &sITEM, EMIDTYPE emFROM, DWORD dwFrom );
	void ReqRandomItem ( const SITEMCUSTOM &sITEM );
	
	void ReqAction ( DWORD dwCI, EMLOGACTION emACT, EMIDTYPE emTAR, DWORD dwTAR, __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY );
	void ReqPetAction ( int nPetNum, SNATIVEID nItemID, EMPET_ACTION emAction, int nPetFull);
	void ReqVehicleAction ( int nVehicleNum, SNATIVEID nItemID, EMVEHICLE_ACTION emAction, int nVehicleFull);

private:
	GLITEMLMT(void);

public:
	~GLITEMLMT(void);

public:
	static GLITEMLMT& GetInstance();
};

