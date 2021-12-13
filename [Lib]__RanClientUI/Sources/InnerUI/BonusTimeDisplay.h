#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBonusTimeType;
class	CBonusTimeDisplay : public CUIGroup
{
public:
	enum
	{
		BONUS_TIME = NO_ID + 1,
		MAX_TYPE = 1
	};

public:
	CBonusTimeDisplay ();
	virtual	~CBonusTimeDisplay ();

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	bool	START ( );
	bool	RESET ( );
	bool	STOP  ( );

	bool	KEEP_START ();
	void	KEEP_STOP ();

private:
	CBonusTimeType*		m_pBonus_TYPE;
	CUIControl*			m_pBonus_KEEP;

	CUIControl*		m_pPositionControl;	
};