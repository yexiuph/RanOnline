#include "pch.h"

#include "RENDERPARAM.h"
#include "DxSkinDefine.h"

#include "DxSkinMesh9.h"
#include "DxSkinMesh9_CPU.h"
#include "DxSkinMesh9_NORMAL.h"
#include "DxSkinMesh9_HLSL.h"

#include "DxSkinMeshContainer9.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
*/

//----------------------------------------------------------------------------------------------------------------------------------
//													M		A		I		N
//----------------------------------------------------------------------------------------------------------------------------------
HRESULT SMeshContainer::GenerateSkinnedMesh( IDirect3DDevice9 *pd3dDevice, bool &bUseSoftwareVP )
{
    if( !pSkinInfo )	return S_OK;

    bUseSoftwareVP = false;

    SAFE_RELEASE( MeshData.pMesh );
    SAFE_RELEASE( pBoneCombinationBuf );

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
		GenerateSkinnedMeshCPU( pd3dDevice );
		break;

	case EMCRT_NORMAL:
		GenerateSkinnedMeshNON( pd3dDevice );
		break;

	case EMCRT_VERTEX:
	case EMCRT_PIXEL:
		GenerateSkinnedMeshHLSL( pd3dDevice, bUseSoftwareVP );
		break;

	default:
		return E_INVALIDARG;
		break;
	};

//
//#ifdef USE_SKINMESH_LOD
//
//	LPD3DXMESH pMeshLOD[2];
//
//	switch( RENDERPARAM::emCharRenderTYPE )
//	{
//	case EMCRT_SOFTWARE:
//		CreateLOD_SMeshCPU( pd3dDevice, MeshData.pMesh, pMeshLOD[0], pMeshLOD[1] );
//		m_sMeshLOD[0].Convert( pd3dDevice, pMeshLOD[0] );
//		m_sMeshLOD[1].Convert( pd3dDevice, pMeshLOD[1] );
//		break;
//
//	case EMCRT_NORMAL:
//		break;
//
//	case EMCRT_VERTEX:
//	case EMCRT_PIXEL:
//		break;
//
//	default:
//		return E_INVALIDARG;
//		break;
//	};
//
//	SAFE_RELEASE( pMeshLOD[0] );
//	SAFE_RELEASE( pMeshLOD[1] );
//
//#endif

    return S_OK;
}

