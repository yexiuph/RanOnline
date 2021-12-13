#include "pch.h"

#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./StlFunctions.h"
#include "./DxEffectMan.h"
#include "./DxEffectParticleSys.h"

#include "./DxEffcharData.h"
#include "./DxEffGroupPlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEffGroupPlayer& DxEffGroupPlayer::GetInstance()
{
	static DxEffGroupPlayer Instance;
	return Instance;
}

DxEffGroupPlayer::DxEffGroupPlayer (void) :
	m_fTimer(0.0f),
	m_pd3dDevice(NULL)
{
}

DxEffGroupPlayer::~DxEffGroupPlayer (void)
{
}

D3DXVECTOR3 DxEffGroupPlayer::GetTargetIDPos ( STARGETID *pTargetID )
{
	D3DXVECTOR3 vPos;
	
	vPos.x = 0.f;
	vPos.y = 10.f;
	vPos.z = -50.f;

	return vPos;
}

HRESULT DxEffGroupPlayer::PassiveEffect ( const char* szFileName, const D3DXMATRIX &matTrans, const STARGETID &sTargetID )
{
	if ( sTargetID.dwID==EMTARGET_NULL )		return E_FAIL;
	if ( !szFileName || szFileName[0]==NULL )	return E_FAIL;

	std::string strFileName = szFileName;
	std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

	BOOL bExist = FALSE;

	EFFSGMAP_PAIR pairESG = m_mapESG.equal_range ( sTargetID );
	EFFSGMAP_ITER iter = pairESG.first;
	for ( ; iter != pairESG.second; ++iter )
	{
		DxEffSingleGroup* pEffSingleGroup = (*iter).second;
		if ( pEffSingleGroup->m_strFileName==strFileName )
		{
			bExist = TRUE;
		}
	}

	if ( !bExist )
	{
		NewEffGroup ( strFileName.c_str(), matTrans, &sTargetID );
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::DeletePassiveEffect ( const STARGETID &sTargetID )
{
	if ( sTargetID.dwID==EMTARGET_NULL )	return E_FAIL;

	EFFSGMAP_PAIR pairESG = m_mapESG.equal_range ( sTargetID );
	EFFSGMAP_ITER iter = pairESG.first;
	for ( ; iter != pairESG.second; )
	{
		DxEffSingleGroup* pEffSingleGroup = (*iter).second;
		EFFSGMAP_ITER iter_del = iter;
		++iter;

		SAFE_DELETE((*iter_del).second);
		m_mapESG.erase(iter_del);
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::DeletePassiveEffect ( const char* szFileName, const STARGETID &sTargetID )
{
	if ( sTargetID.dwID==EMTARGET_NULL )	return E_FAIL;

	std::string strFileName = szFileName;
	std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

	EFFSGMAP_PAIR pairESG = m_mapESG.equal_range ( sTargetID );
	EFFSGMAP_ITER iter = pairESG.first;
	for ( ; iter != pairESG.second; )
	{
		DxEffSingleGroup* pEffSingleGroup = (*iter).second;
		if ( pEffSingleGroup->m_strFileName==strFileName )
		{
			EFFSGMAP_ITER iter_del = iter;
			++iter;

			SAFE_DELETE((*iter_del).second);
			m_mapESG.erase(iter_del);
		}
		else
		{
			++iter;
		}
	}

	return S_OK;
}

BOOL DxEffGroupPlayer::FindPassiveEffect ( const char* szFileName, const STARGETID &sTargetID )
{
	if ( sTargetID.dwID==EMTARGET_NULL )	return FALSE;

	std::string strFileName = szFileName;
	std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

	EFFSGMAP_PAIR pairESG = m_mapESG.equal_range ( sTargetID );
	EFFSGMAP_ITER iter = pairESG.first;
	for ( ; iter != pairESG.second; ++iter )
	{
		DxEffSingleGroup* pEffSingleGroup = (*iter).second;
		if ( pEffSingleGroup->m_strFileName==strFileName )	return TRUE;
	}

	return FALSE;
}

//HRESULT DxEffGroupPlayer::NewEffSingle ( EFF_PROPERTY* pTargetProp, const D3DXMATRIX &matTrans )
//{
//	//	Note : Eff Group instance 생성.
//	//
//	DxEffSingleGroup* pEffSingleGroup = EFF_PROPGROUP::NEWEFFSINGLE ( pTargetProp );
//	if ( !pEffSingleGroup )	return E_FAIL;
//
//	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
//	//
//	pEffSingleGroup->m_matWorld = matTrans;
//
//	//	Note : Eff Group Create Dev.
//	//
//	pEffSingleGroup->Create ( m_pd3dDevice );
//
//	//	Note : 리스트에 등록함.
//	//
//	m_mapESG.insert ( std::make_pair(STARGETID(),pEffSingleGroup) );
//
//	return S_OK;
//}

HRESULT DxEffGroupPlayer::NewEffGroup ( EFF_PROPGROUP* pPropGroup, EFF_PROPERTY* pTargetProp, const D3DXMATRIX &matTrans )
{
	//	Note : Eff Group instance 생성.
	//
	GASSERT(pPropGroup);
	DxEffSingleGroup* pEffSingleGroup = pPropGroup->NEWEFFGROUP ( pTargetProp );
	if ( !pEffSingleGroup )	return E_FAIL;

	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
	//
	pEffSingleGroup->m_matWorld = matTrans;

	//	Note : Eff Group Create Dev.
	//
	pEffSingleGroup->Create ( m_pd3dDevice );

	//	Note : 리스트에 등록함.
	//
	m_mapESG.insert ( std::make_pair(STARGETID(),pEffSingleGroup) );

	return S_OK;
}

DxEffSingleGroup* DxEffGroupPlayer::NewEffGroup ( EFF_PROPGROUP* pPropGroup, const D3DXMATRIX &matTrans, const STARGETID* pTargetID )
{
	//	Note : Eff Group instance 생성.
	//
	GASSERT(pPropGroup);
	DxEffSingleGroup* pEffSingleGroup = pPropGroup->NEWEFFGROUP ();
	if ( !pEffSingleGroup )	return NULL;

	//	Note : 타겟지정.
	//
	pEffSingleGroup->SetTargetID ( pTargetID );

	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
	//
	pEffSingleGroup->m_matWorld = matTrans;

	//	Note : Eff Group Create Dev.
	//
	pEffSingleGroup->Create ( m_pd3dDevice );

	//	Note : 리스트에 등록함.
	//
	STARGETID sTargetID;
	if ( pTargetID )	sTargetID = *pTargetID;
	m_mapESG.insert ( std::make_pair(sTargetID,pEffSingleGroup) );

	return pEffSingleGroup;
}

DxEffSingleGroup* DxEffGroupPlayer::NewEffGroup ( const char* szFileName, const D3DXMATRIX &matTrans, const STARGETID* pTargetID )
{
	GASSERT(m_pd3dDevice);
	EFF_PROPGROUP* pPropGroup;
	
	//	Note : Prop Group 을 가져옴.
	//
	pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( szFileName );
	if ( !pPropGroup )	return NULL;

	//	Note : Eff Group instance 생성.
	//
	DxEffSingleGroup* pEffSingleG = NewEffGroup ( pPropGroup, matTrans, pTargetID );

	return pEffSingleG;
}

HRESULT DxEffGroupPlayer::NewEffBody ( const char* szFileName, const STARGETID* pTargetID, D3DXVECTOR3 *pDir )
{
	GASSERT(pTargetID);

	DxSkinChar* pSkinChar = DxEffectMan::GetInstance().GetSkinChar ( *pTargetID );
	if ( !pSkinChar )	return E_FAIL;

	D3DXVECTOR3 vDIR_ORG(0.f,0.f,-1.f);
	float fdir_y = 0;
	if ( pDir )		fdir_y = DXGetThetaYFromDirection ( *pDir, vDIR_ORG );

	BOOL bOk = DxEffcharDataMan::GetInstance().PutEffect ( pSkinChar, szFileName, pDir, fdir_y );
	if ( !bOk )			return E_FAIL;

	return S_OK;
}

HRESULT DxEffGroupPlayer::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	m_pd3dDevice = pd3dDevice;

	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).second->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::RestoreDeviceObjects ()
{
	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).second->RestoreDeviceObjects ( m_pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::InvalidateDeviceObjects ()
{
	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).second->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::DeleteDeviceObjects ()
{
	RemoveAllEff ();

	return S_OK;
}

HRESULT DxEffGroupPlayer::RemoveAllEff ()
{
	std::for_each ( m_mapESG.begin(), m_mapESG.end(), std_afunc::DeleteMapObject() );
	m_mapESG.clear();

	return S_OK;
}

HRESULT DxEffGroupPlayer::DeleteEff ( const char* szFile )
{
	if ( !szFile )						return S_FALSE;
	if ( m_mapESG.empty() )				return S_FALSE;

	std::string strFileName = szFile;
	std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; ++iter )
	{
		if ( (*iter).second->m_strFileName==strFileName )
		{
			SAFE_DELETE ( (*iter).second );
			m_mapESG.erase ( iter );
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT DxEffGroupPlayer::DeleteAllEff ( const char* szFile )
{
	if ( !szFile )						return S_FALSE;
	if ( m_mapESG.empty() )				return S_FALSE;

	std::string strFileName = szFile;
	std::transform ( strFileName.begin(), strFileName.end(), strFileName.begin(), tolower );

	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; )
	{
		EFFSGMAP_ITER iter_cur = iter++;
		if ( (*iter_cur).second->m_strFileName==strFileName )
		{
			SAFE_DELETE ( (*iter_cur).second );
			m_mapESG.erase ( iter_cur );
		}
	}

	return S_OK;
}

void DxEffGroupPlayer::NewAfterEff ( DxEffSingleGroup* pEffSG )
{
	if ( pEffSG->m_listEffSAfter.empty() )	return;

	STARGETID sTargetID = pEffSG->m_TargetID;
	D3DXVECTOR3 vTARPOS = DxEffectMan::GetInstance().GetCrowPos ( sTargetID );
	vTARPOS.y += 15.0f;

	float fTARDIR = DxEffectMan::GetInstance().GetCrowDir ( sTargetID );

	DxEffSingleGroup::AFTERLIST_ITER iter = pEffSG->m_listEffSAfter.begin();
	DxEffSingleGroup::AFTERLIST_ITER iter_end = pEffSG->m_listEffSAfter.end();
	for ( ; iter!=iter_end; ++iter )
	{
		CString strTemp = (*iter).m_strEffect.c_str();
		if ( !((*iter).m_strEffect.empty()) )
		{
			switch ( (*iter).m_emType )
			{
			case EFFABODY:
				{
					DxSkinChar* pSkinChar = DxEffectMan::GetInstance().GetSkinChar ( sTargetID );
					if ( !pSkinChar )	continue;

					D3DXVECTOR3 vDir = vTARPOS - pEffSG->m_vStartPos;
					D3DXVec3Normalize ( &vDir, &vDir );

					DxEffcharDataMan::GetInstance().PutEffect
					(
						pSkinChar,
						(*iter).m_strEffect.c_str(),
						&vDir,
						fTARDIR
					);
				}
				break;

			case EFFASINGLE:
				{
					//	보이지 않는 타갯일 경우 타격 이팩트는 생략됨.
					if ( !DxEffectMan::GetInstance().IsCrowVisible(sTargetID) )	break;

					//	타겟의 위치.
					sTargetID.vPos = vTARPOS;

					D3DXMATRIX matEffect;
					D3DXMatrixTranslation ( &matEffect, vTARPOS.x, vTARPOS.y, vTARPOS.z );

					NewEffGroup
					(
						(*iter).m_strEffect.c_str(),
						matEffect,
						&sTargetID
					);
				}
				break;
			};
		}
	}

	pEffSG->m_listEffSAfter.clear();
}

HRESULT DxEffGroupPlayer::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTimer += fElapsedTime;

	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; )
	{
		(*iter).second->FrameMove ( fTime, fElapsedTime );

		if ( (*iter).second->IsBeginAfterEffect() )
		{
			EFFSGMAP_ITER iter_del = iter;
			++iter;

			NewAfterEff ( (*iter_del).second );
		}
		else if ( (*iter).second->m_dwAliveCount==0 )
		{
			EFFSGMAP_ITER iter_del = iter;
			++iter;

			SAFE_DELETE((*iter_del).second);
			m_mapESG.erase(iter_del);
		}
		else
		{
			++iter;
		}
	}

	//CDebugSet::ToView ( 1, 2, "PARTICLE SLEEP OBJ : %d", DxEffectParticleSys::GetParticlePool()->GetAmount() );

	return S_OK;
}

HRESULT	DxEffGroupPlayer::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFSGMAP_ITER iter = m_mapESG.begin();
	EFFSGMAP_ITER iter_end = m_mapESG.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).second->Render ( pd3dDevice );
	}

	return S_OK;
}



