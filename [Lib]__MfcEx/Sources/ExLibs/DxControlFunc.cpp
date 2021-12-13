#include "pch.h"
#include <afxcmn.h>

#include "DxControlFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HTREEITEM FindTreeItem ( CTreeCtrl &TreeCtrl, HTREEITEM &hCurItem, CString &StrFindItemText )
{
	CString &StrItemText = TreeCtrl.GetItemText ( hCurItem );
	if ( StrItemText == StrFindItemText )	return hCurItem;

	if ( TreeCtrl.ItemHasChildren ( hCurItem ) )
	{
		HTREEITEM hChildItem = TreeCtrl.GetChildItem ( hCurItem );

		while ( hChildItem != NULL )
		{
			HTREEITEM hFindItem = FindTreeItem ( TreeCtrl, hChildItem, StrFindItemText );
			if ( hFindItem )	return hFindItem;

			hChildItem = TreeCtrl.GetNextItem ( hChildItem, TVGN_NEXT );
		}
	}

	return NULL;
}

HTREEITEM FindTreeItem ( CTreeCtrl &TreeCtrl, CString StrFindItemText )
{
	HTREEITEM hRootItem = TreeCtrl.GetRootItem();

	return FindTreeItem ( TreeCtrl, hRootItem, StrFindItemText );
}

HTREEITEM FindTreeItem ( CTreeCtrl &TreeCtrl, HTREEITEM &hCurItem, DWORD dwItemData )
{
	DWORD dwCurData = (DWORD) TreeCtrl.GetItemData ( hCurItem );
	if ( dwCurData == dwItemData )	return hCurItem;

	if ( TreeCtrl.ItemHasChildren ( hCurItem ) )
	{
		HTREEITEM hChildItem = TreeCtrl.GetChildItem ( hCurItem );

		while ( hChildItem != NULL )
		{
			HTREEITEM hFindItem = FindTreeItem ( TreeCtrl, hChildItem, dwItemData );
			if ( hFindItem )	return hFindItem;

			hChildItem = TreeCtrl.GetNextItem ( hChildItem, TVGN_NEXT );
		}
	}

	return NULL;
}

HTREEITEM FindTreeItem ( CTreeCtrl &TreeCtrl, DWORD dwItemData )
{
	HTREEITEM hRootItem = TreeCtrl.GetRootItem();

	return FindTreeItem ( TreeCtrl, hRootItem, dwItemData );
}

