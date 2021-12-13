#pragma	once

#include "UIOuterWindow.h"

class CCreateCharacterWeb : public CUIOuterWindow
{
protected:
	enum
	{
		WEB_ID = 0,		// �� ������ �ĺ� ��ȣ

		ALIGN_LEFT = 2,	// ����, �� �׵θ� ����
		ALIGN_RIGHT = 4	// ������, �Ʒ� �׵θ� ����
	};

public:
	VOID CreateWeb();
	VOID AlignWeb();

	void ShiftCharClass( WORD nIndex );

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
};