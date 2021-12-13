//-----------------------------------------------------------------------------
// File: D3DApp.cpp
//
// Desc: Application class for the Direct3D samples framework library.
//-----------------------------------------------------------------------------
#include "pch.h"

#ifdef STRICT
#undef STRICT
#endif
#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <basetsd.h>
#include <mmsystem.h>
#include <stdio.h>
#include <tchar.h>
#include <D3D9.h>
#include "D3DApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern BOOL	g_bFRAME_LIMIT;

//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
static CD3DApplication* g_pD3DApp = NULL;

//	Note : 전역에서 현제 시간을 알기 위해서 필요하다.
//
FLOAT CD3DApplication::m_fTime;             // Current time in seconds
FLOAT CD3DApplication::m_fElapsedTime;      // Time elapsed since last frame
BOOL  CD3DApplication::m_bDefWin = TRUE;


// DirectX 위에 GUI 설치할꺼냐? 말꺼냐? ( 준혁 )
#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(CH_PARAM) || defined(TH_PARAM) || defined(TW_PARAM) || defined(HK_PARAM) || defined( PH_PARAM )  || defined(ID_PARAM) || defined ( JP_PARAM ) || defined(GS_PARAM) || defined(_RELEASED)  // 필리핀 아이템샵
	BOOL  CD3DApplication::m_bEnableGUI = TRUE;
#else
	BOOL  CD3DApplication::m_bEnableGUI = FALSE;
#endif

//-----------------------------------------------------------------------------
// Name: CD3DApplication()
// Desc: Constructor
//-----------------------------------------------------------------------------
CD3DApplication::CD3DApplication() :
	m_dwDefScrX(800),
	m_dwDefScrY(600),
	m_emDefScrFmt(EMSCREEN_F16),
	m_uDefScrHz(60),

	m_dwNumAdapters(0),
	m_dwAdapter(0L),
	m_pD3D(NULL),
	m_pd3dDevice(NULL),
	m_hWndApp(NULL),
	m_hWndFocus(NULL),
	m_bActive(FALSE),
	m_bReady(FALSE),
	m_bHasFocus(FALSE),
	m_dwCreateFlags(0L),

	m_bFrameMoving(TRUE),
	m_bSingleStep(FALSE),
	m_fFPS(0.0f),

	m_bVisibleGUI( FALSE )
{
    g_pD3DApp           = this;

	memset( m_strDeviceStats, 0, sizeof(TCHAR)*STRING_NUM_90 );
	memset( m_strFrameStats, 0, sizeof(TCHAR)*STRING_NUM_90 );

	memset( &m_rtBoundGUI, 0, sizeof( RECT ) );

	m_strWindowTitle    = _T("D3D9 Application");
	m_dwCreationWidth   = 800;
	m_dwCreationHeight  = 600;
	m_bUseDepthBuffer   = FALSE;
	m_dwMinDepthBits    = 16;
	m_dwMinStencilBits  = 0;
	m_bShowCursorWhenFullscreen = FALSE;

	// When m_bClipCursorWhenFullscreen is TRUE, the cursor is limited to
	// the device window when the app goes fullscreen.  This prevents users
	// from accidentally clicking outside the app window on a multimon system.
	// This flag is turned off by default for debug builds, since it makes 
	// multimon debugging difficult.
#if defined(_DEBUG) || defined(DEBUG)
	m_bClipCursorWhenFullscreen = FALSE;
#else
	m_bClipCursorWhenFullscreen = TRUE;
#endif
}


//--------------------------------------------------------------------------------------
// Return if windowed in the current device.  If no device exists yet, then returns false
//--------------------------------------------------------------------------------------
bool CD3DApplication::DXUTIsWindowed()                     
{ 
	D3DDeviceInfo* pDeviceInfo = g_pD3DApp->GetCurDeviceInfo();
    if(pDeviceInfo) return (pDeviceInfo->bWindowed != 0); 
    else			return false; 
}

//--------------------------------------------------------------------------------------
// Gives the D3D device a cursor with image and hotspot from hCursor.
//--------------------------------------------------------------------------------------
HRESULT CD3DApplication::DXUTSetDeviceCursor( IDirect3DDevice9* pd3dDevice, HCURSOR hCursor, bool bAddWatermark )
{
    HRESULT hr = E_FAIL;
    ICONINFO iconinfo;
    bool bBWCursor;
    LPDIRECT3DSURFACEQ pCursorSurface = NULL;
    HDC hdcColor = NULL;
    HDC hdcMask = NULL;
    HDC hdcScreen = NULL;
    BITMAP bm;
    DWORD dwWidth;
    DWORD dwHeightSrc;
    DWORD dwHeightDest;
    COLORREF crColor;
    COLORREF crMask;
    UINT x;
    UINT y;
    BITMAPINFO bmi;
    COLORREF* pcrArrayColor = NULL;
    COLORREF* pcrArrayMask = NULL;
    DWORD* pBitmap;
    HGDIOBJ hgdiobjOld;

    SecureZeroMemory( &iconinfo, sizeof(iconinfo) );
    if( !GetIconInfo( hCursor, &iconinfo ) )
        goto End;

    if (0 == GetObject((HGDIOBJ)iconinfo.hbmMask, sizeof(BITMAP), (LPVOID)&bm))
        goto End;
    dwWidth = bm.bmWidth;
    dwHeightSrc = bm.bmHeight;

    if( iconinfo.hbmColor == NULL )
    {
        bBWCursor = TRUE;
        dwHeightDest = dwHeightSrc / 2;
    }
    else 
    {
        bBWCursor = FALSE;
        dwHeightDest = dwHeightSrc;
    }

    // Create a surface for the fullscreen cursor
    if( FAILED( hr = pd3dDevice->CreateOffscreenPlainSurface( dwWidth, dwHeightDest, 
        D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pCursorSurface, NULL ) ) )
    {
        goto End;
    }

    pcrArrayMask = new DWORD[dwWidth * dwHeightSrc];

    SecureZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = dwWidth;
    bmi.bmiHeader.biHeight = dwHeightSrc;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hdcScreen = GetDC( NULL );
    hdcMask = CreateCompatibleDC( hdcScreen );
    if( hdcMask == NULL )
    {
        hr = E_FAIL;
        goto End;
    }
    hgdiobjOld = SelectObject(hdcMask, iconinfo.hbmMask);
    GetDIBits(hdcMask, iconinfo.hbmMask, 0, dwHeightSrc, 
        pcrArrayMask, &bmi, DIB_RGB_COLORS);
    SelectObject(hdcMask, hgdiobjOld);

    if (!bBWCursor)
    {
        pcrArrayColor = new DWORD[dwWidth * dwHeightDest];
        hdcColor = CreateCompatibleDC( hdcScreen );
        if( hdcColor == NULL )
        {
            hr = E_FAIL;
            goto End;
        }
        SelectObject(hdcColor, iconinfo.hbmColor);
        GetDIBits(hdcColor, iconinfo.hbmColor, 0, dwHeightDest, 
            pcrArrayColor, &bmi, DIB_RGB_COLORS);
    }

    // Transfer cursor image into the surface
    D3DLOCKED_RECT lr;
    pCursorSurface->LockRect( &lr, NULL, 0 );
    pBitmap = (DWORD*)lr.pBits;
    for( y = 0; y < dwHeightDest; y++ )
    {
        for( x = 0; x < dwWidth; x++ )
        {
            if (bBWCursor)
            {
                crColor = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
                crMask = pcrArrayMask[dwWidth*(dwHeightSrc-1-y) + x];
            }
            else
            {
                crColor = pcrArrayColor[dwWidth*(dwHeightDest-1-y) + x];
                crMask = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
            }
            if (crMask == 0)
                pBitmap[dwWidth*y + x] = 0xff000000 | crColor;
            else
                pBitmap[dwWidth*y + x] = 0x00000000;

            // It may be helpful to make the D3D cursor look slightly 
            // different from the Windows cursor so you can distinguish 
            // between the two when developing/testing code.  When
            // bAddWatermark is TRUE, the following code adds some
            // small grey "D3D" characters to the upper-left corner of
            // the D3D cursor image.
            if( bAddWatermark && x < 12 && y < 5 )
            {
                // 11.. 11.. 11.. .... CCC0
                // 1.1. ..1. 1.1. .... A2A0
                // 1.1. .1.. 1.1. .... A4A0
                // 1.1. ..1. 1.1. .... A2A0
                // 11.. 11.. 11.. .... CCC0

                const WORD wMask[5] = { 0xccc0, 0xa2a0, 0xa4a0, 0xa2a0, 0xccc0 };
                if( wMask[y] & (1 << (15 - x)) )
                {
                    pBitmap[dwWidth*y + x] |= 0xff808080;
                }
            }
        }
    }
    pCursorSurface->UnlockRect();

    // Set the device cursor
    if( FAILED( hr = pd3dDevice->SetCursorProperties( iconinfo.xHotspot, 
        iconinfo.yHotspot, pCursorSurface ) ) )
    {
        goto End;
    }

    hr = S_OK;

End:
    if( iconinfo.hbmMask != NULL )
        DeleteObject( iconinfo.hbmMask );
    if( iconinfo.hbmColor != NULL )
        DeleteObject( iconinfo.hbmColor );
    if( hdcScreen != NULL )
        ReleaseDC( NULL, hdcScreen );
    if( hdcColor != NULL )
        DeleteDC( hdcColor );
    if( hdcMask != NULL )
        DeleteDC( hdcMask );
    SAFE_DELETE_ARRAY( pcrArrayColor );
    SAFE_DELETE_ARRAY( pcrArrayMask );
    SAFE_RELEASE( pCursorSurface );
    return hr;
}

//--------------------------------------------------------------------------------------
// Setup cursor based on current settings (window/fullscreen mode, show cursor state, clip cursor state)
//--------------------------------------------------------------------------------------
void CD3DApplication::DXUTSetupCursor()
{
	// Show the cursor again if returning to fullscreen 
	if( !DXUTIsWindowed() && g_pD3DApp->m_bShowCursorWhenFullscreen && g_pD3DApp->m_pd3dDevice )
	{
		HCURSOR hCursor = (HCURSOR)(ULONG_PTR)GetClassLongPtr( DXUTGetHWNDDeviceFullScreen(), GCLP_HCURSOR );
		DXUTSetDeviceCursor( g_pD3DApp->m_pd3dDevice, hCursor, false );
	}

	// Clip cursor if requested
	if( !DXUTIsWindowed() && g_pD3DApp->m_bClipCursorWhenFullscreen )
	{
		// Confine cursor to full screen window
		RECT rcWindow;
		GetWindowRect( m_hWndApp, &rcWindow );
		ClipCursor( &rcWindow );
	}
	else
	{
		ClipCursor( NULL );
	}
}

