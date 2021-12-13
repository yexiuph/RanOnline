// FieldServer.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "pch.h"
#include "resource.h"
#include "AgentServer.h"
#include "s_NetGlobal.h"
#include "gassert.h"
#include "GLogicData.h"
#include "GLogic.h"
#include "ServerControllerMsgDefine.h"
#include "../[Lib]__MfcEx/Sources/MinBugTrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_LOADSTRING 100

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	BUG_TRAP::BugTrapInstall( std::string(_T("AgentServer")));
	g_hInst = hInstance;
	g_pServer = NULL;
	SERVER_CONTROLLER::checkCmdParameter(lpCmdLine);

	// Note : 순수 가상함수 호출 감지 핸들러.
//	_set_purecall_handler(smtm_PurecallHandler);

	GLOGIC::bGLOGIC_PACKFILE = FALSE;
	GLOGIC::bGLOGIC_ZIPFILE = TRUE;
	GLOGIC::bENGLIB_ZIPFILE = TRUE;

	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), HWND_DESKTOP, MainDlgProc);
	return 0;
}

void executeCmdParameter(HWND hDlg)
{
	if (SERVER_CONTROLLER::bStart) 
	{
		cmdStartOrStop(hDlg);
	}
	// if (bStop)  MessageBox(hDlg, _T("Stop"), _T("Stop"), MB_OK);
	// if (bExit)  MessageBox(hDlg, _T("Exit"), _T("Exit"), MB_OK);
	// if (bRestart) MessageBox(hDlg, _T("Restart"), _T("Restart"), MB_OK);
}

void setFirstFocus(HWND hDlg)
{
	HWND pWnd = GetDlgItem(hDlg, IDC_BTN_EXECUTE);
	SetFocus(pWnd);
}

void initDialog(HWND hDlg)
{
	HWND pWnd = NULL;
	pWnd = GetDlgItem(hDlg, IDC_BTN_STOP);
	EnableWindow(pWnd, TRUE);

	pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
	EnableWindow(pWnd, FALSE);

	setFirstFocus(hDlg);

	::SetTimer(hDlg, 
		SERVER_CONTROLLER_TIMER_ID,
		SERVER_CONTROLLER_TIMER_MILLISECOND,
		NULL);
}

void executeCommand(HWND hDlg)
{
	if (g_pServer != NULL)
	{
		char cExecText[100] = {0};
		GetDlgItemText(hDlg, IDC_EDIT_EXEC, cExecText, 100);
		g_pServer->ExecuteCommand(cExecText);
	}
}

void cmdExit(HWND hDlg)
{
	if (g_pServer != NULL)
	{
		g_pServer->Stop();
		delete g_pServer;
		g_pServer = NULL;
	}
}

void cmdPause(HWND hDlg)
{
	if (g_pServer != NULL)
	{
		if (g_pServer->GetStatus() == S_SERVER_RUNING) // 가동중이면
		{
            // 중단시킨다.
			g_pServer->Pause();
			SetDlgItemText(hDlg, IDC_BTN_PAUSE, "Resume");
			
			HWND pWnd = GetDlgItem(hDlg, IDC_BTN_STOP);
			EnableWindow(pWnd, FALSE);
		}
		else if (g_pServer->GetStatus() == S_SERVER_PAUSE) // 중단중이면
		{
			// 가동시킨다.
			g_pServer->Resume();
			SetDlgItemText(hDlg, IDC_BTN_PAUSE, "Pause");

			HWND pWnd = GetDlgItem(hDlg, IDC_BTN_STOP);
			EnableWindow(pWnd, TRUE);
		}
	}
}

void cmdStartOrStop(HWND hDlg)
{
	int nRetCode = 0;
	// Server is running
	if (g_pServer != NULL && g_pServer->IsRunning())
	{
		nRetCode = g_pServer->Stop();
		if ( nRetCode == NET_OK )
			SetDlgItemText(hDlg, IDC_BTN_STOP, "Start");
		delete g_pServer;
		g_pServer = NULL;
		HWND pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
		EnableWindow(pWnd, FALSE);
	}	
	else // Server is not running
	{
		g_pServer = new CAgentServer(hDlg, GetDlgItem(hDlg, IDC_EDITCONSOLE), GetDlgItem(hDlg, IDC_EDITCONSOLE_INFO));
		nRetCode = g_pServer->Start();
		if ( nRetCode == NET_OK )
		{
			SetDlgItemText(hDlg, IDC_BTN_STOP, "Stop");
			HWND pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
			EnableWindow(pWnd, TRUE);
		}
		else
		{
			g_pServer->Stop();
			delete g_pServer;
			g_pServer = NULL;
			HWND pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
			EnableWindow(pWnd, FALSE);
		}
	}
}

void cmdShowWindow(HWND hDlg)
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = hDlg;
	Shell_NotifyIcon(NIM_DELETE, &nid);
	ShowWindow(hDlg, SW_SHOW);
    
	return;
}

void cmdHideWindow(HWND hDlg)
{
	NOTIFYICONDATA nid;

	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = hDlg;

	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_AGENTSERVER));
	lstrcpy(nid.szTip, "AgentServer");
	nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
	Shell_NotifyIcon(NIM_ADD, &nid);
	ShowWindow(hDlg, SW_HIDE);

	return;
}

