#include "pch.h"

#include <algorithm>

#include "./DxEffSingle.h"
#include "./StlFunctions.h"

#include "./DxSkinChar.h"

#include "./DxEffAni.h"
#include "./DxEffAniGhosting.h"
#include "./DxEffAniFaceOff.h"

#include "./DxEffAniPlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEffAniPlayer::DxEffAniPlayer (void) :
	m_fTimer(0.0f),
	m_pd3dDevice(NULL)
{
}

DxEffAniPlayer::~DxEffAniPlayer (void)
{
	RemoveAllEff ();
}

HRESULT DxEffAniPlayer::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	m_pd3dDevice = pd3dDevice;

	EFFLIST_ITER iter = m_listAni.begin();
	EFFLIST_ITER iter_end = m_listAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->InitDeviceObjects( pd3dDevice );
	}

	iter = m_listOneSet.begin();
	iter_end = m_listOneSet.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->InitDeviceObjects( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffAniPlayer::RestoreDeviceObjects ()
{
	EFFLIST_ITER iter = m_listAni.begin();
	EFFLIST_ITER iter_end = m_listAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->RestoreDeviceObjects( m_pd3dDevice );
	}

	iter = m_listOneSet.begin();
	iter_end = m_listOneSet.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->RestoreDeviceObjects( m_pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffAniPlayer::InvalidateDeviceObjects ()
{
	EFFLIST_ITER iter = m_listAni.begin();
	EFFLIST_ITER iter_end = m_listAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->InvalidateDeviceObjects ();
	}

	iter = m_listOneSet.begin();
	iter_end = m_listOneSet.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->InvalidateDeviceObjects();
	}

	return S_OK;
}

HRESULT DxEffAniPlayer::DeleteDeviceObjects ()
{
	EFFLIST_ITER iter = m_listAni.begin();
	EFFLIST_ITER iter_end = m_listAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->DeleteDeviceObjects ();
	}

	iter = m_listOneSet.begin();
	iter_end = m_listOneSet.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->DeleteDeviceObjects();
	}

	return S_OK;
}

void	DxEffAniPlayer::ResetEff ( PSANIMCONTAINER pAnimContainer )
{
	SANIMCONINFO::EFFANILIST_ITER iter = pAnimContainer->m_listEffAni.begin();
	SANIMCONINFO::EFFANILIST_ITER iter_end = pAnimContainer->m_listEffAni.end();

	for ( ; iter!=iter_end; ++iter )
	{
		if ( (*iter)->GetTypeID() == EMEFFANI_GHOSTING )
		{
			((DxEffAniData_Ghosting*)(*iter))->SetCreated( FALSE );
		}
		if ( (*iter)->GetTypeID() == EMEFFANI_FACEOFF )
		{
			((DxEffAniData_FaceOff*)(*iter))->SetCreated( FALSE );
		}
	}
}

void DxEffAniPlayer::CheckCreateEff( PSANIMCONTAINER pAnimContainer )		// 처음 로딩시 셋팅하는 것.
{
	if( !pAnimContainer )		return;

	SANIMCONINFO::EFFANILIST_ITER iter = pAnimContainer->m_listEffAni.begin();
	SANIMCONINFO::EFFANILIST_ITER iter_end = pAnimContainer->m_listEffAni.end();

	for ( ; iter!=iter_end; ++iter )
	{
		if ( (*iter)->GetTypeID() != EMEFFANI_SINGLE )
		{
			CreateEff( (*iter), pAnimContainer );	// 동작 하도록 함.
		}
	}
}

void	DxEffAniPlayer::CheckCreateEff( PSANIMCONTAINER pAnimContainer, int nPrevKeyTime, DWORD dwCurKeyTime )	// 매 프레임 셋팅.
{
	if ( !pAnimContainer )		return;

	SANIMCONINFO::EFFANILIST_ITER iter = pAnimContainer->m_listEffAni.begin();
	SANIMCONINFO::EFFANILIST_ITER iter_end = pAnimContainer->m_listEffAni.end();

	int nCount = 0;
	for ( ; iter!=iter_end; ++iter, ++nCount )
	{
		if ( (*iter)->CheckEff( dwCurKeyTime, nPrevKeyTime ) )
		{
			CreateEff( (*iter), pAnimContainer );	// 동작 하도록 함.
		}
	}
}

void	DxEffAniPlayer::CreateEff ( DxEffAniData* pEff, PSANIMCONTAINER pAnimContainer )		// 복제를 해서 이펙트를 생성한다.
{
	DxEffAni*	pNewEff = pEff->NEWOBJ( pAnimContainer );

	m_listAni.push_back ( pNewEff );
}

HRESULT DxEffAniPlayer::RemoveAllEff ()
{
	std::for_each ( m_listAni.begin(), m_listAni.end(), std_afunc::DeleteObject() );
	m_listAni.clear();

	return S_OK;
}

HRESULT DxEffAniPlayer::DeleteEff ( const TCHAR* szFile )
{
	//if ( !szFile )						return S_FALSE;
	//if ( m_listAni.empty() )	return S_FALSE;

	//EFFLIST_ITER iter = m_listAni.begin();
	//EFFLIST_ITER iter_end = m_listAni.end();
	//for ( ; iter!=iter_end; ++iter )
	//{
	//	if ( !strcmp ( (*iter)->m_szFileName, szFile ) )
	//	{
	//		SAFE_DELETE ( (*iter) );
	//		m_listAni.erase ( iter );
	//		return TRUE;
	//	}
	//}

	return S_OK;
}

HRESULT DxEffAniPlayer::FrameMove ( PSANIMCONTAINER pAnimContainer, float fTime, float fElapsedTime, SKINEFFDATA& sSKINEFFDATA )
{
	m_fTimer += fElapsedTime;

	sSKINEFFDATA.m_bNewHeadDRAW = FALSE;

	EFFLIST_ITER iter = m_listAni.begin();
	EFFLIST_ITER iter_end = m_listAni.end();
	for ( ; iter!=iter_end; )
	{
		(*iter)->CheckFrameMove( pAnimContainer );		// 애니메이션이 같은지 체크한다.!!
		if( (*iter)->IsHeadDrawChild() )
		{
			sSKINEFFDATA.m_bNewHeadDRAW = TRUE;
		}

		(*iter)->FrameMove ( fTime, fElapsedTime );
		EFFLIST_ITER iter_del = iter++;					// 지워지면 정보가 없어질것을 가정해서 먼저 위치를 옮긴다.

		if ( (*iter_del)->IsEnd() )
		{
			SAFE_DELETE((*iter_del));
			m_listAni.erase(iter_del);
		}
	}

	return S_OK;
}

HRESULT	DxEffAniPlayer::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pSkinChar, const D3DXMATRIX& matCurPos )
{
	EFFLIST_ITER iter = m_listAni.begin();
	EFFLIST_ITER iter_end = m_listAni.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter)->Render ( pd3dDevice, pSkinChar, matCurPos );
	}

	//iter = m_listOneSet.begin();
	//iter_end = m_listOneSet.end();
	//for ( ; iter!=iter_end; ++iter )
	//{
	//	//(*iter)->Render ( pd3dDevice, pSkinChar, matCurPos );
	//}

	return S_OK;
}


