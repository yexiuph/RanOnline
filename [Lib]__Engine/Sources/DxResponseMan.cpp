//----------------------------------------------------------------------------------------[DxResponseMan]
//
//		각종 툴, 게임클라이언트에서 공통으로 동작해야하는 장치들을 관리함.
//	
//		Render는 구현하지 않으므로 이부분에 삽입되는 것은  수동으로 해주어야
//		한다.
//		
//=========================================================
//
//		//	Note : Shadow Map Clear
//		//
//		DxShadowMap::GetInstance().ClearShadow ( m_pd3dDevice );
//
//=========================================================
//
//		//	Note : Light 설정.
//		//
//		DxLightMan::GetInstance()->Render ( m_pd3dDevice );
//
//=========================================================
//
//		CDebugSet::Render();
//
//=========================================================
//
//		//	Note : DxEffGroupPlayer Render.
//		//
//		DxEffGroupPlayer::GetInstance().Render ( m_pd3dDevice );
//
//=========================================================
//
//----------------------------------------------------------------------------------------
#include "pch.h"
#include "shlobj.h"

#include "./DxResponseMan.h"

#include "./CharSet.h"
#include "./CommonWeb.h"
#include "./DxInputString.h"
#include "./SeqRandom.h"
#include "./SubPath.h"

#include "./D3DFont.h"
#include "./DxClubMan.h"
#include "./DxCubeMap.h"
#include "./DxCursor.h"
#include "./DxDynamicVB.h"
#include "./DxEnvironment.h"
#include "./DxFontMan.h"
#include "./DxFogMan.h"
#include "./DxGlowMan.h"
#include "./DxGrapUtils.h"
#include "./DxInputDevice.h"
#include "./DxLightMan.h"
#include "./DxMeshTexMan.h"
#include "./DxPostProcess.h"
#include "./DxRenderStates.h"
#include "./DxShadowMap.h"
#include "./DxSkyMan.h"
#include "./DxSurfaceTex.h"
#include "./DxViewPort.h"
#include "./GammaControl.h"
#include "./NsOCTree.h"
#include "./RenderParam.h"
#include "./TextureManager.h"

#include "./DxEffectMan.h"
#include "./DxEffectShadow.h"
#include "./DxLoadShader.h"
#include "./DxMapEditMan.h"
#include "./DxPieceContainer.h"
#include "./DxPieceEffMan.h"
#include "./DxStaticMeshThread.h"
#include "./DxTexEffMan.h"
#include "./DxTextureEffMan.h"
#include "./DxEffChar.h"
#include "./DxEffCharData.h"
#include "./DxEffCharHLSL.h"
#include "./DxEffAni.h"
#include "./DxEffKeep.h"
#include "./DxEffProj.h"
#include "./DxEffGroupPlayer.h"
#include "./DxEffSingleMan.h"
#include "./DxEffSinglePropGMan.h"

#include "./DxLandMan.h"

#include "../[Lib]__EngineSound/Sources/DxSound/BgmSound.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundLib.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundMan.h"

#include "./GLogic.h"
#include "./GLPeriod.h"

#include "../[Lib]__EngineUI/Sources/Cursor.h"
#include "../[Lib]__EngineUI/Sources/UIMan.h"
#include "../[Lib]__EngineUI/Sources/UIRenderQueue.h"

#include "./DxBoneCollector.h"
#include "./DxReplaceContainer.h"
#include "./DxSimpleMeshMan.h"
#include "./DxSkinAniMan.h"
#include "./DxSkinChar.h"
#include "./DxSkinMesh9.h"
#include "./DxSkinMesh9_CPU.h"
#include "./DxSkinMesh9_HLSL.h"
#include "./DxSkinMesh9_NORMAL.h"
#include "./DxSkinMeshMan.h"
#include "./DxSkinObject.h"
#include "./DxSkinPieceContainer.h"
#include "./NsSMeshSceneGraph.h"

#include "./TextUtil.h"

#include "../[Lib]__MfcEx/Sources/RanFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxResponseMan& DxResponseMan::GetInstance()
{
	static DxResponseMan Instance;
	return Instance;
}

namespace LOADINGSTEP
{
	int nSTEP(0);
	
	void RESET ()
	{
		nSTEP = 0;
	}

	void SETSTEP ( int _nStep )
	{
		nSTEP = _nStep;
	}
	
	int GETSTEP ()
	{
		return nSTEP;
	}
};

DxResponseMan::DxResponseMan(void) :
	m_pWndApp(NULL),
	m_pd3dDevice(NULL),
	m_pD3D(NULL)
{
	memset(m_szFullPath, 0, sizeof(char) * (MAX_PATH));
}

