#ifndef __DXCONTROLFUNC__H__
#define __DXCONTROLFUNC__H__

//	Note : Ʈ���� ��� ��带 ��������� ��� Ž���ؼ� �ش� "ItemText"�� ����
//		��带 ã�´�.
//
HTREEITEM FindTreeItem ( CTreeCtrl &TreeCtrl, CString StrFindItemText );

//	Note : Ʈ���� ��� ��带 ��������� ��� Ž���ؼ� �ش� "ItemData"�� ����
//		��带 ã�´�.
//
HTREEITEM FindTreeItem ( CTreeCtrl &TreeCtrl, DWORD ItemData );

#endif //__DXCONTROLFUNC__H__