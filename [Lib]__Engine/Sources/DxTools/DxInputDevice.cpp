// DxInputDevice.cpp: implementation of the DxInputDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "DxInputDevice.h"

#include "ScanCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////--------------------------------------------------------------------------------------------------------------------
////							D	x		J	o	y	s	t	i	c	k		D	e	v	i	c	e
////--------------------------------------------------------------------------------------------------------------------
//#include "DebugSet.h"
//
//LPDIRECTINPUT8       g_pDI = NULL;
//LPDIRECTINPUTDEVICE8 g_pJoystick = NULL;
//BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
//BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
//
////	Note : 싱글톤 클래스 제어.
//DxJoystickDevice& DxJoystickDevice::GetInstance()
//{
//	static DxJoystickDevice Instance;
//	return Instance;
//}
//
//DxJoystickDevice::DxJoystickDevice()
//{
//	memset ( m_JoyState, DXKEY_IDLE, sizeof( BYTE ) * DXJOY_SIZE );
//}
//
//DxJoystickDevice::~DxJoystickDevice()
//{
//}
////-----------------------------------------------------------------------------
//// Name: InitDirectInput()
//// Desc: Initialize the DirectInput variables.
////-----------------------------------------------------------------------------
//HRESULT DxJoystickDevice::OnCreateDevice( HWND hDlg )
//{
//    HRESULT hr;
//
//    // Register with the DirectInput subsystem and get a pointer
//    // to a IDirectInput interface we can use.
//    // Create a DInput object
//    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
//                                         IID_IDirectInput8, (VOID**)&g_pDI, NULL ) ) )
//		return hr;
//
//    // Look for a simple Joystick we can use for this sample program.
//    if( FAILED( hr = g_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL, 
//                                         EnumJoysticksCallback,
//                                         NULL, DIEDFL_ATTACHEDONLY ) ) )
//        return hr;
//
//    // Make sure we got a Joystick
//    if( NULL == g_pJoystick )
//    {
//        return S_OK;
//    }
//
//    // Set the data format to "simple Joystick" - a predefined data format 
//    //
//    // A data format specifies which controls on a device we are interested in,
//    // and how they should be reported. This tells DInput that we will be
//    // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
//    if( FAILED( hr = g_pJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
//        return hr;
//
//    // Set the cooperative level to let DInput know how this device should
//    // interact with the system and with other DInput applications.
//    if( FAILED( hr = g_pJoystick->SetCooperativeLevel( hDlg, DISCL_EXCLUSIVE | 
//                                                             DISCL_FOREGROUND ) ) )
//        return hr;
//
//    // Enumerate the Joystick objects. The callback function enabled user
//    // interface elements for objects that are found, and sets the min/max
//    // values property for discovered axes.
//    if( FAILED( hr = g_pJoystick->EnumObjects( EnumObjectsCallback, 
//                                                (VOID*)hDlg, DIDFT_ALL ) ) )
//        return hr;
//
//    return S_OK;
//}
//
////-----------------------------------------------------------------------------
//// Name: EnumJoysticksCallback()
//// Desc: Called once for each enumerated Joystick. If we find one, create a
////       device interface on it so we can play with it.
////-----------------------------------------------------------------------------
//BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
//                                     VOID* pContext )
//{
//    HRESULT hr;
//
//    // Obtain an interface to the enumerated Joystick.
//    hr = g_pDI->CreateDevice( pdidInstance->guidInstance, &g_pJoystick, NULL );
//
//    // If it failed, then we can't use this Joystick. (Maybe the user unplugged
//    // it while we were in the middle of enumerating it.)
//    if( FAILED(hr) ) 
//        return DIENUM_CONTINUE;
//
//    // Stop enumeration. Note: we're just taking the first Joystick we get. You
//    // could store all the enumerated Joysticks and let the user pick.
//    return DIENUM_STOP;
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// Name: EnumObjectsCallback()
//// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
////       Joystick. This function enables user interface elements for objects
////       that are found to exist, and scales axes min/max values.
////-----------------------------------------------------------------------------
//BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
//                                   VOID* pContext )
//{
//    HWND hDlg = (HWND)pContext;
//
//    static int nSliderCount = 0;  // Number of returned slider controls
//    static int nPOVCount = 0;     // Number of returned POV controls
//
//    // For axes that are returned, set the DIPROP_RANGE property for the
//    // enumerated axis in order to scale min/max values.
//    if( pdidoi->dwType & DIDFT_AXIS )
//    {
//        DIPROPRANGE diprg; 
//        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
//        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
//        diprg.diph.dwHow        = DIPH_BYID; 
//        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
//        diprg.lMin              = -1000; 
//        diprg.lMax              = +1000; 
//    
//        // Set the range for the axis
//        if( FAILED( g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
//            return DIENUM_STOP;
//    }
//
//
//    //// Set the UI to reflect what objects the Joystick supports
//    //if (pdidoi->guidType == GUID_XAxis)
//    //{
//    //    EnableWindow( GetDlgItem( hDlg, IDC_X_AXIS ), TRUE );
//    //    EnableWindow( GetDlgItem( hDlg, IDC_X_AXIS_TEXT ), TRUE );
//    //}
//    //if (pdidoi->guidType == GUID_YAxis)
//    //{
//    //    EnableWindow( GetDlgItem( hDlg, IDC_Y_AXIS ), TRUE );
//    //    EnableWindow( GetDlgItem( hDlg, IDC_Y_AXIS_TEXT ), TRUE );
//    //}
//    //if (pdidoi->guidType == GUID_ZAxis)
//    //{
//    //    EnableWindow( GetDlgItem( hDlg, IDC_Z_AXIS ), TRUE );
//    //    EnableWindow( GetDlgItem( hDlg, IDC_Z_AXIS_TEXT ), TRUE );
//    //}
//    //if (pdidoi->guidType == GUID_RxAxis)
//    //{
//    //    EnableWindow( GetDlgItem( hDlg, IDC_X_ROT ), TRUE );
//    //    EnableWindow( GetDlgItem( hDlg, IDC_X_ROT_TEXT ), TRUE );
//    //}
//    //if (pdidoi->guidType == GUID_RyAxis)
//    //{
//    //    EnableWindow( GetDlgItem( hDlg, IDC_Y_ROT ), TRUE );
//    //    EnableWindow( GetDlgItem( hDlg, IDC_Y_ROT_TEXT ), TRUE );
//    //}
//    //if (pdidoi->guidType == GUID_RzAxis)
//    //{
//    //    EnableWindow( GetDlgItem( hDlg, IDC_Z_ROT ), TRUE );
//    //    EnableWindow( GetDlgItem( hDlg, IDC_Z_ROT_TEXT ), TRUE );
//    //}
//    //if (pdidoi->guidType == GUID_Slider)
//    //{
//    //    switch( nSliderCount++ )
//    //    {
//    //        case 0 :
//    //            EnableWindow( GetDlgItem( hDlg, IDC_SLIDER0 ), TRUE );
//    //            EnableWindow( GetDlgItem( hDlg, IDC_SLIDER0_TEXT ), TRUE );
//    //            break;
//
//    //        case 1 :
//    //            EnableWindow( GetDlgItem( hDlg, IDC_SLIDER1 ), TRUE );
//    //            EnableWindow( GetDlgItem( hDlg, IDC_SLIDER1_TEXT ), TRUE );
//    //            break;
//    //    }
//    //}
//    //if (pdidoi->guidType == GUID_POV)
//    //{
//    //    switch( nPOVCount++ )
//    //    {
//    //        case 0 :
//    //            EnableWindow( GetDlgItem( hDlg, IDC_POV0 ), TRUE );
//    //            EnableWindow( GetDlgItem( hDlg, IDC_POV0_TEXT ), TRUE );
//    //            break;
//
//    //        case 1 :
//    //            EnableWindow( GetDlgItem( hDlg, IDC_POV1 ), TRUE );
//    //            EnableWindow( GetDlgItem( hDlg, IDC_POV1_TEXT ), TRUE );
//    //            break;
//
//    //        case 2 :
//    //            EnableWindow( GetDlgItem( hDlg, IDC_POV2 ), TRUE );
//    //            EnableWindow( GetDlgItem( hDlg, IDC_POV2_TEXT ), TRUE );
//    //            break;
//
//    //        case 3 :
//    //            EnableWindow( GetDlgItem( hDlg, IDC_POV3 ), TRUE );
//    //            EnableWindow( GetDlgItem( hDlg, IDC_POV3_TEXT ), TRUE );
//    //            break;
//    //    }
//    //}
//
//    return DIENUM_CONTINUE;
//}
//
////-----------------------------------------------------------------------------
//// Name: FreeDirectInput()
//// Desc: Initialize the DirectInput variables.
////-----------------------------------------------------------------------------
//VOID DxJoystickDevice::OnDestroyDevice()
//{
//    // Unacquire the device one last time just in case 
//    // the app tried to exit while the device is still acquired.
//    if( g_pJoystick ) 
//        g_pJoystick->Unacquire();
//    
//    // Release any DirectInput objects.
//    SAFE_RELEASE( g_pJoystick );
//    SAFE_RELEASE( g_pDI );
//}
//
////-----------------------------------------------------------------------------
//// Name: UpdateInputState()
//// Desc: Get the input device's state and display it.
////-----------------------------------------------------------------------------
//HRESULT DxJoystickDevice::OnFrameMove( HWND hDlg )
//{
//    HRESULT     hr;
//    TCHAR       strText[512] = {0}; // Device state text
//    DIJOYSTATE2 js;           // DInput Joystick state 
//
//    if( NULL == g_pJoystick ) 
//        return S_OK;
//
//    // Poll the device to read the current state
//    hr = g_pJoystick->Poll(); 
//    if( FAILED(hr) )  
//    {
//        // DInput is telling us that the input stream has been
//        // interrupted. We aren't tracking any state between polls, so
//        // we don't have any special reset that needs to be done. We
//        // just re-acquire and try again.
//        hr = g_pJoystick->Acquire();
//        while( hr == DIERR_INPUTLOST ) 
//            hr = g_pJoystick->Acquire();
//
//        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
//        // may occur when the app is minimized or in the process of 
//        // switching, so just try again later 
//        return S_OK; 
//    }
//
//    // Get the input's device state
//    if( FAILED( hr = g_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &js ) ) )
//        return hr; // The device should have been acquired during the Poll()
//
//    // Display Joystick state to dialog
//
////	m_JoyStateLast = m_JoyState;	// Back Up
//
//	switch( m_JoyState[DXJOY_LEFT] )
//	{
//	case DXKEY_IDLE:
//		if( js.lX == 0 )		m_JoyState[DXJOY_LEFT] = DXKEY_DOWN;
//		break;
//
//	case DXKEY_DOWN:
//	case DXKEY_PRESSED:
//		if( js.lX == 0 )		m_JoyState[DXJOY_LEFT] = DXKEY_PRESSED;
//		else					m_JoyState[DXJOY_LEFT] = DXKEY_UP;
//		break;
//
//	case DXKEY_UP:
//		if( js.lX == 0 )		m_JoyState[DXJOY_LEFT] = DXKEY_DOWN;
//		else					m_JoyState[DXJOY_LEFT] = DXKEY_IDLE;
//		break;
//	}
//
//	switch( m_JoyState[DXJOY_RIGHT] )
//	{
//	case DXKEY_IDLE:
//		if( js.lX == 65535 )	m_JoyState[DXJOY_RIGHT] = DXKEY_DOWN;
//		break;
//
//	case DXKEY_DOWN:
//	case DXKEY_PRESSED:
//		if( js.lX == 65535 )	m_JoyState[DXJOY_RIGHT] = DXKEY_PRESSED;
//		else					m_JoyState[DXJOY_RIGHT] = DXKEY_UP;
//		break;
//
//	case DXKEY_UP:
//		if( js.lX == 65535 )	m_JoyState[DXJOY_RIGHT] = DXKEY_DOWN;
//		else					m_JoyState[DXJOY_RIGHT] = DXKEY_IDLE;
//		break;
//	}
//
//	switch( m_JoyState[DXJOY_DOWN] )
//	{
//	case DXKEY_IDLE:
//		if( js.lY == 0 )		m_JoyState[DXJOY_DOWN] = DXKEY_DOWN;
//		break;
//
//	case DXKEY_DOWN:
//	case DXKEY_PRESSED:
//		if( js.lY == 0 )		m_JoyState[DXJOY_DOWN] = DXKEY_PRESSED;
//		else					m_JoyState[DXJOY_DOWN] = DXKEY_UP;
//		break;
//
//	case DXKEY_UP:
//		if( js.lY == 0 )		m_JoyState[DXJOY_DOWN] = DXKEY_DOWN;
//		else					m_JoyState[DXJOY_DOWN] = DXKEY_IDLE;
//		break;
//	}
//
//	switch( m_JoyState[DXJOY_UP] )
//	{
//	case DXKEY_IDLE:
//		if( js.lY == 65535 )	m_JoyState[DXJOY_UP] = DXKEY_DOWN;
//		break;
//
//	case DXKEY_DOWN:
//	case DXKEY_PRESSED:
//		if( js.lY == 65535 )	m_JoyState[DXJOY_UP] = DXKEY_PRESSED;
//		else					m_JoyState[DXJOY_UP] = DXKEY_UP;
//		break;
//
//	case DXKEY_UP:
//		if( js.lY == 65535 )	m_JoyState[DXJOY_UP] = DXKEY_DOWN;
//		else					m_JoyState[DXJOY_UP] = DXKEY_IDLE;
//		break;
//	}
//
//	// Fill up text with which buttons are pressed
//    for( int i = 0; i < 128; i++ )
//    {
//		switch( m_JoyState[i] )
//		{
//		case DXKEY_IDLE:
//			if( js.rgbButtons[i] & 0x80 )	m_JoyState[i] = DXKEY_DOWN;
//			break;
//
//		case DXKEY_DOWN:
//		case DXKEY_PRESSED:
//			if( js.rgbButtons[i] & 0x80 )	m_JoyState[i] = DXKEY_PRESSED;
//			else							m_JoyState[i] = DXKEY_UP;
//			break;
//
//		case DXKEY_UP:
//			if( js.rgbButtons[i] & 0x80 )	m_JoyState[i] = DXKEY_DOWN;
//			else							m_JoyState[i] = DXKEY_IDLE;
//			break;
//		}
//    }
//
//	// Test
//	//for( DWORD i=0; i<8 ; ++i )
//	//{
//	//	CDebugSet::ToView( i, "%d", m_JoyState[i] );
//	//}
//
//	//CDebugSet::ToView( 9, "%d", m_JoyState[DXJOY_LEFT] );
//	//CDebugSet::ToView( 10, "%d", m_JoyState[DXJOY_RIGHT] );
//	//CDebugSet::ToView( 11, "%d", m_JoyState[DXJOY_DOWN] );
//	//CDebugSet::ToView( 12, "%d", m_JoyState[DXJOY_UP] );
//	CDebugSet::ToView( 12, "%d", js.lX );
//	CDebugSet::ToView( 13, "%d", js.lY );
//
//    //// Axes
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.lX ); 
//    //SetWindowText( GetDlgItem( hDlg, IDC_X_AXIS ), strText );
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.lY ); 
//    //SetWindowText( GetDlgItem( hDlg, IDC_Y_AXIS ), strText );
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.lZ ); 
//    //SetWindowText( GetDlgItem( hDlg, IDC_Z_AXIS ), strText );
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.lRx ); 
//    //SetWindowText( GetDlgItem( hDlg, IDC_X_ROT ), strText );
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.lRy ); 
//    //SetWindowText( GetDlgItem( hDlg, IDC_Y_ROT ), strText );
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.lRz ); 
//    //SetWindowText( GetDlgItem( hDlg, IDC_Z_ROT ), strText );
//
//    //// Slider controls
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.rglSlider[0] ); 
//    //SetWindowText( GetDlgItem( hDlg, IDC_SLIDER0 ), strText );
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.rglSlider[1] ); 
//    //SetWindowText( GetDlgItem( hDlg, IDC_SLIDER1 ), strText );
//
//    //// Points of view
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.rgdwPOV[0] );
//    //SetWindowText( GetDlgItem( hDlg, IDC_POV0 ), strText );
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.rgdwPOV[1] );
//    //SetWindowText( GetDlgItem( hDlg, IDC_POV1 ), strText );
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.rgdwPOV[2] );
//    //SetWindowText( GetDlgItem( hDlg, IDC_POV2 ), strText );
//    //StringCchPrintf( strText, 512, TEXT("%ld"), js.rgdwPOV[3] );
//    //SetWindowText( GetDlgItem( hDlg, IDC_POV3 ), strText );
// 
//   
//    //// Fill up text with which buttons are pressed
//    //for( int i = 0; i < 128; i++ )
//    //{
//    //    if ( js.rgbButtons[i] & 0x80 )
//    //    {
//			
//    //    }
//    //}
//
//    return S_OK;
//}
//
//void DxJoystickDevice::SetKeyState( BYTE ScanCode, EMDXKEYFLAGS dwFlag )
//{
//	if( NULL == g_pJoystick )	return;
//	m_JoyState[ScanCode] |= dwFlag;
//}

