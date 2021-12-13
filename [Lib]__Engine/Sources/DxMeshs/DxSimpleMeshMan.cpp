#include "pch.h"
#include <dxfile.h>
#include "./DxSimpleMeshMan.h"
#include "./StlFunctions.h"
#include "./COLLISION.h"
#include "./DxRenderStates.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-----------------------------------------------------------------------------[DxSimMesh]
//
DxSimMesh::DxSimMesh () :
	m_dwRef(0),
	m_dwFlag(NULL),

	m_vCenter(0,0,0),
	m_fRadius(0),

	m_vMax(0,0,0),
	m_vMin(0,0,0),

	m_pd3dDevice(NULL),
	m_pLocalMesh(NULL),
	m_dwNumMaterials(0L),
	m_pMaterials(NULL),
	m_pTextures(NULL),
	m_emTexType(NULL),
	m_bUseMaterials(TRUE),

	m_bMorph(FALSE),
	m_pTexUV(NULL)
{
	m_pVertex[0] = NULL;
	m_pVertex[1] = NULL;
	m_pVertex[2] = NULL;

	m_pNormal[0] = NULL;
	m_pNormal[1] = NULL;
	m_pNormal[2] = NULL;

	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));
}

DxSimMesh::~DxSimMesh()
{
	Destroy();

    SAFE_DELETE_ARRAY ( m_pVertex[0] );
	SAFE_DELETE_ARRAY ( m_pVertex[1] );
	SAFE_DELETE_ARRAY ( m_pVertex[2] );

	SAFE_DELETE_ARRAY ( m_pNormal[0] );
	SAFE_DELETE_ARRAY ( m_pNormal[1] );
	SAFE_DELETE_ARRAY ( m_pNormal[2] );

	SAFE_DELETE_ARRAY ( m_pTexUV );
}

