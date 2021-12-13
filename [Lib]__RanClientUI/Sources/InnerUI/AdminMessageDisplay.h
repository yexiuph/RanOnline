//	ä�� �ڽ�
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.11.25]
//			@ ���� �ۼ�
//

#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;

class	CAdminMessageDisplay : public CUIGroup
{
private:
static	const	float	fLIFE_TIME;

public:
	CAdminMessageDisplay ();
	virtual	~CAdminMessageDisplay ();

public:
	void	CreateSubControl ();

public:
	void	AddText ( CString strMessage, D3DCOLOR dwMessageColor );

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	float	GetLifeTime ();
	
private:
	CBasicTextBox*		m_pMessageBox;

private:
	float	m_fLifeTime;

private:
	CUIControl*		m_pSelfDummy;

private:
	CD3DFontPar* m_pFont9;
};