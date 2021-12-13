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

#include "./DxEffCharBoneListEff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern BOOL			g_bOBJECT100;
extern std::string	g_strPICK_BONE;
extern BOOL			g_bCHAR_EDIT_RUN;

DWORD		DxEffCharBoneListEff::TYPEID			= EMEFFCHAR_BONELISTEFF;
DWORD		DxEffCharBoneListEff::VERSION			= 0x0100;
char		DxEffCharBoneListEff::NAME[MAX_PATH]	= "2.BoneListEff <...>"; 

DxEffCharBoneListEff::PARTICLEPOOL*	DxEffCharBoneListEff::m_pParticlePool = NULL;
VERTEXCOLORTEX2						DxEffCharBoneListEff::m_pVertices[MAX_FACE*4];

DxEffCharBoneListEff::DxEffCharBoneListEff(void) :
	DxEffChar(),
	m_fElapsedTime(0.f),
	m_nPrevKeyTime(-1),
	m_pParticleHEAD(NULL),
	m_pTexture(NULL),
	m_fParticleADD(0.f),
	m_fSizeDiff(0.f),
	m_fLifeDiff(0.f),
	m_fRotateDiff(0.f),
	m_fVelocityDiff(0.f),
	m_pBoneTran(NULL)
{
}

DxEffCharBoneListEff::~DxEffCharBoneListEff(void)
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

	m_vecPosition.clear();
	m_listCustomBone.clear();
}

DxEffChar* DxEffCharBoneListEff::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharBoneListEff *pEffChar = new DxEffCharBoneListEff;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	// Note : pBoneTran�� ������ �ε� ������ �Ѵ�.. ����������. ��.
	//m_vecPosition.clear();
	std::copy( m_vecPosition.begin(), m_vecPosition.end(), std::back_inserter(pEffChar->m_vecPosition) );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharBoneListEff::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pParticlePool = new CMemPool<DXPARTICLE>;

	return S_OK;
}

HRESULT DxEffCharBoneListEff::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_DELETE( m_pParticlePool );

	return S_OK;
}

HRESULT DxEffCharBoneListEff::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffCharBoneListEff::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_fSizeDiff = m_vSize.y - m_vSize.x;
	m_fLifeDiff = m_vLife.y - m_vLife.x;
	m_fRotateDiff = m_vRotate.y - m_vRotate.x;
	m_fVelocityDiff = m_vVelocity.y - m_vVelocity.x;

	TextureManager::LoadTexture( m_szTexture, pd3dDevice, m_pTexture, 0, 0, FALSE );

	return S_OK;
}

HRESULT DxEffCharBoneListEff::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sPlayData.dwColorR = (m_dwColor&0xff0000)>>16;
	m_sPlayData.dwColorG = (m_dwColor&0xff00)>>8;
	m_sPlayData.dwColorB = m_dwColor&0xff;
	m_sPlayData.dwColorBASE = m_dwColor;

	if( m_dwFlag & USE_SEQUENCE )	m_sSequenceUV.CreateData( m_dwSequenceCol, m_dwSequenceRow );

	return S_OK;
}

HRESULT DxEffCharBoneListEff::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharBoneListEff::DeleteDeviceObjects ()
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

HRESULT DxEffCharBoneListEff::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxEffCharBoneListEff::FrameMove( float fTime, float fElapsedTime )
{
	m_fElapsedTime = fElapsedTime;

	if( !m_pSkeleton )				return S_OK;
	if( m_vecPosition.empty() )		return S_OK;
	if( m_fElapsedTime > 0.033f )	m_fElapsedTime = 0.033f;

	if( g_bCHAR_EDIT_RUN )
	{
		LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
		for( ; iter!=m_listCustomBone.end(); ++iter )
		{
			if( !(*iter).pBoneTran )
			{
				(*iter).pBoneTran = m_pSkeleton->FindBone( (*iter).strBoneName.c_str() );

				// Note : ���� ��� �ٽ� �ε� �ߴµ� �׷��� �ε��� �ȉ��� ��� �۾�����.
				if( !(*iter).pBoneTran )	return S_OK;
			}
		}
	}

	return S_OK;
}