HRESULT DxSimMesh::Create ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF, const char* szFilename, const char* szFilename1, const char* szFilename2 )
{
	if( !szFilename || !szFilename1 || !szFilename2 )
	{
		CDebugSet::ToListView( "DxSimMesh::Create() -- szFilename -- NULL" );
		return S_OK;
	}

	if( !pd3dDevice )
	{
		CDebugSet::ToListView( "DxSimMesh::Create() -- pd3dDevice -- NULL" );
		return S_OK;
	}

	char			szPath[MAX_PATH] = "";
	LPD3DXBUFFER	pAdjacencyBuffer = NULL;
	LPD3DXBUFFER	pMtrlBuffer = NULL;
	HRESULT			hr;

	UINT nVERSIZE = D3DXGetFVFVertexSize ( dwFVF );

	m_pd3dDevice = pd3dDevice;
	m_bMorph = (strlen(szFilename)>=0) && (strlen(szFilename1)>0) && (strlen(szFilename2)>0);

	if ( m_bMorph )
	{
		DWORD dwVERTICES[3] = { 0, 0, 0 };
		const char *szFILENAME[3] = { szFilename, szFilename1, szFilename2 };
		LPD3DXMESH pLOCALMESH[3] = { NULL, NULL, NULL };

		for ( DWORD i=0; i<3; ++i )
		{
			StringCchCopy( szPath, MAX_PATH, DxSimpleMeshMan::GetInstance().GetPath() );
			StringCchCat( szPath, MAX_PATH, szFILENAME[i] );

			SAFE_RELEASE( pAdjacencyBuffer );
			SAFE_RELEASE( pMtrlBuffer );

			// Load the mesh
			if ( FAILED( hr = D3DXLoadMeshFromX ( szPath, D3DXMESH_MANAGED, pd3dDevice, 
												&pAdjacencyBuffer, &pMtrlBuffer, NULL,
												&m_dwNumMaterials, &pLOCALMESH[i] ) ) )
			{
				SAFE_RELEASE( pLOCALMESH[i] );

				CDebugSet::ToLogFile ( "ERROR : D3DXLoadMeshFromX() %s, hr: %s", szFILENAME[i], DXGetErrorString9(hr) );

				return hr;
			}

			// Optimize the mesh for performance
			if ( FAILED( hr = pLOCALMESH[i]->OptimizeInplace(
								D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
								(DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL ) ) )
			{
				SAFE_RELEASE( pAdjacencyBuffer );
				SAFE_RELEASE( pMtrlBuffer );

				CDebugSet::ToLogFile ( "ERROR : OptimizeInplace() %s, hr: %s", szFilename, DXGetErrorString9(hr) );
				return hr;
			}

			pLOCALMESH[i]->CloneMeshFVF ( D3DXMESH_MANAGED, dwFVF, pd3dDevice, &pLOCALMESH[i] );

			dwVERTICES[i] = pLOCALMESH[i]->GetNumVertices();
			m_pVertex[i] = new D3DXVECTOR3[dwVERTICES[i]];

			BYTE *pBuffer(NULL);
			pLOCALMESH[i]->LockVertexBuffer ( 0L, (VOID**)&pBuffer );
			for ( DWORD n=0; n<dwVERTICES[i]; ++n )
			{
				m_pVertex[i][n] = *(D3DXVECTOR3*)( pBuffer + nVERSIZE*n );
			}
			pLOCALMESH[i]->UnlockVertexBuffer ();

			if ( dwFVF&D3DFVF_NORMAL )
			{
				m_pNormal[i] = new D3DXVECTOR3[dwVERTICES[i]];

				BYTE *pBuffer(NULL);
				pLOCALMESH[i]->LockVertexBuffer ( 0L, (VOID**)&pBuffer );
				for ( DWORD n=0; n<dwVERTICES[i]; ++n )
				{
					m_pNormal[i][n] = *(D3DXVECTOR3*)( pBuffer + nVERSIZE*n + sizeof(D3DXVECTOR3) );
				}
				pLOCALMESH[i]->UnlockVertexBuffer ();
			}
		}

		// 점 갯수가 안 맞을 경우에
		if ( dwVERTICES[0]!=dwVERTICES[1] || dwVERTICES[0]!=dwVERTICES[2] )
		{
			m_bMorph = FALSE;

			SAFE_DELETE_ARRAY ( m_pVertex[0] );
			SAFE_DELETE_ARRAY ( m_pVertex[1] );
			SAFE_DELETE_ARRAY ( m_pVertex[2] );

			SAFE_DELETE_ARRAY ( m_pNormal[0] );
			SAFE_DELETE_ARRAY ( m_pNormal[1] );
			SAFE_DELETE_ARRAY ( m_pNormal[2] );

			MessageBox ( NULL, "Mesh file vertex not same number.", szFilename, MB_OK );
		}

		m_pLocalMesh = pLOCALMESH[0];
		pLOCALMESH[0] = NULL;
		SAFE_RELEASE( pLOCALMESH[1] );
		SAFE_RELEASE( pLOCALMESH[2] );
	}
	else
	{
		StringCchCopy( m_szFileName, MAX_PATH, szFilename );

		StringCchCopy( szPath, MAX_PATH, DxSimpleMeshMan::GetInstance().GetPath() );
		StringCchCat( szPath, MAX_PATH, m_szFileName );

		// Load the mesh
		if ( FAILED( hr = D3DXLoadMeshFromX ( szPath, D3DXMESH_MANAGED, pd3dDevice, 
											&pAdjacencyBuffer, &pMtrlBuffer, NULL,
											&m_dwNumMaterials, &m_pLocalMesh ) ) )
		{
			CDebugSet::ToLogFile ( "ERROR : D3DXLoadMeshFromX() %s, hr: %s", szFilename, DXGetErrorString9(hr) );
			return hr;
		}

		// Optimize the mesh for performance
		if ( FAILED( hr = m_pLocalMesh->OptimizeInplace(
							D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
							(DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL ) ) )
		{
			SAFE_RELEASE( pAdjacencyBuffer );
			SAFE_RELEASE( pMtrlBuffer );

			CDebugSet::ToLogFile ( "ERROR : OptimizeInplace() %s, hr: %s", szFilename, DXGetErrorString9(hr) );
			return hr;
		}
	}

	// Set FVF
	SetFVF ( dwFVF );

	// Get Tex UV
	if ( dwFVF&D3DFVF_TEX1 )
	{
		SAFE_DELETE_ARRAY ( m_pTexUV );
		DWORD dwVertices = m_pLocalMesh->GetNumVertices();
		m_pTexUV = new D3DXVECTOR2[dwVertices];

		int nSize = 0;
		if ( dwFVF&D3DFVF_XYZ )			nSize += sizeof(D3DXVECTOR3);
		if ( dwFVF&D3DFVF_NORMAL )		nSize += sizeof(D3DXVECTOR3);
		if ( dwFVF&D3DFVF_DIFFUSE )		nSize += sizeof(D3DCOLOR);

		D3DXVECTOR2*	pVector;
		BYTE*			pByte(NULL);
		m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pByte );
		for ( DWORD i=0; i<dwVertices; ++i )
		{
			pVector = (D3DXVECTOR2*)(pByte + (nVERSIZE*i) + nSize);
			m_pTexUV[i] = *pVector;
		}
		m_pLocalMesh->UnlockVertexBuffer ();
	}
	else
	{
		SAFE_DELETE_ARRAY ( m_pTexUV );
		DWORD dwVertices = m_pLocalMesh->GetNumVertices();
		m_pTexUV = new D3DXVECTOR2[dwVertices];

		int nSize = 0;
		if ( dwFVF&D3DFVF_XYZ )			nSize += sizeof(D3DXVECTOR3);
		if ( dwFVF&D3DFVF_NORMAL )		nSize += sizeof(D3DXVECTOR3);
		if ( dwFVF&D3DFVF_DIFFUSE )		nSize += sizeof(D3DCOLOR);

		D3DXVECTOR3		vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		D3DXVECTOR3		vMin(FLT_MAX,FLT_MAX,FLT_MAX);
		D3DXVECTOR3*	pPos;
		BYTE*			pByte(NULL);
		m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pByte );
		for ( DWORD i=0; i<dwVertices; ++i )
		{
			pPos = (D3DXVECTOR3*)(pByte + (nVERSIZE*i));

			if ( (*pPos).x > vMax.x )	vMax.x = (*pPos).x;
			if ( (*pPos).y > vMax.y )	vMax.y = (*pPos).y;
			if ( (*pPos).z > vMax.z )	vMax.z = (*pPos).z;

			if ( (*pPos).x < vMin.x )	vMin.x = (*pPos).x;
			if ( (*pPos).y < vMin.y )	vMin.y = (*pPos).y;
			if ( (*pPos).z < vMin.z )	vMin.z = (*pPos).z;
		}

		float fLengthX = vMax.x-vMin.x;
		float fLengthZ = vMax.z-vMin.z;

		for ( DWORD i=0; i<dwVertices; ++i )
		{
			pPos = (D3DXVECTOR3*)(pByte + (nVERSIZE*i));

			m_pTexUV[i].x = pPos->x / fLengthX;
			m_pTexUV[i].y = pPos->z / fLengthZ;
		}
		m_pLocalMesh->UnlockVertexBuffer ();
	}

	// Get material info for the mesh
	// Get the array of materials out of the buffer
	if ( pMtrlBuffer && m_dwNumMaterials > 0 )
	{
		// Allocate memory for the materials and textures
		D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
		if( d3dxMtrls )
		{
			m_pMaterials = new D3DXMATERIAL[m_dwNumMaterials];
			m_pTextures  = new LPDIRECT3DTEXTUREQ[m_dwNumMaterials];
			m_emTexType  = new TextureManager::EM_TEXTYPE[m_dwNumMaterials];

			// Copy each material and create its texture
			for ( DWORD i=0; i<m_dwNumMaterials; ++i )
			{
				// Copy the material
				if( d3dxMtrls[i].pTextureFilename )
				{
					int nStrLen = strlen(d3dxMtrls[i].pTextureFilename)+1;
					m_pMaterials[i].pTextureFilename = new char[ nStrLen ];
					StringCchCopy( m_pMaterials[i].pTextureFilename, nStrLen, d3dxMtrls[i].pTextureFilename );
				}
				else
				{
					m_pMaterials[i].pTextureFilename = NULL;
				}
				m_pMaterials[i].MatD3D			= d3dxMtrls[i].MatD3D;
				m_pMaterials[i].MatD3D.Ambient	= m_pMaterials[i].MatD3D.Diffuse;
				m_pTextures[i]					= NULL;
				m_emTexType[i]					= TextureManager::EMTT_NORMAL;	// Default

				// Create a texture
				if ( d3dxMtrls[i].pTextureFilename )
				{
					hr = TextureManager::LoadTexture( d3dxMtrls[i].pTextureFilename, pd3dDevice, m_pTextures[i], 0, 0, TRUE );
					if ( FAILED(hr) )
					{
						m_pTextures[i] = NULL;
						hr = S_OK;
					}

					if( m_pTextures[i] )
					{
						m_emTexType[i] = TextureManager::GetTexType( d3dxMtrls[i].pTextureFilename );
					}
				}
			}
		}
	}

	SAFE_RELEASE( pAdjacencyBuffer );
	SAFE_RELEASE( pMtrlBuffer );

	return S_OK;
}

