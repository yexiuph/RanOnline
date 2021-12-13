#pragma	once

#include "UIWindowEx.h"
#include "GLParty.h"

class	CMiniPartySlot;
struct	GLPARTY_CLIENT;
class	CPartyBuffInfo;

class	CMiniPartyWindow : public CUIWindowEx
{
protected:
	enum
	{
		MINIPARTY_SLOT_MEMBER0 = ET_CONTROL_NEXT,
		MINIPARTY_SLOT_MEMBER1,
		MINIPARTY_SLOT_MEMBER2,
		MINIPARTY_SLOT_MEMBER3,
		MINIPARTY_SLOT_MEMBER4,
		MINIPARTY_SLOT_MEMBER5,
		MINIPARTY_SLOT_MEMBER6,
		MINIPARTY_SLOT_MEMBER7,

		MINIPARTY_BUFF_INFO,
	};

public:
	CMiniPartyWindow ();
	virtual	~CMiniPartyWindow ();

public:
	void CreateSubControl ();

private:
	GLPARTY_CLIENT*		FindSelfClient ();
	BOOL	IsSameMap ( GLPARTY_CLIENT *pSelf, GLPARTY_CLIENT *pMember );

private:
	void	SetSlotMember ( CMiniPartySlot* pPartySlot, GLPARTY_CLIENT *pMember, BOOL bSameMap, const int nPartySlotID );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CMiniPartySlot*		m_pPartySlot[MAXPARTY];
	CUIControl*			m_pSlotOver[MAXPARTY];
	CPartyBuffInfo*		m_pPartyBuffInfo;
	bool				m_bMOUSEIN;
};