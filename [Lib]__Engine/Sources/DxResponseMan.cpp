//----------------------------------------------------------------------------------------[DxResponseMan]
//
//		���� ��, ����Ŭ���̾�Ʈ���� �������� �����ؾ��ϴ� ��ġ���� ������.
//	
//		Render�� �������� �����Ƿ� �̺κп� ���ԵǴ� ����  �������� ���־��
//		�Ѵ�.
//		
//=========================================================
//
//		//	Note : Shadow Map Clear
//		//
//		DxShadowMap::GetInstance().ClearShadow ( m_pd3dDevice );
//
//=========================================================
//
//		//	Note : Light ����.
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

	//	Note : ������ ���� �ʱ�ȭ.
	seqrandom::init();

	//	Note : �������� ���� ����.
	StringCchCopy( SUBPATH::APP_ROOT, MAX_PATH, szAppPath );

	//	Memo :	����� ���� ������ �����Ǿ�� �ϴ� ���� ����.
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

	//	Note : ��ũ�� ĸ�� ���丮(���׸�)
	//
	DxGrapUtils::SetPath ( szFullPath );

	//	Note : �⺻ ���� ���� ����.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::CACHE );
	CreateDirectory ( szFullPath, NULL );

	//	Note : ����׻��� �ʱ�ȭ.
	//
	CDebugSet::OneTimeSceneInit ( szPROFILE, true );

	//	Note : Ű���� ���콺.
	//
	DxInputDevice::GetInstance().OneTimeSceneInit(m_pWndApp->m_hWnd);

	//	Note : cursor �⺻ ���� ����.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_EDIT );
	CCursor::GetInstance().OneTimeSceneInit(szFullPath,m_pWndApp->m_hWnd);

	//	Note : ��Ʈ �޴��� �ʱ�ȭ.
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

	//	Note : �ؽ��� �⺻ ���� ����.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::TEXTURE_FILE_ROOT );
	TextureManager::SetTexturePath ( szFullPath );

	//	Note : ShaderMan �⺻ ���� ����.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::SHADER_FILE_ROOT );
	DXShaderMan::GetInstance().SetPath ( szFullPath );

	//	Note : ReplaceContainer �⺻ ���� ����.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_PIECE );
	DxReplaceContainer::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : DxBoneCollector �⺻ ���� ����.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKELETON );
	DxBoneCollector::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : Animation �⺻ ���� ����.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_ANIMATION );
	DxSkinAniMan::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : SkinObject �⺻ ���� ����.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKINOBJECT );
	DxSkinObject::SetPath ( szFullPath );
	DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
	DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );
	
	// Note : SimpleMesh �⺻ ���� ����
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_OBJECT );

	DxSimpleMeshMan::GetInstance().SetPath ( szFullPath );

	//	Note : SkinMesh
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_SKIN );
	DxSkinMeshMan::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : ShaderMan �⺻ ���� ����.
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::EFF_FILE_SINGLE );
	DxEffSinglePropGMan::GetInstance().OneTimeSceneInit ( szFullPath );

	//	Note : DxLandMan
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::MAP_FILE );
	DxLandMan::SetPath ( szFullPath );

	//	Note : ��Ʈ�� ��ǲ �ν��Ͻ�.
	//
	DXInputString::GetInstance().Create(pWndApp,CRect(0,0,0,0));
	
	//	Note : GUI Directory
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::GUI_FILE_ROOT );
	CUIMan::SetPath( szFullPath );

	//	Note : ������ ���� ��� ã��.
	//
	TCHAR szPersonal[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPersonal, CSIDL_PERSONAL, FALSE);
	StringCchCat( szPersonal, MAX_PATH, _T("\\") );
	
	//	Note : Ŭ�� ��ũ ���丮(������)
	//
	DxClubMan::GetInstance().SetRegistPath ( szPersonal );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
	GLOGIC::SetPath ( szFullPath );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::GLOGIC_SERVER_FILE );
	GLOGIC::SetServerPath ( szFullPath );

	//	Note : ���� ���� �Ŵ��� �ʱ�ȭ
	//	
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::SOUND_FILE_ROOT );
	DxSoundMan::GetInstance().OneTimeSceneInit ( pWndApp->m_hWnd, szFullPath );

	//	Note : BGM ���� �ʱ�ȭ
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::BGM_FILE_ROOT );
	DxBgmSound::GetInstance().SetPath ( szFullPath );
	DxBgmSound::GetInstance().Run ( pWndApp->m_hWnd );

	//	Note : Char Skin Effect Directory
	StringCchCopy( szFullPath, MAX_PATH,  szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::EFF_FILE_CHAR );
	DxEffcharDataMan::GetInstance().SetPath ( szFullPath );

	//	Note : ���� �׸��� ����� ��ġ ����
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::TEXTURE_FILE_SHADOW );
	DxEffectShadow::SetPath ( szFullPath );

	//	Note : Ŭ�� �����͵��� ����� ��ġ ����
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::TEXTURE_FILE_CLUB );
	DxClubMan::SetPath ( szFullPath );

	// Note : Ÿ�� �����Ͱ� ����� ��ġ ����
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::TILE_FILE_DATA );
	DxMapEditMan::SetPathData ( szFullPath );

	// Note : Ÿ�� �ؽ��İ� ����� ��ġ ����
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::TILE_FILE_TEXTURE );
	DxMapEditMan::SetPathTex ( szFullPath );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::OBJ_FILE_PIECE );
	DxPieceContainer::SetPath( szFullPath );

	//	Note : �ؽ��� �⺻����.
	TextureManager::OneTimeSceneInit();

	// �ؽ�Ʈ �ؽ���
	CTextUtil::Get()->OneTimeSceneInit();

	// Note : 
	NSOCTREE::OneTimeSceneInit();

	// Note : CMemPool ����.
	DxEffSingleMan::GetInstance().OneTimeSceneInit();

	// Note : StaticMesh�� �÷���ȭ�� ���� Thread �⺻����.
	DxStaticMeshColor_THREAD::GetInstance().StartThread( m_pd3dDevice );

	// ���� ��Ʈ�� ���.
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
	
	// Note : ���� Param
	//RENDERPARAM::LOAD( m_pd3dDevice, m_szFullPath );

	//	���ϸ��̼� bin ���� �б�� ������ ����.
	DxSkinAniMan::GetInstance().StartBinFileLoadThread();

	//	Note : ShaderMan �⺻ ����.
	//
	DXShaderMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );

	//	Note : Ű���� ���콺.
	//
	DxInputDevice::GetInstance().InitDeviceObjects ( m_pd3dDevice, FALSE, bCAPTUREMOUSE );

	////	Note : ��Ʈ �޴��� �ʱ�ȭ.
	////
	//DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	//CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	//CD3DFontPar* pD3dFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );

	////	Note	:	����׼� ��� ���� �� �ʱ�ȭ
	//CDebugSet::InitDeviceObjects ( pD3dFont9 );

	// �ؽ�Ʈ �ؽ���
	SIZE sizeTexrure = { (LONG)m_d3dCaps.MaxTextureWidth, (LONG)m_d3dCaps.MaxTextureHeight };
	CTextUtil::Get()->InitDeviceObjects( m_pd3dDevice, &sizeTexrure );

	// UI ����ť
	CUIRenderQueue::Get()->InitDeviceObjects( m_pd3dDevice );

	//	Note : �ؽ��� �⺻����.
	TextureManager::StartThread( m_pd3dDevice );

	DxCursor::GetInstance().OnCreateDevice( m_pd3dDevice, m_pWndApp->m_hWnd );

	//	Note : ġȯ ���� �����̳� �ʱ�ȭ.
	NSCHARSG::OnCreateDevice( m_pd3dDevice );
	DxSkinMesh9::StaticCreate ( m_pd3dDevice );
	DxSkinMesh9_HLSL::StaticCreate ( m_pd3dDevice );
	DxReplaceContainer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxSkinPieceContainer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxSkinMeshMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxSimpleMeshMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	LOADINGSTEP::SETSTEP ( 1 );

	//	Note : ȿ�� ��ü.
	DxEffectMan::GetInstance().InitDeviceObjects ( m_pD3D, m_pd3dDevice, m_d3dsdBackBuffer );
	DxEffSinglePropGMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffGroupPlayer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffCharMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffAniMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffcharDataMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffKeepMan::GetInstance().InitDeviceObjects( m_pd3dDevice );
	DxEffProjMan::GetInstance().OnCreateDevice( m_pd3dDevice );
	LOADINGSTEP::SETSTEP ( 2 );

	DxSurfaceTex::GetInstance().InitDeviceObjects ( m_pD3D, m_pd3dDevice, m_d3dsdBackBuffer );	// �⺻ �����̽� 
	DxWeatherMan::GetInstance()->InitDeviceObjects ( m_pd3dDevice );							// ���� �޴��� �ʱ�ȭ.
	DxShadowMap::GetInstance().InitDeviceObjects ( m_pD3D, m_pd3dDevice, m_d3dsdBackBuffer );	// ������ �� �ʱ�ȭ
	DxEnvironment::GetInstance().InitDeviceObjects ( m_pD3D, m_pd3dDevice, m_d3dsdBackBuffer );	// �ݻ� �� ����
	DxCubeMap::GetInstance().InitDeviceObjects ( m_pD3D, m_pd3dDevice, m_d3dsdBackBuffer );		// ť��� �޴��� �ʱ�ȭ.
	DxSkyMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );									// �ϴ� �޴��� �ʱ�ȭ.
	DxClubMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );								// Ŭ�� �޴��� �ʱ�ȭ

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

	//	Note : ���� ���� �Ӽ� ����.
	//	
	SetRenderState ();

	//	Note : Set up lighting states
	//
	DxLightMan::GetInstance()->InitDeviceObjects ( m_pd3dDevice );

	//	Note : ��Ʈ �޴��� �ʱ�ȭ.
	//
	DxFontMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );

	// �ؽ�Ʈ �ؽ���
	CTextUtil::Get()->RestoreDeviceObjects( m_pd3dDevice );

	// UI ����ť
	CUIRenderQueue::Get()->RestoreDeviceObjects( m_pd3dDevice );

	DxCursor::GetInstance().OnResetDevice( m_pd3dDevice );

	DxPostProcess::GetInstance().RestoreDeviceObjects(m_pd3dDevice);		// ��ó��
	DxShadowMap::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );		// ������ ��
	DxEnvironment::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );		// �ݻ� �� ����
	DxSurfaceTex::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );		// �⺻ �����̽� 
	DxGlowMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// �۷ο� �� 
	DxWeatherMan::GetInstance()->RestoreDeviceObjects ( m_pd3dDevice );		// ���� �޴���
	DxCubeMap::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// ť��� �޴���
	DxSkyMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// �ϴ� �޴��� �ʱ�ȭ.
	DxFogMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// ���� �ʱ�ȭ
	DxDynamicVB::GetInstance().RestoreDeviceObjects( m_pd3dDevice );		// DynamicVB �ʱ�ȭ
	DxPieceEffMan::GetInstance().OnResetDevice( m_pd3dDevice );				// DxPieceEffMan - OnResetDevice
	OPTMManager::GetInstance().OnResetDevice( m_pd3dDevice );				// OPTMManager - OnResetDevice

	//	Note : ġȯ ���� �����̳� �ʱ�ȭ.
	DxSkinMesh9::StaticResetDevice ( m_pd3dDevice );
	DxSkinMesh9_HLSL::StaticResetDevice( m_pd3dDevice );
	DxSkinMesh9_NORMAL::StaticResetDevice( m_pd3dDevice );
	DxReplaceContainer::GetInstance().RestoreDeviceObjects ();
	DxSkinPieceContainer::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	DxSkinMeshMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	DxSimpleMeshMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );

	//	Note : ȿ�� ��ü.
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

	//	Note : Ű���� ���콺.
	//
	PROFILE_BEGIN("DxInputDevice,DXInputString::FrameMove");
	DxInputDevice::GetInstance().FrameMove ( fTime, fElapsedTime, bDefWin, bGame );
	PROFILE_END("DxInputDevice,DXInputString::FrameMove");

	//CCursor::GetInstance().FrameMove ( fTime, fElapsedTime );
	CCursor::GetInstance().FrameMove ( fTime, fElapsedTime );

	// Note : List�� ����ִ����� ����.
	DxStaticMeshColor_THREAD::GetInstance().CheckListEmpty();

	// Note 
	DxShadowMap::GetInstance().FrameMove ( fTime, fElapsedTime );

	// Note : DxPostProcess
	DxPostProcess::GetInstance().FrameMove();

	// Note : Character Shader Setting
	NSCHARHLSL::FrameMove();

	// Note : Character Scene Graph
	NSCHARSG::Reset();

	//// Note : Ŀ�� ���ư�.
	//DxCursor::GetInstance().FrameMove( fElapsedTime );

	//	Note : ġȯ ������ Ani-Time ����.
	//
	PROFILE_BEGIN("DxReplaceContainer::FrameMove");
	DxReplaceContainer::SetTime ( fTime, fElapsedTime );
	DxReplaceContainer::GetInstance().FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxReplaceContainer::FrameMove");

	//	Note : ����׼�
	//
	PROFILE_BEGIN("CDebugSet::FrameMove");
	CDebugSet::FrameMove ( fTime, fElapsedTime );
	PROFILE_END("CDebugSet::FrameMove");

	//	Note : �� ������Ʈ.
	//
	PROFILE_BEGIN("DxLightMan::FrameMove");
	DxLightMan::GetInstance()->FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxLightMan::FrameMove");

	//	Note : ���� �޴��� 
	//
	PROFILE_BEGIN("DxWeatherMan::FrameMove");
	DxWeatherMan::GetInstance()->FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxWeatherMan::FrameMove");

	//	Note : Effect ������Ʈ.
	//
	PROFILE_BEGIN("DxEffGroupPlayer::FrameMove");
	DxEffGroupPlayer::GetInstance().FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxEffGroupPlayer::FrameMove");

	//	Note : ī�޶� ���⿡ ���� �Ҹ���ȭ �ݿ�	
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

	//	Note : ��ũ�� ĸ��.
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

	//	Note : ��Ʈ �޴��� 
	//
	DxFontMan::GetInstance().InvalidateDeviceObjects ();

	// �ؽ�Ʈ �ؽ���
	CTextUtil::Get()->InvalidateDeviceObjects();

	// UI ����ť
	CUIRenderQueue::Get()->InvalidateDeviceObjects();

	// Cursor
	DxCursor::GetInstance().OnLostDevice();

	DxPostProcess::GetInstance().InvalidateDeviceObjects();					// ��ó��
	DxWeatherMan::GetInstance()->InvalidateDeviceObjects ( m_pd3dDevice );	// ���� �޴���
	DxShadowMap::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );	// ������ ��
	DxEnvironment::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );	// �ݻ� �� ����
	DxSurfaceTex::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );	// �⺻ �����̽� 
	DxGlowMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// �۷ο� �� 
	DxCubeMap::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// ť���
	DxSkyMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// �ϴ� �Ŵ���
	DxFogMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// ����
	DxDynamicVB::GetInstance().InvalidateDeviceObjects();					// DynamicVB : OnLostDevice
	DxPieceEffMan::GetInstance().OnLostDevice( m_pd3dDevice );				// DxPieceEffMan : OnLostDevice
	OPTMManager::GetInstance().OnLostDevice();								// OPTMManager - OnLostDevice

	//	Note : ġȯ ���� �����̳� �ʱ�ȭ.
	//
	DxSkinMesh9::StaticLost();
	DxSkinMesh9_HLSL::StaticLost();
	DxSkinMesh9_NORMAL::StaticLost();
	DxReplaceContainer::GetInstance().InvalidateDeviceObjects ();
	DxSkinPieceContainer::GetInstance().InvalidateDeviceObjects ();
	DxSkinMeshMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );

	//	Note : ȿ�� ��ü.
	DxEffectMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );
	DxEffSingleMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );
	DxEffSinglePropGMan::GetInstance().InvalidateDeviceObjects ();
	DxEffGroupPlayer::GetInstance().InvalidateDeviceObjects ();
	DxEffCharMan::GetInstance().InvalidateDeviceObjects ();
	DxEffAniMan::GetInstance().InvalidateDeviceObjects ();
	DxEffKeepMan::GetInstance().InvalidateDeviceObjects();
	DxTexEffMan::GetInstance().OnReleaseDevice( m_pd3dDevice );
	DxEffProjMan::GetInstance().OnLostDevice( m_pd3dDevice );

	//	Note : SimpleMesh�� ����� ��ü���� Invalidate �� ȣ���Ѵ�.
	DxSimpleMeshMan::GetInstance().InvalidateDeviceObjects ();

	DxRenderStates::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );

	return S_OK;
}

