#include "pch.h"
#include "./DxEffSingle.h"
#include "./DxEffectParticleSys.h"
#include "./DxEffectGround.h"
#include "./DxEffectMesh.h"
#include "./DxEffectSequence.h"
#include "./DxEffectDecal.h"
#include "./DxEffectCamera.h"
#include "./DxEffectBlurSys.h"
#include "./DxEffectLighting.h"
#include "./DxEffectMoveRotate.h"
#include "./DxEffectSkinMesh.h"
#include "./DxEffectWave.h"
#include "./DxEffectPointLight.h"
#include "./DxEffectMoveTarget.h"

#include "./DxEffectMan.h"

#include "./SerialFile.h"
#include "./EDITMESHS.h"
#include "./DxViewPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL	g_bEffectAABBBox	= FALSE;
BOOL	g_bEffectTool		= FALSE;
BOOL	g_bEFFtoTEX			= FALSE;

//---------------------------------------------------------------------- [ DxEffSingle ]

BOOL DxEffSingle::IsReadyToDie ()
{
	return ( m_fGAge>=(m_fGBeginTime+m_fGLifeTime) );
}

void DxEffSingle::ReStartEff ()
{
	m_fGAge = 0.f;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;
	m_dwRunFlag &= ~EFF_PLY_RENDPASS;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffSingle::FrameMove ( float fTime, float fElapsedTime )
{
	GASSERT(m_pThisGroup);

	//	���� ������Ʈ
	m_MovSound.Update ( m_pThisGroup->m_vGNowPos, m_fGBeginTime, m_fGAge, m_fGLifeTime );

	if ( m_pChild )
	{
		if ( !( m_pChild->m_dwRunFlag & EFF_PLY_END ) )		++m_pThisGroup->m_dwAliveCount;
		m_pChild->FrameMove ( fTime, fElapsedTime );
	}

	if ( m_pSibling )
	{
		if ( !( m_pSibling->m_dwRunFlag & EFF_PLY_END ) )	++m_pThisGroup->m_dwAliveCount;
		m_pSibling->FrameMove ( fTime, fElapsedTime );
	}

	return S_OK;
}

HRESULT	DxEffSingle::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	GASSERT(0&&"DxEffSingle::Render() not defined!");

	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matComb );
	//if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb ); -- [ERROR CODE]

	return S_OK;
}

HRESULT DxEffSingle::OneTimeSceneInit ()
{
	if ( m_pChild )		m_pChild->OneTimeSceneInit ();
	if ( m_pSibling )	m_pSibling->OneTimeSceneInit ();

	return S_OK;
}

HRESULT DxEffSingle::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pChild )		m_pChild->InitDeviceObjects ( pd3dDevice );
	if ( m_pSibling )	m_pSibling->InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffSingle::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pThisGroup->m_dwFlag & EFF_CFG_NEVERDIE )
	{
		if ( m_pThisGroup->m_fGBeginTime > m_fGBeginTime )	m_pThisGroup->m_fGBeginTime = m_fGBeginTime;
		if ( m_pThisGroup->m_fGLifeTime < m_fGLifeTime )	m_pThisGroup->m_fGLifeTime = m_fGLifeTime;
	}
	if ( m_bMoveObj )
	{
		m_pThisGroup->m_fGBeginTime = m_fGBeginTime;
		m_pThisGroup->m_fGLifeTime = m_fGLifeTime;
	}

	if ( m_pChild )		m_pChild->RestoreDeviceObjects ( pd3dDevice );
	if ( m_pSibling )	m_pSibling->RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffSingle::InvalidateDeviceObjects ()
{
	if ( m_pChild )		m_pChild->InvalidateDeviceObjects ();
	if ( m_pSibling )	m_pSibling->InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxEffSingle::DeleteDeviceObjects ()
{
	if ( m_pChild )		m_pChild->DeleteDeviceObjects ();
	if ( m_pSibling )	m_pSibling->DeleteDeviceObjects ();

	return S_OK;
}

HRESULT DxEffSingle::FinalCleanup ()
{
	if ( m_pChild )		m_pChild->FinalCleanup ();
	if ( m_pSibling )	m_pSibling->FinalCleanup ();

	PutSleepMemory( m_pChild );
	PutSleepMemory( m_pSibling );
//	SAFE_DELETE(m_pChild);
//	SAFE_DELETE(m_pSibling);

	return S_OK;
}

//---------------------------------------------------------------------- [ DxEffSingleGroup ]

void DxEffSingleGroup::SetTargetID ( const STARGETID* pTargetID )
{
	if ( pTargetID )	m_TargetID = *pTargetID;
}

void DxEffSingleGroup::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax = *m_pMax;	vMin = *m_pMin;
}

