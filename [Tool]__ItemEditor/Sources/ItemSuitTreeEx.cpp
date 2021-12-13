#include "pch.h"
#include <vector>
#include "ItemEdit.h"
#include "SheetWithTab.h"
#include "ItemSuitTree.h"
#include "GLStringTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CItemSuitTree::UpdateItem ( SITEM* pItem, HTREEITEM hMainItem )
{
	CString strItem;
	strItem = GetFormatName ( pItem );
	
	if ( !hMainItem )
	{
		hMainItem = m_ctrlItemTree.InsertItem ( strItem, m_TreeRoot );
		m_ctrlItemTree.SetItemData ( hMainItem, (DWORD_PTR)pItem );
	}
	else
	{
		HTREEITEM hSubItem = m_ctrlItemTree.InsertItem ( strItem, hMainItem );
		m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)pItem );
	}

	return TRUE;
}

BOOL CItemSuitTree::DelItem ()
{
	if ( GetSelectItem() )
	{
		WORD WID = GetSelectItem()->m_sItem.sBasicOp.sNativeID.wMainID;
		WORD SID = GetSelectItem()->m_sItem.sBasicOp.sNativeID.wSubID;

		if ( SID == 0 )	// 루트를 삭제하는 것이면...
		{
			for ( int i = 0; i < GLItemMan::MAX_SID; i++ )
			{
				GLStringTable::GetInstance().DeleteString( WID, i, GLStringTable::ITEM );
				GLItemMan::GetInstance().DeleteItem ( WID, i );
			}
		}
		else
		{
			GLStringTable::GetInstance().DeleteString( GetSelectItem()->m_sItem.sBasicOp.strName,
				GetSelectItem()->m_sItem.sBasicOp.strComment, GLStringTable::ITEM );
			GLItemMan::GetInstance().DeleteItem ( GetSelectItem()->m_sItem.sBasicOp.sNativeID );
		}		
		
		m_ctrlItemTree.DeleteItem ( m_ctrlItemTree.GetSelectedItem () );

		return TRUE;	
	}

	return FALSE;
}

void CItemSuitTree::UpdateName ()
{
	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
	GASSERT ( hItem );
	PITEMNODE pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem );
	GASSERT ( pItemNode );

	m_ctrlItemTree.SetItemText ( hItem, GetFormatName ( &pItemNode->m_sItem ) );
}

const char* CItemSuitTree::GetFormatName ( SITEM* pItem )
{
	GASSERT ( pItem );

	//** !!Add New Sex!!     
	static	CString strItem;
	if ( pItem->sBasicOp.dwReqCharClass==GLCC_NONE)
	{
		strItem.Format ( "[%04d/%02d]  %s  [{%s}]",
			pItem->sBasicOp.sNativeID.wMainID, pItem->sBasicOp.sNativeID.wSubID, pItem->GetName(), "Unusable" );
	}
	else if ( pItem->sBasicOp.dwReqCharClass==GLCC_ALL_NEWSEX)
	{
		strItem.Format ( "[%04d/%02d]  %s  [{%s}]",
			pItem->sBasicOp.sNativeID.wMainID, pItem->sBasicOp.sNativeID.wSubID, pItem->GetName(), "Common" );
	}
	else
	{
		strItem.Format ( "[%04d/%02d]  %s  [",
			pItem->sBasicOp.sNativeID.wMainID, pItem->sBasicOp.sNativeID.wSubID, pItem->GetName() );

		if ( pItem->sBasicOp.dwReqCharClass&GLCC_FIGHTER_M )		strItem += "FighterM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_FIGHTER_W )	strItem += "FighterW|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ARMS_M )			strItem += "ArmsM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ARMS_W )		strItem += "ArmsW|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ARCHER_M )		strItem += "ArcherM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ARCHER_W )		strItem += "ArcherW|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_SPIRIT_M )		strItem += "SpiritM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_SPIRIT_W )		strItem += "SpiritW|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_EXTREME_M )	strItem += "ExtremeM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_EXTREME_W )	strItem += "ExtremeW|";
		// geger009 | gunner and assasin not implemented yet, turn it off for now
		/*if ( pItem->sBasicOp.dwReqCharClass&GLCC_SCIENTIST_M )	strItem += "ScienceM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_SCIENTIST_W )	strItem += "ScienceW|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ASSASSIN_M )	strItem += "AssassinM|";
		if ( pItem->sBasicOp.dwReqCharClass&GLCC_ASSASSIN_W )	strItem += "AssassinW";*/

		strItem += "]";
	}

	CString strNewName;
	strNewName.Format( "IN_%03d_%03d", pItem->sBasicOp.sNativeID.wMainID, pItem->sBasicOp.sNativeID.wSubID );
	pItem->sBasicOp.strName = strNewName;
	strNewName.Format( "ID_%03d_%03d", pItem->sBasicOp.sNativeID.wMainID, pItem->sBasicOp.sNativeID.wSubID );
	pItem->sBasicOp.strComment = strNewName;

	return strItem.GetString ();
}

