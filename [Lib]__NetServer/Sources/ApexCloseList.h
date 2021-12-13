#pragma once
#include "s_CLock.h"
#include <list>

class ApexCloseList : public CLock
{
public:
	typedef	std::pair<DWORD,DWORD>		APEX_ID_TIME;
	typedef	std::list<APEX_ID_TIME>		APEX_CLOSE_LIST;
	typedef	APEX_CLOSE_LIST::iterator	APEX_CLOSE_LIST_ITER;

protected:
	APEX_CLOSE_LIST m_listApexClose;

public:
	ApexCloseList(void);
	~ApexCloseList(void);

	void clear()
	{
		LockOn();
		m_listApexClose.clear();
		LockOff();
	}

	void push_back( APEX_ID_TIME apexPair )
	{
		LockOn();
		m_listApexClose.push_back( apexPair );
		LockOff();
	}

	DWORD ApexCloseClientFinal()
	{
		LockOn();
		
		APEX_CLOSE_LIST_ITER iter = m_listApexClose.begin();
		APEX_CLOSE_LIST_ITER iter_end = m_listApexClose.end();

		for( ; iter != iter_end; )
		{
			APEX_ID_TIME & apexPair = *iter;
			if( apexPair.second < timeGetTime() )
			{
                DWORD dwClientID = apexPair.first;
				iter = m_listApexClose.erase( iter );
				LockOff();
				return dwClientID;
			}
			else
			{
				++iter;
			}
		}
		LockOff();

		return 0;
	}


};
