#include "pch.h"

#include "./EditMeshs.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectMoveTarget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note	:	����׿�,	
extern BOOL			g_bEFFtoTEX;
extern BOOL			g_bEffectAABBBox;

//---------------------------------------------------------------------[MOVETARGET_PROPERTY]
const DWORD	MOVETARGET_PROPERTY::VERSION	= 0x0101;
const char	MOVETARGET_PROPERTY::NAME[]	= "Move (Target)";

DxEffSingle* MOVETARGET_PROPERTY::NEWOBJ ()
{
	//DxEffectMoveTarget *pEffect = new DxEffectMoveTarget;
	DxEffectMoveTarget *pEffect = DxEffectMoveTarget::m_pPool->New();

	//	Note : TransPorm Set.		[���뼳��]
	pEffect->m_matLocal = m_matLocal;

	//	Note : �ð� ����.			[���뼳��]
	//
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : ���ư��� ������Ʈ	[���뼳��]
	//
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_fObjectRadius = m_fObjectRadius;

	pEffect->m_vCustom_Velocity = m_vCustom_Velocity;
	pEffect->m_fRandom_Angel	= m_fRandom_Angel;
	pEffect->m_fGuided_Accel	= m_fGuided_Accel;

	StringCchCopy( pEffect->m_szFileName, MAX_PATH, m_szFileName );

	return pEffect;
}

HRESULT MOVETARGET_PROPERTY::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : �θ� Ŭ������ ����.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : ����, �ڽ� ����.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT MOVETARGET_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	if( dwVer == VERSION )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if( dwVer == 0x0100 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� �б�.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice );

	return S_OK;
}

HRESULT MOVETARGET_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT MOVETARGET_PROPERTY::DeleteDeviceObjects ()
{
	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

const DWORD	DxEffectMoveTarget::TYPEID	= EFFSINGLE_MOVETARGET;
const DWORD	DxEffectMoveTarget::FLAG	= NULL;
const char	DxEffectMoveTarget::NAME[]	= "Move (Target)";

CMemPool<DxEffectMoveTarget> *DxEffectMoveTarget::m_pPool = NULL;

//	Note :
//
DxEffectMoveTarget::DxEffectMoveTarget(void) :
	m_vGLocation(0,0,0),

	m_vCustom_Velocity(1.f, 0.5f, 0.f),
	m_fRandom_Angel(0.1f),
	m_fGuided_Accel(10.f),

	m_fElapsedTime(0.f),

	m_bGetInitPosDir(FALSE),

	m_vInitDir(0,0,0),

	m_SingleRender(FALSE),
	m_pPropGroup(NULL),
	m_pSingleGroup(NULL),

	m_pd3dDevice(NULL)
{
	D3DXMatrixIdentity ( &m_matTrans );
}

DxEffectMoveTarget::~DxEffectMoveTarget(void)
{
}

HRESULT DxEffectMoveTarget::CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK; 
}
HRESULT DxEffectMoveTarget::ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }

void DxEffectMoveTarget::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectMoveTarget>;
}

void DxEffectMoveTarget::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
}

