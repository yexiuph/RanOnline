#include "pch.h"

#include "./DxMethods.h"
#include "./DxViewPort.h"
#include "./TextureManager.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"

#include "./dxeffectmesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//------------------------------- 구동 클래스(CMeshEffect)시작 ----------------------------------//
const DWORD DxEffectMesh::TYPEID = EFFSINGLE_MESH;
const DWORD DxEffectMesh::FLAG = NULL;
const char DxEffectMesh::NAME[] = "메쉬효과";

CMemPool<DxEffectMesh> *DxEffectMesh::m_pPool = NULL;

LPDIRECT3DSTATEBLOCK9		DxEffectMesh::m_pSavedRenderStats = NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectMesh::m_pDrawRenderStats = NULL;

DxEffectMesh::DxEffectMesh(void):
	m_fElapsedTime(0.f),
	m_pTexture(NULL),
	m_pMesh(NULL),
	m_vDistance( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ),
	m_fTimeSum(0.f),
	m_iAllSprite(1),
	m_iNowSprite(0),
	m_bSequenceRoop(FALSE),
	m_pCollTEX(NULL)

{
	m_fSizeStart = 0.f;
	m_fAlphaStart = 0.f;

	m_fAngle			= 0.0f;
	m_nDelta			= 0;
	m_nMaterialDelta	= 0;
	m_vDistance			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_nBlend			= 1;
	m_vTexVel			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vTexVelSum		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_pTexture			= NULL;
	m_dwVertices		= 0;

	D3DXMatrixIdentity( &m_matWorld );
}

DxEffectMesh::~DxEffectMesh(void)
{
	CleanUp ();
}

//Note: CreateDivece와 ReleaseDevice는 모두 DxEffSingleMan에서
//      InvalidateDeviceObjects()와 RestoreDeviceObjects()에서 호출하는 부분이다.
HRESULT DxEffectMesh::CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// 변하지 않음 ( 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default랑 틀림. 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_BLENDOP,			D3DBLENDOP_ADD );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
	
		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedRenderStats );
		else			pd3dDevice->EndStateBlock ( &m_pDrawRenderStats );
	}

	return S_OK;
}


HRESULT DxEffectMesh::ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedRenderStats );
	SAFE_RELEASE( m_pDrawRenderStats );

	return S_OK;
}

void DxEffectMesh::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectMesh>;
}

void DxEffectMesh::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
}

void DxEffectMesh::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	if( !m_pMesh )	return;

	D3DXVECTOR3 vMaxMesh, vMinMesh;
	m_pMesh->GetBoundBox( vMaxMesh, vMinMesh );

	D3DXMatrixAABBTransform( vMaxMesh, vMinMesh, m_matWorld );

	if( vMax.x < vMaxMesh.x )	vMax.x = vMaxMesh.x;
	if( vMax.y < vMaxMesh.y )	vMax.y = vMaxMesh.y;
	if( vMax.z < vMaxMesh.z )	vMax.z = vMaxMesh.z;

	if( vMin.x > vMinMesh.x )	vMin.x = vMinMesh.x;
	if( vMin.y > vMinMesh.y )	vMin.y = vMinMesh.y;
	if( vMin.z > vMinMesh.z )	vMin.z = vMinMesh.z;

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectMesh::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );	

	return S_OK;
}

