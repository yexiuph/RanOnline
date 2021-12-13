#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"

const	int	CHAT_NO			= 0x0000;	//	메시지 없음
const	int	CHAT_NORMAL		= 0x0001;	//	일반
const	int	CHAT_PRIVATE	= 0x0002;	//	귓속말
const	int	CHAT_PARTY		= 0x0004;	//	파티
const	int	CHAT_GUILD		= 0x0008;	//	클럽
const	int	CHAT_TOALL		= 0x0010;	//	확성기
const	int CHAT_ALLIANCE	= 0x0020;	//	동맹
const	int	CHAT_SYSTEM		= 0x0100;	//	시스템
const	int	CHAT_ALL		= CHAT_TOALL|CHAT_SYSTEM|CHAT_NORMAL|CHAT_PRIVATE|CHAT_PARTY|CHAT_GUILD|CHAT_ALLIANCE;

struct SRecordChatMsg
{
	CTime   recordTime;
	CString strChatMsg;
};

class CBasicTextBoxEx;
class CBasicScrollBarEx;
class CUIEditBox;
class CBasicButton;
class CSystemMessageDisplay;
class CBasicTextButton;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_RIGHTBODY = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CBasicChatRightBody : public CUIGroup
{
	static	const int nLIMITCHAR;
	static	const int nSTARTLINE;
	static	const int nLIMITLINE;
	static	const D3DCOLOR dwCLICKCOLOR;

	static	const CString BLANK_SYMBOL;
	static	const CString PRIVATE_SYMBOL;
	static	const CString PARTY_SYMBOL;
	static	const CString TOALL_SYMBOL;
	static	const CString GUILD_SYMBOL;
	static	const CString ALLIANCE_SYMBOL;
	static	const CString SYSTEM_SYMBOL;

public:
	enum
	{
		CHAT_EDITBOX = NO_ID + 1,
		CHAT_NATIVE_BUTTON,
		CHAT_ENGLISH_BUTTON,

		CHAT_CHANNEL_TOTAL,
		CHAT_CHANNEL_PRIVATE,
		CHAT_CHANNEL_PARTY,
		CHAT_CHANNEL_CLUB,
		CHAT_CHANNEL_ALLIANCE,
		CHAT_CHANNEL_SYSTEM,

		CHAT_TEXTBOX_ALL,
		CHAT_TEXTBOX_PARTY,
		CHAT_TEXTBOX_CLUB,
		CHAT_TEXTBOX_ALLIANCE,
		CHAT_TEXTBOX_PRIVATE,
		CHAT_TEXTBOX_SYSTEM
	};

	//	간단한 상태 정의
	enum
	{
		OUT_OF_RANGE = -1,	//	채팅 로그 범위밖(사용하지 않고 있음)
	};

private:
	CUIControl*			m_pEditBoxBack;
	CUIEditBox*			m_pEditBox;
	//	CSystemMessageDisplay* m_pSystemMessageDisplay;

private:	//	전체
	CBasicTextBoxEx*	m_pTextBox_ALL;
	CBasicScrollBarEx*	m_pScrollBar_ALL;

	CBasicTextBoxEx*	m_pTextBox_PARTY;
	CBasicScrollBarEx*	m_pScrollBar_PARTY;

	CBasicTextBoxEx*	m_pTextBox_CLUB;
	CBasicScrollBarEx*	m_pScrollBar_CLUB;

	CBasicTextBoxEx*	m_pTextBox_ALLIANCE;
	CBasicScrollBarEx*	m_pScrollBar_ALLIANCE;

	CBasicTextBoxEx*	m_pTextBox_PRIVATE;
	CBasicScrollBarEx*	m_pScrollBar_PRIVATE;

	CBasicTextBoxEx*	m_pTextBox_SYSTEM;
	CBasicScrollBarEx*	m_pScrollBar_SYSTEM;

	CUIControl*			m_pScrollBarDummy;

	CBasicButton*		m_pNativeButton;
	CBasicButton*		m_pEnglishButton;

	CBasicTextButton*	m_pChatTotalButton;
	CBasicTextButton*	m_pChatPrivateButton;
	CBasicTextButton*	m_pChatPartyButton;
	CBasicTextButton*	m_pChatClubButton;
	CBasicTextButton*	m_pChatAllianceButton;
	CBasicTextButton*	m_pChatSystemButton;

public:
	CBasicChatRightBody ();
	virtual	~CBasicChatRightBody ();

public:
	void	CreateSubControl ( CD3DFontPar*	pFont );

private:
	void	AddStringToNORMAL ( CString strTemp, D3DCOLOR dwColor );
	void	SAVE_CHATLOG( CString strTemp, WORD wType );
	void	RECORD_CHAT( const CString strChatMsg, WORD wType );

public:
	void	AddChat ( const CString& strName, const CString& strMsg, const WORD& wType );
	void	AddStringToChatEx ( CString strTemp, WORD wType );
	void	AddStringToSystemMessage ( CString strTemp, D3DCOLOR dwColor );
	void	AddChatMacro(CString strMsg);
	void	ChatLog( bool bChatLog, int nChatLogType );
	CString GET_RECORD_CHAT();

private:
	CBasicTextBoxEx*	CreateTextBoxEx ( CD3DFontPar* pFont, UIGUID uID );
	CBasicScrollBarEx*	CreateScrollBarEx ( const int& nTotalLine, const int& nVisibleLine );
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );

public:
	void	ADD_FRIEND ( const CString& strName ); // 친구이름을 리스트에 추가
	void	DEL_FRIEND ( const CString& strName ); // 친구이름을 리스트에서 삭제
	void	ADD_FRIEND_LIST ();

