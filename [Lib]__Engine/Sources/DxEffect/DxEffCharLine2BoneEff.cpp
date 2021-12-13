#include "pch.h"

#include <string>
//#include <strstream>

#include <algorithm>
#include "./StlFunctions.h"
#include "./DxInputDevice.h"
#include "./TextureManager.h"
#include "./SerialFile.h"
#include "./DxDynamicVB.h"

#include "./GLDefine.h"
#include "./EDITMESHS.h"
#include "./DxViewPort.h"

#include "./DxEffCharLine2BoneEff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern BOOL			g_bOBJECT100;
extern std::string	g_strPICK_BONE;
extern BOOL			g_bCHAR_EDIT_RUN;

DWORD		DxEffCharLine2BoneEff::TYPEID			= EMEFFCHAR_LINE2BONE;
DWORD		DxEffCharLine2BoneEff::VERSION			= 0x0101;
char		DxEffCharLine2BoneEff::NAME[MAX_PATH]	= "2.Line2BoneEff <...>"; 

DxEffCharLine2BoneEff::PARTICLEPOOL*	DxEffCharLine2BoneEff::m_pParticlePool = NULL;
VERTEXCOLORTEX2							DxEffCharLine2BoneEff::m_pVertices[MAX_FACE*4];

void EFFCHAR_PROPERTY_LINE2BONEEFF::Assign( EFFCHAR_PROPERTY_LINE2BONEEFF_0100 &sOldProperty )
{
	m_dwFlag		= sOldProperty.m_dwFlag;
	m_dwOtherColor  = sOldProperty.m_dwOtherColor;
	m_dwParticlePec = sOldProperty.m_dwParticlePec;
	m_dwColor		= sOldProperty.m_dwColor;
	m_dwColor2		= sOldProperty.m_dwColor2;
	m_vLife			= sOldProperty.m_vLife;
	m_vSize			= sOldProperty.m_vSize;
	m_vScaleX		= sOldProperty.m_vScaleX;
	m_vScaleY		= sOldProperty.m_vScaleY;
	m_fShake		= sOldProperty.m_fShake;
	m_dwSequenceCol	= sOldProperty.m_dwSequenceCol;
	m_dwSequenceRow	= sOldProperty.m_dwSequenceRow;
	m_fCreateLength	= sOldProperty.m_fCreateLength;
	m_fMinMaxDist.min = 4.0f;
	m_fMinMaxDist.max = 4.0f;
	m_fMinMaxTexRotate.min = 0.0f;
	m_fMinMaxTexRotate.max = 0.0f;
	m_bWithCamMove	= TRUE;
	strcpy_s( m_szTexture, sOldProperty.m_szTexture );
	strcpy_s( m_szTexture2, sOldProperty.m_szTexture2 );

}

DxEffCharLine2BoneEff::ParticleListTEX::ParticleListTEX() :
	m_pParticleHEAD(NULL),
	m_pTexture(NULL)
{
}

void DxEffCharLine2BoneEff::ParticleListTEX::CleanUp()
{
	DXPARTICLE* pDel = NULL;
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->m_pNext;
		SAFE_DELETE( pDel );	// m_pParticlePool은 NULL로 되어 있어서 스스로 Delete를 할 수 밖에 없다.
	}
}

void DxEffCharLine2BoneEff::ParticleListTEX::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pName )
{
	m_strTexName = pName;
	TextureManager::LoadTexture( m_strTexName.c_str(), pd3dDevice, m_pTexture, 0, 0, FALSE );
}

void DxEffCharLine2BoneEff::ParticleListTEX::OnDestroyDevice()
{
	TextureManager::ReleaseTexture( m_strTexName.c_str(), m_pTexture );

	if( !m_pParticlePool )	return;

	DXPARTICLE* pDel(NULL);
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->m_pNext;
		m_pParticlePool->Release( pDel );
	}

	m_pParticleHEAD = NULL;
}

void DxEffCharLine2BoneEff::ParticleListTEX::CreateParticle( DXPARTICLE* pNew )
{
	pNew->m_pNext = m_pParticleHEAD;
	m_pParticleHEAD = pNew;
}

