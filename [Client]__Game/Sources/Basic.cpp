#define STRICT
#include "pch.h"
//#include <vld.h> // vld.h 헤더파일은 stdafx.h 다음에 위치해야 합니다.

#include "SUBPATH.h"
#include "StringUtils.h"
#include "../[Lib]__EngineUI/Sources/Cursor.h"

#include "RANPARAM.h"
#include "dxparamset.h"

#include "Basic.h"
#include "BasicWnd.h"
#include "gassert.h"
#include "getdxver.h"

#include "DaumGameParameter.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "../[Lib]__MfcEx/Sources/MinBugTrap.h"

#include <afxdisp.h>

#include "Protection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicApp

DO_RTC_CATCH _rtc_catch;

// Nprotect 정의
#if defined(MY_PARAM) || defined(MYE_PARAM)
	CNPGameLib npgl("RanOnlineMY");
#elif defined(PH_PARAM)
	CNPGameLib npgl("RanOnlinePH");
//#elif defined(TW_PARAM)
//	CNPGameLib npgl("RanMinTW");
//#elif defined( VN_PARAM ) 
//	CNPGameLib npgl("RanOnlineVN");
//#elif defined(TH_PARAM)
//	CNPGameLib npgl("RanOnlineTH");
#endif

BEGIN_MESSAGE_MAP(CBasicApp, CWinApp)
	//{{AFX_MSG_MAP(CBasicApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicApp construction

CBasicApp::CBasicApp() :
	m_bIsActive(TRUE),
	m_bRedrawScreen(FALSE),
	m_bShutDown(FALSE)
{
	//	Note : 순수 가상함수 호출 감지 핸들러.
	//
	//_set_purecall_handler(smtm_PurecallHandler);
	
	BUG_TRAP::BugTrapInstall( std::string(_T("Game")));

#ifdef CH_PARAM
	RANPARAM::emSERVICE_TYPE = EMSERVICE_CHINA;
#endif

#ifdef TH_PARAM
	RANPARAM::emSERVICE_TYPE = EMSERVICE_THAILAND;
#endif

#ifdef MY_PARAM
	RANPARAM::emSERVICE_TYPE = EMSERVICE_MALAYSIA_CN;
#endif

#ifdef MYE_PARAM
	RANPARAM::emSERVICE_TYPE = EMSERVICE_MALAYSIA_EN;
#endif

#ifdef ID_PARAM
	RANPARAM::emSERVICE_TYPE = EMSERVICE_INDONESIA;
#endif

#ifdef PH_PARAM
	//RANPARAM::bScrWndHalfSize = FALSE;
	RANPARAM::emSERVICE_TYPE = EMSERVICE_PHILIPPINES;
#endif

#ifdef VN_PARAM
	RANPARAM::emSERVICE_TYPE = EMSERVICE_VIETNAM;
#endif

#ifdef JP_PARAM
	RANPARAM::emSERVICE_TYPE = EMSERVICE_JAPAN;
#endif

#ifdef TW_PARAM
	RANPARAM::emSERVICE_TYPE = EMSERVICE_FEYA;
#endif

#ifdef HK_PARAM
	RANPARAM::emSERVICE_TYPE = EMSERVICE_FEYA;
#endif

#if defined(RZ_PARAM) || defined(KR_PARAM) || defined(KRT_PARAM) 
	RANPARAM::emSERVICE_TYPE = EMSERVICE_KOREA;
#endif

#if defined(GS_PARAM)
	RANPARAM::emSERVICE_TYPE = EMSERVICE_GLOBAL;
#endif

	SecureZeroMemory( m_szAppPath, sizeof(m_szAppPath) );
}

CBasicApp::~CBasicApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBasicApp object

CBasicApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBasicApp initialization

