#pragma once

namespace order_ctrl
{
	#define _CHECK_TEXT			("Check")
	#define _CHECK_END_TEXT		("Check End")

	enum { CO_WEEKDAY = 8, CO_TIME = 24, CO_MINUTE = 6, CO_TIMER = 1000 };

	void init ( HWND hDlg );
	void timer ( HWND hDlg );
	void command ( HWND hDlg );
	void reset ();
};