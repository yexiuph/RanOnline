#pragma once

#include "./GLBusData.h"

class GLBusStation : public GLBusData
{
protected:
	std::string				m_strPATH;

protected:
	void insert( SSTATION &sSTATION );

public:
	void SetPath ( std::string strPATH )	{ m_strPATH = strPATH; }
	std::string GetPath ()					{ return m_strPATH.c_str(); }

public:
	virtual bool LOAD ( std::string strFILE );

	virtual SSTATION* GetStation ( DWORD dwID );

protected:
	GLBusStation(void);

public:
	virtual ~GLBusStation(void);

public:
	static GLBusStation& GetInstance ();
};