void DxEffectMoveTarget::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	D3DXVECTOR3 vPos = m_sData.m_vPos;

	if( vMax.x < vPos.x )	vMax.x = vPos.x;
	if( vMax.y < vPos.y )	vMax.y = vPos.y;
	if( vMax.z < vPos.z )	vMax.z = vPos.z;

	if( vMin.x > vPos.x )	vMin.x = vPos.x;
	if( vMin.y > vPos.y )	vMin.y = vPos.y;
	if( vMin.z > vPos.z )	vMin.z = vPos.z;

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectMoveTarget::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	���ڿ��� ���� ���� �ε��Ѵ�.
	if ( !m_pPropGroup && strlen(m_szFileName) )
	{
		m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName );
		if ( !m_pPropGroup )	return E_FAIL;
	}

	if ( m_pPropGroup )
	{	
		SAFE_DELETE(m_pSingleGroup);						// ���� �����ʹ� �����ش�.
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP ();
		if ( !m_pSingleGroup )	return E_FAIL;

		//	Note : Eff Group Create Dev.
		//
		m_pSingleGroup->Create ( pd3dDevice );

		D3DXMatrixIdentity ( &m_matTrans );

		m_pSingleGroup->m_matWorld = m_matTrans;
	}

	if ( m_pSingleGroup )		m_pSingleGroup->InitDeviceObjects ( pd3dDevice );

	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectMoveTarget::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	if ( m_pSingleGroup )		m_pSingleGroup->RestoreDeviceObjects ( pd3dDevice );

	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectMoveTarget::InvalidateDeviceObjects ()
{
	if ( m_pSingleGroup )		m_pSingleGroup->InvalidateDeviceObjects ();

	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxEffectMoveTarget::DeleteDeviceObjects ()
{
	if ( m_pSingleGroup )		m_pSingleGroup->DeleteDeviceObjects ();

	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::DeleteDeviceObjects ();

	return S_OK;
}

HRESULT DxEffectMoveTarget::FinalCleanup ()
{
	if ( m_pSingleGroup )		m_pSingleGroup->FinalCleanup();
	SAFE_DELETE(m_pSingleGroup);
	//DxEffSinglePropGMan::GetInstance().DeleteEffectGProp ( m_szFileName );

	//	Note : ���� �ڽ� ���.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

void DxEffectMoveTarget::ReStartEff ()
{
	m_fGAge			= 0.f;
	m_dwRunFlag		&= ~EFF_PLY_PLAY;
	m_dwRunFlag		&= ~EFF_PLY_DONE;
	m_dwRunFlag		&= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectMoveTarget::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff�� ���̸� ���.
	//
	m_fGAge += fElapsedTime;
	m_fElapsedTime = fElapsedTime;

	//	Note : �ݺ��� �� �ð� �� ���� ���� �ʱ�ȭ �Ѵ�.
	//
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_pThisGroup->m_fGBeginTime+m_pThisGroup->m_fGLifeTime) )
		{
			m_fGAge			= 0.f;
			m_dwRunFlag		&= ~EFF_PLY_PLAY;
			m_dwRunFlag		&= ~EFF_PLY_DONE;
			m_dwRunFlag		&= ~EFF_PLY_END;
		}
	}

	if ( m_pSingleGroup && m_SingleRender )
	{
		m_pSingleGroup->FrameMove ( fTime, fElapsedTime );
	}

	if ( m_dwRunFlag & EFF_PLY_RENDPASS )
	{
		if ( m_pSingleGroup )
		{
			if ( m_pSingleGroup->m_dwAliveCount == 0 )
			{
				m_dwRunFlag |= EFF_PLY_END;		

				m_pSingleGroup->InvalidateDeviceObjects ();
				m_pSingleGroup->DeleteDeviceObjects ();
				m_pSingleGroup->FinalCleanup();
				SAFE_DELETE(m_pSingleGroup);
			}
		}
		else	m_dwRunFlag |= EFF_PLY_END;
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : ���۵��� �ʾҴ��� �˻�, ���� �ð��̸� Ȱ��ȭ ��Ŵ.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	m_sData.FrameMove ( fElapsedTime );

	//	Note : ���� ������ �Ǿ����� �˻�.
	//
	if ( IsReadyToDie() )
	{
		if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_TIMEOVER) )		// �ð� ���ῡ ���� �߰� Effect ����
		{
			if ( m_pSingleGroup )		m_SingleRender = TRUE;
		}

		m_dwRunFlag |= EFF_PLY_RENDPASS;
	}

