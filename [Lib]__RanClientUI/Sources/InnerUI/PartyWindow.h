//	파티 윈도우
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.27]
//			@ 작성
//

#pragma	once

#include "UIWindowEx.h"
#include "GLParty.h"

class	CPartySlot;
struct	GLPARTY_CLIENT;
class	CBasicTextBox;

class	CPartyWindow : public CUIWindowEx
{
protected:
	enum
	{
		PARTY_SLOT_MEMBER0 = ET_CONTROL_NEXT,
		PARTY_SLOT_MEMBER1,
		PARTY_SLOT_MEMBER2,
		PARTY_SLOT_MEMBER3,
		PARTY_SLOT_MEMBER4,
		PARTY_SLOT_MEMBER5,
		PARTY_SLOT_MEMBER6,
		PARTY_SLOT_MEMBER7,
	};

public:
	CPartyWindow ();
	virtual	~CPartyWindow ();

public:
    void	CreateSubControl ();

public:
	void	SetPartyInfo ( DWORD dwPartyID, DWORD dwMasterID );

	DWORD	GetPartyID ();
	DWORD	GetMasterID();

private:
	GLPARTY_CLIENT*		FindSelfClient ();
	BOOL	IsSameMap ( GLPARTY_CLIENT *pSelf, GLPARTY_CLIENT *pMember );

private:
	void	SetSlotMember ( CPartySlot* pPartySlot, GLPARTY_CLIENT *pMember, BOOL bSameMap, BOOL bQuitEnable, BOOL bAuthEnable );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CPartySlot*		m_pPartySlot[MAXPARTY];
	CBasicTextBox*	m_pConditionTextBox;

private:
	SPARTY_OPT	m_sPartyOption;

private:
	DWORD	m_dwPartyID;
	DWORD	m_dwMasterID;
};