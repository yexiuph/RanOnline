#pragma	once

#include "UIWindowEx.h"

#define DAY_BOX_NUM		35	// 5�����϶� ����
#define	DAY_BOX_COUNT	37	// �ѷ����� ����

class	CDayBox;
class	CBasicTextBox;


class CAttendanceBookWindow : public CUIWindowEx
{

	enum
	{
		MONTH_IMAGE = ET_CONTROL_NEXT,
	    COMBO_ATTEND,
//		MONTH_IMAGE,
//		MONTH_IMAGE_TEN,
		DAY_BOX_IMAGE,
		DAY_BOX_IMAGE2 = DAY_BOX_IMAGE + DAY_BOX_COUNT,		
	};


public:
	CAttendanceBookWindow ();
	virtual	~CAttendanceBookWindow ();

public:
	void CreateSubControl();

	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:

	void ResetDayBox();
	void SetDayBox();
	void RefreshAttend();
	void SetSixLinePos( int nIndex );
	void ReSetSixLinePos();
	void SetMonthTexPos( int nMonth );
	void SetComboTexPos( int nComboDay );

public:
	int		m_nCurDay;		//	���� ��¥
	int		m_nFirstDay;	//	ù°�� ����
	bool	m_bSixLine;		//	6°�� ���� ���
	bool	m_bAttend;		//	�⼮üũ��

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	CDayBox*	m_pDayBox[DAY_BOX_COUNT];

    CUIControl*	m_pMonthImage;
	CUIControl* m_pMonthImageTen;
	CUIControl* m_pMonthImageEng;

	CUIControl* m_pComboImage;
	CUIControl* m_pComboImageTen;

private:
	CUIControl * CreateControl( const char * szControl ); // Note : ���� Ŭ���� �Լ� ������
	CBasicTextBox* CreateStaticControl( const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID );
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

};