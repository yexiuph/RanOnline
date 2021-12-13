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
		SAFE_DELETE( pDel );	// m_pParticlePool은 NULL로 되어 있어서 스스로 Delete를 할 수 밖에 없다.
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

	// Note : pBoneTran이 포인터 인데 공유를 한다.. 괜찮을런지. 쩝.
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

				// Note : 본이 없어서 다시 로드 했는데 그래도 로딩이 안됬을 경우 작업안함.
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

			// Note : 본이 없어서 다시 로드 했는데 그래도 로딩이 안됬을 경우 작업안함.
			if( !m_vecPosition[i].pBoneTran )	return;
		}
	}

	// Note : BoneMatrix가 변한다면 거기에 셋팅된 위치좌표도 변해야 한다.
	UpdateVECPOSION();

	if( m_dwFlag & USE_REMAIN )
	{
		DXPARTICLE* pPrev = NULL;
		DXPARTICLE* pDel = NULL;
		DXPARTICLE* pCur = m_pParticleHEAD;
		while( pCur )
		{
			// Note : 생명이 다 되었는지도 여기서 체크
			if( pCur->FrameMoveAbsolute( m_Property, m_sPlayData, m_vecPosition, fElapsedTime ) )	// 삭제 없음.
			{
				pPrev = pCur;	// 이전 위치 백업
				pCur = pCur->m_pNext;
			}
			else if( pPrev )	// 중간 녀석을 삭제	// 백업 필요 없다. pPrev이기 때문에.
			{
				pDel = pCur;
				pPrev->m_pNext = pCur->m_pNext;

				pCur = pCur->m_pNext;
				m_pParticlePool->Release( pDel );
			}
			else				// 처음 녀석을 삭제 // 백업 필요 없음.
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
			// Note : 생명이 다 되었는지도 여기서 체크
			if( pCur->FrameMove( m_Property, m_sPlayData, m_vecPosition, fElapsedTime ) )	// 삭제 없음.
			{
				pPrev = pCur;	// 이전 위치 백업
				pCur = pCur->m_pNext;
			}
			else if( pPrev )	// 중간 녀석을 삭제	// 백업 필요 없다. pPrev이기 때문에.
			{
				pDel = pCur;
				pPrev->m_pNext = pCur->m_pNext;

				pCur = pCur->m_pNext;
				m_pParticlePool->Release( pDel );
			}
			else				// 처음 녀석을 삭제 // 백업 필요 없음.
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
		// Note : 생명이 다 되었는지도 여기서 체크
		pCur->ComputeCreatePos( m_vecPosition );
		pCur = pCur->m_pNext;
	}
}

