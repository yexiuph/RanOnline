#include "pch.h"

#include "DxRenderStates.h"
#include "DxMaterial.h"
#include "DxSkinPieceContainer.h"
#include "./TextureManager.h"
//#include "DxMethods.h"

#include "DxSkinMesh9.h"
#include "DxSkinMeshContainer9.h"
#include "DxMaterial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern DWORD SKINDRAWCOUNT;

void DxSkinMesh9::SetDrawState ( BOOL bOriginDraw, BOOL bAlpha, BOOL bShadow, BOOL bAlphaTex, BOOL bLevel, float fHeight )
{
	m_bOriginDraw	= bOriginDraw;
	m_bAlpha		= bAlpha;
	m_bShadow		= bShadow;
	m_bAlphaTex		= bAlphaTex;
	m_bLevel		= bLevel;
	m_fHeight		= fHeight;
}

void DxSkinMesh9::Init_Detail_Texture ()
{
	m_bOriginDraw	= TRUE;
	m_bAlpha		= FALSE;
	m_bShadow		= FALSE;
	m_bAlphaTex		= TRUE;
	m_bLevel		= FALSE;
	m_fHeight		= 0.f;
}

BOOL DxSkinMesh9::CheckLoadTexture( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pMeshContainer, DWORD dwThisAttribId )
{
	DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
	if( !pMaterialEx )	return FALSE;

	TextureManager::GetTexture( pMeshContainer->pMaterials[dwThisAttribId].pTextureFilename, pMaterialEx->pTexture );
			
	if( pMaterialEx->pTexture )
	{
		TextureManager::EM_TEXTYPE emType = TextureManager::GetTexType( pMeshContainer->pMaterials[dwThisAttribId].pTextureFilename );

		DWORD &dwFlags = pMaterialEx->dwFlags;
		if( emType==TextureManager::EMTT_ALPHA_HARD )			dwFlags |= DXMATERIALEX::FLAG_ALPHA_HARD;
		else if( emType==TextureManager::EMTT_ALPHA_SOFT )		dwFlags |= DXMATERIALEX::FLAG_ALPHA_SOFT;
		else if( emType==TextureManager::EMTT_ALPHA_SOFT01 )	dwFlags |= DXMATERIALEX::FLAG_ALPHA_SOFT;
		else if( emType==TextureManager::EMTT_ALPHA_SOFT02 )	dwFlags |= DXMATERIALEX::FLAG_ALPHA_SOFT;

		return TRUE;
	}

	return FALSE;
}

