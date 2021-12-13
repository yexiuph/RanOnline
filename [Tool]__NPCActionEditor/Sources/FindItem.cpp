// FindItem.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "NpcAction.h"
#include "FindItem.h"
#include ".\finditem.h"


// CFindItem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFindItem, CDialog)
CFindItem::CFindItem(CWnd* pParent /*=NULL*/)
	: CDialog(CFindItem::IDD, pParent)
	, m_nItemMID(0)
	, m_nItemSID(0)
{
}

CFindItem::~CFindItem()
{
}

void CFindItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nItemMID);
	DDV_MinMaxInt(pDX, m_nItemMID, 0, 1024);
	DDX_Text(pDX, IDC_EDIT2, m_nItemSID);
	DDV_MinMaxInt(pDX, m_nItemSID, 0, 1024);
}


BEGIN_MESSAGE_MAP(CFindItem, CDialog)
END_MESSAGE_MAP()



