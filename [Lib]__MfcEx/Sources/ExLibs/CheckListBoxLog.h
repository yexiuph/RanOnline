#pragma once
#include "afxwin.h"

class CCheckListBoxLog :
	public CCheckListBox
{
public:
	CCheckListBoxLog(void);
	~CCheckListBoxLog(void);

public:
	virtual	void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	DECLARE_MESSAGE_MAP()	
};