BOOL DxEffSingleGroup::IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3 vPos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
	return COLLISION::IsWithInPoint ( vDivMax, vDivMin, vPos );
}

HRESULT DxEffSingleGroup::FrameMove ( float fTime, float fElapsedTime )
{
	m_dwAliveCount = 0;

	//Note : 2 fps  �ؾ� �ӵ�, ó�� �ε��� ���� �߻�. 
	//
	if ( fElapsedTime > 0.5f )		fElapsedTime = 0.03f;

	m_fElapsedTime = fElapsedTime;

	if ( m_pEffSingleRoot )
	{
		//	ȿ�� �׷� �Ӽ��� ���� �ϴ� �ൿ ��ȭ	- ��ġ ����
		//
		if ( m_dwFlag & EFF_CFG_TARGETPOS )			// ����
		{
			//	m_TargetID	: ��ǥ ��ġ
			//	m_pmatLocal : �߻� ��ġ

			//	Note : Eff�� ���̸� ���.
			//
			m_fGAge += fElapsedTime;

			if ( m_fGAge > m_fGBeginTime && m_fGAge < m_fGBeginTime + m_fGLifeTime )
			{
				if ( m_dwFlag & EFF_CFG_AVERAGE )			// ������ �ӵ��� �̵�.
				{
					m_vGDelta.x = (m_TargetID.vPos.x-m_matWorld._41) / m_fGLifeTime;	// ���ݰ��� ���󰡴� ��
					m_vGDelta.y = (m_TargetID.vPos.y-m_matWorld._42) / m_fGLifeTime;
					m_vGDelta.z = (m_TargetID.vPos.z-m_matWorld._43) / m_fGLifeTime;

					m_vGNowPos.x = m_matWorld._41 + m_vGDelta.x*(m_fGAge-m_fGBeginTime);
					m_vGNowPos.y = m_matWorld._42 + m_vGDelta.y*(m_fGAge-m_fGBeginTime);
					m_vGNowPos.z = m_matWorld._43 + m_vGDelta.z*(m_fGAge-m_fGBeginTime);
				}
				else if ( m_dwFlag & EFF_CFG_SLOWFAST )		// ���� ������.
				{
					float fRate = m_fGAge/m_fGLifeTime;
					fRate = powf(fRate,3);

					m_vGDelta = m_TargetID.vPos - m_vStartPos;

					m_vGNowPos = m_vStartPos + m_vGDelta*fRate;
				}
				else if ( m_dwFlag & EFF_CFG_FASTSLOW )		// ���� ������.
				{
					float fRate = (m_fGLifeTime-m_fGAge)/m_fGLifeTime;
					fRate = powf(fRate,3);
					fRate = 1.f - fRate;

					m_vGDelta = m_TargetID.vPos - m_vStartPos;

					m_vGNowPos = m_vStartPos + m_vGDelta*fRate;
				}
			}
			else
			{
				m_vGNowPos.x = m_matWorld._41;
				m_vGNowPos.y = m_matWorld._42;
				m_vGNowPos.z = m_matWorld._43;
			}
		}
		else if ( m_dwFlag & EFF_CFG_TARGETID )		// ID
		{
			D3DXVECTOR3	vPos = DxEffectMan::GetInstance().GetCrowPos ( m_TargetID );
			m_vGNowPos = vPos;
		}
		else if ( m_dwFlag & EFF_CFG_TARGETLINE )
		{
			m_vGNowPos = m_TargetID.vPos;
		}
		else
		{
			m_vGNowPos.x = m_matWorld._41;
			m_vGNowPos.y = m_matWorld._42;
			m_vGNowPos.z = m_matWorld._43;
		}


		if ( !( m_pEffSingleRoot->m_dwRunFlag & EFF_PLY_END ) )		++m_dwAliveCount;
		m_pEffSingleRoot->FrameMove ( fTime, fElapsedTime );
	}

	return S_OK;
}