DxResponseMan::~DxResponseMan(void)
{
	m_pd3dDevice = NULL;
}

HRESULT DxResponseMan::OneTimeSceneInit ( const char* szAppPath, CWnd *pWndApp, CString strFontSys, DWORD dwLang, CString strFontName  )
{
	GASSERT(pWndApp);
	GASSERT(szAppPath);

	m_pWndApp = pWndApp;
	StringCchCopy( m_szFullPath, MAX_PATH, szAppPath );

	TCHAR szFullPath[MAX_PATH] = {0};

	//	Note : 시퀀스 랜덤 초기화.
	seqrandom::init();

	//	Note : 실행파일 폴더 지정.
	StringCchCopy( SUBPATH::APP_ROOT, MAX_PATH, szAppPath );

	//	Memo :	사용자 계정 폴더에 생성되어야 하는 폴더 생성.
	//
	TCHAR szPROFILE[MAX_PATH] = {0};

	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

	StringCchCopy( szFullPath, MAX_PATH, szPROFILE );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::SAVE_ROOT );
	CreateDirectory( szFullPath, NULL );

	StringCchCopy( szFullPath, MAX_PATH, szPROFILE );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::DEBUGINFO_ROOT );
	CreateDirectory ( szFullPath, NULL );

	StringCchCopy( szFullPath, MAX_PATH, szPROFILE );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::PLAYINFO_ROOT );
	CreateDirectory ( szFullPath, NULL );

	StringCchCopy( szFullPath, MAX_PATH, szPROFILE );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::CAPTURE );
	CreateDirectory ( szFullPath, NULL );

	//	Note : 스크린 캡쳐 디랙토리(내그림)
	//
	DxGrapUtils::SetPath ( szFullPath );

	//	Note : 기본 저장 폴더 생성.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::CACHE );
	CreateDirectory ( szFullPath, NULL );

	//	Note : 디버그샛의 초기화.
	//
	CDebugSet::OneTimeSceneInit ( szPROFILE, true );

	//	Note : 키보드 마우스.
	//
	DxInputDevice::GetInstance().OneTimeSceneInit(m_pWndApp->m_hWnd);

	//	Note : cursor 기본 폴더 지정.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_EDIT );
	CCursor::GetInstance().OneTimeSceneInit(szFullPath,m_pWndApp->m_hWnd);

	//	Note : 폰트 메니져 초기화.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::CACHE );
	DxFontMan::GetInstance().SetPath ( szFullPath );
	DxFontMan::GetInstance().SetFontSys ( strFontSys, dwLang );

	CHARSET::SetCodePage ( CD3DFontPar::nCodePage[dwLang] );
	CRanFilter::GetInstance().SetCodePage( CD3DFontPar::nCodePage[dwLang] );

	StringCchCopy( _DEFAULT_FONT, MAX_PATH, strFontName.GetString() );
	switch ( dwLang )
	{
	case DEFAULT:
		_DEFAULT_FONT_FLAG = NULL;
		break;

	case TRADITIONAL_CHINESE:
		_DEFAULT_FONT_FLAG = D3DFONT_TCBIGFIVE;
		_DEFAULT_FONT_SHADOW_FLAG = D3DFONT_SHADOW | D3DFONT_TCBIGFIVE;
		_DEFAULT_FONT_SHADOW_EX_FLAG = D3DFONT_SHADOW_EX | D3DFONT_TCBIGFIVE;
		break;

	case JAPANESE:
		_DEFAULT_FONT_FLAG = D3DFONT_SHIFTJIS;
		_DEFAULT_FONT_SHADOW_FLAG = D3DFONT_SHADOW | D3DFONT_SHIFTJIS;
		_DEFAULT_FONT_SHADOW_EX_FLAG = D3DFONT_SHADOW_EX | D3DFONT_SHIFTJIS;
		break;

	case KOREAN:
		_DEFAULT_FONT_FLAG = NULL;
		break;

	case SIMPLIFIED_CHINESE:
		_DEFAULT_FONT_FLAG = D3DFONT_SCGB;
		_DEFAULT_FONT_SHADOW_FLAG = D3DFONT_SHADOW | D3DFONT_SCGB;
		_DEFAULT_FONT_SHADOW_EX_FLAG = D3DFONT_SHADOW_EX | D3DFONT_SCGB;
		break;

	case THAILAND:
		_DEFAULT_FONT_FLAG = D3DFONT_THAI;
		_DEFAULT_FONT_SHADOW_FLAG = D3DFONT_SHADOW | D3DFONT_THAI;
		_DEFAULT_FONT_SHADOW_EX_FLAG = D3DFONT_SHADOW_EX | D3DFONT_THAI;
		break;

	case VIETNAM:
		_DEFAULT_FONT_FLAG = D3DFONT_VIETNAM;
		_DEFAULT_FONT_SHADOW_FLAG = D3DFONT_SHADOW | D3DFONT_VIETNAM;
		_DEFAULT_FONT_SHADOW_EX_FLAG = D3DFONT_SHADOW_EX | D3DFONT_VIETNAM;
		break;
	};

	//	Note : 텍스쳐 기본 폴더 지정.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::TEXTURE_FILE_ROOT );
	TextureManager::SetTexturePath ( szFullPath );

	//	Note : ShaderMan 기본 폴더 지정.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::SHADER_FILE_ROOT );
	DXShaderMan::GetInstance().SetPath ( szFullPath );

	//	Note : ReplaceContainer 기본 폴더 지정.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_PIECE );
	DxReplaceContainer::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : DxBoneCollector 기본 폴더 지정.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKELETON );
	DxBoneCollector::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : Animation 기본 폴더 지정.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_ANIMATION );
	DxSkinAniMan::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : SkinObject 기본 폴더 지정.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKINOBJECT );
	DxSkinObject::SetPath ( szFullPath );
	DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
	DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );
	
	// Note : SimpleMesh 기본 폴더 지정
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_OBJECT );

	DxSimpleMeshMan::GetInstance().SetPath ( szFullPath );

	//	Note : SkinMesh
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKIN );
	DxSkinMeshMan::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : ShaderMan 기본 폴더 지정.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::EFF_FILE_SINGLE );
	DxEffSinglePropGMan::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : DxLandMan
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::MAP_FILE );
	DxLandMan::SetPath ( szFullPath );

	//	Note : 스트링 인풋 인스턴스.
	//
	DXInputString::GetInstance().Create(pWndApp,CRect(0,0,0,0));
	
	//	Note : GUI Directory
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::GUI_FILE_ROOT );
	CUIMan::SetPath( szFullPath );

	//	Note : 내문서 폴더 경로 찾기.
	//
	TCHAR szPersonal[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPersonal, CSIDL_PERSONAL, FALSE);
	StringCchCat( szPersonal, MAX_PATH, _T("\\") );
	
	//	Note : 클럽 마크 디렉토리(내문서)
	//
	DxClubMan::GetInstance().SetRegistPath ( szPersonal );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
	GLOGIC::SetPath ( szFullPath );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::GLOGIC_SERVER_FILE );
	GLOGIC::SetServerPath ( szFullPath );

	//	Note : 사운드 파일 매니저 초기화
	//	
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::SOUND_FILE_ROOT );
	DxSoundMan::GetInstance().OneTimeSceneInit ( pWndApp->m_hWnd, szFullPath );

	//	Note : BGM 사운드 초기화
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::BGM_FILE_ROOT );
	DxBgmSound::GetInstance().SetPath ( szFullPath );
	DxBgmSound::GetInstance().Run ( pWndApp->m_hWnd );

	//	Note : Char Skin Effect Directory
	StringCchCopy( szFullPath, MAX_PATH,  szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::EFF_FILE_CHAR );
	DxEffcharDataMan::GetInstance().SetPath ( szFullPath );

	//	Note : 정적 그림자 저장될 위치 지정
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::TEXTURE_FILE_SHADOW );
	DxEffectShadow::SetPath ( szFullPath );

	//	Note : 클럽 데이터들이 저장될 위치 지정
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::TEXTURE_FILE_CLUB );
	DxClubMan::SetPath ( szFullPath );

	// Note : 타일 데이터가 저장될 위치 지정
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::TILE_FILE_DATA );
	DxMapEditMan::SetPathData ( szFullPath );

	// Note : 타일 텍스쳐가 저장될 위치 지정
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::TILE_FILE_TEXTURE );
	DxMapEditMan::SetPathTex ( szFullPath );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_PIECE );
	DxPieceContainer::SetPath( szFullPath );

	//	Note : 텍스쳐 기본설정.
	TextureManager::OneTimeSceneInit();

	// 텍스트 텍스쳐
	CTextUtil::Get()->OneTimeSceneInit();

	// Note : 
	NSOCTREE::OneTimeSceneInit();

	// Note : CMemPool 때문.
	DxEffSingleMan::GetInstance().OneTimeSceneInit();

	// Note : StaticMesh의 컬러변화를 위한 Thread 기본설정.
	DxStaticMeshColor_THREAD::GetInstance().StartThread( m_pd3dDevice );

	// 감마 컨트롤 백업.
	GammaControl::GetInstance().BackUp( m_pWndApp->m_hWnd );

	return S_OK;
}

