//	�⺻ ���� ǥ��
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.11.27]
//			@ �ۼ�
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextButtonImage : public CUIGroup
{
public:
	CBasicTextButtonImage ();
	virtual	~CBasicTextButtonImage ();

public:
	void	CreateButtonImage ( const char* szLeft, const char* szCenter, const char* szRight );
};