_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT	DxEffectMoveTarget::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld = matComb;

	if ( m_pSingleGroup && m_SingleRender )
	{
		m_pSingleGroup->m_matWorld = matWorld;
		m_pSingleGroup->m_matWorld._41 = m_sData.m_vPos.x;
		m_pSingleGroup->m_matWorld._42 = m_sData.m_vPos.y;
		m_pSingleGroup->m_matWorld._43 = m_sData.m_vPos.z;

		m_pSingleGroup->Render ( pd3dDevice );
	}

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

	//	Render
	if ( m_dwFlag & USE_S_TARGET )	// ��ǥ�� ��ġ ����
	{
		if ( !Render_Target ( matWorld ) )	goto _RETURN;
	}
	else if ( m_dwFlag & USE_S_DIRECT )		Render_Direct ( matWorld );

	matWorld._41 = m_sData.m_vPos.x;
	matWorld._42 = m_sData.m_vPos.y;
	matWorld._43 = m_sData.m_vPos.z;

	// EFFECT TOOl ������ ��ġ�� ǥ�� ���� ��.
	if( g_bEffectAABBBox )
	{
		if( !g_bEFFtoTEX )
		{
			EDITMESHS::RENDERSPHERE ( pd3dDevice, m_sData.m_vPos, m_fObjectRadius );

			if ( m_dwFlag & USE_S_TARGET )		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_pThisGroup->m_TargetID.vPos, m_fObjectRadius );
		}
	}

_RETURN:
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� ���.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	return S_OK;
}

// ��ǥ�� ��ġ ����
BOOL	DxEffectMoveTarget::Render_Target ( D3DXMATRIX &matWorld )
{
	//	Note : �ʱ� ��ġ, ������ ����.
	if ( !m_bGetInitPosDir )
	{
		D3DXVECTOR3	vDir;
		float		fPower;

		m_sData.m_vPos.x = matWorld._41;
		m_sData.m_vPos.y = matWorld._42;
		m_sData.m_vPos.z = matWorld._43;

		//	��ǥ ������ �������� üũ �ϱ� ���Ͽ� �̰��� ����.
		m_vInitDir = m_sData.m_vPos - m_pThisGroup->m_TargetID.vPos;
		D3DXVec3Normalize ( &m_vInitDir, &m_vInitDir );

		////	���� �ϴ� ������ �����ؼ� ������ �����ش�.
		////	�� ���Ҵµ�. ��ġ�� �� �� �°�, ���� ��� ����� ���, ���� �ö󰡰�, ���� ���� �� ��� �������� ���Ѵ�.
		//m_vDir = DXPHYSICAL::COMPUTE_DIR_V ( m_vPos, m_pThisGroup->m_TargetID.vPos, m_fVelocity );

		//	���������� ������ ���� ����.. ��ƿø��� ���� ���Ѵ�.
		vDir = DXPHYSICAL::COMPUTE_DIR_H ( m_sData.m_vPos, m_pThisGroup->m_TargetID.vPos, 0.3f );
		fPower = DXPHYSICAL::COMPUTE_POWER ( m_sData.m_vPos, m_pThisGroup->m_TargetID.vPos, vDir );

		vDir = vDir * fPower;

		if ( m_dwFlag & USE_S_RANDOM )
		{
			vDir.x = vDir.x + (RANDOM_NUM*m_fRandom_Angel);
			vDir.y = vDir.y + (RANDOM_NUM*m_fRandom_Angel);
			vDir.z = vDir.z + (RANDOM_NUM*m_fRandom_Angel);
		}

		// ��ġ, �ӵ�, ������, ������, ź����
		m_sData.InitData ( m_sData.m_vPos, vDir, m_fObjectRadius, 0.8f, 0.8f );

		//	�ʱ� ��ġ�� ������ �������� �ٽ� �� ���� �� ������ �Ѵ�.
		m_bGetInitPosDir = TRUE;
	}

	if ( (m_dwFlag&USE_E_CRASH) || ( (m_dwFlag&USE_A_EFF)&&(m_dwFlag&USE_A_CRASH) ) )
	{
		// ��ǥ ��ġ�� ����ߴ��� üũ�ض�.
		if ( DXPHYSICAL::CHECK_PASS ( m_vInitDir, m_sData.m_vPos, m_pThisGroup->m_TargetID.vPos ) )
		{
			// ��ǥ ��ġ�� �����ϸ� ������.
			if ( m_dwFlag&USE_E_CRASH )	m_dwRunFlag |= EFF_PLY_RENDPASS;

			// ��ǥ ��ġ�� �����ϸ� �߰� Effect ����
			if ( (m_dwFlag&USE_A_EFF)&&(m_dwFlag&USE_A_CRASH) )
			{
				if ( m_pSingleGroup )		m_SingleRender = TRUE;
			}
		}
	}

	//	ó�� �
	if ( m_dwFlag&USE_S_GUIDED )	m_sData.Render ( m_pThisGroup->m_TargetID.vPos, m_fGuided_Accel );
	else							m_sData.Render ();

	//  �ְ������� �߰� ȿ�� ����
	if ( m_sData.IsCheckBestHigh() )
	{
		if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_HIGH) )		//�浹�� ���� �߰� Effect ����
		{
			if ( m_pSingleGroup )		m_SingleRender = TRUE;
		}
	}

	//	�浹
	if ( m_dwFlag & USE_GROUND_CRASH )		Crash(0);

	//	������ �� ���� ���� �۵�
	if ( m_sData.IsCheckDontMove() )
	{
		if (m_dwFlag&USE_E_DONTMOVE)	m_dwRunFlag |= EFF_PLY_RENDPASS;

		if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_DONTMOVE) )
		{
			if ( m_pSingleGroup )		m_SingleRender = TRUE;
		}
	}

	return TRUE;
}