HRESULT DxResponseMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bCAPTUREMOUSE )
{
	m_pd3dDevice = pd3dDevice;
	
	m_pd3dDevice->GetDirect3D ( &m_pD3D );
	m_pd3dDevice->GetDeviceCaps ( &m_d3dCaps );

	LPDIRECT3DSURFACEQ pBackBuffer;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();
	
	// Note : 렌더 Param
	//RENDERPARAM::LOAD( m_pd3dDevice, m_szFullPath );

	//	에니메이션 bin 파일 읽기용 스레드 시작.
	DxSkinAniMan::GetInstance().StartBinFileLoadThread();

	//	Note : ShaderMan 기본 설정.
	//
	DXShaderMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );

	//	Note : 키보드 마우스.
	//
	DxInputDevice::GetInstance().InitDeviceObjects ( m_pd3dDevice, FALSE, bCAPTUREMOUSE );

	////	Note : 폰트 메니져 초기화.
	////
	//DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	//CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	//CD3DFontPar* pD3dFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );

	////	Note	:	디버그셋 경로 설정 및 초기화
	//CDebugSet::InitDeviceObjects ( pD3dFont9 );

	// 텍스트 텍스쳐
	SIZE sizeTexrure = { (LONG)m_d3dCaps.MaxTextureWidth, (LONG)m_d3dCaps.MaxTextureHeight };
	CTextUtil::Get()->InitDeviceObjects( m_pd3dDevice, &sizeTexrure );

	// UI 렌더큐
	CUIRenderQueue::Get()->InitDeviceObjects( m_pd3dDevice );

	//	Note : 텍스쳐 기본설정.
	TextureManager::StartThread( m_pd3dDevice );

	DxCursor::GetInstance().OnCreateDevice( m_pd3dDevice, m_pWndApp->m_hWnd );

	//	Note : 치환 조각 컨테이너 초기화.
	NSCHARSG::OnCreateDevice( m_pd3dDevice );
	DxSkinMesh9::StaticCreate ( m_pd3dDevice );
	DxSkinMesh9_HLSL::StaticCreate ( m_pd3dDevice );
	DxReplaceContainer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxSkinPieceContainer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxSkinMeshMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxSimpleMeshMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	LOADINGSTEP::SETSTEP ( 1 );

	//	Note : 효과 개체.
	DxEffectMan::GetInstance().InitDeviceObjects ( m_pD3D, m_pd3dDevice, m_d3dsdBackBuffer );
	DxEffSinglePropGMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffGroupPlayer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffCharMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffAniMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffcharDataMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffKeepMan::GetInstance().InitDeviceObjects( m_pd3dDevice );
	DxEffProjMan::GetInstance().OnCreateDevice( m_pd3dDevice );
	LOADINGSTEP::SETSTEP ( 2 );

	DxSurfaceTex::GetInstance().InitDeviceObjects ( m_pD3D, m_pd3dDevice, m_d3dsdBackBuffer );	// 기본 서페이스 
	DxWeatherMan::GetInstance()->InitDeviceObjects ( m_pd3dDevice );							// 날씨 메니져 초기화.
	DxShadowMap::GetInstance().InitDeviceObjects ( m_pD3D, m_pd3dDevice, m_d3dsdBackBuffer );	// 쉐도우 맵 초기화
	DxEnvironment::GetInstance().InitDeviceObjects ( m_pD3D, m_pd3dDevice, m_d3dsdBackBuffer );	// 반사 및 굴절
	DxCubeMap::GetInstance().InitDeviceObjects ( m_pD3D, m_pd3dDevice, m_d3dsdBackBuffer );		// 큐브맵 메니져 초기화.
	DxSkyMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );									// 하늘 메니져 초기화.
	DxClubMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );								// 클럽 메니져 초기화

	LOADINGSTEP::SETSTEP ( 3 );

	return S_OK;
}