void DxEffCharLine2BoneEff::ParticleListTEX::FrameMove( EFFCHAR_PROPERTY_LINE2BONEEFF& sProp, DXDATA& sData, float fElapsedTime )
{
	DXPARTICLE* pPrev = NULL;
	DXPARTICLE* pDel = NULL;
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		// Note : 생명이 다 되었는지도 여기서 체크
		if( pCur->FrameMove( sProp, sData, fElapsedTime ) )	// 삭제 없음.
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

void DxEffCharLine2BoneEff::ParticleListTEX::Render( LPDIRECT3DDEVICEQ pd3dDevice, const float fScale, const D3DXVECTOR3& vViewLeft, const D3DXVECTOR3& vViewUP,
													const EFFCHAR_PROPERTY_LINE2BONEEFF& sProp, DxSequenceUV& sSequenceUV )
{
	// Note : 시퀀스인지 체크한다.
	D3DXVECTOR2* pTexUV(NULL);
	D3DXVECTOR2 vTexUV00(0.f,0.f);
	D3DXVECTOR2 vTexUV10(0.f,0.f);
	D3DXVECTOR2 vTexUV01(0.f,0.f);
	D3DXVECTOR2 vTexUV11(0.f,0.f);
	float		fOffsetWidth(0.f);
	float		fOffsetHeight(0.f);
	if( sProp.m_dwFlag & USE_SEQUENCE )
	{
		fOffsetWidth = sSequenceUV.GetOffsetWidth();
		fOffsetHeight = sSequenceUV.GetOffsetHeight();
	}

	const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
	const D3DXVECTOR3& vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vViewDir = vLookatPt - vFromPt;
	D3DXVec3Normalize( &vViewDir, &vViewDir );

	float		fSize(0.f);
	int			nCount(0);
	D3DXVECTOR2 vDot3(0.f,0.f);
	D3DXVECTOR3 vUP(0.f,1.f,0.f);
	D3DXVECTOR3	vLeft(0.f,1.f,0.f);
	D3DXVECTOR3 vBasePos(0.f,0.f,0.f);

	D3DXVECTOR2 vScaleDiff;
	vScaleDiff.x = (sProp.m_vScaleX.y - sProp.m_vScaleX.x);
	vScaleDiff.y = (sProp.m_vScaleY.y - sProp.m_vScaleY.x);

	// Note : 만든 후 뿌린다.
	DXPARTICLE* pCur = m_pParticleHEAD;
	while( pCur )
	{
		if( nCount >= MAX_FACE )	break;

		// Note : 사이즈 변화를 줌
		fSize = pCur->m_fSize*fScale;

		if( sProp.m_bWithCamMove )
		{
			vDot3.x = D3DXVec3Dot( &vViewLeft, &pCur->m_vDir );
			vDot3.y = D3DXVec3Dot( &vViewUP, &pCur->m_vDir );

			if( vDot3.y < 0.f )	vDot3.y = -vDot3.y;		// 위쪽으로만 바라보도록...
			vDot3.y += 0.1f;							// 바라보는 시점과 방향이 같지 않도록 하기 위함..

			D3DXVec2Normalize( &vDot3, &vDot3 );

			// Note : 새로운 방향을 구한다.
			vUP = vViewLeft*vDot3.x + vViewUP*vDot3.y;

			// .
			//vDir = pCur->m_vPos - vFromPt;
			D3DXVec3Cross( &vLeft, &vViewDir, &vUP );
			D3DXVec3Normalize( &vLeft, &vLeft );
			D3DXVec3Normalize( &vUP, &vUP );
		}else{
			vUP   = pCur->m_vDir;
			vLeft = D3DXVECTOR3( -1.0f, 0.0f, 0.0f );
		}

		
		vUP *= fSize;
		vLeft *= fSize;

		float fRateX = sProp.m_vScaleX.x + (pCur->m_fRate * vScaleDiff.x );
		D3DXVECTOR3 vRateY1 = vUP * sProp.m_vScaleY.x * pCur->m_fRate;
		D3DXVECTOR3 vRateY2 = vUP * sProp.m_vScaleY.y * pCur->m_fRate;
		D3DXVECTOR3 vRateY3 = vUP * sProp.m_vScaleY.z;
		D3DXVECTOR3 vParticlePos1, vParticlePos2, vSizePos;		
		D3DXMATRIX	matRotate, matBillboard1, matBillboard2, matLocal;

		vBasePos = pCur->m_vPos;
		vViewDir = vLookatPt - vFromPt;
		D3DXVec3Normalize( &vViewDir, &vViewDir );

		vBasePos += vViewDir * pCur->m_fBoneToCamDist;	// 약간 앞뒤로 효과를 준다.

		if( sProp.m_bWithCamMove )
		{
			m_pVertices[(nCount*4)+0].vPos = vBasePos + vUP + vRateY1 + vRateY3 + (vLeft*fRateX);
			m_pVertices[(nCount*4)+1].vPos = vBasePos + vUP + vRateY1 + vRateY3 - (vLeft*fRateX);
			m_pVertices[(nCount*4)+2].vPos = vBasePos - vUP + vRateY2 + vRateY3 + (vLeft*fRateX);
			m_pVertices[(nCount*4)+3].vPos = vBasePos - vUP + vRateY2 + vRateY3 - (vLeft*fRateX);
		}else
		{
			vParticlePos1 = vBasePos + vRateY1 + vRateY3;
			vParticlePos2 = vBasePos + vRateY1 + vRateY2;

			matBillboard1 = DxBillboardLookAt( &vParticlePos1, DxViewPort::GetInstance().GetMatView() );
			matBillboard2 = DxBillboardLookAt( &vParticlePos2, DxViewPort::GetInstance().GetMatView() );

			
			D3DXMatrixRotationZ( &matRotate, pCur->m_fTexRotate );

			vSizePos = vUP + (vLeft*fRateX);
			matLocal = matRotate * matBillboard1;
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+0].vPos, &vSizePos, &matLocal );

			vSizePos = vUP - (vLeft*fRateX);
			matLocal = matRotate * matBillboard1;
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+1].vPos, &vSizePos, &matLocal );

			vSizePos = -vUP + (vLeft*fRateX);
			matLocal = matRotate * matBillboard2;
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+2].vPos, &vSizePos, &matLocal );

			vSizePos = -vUP - (vLeft*fRateX);
			matLocal = matRotate * matBillboard2;
			D3DXVec3TransformCoord( &m_pVertices[(nCount*4)+3].vPos, &vSizePos, &matLocal );
		}

		const DWORD& dwColor = pCur->m_dwColor;
		m_pVertices[(nCount*4)+0].dwColor = dwColor;
		m_pVertices[(nCount*4)+1].dwColor = dwColor;
		m_pVertices[(nCount*4)+2].dwColor = dwColor;
		m_pVertices[(nCount*4)+3].dwColor = dwColor;




		// Note : 시퀀스를 사용할 경우와 아닐 경우	
		if( sProp.m_dwFlag & USE_SEQUENCE )
		{
			pTexUV = sSequenceUV.GetTexUV( sSequenceUV.GetTexCount(pCur->m_fRate) );
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
		pData->bAlphaTex = TRUE;
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

DxEffCharLine2BoneEff::DxEffCharLine2BoneEff(void) :
	DxEffChar(),
	m_fElapsedTime(0.f),
	m_fElapsedTimeADD(0.f),
	m_bEffStart(FALSE),
	m_vCharPos(0.f,0.f,0.f),
	m_fParticleADD(0.f),
	m_fLengthFULL(0.f),
	m_fSizeDiff(0.f),
	m_fLifeDiff(0.f),
	m_nSelectIndexLine(-1)
{
}

DxEffCharLine2BoneEff::~DxEffCharLine2BoneEff(void)
{
	CleanUp ();

	m_sParticleListTEX[0].CleanUp();
	m_sParticleListTEX[1].CleanUp();

	std::for_each( m_vecBoneINF.begin(), m_vecBoneINF.end(), std_afunc::DeleteObject() );
	m_vecBoneINF.clear();

	std::for_each( m_vecBoneLINE.begin(), m_vecBoneLINE.end(), std_afunc::DeleteObject() );
	m_vecBoneLINE.clear();
}

DxEffChar* DxEffCharLine2BoneEff::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharLine2BoneEff *pEffChar = new DxEffCharLine2BoneEff;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	std::for_each( pEffChar->m_vecBoneINF.begin(), pEffChar->m_vecBoneINF.end(), std_afunc::DeleteObject() );
	pEffChar->m_vecBoneINF.clear();

	std::for_each( pEffChar->m_vecBoneLINE.begin(), pEffChar->m_vecBoneLINE.end(), std_afunc::DeleteObject() );
	pEffChar->m_vecBoneLINE.clear();

	for( DWORD i=0; i<m_vecBoneINF.size(); ++i )
	{
		BONEINF* pNew = new BONEINF;
		pNew->m_rBoneTran = m_vecBoneINF[i]->m_rBoneTran;
		pNew->m_strName = m_vecBoneINF[i]->m_strName.c_str();
		
		pEffChar->m_vecBoneINF.push_back( pNew );
	}

	for( DWORD i=0; i<m_vecBoneLINE.size(); ++i )
	{
		BONELINE* pNew = new BONELINE;
		pNew->m_dwBONEID[0]		= m_vecBoneLINE[i]->m_dwBONEID[0];
		pNew->m_dwBONEID[1]		= m_vecBoneLINE[i]->m_dwBONEID[1];
		pNew->m_fLength			= m_vecBoneLINE[i]->m_fLength;
		pNew->m_fLengthStart	= m_vecBoneLINE[i]->m_fLengthStart;
		pNew->m_fLengthEnd		= m_vecBoneLINE[i]->m_fLengthEnd;
		
		pEffChar->m_vecBoneLINE.push_back( pNew );
	}

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharLine2BoneEff::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pParticlePool = new CMemPool<DXPARTICLE>;

	return S_OK;
}