//--------------------------------------------------------------------------------------------------------------------
//								D	x		I	n	p	u	t		D	e	v	i	c	e
//--------------------------------------------------------------------------------------------------------------------
//	Note : 싱글톤 클래스 제어.
//
DxInputDevice& DxInputDevice::GetInstance()
{
	static DxInputDevice Instance;
	return Instance;
}

DxInputDevice::DxInputDevice() :
	m_pDInput(NULL),
	m_pDInputMouseDev(NULL),
	m_pDInputKeyboardDev(NULL),
	m_bDEVICEMOUSE(FALSE),
	m_bCAPTUREMOUSE(FALSE),
	m_bActive(FALSE),

	m_hWnd(NULL),
	m_dwMouseCoopFlags(DISCL_NONEXCLUSIVE|DISCL_FOREGROUND ),
	m_dwKeyboardCoopFlags(DISCL_NONEXCLUSIVE|DISCL_FOREGROUND|DISCL_NOWINKEY ),

	m_MouseMoveDX(0),
	m_MouseMoveDY(0),
	m_MouseMoveDZ(0),

	m_OldMouseLocateX(0),
	m_OldMouseLocateY(0),
	m_OldMouseLocateZ(0),

	m_MouseLocateX(0),
	m_MouseLocateY(0),
	m_MouseLocateZ(0),

	m_dwAppWidth(0L),
	m_dwAppHeight(0L),

	m_bUpdateInputState(FALSE),
	m_bUpdateKeyState(FALSE),
	m_bUpdateMouseState(FALSE)
{
	m_OldCursorPos.x = 0;
	m_OldCursorPos.y = 0;
	m_CursorPos.x = 0;
	m_CursorPos.y = 0;

	m_ptWindowSize.x = 0;
	m_ptWindowSize.y = 0;
}