HRESULT DxResponseMan::SetRenderState ()
{
	HRESULT hr = S_OK;

	//	Note : Setup a material
	//
	D3DMATERIALQ mtrl;
	D3DUtil_InitMaterial ( mtrl, 1.0f, 1.0f, 1.0f, 1.0f );
	m_pd3dDevice->SetMaterial( &mtrl );

	//	Note : Set up the textures
	//
	// Set up the textures
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );

	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_CURRENT );

	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );

	
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_TEXCOORDINDEX, 3 );

	//	Note : Set miscellaneous render states
	float fBias = -0.0000f;
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,		TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,		FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,			TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,				D3DCMP_LESSEQUAL );
	m_pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,				*((DWORD*)&fBias) );
	m_pd3dDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS,	*((DWORD*)&fBias) );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,			TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x40 );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATEREQUAL );

	float fFogStart=1720.0f, fFogEnd=1790.0f, fFongDensity=0.0f;
	D3DCOLOR colorClear = D3DCOLOR_XRGB(0x3F,0xB0,0xFF);
	m_pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,		colorClear);
	m_pd3dDevice->SetRenderState ( D3DRS_FOGSTART,		*((DWORD *)(&fFogStart)) );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGEND,		*((DWORD *)(&fFogEnd)) );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGDENSITY,	*((DWORD *)(&fFongDensity)) );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGTABLEMODE,	D3DFOG_NONE );

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGRANGE )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_RANGEFOGENABLE,	TRUE );
	}
	

	//	Note : Set the world matrix
	//
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

	return S_OK;
}

