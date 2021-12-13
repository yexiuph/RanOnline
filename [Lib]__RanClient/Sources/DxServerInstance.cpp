#include "pch.h"
#include "shlobj.h"

#include "GLITEMLMT.h"
#include "SUBPATH.h"
#include "../[Lib]__Engine/Sources/DxResponseMan.h"
#include "GLGaeaServer.h"
#include "GLOGIC.h"
#include "DxLandMan.h"
#include "GLPeriod.h"
#include "seqrandom.h"
#include "GLAgentServer.h"
#include "NpcDialogueSet.h"
#include "RANPARAM.h"
#include "GLQuestMan.h"
#include "GLBusStation.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "../[Lib]__EngineUI/Sources/GameTextLoader.h"
#include "../[Lib]__RanClientUI/Sources/TextUI//UITextControl.h"
#include "GLGuidance.h"
#include "GLClubDeathMatch.h"
#include "DxConsoleMsg.h"
#include "DxMsgServer.h"
#include "GLAutoLevelMan.h"

#include "DxServerInstance.h"

#include "RanFilter.h"
#include "GLTaxiStation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace DxFieldInstance
{
	HRESULT Create ( const char *_szAppPath, DxMsgServer *pMsgServer, DxConsoleMsg* pConsoleMsg, GLDBMan *pDBMan, int nServiceProvider, 
					 const char* szMapList, DWORD dwFieldSID, DWORD dwMaxClient, bool bPK_MODE, int nChannel, BOOL bPKLess, BOOL bUseIntelTBB,
					 DWORD dwLangSet )
	{
		HRESULT hr = S_OK;

		//	Note : 가이아 서버에 필요한 각종 장치들을 초기화.
		//
		
		CString strAppPath;
		char szAppPath[MAX_PATH] = {0};
		char szFullPath[MAX_PATH] = {0};
		strAppPath = _szAppPath;
		StringCchCopy ( szAppPath, MAX_PATH, strAppPath.GetString () );

		//	Memo :	사용자 계정 폴더에 생성되어야 하는 폴더 생성.
		//
		TCHAR szPROFILE[MAX_PATH] = {0};

		SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );
		
		StringCchCopy ( szFullPath, MAX_PATH, szPROFILE );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::SAVE_ROOT );
		CreateDirectory ( szFullPath, NULL );

		StringCchCopy ( szFullPath, MAX_PATH, szPROFILE );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::DEBUGINFO_ROOT );
		CreateDirectory ( szFullPath, NULL );

		StringCchCopy ( szFullPath, MAX_PATH, szPROFILE );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::PLAYINFO_ROOT );
		CreateDirectory ( szFullPath, NULL );

		StringCchCopy ( szFullPath, MAX_PATH, szPROFILE );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::CAPTURE );
		CreateDirectory ( szFullPath, NULL );

		//	Note : 기본 저장 폴더 생성.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::CACHE );
		CreateDirectory ( szFullPath, NULL );

		//	Note : 디버그샛의 초기화.
		//
		CDebugSet::OneTimeSceneInit ( szAppPath );

		//	설정파일 읽기.
		RANPARAM::LOAD ( strAppPath.GetString(), true );

		//	Note : 각종 장치들.
		//

		//	Note : 시퀀스 랜덤 초기화.
		seqrandom::init();

		//	Note : DxBoneCollector 기본 폴더 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKELETON );
		DxBoneCollector::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : Animation 기본 폴더 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_ANIMATION );
		DxSkinAniMan::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : SkinObject 기본 폴더 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKINOBJECT );
		DxSkinObject::SetPath ( szFullPath );
		DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
		DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );

		//	Note : DxLandMan
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::MAP_FILE );
		DxLandMan::SetPath ( szFullPath );

		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::MAP_FILE );
		GLMapAxisInfo::SetPath ( szFullPath );

		//	Note : GLogic
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
		GLOGIC::SetPath ( szFullPath );

		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_SERVER_FILE );
		GLOGIC::SetServerPath ( szFullPath );

		// Note : GLogic zip 파일 패스 셋팅
		// zip 파일 패스는 계속 추가 by 경대
		GLOGIC::SetFullPath( GLOGIC::bGLOGIC_PACKFILE );
		GLOGIC::SetEngFullPath();
		GLOGIC::OpenPackFile( szAppPath );

		// Note : Gui, Load ServerText_kr.txt
		// By 경대
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GUI_FILE_ROOT );
		StringCchCat ( szFullPath, MAX_PATH, RANPARAM::strServerText.GetString() );

		CGameTextMan::GetInstance().LoadText(szFullPath, CGameTextMan::EM_SERVER_TEXT, TRUE);

		//	Note : Npc 대화 파일
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::NTK_FILE_ROOT );
		CNpcDialogueSet::SetPath ( szFullPath );

		//	Note : Quest 파일
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::QST_FILE_ROOT );
		GLQuestMan::GetInstance().SetPath ( szFullPath );

		//	Note : 버스 파일. 택시파일
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
		GLBusStation::GetInstance().SetPath ( szFullPath );
		GLTaxiStation::GetInstance().SetPath ( szFullPath );

		//	Note : GLLandMan 경로 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLMAP_FILE );
		GLLandMan::SetPath ( szFullPath );

		CRanFilter::GetInstance().SetCodePage( CD3DFontPar::nCodePage[dwLangSet] );
		if( !CRanFilter::GetInstance().Init(GLOGIC::bGLOGIC_ZIPFILE,
										GLOGIC::bGLOGIC_PACKFILE,
										GLOGIC::strGLOGIC_ZIPFILE.c_str(), 
										GLOGIC::GetPath() ) ) 
		{
			GASSERT( 0 && "[ERROR] : Ran Filter Initialize." );
			return E_FAIL;
		}

		pConsoleMsg->Write (  "[Gaea Server] Start" );


		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, "\\" );
		GLITEMLMT::GetInstance().SetPath ( szFullPath );

		//	Note : period 설정 로드.
		//
		GLPeriod::GetInstance().LoadFile ( "period.ini" );

		//	Note : 로직 대이터 초기화.
		//
		hr = GLogicData::GetInstance().LoadData ( TRUE );
		if ( FAILED(hr) )
		{
			pConsoleMsg->Write( _T("[GLogicData] Start Failed : GLogicData::LoadData ()") );
			return hr;
		}

		//	Note : 선도지역 로드.
		GLGuidanceFieldMan::GetInstance().Load ( GLCONST_CHAR::vecGUIDANCE_FILE );

		//	Note : 클럽 데스매치 로드.
		GLClubDeathMatchFieldMan::GetInstance().Load ( GLCONST_CHAR::vecClubDM_FILE );

		//	Note : pk 모드 설정.
		GLCONST_CHAR::bPK_MODE = bPK_MODE;
		GLCONST_CHAR::bPKLESS  = bPKLess;
		pConsoleMsg->Write (  "PK MODE : %d", bPK_MODE );

		//	Note : 서버 가동.
		//
		hr = GLGaeaServer::GetInstance().Create ( dwMaxClient, pMsgServer, pConsoleMsg, pDBMan, nServiceProvider, 
												  szMapList, dwFieldSID, nChannel, bUseIntelTBB );
		if ( FAILED(hr) )
		{
			pConsoleMsg->Write (  "[Gaea Server] Start Failed : GLGaeaServer::Create ()" );
			return E_FAIL;
		}

		SPRERIODTIME sPRERIODTIME;
		GETSEEDGAMETIME ( GLPeriod::REALTIME_TO_VBR, sPRERIODTIME );
		GLPeriod::GetInstance().SetPeriod ( sPRERIODTIME );
		GLPeriod::GetInstance().UpdateWeather ( 0.0f );

		pConsoleMsg->Write (  "[Gaea Server] Start Succeeded" );
		pConsoleMsg->Write (  "[Gaea Server] ---------------" );

		return S_OK;
	}

	HRESULT FrameMove ()
	{
		HRESULT hr;
		static float fAppTime(0.0f), fNewAppTime(0.0f), fElapsedAppTime(0.0f), fReservationTime(0.0f);

		fNewAppTime = DXUtil_Timer( TIMER_GETAPPTIME );

		if ( fAppTime == 0.0f )		fAppTime = fNewAppTime;

		if ( (fNewAppTime-fAppTime) < 0.020f )
		{
			Sleep( 0 );
			//return S_FALSE;
		}

		// fElapsedAppTime ==> 2개의 스레드가 호출하여 이 변수를 사용하는데
		// Sleep(0) 을 수행하는 동안 스레드가 fElapsedAppTime 변수를 사용할 수도 있다.
		// 그러므로 정확하게 수정할 수 있지 않다면 수정하지 않는 것이 좋다!!!
		fElapsedAppTime = fNewAppTime-fAppTime;
		fAppTime = fNewAppTime;

		//	float 정확도 향상
		CheckControlfp();

		//	Note : 날씨 갱신.
		GLPeriod::GetInstance().UpdateWeather ( fElapsedAppTime );
		GLPeriod::GetInstance().FrameMove ( fAppTime, fElapsedAppTime );

		//	Note : 선도 지역 갱신.
		GLGuidanceFieldMan::GetInstance().FrameMove ( fElapsedAppTime );

		//	Note : 클럽데스매치 갱신.
		GLClubDeathMatchFieldMan::GetInstance().FrameMove ( fElapsedAppTime );

		//	Note : 가이아 갱신.
		hr = GLGaeaServer::GetInstance().FrameMove ( fAppTime, fElapsedAppTime );
		if ( FAILED(hr) )	return hr;

		return S_OK;
	}

	void CleanUp ()
	{
		//	Note : 디버그샛의 초기화.
		//
		CDebugSet::FinalCleanup ();
		GLGaeaServer::GetInstance().CleanUp ();
	}
};

