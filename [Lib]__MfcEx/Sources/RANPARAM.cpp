#include "pch.h"
#include "./RANPARAM.h"

#include "../[Lib]__Engine/Sources/Common/Rijndael.h"
#include "../[Lib]__Engine/Sources/Common/StringFile.h"
#include "../[Lib]__Engine/Sources/Common/STRINGUTILS.h"
#include "../[Lib]__Engine/Sources/Common/compbyte.h"
#include "../[Lib]__Engine/Sources/Common/SUBPATH.h"
#include "../[Lib]__Engine/Sources/Common/StlFunctions.h"
#include "../[Lib]__Engine/Sources/Common/gltexfile.h"
#include "../[Lib]__Engine/Sources/Common/IniLoader.h"
#include "../[Lib]__Engine/Sources/DxTools/DebugSet.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace RANPARAM
{
	std::string			strRootPath			= "";
	std::string			strParamFile		= "";
	std::string			strOptionFile		= "";

	EMSERVICE_TYPE		emSERVICE_TYPE		= EMSERVICE_DEFAULT;
	BOOL				bXML_USE			= TRUE;

	BOOL				bSAVE_USERID		= FALSE;
	std::string			strENC_USERID		= "";
	std::string			strENC_CHARID		= "";

	const std::string	strIDNULL			= "(null)";

	BOOL				bDIS_CONFT			= FALSE;		//	'자동' 대련 거부.
	BOOL				bDIS_TRADE			= FALSE;		//	'자동' 거래 거부.
	BOOL				bDIS_PARTY			= FALSE;		//	'자동' 파티 거부.
	BOOL				bDIS_FRIEND			= FALSE;		//	'자동' 친구 요청 거부.
	BOOL				bSHOW_SIMPLEHP		= TRUE;			//	미니 HP 표시
	BOOL				b3D_SOUND			= FALSE;
	BOOL				bSHOW_TIP			= TRUE;			//	팁 보여주기
	BOOL				bMOVABLE_ON_CHAT	= FALSE;		//	채팅창 위에서 움직이기
	BOOL				bFORCED_ATTACK		= TRUE;			//	강제 공격
	BOOL				bNAME_DISPLAY		= TRUE;			//	이름 항상 표시
	BOOL				bNON_Rebirth		= FALSE;		//  부활 스킬 금지

	BOOL				bCONFT_BOUND		= TRUE;			//	대련시 영역 제한 여부.
	WORD				wCONFT_RECOVER		= 0;			//	대련시 회복약 제한.
	float				fCONFT_HP_RATE		= 1.0f;			//	대련시 체력 확대 비율.
	float				fCONFT_TAR_HP_RATE	= 1.0f;			//	대련시 체력 확대 비율 [ 상대방(개인대련시만) ].

	DWORD				dwPARTY_GET_ITEM	= 3;			//	파티옵션 아이템 분배 규칙.(디폴트는 순차습득)
	DWORD				dwPARTY_GET_MONEY	= 3;			//	파티옵션 돈 분배 규칙.(디폴트는 균등분배)
	
	int					QuickSlot[QUICK_SLOT_NUM]		= { DIK_Q, DIK_W, DIK_E, DIK_A, DIK_S, DIK_D };
	
	int					SkillSlot[QUICK_SKILL_NUM]		= { DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6,
															DIK_7, DIK_8, DIK_9, DIK_0};
	
	int					MenuShotcut[QUICK_MENU_NUM]		= { DIK_I, DIK_C, DIK_K, DIK_P, DIK_T, DIK_G, 
															DIK_F, DIK_M, DIK_B, DIK_R, DIK_H, DIK_L,
															DIK_X, DIK_Z ,DIK_U, DIK_J, DIK_O};
	
	int					DefaultShotcutKey[QUICK_ALL_NUM]	= { DIK_Q, DIK_W, DIK_E, DIK_A, DIK_S, DIK_D,
																DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6,
																DIK_7, DIK_8, DIK_9, DIK_0,	DIK_I, DIK_C,
																DIK_K, DIK_P, DIK_T, DIK_G, DIK_F, DIK_M, 
																DIK_B, DIK_R, DIK_H, DIK_L, DIK_X, DIK_Z,
																DIK_U, DIK_J, DIK_O };

	// KeySettingWindow 참조 테이블 단축키 순서가 다르게 설정되어 있어서 필요
	int					KeySettingToRanparam[QUICK_MENU_NUM] = { SHOTCUT_INVEN, SHOTCUT_CHAR, SHOTCUT_SKILL, SHOTCUT_PARTY,
																 SHOTCUT_QUEST, SHOTCUT_CLUB, SHOTCUT_FRIEND, SHOTCUT_MAP,
																 SHOTCUT_CHATMACRO, SHOTCUT_ITEMBANK, SHOTCUT_ITEMSHOP, SHOTCUT_RUN,
																 SHOTCUT_HELP, SHOTCUT_PET, SHOTCUT_ATTACKMODE, SHOTCUT_PKMODE, SHOTCUT_SUMMON};

	// BasicMenu 참조 테이블 단축키 순서가 다르게 설정되어 있어서 필요
	int					BasicMenuToRanparam[BASICMENU_NUM] = { SHOTCUT_INVEN, SHOTCUT_CHAR,	SHOTCUT_SKILL, SHOTCUT_PARTY,
															   SHOTCUT_QUEST, SHOTCUT_CLUB, SHOTCUT_FRIEND, SHOTCUT_MAP,
															   SHOTCUT_ITEMBANK, SHOTCUT_RUN, SHOTCUT_ITEMSHOP, SHOTCUT_CHATMACRO};
	
	CString				ChatMacro[CHAT_MACRO_NUM];

	BOOL				bCHECKWHQL			= TRUE;
	DWORD				dwScrWidth			= 800;
	DWORD				dwScrHeight			= 600;
	EMSCREEN_FORMAT		emScrFormat			= EMSCREEN_F16;
	UINT				uScrRefreshHz		= 60;
	BOOL				bScrWindowed		= FALSE;
	BOOL				bScrWndHalfSize		= TRUE;
	BOOL				bScrWndFullSize		= FALSE;

	bool				bDISP_CONSOLE		= false;
	bool				bDISP_FPS			= false;

	BOOL				bGameCursor			= TRUE;

	LONG				nSndSfx				= 0;
	LONG				nSndMap				= 0;
	LONG				nSndMusic			= 0;
	DWORD				dw3DAlgorithm		= 0;
	DWORD				dwSndBufferSize		= 10;

	BOOL				bSndMuteSfx			= FALSE;
	BOOL				bSndMuteMap			= FALSE;
	BOOL				bSndMuteMusic		= FALSE;
	BOOL				bUsePassiveDN		= FALSE;


	DWORD				dwVideoLevel		= 0;
	DWORD				dwShadowChar		= 4;
	BOOL				bBuff				= TRUE;
	BOOL				bShadowLand			= TRUE;
	BOOL				bRealReflect		= TRUE;
	BOOL				bRefract			= TRUE;
	BOOL				bGlow				= TRUE;
	BOOL				bPost				= TRUE;
	BOOL				bFrameLimit			= TRUE;
	DWORD				dwSkinDetail		= 2;

	BOOL				bBUFF_SFX			= TRUE;
	BOOL				bEMOTICON			= TRUE;

	DWORD				dwFogRange			= 0;

	TCHAR				LoginAddress[128]	= "211.172.252.50";

	TCHAR				FtpAddressTable[MAX_FTP][128] =
											{
												"", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "",
											};
	TCHAR				HttpAddressTable[MAX_HTTP][128] =
											{
												"patch.ran-online.co.kr", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "",
											};

	CString				strNewsURL			= "ran-online.co.kr/news/patch_list/list2.aspx";
	CString				strConnectionURL	= "2046.5u56.com";

	UINT				nChinaRegion		= MAX_CHINA_REGION;

	SERVER_SET			China_Region[MAX_CHINA_REGION];
	
	DWORD				dwLangSet			= 0;
	CString				strGDIFont			= "돋움체";
	CString				strFontType			= "D3DFONT";
	
	CString				strCountry			= "kr";
	CString				strLauncherText		= "Launcher.xml";
	CString				strGameInText		= "gameintext.xml";
	CString				strGameExText		= "gameextext.xml";
	CString				strGameWord			= "gameword.xml";
	CString				strServerText		= "servertext.xml";

	CString				strUIOuterCfg		= "uioutercfg.xml";
	CString				strUIInnerCfg01		= "uiinnercfg01.xml";
	CString				strUIInnerCfg02		= "uiinnercfg02.xml";
	CString				strUIInnerCfg03		= "uiinnercfg03.xml";
	CString				strUIExtCfg			= "uiextcfg.xml";	

	TCHAR				HelpAddress[STRING_NUM_128] = _T("\\data\\help\\help.htm");
	// 중국 : http://2046.5u56.com/gameshop/gameshop.html
	TCHAR				ItemShopAddress[STRING_NUM_128] = _T("about:blank");
	TCHAR				RPFDataPath[STRING_NUM_128] = _T("\\package\\data.rpf");
	// Modified to use the title in param.ini - YeXiuPH
	TCHAR				WindowTitle[STRING_NUM_128] = _T("Ran Client");

	BOOL LOAD_CFG ( const TCHAR *szRootPath )
	{
		strRootPath = szRootPath;
		strParamFile = strRootPath + "\\param.cfg";

		gltexfile cFILE;
		cFILE.reg_sep ( '\t' );
		cFILE.reg_sep ( ' ' );

		if ( !cFILE.open(strParamFile.c_str(),false) )	return FALSE;

		int nTEMP(0);

		#if !defined(KR_PARAM) && !defined(RZ_PARAM)
		{
			//#ifndef TW_PARAM
			//	cFILE.getflag ( "bSAVE_USERID", 1, 1, bSAVE_USERID );
			//	if ( bSAVE_USERID )
			//#endif
				{
					cFILE.getflag ( "strENC_USERID", 1, 1, strENC_USERID );
					if ( strIDNULL == strENC_USERID )	strENC_USERID = "";
				}
		}
		#endif

		cFILE.getflag ( "bDISP_CONSOLE", 1, 1, bDISP_CONSOLE );
		cFILE.getflag ( "bDISP_FPS", 1, 1, bDISP_FPS );

		cFILE.getflag ( "bCHECKWHQL", 1, 1, bCHECKWHQL );
		cFILE.getflag ( "dwScrWidth", 1, 1, dwScrWidth );
		cFILE.getflag ( "dwScrHeight", 1, 1, dwScrHeight );
		cFILE.getflag ( "uScrRefreshHz", 1, 1, uScrRefreshHz );
		cFILE.getflag ( "emScrFormat", 1, 1, nTEMP );
		emScrFormat = (EMSCREEN_FORMAT) nTEMP;

		cFILE.getflag ( "bScrWindowed", 1, 1, bScrWindowed );
		cFILE.getflag ( "bScrWndFullSize", 1, 1, bScrWndFullSize );
		cFILE.getflag ( "bGameCursor", 1, 1, bGameCursor );

		cFILE.getflag ( "b3D_SOUND", 1, 1, b3D_SOUND );
		cFILE.getflag ( "nSndSfx", 1, 1, nSndSfx );
		cFILE.getflag ( "nSndMap", 1, 1, nSndMap );
		cFILE.getflag ( "nSndMusic", 1, 1, nSndMusic );
		cFILE.getflag ( "dw3DAlgorithm", 1, 1, dw3DAlgorithm );
		cFILE.getflag ( "dwSndBufferSize", 1, 1, dwSndBufferSize );

		cFILE.getflag ( "bSndMuteSfx", 1, 1, bSndMuteSfx );
		cFILE.getflag ( "bSndMuteMap", 1, 1, bSndMuteMap );
		cFILE.getflag ( "bSndMuteMusic", 1, 1, bSndMuteMusic );
		cFILE.getflag ( "bUsePassiveDN", 1, 1, bUsePassiveDN );		

		cFILE.getflag ( "dwShadowChar", 1, 1, dwShadowChar );
		cFILE.getflag ( "bBuff", 1, 1, bBuff );
		cFILE.getflag ( "bShadowLand", 1, 1, bShadowLand );
		cFILE.getflag ( "bRealReflect", 1, 1, bRealReflect );
		cFILE.getflag ( "bRefract", 1, 1, bRefract );
		cFILE.getflag ( "bGlow", 1, 1, bGlow );
		cFILE.getflag ( "bPost", 1, 1, bPost );
		cFILE.getflag ( "dwSkinDetail", 1, 1, dwSkinDetail );
		cFILE.getflag ( "dwFogRange", 1, 1, dwFogRange );
		cFILE.getflag ( "bFrameLimit", 1, 1, bFrameLimit );

		cFILE.getflag ( "bBUFF_SFX", 1, 1, bBUFF_SFX );
		cFILE.getflag ( "bEMOTICON", 1, 1, bEMOTICON );

		cFILE.getflag_sz( "LoginAddress", 1, 1, LoginAddress, STRING_NUM_128 );

		for ( int i=0; i<MAX_FTP; ++i )
		{
			CString strFTPADD;
			strFTPADD.Format ( "FtpAddress%02d", i );



			memset ( FtpAddressTable[i], 0, sizeof(char)*128 );

			gltexfile::VECSTRING* pVECSTR = cFILE.findflag ( strFTPADD.GetString() );
			if ( pVECSTR )
			{
				cFILE.getflag_sz( strFTPADD.GetString(), 1, 1, FtpAddressTable[i], STRING_NUM_128 );
			}
		}

		for ( int i=0; i<MAX_HTTP; ++i )
		{
			CString strHTTPADD;
			strHTTPADD.Format ( "HttpAddress%02d", i );

			memset ( HttpAddressTable[i], 0, sizeof(char)*128 );

			gltexfile::VECSTRING* pVECSTR = cFILE.findflag ( strHTTPADD.GetString() );
			if ( pVECSTR )
			{
				cFILE.getflag_sz( strHTTPADD.GetString(), 1, 1, HttpAddressTable[i], STRING_NUM_128 );
			}
		}

		cFILE.getflag ( "strNewsURL", 1, 1, strNewsURL );
		cFILE.getflag ( "strConnectionURL", 1, 1, strConnectionURL );
		cFILE.getflag ( "nChinaRegion", 1, 1, nChinaRegion );

		cFILE.getflag ( "strFontType", 1, 1, strFontType );
		cFILE.getflag ( "dwLangSet", 1, 1, dwLangSet );
		cFILE.getflag ( "strGDIFont", 1, 1, strGDIFont );

		cFILE.getflag ( "strCountry", 1, 1, strCountry );
		cFILE.getflag ( "strLauncherText", 1, 1, strLauncherText );
		cFILE.getflag ( "strGameInText", 1, 1, strGameInText );
		cFILE.getflag ( "strGameExText", 1, 1, strGameExText );
		cFILE.getflag ( "strGameWord", 1, 1, strGameWord );
		cFILE.getflag ( "strServerText", 1, 1, strServerText );

		cFILE.getflag ( "strUIOuterCfg", 1, 1, strUIOuterCfg );
		cFILE.getflag ( "strUIInnerCfg01", 1, 1, strUIInnerCfg01 );
		cFILE.getflag ( "strUIInnerCfg02", 1, 1, strUIInnerCfg02 );
		cFILE.getflag ( "strUIInnerCfg03", 1, 1, strUIInnerCfg03 );
		cFILE.getflag ( "strUIExtCfg", 1, 1, strUIExtCfg );

		//	Note : 유효성 체크.
		//
		if ( COMMENT::FONTSYS[0]!=strFontType.GetString() && COMMENT::FONTSYS[1]!=strFontType.GetString() )
		{
			strFontType = COMMENT::FONTSYS[0].c_str();
		}

		if ( !bScrWndHalfSize )		bScrWndFullSize = TRUE;

		return TRUE;
	}
	
	BOOL LOAD ( const TCHAR *szRootPath, bool bServer )
	{

		strRootPath = szRootPath;

		if ( !LOAD_PARAM ( szRootPath ) )
		{
			return LOAD_CFG( szRootPath );
		}

		if ( !LOAD_OPT( szRootPath, bServer ) )
		{
			return FALSE;
		}

        //	중국일 경우 지역 선택시 값을 갱신해준다.
#ifdef CH_PARAM
		
		if ( nChinaRegion == MAX_CHINA_REGION )	return TRUE;
                                   
		StringCchCopy( LoginAddress, STRING_NUM_128, China_Region[nChinaRegion].LoginAddress.GetString() );

		for( int i=0 ; ; ++i )
		{
			if( China_Region[nChinaRegionW].HttpAddressTable[i].IsEmpty() ) 
				break;

			StringCchCopy( HttpAddressTable[i], STRING_NUM_128, China_Region[nChinaRegion].HttpAddressTable[i].GetString() );
		}

		strNewsURL = China_Region[nChinaRegion].strNewsURL;
#endif

		return TRUE;
	}

	BOOL SAVE ()
	{
		if ( strOptionFile.empty() )	return FALSE;

		CString strBuffer;

		strBuffer += _T("[GAME OPTION]\n");

		#if !defined(KR_PARAM) && !defined(RZ_PARAM)
		{
			if ( strENC_USERID.empty() )
			{
				strBuffer.AppendFormat( "bSAVE_USERID = %d\n", FALSE );
				strBuffer.AppendFormat( "strENC_USERID = %s\n", strIDNULL.c_str() );
			}
			else
			{
				strBuffer.AppendFormat( "bSAVE_USERID = %d\n", TRUE );
				strBuffer.AppendFormat( "strENC_USERID = %s\n", strENC_USERID.c_str());
			}

		}
		#endif

		strBuffer.AppendFormat( _T("nChinaRegion = %d\n"), nChinaRegion );

		{
			strBuffer += _T(";\n");
			strBuffer += _T("[SCREEN OPTION]\n");
			strBuffer.AppendFormat( "bCHECKWHQL	= %d\n", bCHECKWHQL );
			strBuffer.AppendFormat( "dwScrWidth	= %d\n", dwScrWidth );
			strBuffer.AppendFormat( "dwScrHeight = %d\n", dwScrHeight );
			strBuffer.AppendFormat( "emScrFormat = %d\n", emScrFormat );
			strBuffer.AppendFormat( "uScrRefreshHz = %d\n", uScrRefreshHz );
			strBuffer.AppendFormat( "bScrWindowed = %d\n", bScrWindowed );
			strBuffer.AppendFormat( "bScrWndFullSize = %d\n", bScrWndFullSize );
			strBuffer.AppendFormat( "bGameCursor = %d\n", bGameCursor );
		}

		{
			strBuffer += _T(";\n");
			strBuffer += _T("[SOUND OPTION]\n");
			strBuffer.AppendFormat( "b3D_SOUND = %d\n", b3D_SOUND );
			strBuffer.AppendFormat( "nSndSfx = %d\n", nSndSfx );
			strBuffer.AppendFormat( "nSndMap = %d\n", nSndMap );
			strBuffer.AppendFormat( "nSndMusic = %d\n", nSndMusic );
			strBuffer.AppendFormat( "dw3DAlgorithm = %d\n", dw3DAlgorithm );
			strBuffer.AppendFormat( "dwSndBufferSize = %d\n", dwSndBufferSize );
			strBuffer.AppendFormat( "bSndMuteSfx = %d\n", bSndMuteSfx );
			strBuffer.AppendFormat( "bSndMuteMap = %d\n", bSndMuteMap );
			strBuffer.AppendFormat( "bSndMuteMusic = %d\n", bSndMuteMusic );
		}

		{
			strBuffer += _T(";\n");
			strBuffer += _T("[GRAPHIC OPTION]\n");
			strBuffer.AppendFormat( "dwVideoLevel = %d\n", dwVideoLevel );
			strBuffer.AppendFormat( "dwShadowChar = %d\n", dwShadowChar );
			strBuffer.AppendFormat( "bBuff = %d\n", bBuff );
			strBuffer.AppendFormat( "bShadowLand = %d\n", bShadowLand );
			strBuffer.AppendFormat( "bRealReflect = %d\n", bRealReflect );
			strBuffer.AppendFormat( "bRefract = %d\n", bRefract );
			strBuffer.AppendFormat( "bGlow = %d\n", bGlow );
			strBuffer.AppendFormat( "bPost = %d\n", bPost );
			strBuffer.AppendFormat( "bFrameLimit = %d\n", bFrameLimit );
			strBuffer.AppendFormat( "dwSkinDetail = %d\n", dwSkinDetail );
			strBuffer.AppendFormat( "bBUFF_SFX = %d\n", bBUFF_SFX );
			strBuffer.AppendFormat( "bEMOTICON = %d\n", bEMOTICON );
			strBuffer.AppendFormat( "dwFogRange = %d\n", dwFogRange );
			strBuffer.AppendFormat( "strFontType = %s\n", strFontType );
		}

		CRijndael oRijndael;
		oRijndael.Initialize( CRijndael::sm_Version[CRijndael::VERSION-1].c_str(), CRijndael::sm_KeyLength[CRijndael::VERSION-1], CRijndael::VERSION_DATE, CRijndael::VERSION);

		int nLen = oRijndael.GetEncryptStringLength( strBuffer );
		char * szBuffer = new char[nLen];
		memset( szBuffer, 0, nLen );

		oRijndael.EncryptEx( strBuffer.GetString(), szBuffer, strBuffer.GetLength() );
		int nVersion = CRijndael::VERSION;

		if ( nVersion <= 0 && strlen( szBuffer ) <= 0 )	return FALSE;

		CFile file;
		if ( ! file.Open( strOptionFile.c_str(), CFile::modeCreate|CFile::modeWrite ) )	// 파일 열기
		{
			delete [] szBuffer;
			return FALSE;
		}
		
		file.Write( &nVersion, sizeof(int) );							// 버젼 쓰기
		file.Write( szBuffer, nLen );		// 파일 쓰기
		file.Close();

		delete [] szBuffer;

		return TRUE;
	}

	std::string strIDPARAM;
	std::string strIDPARAM2;
	std::string strIDPARAM3;

	std::string strJPUUID;

	void SETIDPARAM( std::string strPARAM )		{ strIDPARAM = strPARAM; }
	// geger009 | check setidparam and getidparam maybe not needed
	void SETIDPARAM2( std::string strPARAM )	{ strIDPARAM2 = strPARAM; }
	void SETIDPARAM3( std::string strPARAM )	{ strIDPARAM3 = strPARAM; }
	void SETJAPANUUID( std::string strUUID )	{ strJPUUID = strUUID; }

	const TCHAR* GETIDPARAM()					{ return strIDPARAM.c_str(); }
	// geger009 | check setidparam and getidparam maybe not needed
	const TCHAR* GETIDPARAM2()					{ return strIDPARAM2.c_str(); }
	const TCHAR* GETIDPARAM3()					{ return strIDPARAM3.c_str(); }
	const TCHAR* GETJAPANUUID()					{ return strJPUUID.c_str(); }

	bool VALIDIDPARAM ()						{ return !strIDPARAM.empty(); }

	enum { emENCKEYLENG = MAX_COMPUTERNAME_LENGTH+32 };
	char szEncryptKEY[emENCKEYLENG] = "unnamed";

	void SETENCRYPT ()
	{
		//DWORD dwNAMELENG = emENCKEYLENG;
		//BOOL bOK = GetComputerName ( szEncryptKEY, &dwNAMELENG );
		//if ( !bOK )
		//{
		//	DWORD dwERR = GetLastError();
		//	CDebugSet::ToLogFile ( "error GetComputerName(), code : %d", dwERR );
		//}

		//StringCchCat ( szEncryptKEY, emENCKEYLENG, "XWeRtSOn32QwRtQs" );
		StringCchCopy ( szEncryptKEY, emENCKEYLENG, "XWeRtSOn32QwRtQs" );
	}

	void INIT_DATA()
	{
		bDIS_CONFT			= FALSE;		//	'자동' 대련 거부.
		bDIS_TRADE			= FALSE;		//	'자동' 거래 거부.
		bDIS_PARTY			= FALSE;		//	'자동' 파티 거부.
		bDIS_FRIEND			= FALSE;		//	'자동' 친구 요청 거부.
		bSHOW_SIMPLEHP		= TRUE;
		b3D_SOUND			= FALSE;
		bSHOW_TIP			= TRUE;			//	팁 보여주기
		bMOVABLE_ON_CHAT	= FALSE;		//	채팅창 위에서 움직이기
		bFORCED_ATTACK		= TRUE;			//	강제 공격 - 중국
		bNAME_DISPLAY		= TRUE	;		//	이름 항상 표시
		bNON_Rebirth		= FALSE;		//  부활 스킬 금지

		bCONFT_BOUND		= TRUE;			//	대련시 영역 제한 여부.
		wCONFT_RECOVER		= 0;			//	대련시 회복약 제한.
		fCONFT_HP_RATE		= 1.0f;			//	대련시 체력 확대 비율.
		fCONFT_TAR_HP_RATE	= 1.0f;			//	대련시 체력 확대 비율 [ 상대방(개인대련시만) ].

		dwPARTY_GET_ITEM	= 3;			//	파티옵션 아이템 분배 규칙.(디폴트는 순차습득)
		dwPARTY_GET_MONEY	= 3;			//	파티옵션 돈 분배 규칙.(디폴트는 균등분배)

		int i=0; 

		for( i = 0; i < QUICK_SLOT_NUM; ++i){
			QuickSlot[i]  =	DefaultShotcutKey[i];
		}

		for( i = 0; i < QUICK_SKILL_NUM; ++i){
			SkillSlot[i]  =	DefaultShotcutKey[i+QUICK_SLOT_NUM];
		}

		for( i = 0; i < QUICK_MENU_NUM; ++i){
			MenuShotcut[i] = DefaultShotcutKey[i+QUICK_SLOT_NUM+QUICK_SKILL_NUM];
		}
		
		for( i = 0; i < CHAT_MACRO_NUM; ++i){
			ChatMacro[i] = "";
		}
	}

	BOOL SETCHARID( const TCHAR* szCHARID )
	{
		strENC_CHARID = szCHARID;
		
		return TRUE;
	}

	std::string GETCHARID()
	{
		return strENC_CHARID;
	}


	BOOL SETUSERID ( const TCHAR* szUSERID )
	{
		SETENCRYPT ();

		BYTE szENC_USERID[MAX_PATH] = "";

		size_t nKEYLENG = strlen ( szEncryptKEY );
		size_t nIDLENG = strlen ( szUSERID );
		compbyte::encrypt ( (UCHAR*) szEncryptKEY, (DWORD)nKEYLENG, (const UCHAR*) szUSERID, szENC_USERID, (DWORD)nIDLENG );
		compbyte::hex2string( szENC_USERID, (DWORD)nIDLENG, strENC_USERID );
	
		return TRUE;
	}

	std::string GETUSERID_ENC ()
	{
		return strENC_USERID;
	}

	std::string GETUSERID_DEC ()
	{
		SETENCRYPT ();

		const char* szENC_USERID = strENC_USERID.c_str();

		size_t nKEYLENG = strlen ( szEncryptKEY );
		size_t nIDLENG = strlen ( szENC_USERID );
		if ( nIDLENG == 0 )		return "";

		BYTE szHEX_USERID[MAX_PATH] = "";
		char szDEC_USERID[MAX_PATH] = "";

		SecureZeroMemory ( szHEX_USERID, MAX_PATH );
		SecureZeroMemory ( szDEC_USERID, MAX_PATH );

		BOOL bOK = compbyte::string2hex ( (UCHAR*)szENC_USERID, (UCHAR*) szHEX_USERID, (DWORD) nIDLENG );
		size_t nHEXLENG = strlen ( (char*) szHEX_USERID );

		compbyte::decrypt ( (UCHAR*) szEncryptKEY, (DWORD) nKEYLENG, (const UCHAR*) szHEX_USERID, (UCHAR*) szDEC_USERID, (DWORD) nHEXLENG );

		return szDEC_USERID;
	}

	BOOL LOAD_GAMEOPT ()
	{
		INIT_DATA();
		std::string strUSERID = GETCHARID(); // 파일명 가져오는 부분...
		if ( strUSERID.empty() )
		{
			MessageBox ( NULL, "USERID가 지정되어 있지 않아 LOAD_GAMEOPT() 수행불가.", "ERROR", MB_OK );
			return FALSE;
		}

		TCHAR szPROFILE[MAX_PATH]={0};
		SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

		std::string strOPTFILE(szPROFILE);
		strOPTFILE += SUBPATH::PLAYINFO_ROOT;
		strOPTFILE += strUSERID;
		strOPTFILE += _T(".gameopt");

		gltexfile cFILE;
		cFILE.reg_sep ( '\t' );
//		cFILE.reg_sep ( ' ' );

		if ( !cFILE.open(strOPTFILE.c_str(),false) )	return FALSE;

		DWORD dwVER;
		bool bok = cFILE.getflag ( "EMGAMEOPT_VER", 1, 1, dwVER );
		if ( !bok )		return FALSE;		


		if( dwVER == EMGAMEOPT_VER ) // 채팅 매크로 버전
		{
			cFILE.getflag ( "bDIS_CONFT", 1, 1, bDIS_CONFT );
			cFILE.getflag ( "bDIS_TRADE", 1, 1, bDIS_TRADE );
			cFILE.getflag ( "bDIS_PARTY", 1, 1, bDIS_PARTY );
			cFILE.getflag ( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
			cFILE.getflag ( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
			cFILE.getflag ( "bSHOW_TIP", 1, 1, bSHOW_TIP );
			cFILE.getflag ( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
			cFILE.getflag ( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
			cFILE.getflag ( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
			cFILE.getflag ( "bNON_Rebirth", 1, 1, bNON_Rebirth );

			cFILE.getflag ( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
			cFILE.getflag ( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
			cFILE.getflag ( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
			cFILE.getflag ( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

			cFILE.getflag ( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
			cFILE.getflag ( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

			CString strTemp;
			int i = 0;

			// QUICK_POTION_SHOTCUT
			for (i = 0; i< QUICK_SLOT_NUM; ++i){
				strTemp.Format("QUICK_SLOT%d", i);
				cFILE.getflag ( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
			}

			// QUICK_SKILL_SHOTCUT
			for (i = 0; i< QUICK_SKILL_NUM; ++i){
				strTemp.Format("SKILL_SLOT%d", i);
				cFILE.getflag ( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
			}


			// MENU_SHOTCUT
			cFILE.getflag ("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
			cFILE.getflag ("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
			cFILE.getflag ("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
			cFILE.getflag ("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
			cFILE.getflag ("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
			cFILE.getflag ("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
			cFILE.getflag ("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
			cFILE.getflag ("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
			cFILE.getflag ("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
			cFILE.getflag ("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
			cFILE.getflag ("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
			cFILE.getflag ("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
			cFILE.getflag ("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
			cFILE.getflag ("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
			cFILE.getflag ("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
			cFILE.getflag ("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
			cFILE.getflag ("SHOTCUT_SUMMON", 1, 1, MenuShotcut[SHOTCUT_SUMMON] );

			// 채팅 매크로
			for (i = 0; i< CHAT_MACRO_NUM; ++i){
				strTemp.Format("CHATMACRO%d", i);
				cFILE.getflag ( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
			}


		}
		if( dwVER == 0x0110 ) // 채팅 매크로 버전
		{
			cFILE.getflag ( "bDIS_CONFT", 1, 1, bDIS_CONFT );
			cFILE.getflag ( "bDIS_TRADE", 1, 1, bDIS_TRADE );
			cFILE.getflag ( "bDIS_PARTY", 1, 1, bDIS_PARTY );
			cFILE.getflag ( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
			cFILE.getflag ( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
			cFILE.getflag ( "bSHOW_TIP", 1, 1, bSHOW_TIP );
			cFILE.getflag ( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
			cFILE.getflag ( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
			cFILE.getflag ( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
			cFILE.getflag ( "bNON_Rebirth", 1, 1, bNON_Rebirth );

			cFILE.getflag ( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
			cFILE.getflag ( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
			cFILE.getflag ( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
			cFILE.getflag ( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

			cFILE.getflag ( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
			cFILE.getflag ( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

			CString strTemp;
			int i = 0;

			// QUICK_POTION_SHOTCUT
			for (i = 0; i< QUICK_SLOT_NUM; ++i){
				strTemp.Format("QUICK_SLOT%d", i);
				cFILE.getflag ( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
			}

			// QUICK_SKILL_SHOTCUT
			for (i = 0; i< QUICK_SKILL_NUM; ++i){
				strTemp.Format("SKILL_SLOT%d", i);
				cFILE.getflag ( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
			}


			// MENU_SHOTCUT
			cFILE.getflag ("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
			cFILE.getflag ("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
			cFILE.getflag ("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
			cFILE.getflag ("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
			cFILE.getflag ("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
			cFILE.getflag ("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
			cFILE.getflag ("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
			cFILE.getflag ("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
			cFILE.getflag ("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
			cFILE.getflag ("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
			cFILE.getflag ("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
			cFILE.getflag ("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
			cFILE.getflag ("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
			cFILE.getflag ("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
			cFILE.getflag ("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
			cFILE.getflag ("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );

			// 채팅 매크로
			for (i = 0; i< CHAT_MACRO_NUM; ++i){
				strTemp.Format("CHATMACRO%d", i);
				cFILE.getflag ( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
			}


		}
		if( dwVER == 0x0109 ) // 채팅 매크로 버전
		{
			cFILE.getflag ( "bDIS_CONFT", 1, 1, bDIS_CONFT );
			cFILE.getflag ( "bDIS_TRADE", 1, 1, bDIS_TRADE );
			cFILE.getflag ( "bDIS_PARTY", 1, 1, bDIS_PARTY );
			cFILE.getflag ( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
			cFILE.getflag ( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
			cFILE.getflag ( "bSHOW_TIP", 1, 1, bSHOW_TIP );
			cFILE.getflag ( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
			cFILE.getflag ( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
			cFILE.getflag ( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );

			cFILE.getflag ( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
			cFILE.getflag ( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
			cFILE.getflag ( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
			cFILE.getflag ( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

			cFILE.getflag ( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
			cFILE.getflag ( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

			CString strTemp;
			int i = 0;

			// QUICK_POTION_SHOTCUT
			for (i = 0; i< QUICK_SLOT_NUM; ++i){
				strTemp.Format("QUICK_SLOT%d", i);
                cFILE.getflag ( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
			}

			// QUICK_SKILL_SHOTCUT
			for (i = 0; i< QUICK_SKILL_NUM; ++i){
				strTemp.Format("SKILL_SLOT%d", i);
                cFILE.getflag ( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
			}


            // MENU_SHOTCUT
			cFILE.getflag ("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
			cFILE.getflag ("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
			cFILE.getflag ("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
			cFILE.getflag ("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
			cFILE.getflag ("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
			cFILE.getflag ("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
			cFILE.getflag ("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
			cFILE.getflag ("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
			cFILE.getflag ("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
			cFILE.getflag ("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
			cFILE.getflag ("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
			cFILE.getflag ("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
			cFILE.getflag ("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
			cFILE.getflag ("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
			cFILE.getflag ("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
			cFILE.getflag ("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );

			// 채팅 매크로
			for (i = 0; i< CHAT_MACRO_NUM; ++i){
				strTemp.Format("CHATMACRO%d", i);
				cFILE.getflag ( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
			}


		}
		else if ( dwVER==0x0108 ) // 단축키 저장 버전
		{ 
			cFILE.getflag ( "bDIS_CONFT", 1, 1, bDIS_CONFT );
			cFILE.getflag ( "bDIS_TRADE", 1, 1, bDIS_TRADE );
			cFILE.getflag ( "bDIS_PARTY", 1, 1, bDIS_PARTY );
			cFILE.getflag ( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
			cFILE.getflag ( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
			cFILE.getflag ( "bSHOW_TIP", 1, 1, bSHOW_TIP );
			cFILE.getflag ( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
			cFILE.getflag ( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
			cFILE.getflag ( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );

			cFILE.getflag ( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
			cFILE.getflag ( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
			cFILE.getflag ( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
			cFILE.getflag ( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

			cFILE.getflag ( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
			cFILE.getflag ( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

			CString strTemp;
			int i = 0;

			// QUICK_POTION_SHOTCUT
			for (i = 0; i< QUICK_SLOT_NUM; ++i){
				strTemp.Format("QUICK_SLOT%d", i);
                cFILE.getflag ( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
			}

			// QUICK_SKILL_SHOTCUT
			for (i = 0; i< QUICK_SKILL_NUM; ++i){
				strTemp.Format("SKILL_SLOT%d", i);
                cFILE.getflag ( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
			}

			// QUICK_MENU_SHOTCUT
			for (i = 0; i< QUICK_MENU_NUM; ++i){
				strTemp.Format("MENU_SHOTCUT%d", i);
                cFILE.getflag ( (LPCTSTR)strTemp, 1, 1, MenuShotcut[i] );
			}
		}
		else if ( dwVER==0x0107 )
		{
			cFILE.getflag ( "bDIS_CONFT", 1, 1, bDIS_CONFT );
			cFILE.getflag ( "bDIS_TRADE", 1, 1, bDIS_TRADE );
			cFILE.getflag ( "bDIS_PARTY", 1, 1, bDIS_PARTY );
			cFILE.getflag ( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
			cFILE.getflag ( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
			cFILE.getflag ( "bSHOW_TIP", 1, 1, bSHOW_TIP );
			cFILE.getflag ( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
			cFILE.getflag ( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
			cFILE.getflag ( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );

			cFILE.getflag ( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
			cFILE.getflag ( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
			cFILE.getflag ( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
			cFILE.getflag ( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

			cFILE.getflag ( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
			cFILE.getflag ( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
		}
		else if ( dwVER==0x0106 )
		{
			cFILE.getflag ( "bDIS_CONFT", 1, 1, bDIS_CONFT );
			cFILE.getflag ( "bDIS_TRADE", 1, 1, bDIS_TRADE );
			cFILE.getflag ( "bDIS_PARTY", 1, 1, bDIS_PARTY );
			cFILE.getflag ( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
			cFILE.getflag ( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
			cFILE.getflag ( "bSHOW_TIP", 1, 1, bSHOW_TIP );
			cFILE.getflag ( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );

			cFILE.getflag ( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
			cFILE.getflag ( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
			cFILE.getflag ( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
			cFILE.getflag ( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

			cFILE.getflag ( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
			cFILE.getflag ( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
		}
		else if ( dwVER==0x0105 )
		{
			cFILE.getflag ( "bDIS_CONFT", 1, 1, bDIS_CONFT );
			cFILE.getflag ( "bDIS_TRADE", 1, 1, bDIS_TRADE );
			cFILE.getflag ( "bDIS_PARTY", 1, 1, bDIS_PARTY );
			cFILE.getflag ( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
			cFILE.getflag ( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );		

			cFILE.getflag ( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
			cFILE.getflag ( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
			cFILE.getflag ( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
			cFILE.getflag ( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

			cFILE.getflag ( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
			cFILE.getflag ( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
		}
		else if ( dwVER==0x0104 || dwVER==0x0103 )
		{
			cFILE.getflag ( "bDIS_CONFT", 1, 1, bDIS_CONFT );
			cFILE.getflag ( "bDIS_TRADE", 1, 1, bDIS_TRADE );
			cFILE.getflag ( "bDIS_PARTY", 1, 1, bDIS_PARTY );
			cFILE.getflag ( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );			

			cFILE.getflag ( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
			cFILE.getflag ( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
			cFILE.getflag ( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
			cFILE.getflag ( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

			cFILE.getflag ( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
			cFILE.getflag ( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
		}
		else if ( dwVER==0x0102 )
		{
			cFILE.getflag ( "bDIS_CONFT", 1, 1, bDIS_CONFT );
			cFILE.getflag ( "bDIS_TRADE", 1, 1, bDIS_TRADE );
			cFILE.getflag ( "bDIS_PARTY", 1, 1, bDIS_PARTY );

			cFILE.getflag ( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
			cFILE.getflag ( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
			cFILE.getflag ( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
			cFILE.getflag ( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

			cFILE.getflag ( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
			cFILE.getflag ( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
		}
		else if ( dwVER==0x0101 )
		{
			cFILE.getflag ( "bDIS_CONFT", 1, 1, bDIS_CONFT );
			cFILE.getflag ( "bDIS_TRADE", 1, 1, bDIS_TRADE );
			cFILE.getflag ( "bDIS_PARTY", 1, 1, bDIS_PARTY );

			cFILE.getflag ( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
			cFILE.getflag ( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
			cFILE.getflag ( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
			cFILE.getflag ( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

			cFILE.getflag ( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
			cFILE.getflag ( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
		}

		return TRUE;
	}

	BOOL SAVE_GAMEOPT ()
	{
		std::string strUSERID = GETCHARID(); // 파일명 가져오는 부분...
		if ( strUSERID.empty() )
		{
			//MessageBox ( NULL, "USERID가 지정되어 있지 않아 SAVE_GAMEOPT() 수행불가.", "ERROR", MB_OK );
			return FALSE;
		}
		
		TCHAR szPROFILE[MAX_PATH]={0};
		SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

		std::string strOPTFILE(szPROFILE);
		strOPTFILE += SUBPATH::PLAYINFO_ROOT;
		strOPTFILE += strUSERID;
		strOPTFILE += _T(".gameopt");

		FILE* file=NULL;
		fopen_s (&file, strOPTFILE.c_str(), "wt");
		if ( !file )	return FALSE;

		fprintf ( file, "\n" );
		fprintf ( file, "// GAME OPTION\n" );

		fprintf ( file, "EMGAMEOPT_VER\t%d\n", EMGAMEOPT_VER );
		fprintf ( file, "bDIS_CONFT\t%d\n", bDIS_CONFT );
		fprintf ( file, "bDIS_TRADE\t%d\n", bDIS_TRADE );
		fprintf ( file, "bDIS_PARTY\t%d\n", bDIS_PARTY );
		fprintf ( file, "bDIS_FRIEND\t%d\n", bDIS_FRIEND );	
		fprintf ( file, "bSHOW_SIMPLEHP\t%d\n", bSHOW_SIMPLEHP );	
		fprintf ( file, "bSHOW_TIP\t%d\n", bSHOW_TIP );
		fprintf ( file, "bMOVABLE_ON_CHAT\t%d\n", bMOVABLE_ON_CHAT );
		fprintf ( file, "bFORCED_ATTACK\t%d\n", bFORCED_ATTACK );
		fprintf ( file, "bNAME_DISPLAY\t%d\n", bNAME_DISPLAY );
		fprintf ( file, "bNON_Rebirth\t%d\n", bNON_Rebirth );

		fprintf ( file, "bCONFT_BOUND\t%d\n", bCONFT_BOUND );
		fprintf ( file, "wCONFT_RECOVER\t%d\n", wCONFT_RECOVER );
		fprintf ( file, "fCONFT_HP_RATE\t%1.1f\n", fCONFT_HP_RATE );
		fprintf ( file, "fCONFT_TAR_HP_RATE\t%1.1f\n", fCONFT_TAR_HP_RATE );

		fprintf ( file, "dwPARTY_GET_ITEM\t%d\n", dwPARTY_GET_ITEM );
		fprintf ( file, "dwPARTY_GET_MONEY\t%d\n", dwPARTY_GET_MONEY );


		CString strTemp;
		int i=0;

		// QUICK_POTION_SHOTCUT
		for (i=0; i< QUICK_SLOT_NUM; ++i){
			if(QuickSlot[i] > 0){
				strTemp.Format("QUICK_SLOT%d\t%d\n", i, QuickSlot[i]);			
				fprintf ( file, strTemp);
			}
		}

		// QUICK_SKILL_SHOTCUT
		for (i=0; i< QUICK_SKILL_NUM; ++i){
			if(SkillSlot[i] > 0){
				strTemp.Format("SKILL_SLOT%d\t%d\n", i, SkillSlot[i]);
				fprintf ( file, strTemp);
			}
		}

		// MENU_SHOTCUT
		fprintf ( file, "SHOTCUT_INVEN\t%d\n", MenuShotcut[SHOTCUT_INVEN] );
		fprintf ( file, "SHOTCUT_CHAR\t%d\n", MenuShotcut[SHOTCUT_CHAR] );
		fprintf ( file, "SHOTCUT_SKILL\t%d\n", MenuShotcut[SHOTCUT_SKILL] );
		fprintf ( file, "SHOTCUT_PARTY\t%d\n", MenuShotcut[SHOTCUT_PARTY] );
		fprintf ( file, "SHOTCUT_QUEST\t%d\n", MenuShotcut[SHOTCUT_QUEST] );
		fprintf ( file, "SHOTCUT_CLUB\t%d\n", MenuShotcut[SHOTCUT_CLUB] );
		fprintf ( file, "SHOTCUT_FRIEND\t%d\n", MenuShotcut[SHOTCUT_FRIEND] );
		fprintf ( file, "SHOTCUT_MAP\t%d\n", MenuShotcut[SHOTCUT_MAP] );
		fprintf ( file, "SHOTCUT_ITEMBANK\t%d\n", MenuShotcut[SHOTCUT_ITEMBANK] );
		fprintf ( file, "SHOTCUT_RUN\t%d\n", MenuShotcut[SHOTCUT_RUN] );
		fprintf ( file, "SHOTCUT_HELP\t%d\n", MenuShotcut[SHOTCUT_HELP] );
		fprintf ( file, "SHOTCUT_PET\t%d\n", MenuShotcut[SHOTCUT_PET] );
		fprintf ( file, "SHOTCUT_ATTACKMODE\t%d\n", MenuShotcut[SHOTCUT_ATTACKMODE] );
		fprintf ( file, "SHOTCUT_PKMODE\t%d\n", MenuShotcut[SHOTCUT_PKMODE] );
		fprintf ( file, "SHOTCUT_ITEMSHOP\t%d\n", MenuShotcut[SHOTCUT_ITEMSHOP] );
		fprintf ( file, "SHOTCUT_CHATMACRO\t%d\n", MenuShotcut[SHOTCUT_CHATMACRO] );
		fprintf ( file, "SHOTCUT_SUMMON\t%d\n", MenuShotcut[SHOTCUT_SUMMON] );

		// 채팅 매크로
		CString strMacro;
		for (i=0; i< CHAT_MACRO_NUM; ++i){
			if(ChatMacro[i].GetLength() > 0){
				strMacro = ChatMacro[i];
				strMacro.Replace("%", "%%");
				strTemp.Format("CHATMACRO%d\t=\t%s\n", i, strMacro);
				fprintf ( file, strTemp);
			}
		}


		fclose ( file );
	
		return TRUE;
	}

	
	BOOL LOAD_PARAM( const TCHAR *szRootPath )
	{
		strParamFile = strRootPath;
		strParamFile += "\\param.ini";

		CIniLoader cFILE;
		
		if( !cFILE.open( strParamFile, true) )
		{
			return FALSE;
		}

		// param.ini use not error msg box
		cFILE.SetUseErrorMsgBox( FALSE );

		{
			cFILE.getflag_sz( "SERVER SET", "LoginAddress", 0, 1, LoginAddress, STRING_NUM_128 );

			UINT nNum = cFILE.GetKeyDataSize( "SERVER SET", "HttpAddress" );

			memset( HttpAddressTable, 0, sizeof(HttpAddressTable) );
			
			for ( UINT i=0; i<nNum; ++i )
			{
				CString strHttpAddress;
				cFILE.getflag( "SERVER SET", "HttpAddress", i, nNum, strHttpAddress );
				strHttpAddress.Trim( _T(" ") );

				StringCchCopy( HttpAddressTable[i], STRING_NUM_128, strHttpAddress );
			}

			cFILE.getflag ( "SERVER SET", "strNewsURL", 0, 1, strNewsURL );
			cFILE.getflag ( "SERVER SET", "strConnectionURL", 0, 1, strConnectionURL );
			cFILE.getflag ( "SERVER SET", "bUsePassiveDN", 0, 1, bUsePassiveDN );			
		}

		#ifdef CH_PARAM
		{
			CString strSection;

			for( UINT i=0; i<MAX_CHINA_REGION; ++i )
			{
				strSection.Format( "CH_REGION_%d", i );
				cFILE.getflag( strSection.GetString(), "strName", 0, 1, China_Region[i].strName );
				cFILE.getflag( strSection.GetString(), "LoginAddress", 0, 1, China_Region[i].LoginAddress );

				UINT nNum = cFILE.GetKeyDataSize( strSection.GetString(), "HttpAddress" );
				if( nNum > MAX_HTTP ) nNum = MAX_HTTP;
				for( UINT j=0; j<nNum; ++j )
				{
					cFILE.getflag( strSection.GetString(), "HttpAddress", j, nNum, China_Region[i].HttpAddressTable[j] );
					China_Region[i].HttpAddressTable[j].Trim( _T(" ") );
				}

				cFILE.getflag( strSection.GetString(), "strNewsURL", 0, 1, China_Region[i].strNewsURL );

				nNum = cFILE.GetKeyDataSize( strSection.GetString(), "ServerName" );
				if( nNum > MAX_SERVER ) nNum = MAX_SERVER;
				for( UINT j=0; j<nNum; ++j )
				{
					cFILE.getflag( strSection.GetString(), "ServerName", j, nNum, China_Region[i].ServerName[j] );
					China_Region[i].ServerName[j].Trim( _T(" ") );
				}

				cFILE.getflag( strSection.GetString(), "ItemShopAddress", 0, 1, China_Region[i].strItemShop );
			}
		}
		#endif

		{
			cFILE.getflag ( "GUI OPTION", "dwLangSet", 0, 1, dwLangSet );
			cFILE.getflag ( "GUI OPTION", "strGDIFont", 0, 1, strGDIFont );
		}

		{
			cFILE.getflag ( "TEXT FILE", "strCountry", 0, 1, strCountry );
			cFILE.getflag ( "TEXT FILE", "strLauncherText", 0, 1, strLauncherText );	
			cFILE.getflag ( "TEXT FILE", "strGameInText", 0, 1, strGameInText );
			cFILE.getflag ( "TEXT FILE", "strGameExText", 0, 1, strGameExText );
			cFILE.getflag ( "TEXT FILE", "strGameWord", 0, 1, strGameWord );
			cFILE.getflag ( "TEXT FILE", "strServerText", 0, 1, strServerText );

			cFILE.getflag ( "TEXT FILE", "strUIOuterCfg", 0, 1, strUIOuterCfg );
			cFILE.getflag ( "TEXT FILE", "strUIInnerCfg01", 0, 1, strUIInnerCfg01 );
			cFILE.getflag ( "TEXT FILE", "strUIInnerCfg02", 0, 1, strUIInnerCfg02 );
			cFILE.getflag ( "TEXT FILE", "strUIInnerCfg03", 0, 1, strUIInnerCfg03 );
			cFILE.getflag ( "TEXT FILE", "strUIExtCfg", 0, 1, strUIExtCfg );
		}

		// 도움말 웹 페이지 주소, 아직 사용 안함 ( 준혁 )
		cFILE.getflag_sz( "ETC OPTION", "HelpAddress", 0, 1, HelpAddress, STRING_NUM_128 );
		cFILE.getflag_sz( "ETC OPTION", "ItemShopAddress", 0, 1, ItemShopAddress, STRING_NUM_128 );
		cFILE.getflag_sz( "ETC OPTION", "RPFDataPath", 0, 1, RPFDataPath, STRING_NUM_128 );

		//	Note : 유효성 체크.
		//
		if ( COMMENT::FONTSYS[0]!=strFontType.GetString() && COMMENT::FONTSYS[1]!=strFontType.GetString() )
		{
			strFontType = COMMENT::FONTSYS[0].c_str();
		}		

		return TRUE;
	}

	
	BOOL LOAD_OPT( const TCHAR *szRootPath, bool bServer )
	{
		strOptionFile = strRootPath;
		strOptionFile += "\\option.ini";


		if ( !PathFileExists( strOptionFile.c_str() ) )
		{
			return FALSE;
		}

		CIniLoader cFILE;
		
		if( !cFILE.open( strOptionFile, true) )
		{
			return FALSE;
		}

		// param.ini use not error msg box
		cFILE.SetUseErrorMsgBox( FALSE );

		#if !defined(KR_PARAM) && !defined(RZ_PARAM)
		{
			if( !bServer )
			{
			//#ifndef TW_PARAM
			//	cFILE.getflag( "GAME OPTION", "bSAVE_USERID", 0, 1, bSAVE_USERID );
			//	if ( bSAVE_USERID )
			//#endif
				{
					cFILE.getflag( "GAME OPTION", "strENC_USERID", 0, 1, strENC_USERID );
					if ( strIDNULL == strENC_USERID )	strENC_USERID = "";
				}
			}
			//cFILE.getflag ( "GAME OPTION", "bDISP_CONSOLE", 0, 1, bDISP_CONSOLE );
			//cFILE.getflag ( "GAME OPTION", "bDISP_FPS", 0, 1, bDISP_FPS );
		}
		#endif

		cFILE.getflag( "GAME OPTION", "nChinaRegion", 0, 1, nChinaRegion );
		
		{
			cFILE.getflag ( "SCREEN OPTION", "bCHECKWHQL", 0, 1, bCHECKWHQL );
			cFILE.getflag ( "SCREEN OPTION", "dwScrWidth", 0, 1, dwScrWidth );
			cFILE.getflag ( "SCREEN OPTION", "dwScrHeight", 0, 1, dwScrHeight );
			int nTEMP(0);
			cFILE.getflag ( "SCREEN OPTION", "emScrFormat", 0, 1, nTEMP );
			emScrFormat = (EMSCREEN_FORMAT) nTEMP;
			cFILE.getflag ( "SCREEN OPTION", "uScrRefreshHz", 0, 1, uScrRefreshHz );
			cFILE.getflag ( "SCREEN OPTION", "bScrWindowed", 0, 1, bScrWindowed );
			cFILE.getflag ( "SCREEN OPTION", "bScrWndFullSize", 0, 1, bScrWndFullSize );
			cFILE.getflag ( "SCREEN OPTION", "bGameCursor", 0, 1, bGameCursor );
		}
		
		{
			cFILE.getflag ( "SOUND OPTION", "b3D_SOUND", 0, 1, b3D_SOUND );
			cFILE.getflag ( "SOUND OPTION", "nSndSfx", 0, 1, nSndSfx );
			cFILE.getflag ( "SOUND OPTION", "nSndMap", 0, 1, nSndMap );
			cFILE.getflag ( "SOUND OPTION", "nSndMusic", 0, 1, nSndMusic );
			cFILE.getflag ( "SOUND OPTION", "dw3DAlgorithm", 0, 1, dw3DAlgorithm );
			cFILE.getflag ( "SOUND OPTION", "dwSndBufferSize", 0, 1, dwSndBufferSize );
			cFILE.getflag ( "SOUND OPTION", "bSndMuteSfx", 0, 1, bSndMuteSfx );
			cFILE.getflag ( "SOUND OPTION", "bSndMuteMap", 0, 1, bSndMuteMap );
			cFILE.getflag ( "SOUND OPTION", "bSndMuteMusic", 0, 1, bSndMuteMusic );
		}

		{
			cFILE.getflag ( "GRAPHIC OPTION", "dwVideoLevel", 0, 1, dwVideoLevel );
			cFILE.getflag ( "GRAPHIC OPTION", "dwShadowChar", 0, 1, dwShadowChar );
			cFILE.getflag ( "GRAPHIC OPTION", "bBuff", 0, 1, bBuff );
			cFILE.getflag ( "GRAPHIC OPTION", "bShadowLand", 0, 1, bShadowLand );
			cFILE.getflag ( "GRAPHIC OPTION", "bRealReflect", 0, 1, bRealReflect );
			cFILE.getflag ( "GRAPHIC OPTION", "bRefract", 0, 1, bRefract );
			cFILE.getflag ( "GRAPHIC OPTION", "bGlow", 0, 1, bGlow );
			cFILE.getflag ( "GRAPHIC OPTION", "bPost", 0, 1, bPost );
			cFILE.getflag ( "GRAPHIC OPTION", "bFrameLimit", 0, 1, bFrameLimit );
			cFILE.getflag ( "GRAPHIC OPTION", "dwSkinDetail", 0, 1, dwSkinDetail );
			cFILE.getflag ( "GRAPHIC OPTION", "dwFogRange", 0, 1, dwFogRange );
			cFILE.getflag ( "GRAPHIC OPTION", "bBUFF_SFX", 0, 1, bBUFF_SFX );
			cFILE.getflag ( "GRAPHIC OPTION", "bEMOTICON", 0, 1, bEMOTICON );
			cFILE.getflag ( "GRAPHIC OPTION", "strFontType", 0, 1, strFontType );
		}

		if ( !bScrWndHalfSize )		bScrWndFullSize = TRUE;

		return TRUE;		
	}
};

