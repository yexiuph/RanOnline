#include "pch.h"
#include "s_CRandomNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace SERVER_UTIL;

CRandomNumber::CRandomNumber(void)
{
}

CRandomNumber::~CRandomNumber(void)
{
}

// Generate Random String
bool CRandomNumber::GenerateRandomString(CString& str, int Length, int Small, int Capital, int Numbers)
{
	CString SmallCharacters = "abcdefghijklmnopqrstuvwxyz";
	CString CapitalCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	CString NumbersSet = "0123456789";
	
	int Number = 26;
	int Number2 = 10;
	int Choice = 4;
	int Flag = 0;
	int i = 0;

	if ((Small) || (Capital) || (Numbers))
	{		
		for (i=1;i<=Length; ++i)
		{
			while ((Choice>=4) || (Choice==0))
			{
				Flag = 0;
				while (Flag==0)
				{
					Choice = int (rand()/1000);
					if (Choice==Small)
					{
						Flag=1;
						break;
					}
					if (Choice==Capital)
					{
						Flag=1;
						break;
					}
					if (Choice==Numbers)
					{
						Flag=1;
						break;
					}
				}
			}
			switch (Choice)
			{
			case 1:
				{
					while (Number>=26) 
						Number  = int(rand()/1000);
					str.Insert(0,SmallCharacters.GetAt(Number));
					Number=26;
					Choice=4;
					break;
				}
			case 2:
				{
					while (Number>=26) 
						Number  = int(rand()/1000);
					str.Insert(0,CapitalCharacters.GetAt(Number));
					Number=26;
					Choice=4;
					break;
				}
			case 3:
				{
					while (Number>=10) 
						Number  = int(rand()/1000);
					str.Insert(0,NumbersSet.GetAt(Number));
					Number=26;
					Choice=4;
				}
			}
		}
		
	}
	else // exception
	{
		return false;
	}
	return true;
}
