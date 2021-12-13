#pragma	once

#include "UIOuterWindow.h"

class CCreateCharacterWeb : public CUIOuterWindow
{
protected:
	enum
	{
		WEB_ID = 0,		// 웹 브라우저 식별 번호

		ALIGN_LEFT = 2,	// 왼쪽, 위 테두리 보정
		ALIGN_RIGHT = 4	// 오른쪽, 아래 테두리 보정
	};

public:
	VOID CreateWeb();
	VOID AlignWeb();

	void ShiftCharClass( WORD nIndex );

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
};