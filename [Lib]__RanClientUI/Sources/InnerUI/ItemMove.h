#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLDefine.h"

class	CItemImage;

class CItemMove : public CUIGroup
{
protected:
static const DWORD	dwDEFAULT_TRANSPARENCY;
static const float	fDEFAULT_MOUSE_INTERPOLIATION;

public:
	CItemMove ();
	virtual	~CItemMove ();

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void	SetItemIcon ( SNATIVEID sICONINDEX, const char* szTexture );
	SNATIVEID	GetItemIcon ();
	const CString&	GetItemIconTexture () const;
	void	ResetItemIcon ();

public:
	SNATIVEID	GetItem ();

private:
	CItemImage*	m_pItemImage;

private:
	BOOL	m_bUseSnap;
};