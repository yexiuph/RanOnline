//-----------------------------------------------------------------------------
// File: D3DApp.h
//
// Desc: Application class for the Direct3D samples framework library.
//-----------------------------------------------------------------------------
#ifndef D3DAPP_H
#define D3DAPP_H

// basetsd.h defines INT_PTR (used below).  It is not included by default
// under VC 5.0.  If you are using VC6 or later, it is included via Windows.h.
#include <basetsd.h> 
#include <d3d9types.h> 
#include <d3d9caps.h> 
#include <d3d9.h> 

#include "CList.h"

//-----------------------------------------------------------------------------
// Error codes
//-----------------------------------------------------------------------------
enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOREF };

#define STRING_NUM_90	90

#define D3DAPPERR_NODIRECT3D          0x82000001
#define D3DAPPERR_NOWINDOW            0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE    0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE    0x82000006
#define D3DAPPERR_NOWINDOWEDHAL       0x82000007
#define D3DAPPERR_NODESKTOPHAL        0x82000008
#define D3DAPPERR_NOHALTHISMODE       0x82000009
#define D3DAPPERR_NONZEROREFCOUNT     0x8200000a
#define D3DAPPERR_MEDIANOTFOUND       0x8200000b
#define D3DAPPERR_RESIZEFAILED        0x8200000c
#define D3DAPPERR_NULLREFDEVICE       0x8200000d


//-----------------------------------------------------------------------------
// Name: struct D3DModeInfo
// Desc: Structure for holding information about a display mode
//-----------------------------------------------------------------------------
struct D3DModeInfo
{
    DWORD			Width;      // Screen width in this mode
    DWORD			Height;     // Screen height in this mode
    D3DFORMAT		Format;     // Pixel format in this mode

	UINT			nCurrentHz;	//
	CMList<UINT>	RefreshRate;// 

    DWORD			dwBehavior; // Hardware / Software / Mixed vertex processing
    D3DFORMAT		DepthStencilFormat; // Which depth/stencil format to use with this mode

	D3DModeInfo () :
		Width(0),
		Height(0),
		Format(D3DFMT_R5G6B5),
		nCurrentHz(0),
		dwBehavior(0),
		DepthStencilFormat(D3DFMT_D16)
	{
	}
};




//-----------------------------------------------------------------------------
// Name: struct D3DDeviceInfo
// Desc: Structure for holding information about a Direct3D device, including
//       a list of modes compatible with this device
//-----------------------------------------------------------------------------
struct D3DDeviceInfo
{
	enum
	{
		EMMAX_MODES	= 150,
	};

    // Device data
    D3DDEVTYPE   DeviceType;      // Reference, HAL, etc.
    D3DCAPSQ     d3dCaps;         // Capabilities of this device
    const TCHAR* strDesc;         // Name of this device
    BOOL         bCanDoWindowed;  // Whether this device can work in windowed mode

    // Modes for this device
    DWORD        dwNumModes;
    D3DModeInfo  modes[EMMAX_MODES];

    // Current state
    DWORD        dwCurrentMode;
    BOOL         bWindowed;
    D3DMULTISAMPLE_TYPE MultiSampleTypeWindowed;
    D3DMULTISAMPLE_TYPE MultiSampleTypeFullscreen;
};




//-----------------------------------------------------------------------------
// Name: struct D3DAdapterInfo
// Desc: Structure for holding information about an adapter, including a list
//       of devices available on this adapter
//-----------------------------------------------------------------------------
struct D3DAdapterInfo
{
    // Adapter data
    D3DADAPTER_IDENTIFIER9 d3dAdapterIdentifier;
    D3DDISPLAYMODE d3ddmDesktop;      // Desktop display mode for this adapter

    // Devices for this adapter
    DWORD          dwNumDevices;
    D3DDeviceInfo  devices[5];

    // Current state
    DWORD          dwCurrentDevice;
};

