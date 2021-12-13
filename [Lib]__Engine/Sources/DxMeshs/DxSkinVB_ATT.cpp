#include "pch.h"

#include "SerialFile.h"
#include "DxSkinMesh9_NORMAL.h"

#include "DxSkinVB_ATT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------------------------
// Name: struct DxSkinVB_ATT
// Desc: 
//--------------------------------------------------------------------------------------
DxSkinVB_ATT::DxSkinVB_ATT() :
	m_dwVertices(0L),
	m_dwFaces(0L),
	m_dwFVF(0L),
	m_dwSizeFVF(0L),
	m_dwAttrib(0L),
	m_dwScaleFVF(0L),
	m_dwScaleSizeFVF(0L),
	m_pScaleVB(NULL),
	m_pAttribTable(NULL)
{
}

DxSkinVB_ATT::~DxSkinVB_ATT()
{
	Clear();
}

void DxSkinVB_ATT::Clear()
{
	SAFE_RELEASE( m_pScaleVB );
	SAFE_DELETE_ARRAY( m_pAttribTable );
}

void DxSkinVB_ATT::CloneMeshNONINDEXED( IDirect3DDevice9 *pd3dDevice, LPD3DXMESH pMesh )
{
	HRESULT hr(S_OK);

	Clear();
	if( !pMesh )	return;

	m_dwVertices = pMesh->GetNumVertices();
	m_dwFaces = pMesh->GetNumFaces();	
	m_dwFVF = pMesh->GetFVF ();
	m_dwSizeFVF = D3DXGetFVFVertexSize( m_dwFVF );

	DWORD dwPosFVF = m_dwFVF&D3DFVF_POSITION_MASK;
	DWORD dwPosSize = D3DXGetFVFVertexSize( dwPosFVF );

	pMesh->GetAttributeTable( NULL, &m_dwAttrib );

	// Note : 정점 데이터를 32byte로 맞추기 위한 작업.
	m_dwScaleFVF = dwPosFVF;
	DWORD dwData = 32 - dwPosSize;
	switch( dwData )
	{
	case 4:
		m_dwScaleFVF |= D3DFVF_DIFFUSE;
		break;
	case 8:
		m_dwScaleFVF |= D3DFVF_TEX1;
		break;
	case 12:
		m_dwScaleFVF |= D3DFVF_DIFFUSE;
		m_dwScaleFVF |= D3DFVF_TEX1;
		break;
	case 16:
		m_dwScaleFVF |= D3DFVF_TEX2;
		break;
	case 20:
		m_dwScaleFVF |= D3DFVF_DIFFUSE;
		m_dwScaleFVF |= D3DFVF_TEX2;
		break;
	default:
		m_dwScaleFVF = m_dwFVF;
		break;
	};

	m_dwScaleSizeFVF = D3DXGetFVFVertexSize( m_dwScaleFVF );

	//Note : 생성
	hr = pd3dDevice->CreateVertexBuffer( m_dwScaleSizeFVF*m_dwVertices,  0L, m_dwScaleFVF, D3DPOOL_MANAGED, &m_pScaleVB, NULL );

	// Note : Clone
	LPD3DXMESH	pScaleMesh(NULL);
	hr = pMesh->CloneMeshFVF( D3DXMESH_MANAGED, m_dwScaleFVF, pd3dDevice, &pScaleMesh );

	//// Note :  VB
	BYTE *pbSrc = NULL;
	BYTE *pbDest = NULL;
	pbSrc = NULL;
	pbDest = NULL;
	hr = pScaleMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbSrc );
	if ( FAILED(hr) )	return;

	m_pScaleVB->Lock( 0, 0, (VOID**)&pbDest, D3DLOCK_DISCARD );
	memcpy( pbDest, pbSrc, m_dwScaleSizeFVF*m_dwVertices );
	m_pScaleVB->Unlock();

	hr = pScaleMesh->UnlockVertexBuffer();
	if ( FAILED(hr) )	return;

	m_pAttribTable = new D3DXATTRIBUTERANGE[ m_dwAttrib ];
	pMesh->GetAttributeTable( m_pAttribTable, &m_dwAttrib );

	SAFE_RELEASE( pScaleMesh );
}

