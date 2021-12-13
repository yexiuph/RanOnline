#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"
#include "./ShaderConstant.h"
#include "./DxDynamicVB.h"
#include "./EditMeshs.h"
#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectParticleSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note	:	디버그용,
extern BOOL		g_bEFFtoTEX;
BOOL			g_bDebugParticleRange = FALSE;


HRESULT DxEffectParticleSys::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;

	// Note : 랜덤 옵션에 랜덤 방향일 경우 계산을 해주어야 한다.
	if( m_bRandomPlay )
	{
		D3DXMatrixMultiply( &matComb, &m_matRandomR, &matComb );
	}

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
				std::string m_szCollTex = szName;
				TextureManager::LoadTexture( szName, pd3dDevice, m_pCollTEX, 0L, 0L, TRUE );
			}
		}
	}

	D3DXMATRIX	matView, matProj, matWVP;
	D3DXMATRIX	matEmitter, matWorld, matRotate, matRotateL, matScale, matTrans, matMesh, matParent, matChild, matIdentity;
	LPPARTICLE	pParticleNode;
	D3DXVECTOR3	lookAt;
	D3DXVECTOR3	vDir;
	D3DXVECTOR3	vPos_NowPrev;

	//	Note : 범위 설정 박스 나타내게 함 !!
	//
	if ( g_bDebugParticleRange && !g_bEFFtoTEX )
	{
		if ( m_dwFlag & USERANGE )
		{
			D3DXVECTOR3	vMax, vMin;
			vMax = D3DXVECTOR3 ( m_vRange.x*0.5f, m_vRange.y*0.5f, m_vRange.z*0.5f );
			vMin = D3DXVECTOR3 ( -m_vRange.x*0.5f, -m_vRange.y*0.5f, -m_vRange.z*0.5f );

			D3DXMatrixIdentity ( &matWorld );
			matWorld._41 = m_matLocal._41;
			matWorld._42 = m_matLocal._42;
			matWorld._43 = m_matLocal._43;
			D3DXVec3TransformCoord ( &vMax, &vMax, &matWorld );
			D3DXVec3TransformCoord ( &vMin, &vMin, &matWorld );

			EDITMESHS::RENDERAABB ( pd3dDevice, vMax, vMin );
		}
	}

	lookAt = D3DXVECTOR3(0.f,0.f,0.f);

	//	Note : 매트릭스 셋팅
	SetMatrix ( matWorld, matComb );

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

	if( m_dwFlag & USENEWEFF )
	{
		pParticleNode = m_pParticleHead;
		while ( pParticleNode )
		{
			if( pParticleNode->m_pSingleGroup )
			{
				pParticleNode->m_pSingleGroup->m_matWorld._41 = pParticleNode->m_vLocation.x;
				pParticleNode->m_pSingleGroup->m_matWorld._42 = pParticleNode->m_vLocation.y;
				pParticleNode->m_pSingleGroup->m_matWorld._43 = pParticleNode->m_vLocation.z;
				pParticleNode->m_pSingleGroup->Render( pd3dDevice );
			}
			pParticleNode = pParticleNode->m_pNext;
		}
	}

	//	Note : 본체의 힘에 의해 작은 파티클의 약간의 이동 움직임
	//
	if ( (matPrevComb._41==0.f) && (matPrevComb._42==0.f) && (matPrevComb._43==0.f) )
	{
		vPos_NowPrev = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
	}
	else
	{
		vPos_NowPrev = D3DXVECTOR3 ( matComb._41-matPrevComb._41, matComb._42-matPrevComb._42, matComb._43-matPrevComb._43 );
	}

	//	Note : 새로운 파티클을 계속 생산해 낸다.
	//
	NewParticleCreate( pd3dDevice, matComb, vPos_NowPrev );

	if ( m_uParticlesAlive==0 )					goto _RETURN;
	if ( m_dwFlag & USEMESH ) 
	if ( !m_pMesh || !m_pMesh->m_pLocalMesh )	goto _RETURN;

