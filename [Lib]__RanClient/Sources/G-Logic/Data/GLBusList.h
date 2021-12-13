#pragma once

#include "./GLBusData.h"

class GLBusList : public GLBusData
{
public:
	GLBusList(void);
	virtual ~GLBusList(void);

protected:
	std::vector<SVILLAGE>	m_vecVILLAGE;

protected:
	void insert( DWORD dwSTATIONID, SSTATION &sSTATION );

public:
	virtual bool LOAD( std::string strFILE );
	
public:
	DWORD GetVillageNum();
	SVILLAGE* GetVillage( DWORD dwID );

	virtual SSTATION* GetStation ( DWORD dwID );
};