////--------------------------------------------------------------------------------------
//// Called to render a mesh in the hierarchy
////--------------------------------------------------------------------------------------
//void DxSkinMesh9::DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPiece, 
//									DXMATERIAL_CHAR_EFF* pmtrlSpecular, BOOL bWorldIdentity )
//{
//    HRESULT hr;
//    SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;
//
//	DWORD dwNumInfl(0);
//	UINT NumAttributeGroups(0);
//	DWORD dwThisAttribId(0);
//	DXMATERIALEX* pMaterialEx(NULL);
//    UINT iMaterial;
//    UINT NumBlend;
//    UINT iAttrib;
//    DWORD AttribIdPrev;
//    LPD3DXBONECOMBINATION pBoneComb;
//	LPD3DXBONECOMBINATION pBoneCur;
//
//    UINT iMatrixIndex;
//    UINT iPaletteEntry;
//    D3DXMATRIXA16 matTemp, matBoneMatrix_UP;
//
//	BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
//	if( m_fHeight!=0.f)	bHeight = TRUE;
//
//    // first check for skinning
//    if (pMeshContainer->pSkinInfo != NULL)
//    {
//        if( m_SkinningMethod == D3DNONINDEXED )
//        {
//            AttribIdPrev = UNUSED32; 
//            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
//
//			// Draw using default vtx processing of the device (typically HW)
//			NumAttributeGroups = pMeshContainer->NumAttributeGroups;
//			for( iAttrib = 0; iAttrib < NumAttributeGroups; ++iAttrib )
//			{
//				pBoneCur = &pBoneComb[iAttrib];
//				dwThisAttribId = pBoneCur->AttribId;
//				pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
//				if( !pMaterialEx )	continue;
//
//				// Note : 텍스쳐가 있을 경우와 없을 경우
//				if( !pMaterialEx->pTexture )
//				{
//					// Note : 먼저 로드가 되었는지 살펴본다.
//					CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
//				}
//
//				NumBlend = 0;
//				dwNumInfl = pMeshContainer->NumInfl;
//				for (DWORD i = 0; i < dwNumInfl; ++i)
//				{
//					if (pBoneCur->BoneId[i] != UINT_MAX)
//					{
//						NumBlend = i;
//					}
//				}
//
//				if( m_dwMaxVertexBlendMatrices >= NumBlend + 1 )
//				{
//					if( bHeight )
//					{
//						// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
//						for (DWORD i = 0; i < dwNumInfl; ++i)
//						{
//							iMatrixIndex = pBoneCur->BoneId[i];
//							if (iMatrixIndex != UINT_MAX)
//							{
//								matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
//								matBoneMatrix_UP._42 += m_fHeight;
//
//								D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
//								pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
//							}
//						}
//					}
//					else
//					{
//						// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
//						for (DWORD i = 0; i < dwNumInfl; ++i)
//						{
//							iMatrixIndex = pBoneCur->BoneId[i];
//							if (iMatrixIndex != UINT_MAX)
//							{
//								D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
//								pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
//							}
//						}
//					}
//					
//
//					pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);
//
//					// lookup the material used for this subset of faces
//					if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
//					{
//						pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D );
//					//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[dwThisAttribId].pTexture ) );
//						AttribIdPrev = dwThisAttribId;
//					}
//
//					//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
//					//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
//					BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && pmtrlSpecular[dwThisAttribId].bEffUse );
//					if ( bDraw )
//					{
//						++SKINDRAWCOUNT;
//						RenderDefault ( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
//					}
//				}
//			}
//
//			// If necessary, draw parts that HW could not handle using SW
//			if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
//			{
//				AttribIdPrev = UNUSED32; 
//				V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
//				for (iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; ++iAttrib)
//				{
//					pBoneCur = &pBoneComb[iAttrib];
//					dwThisAttribId = pBoneCur->AttribId;
//					pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
//					if( !pMaterialEx )	continue;
//
//					// Note : 텍스쳐가 있을 경우와 없을 경우
//					if( !pMaterialEx->pTexture )
//					{
//						// Note : 먼저 로드가 되었는지 살펴본다.
//						CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
//					}
//
//					NumBlend = 0;
//					dwNumInfl = pMeshContainer->NumInfl;
//					for (DWORD i = 0; i < dwNumInfl; ++i)
//					{
//						if (pBoneCur->BoneId[i] != UINT_MAX)
//						{
//							NumBlend = i;
//						}
//					}
//
//					if( m_dwMaxVertexBlendMatrices < NumBlend + 1)
//					{
//						// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
//						for (DWORD i = 0; i < dwNumInfl; ++i)
//						{
//							iMatrixIndex = pBoneCur->BoneId[i];
//							if (iMatrixIndex != UINT_MAX)
//							{
//								matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
//								matBoneMatrix_UP._42 += m_fHeight;
//
//								D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
//								V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
//							}
//						}
//
//						V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend) );
//
//						// lookup the material used for this subset of faces
//						if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
//						{
//							V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D ) );
//						//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[dwThisAttribId].pTexture ) );
//							AttribIdPrev = dwThisAttribId;
//						}
//
//						//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
//						//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
//						BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && pmtrlSpecular[dwThisAttribId].bEffUse );
//						if ( bDraw )
//						{
//							RenderDefault ( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
//						}
//					}
//				}
//				V( pd3dDevice->SetSoftwareVertexProcessing( FALSE ) );
//			}
//
//            V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0) );
//        }
//        else if (m_SkinningMethod == D3DINDEXEDHLSLVS) 
//        {
//			D3DXMATRIX matViewProj, matView, matProj;
//			pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
//			pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
//			D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
//			m_pEffect->SetMatrix( "mViewProj", &matViewProj );
//
//			// Set Light for vertex shader
//			D3DXVECTOR4 vLightDir( 0.0f, 1.0f, -1.0f, 0.0f );
//			D3DXVec4Normalize( &vLightDir, &vLightDir );
//			V( pd3dDevice->SetVertexShaderConstantF(1, (float*)&vLightDir, 1) );
//			V( m_pEffect->SetVector( "lhtDir", &vLightDir) );
//
//            if (pMeshContainer->UseSoftwareVP)
//            {
//                // If hw or pure hw vertex processing is forced, we can't render the
//                // mesh, so just exit out.  Typical applications should create
//                // a device with appropriate vertex processing capability for this
//                // skinning method.
//                if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
//                    return;
//
//                V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
//            }
//
//            pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
//            for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; ++iAttrib)
//            { 
//				if( bHeight )
//				{
//					 // first calculate all the world matrices
//					for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
//					{
//						iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
//						if (iMatrixIndex != UINT_MAX)
//						{
//							//	char shadow map - 높이 조절.
//							matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
//							matBoneMatrix_UP._42 += m_fHeight;
//
//							D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
//							//D3DXMatrixMultiply(&m_pBoneMatrices[iPaletteEntry], &matTemp, &m_matView);
//						}
//					}
//				}
//				else
//				{
//					// first calculate all the world matrices
//					for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
//					{
//						iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
//						if (iMatrixIndex != UINT_MAX)
//						{
//							D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
//						}
//					}
//				}
//                V( m_pEffect->SetMatrixArray( "mWorldMatrixArray", g_pBoneMatrices, pMeshContainer->NumPaletteEntries) );
//
//                // Sum of all ambient and emissive contribution
//                D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
//                D3DXCOLOR color2(.25, .25, .25, 1.0);
//                D3DXCOLOR ambEmm;
//                D3DXColorModulate(&ambEmm, &color1, &color2);
//                ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);
//
//                // set material color properties 
//                V( m_pEffect->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse)) );
//                V( m_pEffect->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm) );
//
//                // setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
//		//		V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[pBoneComb[iAttrib].AttribId].pTexture ) );
//
//                // Set CurNumBones to select the correct vertex shader for the number of bones
//                V( m_pEffect->SetInt( "CurNumBones", pMeshContainer->NumInfl -1) );
//
//                // Start the effect now all parameters have been updated
//                UINT numPasses;
//                V( m_pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE ) ); 
//                for( UINT iPass = 0; iPass < numPasses; iPass++ )
//                {
//                    V( m_pEffect->BeginPass( iPass ) );
//
//					 // draw the subset with the current world matrix palette and material state
//                 //   V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );
//
//                    //	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
//					//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
//					DWORD dwThisAttribId = pBoneComb[iAttrib].AttribId;
//					BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && pmtrlSpecular[dwThisAttribId].bEffUse );
//					if ( bDraw )
//					{
//						++SKINDRAWCOUNT;
//						RenderDefault ( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece, pmtrlSpecular );
//					}
//
//                    V( m_pEffect->EndPass() );
//                }
//
//                V( m_pEffect->End() );
//
//                V( pd3dDevice->SetVertexShader(NULL) );
//            }
//
//            // remember to reset back to hw vertex processing if software was required
//            if (pMeshContainer->UseSoftwareVP)
//            {
//                V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
//            }
//        }
//        else // bug out as unsupported mode
//        {
//            return;
//        }
//    }
//    else  // standard mesh, just draw it after setting material properties
//    {
//		if( bWorldIdentity )
//		{
//			D3DXMATRIX matIdentity;
//			D3DXMatrixIdentity( &matIdentity );
//			V( pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity) );
//		}
//		else
//		{
//			D3DXMATRIX matCombined;
//			D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[1], pMeshContainer->ppBoneMatrixPtrs[2] );
//			D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[0], &matCombined );
//
//			matCombined._42 += m_fHeight;
//			m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.
//
//			V( pd3dDevice->SetTransform(D3DTS_WORLD, &matCombined) );
//		}
//
//		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
//		{
//			DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[iMaterial];
//			if( !pMaterialEx )	continue;
//
//			// Note : 텍스쳐가 있을 경우와 없을 경우
//			if( !pMaterialEx->pTexture )
//			{
//				// Note : 먼저 로드가 되었는지 살펴본다.
//				CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
//			}
//
//			V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
//		//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[iMaterial].pTexture ) );
//
//			//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
//			//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
//			BOOL bDraw = m_bOriginDraw || ( pmtrlSpecular && pmtrlSpecular[iMaterial].bEffUse );
//			if ( bDraw )
//			{
//				RenderDefault ( pd3dDevice, pMeshContainer, iMaterial, iMaterial, pmtrlPiece, pmtrlSpecular );
//			}
//		}
//    }
//
//	// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
//}