public:
	void	ADD_CHATLOG ( const CString& strChat ); // 채팅메세지를 리스트에 추가

public:
	void	BEGIN_NORMAL_CHAT ( const CString& strMessage = "" ); 
	void	BEGIN_PRIVATE_CHAT ( const CString& strName = "", const CString strMessage = "" );
	void	BEGIN_PARTY_CHAT ( const CString& strMessage = "" );	
	void	BEGIN_TOALL_CHAT ( const CString& strMessage = "" );
	void	BEGIN_GUILD_CHAT ( const CString& strMessage = "" );
	void	BEGIN_ALLIANCE_CHAT ( const CString& strMessage = "" );

public:
	bool	IsEDIT_BEGIN ()									{ return m_pEditBox->IsBegin(); }

private:
	void	EDIT_BEGIN ( const CString& strMessage = "" );		
	void	EDIT_END ();
	void	EDIT_CLEAR()									{ m_pEditBox->ClearEdit (); }

	void	KEEP_LAST_STATE ();

private:
	void	SELECT_CHAT_PAGE ();
	void	UPDATE_CHAT_PAGE ( const BOOL& bFirstControl );
	void	UPDATE_CHAT_PAGE_TEXTBOX ( CBasicTextBoxEx* pTextBox, CBasicScrollBarEx* pScrollBar );
	void	ADD_CHAT_PAGE_TEXTBOX ( const CString& strTemp, const D3DCOLOR& dwColor, CBasicTextBoxEx* pTextBox, CBasicScrollBarEx* pScrollBar );

private:
	bool	SEND_CHAT_MESSAGE ();
	bool	SEND_CHAT_MESSAGE ( const CString& strOrigin );
	bool	SEND_COMMON_MESSAGE ( const CString& strOrigin );
	bool	SEND_PRIVATE_MESSAGE( const CString& strOrigin );
	bool	SEND_PARTY_MESSAGE ( const CString& strOrigin );
	bool	SEND_TOALL_MESSAGE ( const CString& strOrigin );
	bool	SEND_GUILD_MESSAGE ( const CString& strOrigin );
	bool	SEND_ALLIANCE_MESSAGE ( const CString& strOrigin );

private:
	void	ChangePrivateName ();
	void	ChangeLastChat ();
	void	ChangeChatType ();

	void	UPDATE_SHORTKEY ();

	void	UPDATE_CHATTYPE_STATE ( const int nCHATTYPE_BACK );

	bool	IS_FRIEND ( CString strName );
    CString	GET_ONLINE_FRIEND ( CString strName );

private:
	void	ShowSelfMessageOnHead ( const CString& strOrigin );

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	HRESULT	RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void	SetChatFlagState( WORD wChatFlag )		{ m_wDisplayState = wChatFlag; }
	WORD	GetChatFlagState()						{ return m_wDisplayState; }
	void	AddChatFlagState( WORD wChatFlag )		{ m_wDisplayState |= wChatFlag; }

	void	SetIgnoreState ( bool bIgnoreState )	{ m_bIgnoreState = bIgnoreState; }

	void	CLASSIFY_CHATTYPE ();
	int		GetCHATTYPE()							{ return m_nCHATTYPE; }
	int		GetDISPLAYTYPE()						{ return m_wDISPLAYTYPE; }

	void	SetFreezeControl( bool bFreeze )		{ m_bFREEZE = bFreeze; }

	void	SetVisibleChatButton( BOOL bVisible );

private:
	typedef std::list<CString>	NAMELIST;
	typedef NAMELIST::iterator	NAMELIST_ITER;	

	typedef	std::list<CString>	NORMAL_CHAT_LOG;
	typedef	NORMAL_CHAT_LOG::iterator	NORMAL_CHAT_LOG_ITER;

	typedef std::vector<SRecordChatMsg> RECORD_CHAT_LOG;
	typedef RECORD_CHAT_LOG::iterator	RECORD_CHAT_LOG_ITER;

protected: // 메세지 중복 입력(도배) 처리
	static const FLOAT	fLIMIT_TIME;
	static const FLOAT	fINTERVAL_INPUT;
	static const INT	nCOUNT_INPUT;
	FLOAT	m_fLifeTime;
	INT		m_nInputCount;
	INT		m_nTimeCount;
	CString m_strLastMsg;
	BOOL	m_bPapering;

	RECORD_CHAT_LOG m_vecRecordChatMsg;

	VOID SetLifeTime( float fTime )					{ m_fLifeTime = fTime; }
	FLOAT GetLifeTime()								{ return m_fLifeTime; }
	VOID DecreaseTime( float fElapsedTime );			
	BOOL IsPapering( const CString& strOrigin );
	BOOL IsThaiCheck( const CString& strOrigin );
	BOOL IsVnCheck( const CString& strOrigin );


private:
	NORMAL_CHAT_LOG		m_ChatLog;
	int					m_ChatPos;
	int					m_ChatCountBack;

	NAMELIST	m_NameList;
	int			m_NamePos;
	int			m_NameCountBack;

	CString		m_strChatBack;

	int			m_nFRIENDSIZE_BACK;

	bool	m_bFREEZE;
	bool	m_bMINIMUM_SIZE_TEXT;
	bool	m_bMINIMUM_SIZE_SCROLL;

	int		m_nCHATTYPE;

	WORD	m_wDisplayState;
	WORD	m_wDISPLAYTYPE;

	bool	m_bChatLog;
	int		m_nChatLogType;

private:
	bool	m_bIgnoreState;
	HMODULE m_hCheckStrDLL;
	BOOL (_stdcall *m_pCheckString)(CString);
};