#pragma once
#include "d3dapp.h"
#include <string>

#define STRING_NUM_128	128
#define	QUICK_SLOT_NUM	6
#define	QUICK_SKILL_NUM	10
#define	QUICK_MENU_NUM	17
#define	QUICK_ALL_NUM	(QUICK_SLOT_NUM + QUICK_SKILL_NUM + QUICK_MENU_NUM)
#define CHAT_MACRO_NUM	10
#define BASICMENU_NUM	12

enum EMSERVICE_TYPE
{
	EMSERVICE_DEFAULT		= 0,
	EMSERVICE_KOREA			= 1,
	EMSERVICE_FEYA			= 2,
	EMSERVICE_MALAYSIA_CN	= 3,
	EMSERVICE_MALAYSIA_EN	= 4,
	EMSERVICE_JAPAN			= 5,
	EMSERVICE_THAILAND		= 6,
	EMSERVICE_CHINA			= 7,
	EMSERVICE_INDONESIA		= 8,
	EMSERVICE_PHILIPPINES	= 9,
	EMSERVICE_VIETNAM		= 10,
	EMSERVICE_GLOBAL		= 11,	//	�۷ι� ����
};

// SHOTCUT KEY
enum
{
	SHOTCUT_INVEN = 0,
	SHOTCUT_CHAR,
	SHOTCUT_SKILL,
	SHOTCUT_PARTY,
	SHOTCUT_QUEST,
	SHOTCUT_CLUB,
	SHOTCUT_FRIEND,
	SHOTCUT_MAP,
	SHOTCUT_ITEMBANK,
	SHOTCUT_RUN,
	SHOTCUT_HELP,
	SHOTCUT_PET,
	SHOTCUT_ATTACKMODE,
	SHOTCUT_PKMODE,
	SHOTCUT_ITEMSHOP,
	SHOTCUT_CHATMACRO,
	SHOTCUT_SUMMON
};

namespace RANPARAM
{
	enum
	{ 
		MAX_FTP = 24, 
		EMGAMEOPT_VER = 0x0111, 
		MAX_HTTP = 24,
		MAX_SERVER = 20,
		MAX_CHINA_REGION = 8
	};

	struct SERVER_SET
	{
		CString	strName;
		CString	LoginAddress;
		CString	HttpAddressTable[MAX_HTTP];
		CString	strNewsURL;
		CString	ServerName[MAX_SERVER];
		CString strItemShop;
	};

	extern EMSERVICE_TYPE	emSERVICE_TYPE;
	extern BOOL				bXML_USE;

	//	���� ����.
	extern BOOL				bSAVE_USERID;

	extern BOOL				bDIS_CONFT;			//	'�ڵ�' ��� �ź�.
	extern BOOL				bDIS_TRADE;			//	'�ڵ�' �ŷ� �ź�.
	extern BOOL				bDIS_PARTY;			//	'�ڵ�' ��Ƽ �ź�.
	extern BOOL				bDIS_FRIEND;
	extern BOOL				bSHOW_SIMPLEHP;
	extern BOOL				b3D_SOUND;			//	3���� ����
	extern BOOL				bSHOW_TIP;			//	�� �����ֱ�
	extern BOOL				bMOVABLE_ON_CHAT;	//	ä��â ������ �����̱�
	extern BOOL				bFORCED_ATTACK;		//	���� ����
	extern BOOL				bNAME_DISPLAY;		//	�̸� �׻� ǥ��
	extern BOOL				bNON_Rebirth;		//	��Ȱ ��ų ����

	extern BOOL				bCONFT_BOUND;		//	��ý� ���� ���� ����.
	extern WORD				wCONFT_RECOVER;		//	��ý� ȸ���� ����.
	extern float			fCONFT_HP_RATE;		//	��ý� ü�� Ȯ�� ����.
	extern float			fCONFT_TAR_HP_RATE;	//	��ý� ü�� Ȯ�� ���� [ ����(���δ�ýø�) ].

	extern DWORD			dwPARTY_GET_ITEM;	//	��Ƽ�ɼ� ������ �й� ��Ģ.
	extern DWORD			dwPARTY_GET_MONEY;	//	��Ƽ�ɼ� �� �й� ��Ģ.
	
	extern int				QuickSlot[QUICK_SLOT_NUM]; // ������
	extern int				SkillSlot[QUICK_SKILL_NUM]; // ����ų
	extern int				MenuShotcut[QUICK_MENU_NUM]; // �޴�����Ű
	extern int				DefaultShotcutKey[QUICK_ALL_NUM]; // ����Ʈ Ű Ÿ��
	extern int				KeySettingToRanparam[QUICK_MENU_NUM];
	extern int				BasicMenuToRanparam[BASICMENU_NUM];

