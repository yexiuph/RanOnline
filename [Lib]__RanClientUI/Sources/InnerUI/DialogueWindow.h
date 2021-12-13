#pragma	once

#include "UIWindowEx.h"
#include "GLDefine.h"
#include "GLogicEx.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Npc/NpcDialogueSet.h"

class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
struct	SNpcTalk;
class	CNpcDialogueCase;
class	CBasicTextButton;
class	CBasicLineBox;

class CDialogueWindow : public CUIWindowEx
{
private:
static	const	int		nSTARTINDEX;
static	const	int		nSTARTLINE;
static	const	float	fANSWER_PART_INTERVAL;

private:
	enum
	{
		DIALOGUE_QUESTION_TEXTBOX = ET_CONTROL_NEXT,
		BASIC_DIALOGUE_SCROLLBAR,
		DIALOGUE_ANSWER_TEXTBOX,
		BASIC_DIALOGUE_ANSWER_SCROLLBAR,
		RANDOM_TIME_BUTTON,
		RANDOM_TIME_TEXT
	};

public:
	CDialogueWindow ();
	virtual	~CDialogueWindow ();
	
	float	m_fRandomTime;		// 흐른시간
	DWORD	m_dwRandomTime;		// 제한시간

public:
	void	CreateSubControl ();

public:
	BOOL	SetDialogueData ( STARGETID sCrowID, GLCHARLOGIC *pCHAR );
	STARGETID	GetCrowID ()				{ return m_sCrowID; }

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	void	TranslateTalkMessage ( SNpcTalk* pNpcTalk );
	void	TranslateBasicMessage ( SNpcTalk* pNpcTalk );
	void	TranslateQuestStartMessage ( SNpcTalk* pNpcTalk );
	void	TranslateQuestStepMessage ( SNpcTalk* pNpcTalk );

private:
	void	LoadNode(DWORD NativeID, bool bRandom = false, DWORD dwRandomTime = 0 );
	void	LoadBasicTalk ();
	void	LoadButtonTalk ( bool bRandom = false );	

private:
	CBasicTextBox*		m_pQuestionBox;
	CBasicScrollBarEx*	m_pScrollBar;
	CBasicTextBoxEx*	m_pAnswerBox;
	CBasicScrollBarEx*	m_pAnswerScrollBar;
	CBasicTextButton*	m_pRandomTimeButton;
	CBasicTextBox*		m_pRandomTimeText;
	CBasicLineBox*		m_pRandomTimeBack;

private:
	CNpcDialogueSet		m_DialogueSet;
	CNpcDialogue*		m_pDialogue;
	CNpcDialogueCase*	m_pDialogueCase;

private:
	GLCHARLOGIC*	m_pCHAR;
	STARGETID		m_sCrowID;
};