namespace DxAgentInstance
{
	HRESULT Create ( const char *_szAppPath, DxMsgServer *pMsgServer, DxConsoleMsg* pConsoleMsg, GLDBMan *pDBMan, char* szMapList, DWORD dwMaxClient, F_SERVER_INFO* pFieldInfo, bool bPK_MODE, BOOL bPKLess,
					 DWORD dwLangSet )
	{
		HRESULT hr = S_OK;
		pConsoleMsg->Write (  "[Gaea Server] Start" );		

		//	Note : 가이아 서버에 필요한 각종 장치들을 초기화.
		//
		CString strAppPath;
		char szAppPath[MAX_PATH] = {0};
		char szFullPath[MAX_PATH] = {0};
		strAppPath = _szAppPath;
		StringCchCopy ( szAppPath, MAX_PATH, strAppPath.GetString () );

		//	Memo :	사용자 계정 폴더에 생성되어야 하는 폴더 생성.
		//
		TCHAR szPROFILE[MAX_PATH] = {0};

		SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

		StringCchCopy ( szFullPath, MAX_PATH, szPROFILE );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::SAVE_ROOT );
		CreateDirectory ( szFullPath, NULL );

		StringCchCopy ( szFullPath, MAX_PATH, szPROFILE );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::DEBUGINFO_ROOT );
		CreateDirectory ( szFullPath, NULL );

		StringCchCopy ( szFullPath, MAX_PATH, szPROFILE );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::PLAYINFO_ROOT );
		CreateDirectory ( szFullPath, NULL );

		StringCchCopy ( szFullPath, MAX_PATH, szPROFILE );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::CAPTURE );
		CreateDirectory ( szFullPath, NULL );

		//	Note : 기본 저장 폴더 생성.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::CACHE );
		CreateDirectory ( szFullPath, NULL );

		//	Note : 디버그샛의 초기화.
		//
		CDebugSet::OneTimeSceneInit ( szAppPath );

		//	설정파일 읽기.
		RANPARAM::LOAD ( strAppPath.GetString(), true );

		//	Note : 각종 장치들.
		//

		//	Note : 시퀀스 랜덤 초기화.
		seqrandom::init();

		//	Note : DxBoneCollector 기본 폴더 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKELETON );
		DxBoneCollector::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : Animation 기본 폴더 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_ANIMATION );
		DxSkinAniMan::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : SkinObject 기본 폴더 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKINOBJECT );
		DxSkinObject::SetPath ( szFullPath );
		DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
		DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );

		//	Note : DxLandMan
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::MAP_FILE );
		DxLandMan::SetPath ( szFullPath );

		//	Note : GLogic
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
		GLOGIC::SetPath ( szFullPath );

		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_SERVER_FILE );
		GLOGIC::SetServerPath ( szFullPath );

		GLOGIC::SetFullPath( GLOGIC::bGLOGIC_PACKFILE );
		GLOGIC::SetEngFullPath();
		GLOGIC::OpenPackFile( szAppPath );

		// Note : Gui, Load ServerText_kr.txt
		// 
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GUI_FILE_ROOT );
		StringCchCat ( szFullPath, MAX_PATH, RANPARAM::strServerText.GetString() );

		CGameTextMan::GetInstance().LoadText(szFullPath, CGameTextMan::EM_SERVER_TEXT, TRUE);

		//	Note : Npc 대화 파일
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::NTK_FILE_ROOT );
		CNpcDialogueSet::SetPath ( szFullPath );

		//	Note : Quest 파일
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::QST_FILE_ROOT );
		GLQuestMan::GetInstance().SetPath ( szFullPath );

		//	Note : 버스 파일. 택시 파일
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
		GLBusStation::GetInstance().SetPath ( szFullPath );
		GLTaxiStation::GetInstance().SetPath ( szFullPath );

		//	Note : GLLandMan 경로 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLMAP_FILE );
		GLLandMan::SetPath ( szFullPath );

		//	Note : period 설정 로드.
		//
		GLPeriod::GetInstance().LoadFile ( "period.ini" );

		//	Note : 로직 대이터 초기화.
		//
		hr = GLogicData::GetInstance().LoadData ( TRUE );
		if ( FAILED(hr) )
		{
			pConsoleMsg->Write( _T("[GLogicData] Start Failed : GLogicData::LoadData ()") );
			return hr;
		}

		//	Note : 선도 지역 로드.
		//
		GLGuidanceAgentMan::GetInstance().Load ( GLCONST_CHAR::vecGUIDANCE_FILE );

		//	Note : 클럽 데스매치 로드.
		GLClubDeathMatchAgentMan::GetInstance().Load ( GLCONST_CHAR::vecClubDM_FILE );

		// Note : 자동 레벨 설정 파일 로드
		//
		GLAutoLevelMan::GetInstance().LOAD( std::string("AutoLevelSet.ini") );

		//	Note : pk 모드 설정.
		GLCONST_CHAR::bPK_MODE = bPK_MODE;
		GLCONST_CHAR::bPKLESS  = bPKLess;
		pConsoleMsg->Write (  "PK MODE : %d", bPK_MODE );

		//	Note : GL 에이젼트 서버 가동.
		//
		GLAgentServer::GetInstance().Create ( dwMaxClient, pMsgServer, pConsoleMsg, pDBMan, szMapList, pFieldInfo );

		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, "\\" );
		GLITEMLMT::GetInstance().SetPath ( szFullPath );

		SPRERIODTIME sPRERIODTIME;
		GETSEEDGAMETIME ( GLPeriod::REALTIME_TO_VBR, sPRERIODTIME );
		GLPeriod::GetInstance().SetPeriod ( sPRERIODTIME );
		GLPeriod::GetInstance().UpdateWeather ( 0.0f );

		CRanFilter::GetInstance().SetCodePage( CD3DFontPar::nCodePage[dwLangSet] );
		if( !CRanFilter::GetInstance().Init(GLOGIC::bGLOGIC_ZIPFILE,
										GLOGIC::bGLOGIC_PACKFILE,
										GLOGIC::strGLOGIC_ZIPFILE.c_str(), 
										GLOGIC::GetPath() ) ) 
		{
			GASSERT( 0 && "[ERROR] : Ran Filter Initialize." );
			return E_FAIL;
		}

		pConsoleMsg->Write (  "[Gaea Server] Start Succeeded" );
		pConsoleMsg->Write (  "[Gaea Server] ---------------" );

		return S_OK;
	}

	HRESULT FrameMove()
	{
		static float fAppTime(0.0f), fNewAppTime(0.0f), fElapsedAppTime(0.0f);

		fNewAppTime = DXUtil_Timer( TIMER_GETAPPTIME );
		if ( fAppTime == 0.0f )		fAppTime = fNewAppTime;

		if ( (fNewAppTime-fAppTime) < 0.020f )
		{
			Sleep( 0 );
			//return S_FALSE;
		}

		// fElapsedAppTime ==> 2개의 스레드가 호출하여 이 변수를 사용하는데
		// Sleep(0) 을 수행하는 동안 스레드가 fElapsedAppTime 변수를 사용할 수도 있다.
		// 그러므로 정확하게 수정할 수 있지 않다면 수정하지 않는 것이 좋다!!!
		fElapsedAppTime = fNewAppTime-fAppTime;
		fAppTime = fNewAppTime;

		//	float 정확도 향상
		CheckControlfp();

		//	Note : 날씨 변화 제어.
		GLPeriod::GetInstance().FrameMove ( fAppTime, fElapsedAppTime );

		//	Note : 선도지역 갱신.
		GLGuidanceAgentMan::GetInstance().FrameMove ( fElapsedAppTime );

		//	Note : 클럽데스 매치 갱신.
		GLClubDeathMatchAgentMan::GetInstance().FrameMove ( fElapsedAppTime );

		//	Note : 자동레벨 갱신
		GLAutoLevelMan::GetInstance().FrameMove();

		//	Note : 가이아(AGENT) 갱신.
		GLAgentServer::GetInstance().FrameMove(fAppTime,fElapsedAppTime);

		return S_OK;
	}

	void CleanUp ()
	{
		//	Note : 디버그샛의 초기화.
		//
		CDebugSet::FinalCleanup ();

		GLAgentServer::GetInstance().CleanUp ();
	}
};

