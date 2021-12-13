#pragma	once

#include "UIWindowEx.h"
#include "ClubUnion.h"
#include "ClubMember.h"
#include "ClubAnnounce.h"
#include "ClubBattle.h"

class CBasicTextButton;
class CClubMember;
class CClubAnnounce;
class CClubUnion;
class GLCLUB;
class CClubBattle;

//const DWORD UIMSG_MOUSEIN_ANNOUNCE_MAKE = UIMSG_USER1;
const DWORD UIMSG_MOUSEIN_SUBMASTER_MAKE = UIMSG_USER2;

class	CClubWindow : public CUIWindowEx
{
private:
static	const int nLIMIT_CHAR;

private:
	enum
	{
		CLUB_KICK_BUTTON = ET_CONTROL_NEXT,	// 제명(0) 버튼
		CLUB_LEAVE_BUTTON,					// 탈퇴(6) 버튼
		CLUB_AUTHORITY_BUTTON,				// 위임 버튼
		CLUB_WRITE_BUTTON,					// 쓰기(10) 버튼
		CLUB_WRITE_OK_BUTTON,				// 확인 버튼
		CLUB_WRITE_CANCEL_BUTTON,			// 취소 버튼
		CLUB_WRITE_CLEAR_BUTTON,			// 모두지움(12) 버튼
		CLUB_SUBMASTER_BUTTON,				// 부마설정(11) 버튼
		CLUB_BREAKUP_BUTTON,				// 클럽 해체 버튼
		CLUB_CHANGE_MARK_BUTTON,			// 마크 변경 버튼
		CLUB_CHANGE_TITLE_BUTTON,			// 별명 수정 버튼

		CLUB_UNION_KICK_BUTTON,				// 동맹 제명(0) 버튼
		CLUB_UNION_LEAVE_BUTTON,			// 동맹 탈퇴(6) 버튼
		CLUB_UNION_BREAKUP_BUTTON,			// 동맹 해체 버튼

		CLUB_BATTLE_ARMISTICE_BUTTON,		// 배틀 휴전 버튼
		CLUB_BATTLE_SUBMISSION_BUTTON,		// 배틀 항복 버튼

		CLUB_MEMBER_BUTTON,					// 멥버탭 버튼
		CLUB_ANNOUNCEMENT_BUTTON,			// 공지탭 버튼
		CLUB_UNION_BUTTON,					// 연합탭 버튼
		CLUB_BATTLE_BUTTON,					// 적대탭 버튼

		CLUB_MEMBER_PAGE,					// 멤버 페이지
		CLUB_ANNOUNCEMENT_PAGE,				// 공지 페이지
		CLUB_UNION_PAGE,					// 연합 페이지
		CLUB_BATTLE_PAGE,					// 적대 페이지
	};

public:
	CClubWindow ();
	virtual ~CClubWindow ();

public:
	void CreateSubControl();
	CBasicTextButton* CreateTextButton14( char* szButton, UIGUID ControlID, char* szText );	
	CBasicTextButton* CreateTextButton18( char* szButton, UIGUID ControlID, char* szText, BOOL bFlip );

private:
	CBasicTextBox*	CreateTextBox ( char* szKeyword, CD3DFontPar* pFont );

public:
	const DWORD	GetClubMemberID()		{ return m_pClubMember->GetClubMemberID(); }
	const CString GetClubMemberName()	{ return m_pClubMember->GetClubMemberName(); }
	const DWORD	GetUnionMemberID ()		{ return m_pClubUnion->GetUnionMemberID(); }
	const CString GetUnionMemberName()	{ return m_pClubUnion->GetUnionMemberName(); }
	const DWORD GetBattleClubID()		{ return m_pClubBattle->GetBattleClubID(); }
	const CString GetBattleClubName()	{ return m_pClubBattle->GetBattleClubName(); }
	const bool GetIsAllianceBattle()	{ return m_pClubBattle->GetIsAlliance(); } 

	void SetAnnouncement()				{ m_pClubAnnounce->SetAnnouncement(); }

public:
	virtual	void TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	virtual	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	void ResetState();
	void RefreshState();
	void LoadClubMemberList()			{ m_pClubMember->LoadClubMemberList(); }
	void LoadClubAllianceList()			{ m_pClubUnion->LoadUnionMemberList(); }
	void LoadClubBattleList()			{ m_pClubBattle->LoadClubBattleList(); }

private:
	void	RefreshTime ( const __time64_t& tTime );

protected:
	void SetVisiblePage( UIGUID cID );

private:
	CBasicTextBox*		m_pClubName;
	CBasicTextBox*		m_pClubGrade;
	CBasicTextBox*		m_pMasterName;
	CBasicTextBox*		m_pEstablishDate;
	CBasicTextBox*		m_pEstablishDateStatic;
	CBasicTextBox*		m_pNickName;
	CUIControl*			m_pClubMark;
	CBasicTextBox*		m_pBattlePoint;
	CBasicTextBox*		m_pAllianceBattlePoint;

private:
	CBasicTextButton*	m_pKickButton;
	CBasicTextButton*	m_pLeaveButton;
	CBasicTextButton*	m_pAuthorityButton;
	CBasicTextButton*	m_pWriteButton;
	CBasicTextButton*	m_pWriteButtonOK;
	CBasicTextButton*	m_pWriteButtonCancel;
	CBasicTextButton*	m_pWriteButtonClear;
	CBasicTextButton*	m_pSubMasterButton;
	CBasicTextButton*	m_pBreakupButton;
	CBasicTextButton*	m_pChangeMarkButton;
	CBasicTextButton*	m_pChangeNickButton;

	CBasicTextButton*	m_pUnionKickButton;
	CBasicTextButton*	m_pUnionLeaveButton;
	CBasicTextButton*	m_pUnionBreakupButton;

	CBasicTextButton*	m_pBattleArmisticeButton;
    CBasicTextButton*	m_pBattleSubmissionButton;

	// 탭 버튼
	CBasicTextButton*	m_pMemberButton;
	CBasicTextButton*	m_pAnnouncementButton;
	CBasicTextButton*	m_pUnionButton;
	CBasicTextButton*	m_pBattleButton;

    bool m_bClubMaster;
	INT m_nPageMode;
	BOOL m_bEditBox;

private:
	CClubMember*	m_pClubMember;
	CClubAnnounce*	m_pClubAnnounce;
	CClubUnion*		m_pClubUnion;
	CClubBattle*	m_pClubBattle;
};