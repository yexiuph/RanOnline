#pragma	once

#include "UIWindowEx.h"
#include "GLSkill.h"

class	CPetSkillPage;

class	CPetSkillWindow : public CUIWindowEx
{
protected:
	enum
	{
		PET_SKILL_WINDOW_PAGE = ET_CONTROL_NEXT
	};

public:
	CPetSkillWindow ();
	virtual	~CPetSkillWindow ();

public:
	void	CreateSubControl ();
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CPetSkillPage * m_pPage;
};