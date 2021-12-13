#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CItemShopIcon : public CUIGroup
{
public:
	CItemShopIcon ();
	virtual	~CItemShopIcon ();

	CUIControl* m_pItemShopImage;

	bool	m_bVisible;
	bool	m_bRender;
	bool	m_bUsedMemPool;

public:
	void	CreateSubControl ();

	void	INITIALIZE();

	bool	IsMemPool()					{ return m_bUsedMemPool; }
	void	SetMemPool()				{ m_bUsedMemPool = true; }

	bool	IsVisible()					{ return m_bVisible; }
	void	SetVisible( bool bVisible ) { m_bVisible = bVisible; }

	bool	IsRender()					{ return m_bRender; }
	void	SetRender( bool bRender )	{ m_bRender = bRender; }

};