//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return g_pD3DApp->MsgProc( hWnd, uMsg, wParam, lParam );
}


HRESULT CD3DApplication::BuildDeviceListSet()
{
    HRESULT hr;

    // Create the Direct3D object
    m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if( m_pD3D == NULL )
        return DisplayErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );

    // Build a list of Direct3D adapters, modes and devices. The
    // ConfirmDevice() callback is used to confirm that only devices that
    // meet the app's requirements are considered.
    if( FAILED( hr = BuildDeviceList() ) )
    {
        SAFE_RELEASE( m_pD3D );
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }

	SAFE_RELEASE( m_pD3D );
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Create()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Create( HINSTANCE hInstance )
{
    HRESULT hr;

    // Create the Direct3D object
    m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if( m_pD3D == NULL )
        return DisplayErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );

    // Build a list of Direct3D adapters, modes and devices. The
    // ConfirmDevice() callback is used to confirm that only devices that
    // meet the app's requirements are considered.
    if( FAILED( hr = BuildDeviceList() ) )
    {
        SAFE_RELEASE( m_pD3D );
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }

    // Unless a substitute hWnd has been specified, create a window to
    // render into
    if( m_hWndApp == NULL)
    {
        // Register the windows class
        WNDCLASS wndClass = { 0, WndProc, 0, 0, hInstance,
                              NULL,
                              LoadCursor( NULL, IDC_ARROW ),
                              (HBRUSH)GetStockObject(WHITE_BRUSH),
                              NULL, _T("D3D Window") };
        RegisterClass( &wndClass );

        // Set the window's initial style
        m_dwWindowStyle = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_VISIBLE;

       // Set the window's initial width
        RECT rc;
        SetRect( &rc, 0, 0, m_dwCreationWidth, m_dwCreationHeight );
        AdjustWindowRect( &rc, m_dwWindowStyle, TRUE );

        // Create the render window
        m_hWndApp = CreateWindow( _T("D3D Window"), m_strWindowTitle, m_dwWindowStyle,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               (rc.right-rc.left), (rc.bottom-rc.top), 0L,
                               NULL,
                               hInstance, 0L );
    }

    // The focus window can be a specified to be a different window than the
    // device window.  If not, use the device window as the focus window.
    if( m_hWndFocus == NULL )
        m_hWndFocus = m_hWndApp;

    // Save window properties
    m_dwWindowStyle = GetWindowLong( m_hWndApp, GWL_STYLE );

	// DirectX 위에 설치할 GUI를 위해서 WindowStyle에 WS_CLIPCHILDREN 추가 ( 준혁 )
	if( m_bEnableGUI )
	{
		m_dwWindowStyle |= WS_CLIPCHILDREN;
		SetWindowLong( m_hWndApp, GWL_STYLE, m_dwWindowStyle );
	}

    GetWindowRect( m_hWndApp, &m_rcWindowBounds );
    GetClientRect( m_hWndApp, &m_rcWindowClient );

    // Initialize the application timer
    DXUtil_Timer( TIMER_START );

    // Initialize the app's custom scene stuff
    if( FAILED( hr = OneTimeSceneInit() ) )
    {
        SAFE_RELEASE( m_pD3D );
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }

    // Initialize the 3D environment for the app
    if( FAILED( hr = Initialize3DEnvironment() ) )
    {
        SAFE_RELEASE( m_pD3D );
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }

    // The app is ready to go
    m_bReady = TRUE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SortModesCallback()
// Desc: Callback function for sorting display modes (used by BuildDeviceList).
//-----------------------------------------------------------------------------
static int SortModesCallback( const VOID* arg1, const VOID* arg2 )
{
    D3DDISPLAYMODE* p1 = (D3DDISPLAYMODE*)arg1;
    D3DDISPLAYMODE* p2 = (D3DDISPLAYMODE*)arg2;

    if( p1->Format > p2->Format )   return -1;
    if( p1->Format < p2->Format )   return +1;
    if( p1->Width  < p2->Width )    return -1;
    if( p1->Width  > p2->Width )    return +1;
    if( p1->Height < p2->Height )   return -1;
    if( p1->Height > p2->Height )   return +1;

	if( p1->RefreshRate < p2->RefreshRate )   return -1;
    if( p1->RefreshRate > p2->RefreshRate )   return +1;

    return 0;
}


static BOOL MatchScrSize ( D3DModeInfo &ModeInfo, DWORD dwDefScrX, DWORD dwDefScrY )
{
	return (ModeInfo.Width==dwDefScrX&&ModeInfo.Height==dwDefScrY);
}

static BOOL MatchScrFormat ( D3DModeInfo &ModeInfo, EMSCREEN_FORMAT emFormat )
{
	switch ( emFormat )
	{
	case EMSCREEN_F16:
		return (ModeInfo.Format==D3DFMT_R5G6B5)||(ModeInfo.Format==D3DFMT_X1R5G5B5)||(ModeInfo.Format==D3DFMT_A1R5G5B5);

	case EMSCREEN_F32:
		return (ModeInfo.Format==D3DFMT_R8G8B8)||(ModeInfo.Format==D3DFMT_X8R8G8B8)||(ModeInfo.Format==D3DFMT_A8R8G8B8);
	};

	return FALSE;
}

