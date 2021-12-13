#include "pch.h"
#include "ranparam.h"
#include "DxShadowMap.h"
#include "DxEffectMan.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundMan.h"
#include "../[Lib]__EngineSound/Sources/DxSound/BgmSound.h"
#include "DxSurfaceTex.h"
#include "DxGlowMan.h"
#include "DxFogMan.h"
#include "DxPostProcess.h"

#include "dxparamset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern BOOL				g_bFRAME_LIMIT;

namespace DXPARAMSET
{
	STESTSET& GetInstance()
	{
		static STESTSET Instance;
		return Instance;
	}

	void INIT ()
	{
		//	그래픽 옵션 조정.
		//
		DxShadowMap::GetInstance().SetShadowDetail ( (EMSHADOWDETAIL) RANPARAM::dwShadowChar );

		if ( RANPARAM::bShadowLand )	DxEffectMan::GetInstance().SetSSDetail ( SS_ONETEX );
		else							DxEffectMan::GetInstance().SetSSDetail ( SS_DISABLE );

		DxFogMan::GetInstance().SetFogRange ( (FOGRANGE) RANPARAM::dwFogRange );

		DxEffectMan::GetInstance().SetRealReflect ( RANPARAM::bRealReflect );
		DxEffectMan::GetInstance().SetSkinDetail ( (EMSKINDETAIL) RANPARAM::dwSkinDetail );
		DxSurfaceTex::GetInstance().SetOptionEnable ( RANPARAM::bRefract );
		DxGlowMan::GetInstance().SetOptionEnable ( RANPARAM::bGlow );
		DxPostProcess::GetInstance().SetOptionEnable( RANPARAM::bPost );
		
		g_bFRAME_LIMIT = RANPARAM::bFrameLimit;
		

		//	사운드 설정.
		//		
		DxSoundMan::GetInstance().SetStereo ( !RANPARAM::b3D_SOUND );
		DxSoundMan::GetInstance().SetMapVolume ( RANPARAM::nSndMap );
		DxSoundMan::GetInstance().SetSfxVolume ( RANPARAM::nSndSfx );
		DxBgmSound::GetInstance().SetVolume ( RANPARAM::nSndMusic );
		DxSoundMan::GetInstance().Set3DAlgorithm ( RANPARAM::dw3DAlgorithm );
		DxSoundMan::GetInstance().SetBufferSize ( RANPARAM::dwSndBufferSize );

		DxSoundMan::GetInstance().SetMapMute ( RANPARAM::bSndMuteMap );
		DxSoundMan::GetInstance().SetSfxMute ( RANPARAM::bSndMuteSfx );
		DxBgmSound::GetInstance().SetMute ( RANPARAM::bSndMuteMusic );

		LOADDATA();
	}

	void LOADDATA ()
	{
		GetInstance().m_bDIS_CONFT = RANPARAM::bDIS_CONFT;
		GetInstance().m_bDIS_TRADE = RANPARAM::bDIS_TRADE;
		GetInstance().m_bDIS_PARTY = RANPARAM::bDIS_PARTY;
//		GetInstance().m_bDIS_FRIEND = RANPARAM::bDIS_FRIEND;		

		GetInstance().m_bSHOW_SIMPLEHP = RANPARAM::bSHOW_SIMPLEHP;

		GetInstance().m_dwScrWidth = RANPARAM::dwScrWidth;
		GetInstance().m_dwScrHeight = RANPARAM::dwScrHeight;
		GetInstance().m_emScrFormat = RANPARAM::emScrFormat;

		GetInstance().m_uScrRefreshHz = RANPARAM::uScrRefreshHz;
		GetInstance().m_bScrWindowed = RANPARAM::bScrWindowed;

		GetInstance().m_dwVideoLevel = RANPARAM::dwVideoLevel;		
		GetInstance().m_dwShadowChar = RANPARAM::dwShadowChar;
		GetInstance().m_bBuff = RANPARAM::bBuff;
		GetInstance().m_bShadowLand = RANPARAM::bShadowLand;
		GetInstance().m_bRealReflect = RANPARAM::bRealReflect;
		GetInstance().m_dwSkinDetail = RANPARAM::dwSkinDetail;
		GetInstance().m_bGlow = RANPARAM::bGlow;
		GetInstance().m_bPost = RANPARAM::bPost;
		GetInstance().m_bRefract = RANPARAM::bRefract;
		GetInstance().m_dwFogRange = RANPARAM::dwFogRange;
		GetInstance().m_bFrameLimit = RANPARAM::bFrameLimit;
	}

