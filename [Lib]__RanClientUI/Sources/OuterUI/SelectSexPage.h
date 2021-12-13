#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "NewCreateCharEnum.h"

class CSelectSexPage	: public CUIGroup
{
private:
	enum
	{
		SELECT_SEX_IMAGE0 = NO_ID + 1,
		SELECT_SEX_IMAGE1,
		SELECT_SEX_SET_IMAGE0,
		SELECT_SEX_SET_IMAGE1,
	};

	CUIControl*			m_pSelectSexImg[MAX_SEX];
	CUIControl*			m_pSelectSexSetImg[MAX_SEX];

	int					m_nSex;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

protected:


public:
	CSelectSexPage();
	virtual ~CSelectSexPage();


	void	SELECT_SEX_PROCESS( int nSelect );
	
	int		GetSelectSex() { return m_nSex; }
	void	SetSelectSex(int nSex) { m_nSex = nSex; }

	void	SetTextureImage( int nSchool );

public:
	void	CreateSubControl ();
public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
};