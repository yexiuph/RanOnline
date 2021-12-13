#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CDayBox : public CUIGroup
{
	enum
	{
		NUMBER_IMAGE = NO_ID+1,
		NUMBER_IMAGE_TEN,
	};

public:
	CDayBox();
	~CDayBox();

public:
	void CreateSubControl();
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	void CalcTexturePos();
	void SetDay ( int nDay, int nDayOfWeek );
	int GetDay() { return m_nDay; }
	void SetAttendance( bool bAttendance );
	bool GetAttendance() { return m_bAttendance; }
	void SetAttendReward( bool bAttendReward );
	bool GetAttendReward() { return m_bAttendReward; }

	void SetMarkPos();

	void ResetData();

public:
	CUIControl* m_pNumberImage;
	CUIControl* m_pNumberImageTen;
	CUIControl* m_pAttendanceMark;
	CUIControl*	m_pAttendRewardMark;

	int	m_nDay;
	int m_nDayOfWeek;
	DWORD m_dwColor;
	bool m_bAttendance;
	bool m_bAttendReward;

private:
	CUIControl * CreateControl( const char * szControl ); // Note : 상위 클래스 함수 재정의

};