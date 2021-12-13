#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./TextureSetDXT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTextureSetDXT& CTextureSetDXT::GetInstance()
{
	static CTextureSetDXT Instance;
	return Instance;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										CTextureSetDXT
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD	CTextureSetDXT::VERSION = 0x0100;

CTextureSetDXT::CTextureSetDXT()
{
}

CTextureSetDXT::~CTextureSetDXT()
{
	m_mapTexDXT.clear();
}

void CTextureSetDXT::DeleteDeviceObjects()
{
	m_mapTexDXT.clear();
}

BOOL CTextureSetDXT::GetTextureNameDXT( DWORD i, LPCSTR& szName, int& nType )
{
	if( i >= m_mapTexDXT.size() )	return FALSE;	// ERROR

	MAPTEXTUREDXT_ITER iter = m_mapTexDXT.begin();
	for( DWORD loop=0; loop<i; ++loop )	++iter;
	
	szName = iter->first.c_str();
	nType = iter->second;

	return TRUE;
}

void CTextureSetDXT::SetTexList( const char* pName, int nType )	
{
	m_mapTexDXT.insert ( std::make_pair(pName,nType) );
}

BOOL CTextureSetDXT::DelTexture( DWORD i )
{
	if( i >= m_mapTexDXT.size() )	return FALSE;	// ERROR

	MAPTEXTUREDXT_ITER iter = m_mapTexDXT.begin();
	for( DWORD loop=0; loop<i; ++loop )	++iter;
	
	m_mapTexDXT.erase( iter );

	return TRUE;
}

BOOL CTextureSetDXT::FindTexture( const char* pName, int& nType )
{
	MAPTEXTUREDXT_ITER iter = m_mapTexDXT.find( pName );
	if( iter!=m_mapTexDXT.end() )
	{
		nType = (*iter).second;
		return TRUE;
	}
	return FALSE;
}

void CTextureSetDXT::SaveSet( CSerialFile& SFile )
{
	DWORD dwSize = (DWORD)m_mapTexDXT.size();

	SFile.BeginBlock();
	{
		SFile << dwSize;

		MAPTEXTUREDXT_ITER iter = m_mapTexDXT.begin();
		for( ; iter!=m_mapTexDXT.end(); ++iter )
		{
			SFile << (*iter).first;
			SFile << (*iter).second;
		}
	}
	SFile.EndBlock();
}

void CTextureSetDXT::LoadSet( CSerialFile& SFile )
{
	DWORD dwBuffSize;
	SFile >> dwBuffSize;

	m_mapTexDXT.clear();	// ÃÊ±âÈ­.

	if ( 0x0100==VERSION )
	{
		std::string strName;
		DWORD	dwSize = 0;
		int		nType = 0;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> nType;

			m_mapTexDXT.insert ( std::make_pair(strName.c_str(),nType) );
		}
		return;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
}