BOOL	CItemSuitTree::AddItem ()
{
	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && GetSelectItem () )	//	서브로 추가시
	{		
		return AddItemMain( hMainItem );
	}
	else	//	메인으로 추가시
	{
		return AddItemSub ();
	}
	
	return FALSE;
}

BOOL CItemSuitTree::AddItemMain( HTREEITEM hMainItem )
{
	WORD MID = GetSelectItem()->m_sItem.sBasicOp.sNativeID.wMainID;
	WORD SID = GLItemMan::GetInstance().FindFreeSID ( MID );
	if ( SID == ITEMID_NOTFOUND )
	{
		return FALSE;
	}

	CItemNode sItemNode;
	sItemNode = *m_pDummyItemNode;
	sItemNode.m_sItem.sBasicOp.sNativeID.wMainID = MID;
	sItemNode.m_sItem.sBasicOp.sNativeID.wSubID = SID;

	char szNewName[ITEM_SZNAME];
	sprintf_s ( szNewName, "IN_%03d_%03d", MID, SID );
	sItemNode.m_sItem.sBasicOp.strName = szNewName;
	sprintf_s ( szNewName, "ID_%03d_%03d", MID, SID );
	sItemNode.m_sItem.sBasicOp.strComment = szNewName;
	GLItemMan::GetInstance().InsertItem ( MID, SID, &sItemNode );
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( MID, SID );

    HTREEITEM hParentItem = m_ctrlItemTree.GetParentItem ( hMainItem );
	if ( hParentItem != m_TreeRoot )
	{
		UpdateItem ( pITEM, hParentItem );
	}
	else
	{
		UpdateItem ( pITEM, hMainItem );
	}	

	return TRUE;
}

BOOL CItemSuitTree::AddItemSub ()
{
	WORD MID = GLItemMan::GetInstance().FindFreeMID ();
	if ( MID == ITEMID_NOTFOUND )
	{
		return FALSE;
	}
	WORD SID = GLItemMan::GetInstance().FindFreeSID ( MID );
	if ( SID == ITEMID_NOTFOUND )
	{
		return FALSE;
	}
	
	CItemNode sItemNode;
	sItemNode = *m_pDummyItemNode;
	sItemNode.m_sItem.sBasicOp.sNativeID.wMainID = MID;
	sItemNode.m_sItem.sBasicOp.sNativeID.wSubID = SID;
	
	char szNewName[ITEM_SZNAME];
	sprintf_s ( szNewName, "IN_%03d_%03d", MID, SID );
	sItemNode.m_sItem.sBasicOp.strName = szNewName;
	sprintf_s ( szNewName, "ID_%03d_%03d", MID, SID );
	sItemNode.m_sItem.sBasicOp.strComment = szNewName;
	GLItemMan::GetInstance().InsertItem ( MID, SID, &sItemNode );
	SITEM *pITEM = GLItemMan::GetInstance().GetItem ( MID, SID );

	UpdateItem ( pITEM, NULL );

	return TRUE;
}

void CItemSuitTree::ForceSelectOnRightClick ( POINT pt )
{		
	m_ctrlItemTree.ScreenToClient( &pt );

	UINT uFlag = 0;
	PITEMNODE pItemNode = NULL;
	HTREEITEM hItem = m_ctrlItemTree.HitTest ( CPoint(pt), &uFlag );


    SetSelectItem ( NULL );	
	if ( hItem )
	{
		m_ctrlItemTree.SelectItem ( hItem );
		if ( uFlag & TVHT_ONITEMLABEL )
		{			
			SetSelectItem ( (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem ) );
		}
	}
}

