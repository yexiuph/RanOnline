#pragma once

#include <vector>

class GLSchoolFreePK
{
protected:
	bool	m_bON;

	DWORD	m_dwLAST_DAY;
	float	m_fTIMER;

protected:
	bool DoProgress ( float fElaps );
	bool CheckBegin ();

	bool DoBegin ();
	bool DoEnd ();

public:
	bool IsON ();
	void SET ( bool bON )	{ m_bON = bON; }

public:
	void FrameMove ( float fElaps );

protected:
	GLSchoolFreePK();

public:
	~GLSchoolFreePK();

public:
	static GLSchoolFreePK& GetInstance ();
};

