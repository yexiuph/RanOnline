#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

struct	SNATIVEID;
class	CBasicProgressBar;
class	CSkillImage;

class	CSkillTimeUnit : public CUIGroup
{
public:
	CSkillTimeUnit ();
	virtual	~CSkillTimeUnit ();

public:
	void	CreateSubControl ();

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void	SetLeftTime ( const float& fLeftTime );
	void	SetSkill ( const DWORD& dwID, const float& fLifeTime, const CString& strSkillName );

private:
	float	m_fLEFT_TIME;
	float	m_fLIFE_TIME;

private:
	CString				m_strSkillName;
	CBasicProgressBar*	m_pProgressBar;
	CSkillImage*		m_pSkillImage;
};