// ��ǥ�� ��������..
void	DxEffectMoveTarget::Render_Direct ( D3DXMATRIX &matWorld )
{
	//	Note : �ʱ� ��ġ, ������ ����.
	if ( !m_bGetInitPosDir )
	{
		D3DXVECTOR3 vPos;
		vPos.x = matWorld._41;
		vPos.y = matWorld._42;
		vPos.z = matWorld._43;

		D3DXMATRIX	matRotate;
		matRotate._11 = m_pThisGroup->m_matWorld._11;
		matRotate._12 = m_pThisGroup->m_matWorld._12;
		matRotate._13 = m_pThisGroup->m_matWorld._13;
		matRotate._14 = m_pThisGroup->m_matWorld._14;

		matRotate._21 = m_pThisGroup->m_matWorld._21;
		matRotate._22 = m_pThisGroup->m_matWorld._22;
		matRotate._23 = m_pThisGroup->m_matWorld._23;
		matRotate._24 = m_pThisGroup->m_matWorld._24;

		matRotate._31 = m_pThisGroup->m_matWorld._31;
		matRotate._32 = m_pThisGroup->m_matWorld._32;
		matRotate._33 = m_pThisGroup->m_matWorld._33;
		matRotate._34 = m_pThisGroup->m_matWorld._34;

		matRotate._41 = 0.f;
		matRotate._42 = 0.f;
		matRotate._43 = 0.f;
		matRotate._44 = m_pThisGroup->m_matWorld._34;

		if ( m_dwFlag & USE_S_RANDOM )
		{
			m_vCustom_Velocity.x = m_vCustom_Velocity.x + (RANDOM_NUM*m_fRandom_Angel);
			m_vCustom_Velocity.y = m_vCustom_Velocity.y + (RANDOM_NUM*m_fRandom_Angel);
			m_vCustom_Velocity.z = m_vCustom_Velocity.z + (RANDOM_NUM*m_fRandom_Angel);
		}

		D3DXVec3TransformNormal ( &m_vCustom_Velocity, &m_vCustom_Velocity, &matRotate );	// ĳ���Ϳ� �°� ȸ�� ��Ų��.

		//D3DXVec3Normalize ( &m_vCustom_Dir, &m_vCustom_Dir );

		//m_vCustom_Dir = m_vCustom_Dir * m_fCustom_Vel;

		m_sData.InitData ( vPos, m_vCustom_Velocity, m_fObjectRadius, 0.8f, 0.8f );

		//	�ʱ� ��ġ�� ������ �������� �ٽ� �� ���� �� ������ �Ѵ�.
		m_bGetInitPosDir = TRUE;
	}

	//	ó�� �
	m_sData.Render ();

	//  �ְ������� �߰� ȿ�� ����
	if ( m_sData.IsCheckBestHigh() )
	{
		if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_HIGH) )		//�浹�� ���� �߰� Effect ����
		{
			if ( m_pSingleGroup )		m_SingleRender = TRUE;
		}
	}

	//	�浹
	if ( m_dwFlag & USE_GROUND_CRASH )		Crash (0);

	//	������ �� ���� ���� �۵�
	if ( m_sData.IsCheckDontMove() )
	{
		if (m_dwFlag&USE_E_DONTMOVE)	m_dwRunFlag |= EFF_PLY_RENDPASS;

		if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_DONTMOVE) )
		{
			if ( m_pSingleGroup )		m_SingleRender = TRUE;
		}
	}
}