HRESULT DxSimMesh::GetBoundSphere ( D3DXVECTOR3 &vCenter, float &fRadius )
{
	HRESULT hr = S_OK;
	
	if ( m_dwFlag&EM_BOUNDSPHERE )
	{
		 vCenter = m_vCenter;
		 fRadius = m_fRadius;

		return S_OK;
	}

	DWORD dwFVF = m_pLocalMesh->GetFVF ();
	DWORD dwVertices = m_pLocalMesh->GetNumVertices ();

	BYTE *pbData;
	hr = m_pLocalMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
	if ( FAILED(hr) )	return hr;

	hr = D3DXComputeBoundingSphere ( (D3DXVECTOR3*)pbData, dwVertices, dwFVF, &m_vCenter, &m_fRadius );
	if ( FAILED(hr) )
	{
		m_pLocalMesh->UnlockVertexBuffer ();
		return hr;
	}

	m_pLocalMesh->UnlockVertexBuffer ();

	m_dwFlag |= EM_BOUNDSPHERE;
	vCenter = m_vCenter;
	fRadius = m_fRadius;

	 return S_OK;
}

HRESULT DxSimMesh::GetBoundBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	HRESULT hr = S_OK;
	
	if ( m_dwFlag&EM_BOUNDBOX )
	{
		vMax = m_vMax;
		vMin = m_vMin;

		return S_OK;
	}

	DWORD dwFVF = m_pLocalMesh->GetFVF ();
	DWORD dwStride = D3DXGetFVFVertexSize ( dwFVF );
	DWORD dwVertices = m_pLocalMesh->GetNumVertices ();

	BYTE *pbData;
	hr = m_pLocalMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pbData );
	if ( FAILED(hr) )	return hr;

	D3DXMATRIX matIdent;
	D3DXMatrixIdentity ( &matIdent );
	hr = COLLISION::CalculateBoundingBox ( &matIdent, vMax, vMin, pbData, dwVertices, dwFVF );
	if ( FAILED(hr) )
	{
		m_pLocalMesh->UnlockVertexBuffer ();
		return hr;
	}

	m_pLocalMesh->UnlockVertexBuffer ();

	m_dwFlag |= EM_BOUNDBOX;
	m_vMax = vMax;
	m_vMin = vMin;

	 return S_OK;
}