DxInputDevice::~DxInputDevice()
{

}

void DxInputDevice::InitKeyState ()
{
	m_dwKeyElements = 0;
	memset ( m_KeyDidodata, 0, sizeof(DIDEVICEOBJECTDATA)*KEY_BUFFER_SIZE );

	m_dwMouseElements = 0;
	memset ( m_MouseDidodata, 0, sizeof(DIDEVICEOBJECTDATA)*MOUSE_BUFFER_SIZE );

	memset ( m_KeyState, DXKEY_IDLE, sizeof ( BYTE ) * 256 );
	memset ( m_KeyStateLast, DXKEY_IDLE, sizeof ( BYTE ) * 256 );

	memset ( m_MouseState, DXKEY_IDLE, sizeof ( BYTE ) * 8 );
	memset ( m_MouseStateBack, DXKEY_IDLE, sizeof ( BYTE ) * 8 );
	memset ( m_MouseStateLast, DXKEY_IDLE, sizeof ( BYTE ) * 8 );
	memset ( m_dwLastTime, 0, sizeof ( DWORD ) * 8 );

	m_mapKeyDown.clear();
	m_mapMouseKeyDown.clear ();
}

HRESULT DxInputDevice::OneTimeSceneInit ( HWND hWnd )
{
	m_hWnd = hWnd;

	InitKeyState();

	return S_OK;
}

