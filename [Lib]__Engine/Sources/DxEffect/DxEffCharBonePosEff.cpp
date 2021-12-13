#include "pch.h"

#include <string>
//#include <strstream>

#include "./DxInputDevice.h"
#include "./TextureManager.h"
#include "./SerialFile.h"
#include "./DxDynamicVB.h"

#include "./GLDefine.h"
#include "./EDITMESHS.h"
#include "./DxViewPort.h"

#include "./DxEffCharBonePosEff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern std::string	g_strPICK_BONE;
extern BOOL			g_bOBJECT100;
extern BOOL			g_bCHAR_EDIT_RUN;

DWORD		DxEffCharBonePosEff::TYPEID			= EMEFFCHAR_BONEPOSEFF;
DWORD		DxEffCharBonePosEff::VERSION		= 0x0100;
char		DxEffCharBonePosEff::NAME[MAX_PATH]	= "2.BonePosEff <1>"; 

DxEffCharBonePosEff::PARTICLEPOOL*	DxEffCharBonePosEff::m_pParticlePool = NULL;
VERTEXCOLORTEX2						DxEffCharBonePosEff::m_pVertices[MAX_FACE*4];

DxEffCharBonePosEff::DxEffCharBonePosEff(void) :
	DxEffChar(),
	m_fElapsedTime(0.f),
	m_nPrevKeyTime(-1),
	m_pParticleHEAD(NULL),
	m_pTexture(NULL),
	m_fParticleADD(0.f),
	m_fSizeDiff(0.f),
	m_fLifeDiff(0.f),
	m_fVelocityDiff(0.f),
	m_fRotateDiff(0.f),
	m_pBoneTran(NULL)
{
}

DxEffCharBonePosEff::~DxEffCharBonePosEff(void)
{
	CleanUp ();

	DXPARTICLE* pDel = NULL;
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->m_pNext;
		SAFE_DELETE( pDel );	// m_pParticlePool�� NULL�� �Ǿ� �־ ������ Delete�� �� �� �ۿ� ����.
	}
}

DxEffChar* DxEffCharBonePosEff::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharBonePosEff *pEffCharSingle = new DxEffCharBonePosEff;
	pEffCharSingle->SetLinkObj ( pCharPart, pSkinPiece );
	pEffCharSingle->SetProperty ( &m_Property );

	hr = pEffCharSingle->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffCharSingle);
		return NULL;
	}

	return pEffCharSingle;
}

HRESULT DxEffCharBonePosEff::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pParticlePool = new CMemPool<DXPARTICLE>;

	return S_OK;
}

HRESULT DxEffCharBonePosEff::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_DELETE( m_pParticlePool );

	return S_OK;
}

HRESULT DxEffCharBonePosEff::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffCharBonePosEff::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_fSizeDiff = m_vSize.y - m_vSize.x;
	m_fLifeDiff = m_vLife.y - m_vLife.x;
	m_fVelocityDiff = m_vVelocity.y - m_vVelocity.x;
	m_fRotateDiff = m_vRotate.y - m_vRotate.x;

	TextureManager::LoadTexture( m_szTexture, pd3dDevice, m_pTexture, 0, 0, FALSE );

	return S_OK;
}

HRESULT DxEffCharBonePosEff::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sPlayData.dwColorR = (m_dwColor&0xff0000)>>16;
	m_sPlayData.dwColorG = (m_dwColor&0xff00)>>8;
	m_sPlayData.dwColorB = m_dwColor&0xff;
	m_sPlayData.dwColorBASE = m_dwColor;

	if( m_dwFlag & USE_SEQUENCE )	m_sSequenceUV.CreateData( m_dwSequenceCol, m_dwSequenceRow );

	return S_OK;
}

HRESULT DxEffCharBonePosEff::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharBonePosEff::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	if( !m_pParticlePool )	return S_OK;

	DXPARTICLE* pDel(NULL);
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->m_pNext;
		m_pParticlePool->Release( pDel );
	}

	m_pParticleHEAD = NULL;

	return S_OK;
}

HRESULT DxEffCharBonePosEff::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxEffCharBonePosEff::FrameMove( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fElapsedTime = fElapsedTime;
	if( !m_pBoneTran )				return S_OK;
	if( m_fElapsedTime > 0.033f )	m_fElapsedTime = 0.033f;

	return S_OK;
}

