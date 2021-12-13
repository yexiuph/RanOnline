// DxInputDevice.h: interface for the DxInputDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXINPUTDEVICE_H__E63F7026_E592_455D_B845_BD74A224DE2E__INCLUDED_)
#define AFX_DXINPUTDEVICE_H__E63F7026_E592_455D_B845_BD74A224DE2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//-----------------------------------------------------------------------------------------
//
//	"dinput.h" 에 스켄코드가 디파인되어 있음.

#include <set>

#include "dinput.h"

//	Note : 마우스와 키보드의 키 상태를 알아보기 위해서 사용되는 플래그들이다.
//		마우스의경우 DXKEY_PRESSED, DXKEY_DRAG 모두 마우스 버튼이 눌러져 있는 상태를
//		나타내며 하나는 단순 누르고 있는 상태, 또하나는 드래그상태를 각각 나타낸다.
//		이들 두 플래그는 동시에 나타나지 않으며 만약 누른 상태인지 확인할려면
//		DXKEY_DOWNED 로 확인하는 것이 좋다. ex) ( dwMLeft&DXKEY_DOWNED )
//
//
enum EMDXKEYFLAGS
{
	DXKEY_IDLE		= (0x0001),
	DXKEY_DOWN		= (0x0002),
	DXKEY_PRESSED	= (0x0004),
	DXKEY_DRAG		= (0x0008),
	DXKEY_UP		= (0x0010),
	DXKEY_DUP		= (0x0020),

	DXKEY_DOWNED	= (DXKEY_DOWN|DXKEY_PRESSED|DXKEY_DRAG),
};

enum DX_MOUSEKEYS
{
	DXMOUSE_LEFT	= 0,
	DXMOUSE_RIGHT	= 1,
	DXMOUSE_MIDDLE	= 2,
	DXMOUSE_3		= 3,
	DXMOUSE_4		= 4,
	DXMOUSE_5		= 5,
	DXMOUSE_6		= 6,
	DXMOUSE_7		= 7,
};

//enum DX_JOYKEYS
//{
//	DXJOY_LEFT	= 128,
//	DXJOY_RIGHT	= 129,
//	DXJOY_UP	= 130,
//	DXJOY_DOWN	= 131,
//
//	DXJOY_SIZE	= 132,
//};
//
//class DxJoystickDevice
//{
//private:
//	BYTE	m_JoyState[DXJOY_SIZE];
//
//public:
//	//	Note : 키보드 키 상태.
//	//
//	DWORD GetKeyState( BYTE ScanCode )
//	{		
//		return m_JoyState[ScanCode];
//	}
//
//	void SetKeyState( BYTE ScanCode, EMDXKEYFLAGS dwFlag );
//
//public:
//	HRESULT OnCreateDevice( HWND hDlg );
//	VOID OnDestroyDevice();
//
//	HRESULT OnFrameMove( HWND hDlg );
//
//public:
//	~DxJoystickDevice();
//	DxJoystickDevice();
//
//	//	Note : 싱글톤 클래스 제어.
//	//
//public:
//	static DxJoystickDevice& GetInstance();
//};

class DxInputDevice
{
public:
	enum
	{
		KEY_BUFFER_SIZE = 64,
		MOUSE_BUFFER_SIZE = 128
	};

	typedef std::set<BYTE>			PUTSET;
	typedef PUTSET::iterator		PUTSET_ITER;

protected:
	BOOL					m_bActive;
	LPDIRECTINPUT8			m_pDInput;
	LPDIRECTINPUTDEVICE8	m_pDInputMouseDev;
	LPDIRECTINPUTDEVICE8	m_pDInputKeyboardDev;
	BOOL					m_bDEVICEMOUSE;
	BOOL					m_bCAPTUREMOUSE;

	HWND					m_hWnd;
	DWORD					m_dwMouseCoopFlags;
	DWORD					m_dwKeyboardCoopFlags;

	POINT					m_OldCursorPos;
	POINT					m_CursorPos;
	POINT					m_ptWindowSize;
	
	int						m_OldMouseLocateX, m_OldMouseLocateY, m_OldMouseLocateZ;
	int						m_MouseLocateX, m_MouseLocateY, m_MouseLocateZ;
	int						m_MouseMoveDX, m_MouseMoveDY, m_MouseMoveDZ;

	DWORD					m_dwAppWidth;
	DWORD					m_dwAppHeight;

private:
	DWORD					m_dwKeyElements;
	DIDEVICEOBJECTDATA		m_KeyDidodata[KEY_BUFFER_SIZE];

