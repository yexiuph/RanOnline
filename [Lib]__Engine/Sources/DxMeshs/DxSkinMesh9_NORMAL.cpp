#include "pch.h"


#include "SubPath.h"
#include "SerialFile.h"
#include "DxMaterial.h"
#include "DxMethods.h"
#include "DxRenderStates.h"

#include "DxSkinVB_ATT.h"
#include "DxSkinMeshMan.h"
#include "DxSkinPieceContainer.h"
#include "DxSkinMeshContainer9.h"

#include "DxSkinMesh9_NORMAL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD DxSkinMesh9_NORMAL::VERSION = 0x0100;

DWORD	DxSkinMesh9_NORMAL::m_dwMaxVertexBlendMatrices = 0;

extern DWORD SKINDRAWCOUNT;

DxSkinMesh9_NORMAL::DxSkinMesh9_NORMAL()
{
	StringCchCopy( m_szFileName, 32, _T("") );
	StringCchCopy( m_szSkinName, 32, _T("") );
	StringCchCopy( m_szSkeleton, 32, _T("") );
}

DxSkinMesh9_NORMAL::~DxSkinMesh9_NORMAL()
{
}

void DxSkinMesh9_NORMAL::CleanUp()
{

}

void DxSkinMesh9_NORMAL::StaticResetDevice( IDirect3DDevice9* pd3dDevice )
{
	D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );
	m_dwMaxVertexBlendMatrices = d3dCaps.MaxVertexBlendMatrices;
}

void DxSkinMesh9_NORMAL::StaticLost()
{
}