void DxEffCharBoneListEff::CreateFrameMoveParticle( float fElapsedTime )
{
	if( m_vecPosition.empty() )	return;

	for( DWORD i=0; i<m_vecPosition.size(); ++i )
	{
		if( !m_vecPosition[i].pBoneTran )
		{
			m_vecPosition[i].pBoneTran = m_pSkeleton->FindBone( m_vecPosition[i].strBoneName.c_str() );

			// Note : ���� ��� �ٽ� �ε� �ߴµ� �׷��� �ε��� �ȉ��� ��� �۾�����.
			if( !m_vecPosition[i].pBoneTran )	return;
		}
	}

	// Note : BoneMatrix�� ���Ѵٸ� �ű⿡ ���õ� ��ġ��ǥ�� ���ؾ� �Ѵ�.
	UpdateVECPOSION();

	if( m_dwFlag & USE_REMAIN )
	{
		DXPARTICLE* pPrev = NULL;
		DXPARTICLE* pDel = NULL;
		DXPARTICLE* pCur = m_pParticleHEAD;
		while( pCur )
		{
			// Note : ������ �� �Ǿ������� ���⼭ üũ
			if( pCur->FrameMoveAbsolute( m_Property, m_sPlayData, m_vecPosition, fElapsedTime ) )	// ���� ����.
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
			if( pCur->FrameMove( m_Property, m_sPlayData, m_vecPosition, fElapsedTime ) )	// ���� ����.
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

void DxEffCharBoneListEff::ComputeCreatePos()
{
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		// Note : ������ �� �Ǿ������� ���⼭ üũ
		pCur->ComputeCreatePos( m_vecPosition );
		pCur = pCur->m_pNext;
	}
}

void DxEffCharBoneListEff::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )
{
	// Note : USE_REMAIN�� ����, �ൿ�� �ٲ���� ��� ����. �� �༮�� ������ǥ�� ��츸 �����Ѵٳ�.
	if( m_dwFlag & USE_REMAIN )
	{
		if( m_nPrevKeyTime != m_dwKeyTime )
		{
			ComputeCreatePos();
			m_nPrevKeyTime = m_dwKeyTime;
		}
	}

	// Note : ��Ʈ���� ��ȭ �� ������ ��ġ�� ��� ��ƼŬ�� �����̰� �����Ѵ�.
	CreateFrameMoveParticle( m_fElapsedTime );

	// Note : ĳ���� Tool�� ��츸 ���.
	if( g_bCHAR_EDIT_RUN )
	{
		m_pBoneTran = m_pSkeleton->FindBone( g_strPICK_BONE.c_str() );

		if( !g_bOBJECT100 )
		{
			if( m_pSkeleton )	m_pSkeleton->CheckSphere( pd3dDevice, g_strPICK_BONE.c_str() );

			// Note : ������ ǥ���� �ش�.
			int nBoneCount(0);
			LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
			for( ; iter!=m_listCustomBone.end(); ++iter, ++nBoneCount )
			{
				if( !(*iter).pBoneTran )	return;

				// Note : Render
				(*iter).RenderCount( nBoneCount, (*iter).pBoneTran->matCombined );
			}
		}
	}

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

		vBasePos = pCur->m_vPos;

		m_pVertices[(nCount*4)+0].vPos += vBasePos;	// pCur->m_vPos �̳��� ȸ�� ���Ѿ� ���������� ���´�.
		m_pVertices[(nCount*4)+1].vPos += vBasePos;
		m_pVertices[(nCount*4)+2].vPos += vBasePos;
		m_pVertices[(nCount*4)+3].vPos += vBasePos;

		const DWORD& dwColor = pCur->m_dwColor;
		m_pVertices[(nCount*4)+0].dwColor = dwColor;
		m_pVertices[(nCount*4)+1].dwColor = dwColor;
		m_pVertices[(nCount*4)+2].dwColor = dwColor;
		m_pVertices[(nCount*4)+3].dwColor = dwColor;

		// Note : �������� ����� ���� �ƴ� ���
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

//----------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharBoneListEff::DxCreateParticle( DWORD dwNUM )
{
	D3DXVECTOR3 vDir(0.f,0.f,0.f);

	if( m_dwFlag&USE_REMAIN )
	{
		while( dwNUM > 0 )
		{
			DXPARTICLE* pParticle = m_pParticlePool->New();
			pParticle->m_fLife = m_vLife.x + m_fLifeDiff*RANDOM_POS;		// �����ϰ� ����
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_POS;		// �����ϰ� ����
			pParticle->m_fRotate = m_vRotate.y * RANDOM_POS;				// �����ϰ� ����
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff*RANDOM_POS;		// �����ϰ� ����

			vDir = D3DXVECTOR3( RANDOM_NUM, RANDOM_NUM, RANDOM_NUM );
			pParticle->m_vPosOffSet =  vDir*m_fCreateLength;			// ������ �Ÿ��� ��� ���´�.

			pParticle->m_fSizeBase = pParticle->m_fSize;				// �⺻��
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// ���� ��ȭ�� ����

			pParticle->m_dwFlag = 0L;												// ������ ��츸.
			pParticle->m_fVelocity = m_vVelocity.x + m_fVelocityDiff*RANDOM_POS;	// ������ ��츸.
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
			pParticle->m_fLife = m_vLife.x + m_fLifeDiff*RANDOM_POS;		// �����ϰ� ����
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_POS;		// �����ϰ� ����
			pParticle->m_fRotate = m_vRotate.y * RANDOM_POS;				// �����ϰ� ����
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff*RANDOM_POS;		// �����ϰ� ����

			vDir = D3DXVECTOR3( RANDOM_NUM, RANDOM_NUM, RANDOM_NUM );
			pParticle->m_vPosOffSet =  vDir*m_fCreateLength;			// ������ �Ÿ��� ��� ���´�.

			pParticle->m_fSizeBase = pParticle->m_fSize;				// �⺻��
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// ���� ��ȭ�� ����

			dwNUM -= 1;		// ���� ����

			pParticle->m_pNext = m_pParticleHEAD;
			m_pParticleHEAD = pParticle;
		}
	}
}

void DxEffCharBoneListEff::UpdateVECPOSION()
{
	for( DWORD i=0; i<m_vecPosition.size(); ++i )
	{
		if( m_vecPosition[i].bVertex )
		{
			D3DXVec3TransformCoord( &m_vecPosition[i].vPos, &m_vecPosition[i].vBase, &m_vecPosition[i].pBoneTran->matCombined );
		}
		else
		{
			m_vecPosition[i].vPos.x = m_vecPosition[i].pBoneTran->matCombined._41;
			m_vecPosition[i].vPos.y = m_vecPosition[i].pBoneTran->matCombined._42;
			m_vecPosition[i].vPos.z = m_vecPosition[i].pBoneTran->matCombined._43;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
BOOL DxEffCharBoneListEff::DXPARTICLE::FrameMove( EFFCHAR_PROPERTY_BONELISTEFF& sProp, DXDATA& sData, VECPOSITION& vecData, const float fElpasedTime )
{
	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )	return FALSE;

	// Note : ������ �˾Ƴ�.
	m_fRate = m_fTime / m_fLife;

	// Note : ��ġ ����
	for( DWORD i=1; i<vecData.size(); ++i )
	{
		if( (m_fRate >= vecData[i-1].fRate) && (m_fRate < vecData[i].fRate) )
		{
			float fDisRate = 1.f / (vecData[i].fRate - vecData[i-1].fRate);
			D3DXVECTOR3 vPos_1 = vecData[i-1].vPos*( (vecData[i].fRate-m_fRate)*fDisRate );
			D3DXVECTOR3 vPos_2 = vecData[i].vPos*( (m_fRate-vecData[i-1].fRate)*fDisRate );

			m_vPos = vPos_1 + vPos_2 + m_vPosOffSet;
			break;
		}
	}

	// Note : ��ȭ���� ���� ��츸 ����� �ٲ�
	if( sProp.m_vSize.z != 1.f )		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;

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

BOOL DxEffCharBoneListEff::DXPARTICLE::FrameMoveAbsolute( EFFCHAR_PROPERTY_BONELISTEFF& sProp, DXDATA& sData, VECPOSITION& vecData, const float fElpasedTime )
{
	// Note : �÷��̰� ���� �� �Ǿ����� �������� �ʴ´�.
	if( !(m_dwFlag&USE_PLAY) )	return TRUE;

	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )	return FALSE;

	// Note : ������ �˾Ƴ�.
	m_fRate = m_fTime / m_fLife;

	// Note : ��ġ
	m_vPos += m_vDirDiff*m_fVelocity*fElpasedTime;

	// Note : ��ȭ���� ���� ��츸 ����� �ٲ�
	if( sProp.m_vSize.z != 1.f )		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;

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

void DxEffCharBoneListEff::DXPARTICLE::ComputeCreatePos( VECPOSITION& vecData )
{
	if( m_dwFlag&USE_PLAY )	return;

	if( m_dwFlag&USE_POS1 )		// �ι�° �ɸ���
	{
		// Note : �ι�° ��ġ�� ���Ѵ�.
		D3DXVECTOR3 vPos(0.f,0.f,0.f);
		for( DWORD i=1; i<vecData.size(); ++i )
		{
			if( (m_fFirstPosRate >= vecData[i-1].fRate) && (m_fFirstPosRate < vecData[i].fRate) )
			{
				float fDisRate = 1.f / (vecData[i].fRate - vecData[i-1].fRate);
				D3DXVECTOR3 vPos_1 = vecData[i-1].vPos*( (vecData[i].fRate-m_fFirstPosRate)*fDisRate );
				D3DXVECTOR3 vPos_2 = vecData[i].vPos*( (m_fFirstPosRate-vecData[i-1].fRate)*fDisRate );

				vPos = vPos_1 + vPos_2 + m_vPosOffSet;
			}
		}

		// Note : 2�� ���̿����� ���ο� ���� ���Ѵ�.
		float fRate01 = RANDOM_POS;
		float fRate02 = 1.f-fRate01;
		m_vDirDiff = vPos - m_vFirstPos;
		m_vPos = (m_vFirstPos*fRate01) + (vPos*fRate02);

		m_dwFlag |= USE_PLAY;
	}
	else	// ó�� �ɸ���
	{
		// Note : ó�� ��ġ�� ���Ѵ�.
		for( DWORD i=1; i<vecData.size(); ++i )
		{
			if( (m_fFirstPosRate >= vecData[i-1].fRate) && (m_fFirstPosRate < vecData[i].fRate) )
			{
				float fDisRate = 1.f / (vecData[i].fRate - vecData[i-1].fRate);
				D3DXVECTOR3 vPos_1 = vecData[i-1].vPos*( (vecData[i].fRate-m_fFirstPosRate)*fDisRate );
				D3DXVECTOR3 vPos_2 = vecData[i].vPos*( (m_fFirstPosRate-vecData[i-1].fRate)*fDisRate );

				m_vFirstPos = vPos_1 + vPos_2 + m_vPosOffSet;

				m_dwFlag |= USE_POS1;
				return;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharBoneListEff::InsertBone( const char* szName )
{
	// Note : ���̷����� ���ٸ� ����
	if( !m_pSkeleton )	return;

	DXCUSTOMBONE	sCBone;
	sCBone.strBoneName = szName;
	sCBone.pBoneTran = m_pSkeleton->FindBone( szName );

	// Note : ���� ���ٸ� ����
	if( !sCBone.pBoneTran )	return;

	m_listCustomBone.push_back( sCBone );
}

BOOL DxEffCharBoneListEff::DeleteBone( const DWORD dwIndex )
{
	if( dwIndex >= m_listCustomBone.size() )	return FALSE;

	int nCount = 0;
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			m_listCustomBone.erase( iter );
			return TRUE;
		}
	}

	return TRUE;
}

int DxEffCharBoneListEff::DataUp( DWORD dwIndex )
{
	if( dwIndex == 0 )	return dwIndex;

	DXCUSTOMBONE vTemp;
	int nCount(0);
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			vTemp = (*iter);
			m_listCustomBone.erase( iter );
			break;
		}
	}

	nCount = 0;
	dwIndex -= 1;
	if( dwIndex==0 )
	{
		m_listCustomBone.push_front( vTemp );
		return nCount;
	}

	iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			m_listCustomBone.insert( iter, vTemp );
			return nCount;
		}
	}

	return 0;
}

int DxEffCharBoneListEff::DataDown( DWORD dwIndex )
{
	if( dwIndex >= (m_listCustomBone.size()-1) )	return dwIndex;

	DXCUSTOMBONE vTemp;
	int nCount(0);
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			vTemp = (*iter);
			m_listCustomBone.erase( iter );
			break;
		}
	}

	nCount = 0;
	dwIndex += 1;
	iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			m_listCustomBone.insert( iter, vTemp );
			return nCount;
		}
	}

	// Note : �� ���������� ������.
	m_listCustomBone.push_back( vTemp );

	return nCount;
}