enum EMSCREEN_FORMAT
{
	EMSCREEN_F16		= 0,
	EMSCREEN_F32		= 1,
};

//-----------------------------------------------------------------------------
// Name: class CD3DApplication
// Desc: A base class for creating sample D3D8 applications. To create a simple
//       Direct3D application, simply derive this class into a class (such as
//       class CMyD3DApplication) and override the following functions, as 
//       needed:
//          OneTimeSceneInit()    - To initialize app data (alloc mem, etc.)
//          InitDeviceObjects()   - To initialize the 3D scene objects
//          FrameMove()           - To animate the scene
//          Render()              - To render the scene
//          DeleteDeviceObjects() - To cleanup the 3D scene objects
//          FinalCleanup()        - To cleanup app data (for exitting the app)
//          MsgProc()             - To handle Windows messages
//-----------------------------------------------------------------------------
class CD3DApplication
{
public:
	static BOOL			m_bEnableGUI;

protected:
	DWORD				m_dwDefScrX;
	DWORD				m_dwDefScrY;
	EMSCREEN_FORMAT		m_emDefScrFmt;
	UINT				m_uDefScrHz;
	static BOOL			m_bDefWin;

protected:
	// Internal variables for the state of the app
	D3DAdapterInfo    m_Adapters[10];
	DWORD             m_dwNumAdapters;
	DWORD             m_dwAdapter;
	BOOL              m_bActive;
	BOOL              m_bReady;
	BOOL              m_bHasFocus;

	// Internal variables used for timing
	BOOL              m_bFrameMoving;
	BOOL              m_bSingleStep;

	// Internal error handling function
	HRESULT DisplayErrorMsg( HRESULT hr, DWORD dwType );

	// Internal functions to manage and render the 3D scene
	HRESULT BuildDeviceList();
	BOOL    FindDepthStencilFormat( UINT iAdapter, D3DDEVTYPE DeviceType,
				D3DFORMAT TargetFormat, D3DFORMAT* pDepthStencilFormat );
	HRESULT Initialize3DEnvironment();
	virtual HRESULT Resize3DEnvironment();
	HRESULT ToggleFullscreen();
	HRESULT ForceWindowed();
	HRESULT UserSelectNewDevice();

	virtual HRESULT AdjustWindowForChange();
	static INT_PTR CALLBACK SelectDeviceProc( HWND hDlg, UINT msg, 
					WPARAM wParam, LPARAM lParam );

public:
	HRESULT BuildDeviceListSet();
	DWORD GetCurrentMode ()
	{ 
	    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
		D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
		return pDeviceInfo->dwCurrentMode;
	}

	DWORD FindDeviceMode ( DWORD dwWidth, DWORD dwHeight, EMSCREEN_FORMAT emFMT );
	HRESULT ChangeDeviceMode ( DWORD dwChangeMode, DWORD dwChangeHz, BOOL bWin );

public:
	D3DDeviceInfo* GetCurDeviceInfo ()
	{
	    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
		D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
		
		return pDeviceInfo;
	}

	D3DAdapterInfo* GetCurAdapterInfo ()
	{
		D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
		
		return pAdapterInfo;
	}

public:
	virtual HRESULT FrameMove3DEnvironment();
	virtual HRESULT Render3DEnvironment();
	VOID	Cleanup3DEnvironment();

protected:
	// Main objects used for creating and rendering the 3D scene
	D3DPRESENT_PARAMETERS m_d3dpp;         // Parameters for CreateDevice/Reset
	HWND              m_hWndApp;              // The main app window
	HWND              m_hWndFocus;         // The D3D focus window (usually same as m_hWnd)
	LPDIRECT3DQ       m_pD3D;              // The main D3D object
	LPDIRECT3DDEVICEQ m_pd3dDevice;        // The D3D rendering device
	D3DCAPSQ          m_d3dCaps;           // Caps for the device
	D3DSURFACE_DESC   m_d3dsdBackBuffer;   // Surface desc of the backbuffer
	DWORD             m_dwCreateFlags;     // Indicate sw or hw vertex processing
	DWORD             m_dwWindowStyle;     // Saved window style for mode switches
	RECT              m_rcWindowBounds;    // Saved window bounds for mode switches
	RECT              m_rcWindowClient;    // Saved client area size for mode switches

