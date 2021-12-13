#include "pch.h"

#include <string>
#include "resource.h"
#include "LoginServer.h"

#include "CheckOrder.h"

namespace order_ctrl
{
	std::string g_strWeekDay[CO_WEEKDAY] = { "Everyday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	int g_nWeekDay[CO_WEEKDAY] = { 0, 1, 2, 3, 4, 5, 6, 7 };

	std::string g_strTime[CO_TIME] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12",
									"13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23" };
	int g_nTime[CO_TIME] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };

	std::string g_strMinute[CO_MINUTE] = { "00", "10", "20", "30", "40", "50" };
	int g_nMinute[CO_MINUTE] = { 0, 10, 20, 30, 40, 50 };

	bool g_bCheckOrder = false;
	bool g_bCheckTime = false;
	int g_nOrderWeekDay = 0;
	int g_nOrderTime1 = 0;
	int g_nOrderMinute1 = 0;
	int g_nOrderTime2 = 0;
	int g_nOrderMinute2 = 0;
	int g_nLastCheckDay = 0;
	CTime g_cOrderStartTime;
	CTime g_cOrderEndTime;

	void init ( HWND hDlg )
	{
		HWND pWnd;

		pWnd = GetDlgItem ( hDlg, IDC_COMBO_WEEKDAY );
		if ( pWnd )
		{
			for ( int i=0; i<CO_WEEKDAY; ++i )
			{
				SendMessage ( pWnd, CB_ADDSTRING, 0, (LPARAM)g_strWeekDay[i].c_str() );
			}

			SendMessage ( pWnd, CB_SETCURSEL, 0, 0 );
		}

		pWnd = GetDlgItem ( hDlg, IDC_COMBO_TIME1 );
		if ( pWnd )
		{
			for ( int i=0; i<CO_TIME; ++i )
			{
				SendMessage ( pWnd, CB_ADDSTRING, 0, (LPARAM)g_strTime[i].c_str() );
			}
			SendMessage ( pWnd, CB_SETCURSEL, 0, 0 );
		}

		pWnd = GetDlgItem ( hDlg, IDC_COMBO_MINUTE1 );
		if ( pWnd )
		{
			for ( int i=0; i<CO_MINUTE; ++i )
			{
				SendMessage ( pWnd, CB_ADDSTRING, 0, (LPARAM)g_strMinute[i].c_str() );
			}
			SendMessage ( pWnd, CB_SETCURSEL, 0, 0 );
		}

		pWnd = GetDlgItem ( hDlg, IDC_COMBO_TIME2 );
		if ( pWnd )
		{
			for ( int i=0; i<CO_TIME; ++i )
			{
				SendMessage ( pWnd, CB_ADDSTRING, 0, (LPARAM)g_strTime[i].c_str() );
			}
			SendMessage ( pWnd, CB_SETCURSEL, 0, 0 );
		}

		pWnd = GetDlgItem ( hDlg, IDC_COMBO_MINUTE2 );
		if ( pWnd )
		{
			for ( int i=0; i<CO_MINUTE; ++i )
			{
				SendMessage ( pWnd, CB_ADDSTRING, 0, (LPARAM)g_strMinute[i].c_str() );
			}
			SendMessage ( pWnd, CB_SETCURSEL, 0, 0 );
		}	
	}


	void timer ( HWND hDlg )
	{
		if ( !g_bCheckOrder )	KillTimer ( hDlg, CO_TIMER );

		CTime cCUR = CTime::GetCurrentTime();

		if ( !g_bCheckTime )
		{
			int nCURDAY = cCUR.GetDay();
			int nCURWEEKDAY = cCUR.GetDayOfWeek ();
			int nCURHOUR = cCUR.GetHour();
			int nCURMINUTE = cCUR.GetMinute();
			
			if ( g_nLastCheckDay != nCURDAY )
			{
				if ( g_nOrderWeekDay==0 || nCURWEEKDAY==g_nOrderWeekDay )
				{
					if ( (nCURHOUR>g_nOrderTime1) || ( nCURHOUR==g_nOrderTime1) && nCURMINUTE>=g_nOrderMinute1 )
					{
						g_bCheckTime = true;

						//	시작시간 저장.
						g_nLastCheckDay = cCUR.GetDay();
						g_cOrderStartTime = CTime ( cCUR.GetYear(), cCUR.GetMonth(), cCUR.GetDay(), g_nOrderTime1, g_nOrderMinute1, 0 );

						//	종료시간 설정.
						int nDay = g_cOrderStartTime.GetDay();
						if ( g_nOrderTime2<g_nOrderTime1 ) nDay += 1;
						g_cOrderEndTime = CTime ( g_cOrderStartTime.GetYear(), g_cOrderStartTime.GetMonth(), nDay, g_nOrderTime2, g_nOrderMinute2, 0 );

						// 점검상태로 변환
						if ( g_pServer )	g_pServer->SetInspecting(TRUE);
						SetDlgItemText(hDlg, IDC_BTN_CHECK, _CHECK_END_TEXT );								
					}
				}
			}
		}
		else if ( g_bCheckTime )
		{
			if ( cCUR >= g_cOrderEndTime )
			{
				g_bCheckTime = false;

				char szTEXT[30];
				GetDlgItemText ( hDlg, IDC_BTN_CHECK, szTEXT, 30 );
				
				// 정상상태로 변환
				if ( g_pServer )	g_pServer->SetInspecting(FALSE);
				SetDlgItemText ( hDlg, IDC_BTN_CHECK, _CHECK_TEXT );
			}
		}
	}

	void command ( HWND hDlg )
	{
		g_bCheckOrder = !g_bCheckOrder;

		HWND pWnd = GetDlgItem ( hDlg, IDC_CHECK_CHECKORDER );
		if ( pWnd )
		{
			SendMessage ( pWnd, BM_SETCHECK, g_bCheckOrder?BST_CHECKED:BST_UNCHECKED, 0);
		}

		if ( g_bCheckOrder )
		{
			int nIndex(0);
			pWnd = GetDlgItem ( hDlg, IDC_COMBO_WEEKDAY );
			if ( pWnd )
			{
				nIndex = (int)::SendMessage(pWnd, CB_GETCURSEL, 0, 0);
				g_nOrderWeekDay = g_nWeekDay[nIndex];
			}

			pWnd = GetDlgItem ( hDlg, IDC_COMBO_TIME1 );
			if ( pWnd )
			{
				nIndex = (int)::SendMessage(pWnd, CB_GETCURSEL, 0, 0);
				g_nOrderTime1 = g_nTime[nIndex];
			}

			pWnd = GetDlgItem ( hDlg, IDC_COMBO_MINUTE1 );
			if ( pWnd )
			{
				nIndex = (int)::SendMessage(pWnd, CB_GETCURSEL, 0, 0);
				g_nOrderMinute1 = g_nMinute[nIndex];
			}

			pWnd = GetDlgItem ( hDlg, IDC_COMBO_TIME2 );
			if ( pWnd )
			{
				nIndex = (int)::SendMessage(pWnd, CB_GETCURSEL, 0, 0);
				g_nOrderTime2 = g_nTime[nIndex];
			}

			pWnd = GetDlgItem ( hDlg, IDC_COMBO_MINUTE2 );
			if ( pWnd )
			{
				nIndex = (int)::SendMessage(pWnd, CB_GETCURSEL, 0, 0);
				g_nOrderMinute2 = g_nMinute[nIndex];
			}

			SetTimer ( hDlg, CO_TIMER, 1000, NULL );
		}
		else
		{
			order_ctrl::reset();
			KillTimer ( hDlg, CO_TIMER );
		}
	}

	void reset ()
	{
		g_bCheckTime = false;
		g_nLastCheckDay = 0;
		g_cOrderStartTime = CTime();
		g_cOrderEndTime = CTime();

	}
};