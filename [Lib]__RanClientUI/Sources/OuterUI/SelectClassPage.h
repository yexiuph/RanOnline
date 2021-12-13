#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "NewCreateCharEnum.h"

class CBasicTextBox;

class CSelectClassPage	: public CUIGroup
{
private:
	enum
	{
		SELECT_CLASS_NAME = NO_ID + 1,
		SELECT_CLASS_DEC,
		SELECT_CLASS_IMAGE0,
		SELECT_CLASS_IMAGE1,
		SELECT_CLASS_IMAGE2,
		SELECT_CLASS_IMAGE3,
		SELECT_CLASS_IMAGE4,
		SELECT_CLASS_SET_IMAGE,
	};

	CBasicTextBox*		m_pClassNameStatic;	
	CBasicTextBox*		m_pClassDec;

	CUIControl*			m_pSelectClassImg[MAX_CLASS];
	CUIControl*			m_pSelectClassSetImg;

	int					m_nClass;
	BOOL				m_bCreateExtream;
	int					m_nStart;
	int					m_nSchool;
	int					m_nSex;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

protected:


public:
	CSelectClassPage();
	virtual ~CSelectClassPage();


	void	SELECT_CLASS_PROCESS( int nSelect );
	
	int		GetSelectClass() { return m_nClass; }
	void	SetSelectClass( int nClass) { m_nClass = nClass; }

	void	SetCreateExtream( BOOL bCreate ) { m_bCreateExtream = bCreate; m_nStart = !bCreate; }

	void	SetImagePosition();
	void	SetTextureImage( int nSchool, int nSex );

public:
	void	CreateSubControl ();
public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
};