void DxSkinVB_ATT::CloneMeshHLSL( IDirect3DDevice9 *pd3dDevice, LPD3DXMESH pMesh )
{
	HRESULT hr(S_OK);

	Clear();
	if( !pMesh )	return;

	m_dwVertices = pMesh->GetNumVertices();
	m_dwFaces = pMesh->GetNumFaces();	
	m_dwFVF = pMesh->GetFVF ();
	m_dwSizeFVF = D3DXGetFVFVertexSize( m_dwFVF );

	DWORD dwPosFVF = m_dwFVF&D3DFVF_POSITION_MASK;
	DWORD dwPosSize = D3DXGetFVFVertexSize( dwPosFVF );

	pMesh->GetAttributeTable( NULL, &m_dwAttrib );

	// Note : 정점 데이터를 32byte로 맞추기 위한 작업.
	m_dwScaleFVF = dwPosFVF;
	m_dwScaleFVF |= D3DFVF_NORMAL | D3DFVF_LASTBETA_UBYTE4;
	m_dwScaleSizeFVF = D3DXGetFVFVertexSize( m_dwScaleFVF );

	//Note : 생성
	pd3dDevice->CreateVertexBuffer( m_dwScaleSizeFVF*m_dwVertices,  0L, m_dwScaleFVF, D3DPOOL_MANAGED, &m_pScaleVB, NULL );

	// Note : Clone
	LPD3DXMESH	pScaleMesh(NULL);
	pMesh->CloneMeshFVF( D3DXMESH_MANAGED, m_dwScaleFVF, pd3dDevice, &pScaleMesh );

	//// Note :  VB
	BYTE *pbSrc = NULL;
	BYTE *pbDest = NULL;
	pbSrc = NULL;
	pbDest = NULL;
	hr = pScaleMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbSrc );
	if ( FAILED(hr) )	return;

	m_pScaleVB->Lock( 0, 0, (VOID**)&pbDest, D3DLOCK_DISCARD );
	memcpy( pbDest, pbSrc, m_dwScaleSizeFVF*m_dwVertices );
	m_pScaleVB->Unlock();

	hr = pScaleMesh->UnlockVertexBuffer();
	if ( FAILED(hr) )	return;

	// Note : m_pAttribTable
	m_pAttribTable = new D3DXATTRIBUTERANGE[ m_dwAttrib ];
	pMesh->GetAttributeTable( m_pAttribTable, &m_dwAttrib );

	SAFE_RELEASE( pScaleMesh );
}

void DxSkinVB_ATT::CloneMeshNONINDEXED( IDirect3DDevice9 *pd3dDevice, LPD3DXSKININFO pSkinInfo, DWORD *pAdjacency, LPD3DXMESH pMesh )
{
	HRESULT hr(S_OK);

	DWORD			iAttributeSW(0);
	DWORD			NumInfl(0);
	DWORD			NumAttributeGroups(0);
	LPD3DXBUFFER	pBoneCombinationBuf(NULL);
	LPD3DXMESH		pTempMesh(NULL);

	hr = pSkinInfo->ConvertToBlendedMesh
                                (
                                    pMesh,
                                    D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE, 
                                    pAdjacency, 
                                    NULL, NULL, NULL, 
                                    &NumInfl,
                                    &NumAttributeGroups, 
                                    &pBoneCombinationBuf, 
                                    &pTempMesh
                                );
    if (FAILED(hr))
        goto e_Exit;


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

		if (cInfl > DxSkinMesh9_NORMAL::m_dwMaxVertexBlendMatrices )
        {
            break;
        }
    }

    // if there is both HW and SW, add the Software Processing flag
    if (iAttributeSW < NumAttributeGroups)
    {
        LPD3DXMESH pMeshTmp;

        hr = pTempMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING | pTempMesh->GetOptions(), 
                                            pTempMesh->GetFVF(),
                                            pd3dDevice, &pMeshTmp);
        if (FAILED(hr))
        {
            goto e_Exit;
        }

        pTempMesh->Release();
        pTempMesh = pMeshTmp;
        pMeshTmp = NULL;
    }

e_Exit:
	CloneMeshNONINDEXED( pd3dDevice, pTempMesh );

	SAFE_RELEASE( pTempMesh );
	SAFE_RELEASE( pBoneCombinationBuf );

	return;
}

