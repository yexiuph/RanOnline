// LevelEditor.h : main header file for the LevelEditor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'pch.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CLevelEditorApp:
// See LevelEditor.cpp for the implementation of this class
//

class CLevelEditorApp : public CWinApp
{
public:
	CLevelEditorApp();

public:
	BOOL		m_bIsActive;
	BOOL		m_bRedrawScreen;

public:
	void  SetActive(BOOL bActive);

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int Run();
};

extern CLevelEditorApp theApp;