HRESULT	DxEffSingleGroup::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pEffSingleRoot )		return S_OK;
	if ( !m_bOut_Particle )			return S_OK;

	D3DMATERIALQ	pMaterial;
	D3DUtil_InitMaterial ( pMaterial, 1.0f, 1.0f, 1.0f, 1.0f );
	pd3dDevice->SetMaterial ( &pMaterial );

	D3DXMATRIX matTrans;

	if ( m_bOut_Direct )
	{

	}

	if ( m_dwFlag & EFF_CFG_TARGETPOS )			// ����
	{
		D3DXMatrixIdentity ( &matTrans );
		matTrans._41 = m_vGNowPos.x;
		matTrans._42 = m_vGNowPos.y;
		matTrans._43 = m_vGNowPos.z;
	}
	else if ( m_dwFlag & EFF_CFG_TARGETID )		// ID
	{
		matTrans = m_matWorld;				// ȸ�� ��Ʈ������ ����ִ�.
		matTrans._41 = m_vGNowPos.x;
		matTrans._42 = m_vGNowPos.y;
		matTrans._43 = m_vGNowPos.z;
	}
	else if ( m_dwFlag & EFF_CFG_TARGETLINE )
	{
		matTrans = m_matWorld;				// ȸ�� ��Ʈ������ ����ִ�.
	}
	else
	{
		matTrans = m_matWorld;
	}

	matTrans._42 += m_vLocal.y;


	D3DXVECTOR3 vMin = *m_pMin, vMax = *m_pMax;
	CLIPVOLUME &cCV = DxViewPort::GetInstance().GetClipVolume();

	//	Note : AABB �ڽ� ���.
	//
	COLLISION::TransformAABB ( vMax, vMin, matTrans );

	BOOL bCOL = COLLISION::IsCollisionVolume ( cCV, vMax, vMin );
	if ( !bCOL )	return S_FALSE;

	//EDITMESHS::RENDERAABB ( pd3dDevice, vMax, vMin );
	m_pEffSingleRoot->Render ( pd3dDevice, matTrans );

	
	if( !g_bEFFtoTEX )
	{
		// Note : ȿ�����ΰ� ?, 
		if( g_bEffectTool )
		{
			if(m_dwFlag&EFF_CFG_AUTOAABBBOX)
			{
				// �ڵ� AABB Box�� ���� �Ǿ� �ִ°� ?
				m_pEffSingleRoot->CheckAABBBox( *m_pMax, *m_pMin );
			}
		}

		// AABB �ڽ��� ǥ��
		if( g_bEffectAABBBox )	EDITMESHS::RENDERAABB( pd3dDevice, *m_pMax, *m_pMin );
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::OneTimeSceneInit ()
{
	m_vStartPos.x = m_matWorld._41;
	m_vStartPos.y = m_matWorld._42;
	m_vStartPos.z = m_matWorld._43;

	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->OneTimeSceneInit ();
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::InvalidateDeviceObjects ()
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::DeleteDeviceObjects ()
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->DeleteDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxEffSingleGroup::FinalCleanup ()
{
	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->FinalCleanup ();
	}

	PutSleepMemory( m_pEffSingleRoot );
	//SAFE_DELETE(m_pEffSingleRoot);

	return S_OK;
}

void DxEffSingleGroup::ReStartEff ()
{
	m_fGAge = 0.f;

	if ( m_pEffSingleRoot )
	{
		m_pEffSingleRoot->ReStartEff ();
	}
}

void DxEffSingleGroup::PutSleepMemory( PDXEFFSINGLE& pSingle )
{
//#define EFFSINGLE_ROOT			(0x0001)

	if( pSingle )
	{
		pSingle->m_pThisGroup = NULL;
		pSingle->m_pChild = NULL;
		pSingle->m_pSibling = NULL;

		// Note : ����� ������ ���̴�.
		//		���߿� ����� �ص� Pool�� ���� �Ǿ ����� ����.
		//		������ ����... Pool�� clear �ϰ� ���� ���� �ٽ� Pool�� �����ؼ� �����͸� ����� ��.
		if( !DxEffectParticleSys::m_pPool )
		{
			SAFE_DELETE( pSingle );
			pSingle = NULL;
			return;
		}

		if( pSingle->GetTypeID() == EFFSINGLE_PARTICE )
		{
			DxEffectParticleSys::m_pPool->Release( (DxEffectParticleSys*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_GROUND )
		{
			DxEffectGround::m_pPool->Release( (DxEffectGround*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_MESH )
		{
			DxEffectMesh::m_pPool->Release( (DxEffectMesh*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_SEQUENCE )
		{
			DxEffectSequence::m_pPool->Release( (DxEffectSequence*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_DECAL )
		{
			DxEffectDecal::m_pPool->Release( (DxEffectDecal*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_CAMERA )
		{
			DxEffectCamera::m_pPool->Release( (DxEffectCamera*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_BLURSYS )
		{
			DxEffectBlurSys::m_pPool->Release( (DxEffectBlurSys*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_LIGHTING )
		{
			DxEffectLightning::m_pPool->Release( (DxEffectLightning*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_MOVEROTATE )
		{
			DxEffectMoveRotate::m_pPool->Release( (DxEffectMoveRotate*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_SKINMESH )
		{
			DxEffectSkinMesh::m_pPool->Release( (DxEffectSkinMesh*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_WAVE )
		{
			DxEffectWave::m_pPool->Release( (DxEffectWave*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_POINTLIGHT )
		{
			DxEffectPointLight::m_pPool->Release( (DxEffectPointLight*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_MOVETARGET )
		{
			DxEffectMoveTarget::m_pPool->Release( (DxEffectMoveTarget*)pSingle );
		}
		else
		{
			SAFE_DELETE( pSingle );
		}
	}

	pSingle = NULL;
}

void DxEffSingle::PutSleepMemory( PDXEFFSINGLE& pSingle )
{
	if( pSingle )
	{
		pSingle->m_pThisGroup = NULL;
		pSingle->m_pChild = NULL;
		pSingle->m_pSibling = NULL;

		// Note : ����� ������ ���̴�.
		//		���߿� ����� �ص� Pool�� ���� �Ǿ ����� ����.
		//		������ ����... Pool�� clear �ϰ� ���� ���� �ٽ� Pool�� �����ؼ� �����͸� ����� ��.
		if( !DxEffectParticleSys::m_pPool )
		{
			SAFE_DELETE( pSingle );
			pSingle = NULL;
			return;
		}

		if( pSingle->GetTypeID() == EFFSINGLE_PARTICE )
		{
			DxEffectParticleSys::m_pPool->Release( (DxEffectParticleSys*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_GROUND )
		{
			DxEffectGround::m_pPool->Release( (DxEffectGround*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_MESH )
		{
			DxEffectMesh::m_pPool->Release( (DxEffectMesh*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_SEQUENCE )
		{
			DxEffectSequence::m_pPool->Release( (DxEffectSequence*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_DECAL )
		{
			DxEffectDecal::m_pPool->Release( (DxEffectDecal*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_CAMERA )
		{
			DxEffectCamera::m_pPool->Release( (DxEffectCamera*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_BLURSYS )
		{
			DxEffectBlurSys::m_pPool->Release( (DxEffectBlurSys*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_LIGHTING )
		{
			DxEffectLightning::m_pPool->Release( (DxEffectLightning*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_MOVEROTATE )
		{
			DxEffectMoveRotate::m_pPool->Release( (DxEffectMoveRotate*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_SKINMESH )
		{
			DxEffectSkinMesh::m_pPool->Release( (DxEffectSkinMesh*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_WAVE )
		{
			DxEffectWave::m_pPool->Release( (DxEffectWave*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_POINTLIGHT )
		{
			DxEffectPointLight::m_pPool->Release( (DxEffectPointLight*)pSingle );
		}
		else if( pSingle->GetTypeID() == EFFSINGLE_MOVETARGET )
		{
			DxEffectMoveTarget::m_pPool->Release( (DxEffectMoveTarget*)pSingle );
		}
		else
		{
			SAFE_DELETE( pSingle );
		}
	}

	pSingle = NULL;
}