void DxSkinMesh9_NORMAL::DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, const CHARSETTING& sSetting )
{
	HRESULT hr;
    SMeshContainer *pMeshContainer = (SMeshContainer*)sSetting.pMeshContainerBase;

	DWORD dwNumInfl(0);
	UINT NumAttributeGroups(0);
	DWORD dwThisAttribId(0);
	DXMATERIALEX* pMaterialEx(NULL);
    UINT iMaterial;
    UINT NumBlend;
    UINT iAttrib;
    DWORD AttribIdPrev;
    LPD3DXBONECOMBINATION pBoneComb;
	LPD3DXBONECOMBINATION pBoneCur;

    UINT iMatrixIndex;
    D3DXMATRIXA16 matTemp, matBoneMatrix_UP;

	DWORD dwAlphaBlendEnable, dwAlphaOP;
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );

	BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
	if( m_fHeight!=0.f)	bHeight = TRUE;

    // first check for skinning
    if (pMeshContainer->pSkinInfo != NULL)
    {
        AttribIdPrev = UNUSED32; 
        pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

		// Draw using default vtx processing of the device (typically HW)
		NumAttributeGroups = pMeshContainer->NumAttributeGroups;
		for( iAttrib = 0; iAttrib < NumAttributeGroups; ++iAttrib )
		{
			pBoneCur = &pBoneComb[iAttrib];
			dwThisAttribId = pBoneCur->AttribId;
			pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
			if( !pMaterialEx )	continue;

			// Note : 텍스쳐가 있을 경우와 없을 경우
			if( !pMaterialEx->pTexture )
			{
				// Note : 먼저 로드가 되었는지 살펴본다.
				CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
			}

			NumBlend = 0;
			dwNumInfl = pMeshContainer->NumInfl;
			for (DWORD i = 0; i < dwNumInfl; ++i)
			{
				if (pBoneCur->BoneId[i] != UINT_MAX)
				{
					NumBlend = i;
				}
			}

			if( m_dwMaxVertexBlendMatrices >= NumBlend + 1 )
			{
				if( bHeight )
				{
					// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					for (DWORD i = 0; i < dwNumInfl; ++i)
					{
						iMatrixIndex = pBoneCur->BoneId[i];
						if (iMatrixIndex != UINT_MAX)
						{
							matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
							matBoneMatrix_UP._42 += m_fHeight;

							D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
							pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
						}
					}
				}
				else
				{
					// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					for (DWORD i = 0; i < dwNumInfl; ++i)
					{
						iMatrixIndex = pBoneCur->BoneId[i];
						if (iMatrixIndex != UINT_MAX)
						{
							D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
							pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
						}
					}
				}
				

				pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

				// lookup the material used for this subset of faces
				if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
				{
					pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D );
				//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[dwThisAttribId].pTexture ) );
					AttribIdPrev = dwThisAttribId;
				}

				BOOL bDraw(FALSE);
				switch( sSetting.emRDOP )
				{
				case CTOP_BASE:
					//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
					//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
					bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && sSetting.pmtrlSpecular[dwThisAttribId].bEffUse );
					if( bDraw )
					{
						++SKINDRAWCOUNT;
						RenderDefault ( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, sSetting.pmtrlPiece, sSetting.pmtrlSpecular );
					}
					break;

				case CTOP_NEWTEX:
					++SKINDRAWCOUNT;
					RenderNewTex( pd3dDevice, pMeshContainer, iAttrib );
					break;
				};
			}
		}

		// If necessary, draw parts that HW could not handle using SW
		if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
		{
			AttribIdPrev = UNUSED32; 
			V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
			for (iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; ++iAttrib)
			{
				pBoneCur = &pBoneComb[iAttrib];
				dwThisAttribId = pBoneCur->AttribId;
				pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
				if( !pMaterialEx )	continue;

				// Note : 텍스쳐가 있을 경우와 없을 경우
				if( !pMaterialEx->pTexture )
				{
					// Note : 먼저 로드가 되었는지 살펴본다.
					CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
				}

				NumBlend = 0;
				dwNumInfl = pMeshContainer->NumInfl;
				for (DWORD i = 0; i < dwNumInfl; ++i)
				{
					if (pBoneCur->BoneId[i] != UINT_MAX)
					{
						NumBlend = i;
					}
				}

				if( m_dwMaxVertexBlendMatrices < NumBlend + 1)
				{
					// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					for (DWORD i = 0; i < dwNumInfl; ++i)
					{
						iMatrixIndex = pBoneCur->BoneId[i];
						if (iMatrixIndex != UINT_MAX)
						{
							matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
							matBoneMatrix_UP._42 += m_fHeight;

							D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
							V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
						}
					}

					V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend) );

					// lookup the material used for this subset of faces
					if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
					{
						V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D ) );
					//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[dwThisAttribId].pTexture ) );
						AttribIdPrev = dwThisAttribId;
					}

					BOOL bDraw(FALSE);
					switch( sSetting.emRDOP )
					{
					case CTOP_BASE:
						//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
						//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
						bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && sSetting.pmtrlSpecular[dwThisAttribId].bEffUse );
						if( bDraw )
						{
							RenderDefault ( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, sSetting.pmtrlPiece, sSetting.pmtrlSpecular );
						}
						break;

					case CTOP_NEWTEX:
						RenderNewTex( pd3dDevice, pMeshContainer, iAttrib );
						break;
					};
				}
			}
			V( pd3dDevice->SetSoftwareVertexProcessing( FALSE ) );
		}
        V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0) );
    }
    else  // standard mesh, just draw it after setting material properties
    {
		if( sSetting.bWorldIdentity )
		{
			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity( &matIdentity );
			V( pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity) );
		}
		else
		{
			D3DXMATRIX matCombined;
			D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[1], pMeshContainer->ppBoneMatrixPtrs[2] );
			D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[0], &matCombined );

			matCombined._42 += m_fHeight;
			m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.

			V( pd3dDevice->SetTransform(D3DTS_WORLD, &matCombined) );
		}

		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[iMaterial];
			if( !pMaterialEx )	continue;

			// Note : 텍스쳐가 있을 경우와 없을 경우
			if( !pMaterialEx->pTexture )
			{
				// Note : 먼저 로드가 되었는지 살펴본다.
				CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
			}

			V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
		//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[iMaterial].pTexture ) );

			//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
			//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
			BOOL bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && sSetting.pmtrlSpecular[iMaterial].bEffUse );
			if ( bDraw )
			{
				RenderDefault ( pd3dDevice, pMeshContainer, iMaterial, iMaterial, sSetting.pmtrlPiece, sSetting.pmtrlSpecular );
			}
		}
    }

	//// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
}

void DxSkinMesh9_NORMAL::DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
							DXMATERIAL_CHAR_EFF* pmtrlSpecular, BOOL bWorldIdentity )
{
}