namespace GMTOOL
{
    HRESULT Create ( const char* szAppPath )
    {
        HRESULT hr = S_OK;

		//	Note : 각종 장치들을 초기화.
		//
		char szFullPath[MAX_PATH] = {0};

		//	Note : 시퀀스 랜덤 초기화.
		seqrandom::init();

		//	Note : 디버그샛의 초기화.
		//
		CDebugSet::OneTimeSceneInit ( szAppPath );

		//	Note : DxBoneCollector 기본 폴더 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKELETON );
		DxBoneCollector::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : Animation 기본 폴더 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_ANIMATION );
		DxSkinAniMan::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : SkinObject 기본 폴더 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKINOBJECT );
		DxSkinObject::SetPath ( szFullPath );
		DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
		DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );

		//	Note : DxLandMan
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::MAP_FILE );
		DxLandMan::SetPath ( szFullPath );

		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
		GLOGIC::SetPath ( szFullPath );

		StringCchCopy( szFullPath, MAX_PATH, szAppPath );
		StringCchCat( szFullPath, MAX_PATH, SUBPATH::GLOGIC_SERVER_FILE );
		GLOGIC::SetServerPath ( szFullPath );

		// Note : GLogic zip 파일 패스 셋팅
		// zip 파일 패스는 계속 추가 by 경대
		GLOGIC::SetFullPath( GLOGIC::bGLOGIC_PACKFILE );
		GLOGIC::SetEngFullPath();
		GLOGIC::OpenPackFile( szAppPath );

		//	Note : Npc 대화 파일
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::NTK_FILE_ROOT );
		CNpcDialogueSet::SetPath ( szFullPath );

		//	Note : Quest 파일
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::QST_FILE_ROOT );
		GLQuestMan::GetInstance().SetPath ( szFullPath );

		//	Note : 버스 파일. 택시 파일
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
		GLBusStation::GetInstance().SetPath ( szFullPath );
		GLTaxiStation::GetInstance().SetPath ( szFullPath );

		//	Note : GLLandMan 경로 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLMAP_FILE );
		GLLandMan::SetPath ( szFullPath );

		//	Note : 로직 대이터 초기화.
		//
		hr = GLogicData::GetInstance().LoadData ( TRUE, TRUE );
		if ( FAILED(hr) )	return hr;

        return S_OK;
    }

	void CleanUp ()
	{
		CDebugSet::FinalCleanup ();
	}
};
