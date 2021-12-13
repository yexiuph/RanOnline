// GameServer.cpp : Defines the entry point for the application.
//
#include "pch.h"
#include "resource.h"
#include "s_NetGlobal.h"
#include "CheckOrder.h"
#include "LoginServer.h"
#include "ServerControllerMsgDefine.h"
#include "../[Lib]__MfcEx/Sources/MinBugTrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_LOADSTRING 100
#define CHECK_MAINTENANCE_TIMER_ID 2

HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;

HWND g_hEditBox = NULL;
HWND g_hWindow = NULL;
bool g_bConnect = false;

CLoginServer* g_pServer = NULL;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	BUG_TRAP::BugTrapInstall(std::string(_T("LoginServer")));

	g_hInst=hInstance;
	g_pServer = NULL;
	SERVER_CONTROLLER::checkCmdParameter(lpCmdLine);

	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), HWND_DESKTOP, MainDlgProc);
	return 0;
}

void setFirstFocus(HWND hDlg)
{
	HWND pWnd = GetDlgItem(hDlg, IDC_BTN_EXECUTE);
	SetFocus(pWnd);
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

void cmdStartOrStop(HWND hDlg)
{
	int nRetCode = 0;
	if (g_pServer != NULL && g_pServer->IsRunning())
	{					
		nRetCode = g_pServer->Stop();
		if ( nRetCode == NET_OK )
			SetDlgItemText(hDlg, IDC_BTN_STOP, "Start");
		delete g_pServer;
		g_pServer = NULL;
		HWND pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
		EnableWindow(pWnd, FALSE);

		pWnd = GetDlgItem(hDlg, IDC_CHECK_MAINTENANCE);
		EnableWindow( pWnd, FALSE );
	}			
	else // Server is not running
	{				
		g_pServer = new CLoginServer(hDlg, GetDlgItem(hDlg, IDC_EDITCONSOLE), GetDlgItem(hDlg, IDC_EDITCONSOLE_INFO));
		nRetCode = g_pServer->Start();

		if ( nRetCode == NET_OK )
		{
			SetDlgItemText(hDlg, IDC_BTN_STOP, "Stop");
			HWND pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
			EnableWindow(pWnd, TRUE);					
			
			CString strTemp;
			
			strTemp.Format(_T("%d"), g_pServer->GetGameVersion());
			SetDlgItemText(hDlg, IDC_EDIT_GAMEVERSION, strTemp);
			
			strTemp.Format(_T("%d"), g_pServer->GetLauncherVersion());
			SetDlgItemText(hDlg, IDC_EDIT_PATCHVERSION, strTemp);

			pWnd = GetDlgItem(hDlg, IDC_CHECK_MAINTENANCE);
			EnableWindow( pWnd, TRUE );
		}
		else 
		{
			g_pServer->Stop();
			delete g_pServer;
			g_pServer = NULL;
			HWND pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
			EnableWindow(pWnd, FALSE);

			pWnd = GetDlgItem(hDlg, IDC_CHECK_MAINTENANCE);
			EnableWindow( pWnd, FALSE );
		}				
	}
}

void initDialog(HWND hDlg)
{
	HWND pWnd = GetDlgItem(hDlg, IDC_BTN_STOP);
	EnableWindow(pWnd, TRUE);

	pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
	EnableWindow(pWnd, FALSE);
	
	setFirstFocus(hDlg);
    
	//	check order control
	order_ctrl::init(hDlg);

	::SetTimer(hDlg, 
		SERVER_CONTROLLER_TIMER_ID,
		SERVER_CONTROLLER_TIMER_MILLISECOND,
		NULL);

	::SetTimer(hDlg,
		CHECK_MAINTENANCE_TIMER_ID,
		1000,
		NULL );
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

void cmdExit(HWND hDlg)
{
	if (g_pServer != NULL)
	{
		g_pServer->Stop();
		delete g_pServer;
		g_pServer = NULL;
	}
}

void cmdCheck(HWND hDlg)
{
	if (g_pServer != NULL)
	{
		if (g_pServer->GetInspection()) // 점검중이면
		{
			g_pServer->SetInspecting(FALSE); // 정상상태로 변환
			SetDlgItemText(hDlg, IDC_BTN_CHECK, _CHECK_TEXT );
		}
		else // 정상이면
		{
			g_pServer->SetInspecting(TRUE); // 점검상태로 변환
			SetDlgItemText(hDlg, IDC_BTN_CHECK, _CHECK_END_TEXT );
		}
	}
}

void cmdVersion(HWND hDlg)
{
	TCHAR szGameVersion[10] = {0};
	TCHAR szPatchVersion[10] = {0};

	if (g_pServer != NULL)
	{
		GetDlgItemText(hDlg, IDC_EDIT_GAMEVERSION,  szGameVersion,  10);
		GetDlgItemText(hDlg, IDC_EDIT_PATCHVERSION, szPatchVersion, 10);
		g_pServer->SetVersion(atoi(szGameVersion), atoi(szPatchVersion));
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
}

void cmdHideWindow(HWND hDlg)
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = hDlg;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_NET_LOGINSERVER));
	lstrcpy(nid.szTip, "LoginServer");
	nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
	Shell_NotifyIcon(NIM_ADD, &nid);
	ShowWindow(hDlg, SW_HIDE);
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
}

BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{	
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

	case WM_TIMER:
		{
			switch ( wParam )
			{
			case order_ctrl::CO_TIMER:
				//	check order control
				order_ctrl::timer(hDlg);
				break;
			
			case CHECK_MAINTENANCE_TIMER_ID:
				if ( g_pServer && g_pServer->IsRunning() )
				{
					bool bOK = g_pServer->GetAllowIP();

					if( bOK )
					{
						::SendMessage(GetDlgItem(hDlg,IDC_CHECK_MAINTENANCE), BM_SETCHECK, true, 0);
					}
					else
					{
						::SendMessage(GetDlgItem(hDlg,IDC_CHECK_MAINTENANCE), BM_SETCHECK, false, 0);
					}
				}
				break;
			};

			if (SERVER_CONTROLLER::bInit == false)
			{
				if (wParam == SERVER_CONTROLLER_TIMER_ID)
				{
					::KillTimer(hDlg, SERVER_CONTROLLER_TIMER_ID);
					executeCmdParameter(hDlg);
				}
				SERVER_CONTROLLER::bInit = true;
			}
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
		case IDC_BTN_EXECUTE : // 명령줄 해석
			executeCommand(hDlg);			
			return TRUE;
		
		case IDC_BTN_STOP :
			cmdStartOrStop(hDlg);
			setFirstFocus(hDlg);
			return TRUE;

		case IDC_BTN_PAUSE :
			cmdPause(hDlg);
			setFirstFocus(hDlg);
			return TRUE;			

		case IDC_BTN_CHECK : // 점검중
			cmdCheck(hDlg);			
			return TRUE;

		case IDC_BTN_SETVERSION : // 버전 세팅
			cmdVersion(hDlg);
			return TRUE;		

		case IDC_CLOSE :
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
			if (g_pServer != NULL && g_pServer->IsRunning())
			{
				// g_pServer->SessionConnectSndSvrInfo( true );
				g_pServer->SessionConnectSndSvrInfo();
			}
			return TRUE;

		case IDC_EDITCONSOLE :
			switch (HIWORD(wParam))
			{			
			case EN_MAXTEXT:			
				SetWindowText((HWND) lParam, _T(""));
				break;
			}
			return TRUE;
			break;

        case IDC_EDITCONSOLE_INFO :
            switch (HIWORD(wParam))
			{			
			case EN_MAXTEXT:			
				SetWindowText((HWND) lParam, _T(""));
				break;
			}
			return TRUE;
			break;

		case IDC_CHECK_CHECKORDER:
			{
				//	check order control
				order_ctrl::command(hDlg);
			}
			break;

		case IDC_CHECK_MAINTENANCE:
			{
				if ( g_pServer && g_pServer->IsRunning() )
				{
					bool bOK = g_pServer->GetAllowIP() ? false : true;
					g_pServer->SetAllowIP( bOK );
				}
			}
		}
		return FALSE;
	}
	return FALSE;
}