HRESULT DxInputDevice::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bDEVICEMOUSE, BOOL bCAPTUREMOUSE )
{
	HRESULT hr;

	D3DDISPLAYMODE sMode;
	pd3dDevice->GetDisplayMode( 0, &sMode );
	m_dwAppWidth = sMode.Width;
	m_dwAppHeight = sMode.Height;

	if ( pd3dDevice )
	{
		D3DSURFACE_DESC d3dsdBackBuffer;

		LPDIRECT3DSURFACEQ pBackBuffer=NULL;
		pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

		if ( pBackBuffer )
		{
			pBackBuffer->GetDesc( &d3dsdBackBuffer );
			pBackBuffer->Release();

			m_ptWindowSize.x = static_cast<LONG> ( d3dsdBackBuffer.Width );
			m_ptWindowSize.y = static_cast<LONG> ( d3dsdBackBuffer.Height );
		}
	}

	m_bCAPTUREMOUSE = bCAPTUREMOUSE;
	m_bDEVICEMOUSE = bDEVICEMOUSE;

	//	Note : DirectInput  객체 인터페이스 생성 
	//
	hr = DirectInput8Create ( GetModuleHandle(NULL),
		DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&m_pDInput, NULL );
	if( FAILED( hr ) )
	{
		DeleteDeviceObjects ();
		return hr;
	}
	    
	//	Note : KeyBoard Device 생성 
	//
	hr = m_pDInput->CreateDevice ( GUID_SysKeyboard, &m_pDInputKeyboardDev, NULL );
	if( FAILED(hr) )
	{
		DeleteDeviceObjects();
		return hr;
	}

	//	Note : 키보드 포맷 방식 셋팅 
	//
	hr = m_pDInputKeyboardDev->SetDataFormat ( &c_dfDIKeyboard );
	if( FAILED(hr) )
	{
		DeleteDeviceObjects();
		return hr;
	}

	//	Note : 윈도우와 Device가 연결되어 상호작용을 어떻게 할것인지를 셋업.
	//
	m_dwKeyboardCoopFlags = (DISCL_NONEXCLUSIVE|DISCL_FOREGROUND|DISCL_NOWINKEY);
	hr = m_pDInputKeyboardDev->SetCooperativeLevel ( m_hWnd, m_dwKeyboardCoopFlags );
	if( hr == DIERR_UNSUPPORTED )
	{
		CDebugSet::ToLogFile ( "keyboard device, SetCooperativeLevel(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND|DISCL_NOWINKEY" );

		m_dwKeyboardCoopFlags = (DISCL_BACKGROUND|DISCL_NONEXCLUSIVE);
		hr = m_pDInputKeyboardDev->SetCooperativeLevel ( m_hWnd, m_dwKeyboardCoopFlags );
		if ( hr == DIERR_UNSUPPORTED )
		{
			CDebugSet::ToLogFile ( "keyboard device, SetCooperativeLevel(), DISCL_BACKGROUND|DISCL_NONEXCLUSIVE" );

			m_dwKeyboardCoopFlags = (DISCL_FOREGROUND|DISCL_EXCLUSIVE);
			hr = m_pDInputKeyboardDev->SetCooperativeLevel ( m_hWnd, m_dwKeyboardCoopFlags );
			if ( hr == DIERR_UNSUPPORTED )
			{
				CDebugSet::ToLogFile ( "keyboard device, SetCooperativeLevel(), DISCL_FOREGROUND|DISCL_EXCLUSIVE" );
				
				DeleteDeviceObjects();
				return hr;
			}
		}
	}

	//	키보드 버퍼 초기화
    {	
        DIPROPDWORD dipdw;

        dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj        = 0;
        dipdw.diph.dwHow        = DIPH_DEVICE;
        dipdw.dwData            = KEY_BUFFER_SIZE; // Arbitary buffer size

        if( FAILED( hr = m_pDInputKeyboardDev->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
		{
			CDebugSet::ToLogFile ( "keyboard device, SetProperty(), hr=%d", hr );
            return hr;
		}
    }

	//	Note : Input device를 엑세스 하여 활성화 시킨다 
	//
	hr =  m_pDInputKeyboardDev->Acquire();


	//	Note : 마우스 디바이스 생성 
	//
	hr = m_pDInput->CreateDevice( GUID_SysMouse, &m_pDInputMouseDev, NULL );
	if( FAILED( hr ) )
	{
		DeleteDeviceObjects();
		return hr;
	}

	// 마우스 데이타 포맷 셋팅 
	hr = m_pDInputMouseDev->SetDataFormat( &c_dfDIMouse2 );
	if( FAILED(hr) )
	{
		DeleteDeviceObjects();
		return hr;
	}

	//마우스 레벨 셋
	hr = m_pDInputMouseDev->SetCooperativeLevel( m_hWnd, m_dwMouseCoopFlags );
	if( hr == DIERR_UNSUPPORTED  && ( m_dwMouseCoopFlags == (DISCL_BACKGROUND|DISCL_EXCLUSIVE)) )
	{
		DeleteDeviceObjects();
		return hr;
	}
    
	//	마우스 버퍼 초기화
    {
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj        = 0;
        dipdw.diph.dwHow        = DIPH_DEVICE;
        dipdw.dwData            = MOUSE_BUFFER_SIZE; // Arbitary buffer size

        if( FAILED( hr = m_pDInputMouseDev->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
            return hr;
	}

	//	Note : Input device를 엑세스 하여 활성화 시킨다 
	//
	m_pDInputMouseDev->Acquire();

	OnActivate ( TRUE );

//	DxJoystickDevice::GetInstance().OnCreateDevice( m_hWnd );

	return S_OK;
}

HRESULT DxInputDevice::DeleteDeviceObjects()
{
	if ( m_pDInput )
	{
		if ( m_pDInputKeyboardDev )
		{
			m_pDInputKeyboardDev->Unacquire();
			m_pDInputKeyboardDev->Release();
			m_pDInputKeyboardDev = NULL;
		}

		if ( m_pDInputMouseDev )
		{
			m_pDInputMouseDev->Unacquire();
			m_pDInputMouseDev->Release();
			m_pDInputMouseDev = NULL;
		}

		m_pDInput->Release();
		m_pDInput = NULL;
	}

//	DxJoystickDevice::GetInstance().OnDestroyDevice();

	return S_OK;
}

HRESULT DxInputDevice::OnActivate ( BOOL bActive )
{
	HRESULT hr;
	if ( !m_pDInputKeyboardDev )	return S_FALSE;

	m_bActive = bActive;

	if ( bActive )
	{
		//if ( m_bCAPTUREMOUSE )	SetCapture ( m_hWnd );

		hr =  m_pDInputKeyboardDev->Acquire();
		while ( hr == DIERR_INPUTLOST )		hr = m_pDInputKeyboardDev->Acquire();

		hr =  m_pDInputMouseDev->Acquire();
		while ( hr == DIERR_INPUTLOST )		hr = m_pDInputMouseDev->Acquire();

		ProcessKeyState ();
		InitKeyState ();
	}
	else
	{
		//if ( m_bCAPTUREMOUSE )	ReleaseCapture ();

		hr =  m_pDInputKeyboardDev->Unacquire();
		if( FAILED(hr) )	return FALSE;

		hr =  m_pDInputMouseDev->Unacquire();
		if( FAILED(hr) )	return FALSE;
	}

	return S_OK;
}

BOOL DxInputDevice::ProcessKeyState()
{
	HRESULT hr = S_OK;
	if ( !m_bActive )	return FALSE;

	hr =  m_pDInputKeyboardDev->Acquire();
	if ( SUCCEEDED(hr) )
	{
		m_dwKeyElements = KEY_BUFFER_SIZE;
		hr = m_pDInputKeyboardDev->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), m_KeyDidodata, &m_dwKeyElements, 0 );
		if( hr != DI_OK )
		{
			memset ( m_KeyDidodata, 0, sizeof(DIDEVICEOBJECTDATA)*KEY_BUFFER_SIZE );

			hr = m_pDInputKeyboardDev->Acquire();
			while ( hr == DIERR_INPUTLOST )		hr = m_pDInputKeyboardDev->Acquire();
		}
	}

	hr =  m_pDInputMouseDev->Acquire();
	if ( SUCCEEDED(hr) )
	{
		m_dwMouseElements = MOUSE_BUFFER_SIZE;
		hr = m_pDInputMouseDev->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), m_MouseDidodata, &m_dwMouseElements, 0 );
		if( hr != DI_OK )
		{
			memset ( m_MouseDidodata, 0, sizeof(DIDEVICEOBJECTDATA)*MOUSE_BUFFER_SIZE );

			hr = m_pDInputMouseDev->Acquire();
			while ( hr == DIERR_INPUTLOST )		hr = m_pDInputMouseDev->Acquire();
		}
	}

	return TRUE;
}

