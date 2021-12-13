#include "pch.h"
#include <process.h>
#include "LoadingThread.h"

#include "SubPath.h"
#include "DxGrapUtils.h"
#include "DxResponseMan.h"
#include "UITextControl.h"
#include "GameTextControl.h"

#include "../[Lib]__Engine/Sources/DxTools/TextureManager.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace NLOADINGTHREAD;

//	-------------------- [ CUSTOM VERTEX 설정 ] --------------------
const	DWORD TEXTUREVERTEXFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
struct TEXTUREVERTEX
{
	union
	{
		struct { D3DXVECTOR4 vPos; };
		struct { float x, y, z, rhw; };
	};

	DWORD Diffuse;

	union
	{
		struct { D3DXVECTOR2 vUV; };
		struct { float tu, tv; };
	};

	TEXTUREVERTEX () :
		x(0.0f),
		y(0.0f),
		z(0.0f),
		rhw(1.0f),
		Diffuse(0xFFFFFFFF),
		tu(0.0f),
		tv(0.0f)
	{		
	}

	TEXTUREVERTEX ( float _x, float _y, float _tu, float _tv ) :
		x(_x),
		y(_y),
		z(0.0f),
		rhw(1.0f),
		Diffuse(0xFFFFFFFF),
		tu(_tu),
		tv(_tv)
	{		
	}

	void	SetPos ( float _x, float _y )
	{
		x = _x;
		y = _y;
	}

	void	SetTexPos ( float _tu, float _tv )
	{
		tu = _tu;
		tv = _tv;
	}
};

//	-------------------- [ CUSTOM VERTEX 제어 메소드 설정 ] --------------------