std::string DxEffCharBoneListEff::GetBoneString( const DWORD dwIndex )
{
	if( dwIndex >= m_listCustomBone.size() )	return std::string("");

	int nCount = 0;
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )
		{
			TCHAR szTemp[256] = {0};
			_snprintf_s( szTemp, 256, "%s, %f, %f, %f", 
					(*iter).strBoneName.c_str(),
					(*iter).vVertex.x,
					(*iter).vVertex.y,
					(*iter).vVertex.z );

			//std::strstream	strStream;
			//strStream << (*iter).strBoneName.c_str() << ", " << (*iter).vVertex.x << ", " << (*iter).vVertex.y << ", " << (*iter).vVertex.z << std::ends;

			//std::string strName();
			//strStream.freeze( false );

			return std::string(szTemp);
		}
	}

	return std::string("");
}

void DxEffCharBoneListEff::SetCheckBone( const DWORD dwIndex )
{
	if( dwIndex >= m_listCustomBone.size() )	return;

	int nCount = 0;
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter, ++nCount )
	{
		if( nCount==dwIndex )	
		{
			g_strPICK_BONE = (*iter).strBoneName.c_str();
			return;
		}
	}
}

BOOL DxEffCharBoneListEff::GetEnableBonePos()
{
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter )
	{
		if( g_strPICK_BONE == (*iter).strBoneName.c_str() )	return (*iter).bVertex;
	}

	return FALSE;
}