void DxSkinVB_ATT::CloneMeshHLSL( IDirect3DDevice9 *pd3dDevice, LPD3DXSKININFO pSkinInfo, DWORD *pAdjacency, LPD3DXMESH pMesh )
{
	HRESULT hr(S_OK);

	DWORD			NumPaletteEntries(0);
	DWORD			NumInfl(0);
	DWORD			NumAttributeGroups(0);
	LPD3DXBUFFER	pBoneCombinationBuf(NULL);
	LPD3DXMESH		pTempMesh(NULL);

	// Get palette size
    // First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
    // (96 - 9) /3 i.e. Maximum constant count - used constants 
    UINT MaxMatrices = 26;
    NumPaletteEntries = min(MaxMatrices, pSkinInfo->GetNumBones());


	// Note : HW 나쁘면 절대 HLSL은 쓰지 않을 것이다.
    bool UseSoftwareVP = false;
    DWORD Flags = D3DXMESHOPT_VERTEXCACHE | D3DXMESH_MANAGED;

    hr = pSkinInfo->ConvertToIndexedBlendedMesh
                                            (
                                            pMesh,
                                            Flags, 
                                            NumPaletteEntries, 
                                            pAdjacency, 
                                            NULL, NULL, NULL,             
                                            &NumInfl,
                                            &NumAttributeGroups, 
                                            &pBoneCombinationBuf, 
                                            &pTempMesh);
    if (FAILED(hr))
        goto e_Exit;


    // FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
    DWORD NewFVF = (pTempMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
    if (NewFVF != pTempMesh->GetFVF())
    {
        LPD3DXMESH pMeshAA;
        hr = pTempMesh->CloneMeshFVF( pTempMesh->GetOptions(), NewFVF, pd3dDevice, &pMeshAA );
        if (!FAILED(hr))
        {
            pTempMesh->Release();
            pTempMesh = pMeshAA;
            pMeshAA = NULL;
        }
    }

e_Exit:
	CloneMeshHLSL( pd3dDevice, pTempMesh );

	SAFE_RELEASE( pTempMesh );
	SAFE_RELEASE( pBoneCombinationBuf );

	return;
}

void DxSkinVB_ATT::CloneNormalMESH( IDirect3DDevice9 *pd3dDevice, LPD3DXMESH pMesh )
{
	HRESULT hr(S_OK);

	Clear();
	if( !pMesh )	return;

	m_dwVertices = pMesh->GetNumVertices();
	m_dwFaces = pMesh->GetNumFaces();
	m_dwSizeFVF = 76;
	m_dwScaleSizeFVF = 76;

	pMesh->GetAttributeTable( NULL, &m_dwAttrib );

	//Note : 생성
	pd3dDevice->CreateVertexBuffer( m_dwSizeFVF*m_dwVertices,  0L, 0, D3DPOOL_MANAGED, &m_pScaleVB, NULL );

	//// Note :  VB
	BYTE *pbSrc = NULL;
	BYTE *pbDest = NULL;
	pbSrc = NULL;
	pbDest = NULL;
	hr = pMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbSrc );
	if ( FAILED(hr) )	return;

	m_pScaleVB->Lock( 0, 0, (VOID**)&pbDest, D3DLOCK_DISCARD );
	memcpy( pbDest, pbSrc, m_dwSizeFVF*m_dwVertices );
	m_pScaleVB->Unlock();

	hr = pMesh->UnlockVertexBuffer();
	if ( FAILED(hr) )	return;

	// Note : m_pAttribTable
	m_pAttribTable = new D3DXATTRIBUTERANGE[ m_dwAttrib ];
	pMesh->GetAttributeTable( m_pAttribTable, &m_dwAttrib );
}

void DxSkinVB_ATT::CloneAttribTable( LPD3DXMESH pMesh )
{
	if( !pMesh )	return;

	m_dwVertices = pMesh->GetNumVertices();
	m_dwFaces = pMesh->GetNumFaces();

	pMesh->GetAttributeTable( NULL, &m_dwAttrib );

	// Note : m_pAttribTable
	SAFE_DELETE_ARRAY( m_pAttribTable );
	m_pAttribTable = new D3DXATTRIBUTERANGE[ m_dwAttrib ];
	pMesh->GetAttributeTable( m_pAttribTable, &m_dwAttrib );
}

void DxSkinVB_ATT::Save( CSerialFile& SFile )
{
	SFile << m_dwScaleFVF;
	SFile << m_dwScaleSizeFVF;

	BYTE *pbSrc = NULL;

	m_pScaleVB->Lock( 0, 0, (VOID**)&pbSrc, D3DLOCK_DISCARD );
	SFile.WriteBuffer( pbSrc, m_dwScaleSizeFVF*m_dwVertices );
	m_pScaleVB->Unlock();
}

void DxSkinVB_ATT::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, LPD3DXMESH pMesh )
{
	Clear();
	CloneAttribTable( pMesh );

	SFile >> m_dwScaleFVF;
	SFile >> m_dwScaleSizeFVF;

	////Note : 생성
	SAFE_RELEASE( m_pScaleVB );
	pd3dDevice->CreateVertexBuffer( m_dwScaleSizeFVF*m_dwVertices,  0L, m_dwScaleFVF, D3DPOOL_MANAGED, &m_pScaleVB, NULL );

	//// Note : Load
	BYTE *pbSrc = NULL;
	m_pScaleVB->Lock( 0, 0, (VOID**)&pbSrc, D3DLOCK_DISCARD );
	SFile.ReadBuffer( pbSrc, m_dwScaleSizeFVF*m_dwVertices );
	m_pScaleVB->Unlock();
}