void DxSkinMesh9::DrawFrameMesh( IDirect3DDevice9 *pd3dDevice, SMeshContainer* pMeshContainer, SMATERIAL_PIECE* pmtrlPiece, LPD3DXMATRIX pMatrix )
{
	pMatrix->_42 += m_fHeight;
	pd3dDevice->SetTransform( D3DTS_WORLD, pMatrix );

	for( DWORD iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; ++iMaterial )
	{
		DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[iMaterial];
		if( !pMaterialEx )	continue;

		// Note : 텍스쳐가 있을 경우와 없을 경우
		if( !pMaterialEx->pTexture )
		{
			// Note : 먼저 로드가 되었는지 살펴본다.
			CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
		}

		pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D );
	//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[iMaterial].pTexture ) );

		//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
		//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
		if( m_bOriginDraw )
		{
			RenderDefault( pd3dDevice, pMeshContainer, iMaterial, iMaterial, pmtrlPiece, NULL );
		}
	}

	// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
}

//--------------------------------------------------------------------------------------
// Called to render a frame in the hierarchy
//--------------------------------------------------------------------------------------
void DxSkinMesh9::DrawFrame ( IDirect3DDevice9 *pd3dDevice, LPD3DXFRAME pFrame )
{
    LPD3DXMESHCONTAINER pMeshContainer;

    pMeshContainer = pFrame->pMeshContainer;
    while (pMeshContainer != NULL)
    {
		CHARSETTING sCharSetting;
		sCharSetting.pMeshContainerBase = pMeshContainer;
		sCharSetting.pmtrlPiece			= NULL;
		sCharSetting.pmtrlSpecular		= NULL;
		sCharSetting.bWorldIdentity		= FALSE;
		sCharSetting.emRDOP				= CTOP_BASE;

        DrawMeshContainer( pd3dDevice, sCharSetting );

        pMeshContainer = pMeshContainer->pNextMeshContainer;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        DrawFrame( pd3dDevice, pFrame->pFrameSibling);
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        DrawFrame( pd3dDevice, pFrame->pFrameFirstChild );
    }
}

