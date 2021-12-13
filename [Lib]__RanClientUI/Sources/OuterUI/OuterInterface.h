#pragma	once

#include "../[Lib]__EngineUI/Sources/UIMan.h"

//	PAGE ID
enum ET_UIGUID_OUTER
{
	SELECT_SERVER_PAGE = NO_ID + 1,
	SELECT_CHINA_AREA_PAGE,
	COPYRIGHT,
	CHINA_TITLE,
	LOGIN_PAGE,
	PASS_KEYBOARD,
	SELECT_CHARACTER_PAGE,
	SELECT_CHARACTER_LEFTPAGE,
	CREATE_CHARACTER_PAGE,
	CREATE_CHARACTER_WEB,
	MODAL_WINDOW_OUTER,
	OUTER_UP_IMAGE,
	OUTER_DN_IMAGE,
	FULLSCREEN_OUTER,
	FULLSCREEN_OUTER_DUMMY,
	WAIT_DISPLAY,
	SECPASS_SETPAGE,
	SECPASS_CHECKPAGE,
	MESSAGE_WINDOW,
	MESSAGE_WINDOW_DUMMY,
};

//	MODAL CALLER
enum
{
	OUTER_MODAL_CLOSEGAME = NO_ID + 1,
	OUTER_MODAL_WAITSERVERINFO,
	OUTER_MODAL_RECONNECT,
	OUTER_MODAL_WAITCONFIRM,
	OUTER_MODAL_CREATEFAILED,
	OUTER_MODAL_ALREADYCONNECT,
	OUTER_MODAL_NON_PK_CHANNEL,
	OUTER_MODAL_CONNECTCLOSED,
	OUTER_MODAL_LOGOUT,
	OUTER_MODAL_SECONDPASSWORD,
	OUTER_MODAL_CREATESUCCEED,
	OUTER_MODAL_GENERAL_LOGINFAIL,
	OUTER_MODAL_NAME_ERROR,
	OUTER_MODAL_LOGIN_WAIT_DAUM,
	OUTER_MODAL_LOGIN_WAIT_DAUM_AFTER_KEYRECEIVED,
	OUTER_MODAL_LOGIN_WAIT_DAUM_SERVERFULL,
	OUTER_MODAL_CHARACTERSTAGE_GAME_JOIN_WAIT,
	OUTER_MODAL_CHARACTERSTAGE_GAME_JOIN_FAIL,
	OUTER_MODAL_THAI_CHECK_STRING_ERROR
};

class CModalWindow;
class CSelectServerPage;
class CLoginPage;
class CPassKeyboard;
class CSelectCharacterPage;
class CCreateCharacterPage;
class CSelectCharacterLeftPage;
class CWaitDialogue;
class DxLobyStage;
class CCreateCharacterWeb;
class CSecPassSetPage;
class CSecPassCheckPage;
class CMessageWindow;
class CNewCreateCharPage;

class COuterInterface : public CUIMan
{
	static	const	float	fLIST_LINE_INTERVAL;

private:
	BOOL	m_bOnTheInterface;
	bool	m_bBlockProgramFound;
	float	m_fWaitTime;
	BOOL	m_bUseWaitTime;
	USHORT	m_uChaRemain;			// 생성 가능한 캐릭터 수
	INT		m_nChaRemainExtremeM;	// 생성 가능한 극강부 남자 캐릭터 수
	INT		m_nChaRemainExtremeW;	// 생성 가능한 극강부 여자 캐릭터 수
	int		m_nServerGroup;
	int		m_nServerChannel;
	bool	m_bCHANNEL;
	BOOL	m_bLoginCancel;

	LONG	m_lResolutionBack;

	UIGUID	m_uidMoalCallWindow;
	CString	m_strDaumGID;

private:
	CModalWindow*				m_pModalWindow;
	CSelectServerPage*			m_pSelectServerPage;
	CLoginPage*					m_pLoginPage;
	CSecPassSetPage*			m_pSecPassSetPage;
	CSecPassCheckPage*			m_pSecPassCheckPage;
	CMessageWindow*				m_pMessageWindow;
	CPassKeyboard*				m_pPassKeyboard;
	CCreateCharacterWeb*		m_pCreateCharacterWeb;
	CSelectCharacterPage*		m_pSelectCharacterPage;
	CSelectCharacterLeftPage*	m_pSelectCharacterLeftPage;
	CCreateCharacterPage*		m_pCreateCharacterPage;
	CUIControl*					m_pFullScreenButton;
	CUIControl*					m_pFullScreenButtonDummy;
	CUIControl*					m_pUpImage;
	CUIControl*					m_pDownImage;
	CUIControl*					m_pMessageWindowDummy;
	CNewCreateCharPage*			m_pNewCreateCharPage;

	CWaitDialogue*				m_pWaitDisplay;
	DxLobyStage*				m_pDxLobyStage;