HRESULT DxResponseMan::RestoreDeviceObjects ()
{
	HRESULT hr = S_OK;
	if ( !m_pd3dDevice )	return S_FALSE;

	DxRenderStates::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );

	//	Note : 기초 랜더 속성 조정.
	//	
	SetRenderState ();

	//	Note : Set up lighting states
	//
	DxLightMan::GetInstance()->InitDeviceObjects ( m_pd3dDevice );

	//	Note : 폰트 메니져 초기화.
	//
	DxFontMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );

	// 텍스트 텍스쳐
	CTextUtil::Get()->RestoreDeviceObjects( m_pd3dDevice );

	// UI 렌더큐
	CUIRenderQueue::Get()->RestoreDeviceObjects( m_pd3dDevice );

	DxCursor::GetInstance().OnResetDevice( m_pd3dDevice );

	DxPostProcess::GetInstance().RestoreDeviceObjects(m_pd3dDevice);		// 후처리
	DxShadowMap::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );		// 쉐도우 맵
	DxEnvironment::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );		// 반사 및 굴절
	DxSurfaceTex::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );		// 기본 서페이스 
	DxGlowMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// 글로우 맵 
	DxWeatherMan::GetInstance()->RestoreDeviceObjects ( m_pd3dDevice );		// 날씨 메니져
	DxCubeMap::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// 큐브맵 메니져
	DxSkyMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// 하늘 메니져 초기화.
	DxFogMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// 포그 초기화
	DxDynamicVB::GetInstance().RestoreDeviceObjects( m_pd3dDevice );		// DynamicVB 초기화
	DxPieceEffMan::GetInstance().OnResetDevice( m_pd3dDevice );				// DxPieceEffMan - OnResetDevice
	OPTMManager::GetInstance().OnResetDevice( m_pd3dDevice );				// OPTMManager - OnResetDevice

	//	Note : 치환 조각 컨테이너 초기화.
	DxSkinMesh9::StaticResetDevice ( m_pd3dDevice );
	DxSkinMesh9_HLSL::StaticResetDevice( m_pd3dDevice );
	DxSkinMesh9_NORMAL::StaticResetDevice( m_pd3dDevice );
	DxReplaceContainer::GetInstance().RestoreDeviceObjects ();
	DxSkinPieceContainer::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	DxSkinMeshMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	DxSimpleMeshMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );

	//	Note : 효과 개체.
	DxEffectMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	DxEffSingleMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	DxEffSinglePropGMan::GetInstance().RestoreDeviceObjects ();
	DxEffGroupPlayer::GetInstance().RestoreDeviceObjects ();
	DxEffCharMan::GetInstance().RestoreDeviceObjects ();
	DxEffAniMan::GetInstance().RestoreDeviceObjects ();
	DxEffKeepMan::GetInstance().RestoreDeviceObjects();
	DxTexEffMan::GetInstance().OnCreateDevice( m_pd3dDevice );
	DxEffProjMan::GetInstance().OnResetDevice( m_pd3dDevice );

	return S_OK;
}

DWORD SKINDRAWCOUNT = 0;
DWORD UPDATEBONECOUNT = 0;
DWORD POLYGONCOUNT = 0;

