// ItemEdit.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// �� ��ȣ

struct SSearchItemList
{
	int MID;
	int SID;
	char szItemName[MAX_PATH];
	char szItemValue[MAX_PATH];

	SSearchItemList()
	{
		ZeroMemory( szItemName, MAX_PATH );
		ZeroMemory( szItemValue, MAX_PATH );
	}

};


// CItemEditApp:
// �� Ŭ������ ������ ���ؼ��� ItemEdit.cpp�� �����Ͻʽÿ�.
//

class CItemEditApp : public CWinApp
{
public:
	CItemEditApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CItemEditApp theApp;
