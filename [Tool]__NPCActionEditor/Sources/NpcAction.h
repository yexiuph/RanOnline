// NpcAction.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'pch.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CNpcActionApp:
// �� Ŭ������ ������ ���ؼ��� NpcAction.cpp�� �����Ͻʽÿ�.
//

class CNpcActionApp : public CWinApp
{
public:
	CNpcActionApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CNpcActionApp theApp;