HRESULT DxSimMesh::SetFVF ( DWORD dwFVF )
{
	if ( !m_pd3dDevice )	return E_FAIL;
	if ( !m_pLocalMesh )	return E_FAIL;
	if ( m_pLocalMesh->GetFVF() == dwFVF )	return S_OK;

	LPD3DXMESH pTempLocalMesh  = NULL;
	if( FAILED( m_pLocalMesh->CloneMeshFVF ( D3DXMESH_MANAGED, dwFVF, m_pd3dDevice,
											&pTempLocalMesh ) ) )
	{
		SAFE_RELEASE( pTempLocalMesh );
		return E_FAIL;
	}

	SAFE_RELEASE( m_pLocalMesh );

	if ( pTempLocalMesh )  m_pLocalMesh  = pTempLocalMesh;

	//// Compute normals in case the meshes have them
	//if ( dwFVF & D3DFVF_NORMAL )
	//{
	//	if( m_pLocalMesh )
	//	{
	//		D3DXComputeNormals ( m_pLocalMesh, NULL );
	//	}
	//}

	return S_OK;
}

HRESULT DxSimMesh::Destroy()
{
	for ( UINT i=0; i<m_dwNumMaterials; ++i )
	{
		TextureManager::ReleaseTexture( m_pMaterials[i].pTextureFilename, m_pTextures[i] );
		SAFE_DELETE_ARRAY( m_pMaterials[i].pTextureFilename );
	}

	SAFE_DELETE_ARRAY( m_pTextures );
	SAFE_DELETE_ARRAY( m_pMaterials );
	SAFE_DELETE_ARRAY( m_emTexType );

	SAFE_RELEASE( m_pLocalMesh );

	m_dwNumMaterials = 0L;

	return S_OK;
}

