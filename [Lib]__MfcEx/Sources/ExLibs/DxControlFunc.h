#ifndef __DXCONTROLFUNC__H__
#define __DXCONTROLFUNC__H__

//	Note : 트리의 모든 노드를 재귀적으로 모두 탐색해서 해당 "ItemText"를 가진
//		노드를 찾는다.
//
HTREEITEM FindTreeItem ( CTreeCtrl &TreeCtrl, CString StrFindItemText );

//	Note : 트리의 모든 노드를 재귀적으로 모두 탐색해서 해당 "ItemData"를 가진
//		노드를 찾는다.
//
HTREEITEM FindTreeItem ( CTreeCtrl &TreeCtrl, DWORD ItemData );

#endif //__DXCONTROLFUNC__H__