HRESULT DxEffCharLine2BoneEff::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_DELETE( m_pParticlePool );

	return S_OK;
}

HRESULT DxEffCharLine2BoneEff::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffCharLine2BoneEff::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sParticleListTEX[0].OnCreateDevice( pd3dDevice, m_szTexture );
	m_sParticleListTEX[1].OnCreateDevice( pd3dDevice, m_szTexture2 );

	return S_OK;
}

HRESULT DxEffCharLine2BoneEff::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sPlayData.vDir = D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_sPlayData.dwColorA = (m_dwColor&0xff000000)>>24;
	m_sPlayData.dwColorR = (m_dwColor&0xff0000)>>16;
	m_sPlayData.dwColorG = (m_dwColor&0xff00)>>8;
	m_sPlayData.dwColorB = m_dwColor&0xff;
	m_sPlayData.dwColorBASE = m_dwColor;

	DWORD dwColor2[3];
	dwColor2[0] = (m_dwColor2&0xff0000)>>16;
	dwColor2[1] = (m_dwColor2&0xff00)>>8;
	dwColor2[2] = m_dwColor2&0xff;

	m_sPlayData.nDiffColor[0] = dwColor2[0] - m_sPlayData.dwColorR;
	m_sPlayData.nDiffColor[1] = dwColor2[1] - m_sPlayData.dwColorG;
	m_sPlayData.nDiffColor[2] = dwColor2[2] - m_sPlayData.dwColorB;

	if( m_dwFlag & USE_SEQUENCE )	m_sSequenceUV.CreateData( m_dwSequenceCol, m_dwSequenceRow );

	return S_OK;
}

