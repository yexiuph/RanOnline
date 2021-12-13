#include "pch.h"
#include <process.h>
#include "LoadingMapThread.h"
#include "../[Lib]__Engine/Sources/DxTools/TextureManager.h"
#include "SubPath.h"
#include "dxgraputils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace N_LOADING_MAP_THREAD;

unsigned int WINAPI	LoadingMapThread(
	LPVOID pData )
{
	S_LOADING_MAP_DATA* const pLOADING_MAP_DATA = (S_LOADING_MAP_DATA*)pData;
	const int& nLOADTYPE = pLOADING_MAP_DATA->nLOADTYPE;

	CString strTexture;
	float	fRealImageX = 1023.0f;
	float	fRealImageY = 768.0f;

	float	fImageSize = 1023.0f;

	switch ( nLOADTYPE )
	{
	case E_GENERALTYPE:
		{
			strTexture = "mid_loading.tga";

			fRealImageX = 1023.0f;
			fRealImageY = 768.0f;

			fImageSize = 1023.0f;
		}
		break;

	case E_BUSTYPE:
		{
			strTexture = "bus_mid_loading.tga";

			fRealImageX = 800.0f;
			fRealImageY = 600.0f;

			fImageSize = 1023.0f;
		}
		break;
	}
    
	LPDIRECT3DDEVICEQ& pd3dDevice = *n_ppd3dDevice;
	LPDIRECT3DTEXTUREQ pLoadingTexture = NULL;

	HRESULT hr = S_OK;
	hr = TextureManager::LoadTexture( strTexture.GetString (), pd3dDevice, pLoadingTexture, 0, 0 );
	if ( FAILED ( hr ) )
	{		
		DeWait ();
		n_ExitState = eERROR;
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////////
	//	[TOGGLE SWITCH] 텍스쳐 로딩완료
	DeWait ();
	///////////////////////////////////////////////////////////////////////////////

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

		TEXTUREVERTEX ( float _x, float _y, float _z,
			float _rhw, DWORD _Diffuse, float _tu, float _tv )
		{
			x = _x; y = _y; z = _z;	rhw = _rhw;
			Diffuse = _Diffuse; tu = _tu; tv = _tv;
		}
	};

	RECT rect;
	GetWindowRect ( n_hWnd, &rect );
	
	const float fWidth = float(rect.right - rect.left);
	const float fHeight = float(rect.bottom - rect.top);

	const float fWidthRatio = fRealImageX / fImageSize;
	const float fHeightRatio= fRealImageY / fImageSize;

	//const float LeftPos  = (fWidth - fRealImageX) / 2;
	//const float TopPos   = (fHeight- fRealImageY) / 2;
	//const float RightPos = (LeftPos+ fRealImageX);
	//const float BottomPos= (TopPos + fRealImageY);

	const float LeftPos  = 0.0f;
	const float TopPos   = 0.0f;
	const float RightPos = fWidth;
	const float BottomPos= fHeight;

    TEXTUREVERTEX Vertices[6] =
    {
        TEXTUREVERTEX ( LeftPos,  TopPos, 0.0f, 1.0f,	0xFFFFFFFF, 0.0f, 0.0f ), // x, y, z, rhw, color, tu, tv
        TEXTUREVERTEX ( RightPos, TopPos, 0.0f, 1.0f,	0xFFFFFFFF, fWidthRatio, 0.0f ),
        TEXTUREVERTEX ( LeftPos, BottomPos, 0.0f, 1.0f,	0xFFFFFFFF, 0.0f, fHeightRatio ),

        TEXTUREVERTEX ( LeftPos, BottomPos, 0.0f, 1.0f,	0xFFFFFFFF, 0.0f, fHeightRatio ),
        TEXTUREVERTEX ( RightPos, TopPos, 0.0f, 1.0f,	0xFFFFFFFF, fWidthRatio, 0.0f ),
        TEXTUREVERTEX ( RightPos, BottomPos, 0.0f, 1.0f,0xFFFFFFFF, fWidthRatio, fHeightRatio )
    };	

	LPDIRECT3DVERTEXBUFFERQ pTextureVB        = NULL; // Buffer to hold vertices
    if( FAILED( pd3dDevice->CreateVertexBuffer( 6*sizeof(TEXTUREVERTEX),
                                                  0, TEXTUREVERTEXFVF,
                                                  D3DPOOL_MANAGED, &pTextureVB, NULL ) ) )
    {
		n_ExitState = eERROR;
        return 0;
    }

    VOID* pVertices;
    if( FAILED( pTextureVB->Lock( 0, 6*sizeof(TEXTUREVERTEX), (VOID**)&pVertices, 0 ) ) )
	{
		n_ExitState = eERROR;
        return 0;
	}
    memmove( pVertices, Vertices, 6*sizeof(TEXTUREVERTEX) );
    pTextureVB->Unlock();
    
	//	뿌리기
	while ( n_bRender )
	{
		Sleep( 1 );

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
			{
				continue;
			}
		}

		hr = pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,
						D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

		// Begin the scene
		if( SUCCEEDED( pd3dDevice->BeginScene() ) )
		{
			//	Note : 텍스쳐 설정.
			//			
			pd3dDevice->SetTexture ( 0, pLoadingTexture );

			hr = pd3dDevice->SetStreamSource( 0, pTextureVB, 0, sizeof(TEXTUREVERTEX) );
			hr = pd3dDevice->SetFVF( TEXTUREVERTEXFVF );
			hr = pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );

			pd3dDevice->SetTexture ( 0, NULL );

			// End the scene.
			pd3dDevice->EndScene();
			pd3dDevice->Present( NULL, NULL, NULL, NULL );	
		}
	}

	SAFE_RELEASE( pTextureVB );

	TextureManager::ReleaseTexture( strTexture.GetString(), pLoadingTexture );

	///////////////////////////////////////////////////////////////////////////////
	//	[TOGGLE SWITCH] 로더 스레드 종료
	DeWait ();
	//////////////////////////////////////////////////////////////////////////////
	n_ExitState = eNORMAL;

	return 0;
}

namespace N_LOADING_MAP_THREAD
{
	DWORD				n_dwThreadID;
	LPDIRECT3DDEVICEQ*	n_ppd3dDevice;
	HWND				n_hWnd;
	BOOL				n_bWait;
	BOOL				n_bRender;
	char				n_szAppPath[256];
	int					n_ExitState;
	S_LOADING_MAP_DATA	sLOADING_MAP_DATA;

	BOOL	StartThread ( LPDIRECT3DDEVICEQ* ppd3dDevice, HWND hWnd, char* szAppPath, const int& nLOADTYPE )
	{
		n_ppd3dDevice = ppd3dDevice;
		n_hWnd = hWnd;
		StringCchCopy ( n_szAppPath, MAX_PATH, szAppPath );
		
		n_bRender = TRUE;	//	렌더
		EnWait ();
		n_ExitState = eNORMAL;

		{
			sLOADING_MAP_DATA.nLOADTYPE = nLOADTYPE;
		}

		//HANDLE hThread = CreateThread ( NULL, 0, LoadingMapThread, &sLOADING_MAP_DATA, 0, &n_dwThreadID );
		HANDLE hThread = (HANDLE) ::_beginthreadex(
										NULL,
										0,
										LoadingMapThread,
										&sLOADING_MAP_DATA,
										0,
										(unsigned int*) &n_dwThreadID );
		if ( !hThread )
		{
			DeWait();
			return FALSE;
		}

		return TRUE;
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
};