#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLDefine.h"

class	CBasicProgressBar;

class	CPetSkillImage : public CUIGroup
{
public:
	CPetSkillImage ();
	virtual	~CPetSkillImage ();

public:
	void CreateSubControl ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

	void SetSkillProcess ( SNATIVEID sNativeID );
	void SetUseTwinkle( bool bUse )					{ m_bTwinkle = bUse; }

	virtual	void SetSkill ( SNATIVEID sNativeID );
	void ResetSkill ();

protected:
	virtual	void	CreateProgressBar ();

protected:
	SNATIVEID	m_sICONINDEX;
	CBasicProgressBar*	m_pSkillProcess;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	float m_TotElapsedTime;
	bool m_bSkillUsed;
	bool m_bSkillEnd;

	bool m_bTwinkle;
};