void cmdPopupMenu(HWND hDlg)
{
	HMENU hMenu, hPopupMenu;
	POINT pt;

	hMenu = LoadMenu(g_hInst,MAKEINTRESOURCE(IDR_POPMENU));
	hPopupMenu = GetSubMenu(hMenu,0);
	GetCursorPos(&pt);
	SetForegroundWindow(hDlg);
	TrackPopupMenu(hPopupMenu,TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
					pt.x, pt.y, 0, hDlg, NULL);
	SetForegroundWindow(hDlg);
	DestroyMenu(hPopupMenu);
	DestroyMenu(hMenu);

	return;
}


BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	int nRetCode = 0;
	g_hWindow = hDlg;
	

	switch (iMessage)
	{
	case WM_INITDIALOG :
		initDialog(hDlg);
		break;

	case RAN_MSG_START:
		cmdStartOrStop(hDlg);
		break;

	case RAN_MSG_STOP:
		cmdStartOrStop(hDlg);
		break;

	case RAN_MSG_EXIT:
		cmdExit(hDlg);
		EndDialog(hDlg, 0);
		break;

	/*
	case RAN_MSG_RESTART:
		cmdStartOrStop(hDlg);
		cmdStartOrStop(hDlg);
		break;
	*/

	case WM_TIMER :
		if (SERVER_CONTROLLER::bInit == false)
		{
			if (wParam == SERVER_CONTROLLER_TIMER_ID)
			{
				::KillTimer(hDlg, SERVER_CONTROLLER_TIMER_ID);
				executeCmdParameter(hDlg);
			}
			SERVER_CONTROLLER::bInit = true;
		}
		break;
	case WM_INITMENUPOPUP:
		if (g_pServer != NULL && g_pServer->IsRunning()){
			ModifyMenu(HMENU(wParam),IDC_BTN_STOP,MF_BYCOMMAND | MF_STRING, IDC_BTN_STOP, "Stop");
			ModifyMenu(HMENU(wParam),IDC_BTN_PAUSE,MF_BYCOMMAND | MF_ENABLED, IDC_BTN_PAUSE, "Pause");
		}else{
			ModifyMenu(HMENU(wParam),IDC_BTN_STOP,MF_BYCOMMAND | MF_STRING, IDC_BTN_STOP, "Start");
			ModifyMenu(HMENU(wParam),IDC_BTN_PAUSE,MF_BYCOMMAND | MF_GRAYED, IDC_BTN_PAUSE, "Pause");
		}
		break;
	case WM_SYSCOMMAND:
		switch(LOWORD(wParam))
		{
		case SC_CLOSE:
			cmdHideWindow(hDlg);
			return TRUE;
		}
		break;
	case WM_TRAY_NOTIFICATION:
		switch(lParam)
		{
		case WM_LBUTTONUP:
			cmdShowWindow(hDlg);
			return TRUE;
		case WM_RBUTTONUP:
			cmdPopupMenu(hDlg);
			return TRUE;
		}
		return FALSE;
	case WM_COMMAND :
		switch (LOWORD(wParam))
		{		
		case IDC_BTN_EXECUTE: // 명령줄 해석
			executeCommand(hDlg);
			return TRUE;

		case IDC_BTN_STOP:			
			cmdStartOrStop(hDlg);
			setFirstFocus(hDlg);
			return TRUE;
			
		case IDC_BTN_PAUSE:
			cmdPause(hDlg);
			setFirstFocus(hDlg);
			return TRUE;
			
		case IDC_CLOSE:
			if( MessageBox(hDlg,"Do you really want to exit?", "Warning", MB_YESNO | MB_ICONWARNING ) == IDYES){
				cmdExit(hDlg);
				EndDialog(hDlg,0);
			}
			return TRUE;

		case IDC_OPEN:
			cmdShowWindow(hDlg);
			return TRUE;

		// Session 서버와 연결이 끊어졌을때 다시 연결하기 위해서 추가함.
		case IDC_BTN_CONNECT_SESSION:
			if ( (g_pServer != NULL) &&
				 (g_pServer->IsRunning()) )
			{
				// g_pServer->SessionConnectSndSvrInfo( true );
				g_pServer->SessionConnectSndSvrInfo();
			}
			return TRUE;

		case IDC_BTN_CONNECT_FIELD:
			if ( (g_pServer != NULL) &&
				 (g_pServer->IsRunning()) )
			{
				// g_pServer->FieldConnectAll( true );
				g_pServer->FieldConnectAll();
			}
			return TRUE;

		case IDC_EDITCONSOLE :
			switch (HIWORD(wParam))
			{			
			case EN_MAXTEXT:			
				SetWindowText((HWND)lParam, "");
				break;
			}
			return TRUE;
			
        case IDC_EDITCONSOLE_INFO :
            switch (HIWORD(wParam))
			{			
			case EN_MAXTEXT:			
				SetWindowText((HWND)lParam, "");
				break;
			}
			return TRUE;			
		}
		return FALSE;
	}
	return FALSE;
}