static BOOL MatchScrRefreshHz ( D3DModeInfo &ModeInfo, UINT nRefreshHz )
{
	ModeInfo.nCurrentHz = *ModeInfo.RefreshRate.GetHead();
	for ( ModeInfo.RefreshRate.SetHead(); !ModeInfo.RefreshRate.IsEnd(); ModeInfo.RefreshRate.GoNext() )
	{
		if ( *ModeInfo.RefreshRate.GetCurrent() == nRefreshHz )
		{
			ModeInfo.nCurrentHz = *ModeInfo.RefreshRate.GetCurrent();
			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// Name: BuildDeviceList()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::BuildDeviceList()
{
	const DWORD dwNumDeviceTypes = 2;
	const TCHAR* strDeviceDescs[] = { _T("HAL"), _T("REF") };
	const D3DDEVTYPE DeviceTypes[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_REF };

	BOOL bHALExists = FALSE;
	BOOL bHALIsWindowedCompatible = FALSE;
	BOOL bHALIsDesktopCompatible = FALSE;
	BOOL bHALIsSampleCompatible = FALSE;

    DWORD m,f;

    const D3DFORMAT allowedAdapterFormatArray[] = 
    {   
        D3DFMT_X8R8G8B8, 
        D3DFMT_X1R5G5B5, 
        D3DFMT_R5G6B5, 
        D3DFMT_A2R10G10B10
    };

   const UINT allowedAdapterFormatArrayCount  = sizeof(allowedAdapterFormatArray) / sizeof(allowedAdapterFormatArray[0]);

	// Loop through all the adapters on the system (usually, there's just one
	// unless more than one graphics card is present).
   UINT iAdapterNum = m_pD3D->GetAdapterCount();
	for( UINT iAdapter = 0; iAdapter < iAdapterNum; iAdapter++ )
	{
		// Fill in adapter info
		D3DAdapterInfo* pAdapter  = &m_Adapters[m_dwNumAdapters];
		m_pD3D->GetAdapterIdentifier( iAdapter, 0, &pAdapter->d3dAdapterIdentifier );
		m_pD3D->GetAdapterDisplayMode( iAdapter, &pAdapter->d3ddmDesktop );
		pAdapter->dwNumDevices    = 0;
		pAdapter->dwCurrentDevice = 0;

		// Enumerate all display modes on this adapter
		const DWORD MODE_ASIZE = 260;
		D3DDISPLAYMODE modes[MODE_ASIZE*4];
		D3DFORMAT      formats[20];
		DWORD dwNumFormats      = 0;
		DWORD dwNumModes        = 0;

		for( UINT iFormatList = 0; iFormatList < allowedAdapterFormatArrayCount; iFormatList++ )
		{
            D3DFORMAT allowedAdapterFormat = allowedAdapterFormatArray[iFormatList];

			DWORD dwNumAdapterModes = m_pD3D->GetAdapterModeCount ( iAdapter, allowedAdapterFormat );
			if ( dwNumAdapterModes > MODE_ASIZE )	dwNumAdapterModes = MODE_ASIZE;

			// Add the adapter's current desktop format to the list of formats
			formats[dwNumFormats++] = pAdapter->d3ddmDesktop.Format;

			for( UINT iMode = 0; iMode<dwNumAdapterModes; iMode++ )
			{
				// Get the display mode attributes
				D3DDISPLAYMODE DisplayMode;
				m_pD3D->EnumAdapterModes ( iAdapter, allowedAdapterFormat, iMode, &DisplayMode );

				// Filter out low-resolution modes
				if( DisplayMode.Width  < 800 || DisplayMode.Height < 600 )
					continue;

				// Check if the mode already exists (to filter out refresh rates)
				for( m=0L; m<dwNumModes; m++ )
				{
					if( ( modes[m].Width  == DisplayMode.Width  ) &&
						( modes[m].Height == DisplayMode.Height ) &&
						( modes[m].Format == DisplayMode.Format ) &&
						( modes[m].RefreshRate == DisplayMode.RefreshRate ) )
						break;
				}

				// If we found a new mode, add it to the list of modes
				if( m == dwNumModes )
				{
					modes[dwNumModes].Width			= DisplayMode.Width;
					modes[dwNumModes].Height		= DisplayMode.Height;
					modes[dwNumModes].Format		= DisplayMode.Format;
					
					modes[dwNumModes].RefreshRate	= DisplayMode.RefreshRate;
					dwNumModes++;

					// Check if the mode's format already exists
					for( f =0; f<dwNumFormats; f++ )
					{
						if( DisplayMode.Format == formats[f] )	break;
					}

					// If the format is new, add it to the list
					if( f== dwNumFormats )
						formats[dwNumFormats++] = DisplayMode.Format;
				}
			}
		}

		// Sort the list of display modes (by format, then width, then height)
		qsort( modes, dwNumModes, sizeof(D3DDISPLAYMODE), SortModesCallback );

		// Add devices to adapter
		for( UINT iDevice = 0; iDevice < dwNumDeviceTypes; iDevice++ )
		{
			// Fill in device info
			D3DDeviceInfo* pDevice;
			pDevice                 = &pAdapter->devices[pAdapter->dwNumDevices];
			pDevice->DeviceType     = DeviceTypes[iDevice];
			m_pD3D->GetDeviceCaps( iAdapter, DeviceTypes[iDevice], &pDevice->d3dCaps );
			pDevice->strDesc        = strDeviceDescs[iDevice];
			pDevice->dwNumModes     = 0;
			pDevice->dwCurrentMode  = 0;
			pDevice->bCanDoWindowed = FALSE;
			pDevice->bWindowed      = FALSE;
			pDevice->MultiSampleTypeFullscreen = D3DMULTISAMPLE_NONE;
			pDevice->MultiSampleTypeWindowed = D3DMULTISAMPLE_NONE;

			// Examine each format supported by the adapter to see if it will
			// work with this device and meets the needs of the application.
			BOOL  bFormatConfirmed[20];
			DWORD dwBehavior[20];
			D3DFORMAT fmtDepthStencil[20];

			
			for( DWORD f=0; f<dwNumFormats; f++ )
			{
				bFormatConfirmed[f] = FALSE;
				fmtDepthStencil[f] = D3DFMT_UNKNOWN;

				// Skip formats that cannot be used as render targets on this device
				if( FAILED( m_pD3D->CheckDeviceType( iAdapter, pDevice->DeviceType,
														formats[f], formats[f], FALSE ) ) )
					continue;

				if( pDevice->DeviceType == D3DDEVTYPE_HAL )
				{
					// This system has a HAL device
					bHALExists = TRUE;

					//if( pDevice->d3dCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED )
					{
						// HAL can run in a window for some mode
						bHALIsWindowedCompatible = TRUE;

						if( f == 0 )
						{
							// HAL can run in a window for the current desktop mode
							bHALIsDesktopCompatible = TRUE;
						}
					}
				}

				// Confirm the device/format for HW vertex processing
				if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT )
				{
					if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_PUREDEVICE )
					{
						dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING |
										D3DCREATE_PUREDEVICE;

						if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
														formats[f] ) ) )
							bFormatConfirmed[f] = TRUE;
					}

					if ( FALSE == bFormatConfirmed[f] )
					{
						dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING;

						if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
														formats[f] ) ) )
							bFormatConfirmed[f] = TRUE;
					}

					if ( FALSE == bFormatConfirmed[f] )
					{
						dwBehavior[f] = D3DCREATE_MIXED_VERTEXPROCESSING;

						if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
														formats[f] ) ) )
							bFormatConfirmed[f] = TRUE;
					}
				}

				// Confirm the device/format for SW vertex processing
				if( FALSE == bFormatConfirmed[f] )
				{
					dwBehavior[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

					if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
													formats[f] ) ) )
						bFormatConfirmed[f] = TRUE;
				}

				// Find a suitable depth/stencil buffer format for this device/format
				if( bFormatConfirmed[f] && m_bUseDepthBuffer )
				{
					if( !FindDepthStencilFormat( iAdapter, pDevice->DeviceType,
						formats[f], &fmtDepthStencil[f] ) )
					{
						bFormatConfirmed[f] = FALSE;
					}
				}
			}

			// Add all enumerated display modes with confirmed formats to the
			// device's list of valid modes
			for( DWORD m=0L; m<dwNumModes; m++ )
			{
				for( DWORD f=0; f<dwNumFormats; f++ )
				{
					if( modes[m].Format == formats[f] )
					{
						if( bFormatConfirmed[f] == TRUE )
						{
							DWORD dm;
							for ( dm=0; dm<pDevice->dwNumModes; dm++ )
							{
								if ( (pDevice->modes[dm].Width==modes[m].Width) &&
									(pDevice->modes[dm].Height==modes[m].Height) &&
									(pDevice->modes[dm].Format==modes[m].Format) )
									break;
							}

							// 새로운 모드일 경우.
							if ( dm == pDevice->dwNumModes )
							{
								// Add this mode to the device's list of valid modes
								pDevice->modes[pDevice->dwNumModes].Width      = modes[m].Width;
								pDevice->modes[pDevice->dwNumModes].Height     = modes[m].Height;
								pDevice->modes[pDevice->dwNumModes].Format     = modes[m].Format;
								pDevice->modes[pDevice->dwNumModes].dwBehavior = dwBehavior[f];
								pDevice->modes[pDevice->dwNumModes].DepthStencilFormat = fmtDepthStencil[f];
								pDevice->modes[pDevice->dwNumModes].RefreshRate.AddTail ( modes[m].RefreshRate );

								pDevice->dwNumModes++;
							}
							// 갱신 주기만 틀릴 경우.
							else
							{
								pDevice->modes[dm].RefreshRate.AddTail ( modes[m].RefreshRate );
							}

							if( pDevice->DeviceType == D3DDEVTYPE_HAL )
								bHALIsSampleCompatible = TRUE;
						}
					}
				}
			}

			//	Note : 세팅된 디스플래이 모드 찾기.
			//
			pDevice->dwCurrentMode = -1;
			for ( m=0; m<pDevice->dwNumModes; m++ )
			{
				if ( MatchScrSize(pDevice->modes[m],m_dwDefScrX,m_dwDefScrY) )
				{
					pDevice->dwCurrentMode = m;
					
					if ( MatchScrFormat(pDevice->modes[m],m_emDefScrFmt) )
					{
						MatchScrRefreshHz(pDevice->modes[m],m_uDefScrHz);
						break;
					}
				}
			}

			//	Note : 못 찾았을 경우 기본설정으로 찾음.
			//
			if ( pDevice->dwCurrentMode == -1 )
			{
				for ( m=0; m<pDevice->dwNumModes; m++ )
				{
					if ( MatchScrSize(pDevice->modes[m],800,600) )
					{
						pDevice->dwCurrentMode = m;
						
						if ( MatchScrFormat(pDevice->modes[m],EMSCREEN_F16) )
						{
							MatchScrRefreshHz(pDevice->modes[m],60);
							break;
						}
					}
				}
			}

			// Check if the device is compatible with the desktop display mode
			// (which was added initially as formats[0])
			if( bFormatConfirmed[0] /*&& (pDevice->d3dCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED)*/ )
			{
				pDevice->bCanDoWindowed = TRUE;
				pDevice->bWindowed      = TRUE;
			}

			// If valid modes were found, keep this device
			if( pDevice->dwNumModes > 0 )	pAdapter->dwNumDevices++;
		}

		// If valid devices were found, keep this adapter
		if( pAdapter->dwNumDevices > 0 )	m_dwNumAdapters++;
	}

	// Return an error if no compatible devices were found
	if( 0L == m_dwNumAdapters )		return D3DAPPERR_NOCOMPATIBLEDEVICES;

	// Pick a default device that can render into a window
	// (This code assumes that the HAL device comes before the REF
	// device in the device array).
	for( DWORD a=0; a<m_dwNumAdapters; a++ )
	{
		for( DWORD d=0; d < m_Adapters[a].dwNumDevices; d++ )
		{
			if( m_Adapters[a].devices[d].bWindowed )
			{
				m_Adapters[a].dwCurrentDevice = d;
				m_dwAdapter = a;

				// Display a warning message
				if( m_Adapters[a].devices[d].DeviceType == D3DDEVTYPE_REF )
				{
					if( !bHALExists )
						DisplayErrorMsg( D3DAPPERR_NOHARDWAREDEVICE, MSGWARN_SWITCHEDTOREF );
					else if( !bHALIsSampleCompatible )
						DisplayErrorMsg( D3DAPPERR_HALNOTCOMPATIBLE, MSGWARN_SWITCHEDTOREF );
					else if( !bHALIsWindowedCompatible )
						DisplayErrorMsg( D3DAPPERR_NOWINDOWEDHAL, MSGWARN_SWITCHEDTOREF );
					else if( !bHALIsDesktopCompatible )
						DisplayErrorMsg( D3DAPPERR_NODESKTOPHAL, MSGWARN_SWITCHEDTOREF );
					else // HAL is desktop compatible, but not sample compatible
						DisplayErrorMsg( D3DAPPERR_NOHALTHISMODE, MSGWARN_SWITCHEDTOREF );
				}

				return S_OK;
			}
		}
	}

	return D3DAPPERR_NOWINDOWABLEDEVICES;
}