HRESULT DxInputDevice::FrameMove ( float fTime, float fElapsedTime, BOOL bDefWin, BOOL bGame )
{	
	if ( m_pDInputKeyboardDev == NULL )		return E_FAIL;

	m_OldCursorPos = m_CursorPos;
	m_OldMouseLocateX = m_MouseLocateX;
	m_OldMouseLocateY = m_MouseLocateY;
	m_OldMouseLocateZ = m_MouseLocateZ;

	m_bUpdateInputState = FALSE;
	m_bUpdateKeyState   = FALSE;
	m_bUpdateMouseState = FALSE;

	BOOL bOk = ProcessKeyState();
	if ( bOk )
	{
		UpdateKeyState ();
		UpdateMouseState( bDefWin, bGame );
	}else{
		m_bUpdateInputState = FALSE;
		m_bUpdateKeyState   = FALSE;
		m_bUpdateMouseState = FALSE;
	}

//	DxJoystickDevice::GetInstance().OnFrameMove( m_hWnd );

	return S_OK;
}

void DxInputDevice::HoldCursor ()
{
	m_CursorPos=m_OldCursorPos;
	SetCursorPos ( m_OldCursorPos.x, m_OldCursorPos.y );

	m_MouseLocateX = m_OldMouseLocateX;
	m_MouseLocateY = m_OldMouseLocateY;
	m_MouseLocateZ = m_OldMouseLocateZ;
}