	DWORD					m_dwMouseElements;
	DIDEVICEOBJECTDATA		m_MouseDidodata[MOUSE_BUFFER_SIZE];

	//	키보드 상태
	BYTE	m_KeyState[256];	
	BYTE	m_KeyStateLast[256];	//	지난 번 프레임의 마지막 상태(UP/DOWN)
	PUTSET	m_mapKeyDown;

	//	마우스 상태
	BYTE	m_MouseState[8];
	BYTE	m_MouseStateBack[8];	//	DRAG이면 저장
	BYTE	m_MouseStateLast[8];	//	지난 번 프레임의 마지막 상태(UP/DOWN)
	PUTSET	m_mapMouseKeyDown;
	DWORD	m_dwLastTime[8];
	BOOL	m_bMouseMove;			//	마우스가 이동했나?

	BOOL	m_bUpdateInputState;

	BOOL	m_bUpdateKeyState;
	BOOL	m_bUpdateMouseState;

public:
	void	InitKeyState ();
	BOOL	IsUpdatInputState() { return m_bUpdateInputState; }
	BOOL	IsUpdatKeyState()   { return m_bUpdateKeyState; }
	BOOL	IsUpdatMouseState() { return m_bUpdateMouseState; }

private:
	BOOL	ProcessKeyState();
	void	UpdateKeyState();
	void	UpdateMouseState( BOOL bDefWin, BOOL bGame );

public:
	HRESULT OneTimeSceneInit(HWND hWnd);
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bDEVICEMOUSE=FALSE, BOOL bCAPTUREMOUSE=FALSE );

public:
	HRESULT DeleteDeviceObjects();

public:
	void HoldCursor ();

public:
	HRESULT OnActivate ( BOOL bActive );
	HRESULT FrameMove( float fTime, float fElapsedTime, BOOL bDefWin, BOOL bGame );

protected:
	DxInputDevice();

public:
	virtual ~DxInputDevice();

public:
	//	Note : 키보드 키 상태.
	//
	DWORD GetKeyState ( BYTE ScanCode )
	{		
		return m_KeyState[ScanCode];
	}

	BOOL GetKeyBufStateUp ( BYTE ScanCode );
	BOOL GetKeyBufStateDown ( BYTE ScanCode );

	void SetKeyState( BYTE ScanCode, EMDXKEYFLAGS dwFlag )	{ m_KeyState[ScanCode] |= dwFlag; }

	//	Note : 마우스의 단위 이동 거리.
	//
	void GetMouseMove ( int &_ndx, int &_ndy, int &_ndz )
	{
		_ndx = m_MouseMoveDX;
		_ndy = m_MouseMoveDY;
		_ndz = m_MouseMoveDZ;
	}

	int GetMouseMoveX () { return m_MouseMoveDX; }
	int GetMouseMoveY () { return m_MouseMoveDY; }
	int GetMouseMoveZ () { return m_MouseMoveDZ; }

	//	Note : 마우스의 현제 위치.
	//
	void GetMouseLocate ( int &_nx, int &_ny, int &_nz )
	{
		_nx = m_MouseLocateX;
		_ny = m_MouseLocateY;
		_nz = m_MouseLocateZ;
	}

	int GetMouseLocateX () { return m_MouseLocateX; }
	int GetMouseLocateY () { return m_MouseLocateY; }
	int GetMouseLocateZ () { return m_MouseLocateZ; }

	void ScreenOverArray( BOOL bDefWin );	// 가장자리에 닿으면 반대편으로 커서를 옮긴다.

	//	Note : 마우스 키 상태.
	//
	DWORD GetMouseState ( DX_MOUSEKEYS MKey )
	{
		if ( MKey > DXMOUSE_7 )	return DXKEY_IDLE;
		return m_MouseState[MKey];

		//return m_MouseKeyState[MKey];
	}

	BOOL IsMouseInGameScreen( BOOL bDefWin );

	//	Note : 싱글톤 클래스 제어.
	//
public:
	static DxInputDevice& GetInstance();
};

inline BOOL DxInputDevice::GetKeyBufStateUp ( BYTE ScanCode )
{
	return GetKeyState ( ScanCode ) & DXKEY_UP;
}

inline BOOL DxInputDevice::GetKeyBufStateDown ( BYTE ScanCode )
{
	return GetKeyState ( ScanCode ) & DXKEY_DOWN;
}


#endif // !defined(AFX_DXINPUTDEVICE_H__E63F7026_E592_455D_B845_BD74A224DE2E__INCLUDED_)
