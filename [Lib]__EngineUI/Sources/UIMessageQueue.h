#pragma	once

#include <queue>
#include "UIDataType.h"

class CUIMessageQueue
{
public:
	struct	SUIMSG
	{
		UIGUID	cID;
		DWORD	dwMsg;
	};

private:
	typedef	std::queue<SUIMSG>		UIMSG_QUEUE; // MEMO	

public:
	CUIMessageQueue	();
	virtual	~CUIMessageQueue ();

public:
	void PostUIMessage( UIGUID cID, DWORD dwMsg );
	bool PeekUIMessage( UIGUID* pcID, DWORD* pdwMsg );

private:	
	UIMSG_QUEUE	m_MessageQueue;
};