static HRESULT	CreateVB ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ& pTextureVB, TEXTUREVERTEX VertexArray[6] )
{	
	HRESULT hr = S_OK;
	hr = pd3dDevice->CreateVertexBuffer( 6*sizeof(TEXTUREVERTEX), 0, TEXTUREVERTEXFVF,
											D3DPOOL_MANAGED, &pTextureVB, NULL );
    if( FAILED ( hr ) )	return hr;    

    VOID* pVertices;
	hr = pTextureVB->Lock( 0, sizeof ( TEXTUREVERTEX ) * 6, (VOID**)&pVertices, 0 );
    if( FAILED ( hr ) ) return hr;

    memmove( pVertices, VertexArray, sizeof ( TEXTUREVERTEX ) * 6 );

    hr = pTextureVB->Unlock();
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

static void SetVertexPos ( TEXTUREVERTEX VertexArray[6], float LEFT, float TOP, float SIZEX, float SIZEY )
{
	float RIGHT = LEFT + SIZEX;
	float BOTTOM = TOP + SIZEY;

	VertexArray[0].SetPos ( LEFT,	TOP );
	VertexArray[1].SetPos ( RIGHT,	TOP);
	VertexArray[2].SetPos ( LEFT,	BOTTOM );

	VertexArray[3].SetPos ( LEFT,	BOTTOM );
	VertexArray[4].SetPos ( RIGHT,	TOP);
	VertexArray[5].SetPos ( RIGHT,	BOTTOM );
}

static void SetTexturePos ( TEXTUREVERTEX VertexArray[6], float LEFT, float TOP, float SIZEX, float SIZEY, float TEX_SIZEX, float TEX_SIZEY )
{
	float RIGHT = LEFT + SIZEX;
	float BOTTOM = TOP + SIZEY;

	VertexArray[0].SetTexPos ( LEFT / TEX_SIZEX,	TOP / TEX_SIZEY );
	VertexArray[1].SetTexPos ( RIGHT / TEX_SIZEX,	TOP / TEX_SIZEY);
	VertexArray[2].SetTexPos ( LEFT / TEX_SIZEX,	BOTTOM / TEX_SIZEY );

	VertexArray[3].SetTexPos ( LEFT / TEX_SIZEX,	BOTTOM / TEX_SIZEY );
	VertexArray[4].SetTexPos ( RIGHT / TEX_SIZEX,	TOP / TEX_SIZEY );
	VertexArray[5].SetTexPos ( RIGHT / TEX_SIZEX,	BOTTOM / TEX_SIZEY );
}

static HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pLoadingTexture, LPDIRECT3DVERTEXBUFFERQ pTextureVB )
{
	HRESULT hr = S_OK;

	hr = pd3dDevice->SetTexture ( 0, pLoadingTexture );
	if ( FAILED ( hr ) ) return hr;

    hr = pd3dDevice->SetStreamSource( 0, pTextureVB, 0, sizeof(TEXTUREVERTEX) );
	if ( FAILED ( hr ) ) return hr;

	hr = pd3dDevice->SetFVF( TEXTUREVERTEXFVF );
	if ( FAILED ( hr ) ) return hr;

	hr = pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
	if ( FAILED ( hr ) ) return hr;

	hr = pd3dDevice->SetTexture ( 0, NULL );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

BOOL MessagePump()
{
   MSG msg;

   while(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
   {
       if(AfxGetApp() -> PumpMessage())
       {
             ::PostQuitMessage(0);
             return FALSE;
       }
   }

   return TRUE;
}

unsigned int WINAPI	LoadingThread( LPVOID pData )
{
	if( n_strTextureName.IsEmpty() || n_strTextureName == _T("null") )
	{
		srand ( (UINT)time( NULL ) );
		int nIndex = (rand() % 2) + 11;

		n_strTextureName.Format( "loading_%03d.tga", nIndex );	
	}

	TCHAR szTexture[256] = {0};
	StringCchPrintf ( szTexture, 256, n_strTextureName.GetString(), n_szAppPath, SUBPATH::TEXTURE_FILE_ROOT );

	LPDIRECT3DDEVICEQ& pd3dDevice = *n_ppd3dDevice;
	LPDIRECT3DTEXTUREQ pLoadingTexture = NULL;
	LPDIRECT3DTEXTUREQ pOVER15Tex = NULL;
	LPDIRECT3DTEXTUREQ pCopyRightTex = NULL;

	HRESULT hr = S_OK;
	hr = TextureManager::LoadTexture( szTexture, pd3dDevice, pLoadingTexture, 0, 0 );
	if ( FAILED ( hr ) )
	{		
		DeWait ();
		n_ExitState = eERROR;
		return 0;
	}

	std::string szOver15Name = "OVER15.tga";
	if ( n_bOVER15 )
	{
		hr = TextureManager::LoadTexture( szOver15Name.c_str(), pd3dDevice, pOVER15Tex, 0, 0 );
		if ( FAILED ( hr ) )
		{		
			DeWait ();
			n_ExitState = eERROR;
			return 0;
		}
	}

	//std::string szCopyRight = "CopyRight.tga";
	//hr = TextureManager::LoadTexture( szCopyRight.c_str(), pd3dDevice, pCopyRightTex, 0, 0 );
	//if ( FAILED ( hr ) )
	//{		
	//	DeWait ();
	//	n_ExitState = eERROR;
	//	return 0;
	//}

	///////////////////////////////////////////////////////////////////////////////
	//	[TOGGLE SWITCH] 텍스쳐 로딩완료
	DeWait ();
	///////////////////////////////////////////////////////////////////////////////

	RECT rect;
	GetClientRect ( n_hWnd, &rect );
	
	const float fWidth = float(rect.right - rect.left);
	const float fHeight = float(rect.bottom - rect.top);

	const float	fRealImageX = 1024.0f;
	const float	fRealImageY = 768.0f;
	const float	fImageSize = 1023.0f;

	const float fWidthRatio = fRealImageX / fImageSize;
	const float fHeightRatio= fRealImageY / fImageSize;

	const float LeftPos  = 0.0f;
	const float TopPos   = 0.0f;
	const float RightPos = fWidth;
	const float BottomPos= fHeight;

	float ROOT_LEFT		= LeftPos;
	float ROOT_TOP		= TopPos;
	float ROOT_SIZEX	= fWidth;
	float ROOT_SIZEY	= fHeight;

	//const FLOAT fCopyRightImageSizeX = 1023.0f;
	//const FLOAT fCopyRightImageSizeY = 15.0f;

	const D3DXVECTOR2 vProgressBarTex ( 0, 769 );
	const D3DXVECTOR2 vProgressBarBackTex ( 0, 791 );
	const D3DXVECTOR2 vProgressBarSize ( 582, 9 );
	const D3DXVECTOR2 TextureRenderPos ( 215, 584 );	

	//const D3DXVECTOR2 vCopyrightTex ( 0, 0 );
	//const D3DXVECTOR2 vCopyrightSize ( 1024, 16 );

	const D3DXVECTOR2 vOver15 ( 940, 20 );
	const D3DXVECTOR2 vOver15Size ( 64, 64 );
	const D3DXVECTOR2 vOver15Tex ( 0, 0 );	
	const D3DXVECTOR2 vOver15_800 ( 716, 20 );

	D3DXVECTOR2 vProgressBarAlignSize;
	vProgressBarAlignSize.x = static_cast<float>(floor(vProgressBarSize.x * fWidth / fRealImageX));
	vProgressBarAlignSize.y = static_cast<float>(floor(vProgressBarSize.y * fHeight/ fRealImageX));

	D3DXVECTOR2 vProgressBarAlign;
	vProgressBarAlign.x = (fWidth - vProgressBarAlignSize.x)/2.0f;
	vProgressBarAlign.y = static_cast<float>(floor(TextureRenderPos.y * fHeight/ fRealImageY));

	D3DXVECTOR2 vCopyrightAlign;
	//vCopyrightAlign.x = fWidth - (vCopyrightSize.x + 15.0f);
	vCopyrightAlign.x = 15.0f;
	vCopyrightAlign.y = fHeight - 30.0f;
	
	D3DXVECTOR2 vOver15Align;
	vOver15Align.x = fWidth - (vOver15Size.x+15.0f);
	vOver15Align.y = 15.0f;

	//	밑 배경
	LPDIRECT3DVERTEXBUFFERQ pTextureVB        = NULL; // Buffer to hold vertices
	{
		TEXTUREVERTEX VertexArray[6];
		SetVertexPos ( VertexArray, ROOT_LEFT, ROOT_TOP, ROOT_SIZEX, ROOT_SIZEY );
		SetTexturePos( VertexArray, 0.0f, 0.0f, fRealImageX, fRealImageY, fImageSize, fImageSize );

		if ( FAILED ( CreateVB ( pd3dDevice, pTextureVB, VertexArray ) ) )
		{
			n_ExitState = eERROR;
			return 0;
		}
	}

	//	프로그래스바 바닥
	LPDIRECT3DVERTEXBUFFERQ pProgressBackVB;
	{
		TEXTUREVERTEX VertexArray[6];
		SetVertexPos ( VertexArray, ROOT_LEFT + vProgressBarAlign.x, ROOT_TOP + vProgressBarAlign.y, vProgressBarAlignSize.x, vProgressBarSize.y );
		SetTexturePos( VertexArray, vProgressBarBackTex.x, vProgressBarBackTex.y, vProgressBarSize.x, vProgressBarSize.y, fImageSize, fImageSize );

		if ( FAILED ( CreateVB ( pd3dDevice, pProgressBackVB, VertexArray ) ) )
		{
			n_ExitState = eERROR;
			return 0;
		}
	}

	////	카피라이트
	//LPDIRECT3DVERTEXBUFFERQ pCopyrightVB;
	//{
	//	TEXTUREVERTEX VertexArray[6];
	//	SetVertexPos ( VertexArray, ROOT_LEFT + vCopyrightAlign.x, ROOT_TOP + vCopyrightAlign.y, vCopyrightSize.x, vCopyrightSize.y );
	//	SetTexturePos( VertexArray, vCopyrightTex.x, vCopyrightTex.y, vCopyrightSize.x, vCopyrightSize.y, fCopyRightImageSizeX, fCopyRightImageSizeY );

	//	if ( FAILED ( CreateVB ( pd3dDevice, pCopyrightVB, VertexArray ) ) )
	//	{
	//		n_ExitState = eERROR;
	//		return 0;
	//	}
	//}

	//	프로그래스 바
	LPDIRECT3DVERTEXBUFFERQ pProgressBarVB;
	{
		TEXTUREVERTEX VertexArray[6];
		SetVertexPos ( VertexArray, ROOT_LEFT + vProgressBarAlign.x, ROOT_TOP + vProgressBarAlign.y, vProgressBarAlignSize.x, vProgressBarSize.y );
		SetTexturePos( VertexArray, vProgressBarTex.x, vProgressBarTex.y, vProgressBarSize.x, vProgressBarSize.y, fImageSize, fImageSize );

		if ( FAILED ( CreateVB ( pd3dDevice, pProgressBarVB, VertexArray ) ) )
		{
			n_ExitState = eERROR;
			return 0;
		}
	}

	//	15금표시
	LPDIRECT3DVERTEXBUFFERQ pOver15VB;
	{
		TEXTUREVERTEX VertexArray[6];
		SetVertexPos ( VertexArray, ROOT_LEFT + vOver15Align.x, ROOT_TOP + vOver15Align.y, vOver15Size.x, vOver15Size.y );
		SetTexturePos( VertexArray, vOver15Tex.x, vOver15Tex.y, vOver15Size.x, vOver15Size.y, vOver15Size.x, vOver15Size.y );

		if ( FAILED ( CreateVB ( pd3dDevice, pOver15VB, VertexArray ) ) )
		{
			n_ExitState = eERROR;
			return 0;
		}
	}

	// CopyRight 출력용 폰트에 크리티컬 섹션 사용 설정 ( 준혁 )
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	if( pFont )
		pFont->UsageCS( TRUE );

	//	뿌리기
	while ( n_bRender )
	{
		//	UPDATE
		{
			{
				const int nMAX_STEP = 11;
				int pri_Step = LOADINGSTEP::GETSTEP();

				float fPercent = static_cast<float>(pri_Step) / static_cast<float>(nMAX_STEP);
				float fTEX_SIZEX = (vProgressBarTex.x + vProgressBarSize.x) * fPercent / fImageSize;
				float fSIZEX = static_cast<float>(ceil((vProgressBarAlign.x + vProgressBarAlignSize.x) * fPercent));

				//	프로그래스바
				{
					VOID* pVertices;
					if( FAILED( pProgressBarVB->Lock( 0, sizeof ( TEXTUREVERTEX ) * 6, (VOID**)&pVertices, 0 ) ) )
					{
						n_ExitState = eERROR;
						if( pFont )
							pFont->UsageCS( FALSE );
						return 0;
					}
					TEXTUREVERTEX* pVerticesPart = (TEXTUREVERTEX*)pVertices;
					pVerticesPart[1].x = fSIZEX;
					pVerticesPart[1].tu = fTEX_SIZEX;
					pVerticesPart[4].x = fSIZEX;
					pVerticesPart[4].tu = fTEX_SIZEX;
					pVerticesPart[5].x = fSIZEX;
					pVerticesPart[5].tu = fTEX_SIZEX;

					pProgressBarVB->Unlock();
				}
			}
		}
		
		//if(!MessagePump())		break;

		HRESULT hr;
		if( FAILED( hr = pd3dDevice->TestCooperativeLevel() ) )
		{
			// If the device was lost, do not render until we get it back
			if( D3DERR_DEVICELOST == hr )
			{
				CDebugSet::ToListView ( "[ERROR] D3DERR_DEVICELOST _ LoadingThread() FAILED" );
				continue;
			}

			// Check if the device needs to be resized.
			if( D3DERR_DEVICENOTRESET == hr )
				continue;
		}

		hr = pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

		// Begin the scene
		if( SUCCEEDED( pd3dDevice->BeginScene() ) )
		{
			//	밑 바닥
			hr = Render ( pd3dDevice, pLoadingTexture, pTextureVB );

			DWORD dwAlphaBlendEnable;
			pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );

			pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
			pd3dDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			DWORD dwAlphaOP;
			pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,   &dwAlphaOP );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

			DWORD dwMin, dwMag, dwMip;
			pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER,	&dwMin );
			pd3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER,	&dwMag );
			pd3dDevice->GetSamplerState( 0, D3DSAMP_MIPFILTER,	&dwMip );

			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	D3DTEXF_POINT );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_NONE );

			//	프로그래스 바 바닥
			hr = Render ( pd3dDevice, pLoadingTexture, pProgressBackVB );

			//	카피라이트
			//hr = Render ( pd3dDevice, pLoadingTexture, pCopyrightVB );

			//	카피라이트
			//hr = Render ( pd3dDevice, pCopyRightTex, pCopyrightVB );

			if( pFont )
				pFont->DrawText( vCopyrightAlign.x, vCopyrightAlign.y, NS_UITEXTCOLOR::WHITE, ID2GAMEWORD("COPYRIGHT_COMPANY_LOAD") );

			//	프로그래스 바
			hr = Render ( pd3dDevice, pLoadingTexture, pProgressBarVB );

			//	15금 표시
			if ( n_bOVER15 ) hr = Render ( pd3dDevice, pOVER15Tex, pOver15VB );			

			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   dwAlphaOP );
			pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );

			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	dwMin );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	dwMag );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	dwMip );

			// End the scene.
			pd3dDevice->EndScene();
			pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}

		Sleep(1);
	}

	if( pFont )
		pFont->UsageCS( FALSE );

	SAFE_RELEASE ( pTextureVB );
	SAFE_RELEASE ( pProgressBackVB );
	SAFE_RELEASE ( pProgressBarVB );
	//SAFE_RELEASE ( pCopyrightVB );
	SAFE_RELEASE ( pOver15VB );

	TextureManager::ReleaseTexture( szTexture, pLoadingTexture );
	TextureManager::ReleaseTexture( szOver15Name.c_str(), pOVER15Tex );
	//TextureManager::ReleaseTexture( szCopyRight.c_str(), pCopyRightTex );

	///////////////////////////////////////////////////////////////////////////////
	//	[TOGGLE SWITCH] 로더 스레드 종료
	DeWait ();
	//////////////////////////////////////////////////////////////////////////////
	n_ExitState = eNORMAL;

	return 0;
}

