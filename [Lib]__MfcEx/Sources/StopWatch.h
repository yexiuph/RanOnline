// By °æ´ë
//
#pragma once

#include <time.h>
#include <map>
#include <string>
#include <fstream>

class CStopWatch
{
private:
	typedef struct Clock_Node
	{
		int		count;
		clock_t	moment;
		clock_t	ticks;
		clock_t	last_time;
		bool	clock_on;
	} CLOCK_TYPE, *CLOCK_PTR;

	typedef std::map<std::string, CLOCK_PTR>	CLOCKMAP;
	typedef CLOCKMAP::iterator					CLOCKMAP_ITER;
	typedef CLOCKMAP::const_iterator			CLOCKMAP_CONST_ITER;

public:
	void start_clock(std::string name);
	void stop_clock(std::string name);
	void clock_moment_report(std::string name);
	void clock_report(void);

public:
	CStopWatch(void);
	~CStopWatch(void);

protected:
	CLOCKMAP		m_ClockMap;
	std::ofstream	oFile;

public:
	static CStopWatch& GetInstance();
};