	extern CString			ChatMacro[CHAT_MACRO_NUM];

	//	���÷��� ����.
	extern BOOL				bCHECKWHQL;
	extern DWORD			dwScrWidth;
	extern DWORD			dwScrHeight;
	extern EMSCREEN_FORMAT	emScrFormat;
	extern UINT				uScrRefreshHz;
	extern BOOL				bScrWindowed;
	extern BOOL				bScrWndHalfSize;
	extern BOOL				bScrWndFullSize;
	extern BOOL				bGameCursor;

	extern bool				bDISP_CONSOLE;
	extern bool				bDISP_FPS;

	//	�׷��� ����.
	extern DWORD			dwVideoLevel;
	extern DWORD			dwShadowChar;	//	0~4
	extern BOOL				bBuff;
	extern BOOL				bShadowLand;
	extern BOOL				bRealReflect;
	extern BOOL				bRefract;
	extern BOOL				bGlow;
	extern BOOL				bPost;
	extern BOOL				bFrameLimit;
	extern DWORD			dwSkinDetail;	// 0~2

	extern BOOL				bBUFF_SFX;
	extern BOOL				bEMOTICON;

	//	�þ�
	extern DWORD			dwFogRange;

	extern LONG				nSndSfx;
	extern LONG				nSndMap;
	extern LONG				nSndMusic;
	extern DWORD			dw3DAlgorithm;
	extern DWORD			dwSndBufferSize;

	extern BOOL				bSndMuteSfx;
	extern BOOL				bSndMuteMap;
	extern BOOL				bSndMuteMusic;	

	//	���� ����.
	extern TCHAR			LoginAddress[STRING_NUM_128];
	extern TCHAR			FtpAddressTable[MAX_FTP][STRING_NUM_128];
	extern TCHAR			HttpAddressTable[MAX_HTTP][STRING_NUM_128];	
	extern CString			strNewsURL;
	extern CString			strConnectionURL;
	extern BOOL				bUsePassiveDN;
	extern UINT				nChinaRegion; // �߱� - ��ġ���� ����(MAX_CHINA_REGION:�����, 0~3:��������)

	// �߱� ������ ���� ���� ����
	extern SERVER_SET		China_Region[MAX_CHINA_REGION];

	//	��� ��Ʈ ����.
	extern DWORD			dwLangSet;
	extern CString			strGDIFont;
	extern CString			strFontType;

	//	�⺻ ���� ����. ( �� txt )
	extern CString			strCountry;
	extern CString			strLauncherText;
	extern CString			strGameInText;
	extern CString			strGameExText;
	extern CString			strGameWord;
	extern CString			strServerText;

	extern CString			strUIOuterCfg;
	extern CString			strUIInnerCfg01;
	extern CString			strUIInnerCfg02;
	extern CString			strUIInnerCfg03;
	extern CString			strUIExtCfg;

	// ���� �� ������ �ּ� ( ���� )
	extern TCHAR			HelpAddress[STRING_NUM_128];
	extern TCHAR			ItemShopAddress[STRING_NUM_128];
	extern TCHAR			RPFDataPath[STRING_NUM_128];
	// Modified to use the title in param.ini - YeXiuPH
	extern TCHAR			WindowTitle[STRING_NUM_128];

	void SETIDPARAM ( std::string strPARAM );
	// geger009 | check setidparam and getidparam maybe not needed
	void SETIDPARAM2 ( std::string strPARAM );
	void SETIDPARAM3 ( std::string strPARAM );
	void SETJAPANUUID( std::string strUUID );

	const TCHAR* GETIDPARAM ();
	// geger009 | check setidparam and getidparam maybe not needed
	const TCHAR* GETIDPARAM2 ();
	const TCHAR* GETIDPARAM3 ();
	const TCHAR* GETJAPANUUID();

	bool VALIDIDPARAM ();

	BOOL SETUSERID ( const TCHAR* szUSERID );
	BOOL SETCHARID ( const TCHAR* szCHARID );
	std::string GETCHARID();
	std::string GETUSERID_ENC ();
	std::string GETUSERID_DEC ();

	BOOL LOAD ( const TCHAR *szRootPath, bool bServer = false );
	BOOL LOAD_CFG ( const TCHAR *szRootPath );
	
	BOOL LOAD_PARAM( const TCHAR *szRootPath );	
	BOOL LOAD_OPT( const TCHAR *szRootPath, bool bServer = false );	

	BOOL SAVE ();

    void INIT_DATA();
	BOOL LOAD_GAMEOPT ();
	BOOL SAVE_GAMEOPT ();

};