//----------------------------------------------------------------------------------------------------------------------------------
//											N	O	N		I	N	D	E	X	E	D
//----------------------------------------------------------------------------------------------------------------------------------
void SMeshContainer::GenerateSkinnedMeshNON( IDirect3DDevice9 *pd3dDevice )
{
	HRESULT hr(S_OK);

	D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

	hr = pSkinInfo->ConvertToBlendedMesh
                                (
                                    pOrigMesh,
                                    D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE, 
                                    pAdjacency, 
                                    NULL, NULL, NULL, 
                                    &NumInfl,
                                    &NumAttributeGroups, 
                                    &pBoneCombinationBuf, 
                                    &MeshData.pMesh
                                );
    if (FAILED(hr))
	{
        return;
	}


    // If the device can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all meshes to it
    // Thus we split the mesh in two parts: The part that uses at most 2 matrices and the rest. The first is
    // drawn using the device's HW vertex processing and the rest is drawn using SW vertex processing.
    LPD3DXBONECOMBINATION rgBoneCombinations  = reinterpret_cast<LPD3DXBONECOMBINATION>(pBoneCombinationBuf->GetBufferPointer());

    // look for any set of bone combinations that do not fit the caps
    for (iAttributeSW = 0; iAttributeSW < NumAttributeGroups; iAttributeSW++)
    {
        DWORD cInfl   = 0;

        for (DWORD iInfl = 0; iInfl < NumInfl; iInfl++)
        {
            if (rgBoneCombinations[iAttributeSW].BoneId[iInfl] != UINT_MAX)
            {
                ++cInfl;
            }
        }

        if (cInfl > d3dCaps.MaxVertexBlendMatrices)
        {
            break;
        }
    }

    // if there is both HW and SW, add the Software Processing flag
    if (iAttributeSW < NumAttributeGroups)
    {
		BOOL bSoftWareNONINDEXED = TRUE;


        LPD3DXMESH pMeshTmp;

        hr = MeshData.pMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|MeshData.pMesh->GetOptions(), 
                                            MeshData.pMesh->GetFVF(),
                                            pd3dDevice, &pMeshTmp);
        if (FAILED(hr))
        {
            return;
        }

        MeshData.pMesh->Release();
        MeshData.pMesh = pMeshTmp;
        pMeshTmp = NULL;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------
//													H		L		S		L
//----------------------------------------------------------------------------------------------------------------------------------
void SMeshContainer::GenerateSkinnedMeshHLSL( IDirect3DDevice9 *pd3dDevice, bool &bUseSoftwareVP )
{
	HRESULT hr(S_OK);

	D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

	// Get palette size
    // First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
    // (96 - 9) /3 i.e. Maximum constant count - used constants 
    UINT MaxMatrices = 26;
    NumPaletteEntries = min(MaxMatrices, pSkinInfo->GetNumBones());

    DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
    if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
    {
        UseSoftwareVP = false;
        Flags |= D3DXMESH_MANAGED;
    }
    else
    {
        UseSoftwareVP = true;
        bUseSoftwareVP = true;
        Flags |= D3DXMESH_SYSTEMMEM;
    }

    SAFE_RELEASE(MeshData.pMesh);

    hr = pSkinInfo->ConvertToIndexedBlendedMesh
                                            (
                                            pOrigMesh,
                                            Flags, 
                                            NumPaletteEntries, 
                                            pAdjacency, 
                                            NULL, NULL, NULL,             
                                            &NumInfl,
                                            &NumAttributeGroups, 
                                            &pBoneCombinationBuf, 
                                            &MeshData.pMesh);
    if (FAILED(hr))
	{
		return;
	}


    // FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
    DWORD NewFVF = (MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
    if (NewFVF != MeshData.pMesh->GetFVF())
    {
        LPD3DXMESH pMesh;
        hr = MeshData.pMesh->CloneMeshFVF(MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
        if (!FAILED(hr))
        {
            MeshData.pMesh->Release();
            MeshData.pMesh = pMesh;
            pMesh = NULL;
        }
    }

    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
    LPD3DVERTEXELEMENT9 pDeclCur;
    hr = MeshData.pMesh->GetDeclaration(pDecl);
    if (FAILED(hr))
	{
		return;
	}

    // the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
    //   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
    //          this is more of a "cast" operation
    pDeclCur = pDecl;
    while (pDeclCur->Stream != 0xff)
    {
        if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
            pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
        pDeclCur++;
    }

    hr = MeshData.pMesh->UpdateSemantics(pDecl);
    if (FAILED(hr))
	{
        return;
	}

    // allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
	if( DxSkinMesh9_HLSL::g_NumBoneMatricesMax < pSkinInfo->GetNumBones() )
    {
        DxSkinMesh9_HLSL::g_NumBoneMatricesMax = pSkinInfo->GetNumBones();

  //      // Allocate space for blend matrices
		//if( DxSkinMesh9_HLSL::g_pBoneMatrices )
		//{
		//	free( DxSkinMesh9_HLSL::g_pBoneMatrices );
		//	DxSkinMesh9_HLSL::g_pBoneMatrices = NULL;
		//}
		//DxSkinMesh9_HLSL::g_pBoneMatrices = (D3DXMATRIXA16*)malloc( sizeof(D3DXMATRIXA16) * DxSkinMesh9_HLSL::g_NumBoneMatricesMax);
		delete[] DxSkinMesh9_HLSL::g_pBoneMatrices; 
        DxSkinMesh9_HLSL::g_pBoneMatrices  = new D3DXMATRIXA16[DxSkinMesh9_HLSL::g_NumBoneMatricesMax];
        if( DxSkinMesh9_HLSL::g_pBoneMatrices == NULL )
        {
            hr = E_OUTOFMEMORY;
            return;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------
//												S	O	F	T		W	A	R	E
//----------------------------------------------------------------------------------------------------------------------------------
void SMeshContainer::GenerateSkinnedMeshCPU( IDirect3DDevice9 *pd3dDevice )
{
	HRESULT hr(S_OK);

	SAFE_RELEASE(MeshData.pMesh);

	NumPaletteEntries = min(256, pSkinInfo->GetNumBones());
    hr = pSkinInfo->ConvertToIndexedBlendedMesh
                                            (
                                            pOrigMesh,
                                            D3DXMESHOPT_VERTEXCACHE | D3DXMESH_SYSTEMMEM, 
                                            NumPaletteEntries, 
                                            pAdjacency, 
                                            NULL, NULL, NULL,             
                                            &NumInfl,
                                            &NumAttributeGroups, 
                                            &pBoneCombinationBuf, 
                                            &MeshData.pMesh);
    if (FAILED(hr))
    {
        return;
	}


	//hr = pSkinInfo->ConvertToBlendedMesh
//                                 (
//                                     pOrigMesh,
//                                     D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE, 
//                                     pAdjacency, 
//                                     NULL, NULL, NULL, 
//                                     &NumInfl,
//                                     &NumAttributeGroups, 
//                                     &pBoneCombinationBuf, 
//                                     &MeshData.pMesh
//                                 );
//      if (FAILED(hr))
//          goto e_Exit;


	DWORD NewFVF = (MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
    if (NewFVF != MeshData.pMesh->GetFVF())
    {
        LPD3DXMESH pMesh;
        hr = MeshData.pMesh->CloneMeshFVF(MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
        if (!FAILED(hr))
        {
            MeshData.pMesh->Release();
            MeshData.pMesh = pMesh;
            pMesh = NULL;
        }
    }

	LPD3DXMESH pMesh;
    hr = MeshData.pMesh->CloneMeshFVF(MeshData.pMesh->GetOptions(), MeshData.pMesh->GetFVF(), pd3dDevice, &pMesh);

	LPD3DXMESH pMeshLOD[2];
	CreateLOD_SMeshCPU( pd3dDevice, pMesh, pAdjacency, pMeshLOD[0], pMeshLOD[1]);
	m_sMeshLOD[0].Convert( pd3dDevice, pMeshLOD[0] );
	m_sMeshLOD[1].Convert( pd3dDevice, pMeshLOD[1] );

	SAFE_RELEASE( pMeshLOD[0] );
	SAFE_RELEASE( pMeshLOD[1] );

	SAFE_RELEASE( pMesh );

	//{
	//	m_dwVertexNUM = MeshData.pMesh->GetNumVertices();
	//	m_dwFVF = MeshData.pMesh->GetFVF();
	//	DWORD dwSizeFVF = D3DXGetFVFVertexSize( m_dwFVF );
	//	DWORD dwVertices = MeshData.pMesh->GetNumVertices();
	//	
	//	SAFE_DELETE_ARRAY( m_pVB_DATA );

	//	BYTE* pSrc;
	//	MeshData.pMesh->LockVertexBuffer( 0L, (VOID**)&pSrc );

	//	switch( m_dwFVF )
	//	{
	//	case 0x1116:
	//		ConvertSkinnedMesh_0( m_pVB_DATA, pSrc, dwVertices );
	//		break;
	//	case 0x1118:
	//		ConvertSkinnedMesh_1( m_pVB_DATA, pSrc, dwVertices );
	//		break;
	//	case 0x111a:
	//		ConvertSkinnedMesh_2( m_pVB_DATA, pSrc, dwVertices );
	//		break;
	//	case 0x111c:
	//		ConvertSkinnedMesh_3( m_pVB_DATA, pSrc, dwVertices );
	//		break;
	//	case 0x111e:
	//		ConvertSkinnedMesh_4( m_pVB_DATA, pSrc, dwVertices );
	//		break;
	//	}

	//	MeshData.pMesh->UnlockVertexBuffer();
	//}

	{
		LPD3DXMESH pMesh;
        hr = MeshData.pMesh->CloneMeshFVF( D3DXMESH_WRITEONLY|D3DXMESH_SYSTEMMEM, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, pd3dDevice, &pMesh );
        if (!FAILED(hr))
        {
            MeshData.pMesh->Release();
            MeshData.pMesh = pMesh;
            pMesh = NULL;
        }
	}

    hr = MeshData.pMesh->GetAttributeTable(NULL, &NumAttributeGroups);
    if (FAILED(hr))
    {
        return;
	}

	SAFE_DELETE_ARRAY(pAttributeTable);
    pAttributeTable  = new D3DXATTRIBUTERANGE[NumAttributeGroups];
    if (pAttributeTable == NULL)
    {
        hr = E_OUTOFMEMORY;
        return;
    }

    hr = MeshData.pMesh->GetAttributeTable(pAttributeTable, NULL);
    if (FAILED(hr))
    {
        return;
	}

    // allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
	if( DxSkinMesh9_CPU::g_NumBoneMatricesMax < pSkinInfo->GetNumBones() )
    {
        DxSkinMesh9_CPU::g_NumBoneMatricesMax = pSkinInfo->GetNumBones();

  //      // Allocate space for blend matrices
  //      if( DxSkinMesh9_CPU::g_pBoneMatrices )
		//{
		//	free( DxSkinMesh9_CPU::g_pBoneMatrices );
		//	DxSkinMesh9_CPU::g_pBoneMatrices = NULL;
		//}
		//DxSkinMesh9_CPU::g_pBoneMatrices = (D3DXMATRIXA16*)malloc( sizeof(D3DXMATRIXA16) * DxSkinMesh9_CPU::g_NumBoneMatricesMax);
		delete[] DxSkinMesh9_CPU::g_pBoneMatrices; 
        DxSkinMesh9_CPU::g_pBoneMatrices  = new D3DXMATRIXA16[DxSkinMesh9_CPU::g_NumBoneMatricesMax];
        if( DxSkinMesh9_CPU::g_pBoneMatrices == NULL )
        {
            hr = E_OUTOFMEMORY;
            return;
        }
    }
}