void DxInputDevice::UpdateKeyState  ()
{
	memset ( m_KeyState, DXKEY_IDLE, sizeof ( BYTE ) * 256 );
	for ( DWORD i = 0; i < m_dwKeyElements; i++ )
	{
		BYTE	nScanCode = (BYTE)m_KeyDidodata[i].dwOfs;

		if ( m_KeyDidodata[i].dwData&0x80 )
		{
			m_KeyState[nScanCode] &= ~DXKEY_IDLE;
			m_KeyState[nScanCode] |= DXKEY_DOWN;
			m_KeyStateLast[nScanCode] = DXKEY_DOWN;
			m_mapKeyDown.insert ( nScanCode );
			//CDebugSet::ToListView ( "DOWN" );

			m_bUpdateInputState = TRUE;
			m_bUpdateKeyState   = TRUE;

		}

		if ( !(m_KeyDidodata[i].dwData&0x80) )
		{
			m_KeyState[nScanCode] &= ~DXKEY_IDLE;
			m_KeyState[nScanCode] |= DXKEY_UP;
			m_KeyStateLast[nScanCode] = DXKEY_UP;

			PUTSET_ITER iter = m_mapKeyDown.find ( nScanCode );
			if ( iter!=m_mapKeyDown.end() )
			{
				m_mapKeyDown.erase(iter);
			}
		
			//CDebugSet::ToListView ( "UP" );
		}
	}

	if ( m_mapKeyDown.empty() ) return;
	
	PUTSET_ITER iter = m_mapKeyDown.begin();
	PUTSET_ITER iter_end = m_mapKeyDown.end();
	for ( ; iter!=iter_end; ++iter )
	{
		BYTE nScanCode = (*iter);
	
		//	눌러진 상태
		if ( (m_KeyState[nScanCode] & DXKEY_IDLE) && (m_KeyStateLast[nScanCode] & DXKEY_DOWN) )
		{
			m_KeyState[nScanCode] = DXKEY_PRESSED;
			//CDebugSet::ToListView ( "PRESSED" );
		}
		else if ( (m_KeyState[nScanCode] & DXKEY_IDLE) && (m_KeyStateLast[nScanCode] & ~DXKEY_DOWN) )
		{
			GASSERT ( 0 );
		}

		if ( (m_KeyState[nScanCode] & ~DXKEY_IDLE) )
		{
			//CDebugSet::ToListView ("-. %d - %d", nScanCode, m_KeyState[nScanCode]);
		}
	}
}

