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

		//	Note : ���̾� ������ �ʿ��� ���� ��ġ���� �ʱ�ȭ.
		//
		
		CString strAppPath;
		char szAppPath[MAX_PATH] = {0};
		char szFullPath[MAX_PATH] = {0};
		strAppPath = _szAppPath;
		StringCchCopy ( szAppPath, MAX_PATH, strAppPath.GetString () );

		//	Memo :	����� ���� ������ �����Ǿ�� �ϴ� ���� ����.
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

		//	Note : �⺻ ���� ���� ����.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::CACHE );
		CreateDirectory ( szFullPath, NULL );

		//	Note : ����׻��� �ʱ�ȭ.
		//
		CDebugSet::OneTimeSceneInit ( szAppPath );

		//	�������� �б�.
		RANPARAM::LOAD ( strAppPath.GetString(), true );

		//	Note : ���� ��ġ��.
		//

		//	Note : ������ ���� �ʱ�ȭ.
		seqrandom::init();

		//	Note : DxBoneCollector �⺻ ���� ����.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKELETON );
		DxBoneCollector::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : Animation �⺻ ���� ����.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_ANIMATION );
		DxSkinAniMan::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : SkinObject �⺻ ���� ����.
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

		// Note : GLogic zip ���� �н� ����
		// zip ���� �н��� ��� �߰� by ���
		GLOGIC::SetFullPath( GLOGIC::bGLOGIC_PACKFILE );
		GLOGIC::SetEngFullPath();
		GLOGIC::OpenPackFile( szAppPath );

		// Note : Gui, Load ServerText_kr.txt
		// By ���
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GUI_FILE_ROOT );
		StringCchCat ( szFullPath, MAX_PATH, RANPARAM::strServerText.GetString() );

		CGameTextMan::GetInstance().LoadText(szFullPath, CGameTextMan::EM_SERVER_TEXT, TRUE);

		//	Note : Npc ��ȭ ����
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::NTK_FILE_ROOT );
		CNpcDialogueSet::SetPath ( szFullPath );

		//	Note : Quest ����
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::QST_FILE_ROOT );
		GLQuestMan::GetInstance().SetPath ( szFullPath );

		//	Note : ���� ����. �ý�����
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
		GLBusStation::GetInstance().SetPath ( szFullPath );
		GLTaxiStation::GetInstance().SetPath ( szFullPath );

		//	Note : GLLandMan ��� ����.
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

		//	Note : period ���� �ε�.
		//
		GLPeriod::GetInstance().LoadFile ( "period.ini" );

		//	Note : ���� ������ �ʱ�ȭ.
		//
		hr = GLogicData::GetInstance().LoadData ( TRUE );
		if ( FAILED(hr) )
		{
			pConsoleMsg->Write( _T("[GLogicData] Start Failed : GLogicData::LoadData ()") );
			return hr;
		}

		//	Note : �������� �ε�.
		GLGuidanceFieldMan::GetInstance().Load ( GLCONST_CHAR::vecGUIDANCE_FILE );

		//	Note : Ŭ�� ������ġ �ε�.
		GLClubDeathMatchFieldMan::GetInstance().Load ( GLCONST_CHAR::vecClubDM_FILE );

		//	Note : pk ��� ����.
		GLCONST_CHAR::bPK_MODE = bPK_MODE;
		GLCONST_CHAR::bPKLESS  = bPKLess;
		pConsoleMsg->Write (  "PK MODE : %d", bPK_MODE );

		//	Note : ���� ����.
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

		// fElapsedAppTime ==> 2���� �����尡 ȣ���Ͽ� �� ������ ����ϴµ�
		// Sleep(0) �� �����ϴ� ���� �����尡 fElapsedAppTime ������ ����� ���� �ִ�.
		// �׷��Ƿ� ��Ȯ�ϰ� ������ �� ���� �ʴٸ� �������� �ʴ� ���� ����!!!
		fElapsedAppTime = fNewAppTime-fAppTime;
		fAppTime = fNewAppTime;

		//	float ��Ȯ�� ���
		CheckControlfp();

		//	Note : ���� ����.
		GLPeriod::GetInstance().UpdateWeather ( fElapsedAppTime );
		GLPeriod::GetInstance().FrameMove ( fAppTime, fElapsedAppTime );

		//	Note : ���� ���� ����.
		GLGuidanceFieldMan::GetInstance().FrameMove ( fElapsedAppTime );

		//	Note : Ŭ��������ġ ����.
		GLClubDeathMatchFieldMan::GetInstance().FrameMove ( fElapsedAppTime );

		//	Note : ���̾� ����.
		hr = GLGaeaServer::GetInstance().FrameMove ( fAppTime, fElapsedAppTime );
		if ( FAILED(hr) )	return hr;

		return S_OK;
	}

	void CleanUp ()
	{
		//	Note : ����׻��� �ʱ�ȭ.
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

		//	Note : ���̾� ������ �ʿ��� ���� ��ġ���� �ʱ�ȭ.
		//
		CString strAppPath;
		char szAppPath[MAX_PATH] = {0};
		char szFullPath[MAX_PATH] = {0};
		strAppPath = _szAppPath;
		StringCchCopy ( szAppPath, MAX_PATH, strAppPath.GetString () );

		//	Memo :	����� ���� ������ �����Ǿ�� �ϴ� ���� ����.
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

		//	Note : �⺻ ���� ���� ����.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::CACHE );
		CreateDirectory ( szFullPath, NULL );

		//	Note : ����׻��� �ʱ�ȭ.
		//
		CDebugSet::OneTimeSceneInit ( szAppPath );

		//	�������� �б�.
		RANPARAM::LOAD ( strAppPath.GetString(), true );

		//	Note : ���� ��ġ��.
		//

		//	Note : ������ ���� �ʱ�ȭ.
		seqrandom::init();

		//	Note : DxBoneCollector �⺻ ���� ����.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKELETON );
		DxBoneCollector::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : Animation �⺻ ���� ����.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_ANIMATION );
		DxSkinAniMan::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : SkinObject �⺻ ���� ����.
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

		//	Note : Npc ��ȭ ����
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::NTK_FILE_ROOT );
		CNpcDialogueSet::SetPath ( szFullPath );

		//	Note : Quest ����
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::QST_FILE_ROOT );
		GLQuestMan::GetInstance().SetPath ( szFullPath );

		//	Note : ���� ����. �ý� ����
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
		GLBusStation::GetInstance().SetPath ( szFullPath );
		GLTaxiStation::GetInstance().SetPath ( szFullPath );

		//	Note : GLLandMan ��� ����.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLMAP_FILE );
		GLLandMan::SetPath ( szFullPath );

		//	Note : period ���� �ε�.
		//
		GLPeriod::GetInstance().LoadFile ( "period.ini" );

		//	Note : ���� ������ �ʱ�ȭ.
		//
		hr = GLogicData::GetInstance().LoadData ( TRUE );
		if ( FAILED(hr) )
		{
			pConsoleMsg->Write( _T("[GLogicData] Start Failed : GLogicData::LoadData ()") );
			return hr;
		}

		//	Note : ���� ���� �ε�.
		//
		GLGuidanceAgentMan::GetInstance().Load ( GLCONST_CHAR::vecGUIDANCE_FILE );

		//	Note : Ŭ�� ������ġ �ε�.
		GLClubDeathMatchAgentMan::GetInstance().Load ( GLCONST_CHAR::vecClubDM_FILE );

		// Note : �ڵ� ���� ���� ���� �ε�
		//
		GLAutoLevelMan::GetInstance().LOAD( std::string("AutoLevelSet.ini") );

		//	Note : pk ��� ����.
		GLCONST_CHAR::bPK_MODE = bPK_MODE;
		GLCONST_CHAR::bPKLESS  = bPKLess;
		pConsoleMsg->Write (  "PK MODE : %d", bPK_MODE );

		//	Note : GL ������Ʈ ���� ����.
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

		// fElapsedAppTime ==> 2���� �����尡 ȣ���Ͽ� �� ������ ����ϴµ�
		// Sleep(0) �� �����ϴ� ���� �����尡 fElapsedAppTime ������ ����� ���� �ִ�.
		// �׷��Ƿ� ��Ȯ�ϰ� ������ �� ���� �ʴٸ� �������� �ʴ� ���� ����!!!
		fElapsedAppTime = fNewAppTime-fAppTime;
		fAppTime = fNewAppTime;

		//	float ��Ȯ�� ���
		CheckControlfp();

		//	Note : ���� ��ȭ ����.
		GLPeriod::GetInstance().FrameMove ( fAppTime, fElapsedAppTime );

		//	Note : �������� ����.
		GLGuidanceAgentMan::GetInstance().FrameMove ( fElapsedAppTime );

		//	Note : Ŭ������ ��ġ ����.
		GLClubDeathMatchAgentMan::GetInstance().FrameMove ( fElapsedAppTime );

		//	Note : �ڵ����� ����
		GLAutoLevelMan::GetInstance().FrameMove();

		//	Note : ���̾�(AGENT) ����.
		GLAgentServer::GetInstance().FrameMove(fAppTime,fElapsedAppTime);

		return S_OK;
	}

	void CleanUp ()
	{
		//	Note : ����׻��� �ʱ�ȭ.
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

		//	Note : ���� ��ġ���� �ʱ�ȭ.
		//
		char szFullPath[MAX_PATH] = {0};

		//	Note : ������ ���� �ʱ�ȭ.
		seqrandom::init();

		//	Note : ����׻��� �ʱ�ȭ.
		//
		CDebugSet::OneTimeSceneInit ( szAppPath );

		//	Note : DxBoneCollector �⺻ ���� ����.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKELETON );
		DxBoneCollector::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : Animation �⺻ ���� ����.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_ANIMATION );
		DxSkinAniMan::GetInstance().OneTimeSceneInit ( szFullPath );

		//	Note : SkinObject �⺻ ���� ����.
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

		// Note : GLogic zip ���� �н� ����
		// zip ���� �н��� ��� �߰� by ���
		GLOGIC::SetFullPath( GLOGIC::bGLOGIC_PACKFILE );
		GLOGIC::SetEngFullPath();
		GLOGIC::OpenPackFile( szAppPath );

		//	Note : Npc ��ȭ ����
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::NTK_FILE_ROOT );
		CNpcDialogueSet::SetPath ( szFullPath );

		//	Note : Quest ����
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::QST_FILE_ROOT );
		GLQuestMan::GetInstance().SetPath ( szFullPath );

		//	Note : ���� ����. �ý� ����
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
		GLBusStation::GetInstance().SetPath ( szFullPath );
		GLTaxiStation::GetInstance().SetPath ( szFullPath );

		//	Note : GLLandMan ��� ����.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLMAP_FILE );
		GLLandMan::SetPath ( szFullPath );

		//	Note : ���� ������ �ʱ�ȭ.
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