BOOL CBasicApp::InitInstance()
{
	AfxEnableControlContainer();

	// Note : 실행파일의 경로를 찾아서 저장한다. - 반드시 CBasicWnd(CD3dApp) 이전에 수행해야 한다.
	//
	SetAppPath();

	RANPARAM::LOAD ( m_szAppPath );
	DXPARAMSET::INIT ();

	#if defined(KR_PARAM) || defined(KRT_PARAM)
	{
		//	Note : 다음 게임 파라메타 확인.
		//
		CDaumGameParameter cDGP;
		char szParameter[MAX_DGPARAMETER_LENGTH];
		if ( cDGP.Get ( _T("Parameter"), szParameter, MAX_DGPARAMETER_LENGTH ) == FALSE )
		{
			MessageBox ( NULL, _T("웹에서 실행해주십시오."), _T("란온라인"), MB_OK );
			return FALSE;
		}

		//MessageBox ( NULL, szParameter, "key", MB_OK );
		RANPARAM::SETIDPARAM ( szParameter );
		
		//	userid는 임의로 지정. ( 게임 옵션 저장용. )
		RANPARAM::SETUSERID ( "userid" );

		//	다음 게임 파라메타의 내용을 지워줍니다.
		cDGP.Empty();
	}
	#endif // KR_PARAM

	CString StrCmdLine = m_lpCmdLine;

	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "/" );
	STRUTIL::RegisterSeparator ( " " );

	BOOL bAPM = FALSE;
	//CString strCommandOld = "apm";
	CString strCommand = CString("app") + "_" + "run";

	CStringArray strCmdArray;
	STRUTIL::StringSeparate ( StrCmdLine, strCmdArray );
	
	for ( int i=0; i<strCmdArray.GetCount(); i++ )
	{
		CString strTemp = strCmdArray.GetAt(i);

		if ( strTemp == strCommand )
		{
			bAPM = TRUE;
		}

	#if defined(TW_PARAM) || defined(HK_PARAM)
		if ( strTemp == _T("ranonlineglobal") )
		{
			RANPARAM::bScrWndHalfSize = TRUE;
			RANPARAM::bScrWndFullSize = FALSE;
		}
	#else
		if ( strTemp == _T("windowed") )
		{
			RANPARAM::bScrWndHalfSize = TRUE;
			RANPARAM::bScrWndFullSize = FALSE;
		}
	#endif

		CString strVALUE;
		int nPos(-1);
		CString strKEY;

		strKEY = "TID:";
		nPos = strTemp.Find( strKEY );
		if ( nPos!=-1 )
		{
			strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

			//	값 지정.
			RANPARAM::SETIDPARAM ( strVALUE.GetString() );
			
			//	userid는 임의로 지정. ( 게임 옵션 저장용. )
			RANPARAM::SETUSERID ( "userid" );
		}


		strKEY = "U=";
		nPos = strTemp.Find( strKEY );
		if ( nPos!=-1 )
		{
			strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

			//	값 지정.
			RANPARAM::SETIDPARAM ( strVALUE.GetString() );
			
			//	userid는 임의로 지정. ( 게임 옵션 저장용. )
			RANPARAM::SETUSERID ( "userid" );
		}

		strKEY = "UUID=";
		nPos = strTemp.Find( strKEY );
		if ( nPos!=-1 )
		{
			strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

			//	값 지정.
			RANPARAM::SETIDPARAM ( strVALUE.GetString() );

			//	userid는 임의로 지정. ( 게임 옵션 저장용. )
			RANPARAM::SETUSERID ( "userid" );
		}

		strKEY = "T=";
		nPos = strTemp.Find( strKEY );
		if ( nPos!=-1 )
		{
			strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

			//	값 지정.
			RANPARAM::SETIDPARAM2 ( strVALUE.GetString() );
		}

		strKEY = "MD=";
		nPos = strTemp.Find( strKEY );
		if ( nPos!=-1 )
		{
			strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

			//	값 지정.
			RANPARAM::SETIDPARAM3 ( strVALUE.GetString() );
		}
	}

	//	Note : 웹로그인이고 cmd line param 으로 전달되는 값이 있을 경우.
	//		값이 정확하게 저장되어 있는지 체크.
	if( RANPARAM::emSERVICE_TYPE==EMSERVICE_MALAYSIA_CN || 
		RANPARAM::emSERVICE_TYPE==EMSERVICE_MALAYSIA_EN ||
//		RANPARAM::emSERVICE_TYPE==EMSERVICE_JAPAN ||	// JAPAN 로그인 방식 변경
		RANPARAM::emSERVICE_TYPE==EMSERVICE_PHILIPPINES ||
		RANPARAM::emSERVICE_TYPE==EMSERVICE_VIETNAM )
	{
		if ( !RANPARAM::VALIDIDPARAM() )
		{
			MessageBox ( NULL, "Invalid web account.", "error", MB_OK );
			return FALSE;
		}
	}

	// geger009 | Bypass execution parameter
	bAPM = TRUE;

	#if defined(NDEBUG)
	if ( !bAPM )
	{
		MessageBox ( NULL, "This Exe don't execute independent.", "error", MB_OK );
		return FALSE;
	}
	#endif // !NDEBUG

    DWORD dwDirectXVersion = 0;
    TCHAR strDirectXVersion[10];
	HRESULT hr = getdxversion ( &dwDirectXVersion, strDirectXVersion, 10 );

	if ( hr==S_OK && dwDirectXVersion<0x00090003 )
	{
		MessageBox ( NULL, "DirectX Version too old. must install DirectX 9.0c", "error", MB_OK );
		return FALSE;
	}

	{	// Note : 인터페이스 스킬 텍스트 로딩
		TCHAR szFullPath[MAX_PATH] = {0};

		StringCchCopy( szFullPath, MAX_PATH, ((CBasicApp*)AfxGetApp())->m_szAppPath );
		StringCchCat( szFullPath, MAX_PATH, SUBPATH::GUI_FILE_ROOT);
		CGameTextMan::GetInstance().SetPath(szFullPath);

		CGameTextMan::GetInstance().LoadText(RANPARAM::strGameWord.GetString(), CGameTextMan::EM_GAME_WORD, RANPARAM::bXML_USE );
		CGameTextMan::GetInstance().LoadText(RANPARAM::strGameInText.GetString(), CGameTextMan::EM_GAME_IN_TEXT, RANPARAM::bXML_USE );
		CGameTextMan::GetInstance().LoadText(RANPARAM::strGameExText.GetString(), CGameTextMan::EM_GAME_EX_TEXT, RANPARAM::bXML_USE );
	}