BOOL DxInputDevice::IsMouseInGameScreen( BOOL bDefWin )
{
	RECT rtWindow;
	::GetClientRect( m_hWnd, &rtWindow );

	POINT ptCursorPos;
	::GetCursorPos ( &ptCursorPos );

	POINT ptTOP;
	ptTOP.x = 0;
	ptTOP.y = 0;
	::ClientToScreen( m_hWnd, &ptTOP );

	bool bMouseIn = ptTOP.x<ptCursorPos.x && ptCursorPos.x<(ptTOP.x+rtWindow.right) &&
					ptTOP.y<ptCursorPos.y && ptCursorPos.y<(ptTOP.y+rtWindow.bottom);

	if( bMouseIn )		return TRUE;
	else				return FALSE;
}

void DxInputDevice::UpdateMouseState( BOOL bDefWin, BOOL bGame )
{
	RECT rtWindow;
	GetClientRect ( m_hWnd, &rtWindow );

	POINT ptTOP = { 0, 0 };

	if( bDefWin )	ClientToScreen ( m_hWnd, &ptTOP );

	rtWindow.left += ptTOP.x;
	rtWindow.right += ptTOP.x;
	rtWindow.top += ptTOP.y;
	rtWindow.bottom += ptTOP.y;

	if( bGame )
	{
		if( !IsMouseInGameScreen( bDefWin ) )	return;
	}

	//// 마우스의 포인터가 게임화면안에 있을때만 마우스 상태 업데이트
	//POINT ptMouse = { 0, 0 };
	//GetCursorPos( &ptMouse );
	//if ( ptMouse.x < rtWindow.left || ptMouse.x > rtWindow.right || ptMouse.y < rtWindow.top || ptMouse.y > rtWindow.bottom )
	//	return;

	m_bMouseMove = FALSE;
	memset ( m_MouseState, DXKEY_IDLE, sizeof ( BYTE ) * 8 );

	for ( DWORD i = 0; i < m_dwMouseElements; i++ )
	{
		const int NOTEXIST = -1;
		int	nButton = NOTEXIST;
		switch ( m_MouseDidodata[i].dwOfs )
		{
			case DIMOFS_BUTTON0: nButton = DXMOUSE_LEFT;	break;
			case DIMOFS_BUTTON1: nButton = DXMOUSE_RIGHT;	break;
			case DIMOFS_BUTTON2: nButton = DXMOUSE_MIDDLE;	break;
			case DIMOFS_BUTTON3:
			case DIMOFS_BUTTON4:
			case DIMOFS_BUTTON5:
			case DIMOFS_BUTTON6:
			case DIMOFS_BUTTON7: break;

			case DIMOFS_X:
				if ( m_bDEVICEMOUSE )
				{
					m_MouseMoveDX = m_MouseDidodata[i].dwData;
					m_MouseLocateX += m_MouseMoveDX;
				}

				m_bMouseMove = TRUE;
				break;

			case DIMOFS_Y:
				if ( m_bDEVICEMOUSE )
				{
					m_MouseMoveDY = m_MouseDidodata[i].dwData;
					m_MouseLocateY += m_MouseMoveDY;
				}

				m_bMouseMove = TRUE;
				break;

			case DIMOFS_Z:
				m_MouseLocateZ += m_MouseDidodata[i].dwData;
				m_bMouseMove = TRUE;
				break;
		}

		if( nButton != NOTEXIST || m_bMouseMove == TRUE )
		{
			m_bUpdateInputState = TRUE;
			m_bUpdateMouseState = TRUE;
		}


		if ( nButton == NOTEXIST ) continue;
        
		//	DOWN
		if ( m_MouseDidodata[i].dwData & 0x80 )
		{
			m_MouseState[nButton] &= ~DXKEY_IDLE;
			m_MouseState[nButton] |= DXKEY_DOWN;
			m_MouseStateLast[nButton] = DXKEY_DOWN;
			m_mapMouseKeyDown.insert ( nButton );
			//CDebugSet::ToListView ( "DOWN" );
		}

		//	UP
		if ( !(m_MouseDidodata[i].dwData & 0x80) || (m_MouseStateLast[nButton] & DXKEY_UP) )
		{
			BOOL	bUp = FALSE;
			if ( m_MouseState[nButton] & DXKEY_UP )
			{
				m_MouseState[nButton] = DXKEY_DUP;
				bUp = TRUE;
				//CDebugSet::ToListView ( "DUP" );
				//CDebugSet::ToView ( "더블 클릭1" );
			}
			else if ( m_MouseDidodata[i].dwTimeStamp - m_dwLastTime[nButton] < 330 )
			{
				m_MouseState[nButton] = DXKEY_DUP;
				bUp = TRUE;
				//CDebugSet::ToListView ( "DUP" );
				//CDebugSet::ToView ( "더블 클릭2" );
			}
			else if ( !(m_MouseStateLast[nButton] & DXKEY_UP) )
			{
				m_MouseState[nButton] &= ~DXKEY_IDLE;
				m_MouseState[nButton] |= DXKEY_UP;
				bUp = TRUE;
				m_dwLastTime[nButton] = m_MouseDidodata[i].dwTimeStamp;
				//CDebugSet::ToListView ( "UP" );
			}

			//	버튼이 Up되었을 경우, 그 메시지는 처리가 완료된것으로 확인한다.
			if ( bUp )
			{
				m_MouseStateLast[nButton] = DXKEY_UP;
				PUTSET_ITER iter = m_mapMouseKeyDown.find ( nButton );
				if ( iter!=m_mapMouseKeyDown.end() )
				{
					m_mapMouseKeyDown.erase(iter);
				}
			}
		}
	}

	PUTSET_ITER iter = m_mapMouseKeyDown.begin();
	PUTSET_ITER iter_end = m_mapMouseKeyDown.end();
	for ( ; iter!=iter_end; ++iter )
	{
		BYTE nButton = (*iter);
	
		//	눌러진 상태
		if ( (m_MouseState[nButton] & DXKEY_IDLE) && (m_MouseStateLast[nButton] & DXKEY_DOWN) )
		{
			if ( m_bMouseMove || (m_MouseStateBack[nButton] & DXKEY_DRAG) )
			{
				m_MouseState[nButton] = DXKEY_DRAG;			
				//CDebugSet::ToListView ( "DRAG" );
			}
			else if ( m_MouseStateBack[nButton] & ~DXKEY_DRAG )
			{
				m_MouseState[nButton] = DXKEY_PRESSED;
				//CDebugSet::ToListView ( "PRESSED" );
			}

			m_MouseStateBack[nButton] = m_MouseState[nButton];
		}	
		else
		{
			m_MouseStateBack[nButton] = DXKEY_IDLE;
		}

		if ( (m_MouseState[nButton] & ~DXKEY_IDLE) )
		{
			//CDebugSet::ToListView ("-. 버튼 : %d, 상태 %d", nButton, m_MouseState[nButton]);
		}
	}

	//	Note : 윈도우 마우스에서는 다음과 같이 위치를 변경.
	//
	if ( !m_bDEVICEMOUSE )
	{
		GetCursorPos ( &m_CursorPos );
		m_MouseLocateX = m_CursorPos.x - rtWindow.left;
		m_MouseLocateY = m_CursorPos.y - rtWindow.top;

		m_MouseMoveDX = m_MouseLocateX - m_OldMouseLocateX;
		m_MouseMoveDY = m_MouseLocateY - m_OldMouseLocateY;
	}
	else
	{
		//	Note : 마우스 위치 윈도우 영역안으로 제한.
		//
		if ( m_MouseLocateX < 0 )					m_MouseLocateX = 0;
		if ( m_MouseLocateY < 0 )					m_MouseLocateY = 0;
		if ( m_MouseLocateX > m_ptWindowSize.x )	m_MouseLocateX = m_ptWindowSize.x;
		if ( m_MouseLocateY > m_ptWindowSize.y )	m_MouseLocateY = m_ptWindowSize.y;
	}

	m_MouseMoveDZ = m_MouseLocateZ - m_OldMouseLocateZ;
}