HRESULT DxResponseMan::FrameMove ( float fTime, float fElapsedTime, BOOL bDefWin, BOOL bGame )
{
	// Note : Default 
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );

	//	Note : 키보드 마우스.
	//
	PROFILE_BEGIN("DxInputDevice,DXInputString::FrameMove");
	DxInputDevice::GetInstance().FrameMove ( fTime, fElapsedTime, bDefWin, bGame );
	PROFILE_END("DxInputDevice,DXInputString::FrameMove");

	//CCursor::GetInstance().FrameMove ( fTime, fElapsedTime );
	CCursor::GetInstance().FrameMove ( fTime, fElapsedTime );

	// Note : List가 비워있는지를 조사.
	DxStaticMeshColor_THREAD::GetInstance().CheckListEmpty();

	// Note 
	DxShadowMap::GetInstance().FrameMove ( fTime, fElapsedTime );

	// Note : DxPostProcess
	DxPostProcess::GetInstance().FrameMove();

	// Note : Character Shader Setting
	NSCHARHLSL::FrameMove();

	// Note : Character Scene Graph
	NSCHARSG::Reset();

	//// Note : 커서 돌아감.
	//DxCursor::GetInstance().FrameMove( fElapsedTime );

	//	Note : 치환 조각의 Ani-Time 설정.
	//
	PROFILE_BEGIN("DxReplaceContainer::FrameMove");
	DxReplaceContainer::SetTime ( fTime, fElapsedTime );
	DxReplaceContainer::GetInstance().FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxReplaceContainer::FrameMove");

	//	Note : 디버그셋
	//
	PROFILE_BEGIN("CDebugSet::FrameMove");
	CDebugSet::FrameMove ( fTime, fElapsedTime );
	PROFILE_END("CDebugSet::FrameMove");

	//	Note : 빛 업데이트.
	//
	PROFILE_BEGIN("DxLightMan::FrameMove");
	DxLightMan::GetInstance()->FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxLightMan::FrameMove");

	//	Note : 날씨 메니져 
	//
	PROFILE_BEGIN("DxWeatherMan::FrameMove");
	DxWeatherMan::GetInstance()->FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxWeatherMan::FrameMove");

	//	Note : Effect 업데이트.
	//
	PROFILE_BEGIN("DxEffGroupPlayer::FrameMove");
	DxEffGroupPlayer::GetInstance().FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxEffGroupPlayer::FrameMove");

	//	Note : 카메라 방향에 따라 소리변화 반영	
	//
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3	&vLookDir = DxViewPort::GetInstance().GetLookDir();

	PROFILE_BEGIN("DxSoundMan::FrameMove");
	DxSoundLib::GetInstance()->Update ();
	DxSoundMan::GetInstance().FrameMove( vLookDir, vLookatPt );	
	PROFILE_END("DxSoundMan::FrameMove");

	PROFILE_BEGIN("DxEffectMan::FrameMove");
	DxEffectMan::GetInstance().FrameMove ();
	PROFILE_END("DxEffectMan::FrameMove");

	GLPeriod::GetInstance().FrameMove ( fTime, fElapsedTime );

	DxSurfaceTex::GetInstance().FrameMove ( fTime, fElapsedTime );
	DxEnvironment::GetInstance().FrameMove ( fTime, fElapsedTime );
	DxSkyMan::GetInstance().FrameMove ( fTime, fElapsedTime );
	DxGlowMan::GetInstance().FrameMove( fTime, fElapsedTime );

	OPTMManager::GetInstance().FrameMove();

	//	Note : 스크린 캡쳐.
	//
	DWORD dwSCROLL = DxInputDevice::GetInstance().GetKeyState(DIK_SCROLL);
	DWORD dwPRINTSCR = DxInputDevice::GetInstance().GetKeyState(DIK_SYSRQ);

	if ( dwSCROLL&DXKEY_UP || dwPRINTSCR&DXKEY_UP )
	{
		DxGrapUtils::CaptureScreen ( m_pd3dDevice );
	}

	CDebugSet::ToView( 10, _T("SKINDRAWCOUNT : %d"), SKINDRAWCOUNT );
	CDebugSet::ToView( 11, _T("UPDATEBONECOUNT : %d"), UPDATEBONECOUNT );
	CDebugSet::ToView( 12, _T("VERTEXCOUNT : %d"), POLYGONCOUNT );
	CDebugSet::ToView( 13, _T("g_dwHIGHDRAW_NUM : %d"), DxSkinChar::g_dwHIGHDRAW_NUM );

	DxSkinChar::g_dwHIGHDRAW_NUM = 0;
	SKINDRAWCOUNT = 0;
	UPDATEBONECOUNT = 0;
	POLYGONCOUNT = 0;

	return S_OK;
}

