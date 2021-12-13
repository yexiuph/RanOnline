//	UIModal Ŭ���� - ��� ��� Ŭ����
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2004.03.16]
//				������Ʈ ���ۿ��ο� ���� �÷��׸� �߰��ߴ�.
//		[2003.11.04] - �� �κ�, �����丵�� ������, RanClientLib�� ��������.
//					 ����� ��Ʈ�Ѱ� �޽����� �Բ� �پ� �ִ°��� �´�.
//		[2003.10.31] - ����
//					 enginelib�� RanClientLib�� �и�
#pragma	once

#include "UIWindow.h"

class	CUIModal : public CUIWindow
{	
public:
	CUIModal ();
	virtual	~CUIModal ();

public:
	void SetCallerID ( UIGUID CallerID )	{ m_CallerID = CallerID; }
	UIGUID GetCallerID ()					{ return m_CallerID; }

public:
	void SetUseModaless ( bool bModaless )	{ m_bModaless = bModaless; }
	bool IsUseModaless ()					{ return m_bModaless; }

protected:	
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
    UIGUID		m_CallerID;
	bool		m_bModaless;
};