// CheckAdult.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "s_Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace SERVER_UTIL
{

// 18세 이상인지 체크한다.
// 입력 
// strCheck : 주민등록번호 앞자리 ex) 712212
// 출력
// 18 세 이상이면 true
// 18 세 이하이면 false
bool SERVER_UTIL::CheckAdult(CString strCheck, int nCheckAge)
{
	if (strCheck.GetLength() < 6)	
		return false;

	CTime tNow = CTime::GetCurrentTime(); // 현재시간

	int nYear, nMonth, nDay, nAge=0;

	// 2000 년 이후 출생자는 주민번호 첫번째 숫자가 0
	if (atoi(strCheck.Mid(0, 1).GetString()) == 0) 
	{	
		nYear  = 2000 + atoi(strCheck.Mid(0, 2).GetString());		
	}
	else // 2000 년 이전 출생자
	{
		nYear  = 1900 + atoi(strCheck.Mid(0, 2).GetString());
	}

	nAge = (int) tNow.GetYear() - nYear;

	if (nAge >= nCheckAge) // 체크하려는 나이 이상일때
	{
		return true;
	}
	else if (nAge == (nCheckAge-1)) // 체크하려는 나이보다 작지만 생일이 지났는지 체크
	{
		nMonth = atoi(strCheck.Mid(2, 2).GetString());
		if (nMonth == 0) 
			return false;
		nDay   = atoi(strCheck.Mid(4, 2).GetString());
		if (nDay == 0) 
			return false;

		if ((nMonth <= tNow.GetMonth()) && (nDay <= tNow.GetDay()))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else // 체크하려는 나이보다 작음
	{
		return false;
	}
}

/**
* int 형의 랜덤한 숫자를 발생시킨다.
* 숫자는 1 부터 nMax 까지 발생시킨다.
* 주의:0 은 발생시키지 않는다.
*/
int makeRandomNumber(int nMax)
{
	return (rand() % nMax + 1);
}

} // end of SERVER_UTIL namespace