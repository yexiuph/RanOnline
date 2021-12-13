#pragma	once

#include "UIWindowEx.h"

class CItemShopWindowWeb : public CUIWindowEx
{
protected:
	enum
	{
		ALIGN_LEFT = 2,	// ����, �� �׵θ� ����
		ALIGN_RIGHT = 4	// ������, �Ʒ� �׵θ� ����
	};

public:
	VOID CreateWeb();
	VOID AlignWeb();

public:
	VOID SetVisibleSingle( BOOL bVisible );
	VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
};