D3DXVECTOR3 DxEffCharBoneListEff::GetInfluencePosition()
{
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter )
	{
		if( g_strPICK_BONE == (*iter).strBoneName.c_str() )	return (*iter).vVertex;
	}

	return D3DXVECTOR3(0.f,0.f,0.f);
}

D3DXMATRIX DxEffCharBoneListEff::GetInfluenceMatrix()
{
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter )
	{
		if( g_strPICK_BONE == (*iter).strBoneName.c_str() )
		{
			if( (*iter).pBoneTran )	
			{
				if( (*iter).bVertex )
				{
					D3DXVECTOR3  vPos(0.f,0.f,0.f);
					D3DXVec3TransformCoord( &vPos, &(*iter).vVertex, &(*iter).pBoneTran->matCombined );

					D3DXMATRIX matWorld;
					matWorld = (*iter).pBoneTran->matCombined;
					matWorld._41 = vPos.x;
					matWorld._42 = vPos.y;
					matWorld._43 = vPos.z;

					return matWorld;
				}
				else
				{
					return (*iter).pBoneTran->matCombined;
				}
			}
		}
	}

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	return matIdentity;
}

D3DXVECTOR3 DxEffCharBoneListEff::SetChangeMatrix( const D3DXMATRIX& matWorld )
{
	D3DXVECTOR3* pLocal(NULL);		// �ּҰ��� �޾Ƽ� ����Ѵ�.
	DxBoneTrans* pBoneTran(NULL);
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter )
	{
		if( g_strPICK_BONE == (*iter).strBoneName.c_str() )
		{
			pLocal = &(*iter).vVertex;
			pBoneTran = (*iter).pBoneTran;
		}
	}

	
	if(	!pBoneTran || !pLocal )		return D3DXVECTOR3(0.f,0.f,0.f);

	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &pBoneTran->matCombined );
	D3DXMatrixMultiply( &matInverse, &matWorld, &matInverse );

	pLocal->x = matInverse._41;
	pLocal->y = matInverse._42;
	pLocal->z = matInverse._43;

	if( (pLocal->x>-0.001f) && (pLocal->x<0.001f) )		pLocal->x = 0.f;
	if( (pLocal->y>-0.001f) && (pLocal->y<0.001f) )		pLocal->y = 0.f;
	if( (pLocal->z>-0.001f) && (pLocal->z<0.001f) )		pLocal->z = 0.f;

	return (*pLocal);
}

