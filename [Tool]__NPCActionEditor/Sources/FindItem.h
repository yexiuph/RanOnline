#pragma once


// CFindItem ��ȭ �����Դϴ�.

class CFindItem : public CDialog
{
	DECLARE_DYNAMIC(CFindItem)

public:
	CFindItem(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CFindItem();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FINDITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int m_nItemMID;
	int m_nItemSID;
};