HRESULT DxSimMesh::FrameMove ( float fTime )
{
	if ( !m_pLocalMesh || !m_bMorph ) return S_OK;

	float	fWeight1, fWeight2, fWeight3;

	if ( fTime >= 0.f )
	{
		fWeight1 = fabsf ( fTime );
		fWeight2 = 1.f - fabsf ( fTime );
		fWeight3 = 0.f;
	}
	if ( fTime < 0.f )
	{
		fWeight1 = 0.f;
		fWeight2 = 1.f - fabsf ( fTime );
		fWeight3 = fabsf ( fTime );
	}

	DWORD	dwFVF		= m_pLocalMesh->GetFVF();
	DWORD	dwVertices	= m_pLocalMesh->GetNumVertices();

	UINT nVERSIZE = D3DXGetFVFVertexSize ( dwFVF );

	D3DXVECTOR3*	pVecter(NULL);
	D3DXVECTOR3*	pNormal(NULL);
	BYTE*			pBuffer(NULL);
	m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pBuffer );
	for ( DWORD i=0; i<dwVertices; ++i )
	{
 		pVecter = (D3DXVECTOR3*)( pBuffer + nVERSIZE*i );
		*pVecter = m_pVertex[0][i]*fWeight1 + m_pVertex[1][i]*fWeight2 + m_pVertex[2][i]*fWeight3;

		if ( m_pLocalMesh->GetFVF() & D3DFVF_NORMAL )
		{
			pNormal = (D3DXVECTOR3*)( pBuffer + nVERSIZE*i + sizeof(D3DXVECTOR3) );
			*pNormal = m_pVertex[0][i]*fWeight1 + m_pVertex[1][i]*fWeight2 + m_pVertex[2][i]*fWeight3;
		}
	}
	m_pLocalMesh->UnlockVertexBuffer ();

	return S_OK;
}

void	DxSimMesh::SetMoveTex ( D3DXVECTOR2& vTexVelSum )
{
	DWORD	dwVertices	= m_pLocalMesh->GetNumVertices();
	DWORD	dwFVF		= m_pLocalMesh->GetFVF();
	UINT	nVERSIZE	= D3DXGetFVFVertexSize ( dwFVF );

	int nSize = 0;
	if ( dwFVF&D3DFVF_XYZ )			nSize += sizeof(D3DXVECTOR3);
	if ( dwFVF&D3DFVF_NORMAL )		nSize += sizeof(D3DXVECTOR3);
	if ( dwFVF&D3DFVF_DIFFUSE )		nSize += sizeof(D3DCOLOR);

	D3DXVECTOR2*	pVecter(NULL);
	BYTE*			pBuffer(NULL);
	m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pBuffer );
	for ( DWORD i=0; i<dwVertices; ++i )
	{
		pVecter = (D3DXVECTOR2*)(pBuffer + (i*nVERSIZE) + nSize );
		(*pVecter).x = m_pTexUV[i].x + vTexVelSum.x;
		(*pVecter).y = m_pTexUV[i].y + vTexVelSum.y;
	}
	m_pLocalMesh->UnlockVertexBuffer ();
}

void	DxSimMesh::SetSequenceTex ( int nCol, int nRow, int nNowSprite )
{
	DWORD	dwVertices	= m_pLocalMesh->GetNumVertices();
	DWORD	dwFVF		= m_pLocalMesh->GetFVF();
	UINT	nVERSIZE	= D3DXGetFVFVertexSize ( dwFVF );

	int nSize = 0;
	if ( dwFVF&D3DFVF_XYZ )			nSize += sizeof(D3DXVECTOR3);
	if ( dwFVF&D3DFVF_NORMAL )		nSize += sizeof(D3DXVECTOR3);
	if ( dwFVF&D3DFVF_DIFFUSE )		nSize += sizeof(D3DCOLOR);

	D3DXVECTOR2*	pVecter(NULL);
	BYTE*			pBuffer(NULL);
	m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pBuffer );
	for ( DWORD i=0; i<dwVertices; ++i )
	{
		pVecter = (D3DXVECTOR2*)(pBuffer + (i*nVERSIZE) + nSize );
		(*pVecter).x =  (m_pTexUV[i].x/(float)nCol) + (nNowSprite/(float)nCol);
		(*pVecter).y =  (m_pTexUV[i].y/(float)nRow) + ((nNowSprite/nCol)/(float)nRow);
	}
	m_pLocalMesh->UnlockVertexBuffer ();
}

