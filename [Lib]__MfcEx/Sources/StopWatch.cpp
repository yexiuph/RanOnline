#include "pch.h"
#include "stopwatch.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CStopWatch& CStopWatch::GetInstance()
{
	static CStopWatch Instance;
	return Instance;
}

CStopWatch::CStopWatch(void)
{
	oFile.open("StopWatchReport.txt");

	if ( !oFile )
	{
		std::cerr << "������ ���� ����!" << std::endl;
		exit ( EXIT_FAILURE );
	}

	oFile << "[�� �¶��� - Ŭ���̾�Ʈ ������ �ð� �� �����κ� �м� ����]" << std::endl;
	oFile << "# ***	: 2�� �̻�" << std::endl;
	oFile << "# **	: 0.5�� �̻�" << std::endl;
	oFile << "# *	: 0.1�� �̻�" << std::endl;
}

CStopWatch::~CStopWatch(void)
{
	clock_report();
	oFile.close();
}

void CStopWatch::start_clock(std::string name)
{
	CLOCKMAP_ITER iter = m_ClockMap.find( name );
	if ( iter == m_ClockMap.end() )
	{
		CLOCK_PTR clocktype = new Clock_Node;
		clocktype->count = 0;
		clocktype->moment = 0;
		clocktype->ticks = 0;
		clocktype->clock_on = true;
		clocktype->last_time = clock();

		m_ClockMap.insert( std::make_pair(name, clocktype) );

		return ;
	}
	else if ( iter->second->clock_on )
	{
		std::cerr << "Error : Clock '" << name << "' already on" << std::endl;
		return ;
	}
	
	iter->second->clock_on = true;
	iter->second->last_time = clock();
}

void CStopWatch::stop_clock(std::string name)
{
	clock_t ticks = clock();

	CLOCKMAP_ITER iter = m_ClockMap.find( name );
	if ( iter == m_ClockMap.end() )
	{
		std::cerr << "Error : Clock '" << name << "' not found" << std::endl;
		return ;
	}
	else if ( !iter->second->clock_on )
	{
		std::cerr << "Error : Clock '" << name << "' not started" << std::endl;
		return ;
	}

	iter->second->clock_on = false;
	iter->second->moment = ticks - iter->second->last_time;
	iter->second->ticks += iter->second->moment;
	if( iter->second->moment != 0.0 ) ++(iter->second->count);
}

void CStopWatch::clock_moment_report(std::string name)
{
	clock_t total = clock();

	CLOCKMAP_ITER iter = m_ClockMap.find( name );
	if ( iter == m_ClockMap.end() )
	{
		std::cerr << "Error : Clock '" << name << "' not found" << std::endl;
		return ;
	}
	else if ( iter->second->clock_on )
	{
		std::cerr << "Error : Clock '" << iter->first << "' not stopped" << std::endl;
		return ;
	}

	double moment = iter->second->moment / static_cast<double>(CLOCKS_PER_SEC);

	if ( moment >= 2.0 ) oFile << "[***] ";
	else if ( moment >= 0.5 ) oFile << "[**] ";
	else if ( moment >= 0.1 ) oFile << "[*] ";
	else if ( moment == 0.0 ) return ;

	oFile << "[Routine] : '" << iter->first << "', [�ð�] : " << moment << "��, [Percent] : "
		<< iter->second->moment / static_cast<double>(total) * 100.0 << "%"  << std::endl;
}

void CStopWatch::clock_report(void)
{
	clock_t total = clock();

	oFile << "[               ��   ��               ]" << std::endl;

	CLOCKMAP_ITER iter = m_ClockMap.begin();
	CLOCKMAP_ITER end = m_ClockMap.end();

	for( ; iter != end ; ++iter )
	{
		if ( iter->second->clock_on )
			std::cerr << "Error : Clock '" << iter->first << "' not stopped" << std::endl;
		
		oFile << "[Routine] : '" << iter->first << "', [��սð�] : "
			<< iter->second->ticks / static_cast<double>(CLOCKS_PER_SEC) / iter->second->count<< "��, [�����ð�] : "
			<< iter->second->ticks / static_cast<double>(CLOCKS_PER_SEC) << "��, [Percent] : "
			<< iter->second->ticks / static_cast<double>(total) * 100.0 << "%"  << std::endl;
		
		delete iter->second;
	}
}