//	기본 정보 표시
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.31]
//			@ 작성
//

#pragma	once

#include "BasicLineBox.h"

class	CBasicLineBoxButton : public CBasicLineBox
{
public:
	CBasicLineBoxButton ();
	virtual	~CBasicLineBoxButton ();

public:
	void	CreateBaseButton ( char* szBaseFrame, char* szBoxButton, char* szBoxButtonFlip );

private:
	void	CreateButtonImage ( char* szBoxButton, char* szBoxButtonFlip );
};