namespace	NLOADINGTHREAD
{
	DWORD				n_dwThreadID;
	LPDIRECT3DDEVICEQ*	n_ppd3dDevice;
	HWND				n_hWnd;
	BOOL				n_bWait;
	BOOL				n_bRender;
	bool				n_bOVER15;
	char				n_szAppPath[MAX_PATH] = {0};
	int					n_ExitState;
	int					n_Step = 0;
	HANDLE				n_hThread = NULL;
	CString				n_strTextureName;

	HRESULT	StartThreadLOAD(LPDIRECT3DDEVICEQ* ppd3dDevice, 
							HWND hWnd, 
							char* szAppPath, 
							const CString & strTextureName, 
							bool bOVER15 )
	{
		n_bOVER15 = bOVER15;
		n_ppd3dDevice = ppd3dDevice;
		n_hWnd = hWnd;
		StringCchCopy ( n_szAppPath, MAX_PATH, szAppPath );
		n_strTextureName = strTextureName;
		
		n_bRender = TRUE;	//	렌더
		EnWait ();
		n_ExitState = eNORMAL;

		//n_hThread = CreateThread( NULL, 0, LoadingThread, NULL, 0, &n_dwThreadID );
		n_hThread = (HANDLE) ::_beginthreadex(
									NULL,
									0,
									LoadingThread,
									NULL,
									0,
									(unsigned int*) &n_dwThreadID );
		if ( !n_hThread )
		{
			DeWait();
			return E_FAIL;
		}

		return S_OK;
	}

	void	WaitThread ()
	{
		while ( GetWait () )
		{		
			Sleep( 1 );
		}
		EnWait ();
	}

	void	EndThread ()
	{		
		n_bRender = FALSE;

		if ( n_ExitState == eNORMAL )
		{
			WaitThread ();
		}

		CloseHandle( n_hThread );
		n_hThread = NULL;

		n_strTextureName.Empty();
	}

	BOOL	GetWait ()
	{
		return n_bWait;
	}

	void	DeWait  ()
	{
		n_bWait = FALSE;
	}

	void	EnWait ()
	{
		n_bWait = TRUE;
	}

	void	SetStep ( int step )
	{
		n_Step = step;
	}

	int		GetStep ()
	{
		return n_Step;
	}
};