	void ResetControl();

public:
	COuterInterface ();
	virtual	~COuterInterface ();

public:
	UIGUID	GetRenderStage ();

public:
	virtual HRESULT OneTimeSceneInit();
	virtual	HRESULT FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual HRESULT FinalCleanup();
	virtual HRESULT DeleteDeviceObjects();

public:
	void SetOnTheInterface ()							{ m_bOnTheInterface = TRUE; }
	BOOL IsOnTheInterface ()							{ return m_bOnTheInterface; }
	void ResetOnTheInterface ()							{ m_bOnTheInterface = FALSE; }

	void	START_WAIT_TIME ( float fTime );
	float	GET_WAIT_TIME()								{ return m_fWaitTime; }

	void SetCancelToLogin()								{ m_bLoginCancel = TRUE; }
	void ResetCancelToLogin()							{ m_bLoginCancel = FALSE; }
	BOOL IsLoginCancel()								{ return m_bLoginCancel; }

	void SetModalCallWindowID( UIGUID uID )				{ m_uidMoalCallWindow = uID; }
	UIGUID GetModalCallWindowID()						{ return m_uidMoalCallWindow; }

private:
	void	UPDATE_WAIT_TIME ( float fElapsedTime );
	void	ModalMsgProcess ( UIGUID nCallerID, DWORD dwMsg );
	void	CloseAllWindow ();

public:
	void MsgProcess ( LPVOID pBuffer );

	void SetDaumGID( const CString & strDaumGID )	{ m_strDaumGID = strDaumGID; }
	const TCHAR * GetDaumGID() const				{ return m_strDaumGID.GetString(); }

	void SetCharRemain( const USHORT uChaRemain )		{ m_uChaRemain = uChaRemain; }
	const USHORT GetCharRemain() const					{ return m_uChaRemain; }

	void SetCharRemainExM( const INT nChaRemain )		{ m_nChaRemainExtremeM = nChaRemain; }
	const INT GetCharRemainExM() const					{ return m_nChaRemainExtremeM; }

	void SetCharRemainExW( const INT nChaRemain )		{ m_nChaRemainExtremeW = nChaRemain; }
	const INT GetCharRemainExW() const					{ return m_nChaRemainExtremeW; }

	BOOL IsCreateExtream()								{ return (m_nChaRemainExtremeM>0 || m_nChaRemainExtremeW>0); }

	const UIRECT	GetBackImageRect();

	// 중국 MMOSPEED
	void RunMMOSpeed();

	const char* MakeString ( const char* szFormat, ... );

	void SetBlockProgramFound ( bool bFOUND )			{ m_bBlockProgramFound = bFOUND; }
	bool IsBlockProgramFound ()							{ return m_bBlockProgramFound; }

	void WAITSERVER_DIALOGUE_OPEN ( const CString& strMessage, const int nAction, const float fTimer );
	void WAITSERVER_DIALOGUE_CLOSE();

	void ToSelectServerPage ( UIGUID cID );
	void ToLoginPage ( UIGUID cID );
	void ToSecPassPage ( UIGUID cID, int nCheckFlag );
	void ToChinaAreaPage ( UIGUID cID );
	void ToSelectCharacterPage ( UIGUID cID );
	void ToCreateCharacterPage ( UIGUID cID );

	void OpenMessageWindow ( UIGUID cID, BOOL bOpen );

	void DeleteCharacter ();
	
	bool IsCHANNEL()									{ return m_bCHANNEL; }

	CModalWindow*			GetModalWindow ()			{ return m_pModalWindow; }
	CSelectServerPage*		GetSelectServerPage ()		{ return m_pSelectServerPage; }
	CLoginPage*				GetLoginPage ()				{ return m_pLoginPage; }
	CSecPassSetPage*		GetSecPassSetPage ()		{ return m_pSecPassSetPage;	}
	CSecPassCheckPage*		GetSecPassCheckPage ()		{ return m_pSecPassCheckPage;	}
	CPassKeyboard*			GetPassKeyboard()			{ return m_pPassKeyboard; }
	CSelectCharacterPage*	GetSelectCharacterPage ()	{ return m_pSelectCharacterPage; }
	CCreateCharacterPage*	GetCreateCharacterPage ()	{ return m_pCreateCharacterPage; }
	CNewCreateCharPage*		GetNewCreateCharacterPage (){ return m_pNewCreateCharPage; }
	CCreateCharacterWeb*	GetCreateCharacterWeb()		{ return m_pCreateCharacterWeb; }
	CUIControl*				GetUpImage()				{ return m_pUpImage; }
	CUIControl*				GetDownImage()				{ return m_pDownImage; }

	void SetConnectServerInfo ( const int nServerGroup, const int nServerChannel );
	void GetConnectServerInfo ( int& nServerGroup, int& nServerChannel );

	void SetLobyStage( DxLobyStage * pLobyStage )	{ GASSERT( pLobyStage ); m_pDxLobyStage = pLobyStage; }

	void SetResolutionBack( long lResolution )	{ m_lResolutionBack = lResolution; }
	long GetResolutionBack()						{ return m_lResolutionBack; }

private:
	BOOL LoadClientVersion ( int& nPatchVer, int& nGameVer );

public:
	static COuterInterface& GetInstance();
};