HRESULT DxSimMesh::Render ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture, D3DMATERIALQ* pMaterials )
{
	if ( NULL == m_pLocalMesh )		return E_FAIL;

	for ( DWORD i=0; i<m_dwNumMaterials; ++i )
	{
		if( !m_pTextures[i] )	TextureManager::GetTexture( m_pMaterials[i].pTextureFilename, m_pTextures[i] );

		if ( m_bUseMaterials )
		{
			if ( pMaterials )
			{
				D3DMATERIALQ	matMaterial;

				matMaterial.Diffuse.a = m_pMaterials[i].MatD3D.Diffuse.a * pMaterials->Diffuse.a;
				matMaterial.Diffuse.r = m_pMaterials[i].MatD3D.Diffuse.r * pMaterials->Diffuse.r;
				matMaterial.Diffuse.g = m_pMaterials[i].MatD3D.Diffuse.g * pMaterials->Diffuse.g;
				matMaterial.Diffuse.b = m_pMaterials[i].MatD3D.Diffuse.b * pMaterials->Diffuse.b;

				matMaterial.Ambient.a = m_pMaterials[i].MatD3D.Ambient.a * pMaterials->Ambient.a;
				matMaterial.Ambient.r = m_pMaterials[i].MatD3D.Ambient.r * pMaterials->Ambient.r;
				matMaterial.Ambient.g = m_pMaterials[i].MatD3D.Ambient.g * pMaterials->Ambient.g;
				matMaterial.Ambient.b = m_pMaterials[i].MatD3D.Ambient.b * pMaterials->Ambient.b;

				matMaterial.Emissive.a = m_pMaterials[i].MatD3D.Emissive.a * pMaterials->Emissive.a;
				matMaterial.Emissive.r = m_pMaterials[i].MatD3D.Emissive.r * pMaterials->Emissive.r;
				matMaterial.Emissive.g = m_pMaterials[i].MatD3D.Emissive.g * pMaterials->Emissive.g;
				matMaterial.Emissive.b = m_pMaterials[i].MatD3D.Emissive.b * pMaterials->Emissive.b;

				matMaterial.Specular.a = m_pMaterials[i].MatD3D.Specular.a * pMaterials->Specular.a;
				matMaterial.Specular.r = m_pMaterials[i].MatD3D.Specular.r * pMaterials->Specular.r;
				matMaterial.Specular.g = m_pMaterials[i].MatD3D.Specular.g * pMaterials->Specular.g;
				matMaterial.Specular.b = m_pMaterials[i].MatD3D.Specular.b * pMaterials->Specular.b;

				matMaterial.Power = m_pMaterials[i].MatD3D.Power;

				pd3dDevice->SetMaterial ( &matMaterial );
			}
			else
			{
				pd3dDevice->SetMaterial ( &m_pMaterials[i].MatD3D );
			}

			if ( pTexture )	pd3dDevice->SetTexture ( 0, pTexture );
			else			pd3dDevice->SetTexture ( 0, m_pTextures[i] );
		}

		DWORD dwColorOP, dwAlphaOP;
		if ( !pTexture && !m_pTextures[i] )
		{
			pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP, &dwColorOP );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );

			pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAOP, &dwAlphaOP );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
		}

		
		if( m_pMaterials[i].pTextureFilename && 
			m_pMaterials[i].pTextureFilename[0]!='\0' && 
			!pTexture && 
			!m_pTextures[i] )	
		{
		}
		else
		{
			m_pLocalMesh->DrawSubset(i);
		}

		if ( !pTexture && !m_pTextures[i] )
		{
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, dwColorOP );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, dwAlphaOP );
		}
	}

	return S_OK;
}

