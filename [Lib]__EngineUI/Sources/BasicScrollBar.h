#pragma	once

#include "UIGroup.h"

class CBasicScrollThumbFrame;

class	CBasicScrollBar : public CUIGroup
{
private:
	enum	//	�⺻ ������ ID ( �ܺη� �������� ���� )
	{
		ET_CONTROL_UP = NO_ID + 1,
		ET_CONTROL_DN,
	};

public:
	CBasicScrollBar ();
	virtual	~CBasicScrollBar ();

public:
	void CreateThumb ( char* szFrame, char* szThumb, char* szBodyValue, char* szUpValue, char* szDnValue, const bool& bVERTICAL = true );
	void CreateButton ( char* szUpValue, char* szUpFlipValue, char* szDnValue, char* szDnFlipValue, const bool& bVERTICAL = true );

	CBasicScrollThumbFrame* const GetThumbFrame () const;
	void SetVisibleThumb( bool bVisible );

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicScrollThumbFrame*	m_pThumbFrame;
};