//-----------------------------------------------------------------------------
// Name: FindDepthStencilFormat()
// Desc: Finds a depth/stencil format for the given device that is compatible
//       with the render target format and meets the needs of the app.
//-----------------------------------------------------------------------------
BOOL CD3DApplication::FindDepthStencilFormat( UINT iAdapter, D3DDEVTYPE DeviceType,
    D3DFORMAT TargetFormat, D3DFORMAT* pDepthStencilFormat )
{
    if( m_dwMinDepthBits <= 16 && m_dwMinStencilBits == 0 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D16 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D16;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 15 && m_dwMinStencilBits <= 1 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D15S1 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D15S1;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits == 0 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D24X8 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D24X8;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits <= 8 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D24S8 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D24S8;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits <= 4 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D24X4S4 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D24X4S4;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 32 && m_dwMinStencilBits == 0 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D32 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D32;
                return TRUE;
            }
        }
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message handling function.
//-----------------------------------------------------------------------------
LRESULT CD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam )
{
    HRESULT hr;

    switch( uMsg )
    {
        case WM_PAINT:
            // Handle paint messages when the app is not ready
            if( m_pd3dDevice && !m_bReady )
            {
                if( m_bDefWin )
                {
                    Render();
                    Present();
                }
            }
            break;

        case WM_ACTIVATEAPP:
            m_bHasFocus = (BOOL) wParam;
            break;

        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
            break;

        case WM_ENTERSIZEMOVE:
            // Halt frame movement while the app is sizing or moving
            Pause( TRUE );
            break;

        case WM_SIZE:
            // Check to see if we are losing our window...
            if( SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam )
            {
                if( m_bClipCursorWhenFullscreen && !m_bDefWin )
                    ClipCursor( NULL );
                m_bActive = FALSE;
            }
            else
            {
                m_bActive = TRUE;
            }
            break;

        case WM_EXITSIZEMOVE:
            Pause( FALSE );

            if( m_bActive && m_bDefWin )
            {
                RECT rcClientOld;
                rcClientOld = m_rcWindowClient;

                // Update window properties
                GetWindowRect( m_hWndApp, &m_rcWindowBounds );
                GetClientRect( m_hWndApp, &m_rcWindowClient );

                if( rcClientOld.right - rcClientOld.left !=
                    m_rcWindowClient.right - m_rcWindowClient.left ||
                    rcClientOld.bottom - rcClientOld.top !=
                    m_rcWindowClient.bottom - m_rcWindowClient.top)
                {
                    // A new window size will require a new backbuffer
                    // size, so the 3D structures must be changed accordingly.
                    m_bReady = FALSE;

                    m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
                    m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

                    // Resize the 3D environment
                    if( FAILED( hr = Resize3DEnvironment() ) )
                    {
                        DisplayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
                        return 0;
                    }

                    m_bReady = TRUE;
                }
            }

            break;

        case WM_SETCURSOR:
            // Turn off Windows cursor in fullscreen mode
            if( m_bActive && m_bReady && !m_bDefWin )
            {
                SetCursor( NULL );
                if( m_bShowCursorWhenFullscreen )
                    m_pd3dDevice->ShowCursor( TRUE );
                return TRUE; // prevent Windows from setting cursor to window class cursor
            }
            break;

         case WM_MOUSEMOVE:
            if( m_bActive && m_bReady && m_pd3dDevice != NULL )
            {
                POINT ptCursor;
                GetCursorPos( &ptCursor );
                if( !m_bDefWin )
                    ScreenToClient( m_hWndApp, &ptCursor );
                m_pd3dDevice->SetCursorPosition( ptCursor.x, ptCursor.y, 0L );
            }
            break;

       case WM_ENTERMENULOOP:
            // Pause the app when menus are displayed
            Pause(TRUE);
            break;

        case WM_EXITMENULOOP:
            Pause(FALSE);
            break;

        case WM_CONTEXTMENU:
            // No context menus allowed in fullscreen mode
            if( m_bDefWin == FALSE )
                break;
            break;

        case WM_NCHITTEST:
            // Prevent the user from selecting the menu in fullscreen mode
            if( !m_bDefWin )
                return HTCLIENT;

            break;

        case WM_POWERBROADCAST:
            switch( wParam )
            {
                #ifndef PBT_APMQUERYSUSPEND
                    #define PBT_APMQUERYSUSPEND 0x0000
                #endif
                case PBT_APMQUERYSUSPEND:
                    // At this point, the app should save any data for open
                    // network connections, files, etc., and prepare to go into
                    // a suspended mode.
                    return TRUE;

                #ifndef PBT_APMRESUMESUSPEND
                    #define PBT_APMRESUMESUSPEND 0x0007
                #endif
                case PBT_APMRESUMESUSPEND:
                    // At this point, the app should recover any data, network
                    // connections, files, etc., and resume running from when
                    // the app was suspended.
                    return TRUE;
            }
            break;

        case WM_SYSCOMMAND:
            // Prevent moving/sizing and power loss in fullscreen mode
            switch( wParam )
            {
                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_KEYMENU:
                case SC_MONITORPOWER:
                    if( FALSE == m_bDefWin )
                        return 1;
                    break;
            }
            break;

        case WM_COMMAND:
            break;

		case WM_KEYUP:
			break;

        case WM_CLOSE:
            Cleanup3DEnvironment();
            DestroyMenu( GetMenu(hWnd) );
            DestroyWindow( hWnd );
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}



HRESULT CD3DApplication::CreateObjects()
{
	HRESULT hr;

    // Initialize the app's device-dependent objects	
    hr = InitDeviceObjects();	
    if( SUCCEEDED(hr) )
    {		
        hr = RestoreDeviceObjects();	
        if( SUCCEEDED(hr) )
        {
            return S_OK;
        }
    }

	return hr;
}

//-----------------------------------------------------------------------------
// Name: Initialize3DEnvironment()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Initialize3DEnvironment()
{
    HRESULT hr;

    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
    D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];

    // Prepare window for possible windowed/fullscreen change
    AdjustWindowForChange();

    // Set up the presentation parameters
    SecureZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );

	if ( pDeviceInfo->bWindowed && m_bDefWin )
		m_d3dpp.Windowed           = TRUE;
	else
		m_d3dpp.Windowed           = FALSE;

    m_d3dpp.BackBufferCount        = 1;
    if( pDeviceInfo->bWindowed )
        m_d3dpp.MultiSampleType    = pDeviceInfo->MultiSampleTypeWindowed;
    else
        m_d3dpp.MultiSampleType    = pDeviceInfo->MultiSampleTypeFullscreen;

	// DirectX 위에 설치할 GUI를 위해서 Swap 설정 ( 준혁 )
	if( m_bEnableGUI )
		m_d3dpp.SwapEffect         = D3DSWAPEFFECT_COPY;
	else
		m_d3dpp.SwapEffect         = D3DSWAPEFFECT_DISCARD;

	m_d3dpp.EnableAutoDepthStencil = m_bUseDepthBuffer;
    m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
    m_d3dpp.hDeviceWindow          = m_hWndApp;
    if( m_bDefWin )
    {
        m_d3dpp.BackBufferWidth			= m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight		= m_rcWindowClient.bottom - m_rcWindowClient.top;
        m_d3dpp.BackBufferFormat		= pAdapterInfo->d3ddmDesktop.Format;
		m_d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_ONE;	//D3DPRESENT_INTERVAL_IMMEDIATE;	//D3DPRESENT_INTERVAL_ONE;
    }
    else
    {
        m_d3dpp.BackBufferWidth				= pModeInfo->Width;
        m_d3dpp.BackBufferHeight			= pModeInfo->Height;
        m_d3dpp.BackBufferFormat			= pModeInfo->Format;
		m_d3dpp.FullScreen_RefreshRateInHz	= pModeInfo->nCurrentHz;
		m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE; // Speed Limit Off
    }

    if( pDeviceInfo->d3dCaps.PrimitiveMiscCaps & D3DPMISCCAPS_NULLREFERENCE )
    {
        // Warn user about null ref device that can't render anything
        DisplayErrorMsg( D3DAPPERR_NULLREFDEVICE, 0 );
    }

    // Create the device
    hr = m_pD3D->CreateDevice( m_dwAdapter, pDeviceInfo->DeviceType,
                               m_hWndFocus, pModeInfo->dwBehavior|D3DCREATE_MULTITHREADED, &m_d3dpp,
                               &m_pd3dDevice );
	//hr = m_pD3D->CreateDevice( m_pD3D->GetAdapterCount()-1, D3DDEVTYPE_REF, 
 //                               m_hWndFocus, D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED,
 //                               &m_d3dpp, &m_pd3dDevice );
    if( SUCCEEDED(hr) )
    {
        // When moving from fullscreen to windowed mode, it is important to
        // adjust the window size after recreating the device rather than
        // beforehand to ensure that you get the window size you want.  For
        // example, when switching from 640x480 fullscreen to windowed with
        // a 1000x600 window on a 1024x768 desktop, it is impossible to set
        // the window size to 1000x600 until after the display mode has
        // changed to 1024x768, because windows cannot be larger than the
        // desktop.
        if( m_bDefWin )
        {
            SetWindowPos( m_hWndApp, HWND_NOTOPMOST,
                          m_rcWindowBounds.left, m_rcWindowBounds.top,
                          ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                          ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                          SWP_SHOWWINDOW );
        }

        // Store device Caps
        m_pd3dDevice->GetDeviceCaps( &m_d3dCaps );
        m_dwCreateFlags = pModeInfo->dwBehavior;

        // Store device description
        if( pDeviceInfo->DeviceType == D3DDEVTYPE_REF )
            StringCchCopy( m_strDeviceStats, STRING_NUM_90, TEXT("REF") );
        else if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
            StringCchCopy( m_strDeviceStats, STRING_NUM_90, TEXT("HAL") );
        else if( pDeviceInfo->DeviceType == D3DDEVTYPE_SW )
            StringCchCopy( m_strDeviceStats, STRING_NUM_90, TEXT("SW") );

        if( pModeInfo->dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
            pModeInfo->dwBehavior & D3DCREATE_PUREDEVICE )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                StringCchCat( m_strDeviceStats, STRING_NUM_90, TEXT(" (pure hw vp)") );
            else
                StringCchCat( m_strDeviceStats, STRING_NUM_90, TEXT(" (simulated pure hw vp)") );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                StringCchCat( m_strDeviceStats, STRING_NUM_90, TEXT(" (hw vp)") );
            else
                StringCchCat( m_strDeviceStats, STRING_NUM_90, TEXT(" (simulated hw vp)") );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                StringCchCat( m_strDeviceStats, STRING_NUM_90, TEXT(" (mixed vp)") );
            else
                StringCchCat( m_strDeviceStats, STRING_NUM_90, TEXT(" (simulated mixed vp)") );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
        {
            StringCchCat( m_strDeviceStats, STRING_NUM_90, TEXT(" (sw vp)") );
        }

        if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
        {
            StringCchCat( m_strDeviceStats, STRING_NUM_90, TEXT(": ") );
            StringCchCat( m_strDeviceStats, STRING_NUM_90, pAdapterInfo->d3dAdapterIdentifier.Description );
        }

        // Store render target surface desc
        LPDIRECT3DSURFACEQ pBackBuffer;
        m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
        pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
        pBackBuffer->Release();

        // Set up the fullscreen cursor
        if( m_bShowCursorWhenFullscreen && !m_bDefWin )
        {
            HCURSOR hCursor;
#ifdef _WIN64
            hCursor = (HCURSOR)GetClassLongPtr( m_hWndApp, GCLP_HCURSOR );
#else
            hCursor = (HCURSOR)GetClassLong( m_hWndApp, GCL_HCURSOR );
#endif
            m_pd3dDevice->ShowCursor( TRUE );
        }

        // Confine cursor to fullscreen window
        if( m_bClipCursorWhenFullscreen )
        {
            if (!m_bDefWin )
            {
                RECT rcWindow;
                GetWindowRect( m_hWndApp, &rcWindow );
                ClipCursor( &rcWindow );
            }
            else
            {
                ClipCursor( NULL );
            }
        }

		//	기타 장치 초기화 이전에 작업해야 할 내용들
		//
		hr = CreateObjects ();
		if ( SUCCEEDED ( hr ) )
		{
            m_bActive = TRUE;
			return hr;
		}

        // Cleanup before we try again
        InvalidateDeviceObjects();
        DeleteDeviceObjects();
        SAFE_RELEASE( m_pd3dDevice );
    }

    // If that failed, fall back to the reference rasterizer
    //if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
    //{
    //    // Select the default adapter
    //    m_dwAdapter = 0L;
    //    pAdapterInfo = &m_Adapters[m_dwAdapter];

    //    // Look for a software device
    //    for( UINT i=0L; i<pAdapterInfo->dwNumDevices; i++ )
    //    {
    //        if( pAdapterInfo->devices[i].DeviceType == D3DDEVTYPE_REF )
    //        {
    //            pAdapterInfo->dwCurrentDevice = i;
    //            pDeviceInfo = &pAdapterInfo->devices[i];
    //            m_bWindowed = pDeviceInfo->bWindowed;
    //            break;
    //        }
    //    }

    //    // Try again, this time with the reference rasterizer
    //    if( pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice].DeviceType ==
    //        D3DDEVTYPE_REF )
    //    {
    //        // Make sure main window isn't topmost, so error message is visible
    //        SetWindowPos( m_hWndApp, HWND_NOTOPMOST,
    //                      m_rcWindowBounds.left, m_rcWindowBounds.top,
    //                      ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
    //                      ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
    //                      SWP_SHOWWINDOW );
    //        AdjustWindowForChange();

    //        // Let the user know we are switching from HAL to the reference rasterizer
    //        DisplayErrorMsg( hr, MSGWARN_SWITCHEDTOREF );

    //        hr = Initialize3DEnvironment();
    //    }
    //}

    return hr;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Resize3DEnvironment()
{
    HRESULT hr;

    // Release all vidmem objects
    if( FAILED( hr = InvalidateDeviceObjects() ) )
        return hr;

    // Reset the device
    if( FAILED( hr = m_pd3dDevice->Reset( &m_d3dpp ) ) )
	{
		if( hr==D3DERR_DEVICELOST )
		{
			int a=0;
		}
		else if( hr==D3DERR_DRIVERINTERNALERROR )
		{
			int a=0;
		}
		else if( hr==D3DERR_INVALIDCALL )
		{
			int a=0;
		}
		else if( hr==D3DERR_OUTOFVIDEOMEMORY )
		{
			int a=0;
		}
		else if( hr==E_OUTOFMEMORY )
		{
			int a=0;
		}

		// Test the cooperative level to see if it's okay to render
		if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
		{
			if( hr==D3DERR_DEVICELOST )
			{
				int a=0;
			}
			else if( hr==D3DERR_DEVICENOTRESET )
			{
				int a=0;
			}
			else if( hr==D3DERR_DRIVERINTERNALERROR )
			{
				int a=0;
			}

			// If the device was lost, do not render until we get it back
			if ( D3DERR_DEVICELOST == hr )
			{
				//CDebugSet::ToLogFile ( "[ERROR] TestCooperativeLevel(), D3DERR_DEVICELOST _ Render3DEnvironment() FAILED" );
				return E_FAIL;
			}
			else if ( D3DERR_DEVICENOTRESET == hr )
			{
				if( FAILED( hr = m_pd3dDevice->Reset( &m_d3dpp ) ) )
				{
					//CDebugSet::ToLogFile ( "[ERROR] Reset(), D3DERR_DEVICELOST _ Render3DEnvironment() FAILED" );
					return E_FAIL;
				}
			}
		}
	}
	DXUTSetupCursor();

    // Store render target surface desc
    LPDIRECT3DSURFACEQ pBackBuffer;
    m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();

    // Set up the fullscreen cursor
    if( m_bShowCursorWhenFullscreen && !m_bDefWin )
    {
        HCURSOR hCursor;
#ifdef _WIN64
        hCursor = (HCURSOR)GetClassLongPtr( m_hWndApp, GCLP_HCURSOR );
#else
        hCursor = (HCURSOR)GetClassLong( m_hWndApp, GCL_HCURSOR );
#endif
        m_pd3dDevice->ShowCursor( TRUE );
    }

    // Confine cursor to fullscreen window
    if( m_bClipCursorWhenFullscreen )
    {
        if (!m_bDefWin )
        {
            RECT rcWindow;
            GetWindowRect( m_hWndApp, &rcWindow );
            ClipCursor( &rcWindow );
        }
        else
        {
            ClipCursor( NULL );
        }
    }

    // Initialize the app's device-dependent objects
    hr = RestoreDeviceObjects();
    if( FAILED(hr) )
        return hr;

    // If the app is paused, trigger the rendering of the current frame
    if( FALSE == m_bFrameMoving )
    {
        m_bSingleStep = TRUE;
        DXUtil_Timer( TIMER_START );
        DXUtil_Timer( TIMER_STOP );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ToggleFullScreen()
// Desc: Called when user toggles between fullscreen mode and windowed mode
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::ToggleFullscreen()
{
	// Get access to current adapter, device, and mode
	D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
	D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
	D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];

	// Need device change if going windowed and the current device
	// can only be fullscreen
	if( !m_bDefWin && !pDeviceInfo->bCanDoWindowed )
		return ForceWindowed();

	m_bReady = FALSE;

	// Toggle the windowed state
	m_bDefWin = !m_bDefWin;
	pDeviceInfo->bWindowed = m_bDefWin;

	// Prepare window for windowed/fullscreen change
	AdjustWindowForChange();

	// Set up the presentation parameters
	m_d3dpp.Windowed               = pDeviceInfo->bWindowed;
	if( m_bDefWin )
		m_d3dpp.MultiSampleType    = pDeviceInfo->MultiSampleTypeWindowed;
	else
		m_d3dpp.MultiSampleType    = pDeviceInfo->MultiSampleTypeFullscreen;

	m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
	m_d3dpp.hDeviceWindow          = m_hWndApp;

	// DirectX 위에 설치할 GUI를 위해서 Swap 설정 ( 준혁 )
	if( m_bEnableGUI )
		m_d3dpp.SwapEffect         = D3DSWAPEFFECT_COPY;
	else
		m_d3dpp.SwapEffect         = D3DSWAPEFFECT_DISCARD;

	if( m_bDefWin )
	{
		m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
		m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;
		m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;
		m_d3dpp.FullScreen_RefreshRateInHz = 0;
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
	else
	{
		m_d3dpp.BackBufferWidth  = pModeInfo->Width;
		m_d3dpp.BackBufferHeight = pModeInfo->Height;
		m_d3dpp.BackBufferFormat = pModeInfo->Format;
		m_d3dpp.FullScreen_RefreshRateInHz = pModeInfo->nCurrentHz;
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	// Resize the 3D device
	if( FAILED( Resize3DEnvironment() ) )
	{
		if( m_bDefWin )
			return ForceWindowed();
		else
			return E_FAIL;
	}

	// When moving from fullscreen to windowed mode, it is important to
	// adjust the window size after resetting the device rather than
	// beforehand to ensure that you get the window size you want.  For
	// example, when switching from 640x480 fullscreen to windowed with
	// a 1000x600 window on a 1024x768 desktop, it is impossible to set
	// the window size to 1000x600 until after the display mode has
	// changed to 1024x768, because windows cannot be larger than the
	// desktop.
	if( m_bDefWin )
	{
		SetWindowPos( m_hWndApp, HWND_NOTOPMOST,
						m_rcWindowBounds.left, m_rcWindowBounds.top,
						( m_rcWindowBounds.right - m_rcWindowBounds.left ),
						( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
						SWP_SHOWWINDOW );
	}

	m_bReady = TRUE;

    return S_OK;
}

DWORD CD3DApplication::FindDeviceMode ( DWORD dwWidth, DWORD dwHeight, EMSCREEN_FORMAT emFMT )
{
	// Get access to current adapter, device, and mode
	D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
	D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];

	for ( DWORD i=0; i<pDeviceInfo->dwNumModes; ++i )
	{
		D3DModeInfo &sMode = pDeviceInfo->modes[i];
		
		EMSCREEN_FORMAT emDEVFMT = EMSCREEN_F16;
		if ( sMode.Format==D3DFMT_X8R8G8B8 || sMode.Format==D3DFMT_A8R8G8B8 || sMode.Format==D3DFMT_R8G8B8 )
		{
			emDEVFMT = EMSCREEN_F32;
		}

		if ( emFMT==emDEVFMT )
		{
			if ( sMode.Width==dwWidth && sMode.Height==dwHeight )	return i;
		}
	}

	return pDeviceInfo->dwCurrentMode;
}

HRESULT CD3DApplication::ChangeDeviceMode ( DWORD dwChangeMode, DWORD dwChangeHz, BOOL bWin )
{
	// Get access to current adapter, device, and mode
	D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
	D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];

	if ( pDeviceInfo->dwNumModes <= dwChangeMode )	return E_FAIL;

	pDeviceInfo->dwCurrentMode = dwChangeMode;	//	모드 변경.
	D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];
	pModeInfo->nCurrentHz = dwChangeHz;			//	Hz 변경.

	if( bWin && !pDeviceInfo->bCanDoWindowed )	return E_FAIL;

	m_bReady = FALSE;

	m_bDefWin = bWin;
	pDeviceInfo->bWindowed = m_bDefWin;

	// Set up the presentation parameters
	m_d3dpp.Windowed               = pDeviceInfo->bWindowed;
	if( m_bDefWin )
		m_d3dpp.MultiSampleType    = pDeviceInfo->MultiSampleTypeWindowed;
	else
		m_d3dpp.MultiSampleType    = pDeviceInfo->MultiSampleTypeFullscreen;

	m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
	m_d3dpp.hDeviceWindow          = m_hWndApp;

	// DirectX 위에 설치할 GUI를 위해서 Swap 설정 ( 준혁 )
	if( m_bEnableGUI )
		m_d3dpp.SwapEffect         = D3DSWAPEFFECT_COPY;
	else
		m_d3dpp.SwapEffect         = D3DSWAPEFFECT_DISCARD;

	if( m_bDefWin )
	{
		m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
		m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;
		m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;
		m_d3dpp.FullScreen_RefreshRateInHz = 0;
	}
	else
	{
		m_d3dpp.BackBufferWidth  = pModeInfo->Width;
		m_d3dpp.BackBufferHeight = pModeInfo->Height;
		m_d3dpp.BackBufferFormat = pModeInfo->Format;
		m_d3dpp.FullScreen_RefreshRateInHz = pModeInfo->nCurrentHz;
	}

	// Resize the 3D device
	if( FAILED( Resize3DEnvironment() ) )
			return E_FAIL;

	// When moving from fullscreen to windowed mode, it is important to
	// adjust the window size after resetting the device rather than
	// beforehand to ensure that you get the window size you want.  For
	// example, when switching from 640x480 fullscreen to windowed with
	// a 1000x600 window on a 1024x768 desktop, it is impossible to set
	// the window size to 1000x600 until after the display mode has
	// changed to 1024x768, because windows cannot be larger than the
	// desktop.
	if( m_bDefWin )
	{
		SetWindowPos( m_hWndApp, HWND_NOTOPMOST,
						m_rcWindowBounds.left, m_rcWindowBounds.top,
						( m_rcWindowBounds.right - m_rcWindowBounds.left ),
						( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
						SWP_SHOWWINDOW );
	}

	m_bReady = TRUE;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ForceWindowed()
// Desc: Switch to a windowed mode, even if that means picking a new device
//       and/or adapter
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::ForceWindowed()
{
    HRESULT hr;
    D3DAdapterInfo* pAdapterInfoCur = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfoCur  = &pAdapterInfoCur->devices[pAdapterInfoCur->dwCurrentDevice];
    BOOL bFoundDevice = FALSE;

    if( pDeviceInfoCur->bCanDoWindowed )
    {
        bFoundDevice = TRUE;
    }
    else
    {
        // Look for a windowable device on any adapter
        D3DAdapterInfo* pAdapterInfo;
        DWORD dwAdapter;
        D3DDeviceInfo* pDeviceInfo;
        DWORD dwDevice;
        for( dwAdapter = 0; dwAdapter < m_dwNumAdapters; dwAdapter++ )
        {
            pAdapterInfo = &m_Adapters[dwAdapter];
            for( dwDevice = 0; dwDevice < pAdapterInfo->dwNumDevices; dwDevice++ )
            {
                pDeviceInfo = &pAdapterInfo->devices[dwDevice];
                if( pDeviceInfo->bCanDoWindowed )
                {
                    m_dwAdapter = dwAdapter;
                    pDeviceInfoCur = pDeviceInfo;
                    pAdapterInfo->dwCurrentDevice = dwDevice;
                    bFoundDevice = TRUE;
                    break;
                }
            }
            if( bFoundDevice )
                break;
        }
    }

    if( !bFoundDevice )
        return E_FAIL;

    pDeviceInfoCur->bWindowed = TRUE;
    m_bDefWin = TRUE;

    // Now destroy the current 3D device objects, then reinitialize

    m_bReady = FALSE;

    // Release all scene objects that will be re-created for the new device
    InvalidateDeviceObjects();
    DeleteDeviceObjects();

    // Release display objects, so a new device can be created
    if( m_pd3dDevice->Release() > 0L )
        return DisplayErrorMsg( D3DAPPERR_NONZEROREFCOUNT, MSGERR_APPMUSTEXIT );

    // Create the new device
    if( FAILED( hr = Initialize3DEnvironment() ) )
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    m_bReady = TRUE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Prepare the window for a possible change between windowed mode and
//       fullscreen mode.  This function is virtual and thus can be overridden
//       to provide different behavior, such as switching to an entirely
//       different window for fullscreen mode (as in the MFC sample apps).
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::AdjustWindowForChange()
{
    if( m_bDefWin )
    {
        // Set windowed-mode style
        SetWindowLong( m_hWndApp, GWL_STYLE, m_dwWindowStyle );
    }
    else
    {
        // Set fullscreen-mode style
        SetWindowLong( m_hWndApp, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE );
    }
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UserSelectNewDevice()
// Desc: Displays a dialog so the user can select a new adapter, device, or
//       display mode, and then recreates the 3D environment if needed
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::UserSelectNewDevice()
{
    HRESULT hr(S_OK);

    //// Can't display dialogs in fullscreen mode
    //if( m_bDefWin == FALSE )
    //{
    //    if( FAILED( ToggleFullscreen() ) )
    //    {
    //        DisplayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
    //        return E_FAIL;
    //    }
    //}

    //// Prompt the user to change the mode
    //if( IDOK != DialogBoxParam( (HINSTANCE)GetModuleHandle(NULL),
    //                            MAKEINTRESOURCE(IDD_SELECTDEVICE), m_hWndApp,
    //                            SelectDeviceProc, (LPARAM)this ) )
    //    return S_OK;

    //// Get access to the newly selected adapter, device, and mode
    //DWORD dwDevice;
    //dwDevice  = m_Adapters[m_dwAdapter].dwCurrentDevice;
    //m_bDefWin = m_Adapters[m_dwAdapter].devices[dwDevice].bWindowed;

    //// Release all scene objects that will be re-created for the new device
    //InvalidateDeviceObjects();
    //DeleteDeviceObjects();

    //// Release display objects, so a new device can be created
    //if( m_pd3dDevice->Release() > 0L )
    //    return DisplayErrorMsg( D3DAPPERR_NONZEROREFCOUNT, MSGERR_APPMUSTEXIT );

    //// Inform the display class of the change. It will internally
    //// re-create valid surfaces, a d3ddevice, etc.
    //if( FAILED( hr = Initialize3DEnvironment() ) )
    //    return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );

    //// If the app is paused, trigger the rendering of the current frame
    //if( FALSE == m_bFrameMoving )
    //{
    //    m_bSingleStep = TRUE;
    //    DXUtil_Timer( TIMER_START );
    //    DXUtil_Timer( TIMER_STOP );
    //}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SelectDeviceProc()
// Desc: Windows message handling function for the device select dialog
//-----------------------------------------------------------------------------
INT_PTR CALLBACK CD3DApplication::SelectDeviceProc( HWND hDlg, UINT msg,
                                                    WPARAM wParam, LPARAM lParam )
{
    // Get access to the UI controls
    //HWND hwndAdapterList        = GetDlgItem( hDlg, IDC_ADAPTER_COMBO );
    //HWND hwndDeviceList         = GetDlgItem( hDlg, IDC_DEVICE_COMBO );
    //HWND hwndFullscreenModeList = GetDlgItem( hDlg, IDC_FULLSCREENMODES_COMBO );
    //HWND hwndWindowedRadio      = GetDlgItem( hDlg, IDC_WINDOW );
    //HWND hwndFullscreenRadio    = GetDlgItem( hDlg, IDC_FULLSCREEN );
    //HWND hwndMultiSampleList    = GetDlgItem( hDlg, IDC_MULTISAMPLE_COMBO );
    //BOOL bUpdateDlgControls     = FALSE;

    //// Static state for adapter/device/mode selection
    //static CD3DApplication* pd3dApp;
    //static DWORD  dwOldAdapter, dwNewAdapter;
    //static DWORD  dwOldDevice,  dwNewDevice;
    //static DWORD  dwOldMode,    dwNewMode;
    //static BOOL   bOldWindowed, bNewWindowed;
    //static D3DMULTISAMPLE_TYPE OldMultiSampleTypeWindowed, NewMultiSampleTypeWindowed;
    //static D3DMULTISAMPLE_TYPE OldMultiSampleTypeFullscreen, NewMultiSampleTypeFullscreen;

    //// Working variables
    //D3DAdapterInfo* pAdapter;
    //D3DDeviceInfo*  pDevice;

    //// Handle the initialization message
    //if( WM_INITDIALOG == msg )
    //{
    //    // Old state
    //    pd3dApp      = (CD3DApplication*)lParam;
    //    dwOldAdapter = pd3dApp->m_dwAdapter;
    //    pAdapter     = &pd3dApp->m_Adapters[dwOldAdapter];

    //    dwOldDevice  = pAdapter->dwCurrentDevice;
    //    pDevice      = &pAdapter->devices[dwOldDevice];

    //    dwOldMode    = pDevice->dwCurrentMode;
    //    bOldWindowed = pDevice->bWindowed;
    //    OldMultiSampleTypeWindowed = pDevice->MultiSampleTypeWindowed;
    //    OldMultiSampleTypeFullscreen = pDevice->MultiSampleTypeFullscreen;

    //    // New state is initially the same as the old state
    //    dwNewAdapter = dwOldAdapter;
    //    dwNewDevice  = dwOldDevice;
    //    dwNewMode    = dwOldMode;
    //    bNewWindowed = bOldWindowed;
    //    NewMultiSampleTypeWindowed = OldMultiSampleTypeWindowed;
    //    NewMultiSampleTypeFullscreen = OldMultiSampleTypeFullscreen;

    //    // Set flag to update dialog controls below
    //    bUpdateDlgControls = TRUE;
    //}

    //if( WM_COMMAND == msg )
    //{
    //    // Get current UI state
    //    bNewWindowed  = Button_GetCheck( hwndWindowedRadio );

    //    if( IDOK == LOWORD(wParam) )
    //    {
    //        // Handle the case when the user hits the OK button. Check if any
    //        // of the options were changed
    //        if( dwNewAdapter != dwOldAdapter || dwNewDevice  != dwOldDevice  ||
    //            dwNewMode    != dwOldMode    || bNewWindowed != bOldWindowed ||
    //            NewMultiSampleTypeWindowed != OldMultiSampleTypeWindowed ||
    //            NewMultiSampleTypeFullscreen != OldMultiSampleTypeFullscreen )
    //        {
    //            pd3dApp->m_dwAdapter = dwNewAdapter;

    //            pAdapter = &pd3dApp->m_Adapters[dwNewAdapter];
    //            pAdapter->dwCurrentDevice = dwNewDevice;

    //            pAdapter->devices[dwNewDevice].dwCurrentMode = dwNewMode;
    //            pAdapter->devices[dwNewDevice].bWindowed     = bNewWindowed;
    //            pAdapter->devices[dwNewDevice].MultiSampleTypeWindowed = NewMultiSampleTypeWindowed;
    //            pAdapter->devices[dwNewDevice].MultiSampleTypeFullscreen = NewMultiSampleTypeFullscreen;

    //            EndDialog( hDlg, IDOK );
    //        }
    //        else
    //            EndDialog( hDlg, IDCANCEL );

    //        return TRUE;
    //    }
    //    else if( IDCANCEL == LOWORD(wParam) )
    //    {
    //        // Handle the case when the user hits the Cancel button
    //        EndDialog( hDlg, IDCANCEL );
    //        return TRUE;
    //    }
    //    else if( CBN_SELENDOK == HIWORD(wParam) )
    //    {
    //        if( LOWORD(wParam) == IDC_ADAPTER_COMBO )
    //        {
    //            dwNewAdapter = ComboBox_GetCurSel( hwndAdapterList );
    //            pAdapter     = &pd3dApp->m_Adapters[dwNewAdapter];

    //            dwNewDevice  = pAdapter->dwCurrentDevice;
    //            dwNewMode    = pAdapter->devices[dwNewDevice].dwCurrentMode;
    //            bNewWindowed = pAdapter->devices[dwNewDevice].bWindowed;
    //        }
    //        else if( LOWORD(wParam) == IDC_DEVICE_COMBO )
    //        {
    //            pAdapter     = &pd3dApp->m_Adapters[dwNewAdapter];

    //            dwNewDevice  = ComboBox_GetCurSel( hwndDeviceList );
    //            dwNewMode    = pAdapter->devices[dwNewDevice].dwCurrentMode;
    //            bNewWindowed = pAdapter->devices[dwNewDevice].bWindowed;
    //        }
    //        else if( LOWORD(wParam) == IDC_FULLSCREENMODES_COMBO )
    //        {
    //            dwNewMode = ComboBox_GetCurSel( hwndFullscreenModeList );
    //        }
    //        else if( LOWORD(wParam) == IDC_MULTISAMPLE_COMBO )
    //        {
    //            DWORD dwItem = ComboBox_GetCurSel( hwndMultiSampleList );
    //            if( bNewWindowed )
    //                NewMultiSampleTypeWindowed = (D3DMULTISAMPLE_TYPE)ComboBox_GetItemData( hwndMultiSampleList, dwItem );
    //            else
    //                NewMultiSampleTypeFullscreen = (D3DMULTISAMPLE_TYPE)ComboBox_GetItemData( hwndMultiSampleList, dwItem );
    //        }
    //    }
    //    // Keep the UI current
    //    bUpdateDlgControls = TRUE;
    //}

    //// Update the dialog controls
    //if( bUpdateDlgControls )
    //{
    //    // Reset the content in each of the combo boxes
    //    ComboBox_ResetContent( hwndAdapterList );
    //    ComboBox_ResetContent( hwndDeviceList );
    //    ComboBox_ResetContent( hwndFullscreenModeList );
    //    ComboBox_ResetContent( hwndMultiSampleList );

    //    pAdapter = &pd3dApp->m_Adapters[dwNewAdapter];
    //    pDevice  = &pAdapter->devices[dwNewDevice];

    //    // Add a list of adapters to the adapter combo box
    //    for( DWORD a=0; a < pd3dApp->m_dwNumAdapters; a++ )
    //    {
    //        // Add device name to the combo box
    //        DWORD dwItem = ComboBox_AddString( hwndAdapterList,
    //                         pd3dApp->m_Adapters[a].d3dAdapterIdentifier.Description );

    //        // Set the item data to identify this adapter
    //        ComboBox_SetItemData( hwndAdapterList, dwItem, a );

    //        // Set the combobox selection on the current adapater
    //        if( a == dwNewAdapter )
    //            ComboBox_SetCurSel( hwndAdapterList, dwItem );
    //    }

    //    // Add a list of devices to the device combo box
    //    for( DWORD d=0; d < pAdapter->dwNumDevices; d++ )
    //    {
    //        // Add device name to the combo box
    //        DWORD dwItem = ComboBox_AddString( hwndDeviceList,
    //                                           pAdapter->devices[d].strDesc );

    //        // Set the item data to identify this device
    //        ComboBox_SetItemData( hwndDeviceList, dwItem, d );

    //        // Set the combobox selection on the current device
    //        if( d == dwNewDevice )
    //            ComboBox_SetCurSel( hwndDeviceList, dwItem );
    //    }

    //    // Add a list of modes to the mode combo box
    //    for( DWORD m=0; m < pDevice->dwNumModes; m++ )
    //    {
    //        DWORD BitDepth = 16;
    //        if( pDevice->modes[m].Format == D3DFMT_X8R8G8B8 ||
    //            pDevice->modes[m].Format == D3DFMT_A8R8G8B8 ||
    //            pDevice->modes[m].Format == D3DFMT_R8G8B8 )
    //        {
    //            BitDepth = 32;
    //        }

    //        // Add mode desc to the combo box
    //        TCHAR strMode[80];
    //        _stprintf( strMode, _T("%ld x %ld x %ld"), pDevice->modes[m].Width,
    //                                                   pDevice->modes[m].Height,
    //                                                   BitDepth );
    //        DWORD dwItem = ComboBox_AddString( hwndFullscreenModeList, strMode );

    //        // Set the item data to identify this mode
    //        ComboBox_SetItemData( hwndFullscreenModeList, dwItem, m );

    //        // Set the combobox selection on the current mode
    //        if( m == dwNewMode )
    //            ComboBox_SetCurSel( hwndFullscreenModeList, dwItem );
    //    }

    //    // Add a list of multisample modes to the multisample combo box
    //    for( m=0; m <= 16; m++ )
    //    {
    //        TCHAR strDesc[50];

    //        D3DFORMAT fmt;
    //        if( bNewWindowed )
    //            fmt = pd3dApp->m_Adapters[dwNewAdapter].d3ddmDesktop.Format;
    //        else
    //            fmt = pDevice->modes[dwNewMode].Format;

    //        if ( m == 1 ) // 1 is not a valid multisample type
    //            continue;

    //        if( SUCCEEDED( pd3dApp->m_pD3D->CheckDeviceMultiSampleType( dwNewAdapter,
    //            pDevice->DeviceType, fmt, bNewWindowed, (D3DMULTISAMPLE_TYPE)m ) ) )
    //        {
    //            if( m == 0 )
    //                StringCchCopy( strDesc, 50, _T("none") );
    //            else
    //                 StringCchPrintf( strDesc, 50, _T("%d samples"), m );

    //            // Add device name to the combo box
    //            DWORD dwItem = ComboBox_AddString( hwndMultiSampleList, strDesc );

    //            // Set the item data to identify this multisample type
    //            ComboBox_SetItemData( hwndMultiSampleList, dwItem, m );

    //            // Set the combobox selection on the current multisample type
    //            if( bNewWindowed )
    //            {
    //                if( (D3DMULTISAMPLE_TYPE)m == NewMultiSampleTypeWindowed || m == 0 )
    //                    ComboBox_SetCurSel( hwndMultiSampleList, dwItem );
    //            }
    //            else
    //            {
    //                if( (D3DMULTISAMPLE_TYPE)m == NewMultiSampleTypeFullscreen || m == 0 )
    //                    ComboBox_SetCurSel( hwndMultiSampleList, dwItem );
    //            }
    //        }
    //    }
    //    DWORD dwItem = ComboBox_GetCurSel( hwndMultiSampleList );
    //    if( bNewWindowed )
    //        NewMultiSampleTypeWindowed = (D3DMULTISAMPLE_TYPE)ComboBox_GetItemData( hwndMultiSampleList, dwItem );
    //    else
    //        NewMultiSampleTypeFullscreen = (D3DMULTISAMPLE_TYPE)ComboBox_GetItemData( hwndMultiSampleList, dwItem );
    //    EnableWindow( hwndMultiSampleList, ComboBox_GetCount( hwndMultiSampleList ) > 1);
    //    EnableWindow( hwndWindowedRadio, pDevice->bCanDoWindowed );

    //    if( bNewWindowed )
    //    {
    //        Button_SetCheck( hwndWindowedRadio,   TRUE );
    //        Button_SetCheck( hwndFullscreenRadio, FALSE );
    //        EnableWindow( hwndFullscreenModeList, FALSE );
    //    }
    //    else
    //    {
    //        Button_SetCheck( hwndWindowedRadio,   FALSE );
    //        Button_SetCheck( hwndFullscreenRadio, TRUE );
    //        EnableWindow( hwndFullscreenModeList, TRUE );
    //    }
    //    return TRUE;
    //}

    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: Run()
// Desc:
//-----------------------------------------------------------------------------
INT CD3DApplication::Run()
{
    // Load keyboard accelerators
//    HACCEL hAccel = LoadAccelerators( NULL, MAKEINTRESOURCE(IDR_MAIN_ACCEL) );

    // Now we're ready to recieve and process Windows messages.
    BOOL bGotMsg;
    MSG  msg;
    msg.message = WM_NULL;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

    while( WM_QUIT != msg.message  )
    {
        // Use PeekMessage() if the app is active, so we can use idle time to
        // render the scene. Else, use GetMessage() to avoid eating CPU time.
        if( m_bActive )
            bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
        else
            bGotMsg = GetMessage( &msg, NULL, 0U, 0U );

        if( bGotMsg )
        {
            // Translate and dispatch the message
            //if( 0 == TranslateAccelerator( m_hWndApp, hAccel, &msg ) )
            //{
            //    TranslateMessage( &msg );
            //    DispatchMessage( &msg );
            //}
        }
        else
        {
            // Render a frame during idle time (no messages are waiting)
            if( m_bActive && m_bReady )
            {
                if( FAILED( Render3DEnvironment() ) )
                    SendMessage( m_hWndApp, WM_CLOSE, 0, 0 );
            }
        }
    }

    return (INT)msg.wParam;
}


HRESULT CD3DApplication::FrameMove3DEnvironment()
{
	HRESULT hr;

    // Get the app's time, in seconds. Skip rendering if no time elapsed
    FLOAT fAppTime        = DXUtil_Timer( TIMER_GETAPPTIME );
    FLOAT fElapsedAppTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );
    if( ( 0.0f == fElapsedAppTime ) && m_bFrameMoving )
        return S_OK;

    // FrameMove (animate) the scene
    if( m_bFrameMoving || m_bSingleStep )
    {
        // Store the time for the app
        m_fTime        = fAppTime;
        m_fElapsedTime = fElapsedAppTime;

        // Frame move the scene
        if( FAILED( hr = FrameMove() ) )
            return hr;

		Sleep( 20 );

        m_bSingleStep = FALSE;
    }

#ifndef NDEBUG
    // Keep track of the frame count
    {
        static FLOAT fLastTime = 0.0f;
        static DWORD dwFrames  = 0L;
        FLOAT fTime = DXUtil_Timer( TIMER_GETABSOLUTETIME );
        ++dwFrames;

        // Update the scene stats once per second
        if( fTime - fLastTime > 1.0f )
        {
            m_fFPS    = dwFrames / (fTime - fLastTime);
            fLastTime = fTime;
            dwFrames  = 0L;
        }
    }
#endif

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render3DEnvironment()
// Desc: Draws the scene.
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Render3DEnvironment()
{
    HRESULT hr;

    // Test the cooperative level to see if it's okay to render
    if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
    {
        // If the device was lost, do not render until we get it back
        if( D3DERR_DEVICELOST == hr )
		{
			//CDebugSet::ToLogFile ( "[ERROR] Reset(), D3DERR_DEVICELOST _ Render3DEnvironment() FAILED" );
			Sleep( 50 );
            return S_OK;
		}

        // Check if the device needs to be resized.
        if( D3DERR_DEVICENOTRESET == hr )
        {
            // If we are windowed, read the desktop mode and use the same format for
            // the back buffer
            if( m_bDefWin )
            {
                D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
                m_pD3D->GetAdapterDisplayMode( m_dwAdapter, &pAdapterInfo->d3ddmDesktop );
                m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;
            }

            if( FAILED( hr = Resize3DEnvironment() ) )		return hr;
        }
        return hr;
    }

	static double fBeforeTime = 0.0f;
	static double fCurrentTime = 0.0f;
	static double fTimeDelta = 0.0f;

	if( !m_bDefWin || g_bFRAME_LIMIT )
	{
		// 현재 시간을 얻어옴
		fCurrentTime = DXUtil_Timer( TIMER_GETAPPTIME );

		// 다음 랜더링 될 시점을 구함
		fTimeDelta = fBeforeTime+0.033f;

		// 만약 현재가 첫 프레임이나 timeGetTime()이 랜더링될 시점의 시간보다 크면 랜더링
		if( fBeforeTime == 0.0f || fCurrentTime >= fTimeDelta )	
		{
			// 메인루프 처리
			// 현재 시간을 g_BeforeTime에 넣음
			fBeforeTime = fCurrentTime;
		}
		else
		{
			Sleep(0);
			return S_OK;
		}
	}

	// Get the app's time, in seconds. Skip rendering if no time elapsed
    FLOAT fAppTime        = DXUtil_Timer( TIMER_GETAPPTIME );
    FLOAT fElapsedAppTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );

	if( ( 0.0f == fElapsedAppTime ) && m_bFrameMoving )
        return S_OK;
	
	// FrameMove (animate) the scene
	if( m_bFrameMoving || m_bSingleStep )
	{
		// Store the time for the app
		m_fTime        = fAppTime;
		m_fElapsedTime = fElapsedAppTime;
	
		// Frame move the scene
		if( FAILED( hr = FrameMove() ) )
			return hr;
		
		m_bSingleStep = FALSE;
	}

	// Render the scene as normal
	if( FAILED( hr = Render() ) )
		return hr;

	// Show the frame on the primary surface.
	Present();

//#ifndef NDEBUG
    // Keep track of the frame count
    {
        static FLOAT fLastTime = 0.0f;
        static DWORD dwFrames  = 0L;
        FLOAT fTime = DXUtil_Timer( TIMER_GETABSOLUTETIME );
        ++dwFrames;

        // Update the scene stats once per second
        if( fTime - fLastTime > 1.0f )
        {
            m_fFPS    = dwFrames / (fTime - fLastTime);
            fLastTime = fTime;
            dwFrames  = 0L;

            // Get adapter's current mode so we can report
            // bit depth (back buffer depth may be unknown)
            D3DDISPLAYMODE mode;
            m_pD3D->GetAdapterDisplayMode(m_dwAdapter, &mode);

            _stprintf_s( m_strFrameStats, _T("%.02f fps (%dx%dx%d)"), m_fFPS,
                       m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height,
                       mode.Format==D3DFMT_X8R8G8B8?32:16 );
            D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
            D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
            D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];
            if( m_bUseDepthBuffer )
            {
                switch( pModeInfo->DepthStencilFormat )
                {
                case D3DFMT_D16:
                    StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (D16)") );
                    break;
                case D3DFMT_D15S1:
                    StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (D15S1)") );
                    break;
                case D3DFMT_D24X8:
                    StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (D24X8)") );
                    break;
                case D3DFMT_D24S8:
                    StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (D24S8)") );
                    break;
                case D3DFMT_D24X4S4:
                    StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (D24X4S4)") );
                    break;
                case D3DFMT_D32:
                    StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (D32)") );
                    break;
                }
            }

            D3DMULTISAMPLE_TYPE MultiSampleType;
            if( m_bDefWin )
                MultiSampleType = pDeviceInfo->MultiSampleTypeWindowed;
            else
                MultiSampleType = pDeviceInfo->MultiSampleTypeFullscreen;

            switch( MultiSampleType )
            {
            case D3DMULTISAMPLE_2_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (2x Multisample)") );
                break;
            case D3DMULTISAMPLE_3_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (3x Multisample)") );
                break;
            case D3DMULTISAMPLE_4_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (4x Multisample)") );
                break;
            case D3DMULTISAMPLE_5_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (5x Multisample)") );
                break;
            case D3DMULTISAMPLE_6_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (6x Multisample)") );
                break;
            case D3DMULTISAMPLE_7_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (7x Multisample)") );
                break;
            case D3DMULTISAMPLE_8_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (8x Multisample)") );
                break;
            case D3DMULTISAMPLE_9_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (9x Multisample)") );
                break;
            case D3DMULTISAMPLE_10_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (10x Multisample)") );
                break;
            case D3DMULTISAMPLE_11_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (11x Multisample)") );
                break;
            case D3DMULTISAMPLE_12_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (12x Multisample)") );
                break;
            case D3DMULTISAMPLE_13_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (13x Multisample)") );
                break;
            case D3DMULTISAMPLE_14_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (14x Multisample)") );
                break;
            case D3DMULTISAMPLE_15_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (15x Multisample)") );
                break;
            case D3DMULTISAMPLE_16_SAMPLES:
                StringCchCat( m_strFrameStats, STRING_NUM_90, _T(" (16x Multisample)") );
                break;
            }
        }
    }
//#endif

//	if( !m_bDefWin || g_bFRAME_LIMIT )
//	{
//		// Note : 프레임을 고정시킨다.
////		if( g_bFRAME_LIMIT || m_bFrameLimit )
//		static FLOAT fLastTime;
//
//		while(1)
//		{
//			fLastTime = DXUtil_Timer( TIMER_GETAPPTIME );
//			fLastTime = fLastTime - fAppTime;
//
//			//if( (0.016f < fLastTime) )  // 60 프레임
//			if( (0.033f < fLastTime) )  // 30프레임
//				break;
//
//			Sleep( 0 );
//		}
//
//	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name : Present()
// DirectX 위에 설치된 GUI를 위해서 별도로 Present 관리 ( 준혁 )
//-----------------------------------------------------------------------------
VOID CD3DApplication::Present()
{
	if( m_bVisibleGUI )
	{
		RECT rcTop = { 0, 0, (LONG)m_d3dsdBackBuffer.Width, m_rtBoundGUI.top };
		RECT rcBottom = { 0, m_rtBoundGUI.bottom, (LONG)m_d3dsdBackBuffer.Width, (LONG)m_d3dsdBackBuffer.Height };
		RECT rcLeft	= { 0, m_rtBoundGUI.top, m_rtBoundGUI.left, m_rtBoundGUI.bottom };
		RECT rcRight = { m_rtBoundGUI.right, m_rtBoundGUI.top, (LONG)m_d3dsdBackBuffer.Width, m_rtBoundGUI.bottom };

		m_pd3dDevice->Present( &rcTop, &rcTop, NULL, NULL );
		m_pd3dDevice->Present( &rcBottom, &rcBottom, NULL, NULL );
		m_pd3dDevice->Present( &rcLeft, &rcLeft, NULL, NULL );
		m_pd3dDevice->Present( &rcRight, &rcRight, NULL, NULL );
	}
	else
	{
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
}




//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Called in to toggle the pause state of the app.
//-----------------------------------------------------------------------------
VOID CD3DApplication::Pause( BOOL bPause )
{
    static DWORD dwAppPausedCount = 0L;

    dwAppPausedCount += ( bPause ? +1 : -1 );
    m_bReady          = ( dwAppPausedCount ? FALSE : TRUE );

    // Handle the first pause request (of many, nestable pause requests)
    if( bPause && ( 1 == dwAppPausedCount ) )
    {
        // Stop the scene from animating
        if( m_bFrameMoving )
            DXUtil_Timer( TIMER_STOP );
    }

    if( 0 == dwAppPausedCount )
    {
        // Restart the timers
        if( m_bFrameMoving )
            DXUtil_Timer( TIMER_START );
    }
}




//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
VOID CD3DApplication::Cleanup3DEnvironment()
{
    m_bActive = FALSE;
    m_bReady  = FALSE;

    if( m_pd3dDevice )
    {
        InvalidateDeviceObjects();
        DeleteDeviceObjects();

        FinalCleanup();

        m_pd3dDevice->Release();
        m_pD3D->Release();

        m_pd3dDevice = NULL;
        m_pD3D       = NULL;
    }
    else 
    {
        FinalCleanup();
    }
}




//-----------------------------------------------------------------------------
// Name: DisplayErrorMsg()
// Desc: Displays error messages in a message box
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::DisplayErrorMsg( HRESULT hr, DWORD dwType )
{
    TCHAR strMsg[512];

    switch( hr )
    {
        case D3DAPPERR_NODIRECT3D:
            _tcscpy_s( strMsg, _T("Could not initialize Direct3D. You may\n")
                             _T("want to check that the latest version of\n")
                             _T("DirectX is correctly installed on your\n")
                             _T("system.  Also make sure that this program\n")
                             _T("was compiled with header files that match\n")
                             _T("the installed DirectX DLLs.") );
            break;

        case D3DAPPERR_NOCOMPATIBLEDEVICES:
            _tcscpy_s( strMsg, _T("Could not find any compatible Direct3D\n")
                             _T("devices.") );
            break;

        case D3DAPPERR_NOWINDOWABLEDEVICES:
            _tcscpy_s( strMsg, _T("This program cannot run in a desktop\n")
                             _T("window with the current display settings.\n")
                             _T("Please change your desktop settings to a\n")
                             _T("16- or 32-bit display mode and re-run this\n")
                             _T("application.") );
            break;

        case D3DAPPERR_NOHARDWAREDEVICE:
            _tcscpy_s( strMsg, _T("No hardware-accelerated Direct3D devices\n")
                             _T("were found.") );
            break;

        case D3DAPPERR_HALNOTCOMPATIBLE:
            _tcscpy_s( strMsg, _T("This program requires functionality that is\n")
                             _T("not available on your Direct3D hardware\n")
                             _T("accelerator.") );
            break;

        case D3DAPPERR_NOWINDOWEDHAL:
            _tcscpy_s( strMsg, _T("Your Direct3D hardware accelerator cannot\n")
                             _T("render into a window.\n") );
            break;

        case D3DAPPERR_NODESKTOPHAL:
            _tcscpy_s( strMsg, _T("Your Direct3D hardware accelerator cannot\n")
                             _T("render into a window with the current\n")
                             _T("desktop display settings.\n") );
            break;

        case D3DAPPERR_NOHALTHISMODE:
            _tcscpy_s( strMsg, _T("This program requires functionality that is\n")
                             _T("not available on your Direct3D hardware\n")
                             _T("accelerator with the current desktop display\n")
                             _T("settings.\n") );
            break;

        case D3DAPPERR_MEDIANOTFOUND:
            _tcscpy_s( strMsg, _T("Could not load required media." ) );
            break;

        case D3DAPPERR_RESIZEFAILED:
            _tcscpy_s( strMsg, _T("Could not reset the Direct3D device." ) );
            break;

        case D3DAPPERR_NONZEROREFCOUNT:
            _tcscpy_s( strMsg, _T("A D3D object has a non-zero reference\n")
                             _T("count (meaning things were not properly\n")
                             _T("cleaned up).") );
            break;

        case D3DAPPERR_NULLREFDEVICE:
            _tcscpy_s( strMsg, _T("Warning: Nothing will be rendered.\n")
                             _T("The reference rendering device was selected, but your\n")
                             _T("computer only has a reduced-functionality reference device\n")
                             _T("installed.  Install the DirectX SDK to get the full\n")
                             _T("reference device.\n") );
            break;

        case E_OUTOFMEMORY:
            _tcscpy_s( strMsg, _T("Not enough memory.") );
            break;

        case D3DERR_OUTOFVIDEOMEMORY:
            _tcscpy_s( strMsg, _T("Not enough video memory.") );
            break;

        default:
            _tcscpy_s( strMsg, _T("Generic application error. Enable\n")
                             _T("debug output for detailed information.") );
    }

    if( MSGERR_APPMUSTEXIT == dwType )
    {
        _tcscat_s( strMsg, _T("\n\nThis application will now exit.") );
        MessageBox( NULL, strMsg, m_strWindowTitle, MB_ICONERROR|MB_OK );

        // Close the window, which shuts down the app
        if( m_hWndApp )
            SendMessage( m_hWndApp, WM_CLOSE, 0, 0 );
    }
    else
    {
        if( MSGWARN_SWITCHEDTOREF == dwType )
            _tcscat_s( strMsg, _T("\n\nSwitching to the reference rasterizer,\n")
                             _T("a software device that implements the entire\n")
                             _T("Direct3D feature set, but runs very slowly.") );
        MessageBox( NULL, strMsg, m_strWindowTitle, MB_ICONWARNING|MB_OK );
    }

    return hr;
}




