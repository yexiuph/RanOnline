// Emulator.h : Emulator ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'pch.h' before including this file for PCH
#endif

#include "resource.h"       // �� ��ȣ


// CEmulatorApp:
// �� Ŭ������ ������ ���ؼ��� Emulator.cpp�� �����Ͻʽÿ�.
//

class CEmulatorApp : public CWinApp
{
public:
	void  SetActive(BOOL bActive) { m_bIsActive = bActive; };

public:
	BOOL  m_bIsActive;

	CString	m_strAppPath;
	char	m_szAppPath[MAX_PATH];

public:
	CEmulatorApp();


// ������
public:
	virtual BOOL InitInstance();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int Run();
};

extern CEmulatorApp theApp;
