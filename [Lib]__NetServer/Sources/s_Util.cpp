// CheckAdult.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "pch.h"
#include "s_Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace SERVER_UTIL
{

// 18�� �̻����� üũ�Ѵ�.
// �Է� 
// strCheck : �ֹε�Ϲ�ȣ ���ڸ� ex) 712212
// ���
// 18 �� �̻��̸� true
// 18 �� �����̸� false
bool SERVER_UTIL::CheckAdult(CString strCheck, int nCheckAge)
{
	if (strCheck.GetLength() < 6)	
		return false;

	CTime tNow = CTime::GetCurrentTime(); // ����ð�

	int nYear, nMonth, nDay, nAge=0;

	// 2000 �� ���� ����ڴ� �ֹι�ȣ ù��° ���ڰ� 0
	if (atoi(strCheck.Mid(0, 1).GetString()) == 0) 
	{	
		nYear  = 2000 + atoi(strCheck.Mid(0, 2).GetString());		
	}
	else // 2000 �� ���� �����
	{
		nYear  = 1900 + atoi(strCheck.Mid(0, 2).GetString());
	}

	nAge = (int) tNow.GetYear() - nYear;

	if (nAge >= nCheckAge) // üũ�Ϸ��� ���� �̻��϶�
	{
		return true;
	}
	else if (nAge == (nCheckAge-1)) // üũ�Ϸ��� ���̺��� ������ ������ �������� üũ
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
	else // üũ�Ϸ��� ���̺��� ����
	{
		return false;
	}
}

/**
* int ���� ������ ���ڸ� �߻���Ų��.
* ���ڴ� 1 ���� nMax ���� �߻���Ų��.
* ����:0 �� �߻���Ű�� �ʴ´�.
*/
int makeRandomNumber(int nMax)
{
	return (rand() % nMax + 1);
}

} // end of SERVER_UTIL namespace