HRESULT DxResponseMan::InvalidateDeviceObjects ()
{
	if ( !m_pd3dDevice )	return S_FALSE;

	//	Note : 폰트 메니져 
	//
	DxFontMan::GetInstance().InvalidateDeviceObjects ();

	// 텍스트 텍스쳐
	CTextUtil::Get()->InvalidateDeviceObjects();

	// UI 렌더큐
	CUIRenderQueue::Get()->InvalidateDeviceObjects();

	// Cursor
	DxCursor::GetInstance().OnLostDevice();

	DxPostProcess::GetInstance().InvalidateDeviceObjects();					// 후처리
	DxWeatherMan::GetInstance()->InvalidateDeviceObjects ( m_pd3dDevice );	// 날씨 메니져
	DxShadowMap::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );	// 쉐도우 맵
	DxEnvironment::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );	// 반사 및 굴절
	DxSurfaceTex::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );	// 기본 서페이스 
	DxGlowMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// 글로우 맵 
	DxCubeMap::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// 큐브맵
	DxSkyMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// 하늘 매니져
	DxFogMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// 포그
	DxDynamicVB::GetInstance().InvalidateDeviceObjects();					// DynamicVB : OnLostDevice
	DxPieceEffMan::GetInstance().OnLostDevice( m_pd3dDevice );				// DxPieceEffMan : OnLostDevice
	OPTMManager::GetInstance().OnLostDevice();								// OPTMManager - OnLostDevice

	//	Note : 치환 조각 컨테이너 초기화.
	//
	DxSkinMesh9::StaticLost();
	DxSkinMesh9_HLSL::StaticLost();
	DxSkinMesh9_NORMAL::StaticLost();
	DxReplaceContainer::GetInstance().InvalidateDeviceObjects ();
	DxSkinPieceContainer::GetInstance().InvalidateDeviceObjects ();
	DxSkinMeshMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );

	//	Note : 효과 개체.
	DxEffectMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );
	DxEffSingleMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );
	DxEffSinglePropGMan::GetInstance().InvalidateDeviceObjects ();
	DxEffGroupPlayer::GetInstance().InvalidateDeviceObjects ();
	DxEffCharMan::GetInstance().InvalidateDeviceObjects ();
	DxEffAniMan::GetInstance().InvalidateDeviceObjects ();
	DxEffKeepMan::GetInstance().InvalidateDeviceObjects();
	DxTexEffMan::GetInstance().OnReleaseDevice( m_pd3dDevice );
	DxEffProjMan::GetInstance().OnLostDevice( m_pd3dDevice );

	//	Note : SimpleMesh를 사용한 개체들을 Invalidate 후 호출한다.
	DxSimpleMeshMan::GetInstance().InvalidateDeviceObjects ();

	DxRenderStates::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );

	return S_OK;
}

HRESULT DxResponseMan::DeleteDeviceObjects ()
{
	if ( !m_pd3dDevice )	return S_FALSE;

	//	Note : 바이너리 스레드 읽기 예약 내용 리셋.
	DxSkinAniMan::GetInstance().ClearBinLoadingData ();

	//	에니메이션 bin 파일 읽기용 스레드 종료.
	DxSkinAniMan::GetInstance().EndBinFileLoadThread();

	//	Note : 키보드 마우스.
	//
	DxInputDevice::GetInstance().DeleteDeviceObjects ();

	//	Note : 폰트 메니져 초기화.
	//
	DxFontMan::GetInstance().DeleteDeviceObjects ();

	// 텍스트 텍스쳐
	CTextUtil::Get()->DeleteDeviceObjects();

	// UI 렌더큐
	CUIRenderQueue::Get()->DeleteDeviceObjects();

	// Cursor
	DxCursor::GetInstance().OnDestroyDevice();

	//	Note : 치환 조각 컨테이너 초기화.
	NSCHARSG::OnDestroyDevice();
	DxSkinMesh9::StaticDestroy();
	DxSkinMesh9_HLSL::StaticDestroy();
	DxReplaceContainer::GetInstance().DeleteDeviceObjects ();
	DxSkinPieceContainer::GetInstance().DeleteDeviceObjects ();
	DxSkinMeshMan::GetInstance().DeleteDeviceObjects ();

	//	Note : 효과 개체.
	//
	DxEffectMan::GetInstance().DeleteDeviceObjects ();
	DxEffSinglePropGMan::GetInstance().DeleteDeviceObjects ();
	DxEffGroupPlayer::GetInstance().DeleteDeviceObjects ();
	DxEffCharMan::GetInstance().DeleteDeviceObjects ();
	DxEffAniMan::GetInstance().DeleteDeviceObjects ();
	DxEffKeepMan::GetInstance().DeleteDeviceObjects();
	DxEffProjMan::GetInstance().OnDestroyDevice();

	DxSurfaceTex::GetInstance().DeleteDeviceObjects ();		// 기본 서페이스 
	DxWeatherMan::GetInstance()->DeleteDeviceObjects ();	// 반사 및 굴절
	DxShadowMap::GetInstance().DeleteDeviceObjects ();		// 쉐도우 맵
	DxEnvironment::GetInstance().DeleteDeviceObjects ();	// 반사 및 굴절
	DxCubeMap::GetInstance().DeleteDeviceObjects ();		// 큐브맵
	DxSkyMan::GetInstance().DeleteDeviceObjects ();			// 하늘 매니져	
	DxClubMan::GetInstance().DeleteDeviceObjects ();		// 클럽 메니져

	//	Note : SimpleMesh를 사용한 개체들을 Delete 후 호출한다.
	DxSimpleMeshMan::GetInstance().DeleteDeviceObjects ();

	//	Note : 텍스쳐 메니져 종료 ( 가장 하단에 존제하여야 함. )
	TextureManager::DeleteAllTexture();
	TextureManager::CleanUp();

	//	Note : 참조한 D3D의 카운트를 감소시킴.
	//
	if ( m_pD3D )	m_pD3D->Release();

	m_pd3dDevice = NULL;

	return S_OK;
}

