#pragma once
#include <map>
#include "./GLItemMan.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLDBMan.h"
#include "../[Lib]__Engine/Sources/G-Logic/DxMsgServer.h"

enum
{
	EMMONEY_LOG				= 10000,			// �α׸� ���游�� �ݾ׺�ȭ.
	EMCLUBINCOMEMONEY_LOG	= 100000000,	// Ŭ�� ���� �α�
};

// �������� ��ȯŸ��
enum EMITEM_ROUTE
{
	EMITEM_ROUTE_CHAR		= 0, // ĳ���Ͱ� �ŷ�
	EMITEM_ROUTE_GROUND		= 1, // �ٴڿ� ����
	EMITEM_ROUTE_SHOP		= 2, // ������ �Ǹ�,����
	EMITEM_ROUTE_USERINVEN	= 3, // ����� ��Ŀ�� �ֱ�, ����
	EMITEM_ROUTE_DELETE		= 4, // ������ ����
	EMITEM_ROUTE_CLUB		= 5, // Ŭ�� ��Ŀ�� �ֱ�, ����
	EMITEM_ROUTE_CLUBINCOME	= 6, // ����Ŭ�� ���� ����
	EMITEM_ROUTE_NPCREMAKE  = 7, // ���� �����ϰ� NPC ���Լ� ������ ������ġ ����
	EMITEM_ROUTE_PETCARD	= 8, // ��ī���� ��뿩��
	EMITEM_ROUTE_ODDEVEN	= 9, // Ȧ¦����
	EMITEM_ROUTE_VEHICLE	= 10, // ���� ���� �α�
	EMITEM_ROUTE_VNINVEN	= 11, // ��Ʈ�� Ž�� ���� �κ��丮���� ���� ������ ���
	EMITEM_ROUTE_GARBAGE	= 12, // ���������� ������ ������
	EMITEM_ROUTE_NPCCOME	= 13, // NPC ��ȯ���� ���� ������
	EMITEM_ROUTE_ITEMMIX	= 14, // ������ ����
	EMITEM_ROUTE_GATHERING	= 15, // ä��
	EMITEM_ROUTE_SYSTEM		= 16, // �ý������� �Ͼ �α�
};

enum EMVEHICLE_ACTION
{
	EMVEHICLE_ACTION_BATTERY_BEFORE	= 0, // Ż�� ���͸� ��
	EMVEHICLE_ACTION_BATTERY_AFTER	= 1, // Ż�� ���͸� ��
};

// �� �׼� Ÿ��
enum EMPET_ACTION
{
	EMPET_ACTION_FOOD_BEFORE	= 0, // �� ������
	EMPET_ACTION_FOOD_AFTER		= 1, // �� ������
	EMPET_ACTION_RENAME			= 2, // �� �̸�����
	EMPET_ACTION_COLOR			= 3, // �� �÷�����
	EMPET_ACTION_STYLE			= 4, // �� ��Ÿ�Ϻ���
	EMPET_ACTION_SKILL			= 5, // �� ��ų
	EMPET_ACTION_REVIVE			= 6, // �� ��Ȱī��
	EMPET_ACTION_PETSKINPACK	= 7  // �� ��Ų ��
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
	//	�߻� �������� ����. ( �߻� ���� ������ �ִ��� ���°�. )
	bool DoCheckItemGen ( const SNATIVEID &sNID, EMITEMGEN emTYPE=EMGEN_DEFAULT );

	//	�߻� ���� ���. ( 1�� ����. )
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