HRESULT DxEffCharLine2BoneEff::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharLine2BoneEff::DeleteDeviceObjects ()
{
	m_sParticleListTEX[0].OnDestroyDevice();
	m_sParticleListTEX[1].OnDestroyDevice();

	return S_OK;
}

HRESULT DxEffCharLine2BoneEff::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxEffCharLine2BoneEff::FrameMove( float fTime, float fElapsedTime )
{
	if( !m_pSkeleton )	return S_OK;

	if( m_pSkeleton->pBoneRoot )
	{
		D3DXVECTOR3 vPosPREV = m_vCharPos;

		// 본정보는 캐릭터 공유하기 때문에 랜더링 시점에 미리 좌표를 받아둔다..
//		m_vCharPos.x = m_pSkeleton->pBoneRoot->matCombined._41;
//		m_vCharPos.y = m_pSkeleton->pBoneRoot->matCombined._42;
//		m_vCharPos.z = m_pSkeleton->pBoneRoot->matCombined._43;

		m_sPlayData.vPos = m_vCharPos;

		if( m_bEffStart )
		{
			m_sPlayData.vDir = vPosPREV - m_vCharPos;
		}
		else
		{
			m_bEffStart = TRUE;
		}
	}

	m_fElapsedTime = fElapsedTime;
	m_fElapsedTimeADD += fElapsedTime;

	// Note : FindBone And Link.
	for( DWORD i=0; i<m_vecBoneINF.size(); ++i )
	{
		if( !m_vecBoneINF[i]->m_rBoneTran )
		{
			m_vecBoneINF[i]->m_rBoneTran = m_pSkeleton->FindBone( m_vecBoneINF[i]->m_strName.c_str() );
		}
	}

	return S_OK;
}

