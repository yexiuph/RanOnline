#include "pch.h"
#include "NpcTalkControl.h"
#include "NpcTalk.h"
#include "SerialFile.h"
#include "StlFunctions.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////////////////
//
CNpcTalkControl::CNpcTalkControl ()
{
}

CNpcTalkControl::~CNpcTalkControl ()
{
	RemoveTalk ();
}

BOOL CNpcTalkControl::SaveCSVFile ( std::fstream &SFile )
{
//	SFile << (WORD)VERSION  <<",";

//	SFile << (int)m_mapTalk.size () <<",";
	NPCMAP_IT iter = m_mapTalk.begin();
	NPCMAP_IT iter_end = m_mapTalk.end();	
	for ( ; iter!=iter_end; ++iter )
	{
		SNpcTalk* pNpcTalk = (*iter).second;
		pNpcTalk->SAVECSV ( SFile );
	}

	return TRUE;
}

BOOL CNpcTalkControl::SaveFile ( CSerialFile& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	SFile << (WORD)VERSION;

	SFile << (int)m_mapTalk.size ();
	NPCMAP_IT iter = m_mapTalk.begin();
	NPCMAP_IT iter_end = m_mapTalk.end();	
	for ( ; iter!=iter_end; ++iter )
	{
		SNpcTalk* pNpcTalk = (*iter).second;
		pNpcTalk->SAVE ( SFile );
	}

	return TRUE;
}

BOOL CNpcTalkControl::LoadFile ( basestream& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	WORD wVer = 0;
	SFile >> wVer;

	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
		SNpcTalk* pNpcTalk = new SNpcTalk;
		pNpcTalk->LOAD ( SFile );

		AddTalk ( pNpcTalk );
	}

	return TRUE;
}

void CNpcTalkControl::RemoveTalk ()
{
	std::for_each ( m_mapTalk.begin(), m_mapTalk.end(), std_afunc::DeleteMapObject() );
	m_mapTalk.clear ();
}

DWORD CNpcTalkControl::MakeTalkNID ()
{
	DWORD dwNID = 0;

	NPCMAP_IT iter = m_mapTalk.begin();
	NPCMAP_IT iter_end = m_mapTalk.end();	
	for ( ; iter!=iter_end; ++iter )
	{
		SNpcTalk* pNpcTalk = (*iter).second;
		if ( dwNID < pNpcTalk->m_dwNID )
		{
			dwNID = pNpcTalk->m_dwNID;
		}
	}

	return (dwNID+1);
}

BOOL CNpcTalkControl::AddTalk ( SNpcTalk* pTalk )
{
	NPCMAP_IT found = m_mapTalk.find ( pTalk->m_dwNID );
	if ( found == m_mapTalk.end() )
	{
		m_mapTalk[pTalk->m_dwNID] = pTalk;
		return TRUE;
	}

	return FALSE;
}

void CNpcTalkControl::DelTalk ( DWORD NativeID )
{
	NPCMAP_IT found;
	found = m_mapTalk.find ( NativeID );
	if ( found != m_mapTalk.end() )
	{
		SNpcTalk* pNpcTalk = (*found).second;
		SAFE_DELETE ( pNpcTalk );
		m_mapTalk.erase ( found );
	}
}

SNpcTalk*	CNpcTalkControl::GetTalk ( DWORD NativeID )
{
	NPCMAP_IT found;
	found = m_mapTalk.find ( NativeID );
	if ( found != m_mapTalk.end() )
	{
		return (*found).second;
	}

	return NULL;
}


SNpcTalk*	CNpcTalkControl::GetTalk2 ( DWORD NativeNum )
{
	int iFoundNum = 0;

	NPCMAP_IT iter = m_mapTalk.begin();
	NPCMAP_IT iter_end = m_mapTalk.end();	
	for ( ; iter!=iter_end; ++iter )
	{
		if( iFoundNum == NativeNum )
			return (*iter).second;
		iFoundNum++;
	}

	return NULL;
}

DWORD CNpcTalkControl::ToUp ( DWORD dwNID )
{
	if ( m_mapTalk.empty() )		return UINT_MAX;

	NPCMAP_IT iter = m_mapTalk.find ( dwNID );
	if ( iter==m_mapTalk.begin() )	return UINT_MAX;

	NPCMAP_IT prev = iter;
	--prev;

	SNpcTalk *pTalk_iter = (*iter).second;
	SNpcTalk *pTalk_prev = (*prev).second;

	DWORD dwNID_iter = pTalk_iter->m_dwNID;
	DWORD dwNID_prev = pTalk_prev->m_dwNID;

	pTalk_prev->m_dwNID = dwNID_iter;
	(*iter).second = pTalk_prev;

	pTalk_iter->m_dwNID = dwNID_prev;
	(*prev).second = pTalk_iter;

	return pTalk_iter->m_dwNID;
}

DWORD CNpcTalkControl::ToDown ( DWORD dwNID )
{
	if ( m_mapTalk.empty() )					return UINT_MAX;

	NPCMAP_IT iter = m_mapTalk.find ( dwNID );

	if ( iter == m_mapTalk.end() )				return UINT_MAX;

	SNpcTalk *pTalk_iter = (*iter).second; 
	SNpcTalk *pTalk_end = m_mapTalk.rbegin()->second;

	if ( pTalk_iter->m_dwNID == pTalk_end->m_dwNID ) return UINT_MAX;
	
    NPCMAP_IT next = iter;
	next++;
	
	SNpcTalk *pTalk_next = (*next).second;

	DWORD dwNID_iter = pTalk_iter->m_dwNID;
	DWORD dwNID_next = pTalk_next->m_dwNID;

	pTalk_next->m_dwNID = dwNID_iter;
	(*iter).second = pTalk_next;

	pTalk_iter->m_dwNID = dwNID_next;
	(*next).second = pTalk_iter;

	return pTalk_iter->m_dwNID;
}

CNpcTalkControl& CNpcTalkControl::operator= ( const CNpcTalkControl& rNpcTalkControl )
{
	NPCMAP_CIT iter = rNpcTalkControl.m_mapTalk.begin();
	NPCMAP_CIT iter_end = rNpcTalkControl.m_mapTalk.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SNpcTalk* pNpcTalk = (*iter).second;
		SNpcTalk* pNewTalk = new SNpcTalk;
		*pNewTalk = *pNpcTalk;
		AddTalk ( pNewTalk );
	}

	return *this;
}

CNpcTalkControl::NPCMAP* CNpcTalkControl::GetTalkMap ()
{
	return &m_mapTalk;
}

DWORD CNpcTalkControl::GetTalkNum ()
{
	return (DWORD) m_mapTalk.size();
}