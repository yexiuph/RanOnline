#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"
#include "./EditMeshs.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectMoveRotate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note	:	����׿�,
extern BOOL			g_bEFFtoTEX;
extern BOOL			g_bEffectAABBBox;

//---------------------------------------------------------------------[MOVEROTATE_PROPERTY]
const DWORD	MOVEROTATE_PROPERTY::VERSION	= 0x0103;
const char	MOVEROTATE_PROPERTY::NAME[]	= "�̵�,ȸ�� ����";

DxEffSingle* MOVEROTATE_PROPERTY::NEWOBJ ()
{
	//DxEffectMoveRotate *pEffect = new DxEffectMoveRotate;
	DxEffectMoveRotate *pEffect = DxEffectMoveRotate::m_pPool->New();

	//	Note : TransPorm Set.		[���뼳��]3
	pEffect->m_matLocal = m_matLocal;

	//	Note : �ð� ����.			[���뼳��]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : ���ư��� ������Ʈ	[���뼳��]
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_vVelocity = m_vVelocity;

	pEffect->m_fRotateAngelX = m_fRotateAngelX;
	pEffect->m_fRotateAngelY = m_fRotateAngelY;
	pEffect->m_fRotateAngelZ = m_fRotateAngelZ;

	return pEffect;
}

HRESULT MOVEROTATE_PROPERTY::SaveFile ( CSerialFile &SFile )
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

HRESULT MOVEROTATE_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if ( dwVer == VERSION )
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
	else if( dwVer == 0x0102 )
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
	else if( dwVer == 0x0101 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		PROPERTY_101	pProp;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &pProp, sizeof(PROPERTY_101) );

		m_dwFlag	= 0L;							// Ver.102 ���� �߰�

		m_fRotateAngelX = pProp.m_fRotateAngelX;
		m_fRotateAngelY = pProp.m_fRotateAngelY;
		m_fRotateAngelZ = pProp.m_fRotateAngelZ;

		m_vVelocity = pProp.m_vVelocity;
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

		PROPERTY_100	pProp;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &pProp, sizeof(PROPERTY_100) );

		m_dwFlag	= 0L;							// Ver.102 ���� �߰�

		m_fRotateAngelX = pProp.m_fRotateAngelX;
		m_fRotateAngelY = pProp.m_fRotateAngelY;
		m_fRotateAngelZ = pProp.m_fRotateAngelZ;

		m_vVelocity = D3DXVECTOR3( 0.f, 0.f, 0.f );	// Ver.101 ���� �߰�
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

HRESULT MOVEROTATE_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT MOVEROTATE_PROPERTY::DeleteDeviceObjects ()
{
	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

const DWORD	DxEffectMoveRotate::TYPEID		= EFFSINGLE_MOVEROTATE;
const DWORD	DxEffectMoveRotate::FLAG		= NULL;
const char	DxEffectMoveRotate::NAME[]		= "�̵�,ȸ�� ����";

CMemPool<DxEffectMoveRotate> *DxEffectMoveRotate::m_pPool = NULL;

//	Note :
//
DxEffectMoveRotate::DxEffectMoveRotate(void) :
	m_vGLocation(0,0,0),
	m_fRand(0.f),

	m_fTransTime(0.f),
	m_fRotateTime(0.f),
	m_fRotateAngelXSum(0.0f),
	m_fRotateAngelYSum(0.0f),
	m_fRotateAngelZSum(0.0f),
	m_fRotateAngelX(0.0f),
	m_fRotateAngelY(0.0f),
	m_fRotateAngelZ(0.0f),

	m_fSettingX(0.0f),
	m_fSettingY(0.0f),
	m_fSettingZ(0.0f),

	m_pd3dDevice(NULL)
{
	D3DXMatrixIdentity( &m_matWorld );
}

DxEffectMoveRotate::~DxEffectMoveRotate(void)
{

}

HRESULT DxEffectMoveRotate::CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )	
{
	return S_OK; 
}
HRESULT DxEffectMoveRotate::ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }

void DxEffectMoveRotate::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectMoveRotate>;
}

void DxEffectMoveRotate::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
}

void DxEffectMoveRotate::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	D3DXVECTOR3 vPos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );

	if( vMax.x < vPos.x )	vMax.x = vPos.x;
	if( vMax.y < vPos.y )	vMax.y = vPos.y;
	if( vMax.z < vPos.z )	vMax.z = vPos.z;

	if( vMin.x > vPos.x )	vMin.x = vPos.x;
	if( vMin.y > vPos.y )	vMin.y = vPos.y;
	if( vMin.z > vPos.z )	vMin.z = vPos.z;

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectMoveRotate::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	m_fRand	= (rand()%36)*0.1f;

	m_fSettingX	= (float)(rand()%314)/100.f;
	m_fSettingY	= (float)(rand()%314)/100.f;
	m_fSettingZ	= (float)(rand()%314)/100.f;

	return S_OK;
}