void DxEffCharBonePosEff::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )
{
	if( g_bCHAR_EDIT_RUN )
	{
		if( m_pSkeleton && !g_bOBJECT100 )	
		{
			m_pSkeleton->CheckSphere( pd3dDevice, g_strPICK_BONE.c_str() );
		}
		m_pBoneTran = m_pSkeleton->FindBone( m_szBoneName );
	}
	else
	{
		if( !m_pBoneTran )
		{
			D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
			m_pBoneTran = m_pSkeleton->FindBone( m_szBoneName );
		}
	}

	if( !m_pBoneTran )	return;

	// Note : ���� �Ÿ��� �����.
	D3DXVECTOR3 vOffSetPos(0.f,0.f,0.f);
	if( m_dwFlag & USE_BONEOFFSETPOS )	vOffSetPos = m_vLocalPos;

	// Note : USE_REMAIN�� ����, �ൿ�� �ٲ���� ��� ����. �� �༮�� ������ǥ�� ��츸 �����Ѵٳ�.
	if( m_dwFlag & USE_REMAIN )
	{
		if( m_nPrevKeyTime != m_dwKeyTime )
		{
			ComputeCreatePos( vOffSetPos );
			m_nPrevKeyTime = m_dwKeyTime;
		}
	}

	// Note : ��Ʈ���� ��ȭ �� ������ ��ġ�� ��� ��ƼŬ�� �����̰� �����Ѵ�.
	CreateFrameMoveParticle( m_fElapsedTime );

	int			nCount(0);
	float		fSize(0.f);
	D3DXVECTOR3 vLocal(0.f,0.f,0.f);
	D3DXVECTOR3	vUp(0.f,1.f,0.f);

	// Note : ���������� üũ�Ѵ�.
	D3DXVECTOR2* pTexUV(NULL);
	D3DXVECTOR2 vTexUV00(0.f,0.f);
	D3DXVECTOR2 vTexUV10(0.f,0.f);
	D3DXVECTOR2 vTexUV01(0.f,0.f);
	D3DXVECTOR2 vTexUV11(0.f,0.f);
	float		fOffsetWidth(0.f);
	float		fOffsetHeight(0.f);
	if( m_dwFlag & USE_SEQUENCE )
	{
		fOffsetWidth = m_sSequenceUV.GetOffsetWidth();
		fOffsetHeight = m_sSequenceUV.GetOffsetHeight();
	}

	// Note : �����带 üũ�Ѵ�.
	D3DXVECTOR3 vBasePos(0.f,0.f,0.f);
	D3DXMATRIX	matLocal = DxBillboardLookAt( &vBasePos, DxViewPort::GetInstance().GetMatView() );

	float fHalfPI = D3DX_PI*0.5f;
	float fOneHalfPI = D3DX_PI*1.5f;

	// Note : ���� �� �Ѹ���.
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		if( nCount >= MAX_FACE )	break;

		// Note : ȸ���� ����� ���� �ƴ� ���
		if( m_dwFlag & USE_ROTATE )
		{
			// Note : ȸ���� �־���
			pCur->m_fRotate += pCur->m_fRotateVel*m_fElapsedTime;

			// Note : ������ ��ȭ�� ��
			fSize = pCur->m_fSize*fScale;

			// Note : ������ ����
			vLocal = D3DXVECTOR3( fSize*sinf(pCur->m_fRotate),				fSize*cosf(pCur->m_fRotate), 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+0].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize*sinf(pCur->m_fRotate+fHalfPI),		fSize*cosf(pCur->m_fRotate+fHalfPI), 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+1].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize*sinf(pCur->m_fRotate+fOneHalfPI),	fSize*cosf(pCur->m_fRotate+fOneHalfPI), 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+2].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize*sinf(pCur->m_fRotate+D3DX_PI),		fSize*cosf(pCur->m_fRotate+D3DX_PI), 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+3].vPos, &vLocal, &matLocal );
		}
		else
		{
			// Note : ������ ��ȭ�� ��
			fSize = pCur->m_fSize*fScale;

			// Note : ������ ����
			vLocal = D3DXVECTOR3( -fSize, fSize, 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+0].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize, fSize, 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+1].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( -fSize, -fSize, 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+2].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize, -fSize, 0.f );
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+3].vPos, &vLocal, &matLocal );
		}

		if( m_dwFlag&USE_REMAIN )
		{
			vBasePos = pCur->m_vPos;
		}
		else
		{
			vBasePos = pCur->m_vPos + vOffSetPos;
			D3DXVec3TransformCoord( &vBasePos, &vBasePos, &m_pBoneTran->matCombined );
		}

		m_pVertices[(nCount*4)+0].vPos += vBasePos;	// pCur->m_vPos �̳��� ȸ�� ���Ѿ� ���������� ���´�.
		m_pVertices[(nCount*4)+1].vPos += vBasePos;
		m_pVertices[(nCount*4)+2].vPos += vBasePos;
		m_pVertices[(nCount*4)+3].vPos += vBasePos;

		const DWORD& dwColor = pCur->m_dwColor;
		m_pVertices[(nCount*4)+0].dwColor = dwColor;
		m_pVertices[(nCount*4)+1].dwColor = dwColor;
		m_pVertices[(nCount*4)+2].dwColor = dwColor;
		m_pVertices[(nCount*4)+3].dwColor = dwColor;

		if( m_dwFlag & USE_SEQUENCE )
		{
			pTexUV = m_sSequenceUV.GetTexUV( m_sSequenceUV.GetTexCount(pCur->m_fRate) );
			if( !pTexUV )
			{
				pCur = pCur->m_pNext;	// ������ �����Ƿ� �������� �ʴ´�.
				continue;
			}
			vTexUV00 = vTexUV10 = vTexUV01 = vTexUV11 = *pTexUV;

			vTexUV10.x += fOffsetWidth;
			vTexUV01.y += fOffsetHeight;
			vTexUV11.x += fOffsetWidth;
			vTexUV11.y += fOffsetHeight;

			m_pVertices[(nCount*4)+0].vTex01 = vTexUV00;
			m_pVertices[(nCount*4)+1].vTex01 = vTexUV10;
			m_pVertices[(nCount*4)+2].vTex01 = vTexUV01;
			m_pVertices[(nCount*4)+3].vTex01 = vTexUV11;
		}
		else
		{
			m_pVertices[(nCount*4)+0].vTex01 = D3DXVECTOR2( 0.f, 0.f );
			m_pVertices[(nCount*4)+1].vTex01 = D3DXVECTOR2( 1.f, 0.f );
			m_pVertices[(nCount*4)+2].vTex01 = D3DXVECTOR2( 0.f, 1.f );
			m_pVertices[(nCount*4)+3].vTex01 = D3DXVECTOR2( 1.f, 1.f );
		}

		m_pVertices[(nCount*4)+0].vTex02 = D3DXVECTOR2( 0.f, 0.f );
		m_pVertices[(nCount*4)+1].vTex02 = D3DXVECTOR2( 1.f, 0.f );
		m_pVertices[(nCount*4)+2].vTex02 = D3DXVECTOR2( 0.f, 1.f );
		m_pVertices[(nCount*4)+3].vTex02 = D3DXVECTOR2( 1.f, 1.f );

		pCur = pCur->m_pNext;
		++nCount;
	}

	// Note : nCount �� ��ƼŬ ������ ��Ÿ����.
    if( nCount > 0 )
	{
		OPTMCharParticle::DATA* pData = OPTMManager::GetInstance().m_sCharParticle.GetData();

		// Note : Lock�� �ϱ� ���� D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
		DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
		DWORD dwVertexSizeFULL = nCount*sizeof(VERTEXCOLORTEX2)*4;
		if( DxDynamicVB::m_sVB.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB.nFullByte )
		{
			//CDebugSet::ToLogFile( "DxDynamicVB - Limit - Over" );
			OPTMManager::GetInstance().Render( pd3dDevice );

			dwFlag = D3DLOCK_DISCARD; 
			DxDynamicVB::m_sVB.nVertexCount = 0;
			DxDynamicVB::m_sVB.nOffsetToLock = 0; 
		}

		// Note : �����͸� �ִ´�.
		pData->pTex = m_pTexture;
		pData->nVertexCount = DxDynamicVB::m_sVB.nVertexCount;
		pData->nFaceCount = nCount;

		// Note : ������ ����
		VERTEXCOLORTEX2 *pVertices;
		DxDynamicVB::m_sVB.pVB->Lock( DxDynamicVB::m_sVB.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
		memcpy( pVertices, m_pVertices, dwVertexSizeFULL );
		DxDynamicVB::m_sVB.pVB->Unlock ();

		// Note : ���� ������. 
		DxDynamicVB::m_sVB.nVertexCount += nCount*4;
		DxDynamicVB::m_sVB.nOffsetToLock += dwVertexSizeFULL;

		// Note : ���߿� �Ѹ����� ���� �� ���´�.
		OPTMManager::GetInstance().m_sCharParticle.InsertData( pData );
	}
}