void DxSkinMesh9_NORMAL::DrawMeshCartoon( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPiece, RENDER_OPT emCTOP )
{
	HRESULT hr;
    SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;

	if( !pMeshContainer->m_pSlimMesh )				return;
	if( !pMeshContainer->m_pSlimMesh->m_pScaleVB )	return;

	DWORD dwNumInfl(0);
	UINT NumAttributeGroups(0);
	DWORD dwThisAttribId(0);
	DXMATERIALEX* pMaterialEx(NULL);
    UINT iMaterial;
    UINT NumBlend;
    UINT iAttrib;
    DWORD AttribIdPrev;
    LPD3DXBONECOMBINATION pBoneComb;
	LPD3DXBONECOMBINATION pBoneCur;

    UINT iMatrixIndex;
    D3DXMATRIXA16 matTemp, matBoneMatrix_UP;


	// Note : 일단 미리 해 놓는다.
	LPDIRECT3DINDEXBUFFER9	pIB;
	pMeshContainer->MeshData.pMesh->GetIndexBuffer( &pIB );
	pd3dDevice->SetIndices( pIB );


    // first check for skinning
    if (pMeshContainer->pSkinInfo != NULL)
    {
        AttribIdPrev = UNUSED32; 
        pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

		// Draw using default vtx processing of the device (typically HW)
		NumAttributeGroups = pMeshContainer->NumAttributeGroups;
		for( iAttrib = 0; iAttrib < NumAttributeGroups; ++iAttrib )
		{
			pBoneCur = &pBoneComb[iAttrib];
			dwThisAttribId = pBoneCur->AttribId;
			pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
			if( !pMaterialEx )	continue;

			// Note : 텍스쳐가 있을 경우와 없을 경우
			if( !pMaterialEx->pTexture )
			{
				// Note : 먼저 로드가 되었는지 살펴본다.
				CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
			}

			NumBlend = 0;
			dwNumInfl = pMeshContainer->NumInfl;
			for (DWORD i = 0; i < dwNumInfl; ++i)
			{
				if (pBoneCur->BoneId[i] != UINT_MAX)
				{
					NumBlend = i;
				}
			}

			if( m_dwMaxVertexBlendMatrices >= NumBlend + 1 )
			{
				// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
				for (DWORD i = 0; i < dwNumInfl; ++i)
				{
					iMatrixIndex = pBoneCur->BoneId[i];
					if (iMatrixIndex != UINT_MAX)
					{
						D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
						pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
					}
				}
				

				pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

				// lookup the material used for this subset of faces
				if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
				{
					pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D );
					AttribIdPrev = dwThisAttribId;
				}

				++SKINDRAWCOUNT;

				switch( emCTOP )
				{
				case CTOP_SHADOW:
					RenderCartoon( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece );
					break;
				case CTOP_EDGE:
					RenderEdge( pd3dDevice, pMeshContainer, iAttrib );
					break;
				case CTOP_NEWTEX:
					RenderNewTex( pd3dDevice, pMeshContainer, iAttrib );
					break;
				}
			}
		}

		// If necessary, draw parts that HW could not handle using SW
		if (pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
		{
			AttribIdPrev = UNUSED32; 
			V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
			for (iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; ++iAttrib)
			{
				pBoneCur = &pBoneComb[iAttrib];
				dwThisAttribId = pBoneCur->AttribId;
				pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
				if( !pMaterialEx )	continue;

				// Note : 텍스쳐가 있을 경우와 없을 경우
				if( !pMaterialEx->pTexture )
				{
					// Note : 먼저 로드가 되었는지 살펴본다.
					CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
				}

				NumBlend = 0;
				dwNumInfl = pMeshContainer->NumInfl;
				for (DWORD i = 0; i < dwNumInfl; ++i)
				{
					if (pBoneCur->BoneId[i] != UINT_MAX)
					{
						NumBlend = i;
					}
				}

				if( m_dwMaxVertexBlendMatrices < NumBlend + 1)
				{
					// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					for (DWORD i = 0; i < dwNumInfl; ++i)
					{
						iMatrixIndex = pBoneCur->BoneId[i];
						if (iMatrixIndex != UINT_MAX)
						{
							matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
							matBoneMatrix_UP._42 += m_fHeight;

							D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
							V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
						}
					}

					V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend) );

					// lookup the material used for this subset of faces
					if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
					{
						V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D ) );
					//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[dwThisAttribId].pTexture ) );
						AttribIdPrev = dwThisAttribId;
					}

					switch( emCTOP )
					{
					case CTOP_SHADOW:
						RenderCartoon( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece );
						break;
					case CTOP_EDGE:
						RenderEdge( pd3dDevice, pMeshContainer, iAttrib );
						break;
					case CTOP_NEWTEX:
						RenderNewTex( pd3dDevice, pMeshContainer, iAttrib );
						break;
					}
				}
			}
			V( pd3dDevice->SetSoftwareVertexProcessing( FALSE ) );
		}

        V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0) );
    }
    else  // standard mesh, just draw it after setting material properties
    {
		D3DXMATRIX matCombined;
		D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[1], pMeshContainer->ppBoneMatrixPtrs[2] );
		D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[0], &matCombined );

		matCombined._42 += m_fHeight;
		m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.

		V( pd3dDevice->SetTransform(D3DTS_WORLD, &matCombined) );

		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[iMaterial];
			if( !pMaterialEx )	continue;

			// Note : 텍스쳐가 있을 경우와 없을 경우
			if( !pMaterialEx->pTexture )
			{
				// Note : 먼저 로드가 되었는지 살펴본다.
				CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
			}

			V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
		//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[iMaterial].pTexture ) );

			switch( emCTOP )
			{
			case CTOP_SHADOW:
				RenderCartoon( pd3dDevice, pMeshContainer, iMaterial, iMaterial, pmtrlPiece );
				break;
			case CTOP_EDGE:
				RenderEdge( pd3dDevice, pMeshContainer, iMaterial );
				break;
			case CTOP_NEWTEX:
				RenderNewTex( pd3dDevice, pMeshContainer, iMaterial );
				break;
			}
		}
    }
}