void DxEffCharBoneListEff::SetCheckBoneOffSetPos( BOOL bEnable )
{
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter )
	{
		if( g_strPICK_BONE == (*iter).strBoneName.c_str() )
		{
			(*iter).bVertex = bEnable;
			return;
		}
	}
}

BOOL DxEffCharBoneListEff::EditApplyResetData()
{
	// Note : ���� ������ �Ѵ�.
	m_vecPosition.clear();

	if( m_listCustomBone.size() < 2 )	return FALSE;

	// Note : �ϴ� ����
	int nCount = 0;
	float fDisADD = 0.f;
	D3DXVECTOR3 vPos_CUR(0.f,0.f,0.f);
	D3DXVECTOR3 vDiff(0.f,0.f,0.f);
	LISTCUSTOMBONE_ITER iterPREV;
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter, ++nCount )
	{
		if( nCount==0 )
		{
			DXPOSDELTA sData;
			sData.strBoneName = (*iter).strBoneName;
			sData.vBase = (*iter).vVertex;
			sData.bVertex = (*iter).bVertex;
			sData.fRate = 0.f;
			m_vecPosition.push_back( sData );
		}
		else
		{
			D3DXVECTOR3 vPos1, vPos2;
			D3DXVec3TransformCoord( &vPos1, &(*iter).vVertex, &(*iter).pBoneTran->matCombined );
			D3DXVec3TransformCoord( &vPos2, &(*iterPREV).vVertex, &(*iterPREV).pBoneTran->matCombined );
			vDiff = vPos1 - vPos2;
			float fDis = D3DXVec3Length( &vDiff );
			fDisADD += fDis;

			DXPOSDELTA sData;
			sData.strBoneName = (*iter).strBoneName;
			sData.vBase = (*iter).vVertex;
			sData.bVertex = (*iter).bVertex;
			sData.fRate = fDisADD;
			m_vecPosition.push_back( sData );
		}

		iterPREV = iter;
		vPos_CUR = (*iter).vVertex;
	}

	// Note : ������ �����.
	for( DWORD i=0; i<m_vecPosition.size(); ++i )
	{
		m_vecPosition[i].fRate /= fDisADD;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------------------------
HRESULT	DxEffCharBoneListEff::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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

		//	Note : ����Ʈ�� Property �� ����.
		SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

		// Note : Edit�� �ϱ� ���� Data
		SFile >> dwSize;
		m_listCustomBone.clear();
		for( DWORD i=0; i<dwSize; ++i )
		{
			DXCUSTOMBONE	sData;
			sData.Load( SFile );
			m_listCustomBone.push_back( sData );
		}

		// Note : ���� ����ϱ� ���� Data
		SFile >> dwSize;
		m_vecPosition.clear();
		for( DWORD i=0; i<dwSize; ++i )
		{
			DXPOSDELTA sPosDelta;
			sPosDelta.Load( SFile );
			m_vecPosition.push_back( sPosDelta );
		}

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

HRESULT	DxEffCharBoneListEff::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : ����Ʈ�� Property �� ����.
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	// Note : Edit�� �ϱ� ���� Data
	SFile << (DWORD)m_listCustomBone.size();
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter )
	{
		(*iter).Save( SFile );
	}

	// Note : ���� ����ϱ� ���� Data
	SFile << (DWORD)m_vecPosition.size();
	for( DWORD i=0; i<m_vecPosition.size(); ++i )
	{
		m_vecPosition[i].Save( SFile );
	}

	return S_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