HRESULT DxSimMesh::RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matWld, D3DMATERIALQ* pMaterials )
{
	if ( NULL == m_pLocalMesh )		return E_FAIL;

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWld );

	for ( DWORD i=0; i<m_dwNumMaterials; ++i )
	{
		if ( m_bUseMaterials )
		{
			if ( !pMaterials )	pd3dDevice->SetMaterial ( &m_pMaterials[i].MatD3D );
			else				pd3dDevice->SetMaterial ( pMaterials );

			if( !m_pTextures[i] )
			{
				TextureManager::GetTexture( m_pMaterials[i].pTextureFilename, m_pTextures[i] );

				if( m_pTextures[i] )
				{
					m_emTexType[i] = TextureManager::GetTexType( m_pMaterials[i].pTextureFilename );
				}
			}

			pd3dDevice->SetTexture ( 0, m_pTextures[i] );
		}

		if ( m_bUseMaterials )
		{
			if( (m_emTexType[i]==TextureManager::EMTT_ALPHA_HARD) || (m_emTexType[i]==TextureManager::EMTT_ALPHA_SOFT) ||
				(m_emTexType[i]==TextureManager::EMTT_ALPHA_SOFT01) || (m_emTexType[i]==TextureManager::EMTT_ALPHA_SOFT02))
			{
				DxRenderStates::GetInstance().SetAlphaMap( pd3dDevice );
			}
		}

		m_pLocalMesh->DrawSubset(i);

		if ( m_bUseMaterials )
		{
			if( (m_emTexType[i]==TextureManager::EMTT_ALPHA_HARD) || (m_emTexType[i]==TextureManager::EMTT_ALPHA_SOFT) ||
				(m_emTexType[i]==TextureManager::EMTT_ALPHA_SOFT01) || (m_emTexType[i]==TextureManager::EMTT_ALPHA_SOFT02))
			{
				DxRenderStates::GetInstance().ReSetAlphaMap( pd3dDevice );
			}
		}
	}

	return S_OK;
}

//-----------------------------------------------------------------------------[DxSimpleMeshMan]
//
DxSimpleMeshMan& DxSimpleMeshMan::GetInstance()
{
	static DxSimpleMeshMan Instance;
	return Instance;
}

DxSimpleMeshMan::DxSimpleMeshMan(void) :
	m_pd3dDevice(NULL)
{
	memset(m_szPath, 0, sizeof(char) * (MAX_PATH));
}

DxSimpleMeshMan::~DxSimpleMeshMan(void)
{	

}

void DxSimpleMeshMan::ConvertFileName( char* szOut, int nStrLen, const char* szFile, DWORD dwFVF )
{
	char sz0 = '0';
	char sz1 = '0';
	char sz2 = '0';
	char sz3 = '0';

	if ( dwFVF&D3DFVF_XYZ )		sz0 += 1;
	if ( dwFVF&D3DFVF_NORMAL )	sz1 += 1;
	if ( dwFVF&D3DFVF_DIFFUSE )	sz2 += 1;
	if ( dwFVF&D3DFVF_TEX1 )	sz3 += 1;

	std::string szString = "";
	szString += sz0;
	szString += sz1;
	szString += sz2;
	szString += sz3;

	StringCchCopy( szOut, nStrLen, szFile );
	StringCchCat( szOut, nStrLen, "." );
	StringCchCat( szOut, nStrLen, szString.c_str() );
}

DxSimMesh* DxSimpleMeshMan::Load ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF )
{
	HRESULT hr;
	if( !szFile )
	{
		CDebugSet::ToListView( "DxSimpleMeshMan::Load() -- szFile -- NULL" );
		return NULL;
	}

	char szConvertName[MAX_PATH] = "";
	ConvertFileName( szConvertName, MAX_PATH, szFile, dwFVF );

	DxSimMesh* pMesh = NULL;
	pMesh = Find ( szConvertName );
	if ( pMesh ) 
	{
		pMesh->m_dwRef++;
		return pMesh;
	}

	pMesh = new DxSimMesh;
	hr = pMesh->Create( pd3dDevice, dwFVF, szFile, "", "" );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pMesh);
		CDebugSet::ToListView( "DxSimpleMeshMan::Load() -- Create() -- NULL" );
		return NULL;
	}

	//	Note : 리스트에 삽입.
	//
	m_MeshMap.insert ( std::make_pair(szConvertName,pMesh) );

	pMesh->m_dwRef++;
	return pMesh;
}

