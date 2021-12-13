#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLDefine.h"

class CPetSkillImage;

class CPetQuickSkillSlot : public CUIGroup
{
protected:
	enum
	{
		QUICK_SKILL_MOUSE_OVER = NO_ID + 1,
	};

public:
	CPetQuickSkillSlot ();
	virtual	~CPetQuickSkillSlot ();

public:
	virtual	void	CreateSubControl ();

public:
	UIRECT	GetAbsPosSkillImage();

protected:	
	void	CreateSkillImage ();
	void	CreateMouseOver ();
	void	UpdateSlot( SNATIVEID sNativeID );

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CPetSkillImage*		m_pSkillImage;

protected:
	CUIControl*			m_pSkillMouseOver;

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
};