//									D	X		P	O	S		D	E	L	T	A	< Save, Load >
//--------------------------------------------------------------------------------------------------------------------------
void DxEffCharBoneListEff::DXPOSDELTA::Save( basestream& SFile )
{
	SFile << strBoneName;
	SFile << bVertex;
	SFile << vBase;
	SFile << fRate;
}

void DxEffCharBoneListEff::DXPOSDELTA::Load( basestream& SFile )
{
	SFile >> strBoneName;
	SFile >> bVertex;
	SFile >> vBase;
	SFile >> fRate;
}

//--------------------------------------------------------------------------------------------------------------------------
//								D	X		C	U	S	T	O	M		B	O	N	E	< Save, Load >
//--------------------------------------------------------------------------------------------------------------------------
void DXCUSTOMBONE::RenderCount( DWORD nCount, const D3DXMATRIX& matWorld )
{
	D3DXVECTOR3 vSrc;
	D3DXVec3TransformCoord( &vSrc, &vVertex, &matWorld );

	D3DXVECTOR3 vOut;
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	D3DXVec3Project( &vOut, &vSrc, &DxViewPort::GetInstance().GetViewPort(), &DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(), &matIdentity );

	//std::strstream strStream;
	//strStream << nCount << std::ends;
	CDebugSet::ToPos( vOut.x, vOut.y, "%u", nCount );

	//strStream.freeze( false );
}

void DXCUSTOMBONE::Save( basestream& SFile )
{
	SFile << strBoneName;
	SFile << bVertex;
	SFile << vVertex;
}

void DXCUSTOMBONE::Load( basestream& SFile )
{
	SFile >> strBoneName;
	SFile >> bVertex;
	SFile >> vVertex;
}




