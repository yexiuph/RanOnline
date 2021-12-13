#pragma	once

///////////////////////////////////
//	µð¹ö±ë
#define	__UI_DEGUG__
///////////////////////////////////

namespace	NS_UIDEBUGSET
{
	void	BlockBegin ();
	BOOL	IsInBlock ();
	void	BlockEnd ();

	const	DWORD	BLUE = D3DCOLOR_ARGB(180,0,0,255);
	const	DWORD	YELLOW = D3DCOLOR_ARGB(180,255,255,0);
	const	DWORD	PINK = D3DCOLOR_ARGB(180,255,0,255);
	const	DWORD	RED = D3DCOLOR_ARGB(180,255,0,0);
	const	DWORD	GREEN = D3DCOLOR_ARGB(180,0,255,0);	
};