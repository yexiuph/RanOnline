#pragma once

#include "./GLAutoLevel.h"

class GLAutoLevelMan
{
public:
	typedef std::list<GLAutoLevel*>		AUTOLEVEL_LIST;
	typedef AUTOLEVEL_LIST::iterator	AUTOLEVEL_LIST_ITER;

protected:
	AUTOLEVEL_LIST	m_listAutoLevel;
	STRING_LIST		m_listFileName;
	STRING_LIST		m_listCommand;

public:
	~GLAutoLevelMan();

public:
	bool LOAD( std::string & strFileName );
	bool FrameMove();

protected:
	GLAutoLevelMan() {}

public:
	static GLAutoLevelMan& GetInstance();
};