HRESULT DxEffectMesh::InvalidateDeviceObjects()
{
	TextureManager::ReleaseTexture( m_szCollTex.c_str(), m_pCollTEX ); 

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

void DxEffectMesh::ReStartEff ()
{
	m_fGAge		= 0.f;
	m_fAlpha	= m_fAlphaStart;
	m_fHeight	= m_fHeightStart;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;

	if( m_dwFlag & USESCALE )	m_fSize = m_fSizeStart;
	else						m_fSize = 1.f;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectMesh::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff의 나이를 계산.
	//
	m_fElapsedTime	= fElapsedTime; 
	m_fGAge			+= fElapsedTime;
	m_fTimeSum		+= fElapsedTime;

	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
	//
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_pThisGroup->m_fGBeginTime+m_pThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_fAlpha	= m_fAlphaStart;
			m_fHeight	= m_fHeightStart;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;

			if( m_dwFlag & USESCALE )	m_fSize = m_fSizeStart;
			else						m_fSize = 1.f;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : 시작되지 않았는지 검사, 시작 시간이면 활성화 시킴.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : 종료 시점이 되었는지 검사.
	//
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_END;

	if ( !m_pMesh || !m_pMesh->m_pLocalMesh )	goto _RETURN;

	//	Note: 회전 부분 이다.
	m_fAngle += fElapsedTime*m_fRotationAngle;
	if( m_fAngle > 2*D3DX_PI ) m_fAngle -= 2*D3DX_PI;

	//	Note : 사이즈 적용
	//
	if( m_dwFlag & USESCALE )
	{
		if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fSizeTime1)
			m_fSize += m_fSizeDelta1*fElapsedTime;
		else if ( m_fGAge >= m_fSizeTime1 && m_fGAge < m_fSizeTime2)	
			m_fSize += m_fSizeDelta2*fElapsedTime;
		else														
			m_fSize += m_fSizeDelta3*fElapsedTime;

		if ( m_fSize < 0.f )		m_fSize = 0.f;
	}

	//	Note : 사이즈 조정 XYZ 각각 제어 가능
	//
	if( m_dwFlag & USESIZEXYZ )
	{
		m_vSizeXYZEnd.x = m_fSize*(1.f+m_vSizeXYZ.x*((m_fGAge-m_fGBeginTime)/m_fGLifeTime));
		m_vSizeXYZEnd.y = m_fSize*(1.f+m_vSizeXYZ.y*((m_fGAge-m_fGBeginTime)/m_fGLifeTime));
		m_vSizeXYZEnd.z = m_fSize*(1.f+m_vSizeXYZ.z*((m_fGAge-m_fGBeginTime)/m_fGLifeTime));
	}
	else
	{
		m_vSizeXYZEnd = D3DXVECTOR3 ( m_fSize, m_fSize, m_fSize );
	}

	//	Note : 컬러, 알파 조정 
	//
	{
		float fDelta;

		if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fAlphaTime1)
			m_fAlpha += m_fAlphaDelta1*fElapsedTime;
		else if ( m_fGAge >= m_fAlphaTime1 && m_fGAge < m_fAlphaTime2)	
			m_fAlpha += m_fAlphaDelta2*fElapsedTime;
		else														
			m_fAlpha += m_fAlphaDelta3*fElapsedTime;

		if ( m_fAlpha < 0.f )		m_fAlpha = 0.f;
		if ( m_fAlpha > 1.f )		m_fAlpha = 1.f;

		fDelta = m_fGAge / (m_fGBeginTime + m_fGLifeTime);
		m_cColorVar = m_clrStart + fDelta*( m_clrEnd	 - m_clrStart );
		m_cColorVar.a = m_fAlpha;
	}

	//	Note : 텍스쳐 회전 적용
	//
	if ( m_dwFlag & USETEXROTATE )
	{
	}

	//	Note : 텍스쳐 이동 적용
	//
	if ( m_dwFlag & USETEXMOVE )
	{
		m_vTexVelSum.x += m_vTexVel.x*fElapsedTime;
		m_vTexVelSum.y += m_vTexVel.y*fElapsedTime;
		//if ( m_vTexVelSum.x > INT_MAX )		m_vTexVelSum.x = 0.f;	// 값이 넘어서지 않도록
		//if ( m_vTexVelSum.y > INT_MAX )		m_vTexVelSum.y = 0.f;	// 값이 넘어서지 않도록
		//if ( m_vTexVelSum.z > INT_MAX )		m_vTexVelSum.z = 0.f;	// 값이 넘어서지 않도록
	}

	//	Note : 텍스쳐 스케일 적용
	//
	if ( m_dwFlag & USETEXSCALE )
	{
		m_fTexScale		= m_fTexScaleDelta*fElapsedTime;
	}

	if ( m_dwFlag & USEHEIGHT_MESH )
	{
		m_fHeight += m_fHeightDelta*fElapsedTime;
	}

	//m_cColorVar = m_cColorStart;
	//m_cColorVar.a = m_fAlpha;	

	if ( m_dwFlag & USESEQUENCE )
	{
		if ( m_dwFlag & USESEQUENCELOOP )
		{
			if ( m_fTimeSum > m_fAniTime )
			{
				if ( m_bSequenceRoop )	m_iNowSprite += 1;
				else					m_iNowSprite -= 1;
				m_fTimeSum = 0.0f;
			}

			if ( m_iNowSprite >= m_iAllSprite )
			{
				m_bSequenceRoop = !m_bSequenceRoop;
				m_iNowSprite -= 2;
			}
			else if ( m_iNowSprite < 0 )
			{
				m_bSequenceRoop = !m_bSequenceRoop;
				m_iNowSprite += 2;
			}
		}
		else
		{
			if ( m_fTimeSum > m_fAniTime )
			{
				m_iNowSprite += 1;
				m_fTimeSum = 0.0f;
			}
			if ( m_iNowSprite >= m_iAllSprite )		m_iNowSprite = 0;
		}

		m_pSequenceUV[0].x = m_iNowSprite / (float)m_nCol;
		m_pSequenceUV[0].y = (m_iNowSprite / m_nCol) / (float)m_nRow;

		m_pSequenceUV[1].x = (m_iNowSprite+1) / (float)m_nCol;
		m_pSequenceUV[1].y = (m_iNowSprite / m_nCol) / (float)m_nRow;

		m_pSequenceUV[2].x = m_iNowSprite / (float)m_nCol;
		m_pSequenceUV[2].y = (m_iNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;

		m_pSequenceUV[3].x = (m_iNowSprite+1) / (float)m_nCol;
		m_pSequenceUV[3].y = (m_iNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;
	}

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );
	return S_OK;
}

