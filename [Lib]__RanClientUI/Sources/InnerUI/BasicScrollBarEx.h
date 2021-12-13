#pragma	once

#include "../[Lib]__EngineUI/Sources/BasicScrollBar.h"

class	CBasicScrollBarEx : public CBasicScrollBar
{
public:
	CBasicScrollBarEx ();
	virtual	~CBasicScrollBarEx ();

public:
    void	CreateBaseScrollBar ( char* szBaseScrollBar, const bool& bVERTICAL = true );
};