void DxInputDevice::ScreenOverArray( BOOL bDefWin )
{
	POINT	sCursorPos;
	GetCursorPos( &sCursorPos );

	if( (DWORD)sCursorPos.x >= m_dwAppWidth-1 )
	{
		sCursorPos.x = 1;
		SetCursorPos( sCursorPos.x, sCursorPos.y );
		goto _RETURN;
	}
	if( sCursorPos.x <= 0 )	
	{
		sCursorPos.x = m_dwAppWidth-2;
		SetCursorPos( sCursorPos.x, sCursorPos.y );
		goto _RETURN;
	}
	
	if( (DWORD)sCursorPos.y >= m_dwAppHeight-1 )	
	{
		sCursorPos.y = 1;
		SetCursorPos( sCursorPos.x, sCursorPos.y );
		goto _RETURN;
	}
	if( sCursorPos.y <= 0 )
	{
		sCursorPos.y = m_dwAppHeight-2;
		SetCursorPos( sCursorPos.x, sCursorPos.y );
		goto _RETURN;
	}

	return;

_RETURN:
	RECT rtWindow;
	GetClientRect ( m_hWnd, &rtWindow );

	POINT ptTOP = { 0, 0 };

	if( bDefWin )	ClientToScreen ( m_hWnd, &ptTOP );

	rtWindow.left += ptTOP.x;
	rtWindow.right += ptTOP.x;
	rtWindow.top += ptTOP.y;
	rtWindow.bottom += ptTOP.y;

	//	Note : 윈도우 마우스에서는 다음과 같이 위치를 변경.
	if ( !m_bDEVICEMOUSE )
	{
		GetCursorPos ( &m_CursorPos );
		m_MouseLocateX = m_CursorPos.x - rtWindow.left;
		m_MouseLocateY = m_CursorPos.y - rtWindow.top;

		m_OldMouseLocateX = m_MouseLocateX;
		m_OldMouseLocateY = m_MouseLocateY;
	}
	else
	{
		//	Note : 마우스 위치 윈도우 영역안으로 제한.
		if ( m_MouseLocateX < 0 )					m_MouseLocateX = 0;
		if ( m_MouseLocateY < 0 )					m_MouseLocateY = 0;
		if ( m_MouseLocateX > m_ptWindowSize.x )	m_MouseLocateX = m_ptWindowSize.x;
		if ( m_MouseLocateY > m_ptWindowSize.y )	m_MouseLocateY = m_ptWindowSize.y;

		m_OldMouseLocateX = m_MouseLocateX;
		m_OldMouseLocateY = m_MouseLocateY;
	}
}