void DxEffCharLine2BoneEff::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )
{
	m_sParticleListTEX[0].FrameMove( m_Property, m_sPlayData, m_fElapsedTime );
	m_sParticleListTEX[1].FrameMove( m_Property, m_sPlayData, m_fElapsedTime );

	if( m_fElapsedTimeADD > 0.0333f )
	{
		m_fElapsedTimeADD = 0.0333f;
		m_fParticleADD += m_dwParticlePec*m_fElapsedTimeADD;	// Note : 생성 갯수를 제어한다.
		m_fElapsedTimeADD = 0.f;
	}

	// Note : 전체 길이를 셋팅.
	float	fLength(0.f);
	float	fLengthADD(0.f);
	D3DXVECTOR3	vDir(0.f,0.f,0.f);
	D3DXVECTOR3	vPos1(0.f,0.f,0.f);
	D3DXVECTOR3	vPos2(0.f,0.f,0.f);

	DxBoneTrans*	pBoneTran1;
	DxBoneTrans*	pBoneTran2;
	for( DWORD i=0; i<m_vecBoneLINE.size(); ++i )
	{
		pBoneTran1 = m_vecBoneINF[ m_vecBoneLINE[i]->m_dwBONEID[0] ]->m_rBoneTran;
		pBoneTran2 = m_vecBoneINF[ m_vecBoneLINE[i]->m_dwBONEID[1] ]->m_rBoneTran;
		if( pBoneTran1 && pBoneTran2 )
		{
			vPos1.x = pBoneTran1->matCombined._41;
			vPos1.y = pBoneTran1->matCombined._42;
			vPos1.z = pBoneTran1->matCombined._43;

			vPos2.x = pBoneTran2->matCombined._41;
			vPos2.y = pBoneTran2->matCombined._42;
			vPos2.z = pBoneTran2->matCombined._43;

			vDir = vPos1 - vPos2;
			fLength = D3DXVec3Length( &vDir );
			

			m_vecBoneLINE[i]->m_fLength = fLength;

			m_vecBoneLINE[i]->m_fLengthStart = fLengthADD;
			fLengthADD += fLength;
			m_vecBoneLINE[i]->m_fLengthEnd = fLengthADD;
		}
	}

	// Note : 알맞은 비율을 정함.
	DWORD dwParticleCreate = (DWORD)m_fParticleADD;	// 생성 갯수 구함.
	m_fParticleADD -= dwParticleCreate;

	m_setParticlePoint.clear();

	for( DWORD i=0; i<dwParticleCreate; ++i )
	{
		fLength = fLengthADD * RANDOM_POS;
		m_setParticlePoint.insert( fLength );
	}

	// Note : 파티클 생성. < Line을 돈다.. 생성되는 위치는 정렬되어 있다. >
	float fRate(0.f);
	D3DXMATRIX matInv;
	D3DXMATRIX matLocal;
	SET_MAKEPARTICLE_ITER iter = m_setParticlePoint.begin();
	for( DWORD i=0; i<m_vecBoneLINE.size(); )
	{
		if( iter == m_setParticlePoint.end() )
		{
			i = (DWORD)m_vecBoneLINE.size();
			break;
		}
		pBoneTran1 = m_vecBoneINF[ m_vecBoneLINE[i]->m_dwBONEID[0] ]->m_rBoneTran;
		pBoneTran2 = m_vecBoneINF[ m_vecBoneLINE[i]->m_dwBONEID[1] ]->m_rBoneTran;
		if( pBoneTran1 && pBoneTran2 )
		{

			vPos2.x = pBoneTran2->matCombined._41;
			vPos2.y = pBoneTran2->matCombined._42;
			vPos2.z = pBoneTran2->matCombined._43;

			D3DXMatrixInverse( &matInv, NULL, &pBoneTran1->matCombined );
			D3DXVec3TransformCoord( &vPos2, &vPos2, &matInv );

			if( (*iter) >= m_vecBoneLINE[i]->m_fLengthStart && 
				(*iter) <= m_vecBoneLINE[i]->m_fLengthEnd )
			{
				fRate = ((*iter)-m_vecBoneLINE[i]->m_fLengthStart) / (m_vecBoneLINE[i]->m_fLengthEnd-m_vecBoneLINE[i]->m_fLengthStart);

				DxCreateParticle( vPos2*fRate, pBoneTran1 );
				++iter;
				continue;	// 같은 위치를 한번더 체크해야 한다.
			}
		}

		++i;
	}

	int			nCount(0);
	float		fSize(0.f);
	D3DXVECTOR3 vLocal(0.f,0.f,0.f);

	float fHalfPI = D3DX_PI*0.5f;
	float fOneHalfPI = D3DX_PI*1.5f;
	const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
	const D3DXVECTOR3& vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	const D3DXVECTOR3 vViewDir = vLookatPt - vFromPt;

	D3DXVECTOR3	vCenter(0.f,0.f,0.f);
	D3DXVECTOR3	vViewUP(0.f,1.f,0.f);
	D3DXVECTOR3	vViewLeft(0.f,1.f,0.f);
	vDir = DxViewPort::GetInstance().GetLookDir();
	D3DXVec3Cross( &vViewLeft, &vViewDir, &vViewUP );
	D3DXVec3Cross( &vViewUP, &vViewLeft, &vViewDir );
	D3DXVec3Normalize( &vViewLeft, &vViewLeft );
	D3DXVec3Normalize( &vViewUP, &vViewUP );

	D3DXVECTOR3 vDirStart( 0.f, 0.f, 0.f );
	D3DXVECTOR3 vDirEnd( 0.f, 0.f, 0.f );

	D3DXPLANE sPlane;
	D3DXPlaneFromPoints( &sPlane, &vCenter, &vViewUP, &vViewLeft );

	m_sParticleListTEX[0].Render( pd3dDevice, fScale, vViewLeft, vViewUP, m_Property, m_sSequenceUV );
	m_sParticleListTEX[1].Render( pd3dDevice, fScale, vViewLeft, vViewUP, m_Property, m_sSequenceUV );
}

