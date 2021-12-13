#pragma	once

#include "UIGroup.h"

class CD3DFontPar;
class CBasicTextBox;

class	CUIWindowTitle : public CUIGroup
{
public:
	CUIWindowTitle ();
	virtual	~CUIWindowTitle ();

public:
	void CreateTitle ( char* szLeft, char* szMid, char* szRight );
	void CreateTitleName ( char* szTextBox, const char* szText, CD3DFontPar* pFont8 );

public:
	void SetTitleName ( const char* szTitleName );	// 윈도우 타이틀을 설정
	void SetTitleAlign ( int nAlign );				// 윈도우 타이틀의 문단 정렬(텍스트박스에 정의)

private:
	CBasicTextBox*	m_pTitleName;
};