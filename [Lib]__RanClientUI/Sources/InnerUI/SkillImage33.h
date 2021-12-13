//	기본 정보 표시
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.27]
//			@ 작성
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