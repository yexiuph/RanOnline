//	�⺻ ���� ǥ��
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.11.21]
//			@ �ۼ�
//

#pragma	once

#include "UIOuterWindow.h"

class	CBasicTextBox;

class CSelectCharacterCard : public CUIOuterWindow
{
public:
	CSelectCharacterCard ();
	virtual	~CSelectCharacterCard ();

public:
	void	CreateSubControl ();

public:
	void	SetUsableCard ( const int& nUsableCard );
//	void	SetUsedCard ( const int& nUsedCard );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
    CBasicTextBox* m_pUsableTextBox;
//	CBasicTextBox* m_pUsedTextBox;

private:
	USHORT m_uCharRemain;
};