//	�⺻ ���� ǥ��
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.11.31]
//			@ �ۼ�
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