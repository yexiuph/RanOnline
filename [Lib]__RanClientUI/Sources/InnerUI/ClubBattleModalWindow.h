#pragma	once

#include "UIWindowEx.h"

class CBasicTextBox;
class CBasicTextButton;
class CUIEditBox;

class CClubBattleModal : public CUIWindowEx
{
	enum 
	{
		CLUB_BATTLE_MODAL_EDIT_HOUR = ET_CONTROL_NEXT,
		CLUB_BATTLE_MODAL_EDIT_MIN,
		CLUB_BATTLE_OK,
		CLUB_BATTLE_CANCEL,
	};

public:
	CClubBattleModal();
	~CClubBattleModal();

public:
	void CreateSubControl();

	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	CBasicTextBox * CreateStaticControl( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CUIControl * CreateControl( const char * szControl ); // Note : 상위 클래스 함수 재정의

public:
	void SetClubName( const CString strClubName ) { m_strClubName = strClubName; }
	void SetBattleTime ( DWORD dwTime ) { m_dwTime = dwTime; } 
	void SetAlliance ( bool bAlliance ) { m_bAlliance = bAlliance; }

	void Init();

public:
	DWORD	m_dwTime;
	CString m_strClubName;
	bool	m_bAlliance;
	
	CUIEditBox*	m_pEditBoxHour;
	CUIEditBox*	m_pEditBoxMin;
	CBasicTextBox* m_pClubName;
	CBasicTextBox* m_pInfoText;
};