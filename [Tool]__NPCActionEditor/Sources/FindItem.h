#pragma once


// CFindItem 대화 상자입니다.

class CFindItem : public CDialog
{
	DECLARE_DYNAMIC(CFindItem)

public:
	CFindItem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFindItem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FINDITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nItemMID;
	int m_nItemSID;
};
