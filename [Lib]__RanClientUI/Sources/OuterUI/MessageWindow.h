//	��ų �̵�
//
//	���� �ۼ��� : sjLee
//	���� ������ : 
//	�α�
//		[2007.6.18]
//			@ �ۼ�
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;

class CMessageWindow : public CUIGroup
{
public:
	CMessageWindow ();
	virtual	~CMessageWindow ();

public:
	void	CreateSubControl ();

public:
	void	SetMessage ( const CString& strMessage );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicTextBox*		m_pTextBox;

};