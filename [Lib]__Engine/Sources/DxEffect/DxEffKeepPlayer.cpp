#include "pch.h"
#include <algorithm>
#include "./DxEffKeep.h"
#include "./DxEffKeepPlayer.h"

#include "./StlFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEffKeepPlayer::DxEffKeepPlayer()
{
}

DxEffKeepPlayer::~DxEffKeepPlayer()
{
	CleanUp();
}

void DxEffKeepPlayer::CleanUp()
{
	std::for_each( m_mapEff.begin(), m_mapEff.end(), std_afunc::DeleteMapObject() );
	m_mapEff.clear();
}

void DxEffKeepPlayer::CreateEff( DxEffKeepDataMain* pEff )
{
	const TCHAR* pFileName = pEff->m_strFileName.c_str();
	MAP_EFFKEEPMAIN_ITER iter = m_mapEff.find( pFileName );
	if( iter!=m_mapEff.end() )
	{
		return;		// 값이 있어서 리턴시킨다.
	}

	DxEffKeepMain* pNew = new DxEffKeepMain;
	pNew->Convert( pEff );

	m_mapEff.insert( std::make_pair( pFileName, pNew ) );
}

void DxEffKeepPlayer::DeleteEff( const TCHAR* pName )
{
	MAP_EFFKEEPMAIN_ITER iter = m_mapEff.find( pName );
	if( iter!=m_mapEff.end() )
	{
		SAFE_DELETE( (*iter).second );
		m_mapEff.erase( iter );
	}
}

void DxEffKeepPlayer::Reset()
{
	std::for_each( m_mapEff.begin(), m_mapEff.end(), std_afunc::DeleteMapObject() );
	m_mapEff.clear();
}

void DxEffKeepPlayer::FrameMove( float fElapsedTime, SKINEFFDATA& sSKINEFFDATA )
{
	MAP_EFFKEEPMAIN_ITER iter = m_mapEff.begin();
	for( ; iter!=m_mapEff.end(); ++iter )
	{
		(*iter).second->FrameMove( fElapsedTime, sSKINEFFDATA );
	}
}

void DxEffKeepPlayer::Render( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	MAP_EFFKEEPMAIN_ITER iter = m_mapEff.begin();
	for( ; iter!=m_mapEff.end(); ++iter )
	{
		(*iter).second->Render( pd3dDevice );
	}
}