void DxEffCharLine2BoneEff::RenderEDIT( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )
{
	if( m_pSkeleton && !g_bOBJECT100 )	
	{
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

		m_pSkeleton->CheckSphere( pd3dDevice, g_strPICK_BONE.c_str() );
		RenderLineAndPoint( pd3dDevice );

		pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharLine2BoneEff::DxCreateParticle( const D3DXVECTOR3 vPos, DxBoneTrans* rBoneTran )
{
	DXPARTICLE* pParticle = m_pParticlePool->New();
	pParticle->m_vBasePos = vPos;
	D3DXVec3TransformCoord( &pParticle->m_vPos, &vPos, &rBoneTran->matCombined );	// 위치.
	pParticle->m_vPrevPos = pParticle->m_vPos;
	pParticle->m_vCharPosS = m_sPlayData.vPos;
	pParticle->m_vCenterToPos = pParticle->m_vPos - m_sPlayData.vPos;
	pParticle->m_vDir = D3DXVECTOR3( RANDOM_NUM*0.1f, RANDOM_POS*0.9f+0.1f, RANDOM_NUM*0.1f );
	pParticle->m_vDir += m_sPlayData.vDir;

	// 사방으로 퍼지도록 하기 위함.
	D3DXVECTOR3 vDir = pParticle->m_vPos - m_sPlayData.vPos;
	D3DXVec3Normalize( &vDir, &vDir );

	pParticle->m_vDir += vDir*0.05f;
	D3DXVec3Normalize( &pParticle->m_vDir, &pParticle->m_vDir );
	pParticle->m_rBoneTran = rBoneTran;

	pParticle->m_fLife = m_vLife.x + m_fLifeDiff*RANDOM_POS;		// 랜덤하게 삽입
	pParticle->m_fSize = m_vSize.x + m_fSizeDiff*RANDOM_POS;		// 랜덤하게 삽입

	vDir = D3DXVECTOR3( RANDOM_NUM, RANDOM_NUM, RANDOM_NUM );
	pParticle->m_vPosOffSet =  vDir*m_fCreateLength;			// 일정한 거리를 띄어 놓는다.

	pParticle->m_fSizeBase = pParticle->m_fSize;				// 기본값
	pParticle->m_fSizeDiff = (pParticle->m_fSize*m_vSize.z) - pParticle->m_fSize;	// 값의 변화를 위해

	pParticle->m_fCameraCome = RANDOM_NUM;

	m_sPlayData.dwColorA = (m_dwColor&0xff000000)>>24;
	m_sPlayData.dwColorR = (m_dwColor&0xff0000)>>16;
	m_sPlayData.dwColorG = (m_dwColor&0xff00)>>8;
	m_sPlayData.dwColorB = m_dwColor&0xff;

	float fRate(0.f);
	float fRateINV(0.f);
	switch( m_dwOtherColor )
	{
	case 0:
		pParticle->m_dwColor1[0] = m_sPlayData.dwColorR;
		pParticle->m_dwColor1[1] = m_sPlayData.dwColorG;
		pParticle->m_dwColor1[2] = m_sPlayData.dwColorB;
		break;

	case 1:
		fRate = RANDOM_POS;
		pParticle->m_dwColor1[0] = m_sPlayData.dwColorR + (DWORD)(m_sPlayData.nDiffColor[0]*fRate);
		pParticle->m_dwColor1[1] = m_sPlayData.dwColorG + (DWORD)(m_sPlayData.nDiffColor[1]*fRate);
		pParticle->m_dwColor1[2] = m_sPlayData.dwColorB + (DWORD)(m_sPlayData.nDiffColor[2]*fRate);
		break;

	case 2:
		pParticle->m_dwColor1[0] = m_sPlayData.dwColorR;
		pParticle->m_dwColor1[1] = m_sPlayData.dwColorG;
		pParticle->m_dwColor1[2] = m_sPlayData.dwColorB;
		break;

	case 3:
		fRate = RANDOM_POS;
		if( fRate > 0.5f )
		{
			pParticle->m_dwColor1[0] = m_sPlayData.dwColorR;
			pParticle->m_dwColor1[1] = m_sPlayData.dwColorG;
			pParticle->m_dwColor1[2] = m_sPlayData.dwColorB;
		}
		else
		{
			pParticle->m_dwColor1[0] = m_sPlayData.dwColorR + m_sPlayData.nDiffColor[0];
			pParticle->m_dwColor1[1] = m_sPlayData.dwColorG + m_sPlayData.nDiffColor[1];
			pParticle->m_dwColor1[2] = m_sPlayData.dwColorB + m_sPlayData.nDiffColor[2];
		}
		break;
	};

	pParticle->m_fBoneToCamDist = RandomNumber( m_fMinMaxDist.min, m_fMinMaxDist.max );
	if( m_bWithCamMove )
	{
		pParticle->m_fUpdateTexRotate = 0.0f;		
	}else{
		pParticle->m_fUpdateTexRotate = RandomNumber( m_fMinMaxTexRotate.min, m_fMinMaxTexRotate.max );		
	}
	pParticle->m_fTexRotate = 0.0f;

	if( RANDOM_POS > 0.5f )
	{
		m_sParticleListTEX[0].CreateParticle( pParticle );
	}
	else
	{
		m_sParticleListTEX[1].CreateParticle( pParticle );
	}
}

BOOL DxEffCharLine2BoneEff::DXPARTICLE::FrameMove( EFFCHAR_PROPERTY_LINE2BONEEFF& sProp, DXDATA& sData, float fElapsedTime )
{
	m_fTime += fElapsedTime;
	if( m_fTime >= m_fLife )	return FALSE;

	// Note : 위치를 구함.
	m_vPrevPos = m_vPos;
	m_vPos = sData.vPos + m_vCenterToPos;
	
//	D3DXVec3TransformCoord( &m_vPos, &m_vBasePos, &m_rBoneTran->matCombined );

	//// Note : 위치가 같지 않다면 방향을 변화 시킨다.
	//if( m_vPrevPos.x > (m_vPos.x + 0.0001f) || m_vPrevPos.x < (m_vPos.x - 0.0001f) ||
	//	m_vPrevPos.y > (m_vPos.y + 0.0001f) || m_vPrevPos.y < (m_vPos.y - 0.0001f) || 
	//	m_vPrevPos.z > (m_vPos.z + 0.0001f) || m_vPrevPos.z < (m_vPos.z - 0.0001f) )
	//{
	//	// Note : 방향을 구함.
	//	m_vDir += (m_vPrevPos - m_vPos) * sProp.m_fShake;	// 움직임에 따른 불길의 변화를 준다.
	//	D3DXVec3Normalize( &m_vDir, &m_vDir );
	//}

//	// Note : 방향을 구함.
//	m_vDir += sData.vDir * sProp.m_fShake;	// 움직임에 따른 불길의 변화를 준다.
//	D3DXVec3Normalize( &m_vDir, &m_vDir );

	//// Note : 카메라 방향과 움직임 방향의 내적으로 알파강도를 조절한다.
	//const D3DXVECTOR3& vDir = DxViewPort::GetInstance().GetLookDir();
	//float fDot3 = D3DXVec3Dot( &vDir, &m_vDir );
	//fDot3 = fabsf(fDot3);
	//fDot3 = fDot3 * fDot3;
	//fDot3 = 1.f - fDot3;

	// Note : 비율을 알아냄.
	m_fRate = m_fTime / m_fLife;

	switch( sProp.m_dwOtherColor )
	{
	case 0:
		m_dwColor = m_dwColor1[0]<<16;
		m_dwColor += m_dwColor1[1]<<8;
		m_dwColor += m_dwColor1[2];
		break;

	case 1:
		m_dwColor = m_dwColor1[0]<<16;
		m_dwColor += m_dwColor1[1]<<8;
		m_dwColor += m_dwColor1[2];
		break;

	case 2:
		m_dwColor = (DWORD)(m_dwColor1[0] + sData.nDiffColor[0]*m_fRate)<<16;
		m_dwColor += (DWORD)(m_dwColor1[1] + sData.nDiffColor[1]*m_fRate)<<8;
		m_dwColor += (DWORD)(m_dwColor1[2] + sData.nDiffColor[2]*m_fRate);
		break;

	case 3:
		m_dwColor = m_dwColor1[0]<<16;
		m_dwColor += m_dwColor1[1]<<8;
		m_dwColor += m_dwColor1[2];
		break;
	};

	// Note : 알파 셋팅.
	float fAlpha(0.f);
	if( m_fRate < 0.3f )
	{
		fAlpha = (m_fRate/0.3f);//*fDot3;
		m_dwColor += ((DWORD)(sData.dwColorA*fAlpha)<<24);
	}
	else if( m_fRate > 0.4f )
	{
		fAlpha = ( 1.f - ((m_fRate-0.4f)/0.6f) );//*fDot3;
		m_dwColor += ((DWORD)(sData.dwColorA*fAlpha)<<24);
	}
	else
	{
		fAlpha = 1.f;
		m_dwColor += ((DWORD)(sData.dwColorA)<<24);
	}

	//fAlpha = 1.f - m_fRate;
	//fAlpha = fAlpha*fAlpha;
	//m_dwColor += ((DWORD)(sData.dwColorA*fAlpha)<<24);

	//DWORD dwR = (m_dwColor&0xff0000)>>16;
	//DWORD dwG = (m_dwColor&0xff00)>>8;
	//DWORD dwB = m_dwColor&0xff;

	//m_dwColor = ((DWORD)(dwR*fAlpha))<<16;
	//m_dwColor += ((DWORD)(dwG*fAlpha))<<8;
	//m_dwColor += (DWORD)(dwB*fAlpha);

	m_fTexRotate += m_fUpdateTexRotate * fElapsedTime;

	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharLine2BoneEff::InsertLine( const TCHAR* pName1, const TCHAR* pName2 )
{
	DWORD	dwIndexBone1(0);
	DWORD	dwIndexBone2(0);
	BOOL	bExistBone1(FALSE);
	BOOL	bExistBone2(FALSE);

	// Note : 일단 본 List 에서 찾는다.
	int nCount = 0;
	VEC_BONEINF_ITER iter = m_vecBoneINF.begin();
	for( ; iter!=m_vecBoneINF.end(); ++iter, ++nCount )
	{
		if( (*iter)->m_strName == pName1 )
		{
			bExistBone1 = TRUE;
			dwIndexBone1 = nCount;
		}
		if( (*iter)->m_strName == pName2 )	
		{
			bExistBone2 = TRUE;
			dwIndexBone2 = nCount;
		}
	}

	// 없다면 새로 넣는다.
	if( !bExistBone1 )
	{
		BONEINF* pNew = new BONEINF;
		pNew->m_strName = pName1;
		m_vecBoneINF.push_back( pNew );

		dwIndexBone1 = (DWORD)m_vecBoneINF.size() - 1;
	}
	if( !bExistBone2 )
	{
		BONEINF* pNew = new BONEINF;
		pNew->m_strName = pName2;
		m_vecBoneINF.push_back( pNew );

		dwIndexBone2 = (DWORD)m_vecBoneINF.size() - 1;
	}

	// Note : 인덱스는 다 구했으므로 Line 정보에 넣는다.
	BONELINE* pBoneNEW = new BONELINE;
	pBoneNEW->m_dwBONEID[0] = dwIndexBone1;
	pBoneNEW->m_dwBONEID[1] = dwIndexBone2;
	m_vecBoneLINE.push_back( pBoneNEW );
}

void DxEffCharLine2BoneEff::DeleteLine( int nIndex )
{
	int nCount(0);
	VEC_BONELINE_ITER iter = m_vecBoneLINE.begin();
	for( ; iter!=m_vecBoneLINE.end(); ++iter, ++nCount )
	{
		if( nCount==nIndex )
		{
			m_vecBoneLINE.erase( iter );
			return;
		}
	}
}

void DxEffCharLine2BoneEff::GetBoneName( int nIndex, TSTRING& strBone1, TSTRING& strBone2 )
{
	strBone1 = m_vecBoneINF[ m_vecBoneLINE[nIndex]->m_dwBONEID[0] ]->m_strName.c_str();
	strBone2 = m_vecBoneINF[ m_vecBoneLINE[nIndex]->m_dwBONEID[1] ]->m_strName.c_str();
}

void DxEffCharLine2BoneEff::SelectLine( int nIndex )
{
	m_nSelectIndexLine = nIndex;
}

void DxEffCharLine2BoneEff::RenderLineAndPoint( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR3 vPos1;
	D3DXVECTOR3 vPos2;

	DxBoneTrans*	m_rBoneTran1(NULL);
	DxBoneTrans*	m_rBoneTran2(NULL);

	for( DWORD i=0; i<m_vecBoneLINE.size(); ++i )
	{
		m_rBoneTran1 = m_vecBoneINF[ m_vecBoneLINE[i]->m_dwBONEID[0] ]->m_rBoneTran;
		m_rBoneTran2 = m_vecBoneINF[ m_vecBoneLINE[i]->m_dwBONEID[1] ]->m_rBoneTran;

		if( m_rBoneTran1 && m_rBoneTran2 )
		{
			vPos1.x = m_rBoneTran1->matCombined._41;
			vPos1.y = m_rBoneTran1->matCombined._42;
			vPos1.z = m_rBoneTran1->matCombined._43;

			vPos2.x = m_rBoneTran2->matCombined._41;
			vPos2.y = m_rBoneTran2->matCombined._42;
			vPos2.z = m_rBoneTran2->matCombined._43;

			if( m_nSelectIndexLine == i )
			{
				EDITMESHS::RENDERLINE( pd3dDevice, vPos1, vPos2, 0xff00ffff );
			}
			else
			{
				EDITMESHS::RENDERLINE( pd3dDevice, vPos1, vPos2, 0xff000000 );
			}

			RenderCountText( i, (vPos1+vPos2)*0.5f );
		}
	}
}

void DxEffCharLine2BoneEff::RenderCountText( DWORD nCount, const D3DXVECTOR3& vPos )
{
	D3DXVECTOR3 vOut;
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	D3DXVec3Project( &vOut, &vPos, &DxViewPort::GetInstance().GetViewPort(), &DxViewPort::GetInstance().GetMatProj(), 
						&DxViewPort::GetInstance().GetMatView(), &matIdentity );

	//std::strstream strStream;
	//strStream << nCount << std::ends;
	CDebugSet::ToPos( vOut.x, vOut.y, "%u", nCount );

	//strStream.freeze( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------
HRESULT	DxEffCharLine2BoneEff::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == 0x0100 )
	{
		DWORD dwSize = 0;

		//	Note : 이팩트의 Property 를 로드
		EFFCHAR_PROPERTY_LINE2BONEEFF_0100 sOldProperty;
		SFile.ReadBuffer( &sOldProperty, sizeof(sOldProperty) );

		m_Property.Assign(sOldProperty);

		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			BONEINF* pNew = new BONEINF;
			SFile >> pNew->m_strName;
			m_vecBoneINF.push_back( pNew );
		}

		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			BONELINE* pNew = new BONELINE;
			SFile >> pNew->m_dwBONEID[0];
			SFile >> pNew->m_dwBONEID[1];
			m_vecBoneLINE.push_back( pNew );
		}

		//	Note : Device 자원을 생성한다.
		hr = Create( pd3dDevice );
		if ( FAILED(hr) )	return hr;

	}
	else if ( dwVer == VERSION )
	{
		DWORD dwSize = 0;

		//	Note : 이팩트의 Property 를 로드.
		SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			BONEINF* pNew = new BONEINF;
			SFile >> pNew->m_strName;
			m_vecBoneINF.push_back( pNew );
		}

		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			BONELINE* pNew = new BONELINE;
			SFile >> pNew->m_dwBONEID[0];
			SFile >> pNew->m_dwBONEID[1];
			m_vecBoneLINE.push_back( pNew );
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

HRESULT	DxEffCharLine2BoneEff::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	// Note : m_vecBoneINF
	DWORD dwSize(0);
	dwSize = (DWORD)m_vecBoneINF.size();
	SFile << dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		SFile << m_vecBoneINF[i]->m_strName;
	}

	// Note : m_vecBoneLINE
	dwSize = (DWORD)m_vecBoneLINE.size();
	SFile << dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		SFile << m_vecBoneLINE[i]->m_dwBONEID[0];
		SFile << m_vecBoneLINE[i]->m_dwBONEID[1];
	}

	return S_OK;
}

void DxEffCharLine2BoneEff::SetSkeleton( DxSkeleton* pSkeleton )
{
	m_pSkeleton = pSkeleton;
	
	// 다른 캐릭터에 의해 본값이 바뀌기전에 미리 좌표값을 입력받아 둔다.
	if( m_pSkeleton->pBoneRoot )
	{
		m_vCharPos.x = m_pSkeleton->pBoneRoot->matCombined._41;
		m_vCharPos.y = m_pSkeleton->pBoneRoot->matCombined._42;
		m_vCharPos.z = m_pSkeleton->pBoneRoot->matCombined._43;
	}
}

