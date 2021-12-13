//	스킬 이동
//
//	최초 작성자 : sjLee
//	이후 수정자 : 
//	로그
//		[2007.6.18]
//			@ 작성
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