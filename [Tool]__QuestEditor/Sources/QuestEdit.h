// QuestEdit.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'pch.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CQuestEditApp:
// �� Ŭ������ ������ ���ؼ��� QuestEdit.cpp�� �����Ͻʽÿ�.
//

class CQuestEditApp : public CWinApp
{
public:
	CQuestEditApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CQuestEditApp theApp;