#if defined(MY_PARAM) || defined(MYE_PARAM) || defined(PH_PARAM) //|| defined( VN_PARAM ) || defined(TH_PARAM) || defined ( HK_PARAM ) || defined(TW_PARAM) // Nprotect 정의
	{
		DWORD dwResult = npgl.Init();
		if( dwResult != NPGAMEMON_SUCCESS )
		{
			ErrorMsgByNP( dwResult );
			return FALSE;
		}
	}
#endif

	CBasicWnd* pWnd = new CBasicWnd();
	if ( pWnd->Create() == FALSE )	return FALSE;

	m_pWnd = pWnd;

	return TRUE;
}


void CBasicApp::SetAppPath()
{
	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;

	if ( !strAppPath.IsEmpty() )
	{
		DWORD dwFind = strAppPath.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			m_strAppPath = strAppPath.Left ( dwFind );
			
			if ( !m_strAppPath.IsEmpty() )
			if ( m_strAppPath.GetAt(0) == '"' )
				m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength()-1 );

			StringCchCopy ( m_szAppPath, MAX_PATH, m_strAppPath.GetString() );
		}
	}
	else 
	{
		MessageBox ( NULL, "SetAppPath Error", "Error", MB_OK );
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////

int CBasicApp::ExitInstance() 
{
	m_pWnd = NULL;

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////

int CBasicApp::Run() 
{
	MSG msg;
	BOOL bGotMsg;

	PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE );

	CProtection& cProtection = CProtection::GetInstance();

	cProtection.Init();

	while( WM_QUIT != msg.message )
	{

#if defined(NDEBUG)	
		// software breakpoint check
		if (cProtection.Pesudo_IsDebuggerPresent())	
		{
			m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
		}

		// hardware breakpoint check
		GetThreadContext(GetCurrentThread(), &cProtection.CONTEXTDebug);	
		if ( cProtection.CONTEXTDebug.Dr0 || cProtection.CONTEXTDebug.Dr1 ||
			 cProtection.CONTEXTDebug.Dr2 || cProtection.CONTEXTDebug.Dr3 )	
		{
			m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
		}
#endif


		if ( m_bShutDown == TRUE || m_pWnd == NULL )
		{
			ExitInstance ();
			return 0;
		}
		
		
		if( m_bIsActive )
			bGotMsg = PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
		else
			bGotMsg = GetMessage( &msg, NULL, 0, 0 );

		if( bGotMsg )
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);

			if ( !m_bIsActive && !m_bShutDown )
			{
				if ( FAILED ( m_pWnd->FrameMove3DEnvironment() ) )
					m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
			}
		}
		else
		{
			if ( m_bIsActive )
			{
				if ( FAILED ( m_pWnd->Render3DEnvironment() ) )
					m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
			}
		}		
	}

	ExitInstance ();
	return static_cast<int>(msg.wParam);

	//while(1)
	//{
	//	if ( m_bShutDown == TRUE )
	//	{
	//		ExitInstance ();
	//		return 0;
	//	}
	//
	//	if ( PeekMessage ( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	//	{
	//		if ( GetMessage ( &msg, NULL, 0, 0 ) == FALSE )
	//		{
	//			ExitInstance ();
	//			return static_cast<int>(msg.wParam);
	//		}
	//		
	//		TranslateMessage (&msg);
	//		DispatchMessage (&msg);
	//	}
	//	else
	//	{
	//		if ( m_bIsActive == TRUE )
	//		{
	//			if ( m_pWnd )
	//			{
	//				if ( FAILED ( m_pWnd->Render3DEnvironment() ) )
	//					m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
	//			}
	//		}
	//		else
	//		{
	//			if ( m_pWnd )
	//			{
	//				if ( FAILED ( m_pWnd->FrameMove3DEnvironment() ) )
	//					m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
	//			}
	//			//WaitMessage();
	//		}
	//	}
	//}
}

