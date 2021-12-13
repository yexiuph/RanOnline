#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLCharData.h"

class	CSkillTimeDisplay : public CUIGroup
{
private:
	enum
	{
		SKILLTIME_DUMMY_BASE = NO_ID + 1,
		SKILLTIME_DUMMY_SIZE = 5,
		SKILLTIME_UNIT = 1000,
		SKILLTIME_UNIT_END = SKILLTIME_UNIT + 10000,
	};

public:
	CSkillTimeDisplay ();
	virtual	~CSkillTimeDisplay ();

public:
	void	CreateSubControl ();

private:
	bool	NEW_PLAY_SKILL ( const DWORD& dwSkillID, const WORD wLEVEL );
	UIGUID	GET_EXIST_PLAY_SKILL_CONTROL ( const DWORD& dwSkillID );
	bool	GET_PLAY_SKILLFACT ( const DWORD& dwSkillID, SSKILLFACT& sSKILLFACT );

private:
	bool	GET_SKILL_LIFE_TIME ( const DWORD& dwSkillID, const WORD wLEVEL, float& fLifeTime );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	typedef	std::pair<DWORD,UIGUID>			SKILLTIME_PAIR;
	typedef	std::list<SKILLTIME_PAIR>		SKILLTIME_DISPLAY_LIST;
	typedef	SKILLTIME_DISPLAY_LIST::iterator	SKILLTIME_DISPLAY_LIST_ITER;

	SKILLTIME_DISPLAY_LIST	m_SkillPlayList;
	UIGUID	m_NEWID;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

private:
	CUIControl*		m_pSkillUnitDummy[SKILLFACT_SIZE];
};