HRESULT DxEffectMesh::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;

	// 지면 Tex를 얻어오고자 할 때.
	if( (m_dwFlag&USEGROUNDTEX) && (!m_pCollTEX) )	// 딱 한번만 체크한다.
	{
		DxLandMan*	pLandMan = DxEffSingleMan::GetInstance().GetLandMan();
		if ( pLandMan )
		{
			BOOL		bColl;
			D3DXVECTOR3	vColl;
			LPDXFRAME	pDxFrame = NULL;
			const char* szName = NULL;

			D3DXVECTOR3 vStart(matComb._41,matComb._42,matComb._43);
			D3DXVECTOR3 vEnd=vStart;
			vStart.y += 0.01f;
			vEnd.y -= 1000.f;
			pLandMan->IsCollisionEX ( vStart, vEnd, vColl, bColl, pDxFrame, szName );
			if ( bColl && szName )
			{
				m_szCollTex = szName;
				TextureManager::LoadTexture( szName, pd3dDevice, m_pCollTEX, 0L, 0L, TRUE );
			}
		}
	}

	float fTime;
	
	D3DXVECTOR3	vDelta(0.f,0.f,0.f);
	D3DXVECTOR3	vStartPos;
	D3DXQUATERNION qRotate;
	D3DXMATRIX matWorld, matRotate, matTrans, matScale, matChild;

	matTrans = m_matLocal;
	matTrans._41 *= m_pThisGroup->m_fOut_Scale;
	matTrans._42 *= m_pThisGroup->m_fOut_Scale;
	matTrans._43 *= m_pThisGroup->m_fOut_Scale;
	matWorld	= matTrans;
	vStartPos	= D3DXVECTOR3 (matTrans._41,matTrans._42,matTrans._43);

	//	Note : 센터로 이동
	//
	if ( m_dwFlag & USEGOTOCENTER )
	{
		fTime = m_fGBeginTime+m_fGLifeTime-m_fGAge;

		vDelta = vStartPos/m_fGLifeTime;

		matTrans._41 = vDelta.x*fTime;
		matTrans._42 = vDelta.y*fTime;
		matTrans._43 = vDelta.z*fTime;

		D3DXMatrixMultiply ( &matChild, &matTrans, &matComb );

		if ( (m_dwFlag & USEDIRECTION) && !(m_pThisGroup->m_dwFlag & EFF_CFG_NONE) )	// 목표없음
		{
			matTrans = DxBillboardLookAtDir ( &vDelta, &vStartPos );//m_pAffineParts->vTrans );
			D3DXMatrixMultiply ( &matWorld, &matTrans, &matChild  );	// 회전
		}
		else if ( m_dwFlag & USEDIRECTION )	// 방향, ID, 계속,
		{
			//D3DXVec3TransformCoord ( &vStartPos, &vStartPos, &matComb );
			matTrans = DxBillboardLookAtDir ( &m_pThisGroup->m_TargetID.vPos, &vStartPos );//m_pAffineParts->vTrans );
			D3DXMatrixMultiply ( &matWorld, &matTrans, &matChild  );	// 회전
		}
		else
		{
			matWorld = matChild;
		}
	}
	else
	{
		D3DXMatrixMultiply ( &matChild, &matTrans, &matComb );

		if ( (m_dwFlag & USEDIRECTION) && m_bMoveObj )
		{
			matTrans = DxBillboardLookAtDir ( &m_pThisGroup->m_vGDelta, &vDelta );
			D3DXMatrixMultiply ( &matWorld, &matTrans, &matChild  );	// 회전
		}
		else if ( m_dwFlag & USEDIRECTION )
		{
			matTrans = DxBillboardLookAtDir ( &m_pThisGroup->m_vGDelta );
			D3DXMatrixMultiply ( &matWorld, &matTrans, &matChild  );	// 회전
		}
		else if ( m_pThisGroup->m_bOut_Direct )
		{
			matTrans = m_pThisGroup->m_matOutRotate;
			D3DXMatrixMultiply ( &matWorld, &matTrans, &matChild  );	// 회전
		}
		else
		{
			matWorld = matChild;
		}
	} 

	//	Note : 외부 스케일이 적용 받는다.
	//	Note : 이 부분에 스케일링 부분이 와야 한다.
	if( m_dwFlag & USESCALE )
	{
		float fSize = m_fSize * m_pThisGroup->m_fOut_Scale;
		if ( fSize < 0.00001f )				goto _RETURN;

		D3DXMatrixScaling ( &matScale, fSize, fSize, fSize );
		D3DXMatrixMultiply ( &matWorld, &matScale, &matWorld );
	}
	else
	{
		D3DXVECTOR3	vSize;
		vSize.x = m_vSizeXYZEnd.x * m_pThisGroup->m_fOut_Scale;
		vSize.y = m_vSizeXYZEnd.y * m_pThisGroup->m_fOut_Scale;
		vSize.z = m_vSizeXYZEnd.z * m_pThisGroup->m_fOut_Scale;

		if ( vSize.x < 0.001f )				goto _RETURN;
		if ( vSize.y < 0.001f )				goto _RETURN;
		if ( vSize.z < 0.001f )				goto _RETURN;

		D3DXMatrixScaling ( &matScale, vSize.x, vSize.y, vSize.z );
		D3DXMatrixMultiply ( &matWorld, &matScale, &matWorld );
	}

	//	Note: 외부에서 방향을 주고 이동도 줬다.
	if( m_pThisGroup->m_bAutoMove )
	{
		float		fPer	= (m_fGAge-m_fGBeginTime)/m_fGLifeTime;
		matWorld._41 = m_pThisGroup->m_vStartPos.x + (m_pThisGroup->m_vDirect.x*fPer);
		matWorld._42 = m_pThisGroup->m_vStartPos.y + (m_pThisGroup->m_vDirect.y*fPer);
		matWorld._43 = m_pThisGroup->m_vStartPos.z + (m_pThisGroup->m_vDirect.z*fPer);
	}

	//	Note: 회전 플래그 
	if( m_dwFlag & USEROTATE )
	{
		D3DXVECTOR3 vNormal = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3	vTarget;
		D3DXVec3Normalize( &vTarget, &m_pThisGroup->m_vGDelta );

		switch( m_nRotationType )
		{
			case 0: //	Note: 목표물의 방행을 중심으로 회전
					D3DXQuaternionRotationAxis( &qRotate, &vTarget, m_fAngle );
					D3DXMatrixRotationQuaternion( &matRotate, &qRotate );	
					break;
						
			case 1:	//	Note: 목표물 방향의 각 노말 벡터를 중심으로 회전					
					vNormal.x = vTarget.x ;
					vNormal.z = vTarget.z ;
					if( vTarget.y == 0.0f ) vTarget.y = 1.0f;
					D3DXVec3Cross( &vNormal, &vTarget, &vNormal );
					D3DXQuaternionRotationAxis( &qRotate, &vNormal, m_fAngle );
					D3DXMatrixRotationQuaternion( &matRotate, &qRotate );		
					break;

			case 2:	
					vNormal.x = vTarget.x ;
					vNormal.y = vTarget.y ;
					if( vTarget.z == 0.0f ) vTarget.z = 1.0f;
					D3DXVec3Cross( &vNormal, &vTarget, &vNormal );
					D3DXQuaternionRotationAxis( &qRotate, &vNormal, m_fAngle );
					D3DXMatrixRotationQuaternion( &matRotate, &qRotate );		
					break;

			case 3:
					vNormal.y = vTarget.x ;
					vNormal.z = vTarget.z ;
					if( vTarget.x == 0.0f ) vTarget.x = 1.0f;
					D3DXVec3Cross( &vNormal, &vTarget, &vNormal );
					D3DXQuaternionRotationAxis( &qRotate, &vNormal, m_fAngle );
					D3DXMatrixRotationQuaternion( &matRotate, &qRotate );						
					break;

			case 4: 
					D3DXMatrixRotationX( &matRotate, m_fAngle );
					break;

			case 5:
					D3DXMatrixRotationY( &matRotate, m_fAngle );
					break;

			case 6:
					D3DXMatrixRotationZ( &matRotate, m_fAngle );
					break;
		}	

		//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );	// 회전이 방향성 보다 먼저다. !!
		D3DXMatrixMultiply_RM2( matRotate, matWorld );	// 회전이 방향성 보다 먼저다. !!
	}

	if ( m_dwFlag & USEHEIGHT_MESH )
	{
		matWorld._42 += m_fHeight;
	}
	


	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

	if ( !m_pMesh || !m_pMesh->m_pLocalMesh )	goto _RETURN;



	m_pSavedRenderStats->Capture();
	m_pDrawRenderStats->Apply();


	if( m_dwFlag&USECULLNONE )	pd3dDevice->SetRenderState( D3DRS_CULLMODE,	D3DCULL_NONE );

	//	Note : 블렌딩 요소 셋팅
	//
	switch ( m_nBlend )
	{
	case 0 :	// Src+Dest (ADD)
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_ADD );
		break;
	case 1:		// Src-Dest (SUBTRACT)
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_SUBTRACT );
		break;
	case 2:		// Dest-Src (REVSUBTRACT)
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_REVSUBTRACT );
		break;
	case 3:		// Color MIN
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_MIN );
		break;
	case 4:		// Color MAX
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_MAX );
		break;
	case 5:		// Src+Dest (ALPHA)
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_ADD );
		break;
	case 6:		// Src (MY)
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,		TRUE );
		break;
	}

	//	Note : 빛의 세기 정도
	//
	switch ( m_nPower )
	{
	case 0 :
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
		break;
	case 1:
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );
		break;
	case 2:
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );
		break;
	}


	D3DMATERIALQ	sMaterial;
	sMaterial.Diffuse.r = 1.f;
	sMaterial.Diffuse.g = 1.f;
	sMaterial.Diffuse.b = 1.f;
	sMaterial.Diffuse.a = 1.f;

	sMaterial.Ambient.r = 1.f;
	sMaterial.Ambient.g = 1.f;
	sMaterial.Ambient.b = 1.f;
	sMaterial.Ambient.a = 1.f;

	sMaterial.Specular.r = 1.f;
	sMaterial.Specular.g = 1.f;
	sMaterial.Specular.b = 1.f;
	sMaterial.Specular.a = 1.f;

	sMaterial.Emissive.r = 0.f;
	sMaterial.Emissive.g = 0.f;
	sMaterial.Emissive.b = 0.f;
	sMaterial.Emissive.a = 0.f;

	sMaterial.Power = 1.f;

	if( m_dwFlag&USENORMAL2 )
	{
		sMaterial.Diffuse.r = m_cColorVar.r;
		sMaterial.Diffuse.g = m_cColorVar.g;
		sMaterial.Diffuse.b = m_cColorVar.b;
		sMaterial.Diffuse.a = m_cColorVar.a;

		sMaterial.Ambient.r = m_cColorVar.r;
		sMaterial.Ambient.g = m_cColorVar.g;
		sMaterial.Ambient.b = m_cColorVar.b;
		sMaterial.Ambient.a = m_cColorVar.a;

		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS,	TRUE );
	}
	else
	{
		D3DCOLOR	dwColor = ((DWORD)(m_cColorVar.a*255.f)<<24) +
								((DWORD)(m_cColorVar.r*255.f)<<16) +
								((DWORD)(m_cColorVar.g*255.f)<<8) +
								(DWORD)(m_cColorVar.b*255.f);

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR,	dwColor );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
	}


	//	Note: 메트릭스의 스케일과 머티리얼 보간 부분만 해주면 된다.
	//		  나머진 DxEffectGroup 부분에서 이동에 관한 부분을 메트릭스로 가져오기에 상관 없다.
	if( m_pMesh && m_pMesh->GetLocalMesh() ) 
	{
		if ( m_dwFlag & USETEXMOVE )
		{
			D3DXVECTOR2 vTex;
			vTex.x = m_vTexVelSum.x;
			vTex.y = m_vTexVelSum.y;
			m_pMesh->SetMoveTex ( vTex );
		}

		if ( m_dwFlag & USETEXSCALE )
		{

		}

		if( m_dwFlag & USEBLENDMESH )
		{
			float fFrame = cosf ( ((m_fGAge-m_fGBeginTime)/m_fGLifeTime)*D3DX_PI*m_fMorphRoopNum );
			m_pMesh->FrameMove ( fFrame );
		}

		if ( m_dwFlag & USESEQUENCE )	m_pMesh->SetSequenceTex ( m_nCol, m_nRow, m_iNowSprite );

		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		m_matWorld = matWorld;

		// Note : Get Texture	 !! Check !!
		if( !m_pCollTEX )	TextureManager::GetTexture( m_szCollTex.c_str(), m_pCollTEX );
		if( !m_pTexture )	
		{
			if( m_dwFlag & USEOTHERTEX )	TextureManager::GetTexture( m_strTexture.c_str(), m_pTexture );
		}

		if( m_pCollTEX )					m_pMesh->Render( pd3dDevice, m_pCollTEX, &sMaterial );
		else								m_pMesh->Render( pd3dDevice, m_pTexture, &sMaterial );

		//	Note : 추가 잔상을 만들기 위해서 ^^
		//
		if ( m_dwFlag & USEBLUR )
		{
			D3DXVECTOR3	vNormalize = m_pThisGroup->m_vGDelta;
			D3DXVec3Normalize ( &vNormalize, &vNormalize );
			vNormalize *= 20.f;

			for ( int	i=0; i<m_nBlurObject; i++ )
			{
				matWorld._41 -= vNormalize.x / m_nBlurObject;
				matWorld._42 -= vNormalize.y / m_nBlurObject;
				matWorld._43 -= vNormalize.z / m_nBlurObject;
				pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
				m_pMesh->Render( pd3dDevice, NULL, &sMaterial );
			}
		}
	}

	m_pSavedRenderStats->Apply();


_RETURN:
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 노드.
	//
	if( matWorld._44!=1.f)
	{
		float fSize = 1.f/matWorld._44;
		matWorld._11 = matWorld._11*fSize;	matWorld._12 = matWorld._12*fSize;	matWorld._13 = matWorld._13*fSize;
		matWorld._21 = matWorld._21*fSize;	matWorld._22 = matWorld._22*fSize;	matWorld._23 = matWorld._23*fSize;
		matWorld._31 = matWorld._31*fSize;	matWorld._32 = matWorld._32*fSize;	matWorld._33 = matWorld._33*fSize;
		matWorld._41 = matWorld._41*fSize;	matWorld._42 = matWorld._42*fSize;	matWorld._43 = matWorld._43*fSize;
		matWorld._44 = 1.f;
	}
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	return S_OK;	
}


HRESULT DxEffectMesh::FinalCleanup ()
{
	
	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