DxSimMesh*	DxSimpleMeshMan::BlendLoad ( char** szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF )
{
	HRESULT hr;
	if( !szFile )
	{
		CDebugSet::ToListView( "DxSimpleMeshMan::BlendLoad() -- szFile -- NULL" );
		return NULL;
	}

	char szConvertName[MAX_PATH] = "";
	ConvertFileName( szConvertName, MAX_PATH, szFile[0], dwFVF );

	DxSimMesh* pMesh = NULL;
	pMesh = Find ( szConvertName, TRUE );
	if ( pMesh )
	{
		pMesh->m_dwRef++;
		return pMesh;
	}

	pMesh = new DxSimMesh;
	hr = pMesh->Create ( pd3dDevice, dwFVF, szFile[0], szFile[1], szFile[2] );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxSimpleMeshMan::BlendLoad() -- %s, %s, %s", szFile[0], szFile[1], szFile[2] );
		SAFE_DELETE(pMesh);
		return NULL;
	}

	//	Note : 리스트에 삽입.
	//
	m_BlendMeshMap.insert ( std::make_pair(szConvertName,pMesh) );

	pMesh->m_dwRef++;
	return pMesh;
}

HRESULT DxSimpleMeshMan::Release ( const char* szFile, DWORD dwFVF, BOOL bMorph )
{
	GASSERT(szFile);

	char szConvertName[MAX_PATH] = "";
	ConvertFileName( szConvertName, MAX_PATH, szFile, dwFVF );

	if ( bMorph )
	{
		if ( m_BlendMeshMap.empty() )	return S_OK;

		SIMPLEMAP_ITER iter = m_BlendMeshMap.find ( szConvertName );

		if ( iter!=m_BlendMeshMap.end() )
		{
			DxSimMesh *pMesh = (*iter).second;

			if ( --pMesh->m_dwRef == 0 )
			{
				SAFE_DELETE(pMesh);
				m_BlendMeshMap.erase ( iter );
			}
		}
	}
	else
	{
		if ( m_MeshMap.empty() )	return S_OK;

		SIMPLEMAP_ITER iter = m_MeshMap.find ( szConvertName );

		if ( iter!=m_MeshMap.end() )
		{
			DxSimMesh *pMesh = (*iter).second;

			if ( --pMesh->m_dwRef == 0 )
			{
				SAFE_DELETE(pMesh);
				m_MeshMap.erase ( iter );
			}
		}
	}

	return S_OK;
}

HRESULT DxSimpleMeshMan::Delete ( const char* szFile, DWORD dwFVF, BOOL bMorph )
{
	GASSERT(szFile);

	char szConvertName[MAX_PATH] = "";
	ConvertFileName( szConvertName, MAX_PATH, szFile, dwFVF );

	if ( bMorph )
	{
		SIMPLEMAP_ITER iter = m_BlendMeshMap.find ( szConvertName );
		if ( iter!=m_BlendMeshMap.end() )
		{
			DxSimMesh *pMesh = (*iter).second;
			{
				SAFE_DELETE(pMesh);
				m_BlendMeshMap.erase ( iter );
			}
		}
	}
	else
	{
		SIMPLEMAP_ITER iter = m_MeshMap.find ( szConvertName );
		if ( iter!=m_MeshMap.end() )
		{
			DxSimMesh *pMesh = (*iter).second;
			{
				SAFE_DELETE(pMesh);
				m_MeshMap.erase ( iter );
			}
		}
	}

	return S_OK;
}

DxSimMesh* DxSimpleMeshMan::Find ( const char* szFile, BOOL bMorph )
{
	GASSERT(szFile);

	if ( bMorph )
	{
		SIMPLEMAP_ITER iter = m_BlendMeshMap.find ( szFile );
		if ( iter!=m_BlendMeshMap.end() )	return (*iter).second;
	}
	else
	{
		SIMPLEMAP_ITER iter = m_MeshMap.find ( szFile );
		if ( iter!=m_MeshMap.end() )		return (*iter).second;
	}

	return NULL;
}

HRESULT DxSimpleMeshMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxSimpleMeshMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxSimpleMeshMan::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxSimpleMeshMan::DeleteDeviceObjects ()
{
	std::for_each ( m_BlendMeshMap.begin(), m_BlendMeshMap.end(), std_afunc::DeleteMapObject() );
	std::for_each ( m_MeshMap.begin(), m_MeshMap.end(), std_afunc::DeleteMapObject() );

	m_BlendMeshMap.clear();
	m_MeshMap.clear();

	return S_OK;
}

HRESULT DxSimpleMeshMan::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxSimpleMeshMan::CleanUp ()
{
	InvalidateDeviceObjects ();
	DeleteDeviceObjects ();
	FinalCleanup ();
	
	return S_OK;
}


