//	BasicScrollThumb Ŭ����
//
//	���� �ۼ��� : ���⿱
//	���� ������ :
//	�α�
//		[2003.11.10] - ��������

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