HRESULT DxResponseMan::FinalCleanup ()
{
	// Note : 
	SAFE_DELETE_ARRAY( DxSkinMesh9_CPU::g_pBoneMatrices );
	SAFE_DELETE_ARRAY( DxSkinMesh9_HLSL::g_pBoneMatrices );
	

	//	Note : 폰트 메니져 초기화.
	DxFontMan::GetInstance().DeleteDxFontAll ();

	DxLightMan::GetInstance()->CleanUp ();

	//	Note : 치환 조각 컨테이너 초기화.
	//
	DxSkinCharDataContainer::GetInstance().CleanUp ();
	DxReplaceContainer::GetInstance().FinalCleanup ();
	DxSkinPieceContainer::GetInstance().FinalCleanup ();
	DxBoneCollector::GetInstance().CleanUp ();
	DxSkinAniMan::GetInstance().CleanUp ();
	DxPieceContainer::GetInstance().CleanUp();

	// Note : StaticMesh의 컬러변화를 위한 Thread 기본설정.
	DxStaticMeshColor_THREAD::GetInstance().EndThread();

	// 텍스트 텍스쳐
	CTextUtil::CleanUp();

	// Note : CUIRenderQueue 안 하면 leak
	CUIRenderQueue::CleanUp();

	// 공통으로 사용하는 웹 컨트롤 제거
	CCommonWeb::CleanUp();

	// Note : CMemPool 때문.
	DxEffSingleMan::GetInstance().FinalCleanup();

	//	Note : 사운드 객체 내리기
	//
	DxSoundLib::GetInstance()->ReleaseInstance ();
	DxSoundMan::GetInstance().FinalCleanup ();	

	// Note : 
	NSOCTREE::CleanUp();

	// 감마 컨트롤 백업.
	if( m_pWndApp != NULL )
		GammaControl::GetInstance().RollBack( m_pWndApp->m_hWnd );

	//	Note : 디버그 정보 초기화
	//
	CDebugSet::FinalCleanup();

	return S_OK;
}

HRESULT DxResponseMan::DoInterimClean ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxSkinCharDataContainer::GetInstance().CleanUp ();
	DxReplaceContainer::GetInstance().CleanUp ();
	DxSkinPieceContainer::GetInstance().CleanUp ();

	//DxSimpleMeshMan::GetInstance().CleanUp ();
	//DxBoneCollector::GetInstance().CleanUp ();
	//DxSkinAniMan::GetInstance().CleanUp ();
	//DxSkinMeshMan::GetInstance().CleanUp ( pd3dDevice );

	//	Note : 바이너리 스레드 읽기 예약 내용 리셋.
	DxSkinAniMan::GetInstance().ClearBinLoadingData ();

	//	초기화.
	DxBoneCollector::GetInstance().DoInterimClean ( pd3dDevice );
	DxSkinAniMan::GetInstance().DoInterimClean ( pd3dDevice );
	DxSkinMeshMan::GetInstance().DoInterimClean ( pd3dDevice );
	
	DxEffSinglePropGMan::GetInstance().CleanUp ();
	DxEffGroupPlayer::GetInstance().RemoveAllEff ();

	return S_OK;
}

LRESULT DxResponseMan::MsgProc ( MSG* pMsg )
{
	return 0L;
}