void DxEffCharBonePosEff::ComputeCreatePos( const D3DXVECTOR3& vOffSet )
{
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		// Note : ������ �� �Ǿ������� ���⼭ üũ
		pCur->ComputeCreatePos( &m_pBoneTran->matCombined, vOffSet );
		pCur = pCur->m_pNext;
	}
}

void DxEffCharBonePosEff::CreateFrameMoveParticle( float fElapsedTime )
{
	if( m_dwFlag & USE_REMAIN )
	{
		DXPARTICLE* pPrev = NULL;
		DXPARTICLE* pDel = NULL;
		DXPARTICLE* pCur = m_pParticleHEAD;
		while( pCur )
		{
			// Note : ������ �� �Ǿ������� ���⼭ üũ
			if( pCur->FrameMoveAbsolute( m_Property, m_sPlayData, m_fElapsedTime ) )	// ���� ����.
			{
				pPrev = pCur;	// ���� ��ġ ���
				pCur = pCur->m_pNext;
			}
			else if( pPrev )	// �߰� �༮�� ����	// ��� �ʿ� ����. pPrev�̱� ������.
			{
				pDel = pCur;
				pPrev->m_pNext = pCur->m_pNext;

				pCur = pCur->m_pNext;
				m_pParticlePool->Release( pDel );
			}
			else				// ó�� �༮�� ���� // ��� �ʿ� ����.
			{
				pDel = pCur;
				m_pParticleHEAD = pCur->m_pNext;

				pCur = pCur->m_pNext;
				m_pParticlePool->Release( pDel );
			}
		}
	}
	else
	{
		DXPARTICLE* pPrev = NULL;
		DXPARTICLE* pDel = NULL;
		DXPARTICLE* pCur = m_pParticleHEAD;
		while( pCur )
		{
			// Note : ������ �� �Ǿ������� ���⼭ üũ
			if( pCur->FrameMove( m_Property, m_sPlayData, m_fElapsedTime ))	// ���� ����.
			{
				pPrev = pCur;	// ���� ��ġ ���
				pCur = pCur->m_pNext;
			}
			else if( pPrev )	// �߰� �༮�� ����	// ��� �ʿ� ����. pPrev�̱� ������.
			{
				pDel = pCur;
				pPrev->m_pNext = pCur->m_pNext;

				pCur = pCur->m_pNext;
				m_pParticlePool->Release( pDel );
			}
			else				// ó�� �༮�� ���� // ��� �ʿ� ����.
			{
				pDel = pCur;
				m_pParticleHEAD = pCur->m_pNext;

				pCur = pCur->m_pNext;
				m_pParticlePool->Release( pDel );
			}
		}
	}

	m_fParticleADD += m_dwParticlePec*fElapsedTime;
	DWORD dwParticleCreate = (DWORD)m_fParticleADD;
	m_fParticleADD -= dwParticleCreate;

	DxCreateParticle( dwParticleCreate );
}