	// Variables for timing
	static FLOAT      m_fTime;             // Current time in seconds
	static FLOAT      m_fElapsedTime;      // Time elapsed since last frame
	FLOAT             m_fFPS;              // Instanteous frame rate

	TCHAR             m_strDeviceStats[STRING_NUM_90];// String to hold D3D device stats
	TCHAR             m_strFrameStats[STRING_NUM_90]; // String to hold frame stats

	// Overridable variables for the app
	TCHAR*            m_strWindowTitle;    // Title for the app's window
	BOOL              m_bUseDepthBuffer;   // Whether to autocreate depthbuffer
	DWORD             m_dwMinDepthBits;    // Minimum number of bits needed in depth buffer
	DWORD             m_dwMinStencilBits;  // Minimum number of bits needed in stencil buffer
	DWORD             m_dwCreationWidth;   // Width used to create window
	DWORD             m_dwCreationHeight;  // Height used to create window
	BOOL              m_bShowCursorWhenFullscreen; // Whether to show cursor when fullscreen
	BOOL              m_bClipCursorWhenFullscreen; // Whether to limit cursor pos when fullscreen

	// DirectX 위에 설치된 GUI의 활성화 여부와 영역 ( 준혁 )
	BOOL			  m_bVisibleGUI;
	RECT			  m_rtBoundGUI;

	// Overridable functions for the 3D scene created by the app
	virtual HRESULT ConfirmDevice(D3DCAPSQ*,DWORD,D3DFORMAT)   { return S_OK; }
	virtual HRESULT OneTimeSceneInit()                         { return S_OK; }
	virtual HRESULT InitDeviceObjects()                        { return S_OK; }
	virtual HRESULT RestoreDeviceObjects()                     { return S_OK; }

	virtual HRESULT CreateObjects();

	virtual HRESULT FrameMove()                                { return S_OK; }
	virtual HRESULT Render()                                   { return S_OK; }
	virtual HRESULT InvalidateDeviceObjects()                  { return S_OK; }
	virtual HRESULT DeleteDeviceObjects()                      { return S_OK; }
	virtual HRESULT FinalCleanup()                             { return S_OK; }							

public:
	static float GetCurrentTime ()	{ return m_fTime; }
	static float GetElapsedTime ()	{ return m_fElapsedTime; }

public:
	// DirectX 위에 설치된 GUI를 위해서 별도로 Present 관리 ( 준혁 )
	VOID	Present();

	void SetScreen ( DWORD dwDefScrX, DWORD dwDefScrY, EMSCREEN_FORMAT emDefScrFmt, UINT uDefScrHz, BOOL bWin )
	{
		m_dwDefScrX = dwDefScrX;
		m_dwDefScrY = dwDefScrY;
		m_emDefScrFmt = emDefScrFmt;
		m_uDefScrHz = uDefScrHz;
		m_bDefWin = bWin;
	}

	virtual HRESULT Create ( HWND hWndApp, HWND hWndFocus, HINSTANCE hInstance )
	{
		m_hWndApp = hWndApp;
		m_hWndFocus = hWndFocus;
		return Create ( hInstance );
	}

public:
	// Functions to create, run, pause, and clean up the application
	virtual HRESULT Create( HINSTANCE hInstance );
	virtual INT     Run();
	virtual LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual VOID    Pause( BOOL bPause );

	// Internal constructor
	CD3DApplication();

protected:
	bool DXUTIsWindowed();
	HRESULT DXUTSetDeviceCursor( IDirect3DDevice9* pd3dDevice, HCURSOR hCursor, bool bAddWatermark );
	void DXUTSetupCursor();	// 커서가 이상할 경우.
};

#endif