// CTextureSetDXT.h: interface for the CCollision class.
//
//
//	Name : Sung-Hwan Han
//	Begin :2005/03/23
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <map>

#include "SerialFile.h"

class CTextureSetDXT
{
private:
	static const DWORD	VERSION;

private:
	typedef std::map<std::string,int>	MAPTEXTUREDXT;
	typedef MAPTEXTUREDXT::iterator		MAPTEXTUREDXT_ITER;

	MAPTEXTUREDXT	m_mapTexDXT;

public:
	void DeleteDeviceObjects();

public:
	int GetTexDXTSize()						{ return (int)m_mapTexDXT.size(); }
	BOOL GetTextureNameDXT( DWORD i, LPCSTR& szName, int& nType );
	void SetTexList( const char* pName, int nType );
	BOOL DelTexture( DWORD i );
	BOOL FindTexture( const char* pName, int& nType );

public:
	CTextureSetDXT();
	~CTextureSetDXT();

public:
	void SaveSet( CSerialFile& SFile );
	void LoadSet( CSerialFile& SFile );

public:
	static CTextureSetDXT& GetInstance();
};