void CItemSuitTree::SearchItemSelect( BYTE dwType, char *szItemName )
{
	const char *AllItemTypeName[] = 
	{
		"착용 무기 도구",
		"화살",

		"약품",
		"스킬 습득 서적",
		"귀환서",
		"인증서 ( 열쇠 )",
		"연마제",
		"부적",
		"승차권",
		"스킬 포인트 리셋",
		"스텟 포인트 리셋",
		"스킬, 스텟 포인트 리셋",

		"선물 상자",
		"세탁제",
		"확성기",
		"폭죽",
		"캐릭터 추가 카드",
		"인벤 확장 카드",
		"창고 확장 카드",
		"창고 연결 카드",
		"프리미엄 세트",
		"개인상점 개설 권리",
		"랜덤 아이탬",
		"코스툼 분리",
		"헤어스타일 변경",
		"얼굴 변경",
		"? 아이템",
		"CD",
		"친구에게",
		"클럽호출",
		"헤어샾 이용권",
		"이름변경 카드",
		"헤어스타일",
		"헤어컬러",
		"귀혼주",
		"펫 카드",
		"펫 먹이",
		"팻 이름변경",
		"팻 컬러변경",
		"팻 스타일변경",
		"팻 스킬",
		"SMS문자 발송",
		"팻 부활카드",
		"소방주(소멸방지)",
		"개조 기능",
		"종류갯수"
	};

	m_vecSearchItemList.clear();

	bool bFind = FALSE;
	bool bFindSection = FALSE;
	int  iFindCount = 0;
	int  searchMID = -1, searchSID = -1;
	int  itemType;
	string strItemName = szItemName;

	if( dwType == 0 )
	{
		string strTemp;

		int i, iSize = (int)strlen(szItemName);
		for( i = 0; i < iSize; i++ )
		{
			if( szItemName[i] == ',' )
			{
				searchMID = atoi(strTemp.c_str());
				strTemp.clear();
			}else{
				strTemp += szItemName[i];
			}
		}
		searchSID = atoi(strTemp.c_str());

	}
	if( dwType == 4 )
	{
		itemType = atoi(szItemName);
		strItemName = AllItemTypeName[itemType];
	}

	for ( int MID = 0; MID < GLItemMan::MAX_MID; MID++ )
	{
		for ( int SID = 0; SID < GLItemMan::MAX_SID; SID++ )
		{
			bFindSection = FALSE;

			SITEM* pItem = GLItemMan::GetInstance().GetItem ( MID, SID );
			HTREEITEM hItem = m_TreeItemList[MID][SID];
			m_ctrlItemTree.SetCheck( hItem, FALSE );

			if ( pItem )
			{
				switch( dwType )
				{
				case 0: // MID/SID
					{
						if( searchMID == MID && searchSID == SID )
						{
							bFindSection = TRUE;
						}
					}
					break;
				case 1: // 아이템 이름
					{
						if( _stricmp( pItem->GetName(), strItemName.c_str() ) == 0 ) { bFindSection = TRUE; }
					}
					break;
				case 2: // 사용 DDS
					{
						if( _stricmp( pItem->sBasicOp.strInventoryFile.c_str(), strItemName.c_str() ) == 0 ) { bFindSection = TRUE; }
					}
				    break;
				case 3: // 사용 CPS
					{
						int i;
//						for( i = 0; i < GLCI_NUM_NEWSEX; i++ )
						for( i = 0; i < GLCI_NUM_EX; i++ )
						{
							if( _stricmp( pItem->sBasicOp.strWearingFile[i].c_str(), strItemName.c_str() ) == 0 ) 
							{ 
								bFindSection = TRUE; 
								break;
							}
						}
					}
				    break;
				case 4: // 아이템 종류
					{
						if( pItem->sBasicOp.emItemType == itemType ) { bFindSection = TRUE; }
					}
					break;
				}

				if( bFindSection == TRUE )
				{
					m_ctrlItemTree.SetCheck( hItem, TRUE );
					bFind = TRUE;

					SSearchItemList inputSearchItem;
					inputSearchItem.MID = MID;
					inputSearchItem.SID = SID;
					strcpy_s( inputSearchItem.szItemName, pItem->GetName() );
					strcpy_s( inputSearchItem.szItemValue, strItemName.c_str() );
					m_vecSearchItemList.push_back( inputSearchItem );
					iFindCount++;
				}

			}
		}
	}
	if( bFind == FALSE ) 
	{
		MessageBox( "아이템을 찾지 못했습니다.", "알림" );
	}else{
		char szTempChar[256];
		sprintf_s( szTempChar, "%d개의 아이템을 찾았습니다.", iFindCount );
		MessageBox( szTempChar, "알림" );
	}
}

void CItemSuitTree::SetSelectIndex( int iSelectIndex )
{
	if( iSelectIndex == -1 ) 
	{
		MessageBox( "아이템을 선택하세요.", "알림" );
		return;
	}

	int MID, SID;
	MID = m_vecSearchItemList[iSelectIndex].MID;
	SID = m_vecSearchItemList[iSelectIndex].SID;
	HTREEITEM hItem = m_TreeItemList[MID][SID];


	SetSelectItem ( NULL );	
	m_ctrlItemTree.SelectItem ( hItem );
	SetSelectItem ( (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem ) );

	CWnd *pWnd = GetDlgItem(IDC_TREE_ITEM);
	pWnd->SetFocus();					
}