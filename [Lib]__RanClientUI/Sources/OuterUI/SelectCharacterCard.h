//	기본 정보 표시
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.21]
//			@ 작성
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