HRESULT DxEffectMoveRotate::InvalidateDeviceObjects ()
{
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

void DxEffectMoveRotate::ReStartEff ()
{
	m_fGAge			= 0.f;
	m_fTransTime	= 0.f;
	m_dwRunFlag		&= ~EFF_PLY_PLAY;
	m_dwRunFlag		&= ~EFF_PLY_DONE;
	m_dwRunFlag		&= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectMoveRotate::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff�� ���̸� ���.
	//
	m_fGAge += fElapsedTime;

	//	Note : �ݺ��� �� �ð� �� ���� ���� �ʱ�ȭ �Ѵ�.
	//
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_pThisGroup->m_fGBeginTime+m_pThisGroup->m_fGLifeTime) )
		{
			m_fGAge			= 0.f;
			m_fTransTime	= 0.f;
			m_dwRunFlag		&= ~EFF_PLY_PLAY;
			m_dwRunFlag		&= ~EFF_PLY_DONE;
			m_dwRunFlag		&= ~EFF_PLY_END;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : ���۵��� �ʾҴ��� �˻�, ���� �ð��̸� Ȱ��ȭ ��Ŵ.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : ���� ������ PLAY �� �ɶ� ����ȴ�.
	//
	m_fTransTime	+= fElapsedTime;
	m_fRotateTime	+= fElapsedTime;

	//	Note : ���� ������ �Ǿ����� �˻�.
	//
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_END;

_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT	DxEffectMoveRotate::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld, matTemp, matRotate, matTrans, matChild;

	D3DXVECTOR3	vDelta(0.f,0.f,0.f);

	//matWorld = *m_pmatLocal;
	matTemp = m_matLocal;
	if ( m_dwFlag & USEGOTOCENTER )
	{
		float		fTime;
		fTime = m_fGBeginTime+m_fGLifeTime-m_fGAge;
		fTime = fTime / m_fGLifeTime;
		fTime = fTime * m_pThisGroup->m_fOut_Scale;

		matTemp._41 = m_matLocal._41*fTime;
		matTemp._42 = m_matLocal._42*fTime;
		matTemp._43 = m_matLocal._43*fTime;
		//matWorld._41 = m_matLocal._41*fTime;
		//matWorld._42 = m_matLocal._42*fTime;
		//matWorld._43 = m_matLocal._43*fTime;
	}
	D3DXMatrixMultiply( &matWorld, &matTemp, &matComb );

	
	if ( m_vVelocity.x || m_vVelocity.y || m_vVelocity.z )
	{
		D3DXVECTOR3	vVelocity;

		D3DXVec3TransformNormal ( &vVelocity, &m_vVelocity, &matWorld );

		matWorld._41 += vVelocity.x * m_fTransTime;
		matWorld._42 += vVelocity.y * m_fTransTime;
		matWorld._43 += vVelocity.z * m_fTransTime;
	}

	if ( m_fRotateAngelX )			//	Note : ȸ�� ����
	{
		D3DXMatrixRotationX ( &matRotate, m_fRotateTime*m_fRotateAngelX + m_fRand);
		//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
		D3DXMatrixMultiply_RM2( matRotate, matWorld );
	}
	if ( m_fRotateAngelY )			//	Note : ȸ�� ����
	{
		D3DXMatrixRotationY ( &matRotate, m_fRotateTime*m_fRotateAngelY + m_fRand);
		//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
		D3DXMatrixMultiply_RM2( matRotate, matWorld );
	}
	if ( m_fRotateAngelZ )			//	Note : ȸ�� ����
	{
		D3DXMatrixRotationZ ( &matRotate, m_fRotateTime*m_fRotateAngelZ + m_fRand);
		//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
		D3DXMatrixMultiply_RM2( matRotate, matWorld );
	}

	if ( m_dwFlag & USERANDROTATE )
	{
		D3DXMatrixRotationX ( &matRotate, m_fSettingX );
		//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
		D3DXMatrixMultiply_RM2( matRotate, matWorld );
		D3DXMatrixRotationY ( &matRotate, m_fSettingY );
		//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
		D3DXMatrixMultiply_RM2( matRotate, matWorld );
		D3DXMatrixRotationZ ( &matRotate, m_fSettingZ );
		//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
		D3DXMatrixMultiply_RM2( matRotate, matWorld );
	}

	// EFFECT TOOl ������ ��ġ�� ǥ�� ���� ��.
	if( g_bEffectAABBBox )
	{
		if( !g_bEFFtoTEX )
		{
			D3DXVECTOR3 vPos ( matWorld._41, matWorld._42, matWorld._43 );
			D3DXMatrixIdentity ( &matRotate );
			matRotate._11 = matWorld._11;
			matRotate._12 = matWorld._12;
			matRotate._13 = matWorld._13;
			matRotate._21 = matWorld._21;
			matRotate._22 = matWorld._22;
			matRotate._23 = matWorld._23;
			matRotate._31 = matWorld._31;
			matRotate._32 = matWorld._32;
			matRotate._33 = matWorld._33;
 			EDITMESHS::RENDERSPHERE ( pd3dDevice, vPos, 1.f, &matRotate );
		}
	}

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;


_RETURN:
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� ���.
	m_matWorld = matWorld;
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	return S_OK;
}

HRESULT DxEffectMoveRotate::FinalCleanup()
{
	//	Note : ���� �ڽ� ���.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}
