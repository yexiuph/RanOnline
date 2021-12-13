#pragma	once

#include "UIOuterWindow.h"
#include "s_NetGlobal.h"

class	CBasicTextBox;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CSelectServerChannel;

class CSelectServerPage : public CUIOuterWindow
{
private:
static	const	float		fLIST_PART_INTERVAL;
static	const	D3DCOLOR	dwCLICKCOLOR;
static	const	int			nOUTOFRANGE;
static	const	float		fNORMAL_PERCENT;
static	const	float		fMAX_PERCENT;

private:
	enum
	{
		SELECT_SERVER_CONNECT = ET_CONTROL_NEXT,
		SELECT_SERVER_QUIT,
		SELECT_SERVER_LIST,
		SERVER_SCROLLBAR,
		SELECT_SERVER_CHANNEL,
		SELECT_CHINA_AREA_BACK
	};

	enum
	{
		SERVER_SMOOTH = 0,	// 서버 원활
		SERVER_NORMAL,		// 서버 보통.
		SERVER_CONGEST,		// 서버 혼잡.
		SERVER_NOVACANCY	// 서버 풀, 접속 불가
	};

public:
	CSelectServerPage ();
	virtual	~CSelectServerPage ();

public:
	void	ResetAll ();

public:
	void	CreateSubControl ();
	void	CreateSubControlTW ();

public:
	BOOL	InitServerPage ();
	BOOL	InitServerPageChannel ();

private:
	BOOL	ConnectServer ();
	BOOL	ConnectServerChannel ();

private:
	void	LoadChannel ();

public:
	void	Login ();
	void	SetChannelFull ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	void TranslateUIMessageChannel ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextBoxEx*		m_pServerList;
	CBasicTextBox*			m_pServerState;
	CBasicScrollBarEx*		m_pScrollBar;
	CSelectServerChannel*	m_pSelectServerChannel;

private:
	int		m_nIndex;
	int		m_nServerState[MAX_SERVER_NUMBER];

private:
	LPG_SERVER_CUR_INFO_LOGIN	m_pServerInfo;
};