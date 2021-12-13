#include "pch.h"
#include "./BugTrapStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace BUG_TRAP
{
	TCHAR szEMail[MAX_PATH]			= _T("randev2@mincoms.com");

#ifdef CH_PARAM 
	TCHAR szSupportURL[MAX_PATH]	= _T("http://lx.5u56.com");
	TCHAR szCountry[MAX_PATH]		= _T("_CH");
#elif TH_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://ran.excite.co.jp");
	TCHAR szCountry[MAX_PATH]		= _T("_TH");
#elif MY_PARAM 
	TCHAR szSupportURL[MAX_PATH]	= _T("http://www.myrosso.com");
	TCHAR szCountry[MAX_PATH]		= _T("_MY");
#elif MYE_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://www.myrosso.com");
	TCHAR szCountry[MAX_PATH]		= _T("_MYE");
#elif ID_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://www.ranonline.co.id");
	TCHAR szCountry[MAX_PATH]		= _T("_ID");
#elif PH_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://ranonline.e-games.com.ph");
	TCHAR szCountry[MAX_PATH]		= _T("_PH");
#elif VN_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://www.ranonline.com.vn");
	TCHAR szCountry[MAX_PATH]		= _T("_VN");
#elif JP_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://ranonline.jp");
	TCHAR szCountry[MAX_PATH]		= _T("_JP");
#elif TW_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://www.yong-online.com.tw");
	TCHAR szCountry[MAX_PATH]		= _T("_TW");
#elif HK_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://www.yong-online.com.hk");
	TCHAR szCountry[MAX_PATH]		= _T("_HK");
#elif KR_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://www.ran-online.co.kr");
	TCHAR szCountry[MAX_PATH]		= _T("_KR");
#elif KRT_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://www.ran-online.co.kr");
	TCHAR szCountry[MAX_PATH]		= _T("_KRT");
#elif GS_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://www.ran-world.com");
	TCHAR szCountry[MAX_PATH]		= _T("_GS");
#elif RZ_PARAM
	TCHAR szSupportURL[MAX_PATH]	= _T("http://forum.ragezone.com");
	TCHAR szCountry[MAX_PATH]		= _T("_RZ");
#else
	TCHAR szSupportURL[MAX_PATH]	= _T("http://www.ran-online.co.kr");
	TCHAR szCountry[MAX_PATH]		= _T("");
#endif

	TCHAR szServerAddr[MAX_PATH]	= _T("ranbugreport.mincoms.co.kr");
	SHORT nPort						= 8888;

};
