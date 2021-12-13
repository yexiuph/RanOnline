//	BasicScrollThumb 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 :
//	로그
//		[2003.11.10] - 최초제작

#pragma	once

#include "UIGroup.h"

class	CBasicScrollThumb : public CUIGroup
{
public:
	CBasicScrollThumb ();
	virtual	~CBasicScrollThumb ();

public:
	void	CreateSkin ( char* szBodyValue, char* szUpValue, char* szDnValue, const bool& bVERTICAL );
};