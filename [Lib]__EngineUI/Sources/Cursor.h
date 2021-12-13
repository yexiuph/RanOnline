#pragma once

#include <string>
#include <vector>


enum EMCURSOR
{
	CURSOR_NORMAL	= 0,
	CURSOR_ATTACK	= 1,
	CURSOR_HAND		= 2,
	CURSOR_SCROLL	= 3,
	CURSOR_TALK		= 4,
};

class CCursor
{
public:
	typedef std::vector<HCURSOR>	VECCURSOR; // MEMO

protected:
	BOOL				m_bGameCursor;
	EMCURSOR			m_emType;

	std::string			m_strCurNormal;
	std::string			m_strCurAttack;
	std::string			m_strCurSelect;
	std::string			m_strCurScroll;
	std::string			m_strCurTalk;

	std::string			m_strPath;
	HWND				m_hWnd;

	HCURSOR				m_hCurDefault;
	HCURSOR				m_hCurNormal;
	VECCURSOR			m_vecCurAttack;

	HCURSOR				m_hCurSelect;
	HCURSOR				m_hCurScroll;
	HCURSOR				m_hCurTalk;

public:
	void SetGameCursor ( BOOL _bCursor );
	void SetCursorType ( EMCURSOR emType );
	HCURSOR GetCurCursor ();
	void SetShowCursor ( BOOL bShow );

	void SetCursorNow ();

public:
	HRESULT OneTimeSceneInit ( char *szPath, HWND hWnd );
	HRESULT InitDeviceObjects();
	HRESULT DeleteDeviceObjects();

	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pD3DDevice, int x, int y );

protected:
	CCursor(void);

public:
	~CCursor(void);

public:
	static CCursor& GetInstance();
};
