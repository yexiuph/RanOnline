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

class	CBasicTextBox;

class CSystemMessageWindow : public CUIGroup
{
public:
	CSystemMessageWindow ();
	virtual	~CSystemMessageWindow ();

public:
	void	CreateSubControl ();

public:
	void	SetSystemMessage ( const CString& strMessage, const D3DCOLOR& dwColor );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicTextBox*		m_pTextBox;

private:
	float	m_fElapsedTime;
};