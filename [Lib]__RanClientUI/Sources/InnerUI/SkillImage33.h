//	�⺻ ���� ǥ��
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.11.27]
//			@ �ۼ�
//

#pragma	once

#include "SkillImage.h"

class	CBasicProgressBar;

class	CSkillImage33 : public CSkillImage
{
public:
	CSkillImage33 ();
	virtual	~CSkillImage33 ();

protected:
	virtual	void	CreateProgressBar ();

public:
	virtual	void	SetSkill ( SNATIVEID sNativeID );
};