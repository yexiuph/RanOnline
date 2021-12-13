#include "pch.h"
#include "./GLQuestPlay.h"
#include "./GLQuest.h"
#include "./GLQuestMan.h"

//#include "ByteStream.h"
//#include "StlFunctions.h"
//#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLQuestPlay::GLQuestPlay ()
{
}

GLQuestPlay::~GLQuestPlay ()
{
	DeleteAll ();
}

bool GLQuestPlay::GetReqREADING ( DWORD &dwNID )
{
	MAPQUEST_ITER iter = m_mapQuestProc.begin();
	MAPQUEST_ITER iter_end = m_mapQuestProc.end();
	for ( ; iter!=iter_end; ++iter )
	{
		GLQUESTPROG* pPROG = (*iter).second;

		if ( pPROG->IsReqREADING() )
		{
			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
			if ( pQUEST )
			{
				dwNID = pPROG->m_sNID.dwID;
				return true;
			}
		}
	}

	return false;
}

void GLQuestPlay::ResetReqREADING ( DWORD dwNID )
{
	GLQUESTPROG *pPROG = FindProc ( dwNID );
	if ( pPROG )
	{
		pPROG->ResetReqREADING();
	}

	return;
}

GLQUESTPROG* GLQuestPlay::FindProc ( DWORD dwQuestID )
{
	MAPQUEST_ITER iter = m_mapQuestProc.find ( dwQuestID );
	if ( iter!=m_mapQuestProc.end() )	
	{
		return (*iter).second;
	}
	else
	{
		return NULL;
	}
}

GLQUESTPROG* GLQuestPlay::FindEnd ( DWORD dwQuestID )
{
	MAPQUEST_ITER iter = m_mapQuestEnd.find ( dwQuestID );
	if ( iter!=m_mapQuestEnd.end() )
	{
		return (*iter).second;
	}
	else
	{
		return NULL;
	}
}
	
bool GLQuestPlay::InsertProc ( const GLQUESTPROG &sQuestProg )
{
	GLQUESTPROG* pQuestProg = FindProc ( sQuestProg.m_sNID.dwID );
	if ( pQuestProg )
	{
		*pQuestProg = sQuestProg;
		return false;
	}
	else
	{
		pQuestProg = new GLQUESTPROG;
		*pQuestProg = sQuestProg;

		std::pair< MAPQUEST_ITER, bool > Success;
		Success = m_mapQuestProc.insert ( std::make_pair(sQuestProg.m_sNID.dwID,pQuestProg) );
		return Success.second;
	}
}

bool GLQuestPlay::DeleteProc ( DWORD dwQuestID )
{	
	MAPQUEST_ITER iter = m_mapQuestProc.find ( dwQuestID );
	
	if ( iter!=m_mapQuestProc.end() )
	{
		SAFE_DELETE ( (*iter).second );
		m_mapQuestProc.erase ( iter );
		return true;
	}
	else
	{
		return false;
	}
}

bool GLQuestPlay::InsertEnd ( const GLQUESTPROG &sQuestProg )
{
	GLQUESTPROG* pQuestProg = FindEnd ( sQuestProg.m_sNID.dwID );
	if ( pQuestProg )
	{
		*pQuestProg = sQuestProg;
		return false;
	}
	else
	{
		std::pair< MAPQUEST_ITER, bool > Success;
		pQuestProg = new GLQUESTPROG;
		*pQuestProg = sQuestProg;
		Success = m_mapQuestEnd.insert ( std::make_pair(sQuestProg.m_sNID.dwID, pQuestProg) );
		if (Success.second == true)
		{
			return true;
		}
		else
		{
			SAFE_DELETE(pQuestProg);
			return false;
		}
	}
}

bool GLQuestPlay::DeleteEnd ( DWORD dwQuestID )
{
	MAPQUEST_ITER iter = m_mapQuestEnd.find ( dwQuestID );
	if ( iter!=m_mapQuestEnd.end() )
	{
		SAFE_DELETE ( (*iter).second );
		m_mapQuestEnd.erase ( iter );
		return true;
	}
	else
	{
		return false;
	}
}

void GLQuestPlay::DeleteAll ()
{
	std::for_each ( m_mapQuestProc.begin(), m_mapQuestProc.end(), std_afunc::DeleteMapObject() );
	m_mapQuestProc.clear();

	std::for_each ( m_mapQuestEnd.begin(), m_mapQuestEnd.end(), std_afunc::DeleteMapObject() );
	m_mapQuestEnd.clear();
}

GLQuestPlay& GLQuestPlay::operator= ( GLQuestPlay& value )
{
	DeleteAll ();

	{
		MAPQUEST_ITER iter = value.GetQuestProc().begin();
		MAPQUEST_ITER iter_end = value.GetQuestProc().end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG sPROG;
			sPROG = *(*iter).second;

			InsertProc ( sPROG );
		}
	}

	{
		MAPQUEST_ITER iter = value.GetQuestEnd().begin();
		MAPQUEST_ITER iter_end = value.GetQuestEnd().end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG sPROG;
			sPROG = *(*iter).second;

			InsertEnd( sPROG );
		}
	}

	return *this;
}

bool GLQuestPlay::GET_BYBUFFER ( CByteStream &ByteStream )
{
	DeleteAll ();
	if ( ByteStream.IsEmpty() )		return true;

	DWORD dwVer(0);
	DWORD dwSize(0);

	ByteStream >> dwVer;

	ByteStream >> dwSize;
	for ( DWORD i=0 ; i<dwSize; ++i )
	{
		GLQUESTPROG sQuestProg;
		sQuestProg.GET_BYBUFFER ( ByteStream );

		InsertProc ( sQuestProg );
	}

	ByteStream >> dwSize;
	for ( DWORD i=0 ; i<dwSize; ++i )
	{
		GLQUESTPROG sQuestEnd;
		sQuestEnd.GET_BYBUFFER ( ByteStream );

		InsertEnd ( sQuestEnd );
	}

	return true;
}

bool GLQuestPlay::SET_BYBUFFER ( CByteStream &ByteStream ) const
{
	ByteStream << (DWORD) VERSION;

	ByteStream << (DWORD) m_mapQuestProc.size();
	{
		MAPQUEST_CITER iter = m_mapQuestProc.begin();
		MAPQUEST_CITER iter_end = m_mapQuestProc.end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG* pQuestProg = (*iter).second;
			pQuestProg->SET_BYBUFFER ( ByteStream );
		}
	}

	ByteStream << (DWORD) m_mapQuestEnd.size();
	{
		MAPQUEST_CITER iter = m_mapQuestEnd.begin();
		MAPQUEST_CITER iter_end = m_mapQuestEnd.end();
		for ( ; iter!=iter_end; ++iter )
		{
			GLQUESTPROG* pQuestEnd = (*iter).second;
			pQuestEnd->SET_BYBUFFER ( ByteStream );
		}
	}

	return true;
}

HRESULT GLQuestPlay::FrameMove ( float fTime, float fElapsedTime )
{
	MAPQUEST_ITER iter = m_mapQuestProc.begin();
	MAPQUEST_ITER iter_end = m_mapQuestProc.end();
	for ( ; iter!=iter_end; ++iter )
	{
		GLQUESTPROG* pQuestProg = (*iter).second;

		pQuestProg->FrameMove ( fTime, fElapsedTime );
	}

	return S_OK;
}

