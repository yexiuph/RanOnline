#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicProgressBar;

class	CBonusTimeGauge : public CUIGroup
{

	enum
	{
		CONSUME_GAUGE,
        CHARGE_GAUGE
	};

public:
	CBonusTimeGauge ();
	virtual	~CBonusTimeGauge ();

public:
	void	SetTimeConsume ( int nNOW, int nMAX );
	void	SetTimeCharge  ( int nNOW, int nMAX );
	void	CreateSubControl ();

	void	BONUS_TIME_EVENT_START( bool bCharging ); // 이벤트 시작
	void	BONUS_TIME_EVENT_END(); // 이벤트 종료
	void	BONUS_TIME_BUSTER_START(); // 경험치 시작
	void	BONUS_TIME_BUSTER_END(); // 경험치 종료

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicProgressBar*	m_pConsume;
	CBasicProgressBar*  m_pCharge;
	bool m_bCharging; // 충천 아니면 소비 
};