HRESULT DxSkinMesh9::RenderDefault ( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
							SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular )
{
	HRESULT	hr = S_OK;

	BOOL bZBiasChange(FALSE);
	LPDIRECT3DTEXTUREQ pTexture = pmcMesh->SetMaterial( pd3dDevice, dwThisAttribId, bZBiasChange, pmtrlPiece, pmtrlSpecular );
	if( !pTexture )
	{
		if( dwThisAttribId >= pmcMesh->NumMaterials )
		{
			CDebugSet::ToLogFile( "DxSkinMesh9::RenderDefault() -- pmcMesh->SetMaterial() -- Fail" );
			return S_OK;
		}
	}

	// Note : pmtrlSpecular 이것이 참일 경우는 원본을 뿌릴 경우가 아니다.
	if( !pmtrlSpecular && !m_bAlpha )
	{
		if( pmcMesh->IsAlphaTex_HARD(dwThisAttribId) && m_bAlphaTex )
		{
			//m_nCheckNumSB = 1;
			DxRenderStates::GetInstance().SetOnAlphaApply( pd3dDevice );
		}
		else if( pmcMesh->IsAlphaTex_SOFT(dwThisAttribId) && m_bAlphaTex )
		{
			//m_nCheckNumSB = 2;
			DxRenderStates::GetInstance().SetOnSoftAlphaApply( pd3dDevice );
		}
	}

	if( m_bLevel )			// 렙제가 된다면 이렇게 하는 것이 부하를 줄이게 해 준다.
	{
		LPDIRECT3DTEXTUREQ pTexture(NULL);

		if ( pmtrlPiece )
		{
			if ( pmtrlPiece[dwThisAttribId].m_pTexture )	pTexture = pmtrlPiece[dwThisAttribId].m_pTexture;
		}
		if( !pTexture )
		{
			pTexture = pmcMesh->pMaterialEx[dwThisAttribId].pTexture;
		}

		pd3dDevice->SetTexture ( 1, pTexture );		
	}

	DWORD	dwAlphaBlendEnable, dwAlphaARG1, dwAlphaARG2, dwAlphaOP;
	if( m_bAlpha )
	{
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1,	&dwAlphaARG1 );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2,	&dwAlphaARG2 );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
	}

	//// draw the subset now that the correct material and matrices are loaded
    pmcMesh->MeshData.pMesh->DrawSubset(ipattr);

	if( m_bAlpha )
	{
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	dwAlphaARG1 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	dwAlphaARG2 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
	}

	if( m_bLevel )		pd3dDevice->SetTexture ( 1, NULL );
	if( bZBiasChange )	pmcMesh->ReSetMaterial ( pd3dDevice );

	return hr;
}