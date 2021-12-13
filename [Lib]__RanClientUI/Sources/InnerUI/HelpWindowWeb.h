#pragma	once

#include "UIWindowEx.h"

//--------------------------------------------------------------------
// HelpWindowWeb : 최준혁
//--------------------------------------------------------------------
class CHelpWindowWeb : public CUIWindowEx
{
protected:
	enum
	{
		ALIGN_LEFT = 2,	// 왼쪽, 위 테두리 보정
		ALIGN_RIGHT = 4	// 오른쪽, 아래 테두리 보정
	};

public:
	VOID CreateWeb();
	VOID AlignWeb();

public:
	VOID SetVisibleSingle( BOOL bVisible );
	VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
};