	void STESTSET::APPLY ( CD3DApplication *pD3dApp )
	{
		//	스크린 설정 적용.		
		DWORD dwNewMode = pD3dApp->FindDeviceMode ( m_dwScrWidth, m_dwScrHeight, m_emScrFormat );
		DWORD dwOldMode = pD3dApp->GetCurrentMode();
		if ( dwNewMode != dwOldMode )
		{
			pD3dApp->ChangeDeviceMode ( dwNewMode, m_uScrRefreshHz, m_bScrWindowed );
		}

		//	그래픽 설정.
		//
		DxShadowMap::GetInstance().SetShadowDetail ( (EMSHADOWDETAIL) m_dwShadowChar );

		if ( m_bShadowLand )	DxEffectMan::GetInstance().SetSSDetail ( SS_ONETEX );
		else					DxEffectMan::GetInstance().SetSSDetail ( SS_DISABLE );

		DxEffectMan::GetInstance().SetRealReflect ( m_bRealReflect );
		DxSurfaceTex::GetInstance().SetOptionEnable ( m_bRefract );
		DxGlowMan::GetInstance().SetOptionEnable ( m_bGlow );
		DxPostProcess::GetInstance().SetOptionEnable( m_bPost );
		DxEffectMan::GetInstance().SetSkinDetail ( (EMSKINDETAIL) m_dwSkinDetail );

		DxFogMan::GetInstance().SetFogRange ( (FOGRANGE) m_dwFogRange );
		
		g_bFRAME_LIMIT = m_bFrameLimit;
		
	}

	void STESTSET::VideoOK( CD3DApplication *pD3dApp )
	{
		RANPARAM::dwScrWidth = m_dwScrWidth;
		RANPARAM::dwScrHeight = m_dwScrHeight;
		RANPARAM::emScrFormat = m_emScrFormat;
		RANPARAM::uScrRefreshHz = m_uScrRefreshHz;
		RANPARAM::bScrWindowed = m_bScrWindowed;

		RANPARAM::dwVideoLevel = m_dwVideoLevel;
		RANPARAM::dwShadowChar = m_dwShadowChar;
		RANPARAM::bBuff = m_bBuff;
		RANPARAM::bShadowLand = m_bShadowLand;
		RANPARAM::bRealReflect = m_bRealReflect;
		RANPARAM::dwSkinDetail = m_dwSkinDetail;
		RANPARAM::bGlow = m_bGlow;
		RANPARAM::bPost = m_bPost;
		RANPARAM::bRefract = m_bRefract;
		RANPARAM::bFrameLimit = m_bFrameLimit;

		RANPARAM::dwFogRange = m_dwFogRange;
		

		RANPARAM::SAVE ();
		RANPARAM::SAVE_GAMEOPT ();

		APPLY ( pD3dApp );
	}

	void STESTSET::GameOK()
	{
		RANPARAM::bDIS_CONFT = m_bDIS_CONFT;
		RANPARAM::bDIS_TRADE = m_bDIS_TRADE;
		RANPARAM::bDIS_PARTY = m_bDIS_PARTY;
		//		RANPARAM::bDIS_FRIEND = m_bDIS_FRIEND;		

		RANPARAM::bSHOW_SIMPLEHP = m_bSHOW_SIMPLEHP;

		RANPARAM::SAVE ();
		RANPARAM::SAVE_GAMEOPT ();
	}

	void STESTSET::AudioOK()
	{
		RANPARAM::SAVE ();
		RANPARAM::SAVE_GAMEOPT ();
	}

	void STESTSET::CANCEL ( CD3DApplication *pD3dApp )
	{
		m_bDIS_CONFT = RANPARAM::bDIS_CONFT;
		m_bDIS_TRADE = RANPARAM::bDIS_TRADE;
		m_bDIS_PARTY = RANPARAM::bDIS_PARTY;
//		m_bDIS_FRIEND = RANPARAM::bDIS_FRIEND;


		m_bSHOW_SIMPLEHP = RANPARAM::bSHOW_SIMPLEHP;	

		m_dwScrWidth = RANPARAM::dwScrWidth;
		m_dwScrHeight = RANPARAM::dwScrHeight;
		m_emScrFormat = RANPARAM::emScrFormat;
		m_uScrRefreshHz = RANPARAM::uScrRefreshHz;
		m_bScrWindowed = RANPARAM::bScrWindowed;

		m_dwVideoLevel = RANPARAM::dwVideoLevel;				
		m_dwShadowChar = RANPARAM::dwShadowChar;
		m_bBuff = RANPARAM::bBuff;
		m_bShadowLand = RANPARAM::bShadowLand;
		m_bRealReflect = RANPARAM::bRealReflect;
		m_dwSkinDetail = RANPARAM::dwSkinDetail;
		m_bGlow = RANPARAM::bGlow;
		m_bPost = RANPARAM::bPost;
		m_bRefract = RANPARAM::bRefract;
		m_bFrameLimit = RANPARAM::bFrameLimit;

		m_dwFogRange = RANPARAM::dwFogRange;

		APPLY ( pD3dApp );
	}
};

