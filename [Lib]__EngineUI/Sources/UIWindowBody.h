#pragma	once

#include "UIGroup.h"

class	CUIWindowBody : public CUIGroup
{
public:
	CUIWindowBody ();
	virtual	~CUIWindowBody ();

public:
	void CreateBody ( char* szLeft, char* szUp, char* szMain, char* szDown, char* szRight );
};