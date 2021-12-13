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
//	"dinput.h" �� �����ڵ尡 �����εǾ� ����.

#include <set>

#include "dinput.h"

//	Note : ���콺�� Ű������ Ű ���¸� �˾ƺ��� ���ؼ� ���Ǵ� �÷��׵��̴�.
//		���콺�ǰ�� DXKEY_PRESSED, DXKEY_DRAG ��� ���콺 ��ư�� ������ �ִ� ���¸�
//		��Ÿ���� �ϳ��� �ܼ� ������ �ִ� ����, ���ϳ��� �巡�׻��¸� ���� ��Ÿ����.
//		�̵� �� �÷��״� ���ÿ� ��Ÿ���� ������ ���� ���� �������� Ȯ���ҷ���
//		DXKEY_DOWNED �� Ȯ���ϴ� ���� ����. ex) ( dwMLeft&DXKEY_DOWNED )
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
//	//	Note : Ű���� Ű ����.
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
//	//	Note : �̱��� Ŭ���� ����.
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

	//	Ű���� ����
	BYTE	m_KeyState[256];	
	BYTE	m_KeyStateLast[256];	//	���� �� �������� ������ ����(UP/DOWN)
	PUTSET	m_mapKeyDown;

	//	���콺 ����
	BYTE	m_MouseState[8];
	BYTE	m_MouseStateBack[8];	//	DRAG�̸� ����
	BYTE	m_MouseStateLast[8];	//	���� �� �������� ������ ����(UP/DOWN)
	PUTSET	m_mapMouseKeyDown;
	DWORD	m_dwLastTime[8];
	BOOL	m_bMouseMove;			//	���콺�� �̵��߳�?

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
	//	Note : Ű���� Ű ����.
	//
	DWORD GetKeyState ( BYTE ScanCode )
	{		
		return m_KeyState[ScanCode];
	}

	BOOL GetKeyBufStateUp ( BYTE ScanCode );
	BOOL GetKeyBufStateDown ( BYTE ScanCode );

	void SetKeyState( BYTE ScanCode, EMDXKEYFLAGS dwFlag )	{ m_KeyState[ScanCode] |= dwFlag; }

	//	Note : ���콺�� ���� �̵� �Ÿ�.
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

	//	Note : ���콺�� ���� ��ġ.
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

	void ScreenOverArray( BOOL bDefWin );	// �����ڸ��� ������ �ݴ������� Ŀ���� �ű��.

	//	Note : ���콺 Ű ����.
	//
	DWORD GetMouseState ( DX_MOUSEKEYS MKey )
	{
		if ( MKey > DXMOUSE_7 )	return DXKEY_IDLE;
		return m_MouseState[MKey];

		//return m_MouseKeyState[MKey];
	}

	BOOL IsMouseInGameScreen( BOOL bDefWin );

	//	Note : �̱��� Ŭ���� ����.
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
