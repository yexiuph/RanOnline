#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CAcademyConftDisplay : public CUIGroup
{
private:
	typedef	CUIControl*	P_CUICONTROL;

static	const	float	fALPHA_FULL_TIME;

private:
	enum
	{
		BASE_CONTROL_ID = NO_ID + 1,
		BASE_CONTROL_ID_END = BASE_CONTROL_ID + 1000,
	};

public:
	CAcademyConftDisplay ();
	virtual	~CAcademyConftDisplay ();

public:
	void	CreateSubControl ();

public:
	bool	SET_MARK_L ( const int& nMARK );
	bool	SET_MARK_R ( const int& nMARK );
	bool	SET_NUMBER_L ( const int& nNUMBER );
	bool	SET_NUMBER_R ( const int& nNUMBER );

	int	GET_MARK_L ();
	int	GET_MARK_R ();
	int	GET_NUMBER_L ();
	int	GET_NUMBER_R ();

private:
	void	UPDATE_MARK_POS_L ();
	void	UPDATE_MARK_POS_R ();
	void	UPDATE_NUMBER_POS_L ();
	void	UPDATE_NUMBER_POS_R ();

public:
	void	UPDATED_NUMBER ();
	void	UPDATE_ALPHA ( const WORD Alpha );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl*		REGISTER_NUMBER ( const int& nNUMBER );
	CUIControl*		REGISTER_MARK ( const int& nMARK );
	
	bool	INIT_CONTROL ( P_CUICONTROL& pControl );

	CUIControl*	CreateControl ( char* szControl );

private:
	UIGUID	MAKE_NEWID ();	

private:
	int	m_nNumberLEFT;
	int	m_nMarkLEFT;
	int	m_nNumberRIGHT;
	int	m_nMarkRIGHT;

	CUIControl*	m_pNumberLEFT;	
	CUIControl*	m_pMarkLEFT;	
	CUIControl*	m_pNumberRIGHT;	
	CUIControl*	m_pMarkRIGHT;

	CUIControl*	m_pNumberLEFTDummy;	
	CUIControl*	m_pMarkLEFTDummy;	
	CUIControl*	m_pNumberRIGHTDummy;
	CUIControl*	m_pMarkRIGHTDummy;

	bool	m_bUSE_UPDATE_ALPHA;
	float	m_fUpdateNumberTime;

private:
	UIGUID	m_NEWID;

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
};