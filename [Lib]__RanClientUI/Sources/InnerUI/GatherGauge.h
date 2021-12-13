#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicProgressBar;
class	CBasicLineBoxEx;

class	CGatherGauge : public CUIGroup
{
public:
	CGatherGauge ();
	virtual	~CGatherGauge ();

public:
	void	SetTime ( float fTime );
	void	SetMaterial ( DWORD dwGaeaID ) { m_dwGaeaID = dwGaeaID; }
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	void	UPDATE_POS ( D3DXVECTOR3 vPos );
	bool	IS_VISIBLE_NAME ( D3DXVECTOR3 vPos, D3DXVECTOR3 & vScreen );

private:
	CBasicProgressBar*	m_pGauge;
	
	float	m_fGatherTime;
	float	m_fTime;
	DWORD	m_dwGaeaID;		//	재료아이템 ID
};