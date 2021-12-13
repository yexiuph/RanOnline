//	��ų �̵�
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.12.8]
//			@ �ۼ�
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLDefine.h"

class	CSkillImage;

class CSkillWindowToTray : public CUIGroup
{
protected:
static const DWORD	dwDEFAULT_TRANSPARENCY;
static const float	fDEFAULT_MOUSE_INTERPOLIATION;

public:
	CSkillWindowToTray ();
	virtual	~CSkillWindowToTray ();

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void		SetSkill ( SNATIVEID sNativeID );
	SNATIVEID	GetSkill ()							{ return m_sNativeID; }
	void		ResetSkill ();

public:
	void	SetUseSnap ( BOOL bUseSnap )			{ m_bUseSnap = bUseSnap; }
	BOOL	IsUseSnap ()							{ return m_bUseSnap; }

private:
	CSkillImage*	m_pSkillImage;
	SNATIVEID		m_sNativeID;

private:
	BOOL	m_bUseSnap;
};