void	DxEffectMoveTarget::Crash ( int nColl )
{
//	if ( m_sData.IsCheckDontMove() )	return;

	CDebugSet::ToView ( 10, "%f, %f, %f", m_sData.m_vPos.x, m_sData.m_vPos.y, m_sData.m_vPos.z );

	// ���ۿ� �շ��� ���Ϸ� ������ ���̴�.. ��
	if ( m_sData.m_vPos.y < -100000.f )
	{
		m_sData.SetDontMove();
		return;
	}

	DxLandMan*	pLandMan = DxEffSingleMan::GetInstance().GetLandMan();
	if ( pLandMan )
	{
		BOOL		bCollision;
		D3DXVECTOR3	vCollision;
		D3DXVECTOR3	vReflectNormal;
		LPDXFRAME	pDxFrame = NULL;

		D3DXVECTOR3 vPos_1  = m_sData.m_vPos;
		D3DXVECTOR3 vPos_2  = m_sData.m_vPos_Prev;
		D3DXVECTOR3 vDir	= vPos_1 - vPos_2;
		D3DXVec3Normalize ( &vDir, &vDir );
		D3DXVECTOR3 vVector = (vDir*m_sData.m_fRadius*10.f);	// ������ ������ �������� �� �ȳ��´�..
		vPos_1 += vVector;

		D3DXVECTOR3 vLeft, vRight;
		D3DXVECTOR3 vUp	= D3DXVECTOR3 ( 0.f, 1.f, 0.f );
		D3DXVec3Cross ( &vLeft, &vUp, &vDir );
		D3DXVec3Cross ( &vRight, &vLeft, &vDir );

		const char* szName = NULL;
		pLandMan->IsCollisionEX ( vPos_2, vPos_1, vCollision, bCollision, pDxFrame, szName, &vReflectNormal );

		if ( bCollision )
		{
			if ( m_sData.IsReflect ( vCollision, vReflectNormal, vVector, nColl ) )
			{
				if (m_dwFlag&USE_E_CRASH)	m_dwRunFlag |= EFF_PLY_RENDPASS;

				if ( (m_dwFlag&USE_A_EFF) && (m_dwFlag&USE_A_CRASH) )		//�浹�� ���� �߰� Effect ����
				{
					if ( m_pSingleGroup )		m_SingleRender = TRUE;
				}

				// ���� �浹�� �ִٸ� �� �ٽ� �浹�� ����� ���� üũ �ϱ� ����.
				++nColl;
				Crash (nColl);		// ���ȣ���̴�.  ��� �浹�� ����ٸ� ?? ���� �����ٵ�.��.
			}
		}
	}
}
