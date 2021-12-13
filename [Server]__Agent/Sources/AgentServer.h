#ifndef AGENT_SERVER_H_
#define AGENT_SERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "s_CAgentServer.h"

// value
HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;

HWND g_hEditBox = NULL;
HWND g_hWindow = NULL;
BOOL g_bConnect = FALSE;

CAgentServer*		g_pServer = NULL;

// function
BOOL CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);
void setFirstFocus(HWND hDlg);
void cmdStartOrStop(HWND hDlg);
void cmdPause(HWND hDlg);
void cmdExit(HWND hDlg);
void initDialog(HWND hDlg);
void executeCommand(HWND hDlg);
void executeCmdParameter(HWND hDlg);
void cmdShowWindow(HWND hDlg);
void cmdHideWindow(HWND hDlg);
void cmdPopupMenu(HWND hDlg);

#define WM_TRAY_NOTIFICATION WM_APP+1

#endif // AGENT_SERVER_H_