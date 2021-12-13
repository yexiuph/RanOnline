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
	void SetTitleName ( const char* szTitleName );	// ������ Ÿ��Ʋ�� ����
	void SetTitleAlign ( int nAlign );				// ������ Ÿ��Ʋ�� ���� ����(�ؽ�Ʈ�ڽ��� ����)

private:
	CBasicTextBox*	m_pTitleName;
};