//	// Note : OPTMSingleParticle를 사용하기 위한 초기 작업.
//	OPTMSingleParticle::DATA* pData = OPTMSingleParticle::GetInstance().GetData();

	m_pSavedRenderStats->Capture();
	m_pDrawRenderStats->Apply();
	
	//	Note : 시퀀스 파일 일때
	//
	if ( m_dwFlag&USESEQUENCE )
	{
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	}
	else
	{
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, 0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_BORDER );
	}

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

	// Mesh (Normal(Use/No))와 Particle 일 경우 틀린것들 설정.
	if ( m_dwFlag & USEMESH )
	{
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( m_dwFlag&USENORMAL2 )
		{
			pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			TRUE );
		}
		else
		{
			pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
		}
	}
	else
	{
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
		
		if ( m_dwFlag & USEDIRECTION )
		{
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE,	D3DCULL_NONE );	// 범위. 
		}
	}


	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	m_matWorld = matWorld;

	//	Note : 시퀀스 적용
	//
	if ( m_dwFlag & USESEQUENCE )
	{
		if ( m_dwFlag & USESEQUENCELOOP )
		{
			if ( m_fTimeSum > m_fAniTime )
			{
				m_fTimeSum = 0.f;

				pParticleNode = m_pParticleHead;
				while ( pParticleNode )
				{
					if ( pParticleNode->m_bSequenceRoop )	pParticleNode->m_nNowSprite++;
					else									pParticleNode->m_nNowSprite--;

					pParticleNode = pParticleNode->m_pNext;
				}
			}

			pParticleNode = m_pParticleHead;
			while ( pParticleNode )
			{
				if ( pParticleNode->m_nNowSprite >= m_nAllSprite )
				{
					pParticleNode->m_bSequenceRoop	= !pParticleNode->m_bSequenceRoop;
					pParticleNode->m_nNowSprite		-= 2;
				}
				else if ( pParticleNode->m_nNowSprite < 0 )
				{
					pParticleNode->m_bSequenceRoop	= !pParticleNode->m_bSequenceRoop;
					pParticleNode->m_nNowSprite		+= 2;
				}


				pParticleNode = pParticleNode->m_pNext;
			}
		}
		else
		{
			if ( m_fTimeSum > m_fAniTime )
			{
				m_fTimeSum = 0.f;

				pParticleNode = m_pParticleHead;
				while ( pParticleNode )
				{
					pParticleNode->m_nNowSprite++;

					if ( pParticleNode->m_nNowSprite >= m_nAllSprite )		pParticleNode->m_nNowSprite = 0;

					pParticleNode = pParticleNode->m_pNext;
				}
			}
		}
		
		if ( !(m_dwFlag&USEMESH) )
		{
			//int nDrawPoints = 0;

			//D3DVERTEX *pVertexB;
			//hr = m_pVBuffer->Lock ( 0, 0, (VOID**)&pVertexB, D3DLOCK_DISCARD );
			//if ( FAILED(hr) )	
			//{
			//	m_pSavedRenderStats->Apply();
			//	goto _RETURN;
			//}

			//pParticleNode = m_pParticleHead;
			//while ( pParticleNode )
			//{
			//	pVertexB[(nDrawPoints*4)+0].vTex.x = pParticleNode->m_nNowSprite / (float)m_nCol;
			//	pVertexB[(nDrawPoints*4)+0].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow;

			//	pVertexB[(nDrawPoints*4)+1].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)m_nCol;
			//	pVertexB[(nDrawPoints*4)+1].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow;

			//	pVertexB[(nDrawPoints*4)+2].vTex.x = pParticleNode->m_nNowSprite / (float)m_nCol;
			//	pVertexB[(nDrawPoints*4)+2].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;

			//	pVertexB[(nDrawPoints*4)+3].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)m_nCol;
			//	pVertexB[(nDrawPoints*4)+3].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;

			//	nDrawPoints++;

			//	pParticleNode = pParticleNode->m_pNext;
			//}

			//m_pVBuffer->Unlock ();
		}
	}

	if( m_dwFlag & USETEXTURE ) 
	{
		if( !m_pTexture )	TextureManager::GetTexture( m_strTexture.c_str(), m_pTexture );
	}

	//	Note : 메쉬 뿌릴 것인가, 아니면 파티클로 뿌릴 것인가 ?
	//
	if( m_dwFlag & USEMESH )
	{
		pParticleNode = m_pParticleHead;
		while ( pParticleNode )
		{
			float fSize = pParticleNode->m_fSize*0.2f;
			
			if ( m_dwFlag&USEDIRECTION )	// 방향이 있는 물체면 (범위)를 적용한 매트릭스가 사라진다.
			{
				matTrans = DxBillboardLookAtDir ( &pParticleNode->m_vVelocity, &pParticleNode->m_vPrevLocation );
			}
			else
			{
				matTrans = pParticleNode->m_matRotate;
			}
			D3DXMatrixMultiply ( &matMesh, &matTrans, &matWorld  );	// 회전

			D3DXMatrixTranslation ( &matTrans, pParticleNode->m_vLocation.x, pParticleNode->m_vLocation.y, pParticleNode->m_vLocation.z );
			D3DXMatrixMultiply ( &matMesh, &matMesh, &matTrans );	// 이동	
			D3DXMatrixIdentity ( &matTrans );

			if ( fSize < 0.00001f )
			{
				pParticleNode = pParticleNode->m_pNext;
				continue;
			}

			matTrans._44 /= fSize;
			D3DXMatrixMultiply ( &matMesh, &matTrans, &matMesh );	// 스케일
			pd3dDevice->SetTransform ( D3DTS_WORLD, &matMesh );
			m_matWorld = matMesh;


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
				sMaterial.Diffuse.r = pParticleNode->m_cColor.x;
				sMaterial.Diffuse.g = pParticleNode->m_cColor.y;
				sMaterial.Diffuse.b = pParticleNode->m_cColor.z;
				sMaterial.Diffuse.a = pParticleNode->m_cColor.w;

				sMaterial.Ambient.r = pParticleNode->m_cColor.x;
				sMaterial.Ambient.g = pParticleNode->m_cColor.y;
				sMaterial.Ambient.b = pParticleNode->m_cColor.z;
				sMaterial.Ambient.a = pParticleNode->m_cColor.w;
			}
			else
			{
				D3DCOLOR	dwColor = ((DWORD)(pParticleNode->m_cColor.w*255.f)<<24) +
										((DWORD)(pParticleNode->m_cColor.x*255.f)<<16) +
										((DWORD)(pParticleNode->m_cColor.y*255.f)<<8) +
										(DWORD)(pParticleNode->m_cColor.z*255.f);
				pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR,	dwColor );
			}

			if ( m_dwFlag & USESEQUENCE )	m_pMesh->SetSequenceTex ( m_nCol, m_nRow, pParticleNode->m_nNowSprite );

			if( !m_pCollTEX )	TextureManager::GetTexture( m_szCollTex.c_str(), m_pCollTEX );

			if( m_pCollTEX )				m_pMesh->Render( pd3dDevice, m_pCollTEX, &sMaterial );
			else if ( m_dwFlag&USETEXTURE )	m_pMesh->Render( pd3dDevice, m_pTexture, &sMaterial );
			else							m_pMesh->Render( pd3dDevice, NULL, &sMaterial );

			pParticleNode = pParticleNode->m_pNext;
		}
	}
	else
	{
		if( !m_pTexture )
		{
			m_pSavedRenderStats->Apply();
			goto _RETURN;
		}

		pd3dDevice->SetTexture ( 0, m_pTexture );

		////	Note : 포인트 파티클을 사용 할 수 있을 때		// 현재 포인트 파티클 포기.... 그래픽 드라이버에 따라 느려지는 등.. 문제 많다.
		////
		//if ( DxEffectMan::GetInstance().GetUsePoint() && !DxEffectMan::GetInstance().GetUseSwShader() && 
		//	!( m_dwFlag & ( USETEXSCALE | USETEXROTATE | USESEQUENCE | USEGROUND ) ) )
		//{
		//	//	Note : SetFVF에 World와 WorldViewProjection 행렬 집어 넣기.
		//	//
		//	matView = DxViewPort::GetInstance().GetMatView();
		//	matProj = DxViewPort::GetInstance().GetMatProj();

		//	D3DXMatrixMultiply ( &matWVP, &matWorld, &matView );
		//	D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
		//	D3DXMatrixTranspose( &matWVP, &matWVP );
		//	pd3dDevice->SetVertexShaderConstant ( VSC_MATWVP_02, &matWVP, 4 );

		//	pd3dDevice->SetStreamSource ( 0, m_pParticleVBuffer, sizeof(D3DPARTICLEVERTEX) );
		//	pd3dDevice->SetFVF ( m_dwPointSpriteSh );

		//	D3DXMatrixIdentity ( &matIdentity );
		//	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

		//	D3DPARTICLEVERTEX *pVertexB;
		//	hr = m_pParticleVBuffer->Lock ( 0, 0, (VOID**)&pVertexB, D3DLOCK_DISCARD );
		//	if ( FAILED(hr) )
		//	{
		//		m_pSavedRenderStats->Apply();
		//		goto _RETURN;
		//	}

		//	int nDrawPoints = 0;
		//	pParticleNode = m_pParticleHead;
		//	while ( pParticleNode )
		//	{
		//		(pVertexB+nDrawPoints)->vPos = pParticleNode->m_vLocation;
		//		(pVertexB+nDrawPoints)->fSize = pParticleNode->m_fSize*_SCALEFACT_PAR;
		//		(pVertexB+nDrawPoints)->Diffuse = pParticleNode->m_cColor;

		//		++nDrawPoints;

		//		if ( nDrawPoints== MAX_PARTICLES_VB )
		//		{
		//			m_pParticleVBuffer->Unlock ();

		//			pd3dDevice->DrawPrimitive ( D3DPT_POINTLIST, 0, nDrawPoints );
		//			
		//			//nDrawPoints = 0;
		//			//m_pParticleVBuffer->Lock ( 0, 0, (VOID**)&pVertexB, D3DLOCK_DISCARD );
		//			//if ( FAILED(hr) )
					//{
					//	m_pSavedRenderStats->Apply();
					//	goto _RETURN;
					//}
		//		}
		//		pParticleNode = pParticleNode->m_pNext;
		//	}

		//	m_pParticleVBuffer->Unlock ();

		//	if ( nDrawPoints )		pd3dDevice->DrawPrimitive ( D3DPT_POINTLIST, 0, nDrawPoints );
		//}

		//	Note : 텍스쳐 늘이기를 사용한다.
		//
		if ( m_dwFlag & USETEXSCALE )
		{
			D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();

			D3DXMATRIX matLocal;
			D3DXVECTOR3	vBillPos, vDirc;

			//	Note : 상대 좌표일 때 또는 절대 좌표 일 때 ..!!
			//
			if ( (m_dwFlag&USEPARENTMOVE) )	
			{
				vBillPos.x = m_matLocal._41;
				vBillPos.y = m_matLocal._42;
				vBillPos.z = m_matLocal._43;
				D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
			}
			else
			{
				vBillPos.x = m_pThisGroup->m_matWorld._41;
				vBillPos.y = m_pThisGroup->m_matWorld._42;
				vBillPos.z = m_pThisGroup->m_matWorld._43;
			}

			if ( m_dwFlag&USEBILLBOARDUP )
				matLocal = DxBillboardLookAtHeight ( &vBillPos, DxViewPort::GetInstance().GetMatView() );		// 
			else if ( m_dwFlag&USEGROUND )
				D3DXMatrixBillboardGround( matLocal );
			else
				matLocal = DxBillboardLookAt ( &vBillPos, DxViewPort::GetInstance().GetMatView() );			//
			matLocal._41 = 0.f;
			matLocal._42 = 0.f;
			matLocal._43 = 0.f;

			D3DVERTEX	sVertex[4];
			DWORD dwVertexSize( sizeof(D3DVERTEX)*4 );
			DWORD dwColor(0);
			int nCount00(0);
			int nCount10(0);
			int nCount01(0);
			int nCount11(0);

			float fSize(0.f);
			float fScaleX(0.f);
			float fScaleY(0.f);

			D3DXVECTOR3 vAddPos(0.f,0.f,0.f);

			int nDrawPoints = 0;
			pParticleNode = m_pParticleHead;
			while ( pParticleNode )
			{
				if( MAX_PARTICLES==nDrawPoints )	break;

				if ( m_dwFlag&USEBILLBOARDALL )
				{
					if ( (m_dwFlag&USEPARENTMOVE) )	
					{
						vBillPos = pParticleNode->m_vLocation;
						D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
					}
					else
					{
						vBillPos = pParticleNode->m_vLocation;
					}
					if ( m_dwFlag&USEBILLBOARDUP )
						matLocal = DxBillboardLookAtHeight ( &vBillPos, DxViewPort::GetInstance().GetMatView() );		// 
					else if ( m_dwFlag&USEGROUND )
						D3DXMatrixBillboardGround( matLocal );
					else
						matLocal = DxBillboardLookAt ( &vBillPos, DxViewPort::GetInstance().GetMatView() );			//
					matLocal._41 = 0.f;
					matLocal._42 = 0.f;
					matLocal._43 = 0.f;
				}
				if ( m_dwFlag&USEDIRECTION )	// 방향이 있는 물체면 (범위)를 적용한 매트릭스가 사라진다.
				{
					if ( (m_dwFlag&USEPARENTMOVE) )	
					{
						D3DXVec3TransformCoord ( &vDirc, &pParticleNode->m_vLocation, &matWorld );
						vDir.x = m_pThisGroup->m_matWorld._41;
						vDir.y = m_pThisGroup->m_matWorld._42;
						vDir.z = m_pThisGroup->m_matWorld._43;
						matLocal = DxBillboardLookAtDir ( &vDirc, &vDir );
					}
					else
					{
						matLocal = DxBillboardLookAtDir ( &pParticleNode->m_vLocation, &pParticleNode->m_vPrevLocation );
					}
				}

				fSize = pParticleNode->m_fSize*0.25f;
				fScaleX = fSize*pParticleNode->m_vTexScale.x;
				fScaleY = fSize*pParticleNode->m_vTexScale.y;

				vAddPos = pParticleNode->m_vLocation;
				dwColor = D3DCOLOR_COLORVALUE ( pParticleNode->m_cColor.x, pParticleNode->m_cColor.y, pParticleNode->m_cColor.z, pParticleNode->m_cColor.w);
				nCount00 = (nDrawPoints*4)+0;


				if ( m_iCenterPoint == 0)	// 위
				{
					D3DXVECTOR3 vLocal = D3DXVECTOR3( -fScaleX, 0.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, 0.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( -fScaleX, -fScaleY*2.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, -fScaleY*2.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vLocal, &matLocal );
				}
				else if ( m_iCenterPoint == 1)	// 중심
				{
					D3DXVECTOR3 vLocal = D3DXVECTOR3( -fScaleX, fScaleY, 0.f );
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, fScaleY, 0.f );
					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( -fScaleX, -fScaleY, 0.f );
					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, -fScaleY, 0.f );
					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vLocal, &matLocal );
				}
				else if ( m_iCenterPoint == 2)	// 아래
				{
					D3DXVECTOR3 vLocal = D3DXVECTOR3( -fScaleX, fScaleY*2.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, fScaleY*2.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( -fScaleX, 0.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, 0.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vLocal, &matLocal );
				}

				if ( m_dwFlag & USESEQUENCE )
				{
					sVertex[0].vPos += vAddPos;
					sVertex[0].Diffuse = dwColor;
					sVertex[0].vTex.x = pParticleNode->m_nNowSprite / (float)m_nCol;
					sVertex[0].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow;

					sVertex[1].vPos += vAddPos;
					sVertex[1].Diffuse = dwColor;
					sVertex[1].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)m_nCol;
					sVertex[1].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow;

					sVertex[2].vPos += vAddPos;
					sVertex[2].Diffuse = dwColor;
					sVertex[2].vTex.x = pParticleNode->m_nNowSprite / (float)m_nCol;
					sVertex[2].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;

					sVertex[3].vPos += vAddPos;
					sVertex[3].Diffuse = dwColor;
					sVertex[3].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)m_nCol;
					sVertex[3].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;

					memcpy( &m_pVertices[ nCount00 ], sVertex, dwVertexSize );
				}
				else
				{
					sVertex[0].vPos += vAddPos;
					sVertex[0].Diffuse = dwColor;
					sVertex[0].vTex.x = 0.f;
					sVertex[0].vTex.y = 0.f;

					sVertex[1].vPos += vAddPos;
					sVertex[1].Diffuse = dwColor;
					sVertex[1].vTex.x = 1.f;
					sVertex[1].vTex.y = 0.f;

					sVertex[2].vPos += vAddPos;
					sVertex[2].Diffuse = dwColor;
					sVertex[2].vTex.x = 0.f;
					sVertex[2].vTex.y = 1.f;

					sVertex[3].vPos += vAddPos;
					sVertex[3].Diffuse = dwColor;
					sVertex[3].vTex.x = 1.f;
					sVertex[3].vTex.y = 1.f;

					memcpy( &m_pVertices[ nCount00 ], sVertex, dwVertexSize );
				}

				++nDrawPoints;

				pParticleNode = pParticleNode->m_pNext;
			}


			if ( nDrawPoints > 0 )
			{
				// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
				DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
				DWORD dwVertexSizeFULL = nDrawPoints*sizeof(D3DVERTEX)*4;
				if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
				{
					dwFlag = D3DLOCK_DISCARD; 
					DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
					DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
				}

				D3DVERTEX *pVertices;
				hr = DxDynamicVB::m_sVB_PDT.pVB->Lock ( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
				if ( FAILED(hr) )
				{
					m_pSavedRenderStats->Apply();
					goto _RETURN;
				}
				memcpy( pVertices, m_pVertices, dwVertexSizeFULL );
				DxDynamicVB::m_sVB_PDT.pVB->Unlock ();

				pd3dDevice->SetFVF ( D3DVERTEX::FVF );
				pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PDT.pVB, 0, sizeof(D3DVERTEX) );
				pd3dDevice->SetIndices ( m_pIBuffer );

				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, DxDynamicVB::m_sVB_PDT.nVertexCount, 0, nDrawPoints*4, 0, nDrawPoints*2 );

				// Note : 값을 더해줌. 
				DxDynamicVB::m_sVB_PDT.nVertexCount += nDrawPoints*4;
				DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;
			}
		}
		//	Note : 텍스쳐 회전 사용 한다 ? Yes
		//
		else if ( m_dwFlag & USETEXROTATE )
		{
			D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();

			D3DXVECTOR3	vBillPos;

			//	Note : 상대 좌표일 때 또는 절대 좌표 일 때 ..!!
			//
			if ( (m_dwFlag&USEPARENTMOVE) )	
			{
				vBillPos.x = m_matLocal._41;
				vBillPos.y = m_matLocal._42;
				vBillPos.z = m_matLocal._43;
				D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
			}
			else
			{
				vBillPos.x = m_pThisGroup->m_matWorld._41;
				vBillPos.y = m_pThisGroup->m_matWorld._42;
				vBillPos.z = m_pThisGroup->m_matWorld._43;
			}

			D3DXMATRIX	matLocal;

			if ( m_dwFlag&USEBILLBOARDUP )
				matLocal = DxBillboardLookAtHeight ( &vBillPos, DxViewPort::GetInstance().GetMatView() );		// 경우가 2가지다. 
			else if ( m_dwFlag&USEGROUND )
				D3DXMatrixBillboardGround( matLocal );
			else
				matLocal = DxBillboardLookAt ( &vBillPos, DxViewPort::GetInstance().GetMatView() );			//
			matLocal._41 = 0.f;
			matLocal._42 = 0.f;
			matLocal._43 = 0.f;

			D3DVERTEX	sVertex[4];
			DWORD dwVertexSize( sizeof(D3DVERTEX)*4 );
			DWORD dwColor(0);
			int nCount00(0);
			int nCount10(0);
			int nCount01(0);
			int nCount11(0);
			float		fSize(0.f);
			float		fTexRotateAngelSum(0.f);
			D3DXVECTOR3 vLocal(0.f,0.f,0.f);
			D3DXVECTOR3 vAddPos(0.f,0.f,0.f);
			D3DXMATRIX	matScale, matFinal;

			D3DXVECTOR3 vPos00( -1.f, 1.f, 0.f );
			D3DXVECTOR3 vPos10( 1.f, 1.f, 0.f );
			D3DXVECTOR3 vPos01( -1.f, -1.f, 0.f );
			D3DXVECTOR3 vPos11( 1.f, -1.f, 0.f );

			int nDrawPoints = 0;
			pParticleNode = m_pParticleHead;
			while ( pParticleNode )
			{
				if( MAX_PARTICLES==nDrawPoints )	break;

				if ( m_dwFlag&USEBILLBOARDALL )
				{
					if ( (m_dwFlag&USEPARENTMOVE) )	
					{
						vBillPos = pParticleNode->m_vLocation;
						D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
					}
					else
					{
						vBillPos = pParticleNode->m_vLocation;
					}
					if ( m_dwFlag&USEBILLBOARDUP )
						matLocal = DxBillboardLookAtHeight ( &vBillPos, DxViewPort::GetInstance().GetMatView() );		// 경우가 2가지다. 
					else if ( m_dwFlag&USEGROUND )
						D3DXMatrixBillboardGround( matLocal );
					else
						matLocal = DxBillboardLookAt ( &vBillPos, DxViewPort::GetInstance().GetMatView() );			//
					matLocal._41 = 0.f;
					matLocal._42 = 0.f;
					matLocal._43 = 0.f;
				}
				
				vAddPos = pParticleNode->m_vLocation;
				dwColor = D3DCOLOR_COLORVALUE ( pParticleNode->m_cColor.x, pParticleNode->m_cColor.y, pParticleNode->m_cColor.z, pParticleNode->m_cColor.w);
				nCount00 = (nDrawPoints*4)+0;

				fSize = pParticleNode->m_fSize*0.5f;
				fTexRotateAngelSum = pParticleNode->m_fTexRotateAngelSum;

				D3DXMatrixRotationZ( &matRotate, fTexRotateAngelSum );
				D3DXMatrixScaling( &matScale, fSize, fSize, fSize );
				D3DXMatrixTranslation( &matTrans, vAddPos.x, vAddPos.y, vAddPos.z );
				D3DXMatrixMultiply( &matFinal, &matScale, &matRotate );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matLocal );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matTrans );

				if ( m_dwFlag & USESEQUENCE )
				{
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vPos00, &matFinal );
					sVertex[0].Diffuse = dwColor;
					sVertex[0].vTex.x = pParticleNode->m_nNowSprite / (float)m_nCol;
					sVertex[0].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow;

					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vPos10, &matFinal );
					sVertex[1].Diffuse = dwColor;
					sVertex[1].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)m_nCol;
					sVertex[1].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow;

					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vPos01, &matFinal );
					sVertex[2].Diffuse = dwColor;
					sVertex[2].vTex.x = pParticleNode->m_nNowSprite / (float)m_nCol;
					sVertex[2].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;

					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vPos11, &matFinal );
					sVertex[3].Diffuse = dwColor;
					sVertex[3].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)m_nCol;
					sVertex[3].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;

					memcpy( &m_pVertices[ nCount00 ], sVertex, dwVertexSize );
				}
				else
				{
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vPos00, &matFinal );
					sVertex[0].Diffuse = dwColor;
					sVertex[0].vTex.x = 0.f;
					sVertex[0].vTex.y = 0.f;

					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vPos10, &matFinal );
					sVertex[1].Diffuse = dwColor;
					sVertex[1].vTex.x = 1.f;
					sVertex[1].vTex.y = 0.f;

					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vPos01, &matFinal );
					sVertex[2].Diffuse = dwColor;
					sVertex[2].vTex.x = 0.f;
					sVertex[2].vTex.y = 1.f;

					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vPos11, &matFinal );
					sVertex[3].Diffuse = dwColor;
					sVertex[3].vTex.x = 1.f;
					sVertex[3].vTex.y = 1.f;

					memcpy( &m_pVertices[ nCount00 ], sVertex, dwVertexSize );
				}

				++nDrawPoints;

				pParticleNode = pParticleNode->m_pNext;
			}

			if ( nDrawPoints > 0 )
			{
				// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
				DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
				DWORD dwVertexSizeFULL = nDrawPoints*sizeof(D3DVERTEX)*4;
				if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
				{
					dwFlag = D3DLOCK_DISCARD; 
					DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
					DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
				}

				D3DVERTEX *pVertices;
				hr = DxDynamicVB::m_sVB_PDT.pVB->Lock ( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
				if ( FAILED(hr) )
				{
					m_pSavedRenderStats->Apply();
					goto _RETURN;
				}
				memcpy( pVertices, m_pVertices, dwVertexSizeFULL );
				DxDynamicVB::m_sVB_PDT.pVB->Unlock ();

				pd3dDevice->SetFVF ( D3DVERTEX::FVF );
				pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PDT.pVB, 0, sizeof(D3DVERTEX) );
				pd3dDevice->SetIndices ( m_pIBuffer );

				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, DxDynamicVB::m_sVB_PDT.nVertexCount, 0, nDrawPoints*4, 0, nDrawPoints*2 );

				// Note : 값을 더해줌. 
				DxDynamicVB::m_sVB_PDT.nVertexCount += nDrawPoints*4;
				DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;
			}
		}
		else
		{
			D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();

			D3DXVECTOR3	vBillPos;

			//	Note : 상대 좌표일 때 또는 절대 좌표 일 때 ..!!
			//
			if ( (m_dwFlag&USEPARENTMOVE) )	
			{
				vBillPos.x = m_matLocal._41;
				vBillPos.y = m_matLocal._42;
				vBillPos.z = m_matLocal._43;
				D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
			}
			else
			{
				vBillPos.x = m_pThisGroup->m_matWorld._41;
				vBillPos.y = m_pThisGroup->m_matWorld._42;
				vBillPos.z = m_pThisGroup->m_matWorld._43;
			}

			D3DXMATRIX	matLocal;

			if ( m_dwFlag&USEBILLBOARDUP )
				matLocal = DxBillboardLookAtHeight ( &vBillPos, DxViewPort::GetInstance().GetMatView() );		//
			else if ( m_dwFlag&USEGROUND )
				D3DXMatrixBillboardGround( matLocal );
			else
				matLocal = DxBillboardLookAt ( &vBillPos, DxViewPort::GetInstance().GetMatView() );			//
			matLocal._41 = 0.f;
			matLocal._42 = 0.f;
			matLocal._43 = 0.f;

			D3DVERTEX	sVertex[4];
			DWORD dwVertexSize( sizeof(D3DVERTEX)*4 );
			DWORD dwColor(0);
			int nCount00(0);
			int nCount10(0);
			int nCount01(0);
			int nCount11(0);
			float fSize(0.f);
			D3DXVECTOR3 vLocal(0.f,0.f,0.f);
			D3DXVECTOR3 vAddPos(0.f,0.f,0.f);

			D3DXMATRIX	matScale, matFinal;

			D3DXVECTOR3 vPos00( -1.f, 1.f, 0.f );
			D3DXVECTOR3 vPos10( 1.f, 1.f, 0.f );
			D3DXVECTOR3 vPos01( -1.f, -1.f, 0.f );
			D3DXVECTOR3 vPos11( 1.f, -1.f, 0.f );

			int nDrawPoints = 0;
			pParticleNode = m_pParticleHead;
			while ( pParticleNode )
			{
				if( MAX_PARTICLES==nDrawPoints )	break;

				if ( m_dwFlag&USEBILLBOARDALL )
				{
					if ( (m_dwFlag&USEPARENTMOVE) )	
					{
						vBillPos = pParticleNode->m_vLocation;
						D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
					}
					else
					{
						vBillPos = pParticleNode->m_vLocation;
					}
					if ( m_dwFlag&USEBILLBOARDUP )
						matLocal = DxBillboardLookAtHeight ( &vBillPos, DxViewPort::GetInstance().GetMatView() );		//  
					else if ( m_dwFlag&USEGROUND )
						D3DXMatrixBillboardGround( matLocal );
					else
						matLocal = DxBillboardLookAt ( &vBillPos, DxViewPort::GetInstance().GetMatView() );			//
					matLocal._41 = 0.f;
					matLocal._42 = 0.f;
					matLocal._43 = 0.f;
				}

				fSize = pParticleNode->m_fSize*0.25f;

				vAddPos = pParticleNode->m_vLocation;
				dwColor = D3DCOLOR_COLORVALUE ( pParticleNode->m_cColor.x, pParticleNode->m_cColor.y, pParticleNode->m_cColor.z, pParticleNode->m_cColor.w);
				nCount00 = (nDrawPoints*4)+0;

				D3DXMatrixScaling( &matScale, fSize, fSize, fSize );
				D3DXMatrixTranslation( &matTrans, vAddPos.x, vAddPos.y, vAddPos.z );
				D3DXMatrixMultiply( &matFinal, &matScale, &matLocal );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matTrans );

				if ( m_dwFlag & USESEQUENCE )
				{
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vPos00, &matFinal );
					sVertex[0].Diffuse = dwColor;
					sVertex[0].vTex.x = pParticleNode->m_nNowSprite / (float)m_nCol;
					sVertex[0].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow;

					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vPos10, &matFinal );
					sVertex[1].Diffuse = dwColor;
					sVertex[1].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)m_nCol;
					sVertex[1].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow;

					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vPos01, &matFinal );
					sVertex[2].Diffuse = dwColor;
					sVertex[2].vTex.x = pParticleNode->m_nNowSprite / (float)m_nCol;
					sVertex[2].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;

					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vPos11, &matFinal );
					sVertex[3].Diffuse = dwColor;
					sVertex[3].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)m_nCol;
					sVertex[3].vTex.y = (pParticleNode->m_nNowSprite / m_nCol) / (float)m_nRow + 1.f/(float)m_nRow;

					memcpy( &m_pVertices[ nCount00 ], sVertex, dwVertexSize );
				}
				else
				{
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vPos00, &matFinal );
					sVertex[0].Diffuse = dwColor;
					sVertex[0].vTex.x = 0.f;
					sVertex[0].vTex.y = 0.f;

					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vPos10, &matFinal );
					sVertex[1].Diffuse = dwColor;
					sVertex[1].vTex.x = 1.f;
					sVertex[1].vTex.y = 0.f;

					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vPos01, &matFinal );
					sVertex[2].Diffuse = dwColor;
					sVertex[2].vTex.x = 0.f;
					sVertex[2].vTex.y = 1.f;

					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vPos11, &matFinal );
					sVertex[3].Diffuse = dwColor;
					sVertex[3].vTex.x = 1.f;
					sVertex[3].vTex.y = 1.f;

					memcpy( &m_pVertices[ nCount00 ], sVertex, dwVertexSize );
				}

				++nDrawPoints;

				pParticleNode = pParticleNode->m_pNext;
			}

			if ( nDrawPoints > 0 )
			{
				// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
				DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
				DWORD dwVertexSizeFULL = nDrawPoints*sizeof(D3DVERTEX)*4;
				if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
				{
					dwFlag = D3DLOCK_DISCARD; 
					DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
					DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
				}

				D3DVERTEX *pVertices;
				hr = DxDynamicVB::m_sVB_PDT.pVB->Lock( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
				if ( FAILED(hr) )
				{
					m_pSavedRenderStats->Apply();
					goto _RETURN;
				}
				memcpy( pVertices, m_pVertices, dwVertexSizeFULL );
				DxDynamicVB::m_sVB_PDT.pVB->Unlock ();

				pd3dDevice->SetFVF ( D3DVERTEX::FVF );
				pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PDT.pVB, 0, sizeof(D3DVERTEX) );
				pd3dDevice->SetIndices ( m_pIBuffer );

				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, DxDynamicVB::m_sVB_PDT.nVertexCount, 0, nDrawPoints*4, 0, nDrawPoints*2 );

				// Note : 값을 더해줌. 
				DxDynamicVB::m_sVB_PDT.nVertexCount += nDrawPoints*4;
				DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;
			}
		}
	}

	m_pSavedRenderStats->Apply();

_RETURN:

	//	Note : 예전의 위치를 기억 한다.
	matPrevComb = matComb;

	//	Note : 형제, 자식 노드.
	//
	D3DXMatrixTranslation ( &matTrans, m_vGLocation.x, m_vGLocation.y, m_vGLocation.z );
	D3DXMatrixMultiply ( &matWorld, &matTrans, &matWorld );
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	return hr;
}