void DxEffCharBoneListEff::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )
{
	// Note : USE_REMAIN일 때만, 행동이 바뀌었을 경우 동작. 이 녀석은 절대좌표일 경우만 동작한다네.
	if( m_dwFlag & USE_REMAIN )
	{
		if( m_nPrevKeyTime != m_dwKeyTime )
		{
			ComputeCreatePos();
			m_nPrevKeyTime = m_dwKeyTime;
		}
	}

	// Note : 매트릭스 변화 된 것으로 위치를 얻고 파티클을 움직이고 생성한다.
	CreateFrameMoveParticle( m_fElapsedTime );

	// Note : 캐릭터 Tool일 경우만 사용.
	if( g_bCHAR_EDIT_RUN )
	{
		m_pBoneTran = m_pSkeleton->FindBone( g_strPICK_BONE.c_str() );

		if( !g_bOBJECT100 )
		{
			if( m_pSkeleton )	m_pSkeleton->CheckSphere( pd3dDevice, g_strPICK_BONE.c_str() );

			// Note : 순서를 표시해 준다.
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

	// Note : 시퀀스인지 체크한다.
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

	// Note : 빌보드를 체크한다.
	D3DXVECTOR3 vBasePos(0.f,0.f,0.f);
	D3DXMATRIX	matLocal = DxBillboardLookAt( &vBasePos, DxViewPort::GetInstance().GetMatView() );

	float fHalfPI = D3DX_PI*0.5f;
	float fOneHalfPI = D3DX_PI*1.5f;

	// Note : 만든 후 뿌린다.
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		if( nCount >= MAX_FACE )	break;

		// Note : 회전을 사용할 경우와 아닐 경우
		if( m_dwFlag & USE_ROTATE )
		{
			// Note : 회전값 넣어줌
			pCur->m_fRotate += pCur->m_fRotateVel*m_fElapsedTime;

			// Note : 사이즈 변화를 줌
			fSize = pCur->m_fSize*fScale;

			// Note : 점들의 적용
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
			// Note : 사이즈 변화를 줌
			fSize = pCur->m_fSize*fScale;

			// Note : 점들의 적용
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

		m_pVertices[(nCount*4)+0].vPos += vBasePos;	// pCur->m_vPos 이넘을 회전 시켜야 정상적으로 나온다.
		m_pVertices[(nCount*4)+1].vPos += vBasePos;
		m_pVertices[(nCount*4)+2].vPos += vBasePos;
		m_pVertices[(nCount*4)+3].vPos += vBasePos;

		const DWORD& dwColor = pCur->m_dwColor;
		m_pVertices[(nCount*4)+0].dwColor = dwColor;
		m_pVertices[(nCount*4)+1].dwColor = dwColor;
		m_pVertices[(nCount*4)+2].dwColor = dwColor;
		m_pVertices[(nCount*4)+3].dwColor = dwColor;

		// Note : 시퀀스를 사용할 경우와 아닐 경우
		if( m_dwFlag & USE_SEQUENCE )
		{
			pTexUV = m_sSequenceUV.GetTexUV( m_sSequenceUV.GetTexCount(pCur->m_fRate) );
			if( !pTexUV )
			{
				pCur = pCur->m_pNext;	// 문제가 있으므로 생성하지 않는다.
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

	// Note : nCount 가 파티클 갯수를 나타낸다.
    if( nCount > 0 )
	{
		OPTMCharParticle::DATA* pData = OPTMManager::GetInstance().m_sCharParticle.GetData();

		// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
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

		// Note : 데이터를 넣는다.
		pData->pTex = m_pTexture;
		pData->nVertexCount = DxDynamicVB::m_sVB.nVertexCount;
		pData->nFaceCount = nCount;

		// Note : 데이터 복사
		VERTEXCOLORTEX2 *pVertices;
		DxDynamicVB::m_sVB.pVB->Lock( DxDynamicVB::m_sVB.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
		memcpy( pVertices, m_pVertices, dwVertexSizeFULL );
		DxDynamicVB::m_sVB.pVB->Unlock ();

		// Note : 값을 더해줌. 
		DxDynamicVB::m_sVB.nVertexCount += nCount*4;
		DxDynamicVB::m_sVB.nOffsetToLock += dwVertexSizeFULL;

		// Note : 나중에 뿌리도록 셋팅 해 놓는다.
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
			pParticle->m_fLife = m_vLife.x + m_fLifeDiff*RANDOM_POS;		// 랜덤하게 삽입
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_POS;		// 랜덤하게 삽입
			pParticle->m_fRotate = m_vRotate.y * RANDOM_POS;				// 랜덤하게 삽입
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff*RANDOM_POS;		// 랜덤하게 삽입

			vDir = D3DXVECTOR3( RANDOM_NUM, RANDOM_NUM, RANDOM_NUM );
			pParticle->m_vPosOffSet =  vDir*m_fCreateLength;			// 일정한 거리를 띄어 놓는다.

			pParticle->m_fSizeBase = pParticle->m_fSize;				// 기본값
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// 값의 변화를 위해

			pParticle->m_dwFlag = 0L;												// 절대일 경우만.
			pParticle->m_fVelocity = m_vVelocity.x + m_fVelocityDiff*RANDOM_POS;	// 절대일 경우만.
			pParticle->m_fFirstPosRate = RANDOM_POS;								// 절대일 경우만
			pParticle->m_vFirstPos = D3DXVECTOR3( 0.f, 0.f, 0.f );					// 절대일 경우만

			dwNUM -= 1;		// 값을 빼줌

			pParticle->m_pNext = m_pParticleHEAD;
			m_pParticleHEAD = pParticle;
		}
	}
	else
	{
		while( dwNUM > 0 )
		{
			DXPARTICLE* pParticle = m_pParticlePool->New();
			pParticle->m_fLife = m_vLife.x + m_fLifeDiff*RANDOM_POS;		// 랜덤하게 삽입
			pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_POS;		// 랜덤하게 삽입
			pParticle->m_fRotate = m_vRotate.y * RANDOM_POS;				// 랜덤하게 삽입
			pParticle->m_fRotateVel = m_vRotate.x + m_fRotateDiff*RANDOM_POS;		// 랜덤하게 삽입

			vDir = D3DXVECTOR3( RANDOM_NUM, RANDOM_NUM, RANDOM_NUM );
			pParticle->m_vPosOffSet =  vDir*m_fCreateLength;			// 일정한 거리를 띄어 놓는다.

			pParticle->m_fSizeBase = pParticle->m_fSize;				// 기본값
			pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// 값의 변화를 위해

			dwNUM -= 1;		// 값을 빼줌

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

	// Note : 비율을 알아냄.
	m_fRate = m_fTime / m_fLife;

	// Note : 위치 셋팅
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

	// Note : 변화률이 있을 경우만 사이즈를 바꿈
	if( sProp.m_vSize.z != 1.f )		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;

	// Note : 디퓨즈 셋팅.
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
	// Note : 플레이가 적용 안 되었으면 동작하지 않는다.
	if( !(m_dwFlag&USE_PLAY) )	return TRUE;

	m_fTime += fElpasedTime;
	if( m_fTime >= m_fLife )	return FALSE;

	// Note : 비율을 알아냄.
	m_fRate = m_fTime / m_fLife;

	// Note : 위치
	m_vPos += m_vDirDiff*m_fVelocity*fElpasedTime;

	// Note : 변화률이 있을 경우만 사이즈를 바꿈
	if( sProp.m_vSize.z != 1.f )		m_fSize = m_fSizeBase + m_fSizeDiff*m_fRate;

	// Note : 디퓨즈 셋팅.
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

	if( m_dwFlag&USE_POS1 )		// 두번째 걸릴때
	{
		// Note : 두번째 위치를 구한다.
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

		// Note : 2점 사이에서의 새로운 점을 구한다.
		float fRate01 = RANDOM_POS;
		float fRate02 = 1.f-fRate01;
		m_vDirDiff = vPos - m_vFirstPos;
		m_vPos = (m_vFirstPos*fRate01) + (vPos*fRate02);

		m_dwFlag |= USE_PLAY;
	}
	else	// 처음 걸릴때
	{
		// Note : 처음 위치를 구한다.
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
	// Note : 스켈렉톤이 없다면 리턴
	if( !m_pSkeleton )	return;

	DXCUSTOMBONE	sCBone;
	sCBone.strBoneName = szName;
	sCBone.pBoneTran = m_pSkeleton->FindBone( szName );

	// Note : 본이 없다면 리턴
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

	// Note : 맨 마지막으로 가야함.
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
	D3DXVECTOR3* pLocal(NULL);		// 주소값을 받아서 사용한다.
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
	// Note : 새로 만들어야 한다.
	m_vecPosition.clear();

	if( m_listCustomBone.size() < 2 )	return FALSE;

	// Note : 일단 복제
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

	// Note : 비율을 맞춘다.
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

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		DWORD dwSize = 0;

		//	Note : 이팩트의 Property 를 저장.
		SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

		// Note : Edit를 하기 위한 Data
		SFile >> dwSize;
		m_listCustomBone.clear();
		for( DWORD i=0; i<dwSize; ++i )
		{
			DXCUSTOMBONE	sData;
			sData.Load( SFile );
			m_listCustomBone.push_back( sData );
		}

		// Note : 실제 사용하기 위한 Data
		SFile >> dwSize;
		m_vecPosition.clear();
		for( DWORD i=0; i<dwSize; ++i )
		{
			DXPOSDELTA sPosDelta;
			sPosDelta.Load( SFile );
			m_vecPosition.push_back( sPosDelta );
		}

		//	Note : Device 자원을 생성한다.
		hr = Create( pd3dDevice );
		if ( FAILED(hr) )	return hr;

	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		SFile.SetOffSet( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharBoneListEff::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	// Note : Edit를 하기 위한 Data
	SFile << (DWORD)m_listCustomBone.size();
	LISTCUSTOMBONE_ITER iter = m_listCustomBone.begin();
	for( ; iter!=m_listCustomBone.end(); ++iter )
	{
		(*iter).Save( SFile );
	}

	// Note : 실제 사용하기 위한 Data
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