//----------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharBonePosEff::DxCreateParticle( DWORD dwNUM )
{
	if( m_dwFlag&USE_REMAIN )
	{
		while( dwNUM > 0 )
		{
			DXPARTICLE* pParticle = m_pParticlePool->New();
			pParticle->m_vDir = D3DXVECTOR3( RANDOM_NUM, RANDOM_NUM, RANDOM_NUM );
			pParticle->m_vPos = pParticle->m_vDir * m_fCreateLength;
			pParticle->m_fLife = m_vLife.x + m_fLifeDiff*RANDOM_POS;		// �����ϰ� ����
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_POS;		// �����ϰ� ����
			pParticle->m_fVel = m_vVelocity.x + m_fVelocityDiff*RANDOM_POS;	// �����ϰ� ����
			pParticle->m_fRotate = m_vRotate.y * RANDOM_POS;				// �����ϰ� ����
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff*RANDOM_POS;		// �����ϰ� ����

			pParticle->m_fSizeBase = pParticle->m_fSize;				// �⺻��
			pParticle->m_fVelBase = pParticle->m_fVel;					// �⺻��
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// ���� ��ȭ�� ����
			pParticle->m_fVelDiff = (pParticle->m_fVel*m_vVelocity.z) - pParticle->m_fVel;	// ���� ��ȭ�� ����

			pParticle->m_dwFlag = 0L;												// ������ ��츸.
			pParticle->m_fFirstPosRate = RANDOM_POS;								// ������ ��츸
			pParticle->m_vFirstPos = D3DXVECTOR3( 0.f, 0.f, 0.f );					// ������ ��츸

			dwNUM -= 1;		// ���� ����

			pParticle->m_pNext = m_pParticleHEAD;
			m_pParticleHEAD = pParticle;
		}
	}
	else
	{
		while( dwNUM > 0 )
		{
			DXPARTICLE* pParticle = m_pParticlePool->New();
			pParticle->m_vDir = D3DXVECTOR3( RANDOM_NUM, RANDOM_NUM, RANDOM_NUM );
			pParticle->m_vPos = pParticle->m_vDir * m_fCreateLength;
			pParticle->m_fLife = m_vLife.x + m_fLifeDiff*RANDOM_POS;		// �����ϰ� ����
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_POS;		// �����ϰ� ����
			pParticle->m_fVel = m_vVelocity.x + m_fVelocityDiff*RANDOM_POS;	// �����ϰ� ����
			pParticle->m_fRotate = m_vRotate.y * RANDOM_POS;				// �����ϰ� ����
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff*RANDOM_POS;		// �����ϰ� ����

			pParticle->m_fSizeBase = pParticle->m_fSize;				// �⺻��
			pParticle->m_fVelBase = pParticle->m_fVel;					// �⺻��
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// ���� ��ȭ�� ����
			pParticle->m_fVelDiff = (pParticle->m_fVel*m_vVelocity.z) - pParticle->m_fVel;	// ���� ��ȭ�� ����

			dwNUM -= 1;		// ���� ����

			pParticle->m_pNext = m_pParticleHEAD;
			m_pParticleHEAD = pParticle;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
BOOL DxEffCharBonePosEff::DXPARTICLE::FrameMove( EFFCHAR_PROPERTY_BONEPOSEFF& sProp, DXDATA& sData, const float fElpasedTime )
{
	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )	return FALSE;

	// Note : ������ �˾Ƴ�.
	m_fRate = m_fTime / m_fLife;

	// Note : ��ȭ���� ���� ��츸 ������, �ӵ��� �ٲ�
	if( sProp.m_vSize.z != 1.f )		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;
	if( sProp.m_vVelocity.z != 1.f )	m_fVel = m_fVelBase + m_fVelDiff*m_fRate;

	// Note : ��ġ�� ����
	m_vPos += m_vDir*m_fVel*fElpasedTime;

	// Note : ��ǻ�� ����.
	if( m_fRate < 0.2f )
	{
		const float fAlpha = m_fRate/0.2f;

		m_dwColor = 0xff000000;
		m_dwColor += (((DWORD)(sData.dwColorR*fAlpha))<<16);
		m_dwColor += (((DWORD)(sData.dwColorG*fAlpha))<<8);
		m_dwColor += (DWORD)(sData.dwColorB*fAlpha);
	}
	else if( m_fRate > 0.8f )
	{
		const float fAlpha = 1.f - ((m_fRate-0.8f)/0.2f);

		m_dwColor = 0xff000000;
		m_dwColor += (((DWORD)(sData.dwColorR*fAlpha))<<16);
		m_dwColor += (((DWORD)(sData.dwColorG*fAlpha))<<8);
		m_dwColor += (DWORD)(sData.dwColorB*fAlpha);
	}
	else
	{
		m_dwColor = sData.dwColorBASE;
	}

	return TRUE;
}

BOOL DxEffCharBonePosEff::DXPARTICLE::FrameMoveAbsolute( EFFCHAR_PROPERTY_BONEPOSEFF& sProp, DXDATA& sData, const float fElpasedTime ) 
{
	// Note : �÷��̰� ���� �� �Ǿ����� �������� �ʴ´�.
	if( !(m_dwFlag&USE_PLAY) )	return TRUE;

	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )	return FALSE;

	// Note : ������ �˾Ƴ�.
	m_fRate = m_fTime / m_fLife;

	// Note : ��ġ
	m_vPos += m_vDirDiff*m_fVel*fElpasedTime;

	// Note : ��ǻ�� ����.
	if( m_fRate < 0.2f )
	{
		const float fAlpha = m_fRate/0.2f;

		m_dwColor = 0xff000000;
		m_dwColor += (((DWORD)(sData.dwColorR*fAlpha))<<16);
		m_dwColor += (((DWORD)(sData.dwColorG*fAlpha))<<8);
		m_dwColor += (DWORD)(sData.dwColorB*fAlpha);
	}
	else if( m_fRate > 0.8f )
	{
		const float fAlpha = 1.f - ((m_fRate-0.8f)/0.2f);

		m_dwColor = 0xff000000;
		m_dwColor += (((DWORD)(sData.dwColorR*fAlpha))<<16);
		m_dwColor += (((DWORD)(sData.dwColorG*fAlpha))<<8);
		m_dwColor += (DWORD)(sData.dwColorB*fAlpha);
	}
	else
	{
		m_dwColor = sData.dwColorBASE;
	}

	return TRUE;
}

void DxEffCharBonePosEff::DXPARTICLE::ComputeCreatePos( LPD3DXMATRIX pMatrix, const D3DXVECTOR3& vOffSet )
{
	if( m_dwFlag&USE_PLAY )	return;

	D3DXVECTOR3 vPos_1(0.f,0.f,0.f);
	D3DXVECTOR3 vPos_2(0.f,0.f,0.f);

	if( m_dwFlag&USE_POS1 )		// �ι�° �ɸ���
	{
		// Note : �ι�° ��ġ�� ���Ѵ�.
		D3DXVECTOR3 vPos = m_vPos + vOffSet;
		D3DXVec3TransformCoord( &vPos, &vPos, pMatrix );

		// Note : 2�� ���̿����� ���ο� ���� ���Ѵ�.
		float fRate01 = RANDOM_POS;
		float fRate02 = 1.f-fRate01;
		m_vDirDiff = vPos - m_vFirstPos;
		m_vPos = (m_vFirstPos*fRate01) + (vPos*fRate02);

		m_dwFlag |= USE_PLAY;
	}
	else	// ó�� �ɸ���
	{
		D3DXVECTOR3 vPos = m_vPos + vOffSet;
		D3DXVec3TransformCoord( &m_vFirstPos, &vPos, pMatrix );

		m_dwFlag |= USE_POS1;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------
D3DXMATRIX DxEffCharBonePosEff::GetInfluenceMatrix()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	if( !m_pSkeleton )	return matWorld;
	if( !m_pBoneTran )
	{
		D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
		m_pBoneTran = m_pSkeleton->FindBone( m_szBoneName );

		if( !m_pBoneTran )	return matWorld;
	}

	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	D3DXVec3TransformCoord( &vPos, &m_vLocalPos, &m_pBoneTran->matCombined );

	matWorld = m_pBoneTran->matCombined;
	matWorld._41 = vPos.x;
	matWorld._42 = vPos.y;
	matWorld._43 = vPos.z;

	return matWorld;
}

D3DXVECTOR3 DxEffCharBonePosEff::SetChangeMatrix( const D3DXMATRIX& matWorld )
{
	if(	!m_pBoneTran )	return D3DXVECTOR3(0.f,0.f,0.f);

	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &m_pBoneTran->matCombined );
	D3DXMatrixMultiply( &matInverse, &matWorld, &matInverse );

	m_vLocalPos.x = matInverse._41;
	m_vLocalPos.y = matInverse._42;
	m_vLocalPos.z = matInverse._43;

	if( (m_vLocalPos.x>-0.001f) && (m_vLocalPos.x<0.001f) )		m_vLocalPos.x = 0.f;
	if( (m_vLocalPos.y>-0.001f) && (m_vLocalPos.y<0.001f) )		m_vLocalPos.y = 0.f;
	if( (m_vLocalPos.z>-0.001f) && (m_vLocalPos.z<0.001f) )		m_vLocalPos.z = 0.f;

	return m_vLocalPos;
}

HRESULT	DxEffCharBonePosEff::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if ( dwVer == VERSION )
	{
		DWORD dwSize = 0;
		D3DXVECTOR3 vPos(0.f,0.f,0.f);

		//	Note : ����Ʈ�� Property �� ����.
		SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

		//	Note : Device �ڿ��� �����Ѵ�.
		hr = Create( pd3dDevice );
		if ( FAILED(hr) )	return hr;

	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		SFile.SetOffSet( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharBonePosEff::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : ����Ʈ�� Property �� ����.
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}