void DxSkinMesh9_NORMAL::RenderCartoon( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
							SMATERIAL_PIECE *pmtrlPiece )
{
	LPDIRECT3DTEXTUREQ	pTexture(NULL);
	if( pmtrlPiece )
	{
		if ( pmtrlPiece[dwThisAttribId].m_pTexture )	pTexture = pmtrlPiece[dwThisAttribId].m_pTexture;
	}
	if( !pTexture ) 	pTexture = pmcMesh->pMaterialEx[dwThisAttribId].pTexture;

	pd3dDevice->SetTexture( 0, pTexture );

	pmcMesh->MeshData.pMesh->DrawSubset( ipattr );
}

void DxSkinMesh9_NORMAL::RenderEdge( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr )
{
	pd3dDevice->SetFVF( pmcMesh->m_pSlimMesh->m_dwScaleFVF );
	pd3dDevice->SetStreamSource( 0, pmcMesh->m_pSlimMesh->m_pScaleVB, 0, pmcMesh->m_pSlimMesh->m_dwScaleSizeFVF );

	const D3DXATTRIBUTERANGE* pAttribTable = pmcMesh->m_pSlimMesh->m_pAttribTable;
	const D3DXATTRIBUTERANGE& sAttribTable = pAttribTable[ pAttribTable[ipattr].AttribId ];

	pd3dDevice->DrawIndexedPrimitive
	(
		D3DPT_TRIANGLELIST, 
		0, 
		sAttribTable.VertexStart, 
		sAttribTable.VertexCount, 
		sAttribTable.FaceStart*3, 
		sAttribTable.FaceCount 
	);
}

void DxSkinMesh9_NORMAL::RenderNewTex( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr )
{
	pmcMesh->MeshData.pMesh->DrawSubset( ipattr );
}

void DxSkinMesh9_NORMAL::Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice )
{
	CleanUp();

	CSerialFile	SFile;
	std::string strName = DxSkinMeshMan::GetInstance().GetPath();
	strName += pName;

	strName = ChangeExtName( strName.c_str(), _T("enm") );

	BOOL bOpened = SFile.OpenFile( FOT_READ, strName.c_str() );
	if ( !bOpened )		return;

	DWORD dwVer;
	SFile >> dwVer;

    Load( SFile, pd3dDevice );

	SFile.CloseFile();
}

void DxSkinMesh9_NORMAL::Load( CSerialFile& SFile, IDirect3DDevice9 *pd3dDevice )
{
	DWORD dwCount(0L);
	SMeshContainer* pCur = m_pMeshContainerHead;
	while( pCur )
	{
		++dwCount;
		pCur = pCur->pGlobalNext;	
	}

	DWORD dwCountLOAD(0L);
	DWORD dwBufferSize(0L);
	SFile >> dwCountLOAD;
	SFile >> dwBufferSize;
	if( dwCount != dwCountLOAD )
	{
		SFile.SetOffSet( SFile.GetfTell()+dwBufferSize );
		return;
	}

	pCur = m_pMeshContainerHead;
	while( pCur )
	{
		pCur->LoadNORMAL( pd3dDevice, SFile );
		pCur = pCur->pGlobalNext;	
	}
}



