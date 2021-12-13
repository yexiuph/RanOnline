#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_LEFTBAR_DRAG	= UIMSG_USER1;
const DWORD UIMSG_MOUSEIN_NORMAL_CHAT	= UIMSG_USER2;
const DWORD UIMSG_MOUSEIN_PRIVATE_CHAT	= UIMSG_USER3;
const DWORD UIMSG_MOUSEIN_PARTY_CHAT	= UIMSG_USER4;
const DWORD UIMSG_MOUSEIN_TOALL_CHAT	= UIMSG_USER5;
const DWORD UIMSG_MOUSEIN_GUILD_CHAT	= UIMSG_USER6;
const DWORD UIMSG_MOUSEIN_ALLIANCE_CHAT	= UIMSG_USER7;
////////////////////////////////////////////////////////////////////

class CBasicChatLeftBar : public CUIGroup
{
public:
	enum
	{
		CHAT_LEFT_BAR_TOP = 1,
		CHAT_NORMAL_STATE,
		CHAT_PRIVATE_STATE,
		CHAT_PARTY_STATE,		
		CHAT_TOALL_STATE,
		CHAT_GUILD_STATE,
		CHAT_ALLIANCE_STATE,

		TOTAL_CHAT_STATE = 6
	};

public:
	CBasicChatLeftBar ();
	virtual	~CBasicChatLeftBar ();

public:
	void	CreateSubControl ();

public:
	void	DO_CHAT_STATE ( int nSTATE );

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CUIControl*	m_pCHATSTATE[TOTAL_CHAT_STATE];
};