HRESULT DxResponseMan::DeleteDeviceObjects ()
{
	if ( !m_pd3dDevice )	return S_FALSE;

	//	Note : ���̳ʸ� ������ �б� ���� ���� ����.
	DxSkinAniMan::GetInstance().ClearBinLoadingData ();

	//	���ϸ��̼� bin ���� �б�� ������ ����.
	DxSkinAniMan::GetInstance().EndBinFileLoadThread();

	//	Note : Ű���� ���콺.
	//
	DxInputDevice::GetInstance().DeleteDeviceObjects ();

	//	Note : ��Ʈ �޴��� �ʱ�ȭ.
	//
	DxFontMan::GetInstance().DeleteDeviceObjects ();

	// �ؽ�Ʈ �ؽ���
	CTextUtil::Get()->DeleteDeviceObjects();

	// UI ����ť
	CUIRenderQueue::Get()->DeleteDeviceObjects();

	// Cursor
	DxCursor::GetInstance().OnDestroyDevice();

	//	Note : ġȯ ���� �����̳� �ʱ�ȭ.
	NSCHARSG::OnDestroyDevice();
	DxSkinMesh9::StaticDestroy();
	DxSkinMesh9_HLSL::StaticDestroy();
	DxReplaceContainer::GetInstance().DeleteDeviceObjects ();
	DxSkinPieceContainer::GetInstance().DeleteDeviceObjects ();
	DxSkinMeshMan::GetInstance().DeleteDeviceObjects ();

	//	Note : ȿ�� ��ü.
	//
	DxEffectMan::GetInstance().DeleteDeviceObjects ();
	DxEffSinglePropGMan::GetInstance().DeleteDeviceObjects ();
	DxEffGroupPlayer::GetInstance().DeleteDeviceObjects ();
	DxEffCharMan::GetInstance().DeleteDeviceObjects ();
	DxEffAniMan::GetInstance().DeleteDeviceObjects ();
	DxEffKeepMan::GetInstance().DeleteDeviceObjects();
	DxEffProjMan::GetInstance().OnDestroyDevice();

	DxSurfaceTex::GetInstance().DeleteDeviceObjects ();		// �⺻ �����̽� 
	DxWeatherMan::GetInstance()->DeleteDeviceObjects ();	// �ݻ� �� ����
	DxShadowMap::GetInstance().DeleteDeviceObjects ();		// ������ ��
	DxEnvironment::GetInstance().DeleteDeviceObjects ();	// �ݻ� �� ����
	DxCubeMap::GetInstance().DeleteDeviceObjects ();		// ť���
	DxSkyMan::GetInstance().DeleteDeviceObjects ();			// �ϴ� �Ŵ���	
	DxClubMan::GetInstance().DeleteDeviceObjects ();		// Ŭ�� �޴���

	//	Note : SimpleMesh�� ����� ��ü���� Delete �� ȣ���Ѵ�.
	DxSimpleMeshMan::GetInstance().DeleteDeviceObjects ();

	//	Note : �ؽ��� �޴��� ���� ( ���� �ϴܿ� �����Ͽ��� ��. )
	TextureManager::DeleteAllTexture();
	TextureManager::CleanUp();

	//	Note : ������ D3D�� ī��Ʈ�� ���ҽ�Ŵ.
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
	

	//	Note : ��Ʈ �޴��� �ʱ�ȭ.
	DxFontMan::GetInstance().DeleteDxFontAll ();

	DxLightMan::GetInstance()->CleanUp ();

	//	Note : ġȯ ���� �����̳� �ʱ�ȭ.
	//
	DxSkinCharDataContainer::GetInstance().CleanUp ();
	DxReplaceContainer::GetInstance().FinalCleanup ();
	DxSkinPieceContainer::GetInstance().FinalCleanup ();
	DxBoneCollector::GetInstance().CleanUp ();
	DxSkinAniMan::GetInstance().CleanUp ();
	DxPieceContainer::GetInstance().CleanUp();

	// Note : StaticMesh�� �÷���ȭ�� ���� Thread �⺻����.
	DxStaticMeshColor_THREAD::GetInstance().EndThread();

	// �ؽ�Ʈ �ؽ���
	CTextUtil::CleanUp();

	// Note : CUIRenderQueue �� �ϸ� leak
	CUIRenderQueue::CleanUp();

	// �������� ����ϴ� �� ��Ʈ�� ����
	CCommonWeb::CleanUp();

	// Note : CMemPool ����.
	DxEffSingleMan::GetInstance().FinalCleanup();

	//	Note : ���� ��ü ������
	//
	DxSoundLib::GetInstance()->ReleaseInstance ();
	DxSoundMan::GetInstance().FinalCleanup ();	

	// Note : 
	NSOCTREE::CleanUp();

	// ���� ��Ʈ�� ���.
	if( m_pWndApp != NULL )
		GammaControl::GetInstance().RollBack( m_pWndApp->m_hWnd );

	//	Note : ����� ���� �ʱ�ȭ
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

	//	Note : ���̳ʸ� ������ �б� ���� ���� ����.
	DxSkinAniMan::GetInstance().ClearBinLoadingData ();

	//	�ʱ�ȭ.
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