void CBasicApp::SetActive ( BOOL bActive )
{
	m_bIsActive = bActive;
	m_bRedrawScreen = bActive;
}

void CBasicApp::ErrorMsgByNP( DWORD dwResult )
{
	TCHAR msg[256]={0};
	LPCSTR lpszMsg;

	// ‘6. 주요에러코드’를 참조하여 상황에 맞는 메시지를 출력해줍니다.
	switch (dwResult)
	{
	case NPGAMEMON_ERROR_EXIST:
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_ERROR_EXIST" );
		break;
	case NPGAMEMON_ERROR_GAME_EXIST:
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_ERROR_GAME_EXIST" );
		break;
	case NPGAMEMON_ERROR_INIT:
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_ERROR_INIT" );
		break;
	case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
	case NPGAMEMON_ERROR_NFOUND_GG:
	case NPGAMEMON_ERROR_AUTH_INI:
	case NPGAMEMON_ERROR_NFOUND_INI:
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_ERROR_AUTH_GAMEGUARD" );
		break;
	case NPGAMEMON_ERROR_CRYPTOAPI:
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_ERROR_CRYPTOAPI" );
		break;
	case NPGAMEMON_ERROR_EXECUTE:
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_ERROR_EXECUTE" );
		break;
	case NPGAMEMON_ERROR_ILLEGAL_PRG:
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_ERROR_ILLEGAL_PRG" );
		break;
	case NPGMUP_ERROR_ABORT:
		lpszMsg = ID2GAMEEXTEXT( "NPGMUP_ERROR_ABORT" );
		break;
	case NPGMUP_ERROR_CONNECT:
	case NPGMUP_ERROR_DOWNCFG:
		lpszMsg = ID2GAMEEXTEXT( "NPGMUP_ERROR_CONNECT" );
		break;
	case NPGMUP_ERROR_AUTH:
		lpszMsg = ID2GAMEEXTEXT( "NPGMUP_ERROR_AUTH" );
		break;
	case NPGAMEMON_ERROR_NPSCAN:
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_ERROR_NPSCAN" );
		break;
	default:
		// Display relevant messages.
		lpszMsg = ID2GAMEEXTEXT( "NPGAMEMON_ERROR_DEFAULT" );
		break;
	}

	wsprintf(msg, _T("GameGuard Execute Error : %lu"), dwResult);
	MessageBox(NULL, lpszMsg, msg, MB_OK);
}