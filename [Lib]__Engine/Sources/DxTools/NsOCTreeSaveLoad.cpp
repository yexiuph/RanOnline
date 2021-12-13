#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./glperiod.h"
#include "./SerialFile.h"
#include "DxLightMan.h"
#include "./TextureManager.h"

#include "NsOptimizeMesh.h"
#include "./DxTextureEffMan.h"

#include "./DxTexEffMan.h"
#include "./DxTexEffDiffuse.h"
#include "./DxTexEffFlowUV.h"
#include "./DxTexEffRotate.h"
#include "./DxTexEffSpecular.h"

#include "NsOCTree.h"

#ifdef _DEBUG                                                                                                                                                                                                 
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//												O	B	J		O	C	T	r	e	e
// -----------------------------------------------------------------------------------------------------------------------------------------
void OBJOCTree::Save ( CSerialFile& SFile, BOOL bPiece )
{
	SFile << vMax;
	SFile << vMin;

	// Note : Object의 타입을 설정한다.
	if( bPiece )
	{
		m_emLandType = EMLR_PIECE;
	}
	else
	{
		if( DxLightMan::GetInstance()->IsNightAndDay() )	m_emLandType = EMLR_DAYNIGHT_ON;
		else												m_emLandType = EMLR_DAYNIGHT_OFF;		
	}

	SFile.BeginBlock(EMBLOCK_02);
	{
		BOOL bExist(FALSE);
		if( m_pVB && m_pIB )
		{
			bExist = TRUE;
		}

		SFile << bExist;
		if ( bExist )
		{
			SFile << m_dwVERSION;	// 버젼 저장.

			SFile << m_dwFVF;
			SFile << m_dwVert;
			SFile << m_dwFace;
			SFile << (DWORD)m_emLandType;

			m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );

			EnterCriticalSection(&m_pCSLockVB);
			{
				BYTE* pVertices;
				m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
				SFile.WriteBuffer ( pVertices, m_nFVFSize*m_dwVert );
				m_pVB->Unlock();
			}
			LeaveCriticalSection(&m_pCSLockVB);

			WORD* pIndices;
			m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
			SFile.WriteBuffer ( pIndices, sizeof(WORD)*m_dwFace*3 );
			m_pIB->Unlock();

			switch( m_emLandType )
			{
			case EMLR_DAYNIGHT_ON:
				SFile.WriteBuffer( m_pColor, sizeof(DIRECTPOINTCOLOR)*m_dwVert );
				break;
			};
		}

		bExist = pCollsionTREE ? TRUE : FALSE;
		SFile << bExist;
		if ( pCollsionTREE )
		{
			pCollsionTREE->SaveFile( SFile );
		}
	}
	SFile.EndBlock(EMBLOCK_02);
}

void OBJOCTree::Load( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bDynamicLoad, BOOL bPiece )
{
	CleanUp();

	// Note : Object의 타입을 설정한다.
	if( bPiece )
	{
		m_emLandType = EMLR_PIECE;
	}
	else
	{
		if( DxLightMan::GetInstance()->IsNightAndDay() )	m_emLandType = EMLR_DAYNIGHT_ON;
		else												m_emLandType = EMLR_DAYNIGHT_OFF;		
	}

	SFile >> vMax;
	SFile >> vMin;

	vMax += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
	vMin -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
	
	DWORD dwBufferSize;
	SFile >> dwBufferSize;

	m_dwFileCur = SFile.GetfTell();
	if( bDynamicLoad && NSOCTREE::IsDynamicLoad() )	SFile.SetOffSet( m_dwFileCur+dwBufferSize );
	else											DynamicLoad( pd3dDevice, SFile, FALSE );
}

void OBJOCTree::ThreadLoad( LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading )
{
	if( m_bCheck )	return;	// 다시 안 들어오도록 체크해야 한다.
	m_bCheck = TRUE;

	DWORD dwData = (DWORD)(this);

	// Note : 로딩 데이터에 삽입해 넣는다.
	EnterCriticalSection(&CSLockLoading);
	{
		listLoadingData.push_back( dwData );
	}
	LeaveCriticalSection(&CSLockLoading);
}

BOOL OBJOCTree::DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bThread )
{
	BOOL	bUse = TRUE;

	// Note : Loading Complete
	if ( IsLoad() )		return TRUE;

	if ( !m_dwFileCur )	
	{
		bUse = FALSE;
		goto _RETURN;
	}

	SFile.SetOffSet( m_dwFileCur );

	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		DWORD dwVer;
		SFile >> dwVer;
		if( dwVer < 0x10000 )
		{
			// Note : 예전 버젼으로 되돌린다. 이때는 Normal을 사용했었다.
			//			dwVer는 이때 당시 FVF로 저장되었었다.	그래서 넘겨준 후 로딩을 해야한다.
			if( !OctreeLoadOLD( pd3dDevice, SFile, dwVer, bThread ) )
			{
				bUse = FALSE;
				goto _RETURN;
			}
		}
		else if( dwVer==0x10000 )
		{
			if( !OctreeLoad_100( pd3dDevice, SFile, bThread ) )
			{
				bUse = FALSE;
				goto _RETURN;
			}
		}
		else if( dwVer==m_dwVERSION )
		{
			if( !OctreeLoad( pd3dDevice, SFile, bThread ) )
			{
				bUse = FALSE;
				goto _RETURN;
			}
		}
		else
		{
			// Note : 없다고 생각하고 로딩을 하지 않는다.
			bUse = FALSE;
			goto _RETURN;
		}
	}

	SFile >> bExist;
	if ( bExist )
	{
		if( bThread )	Sleep(0);

		pCollsionTREE = new DxAABBNode;
		if ( pCollsionTREE )	pCollsionTREE->LoadFile( SFile );
	}

	m_bLoad = TRUE;		// 로딩 완료
	return bUse;

_RETURN:
	m_bLoad = FALSE;	// 로딩 실패
	return bUse;
}

BOOL OBJOCTree::OctreeLoad( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bThread )
{
	DWORD dwTemp(0L);
	SFile >> m_dwFVF;
	SFile >> m_dwVert;
	SFile >> m_dwFace;
	SFile >> dwTemp;
	m_emLandType = (EM_LAND_TYPE)dwTemp;

	m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );

	if( bThread )	Sleep(0);

	// Note : 
	BYTE*	pTempVB(NULL);
	pTempVB = new BYTE[m_dwVert*m_nFVFSize];

	EnterCriticalSection(&m_pCSLockVB);
	{		
		SAFE_RELEASE( m_pVB );
		pd3dDevice->CreateVertexBuffer( m_dwVert*m_nFVFSize, D3DUSAGE_WRITEONLY, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, NULL );
		if( !m_pVB )
		{
			// Note : 임시로 만든것을 지워줌.
			SAFE_DELETE_ARRAY( pTempVB );

			LeaveCriticalSection(&m_pCSLockVB);
			return FALSE;
		}
	}
	LeaveCriticalSection(&m_pCSLockVB);

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( !m_pIB )
	{
		// Note : 임시로 만든것을 지워줌.
		SAFE_DELETE_ARRAY( pTempVB );

		EnterCriticalSection(&m_pCSLockVB);
		SAFE_RELEASE( m_pVB );
		LeaveCriticalSection(&m_pCSLockVB);

		return FALSE;
	}

	if( bThread )	Sleep(0);

	// Note : 저장 되었던 VB를 로드한다.
	if( bThread )
	{
		// 기본 데이터를 로드한다.
		DWORD dwSub = 0;
		DWORD dwAdd = 60;	// ??byte * 60 = ????byte
		for( DWORD i=0; i<m_dwVert; )
		{
			dwSub = m_dwVert - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pTempVB+(m_nFVFSize*i), m_nFVFSize*dwSub );
			i += dwSub;
			Sleep(0);
		}
	}
	else
	{
		SFile.ReadBuffer( pTempVB, m_dwVert*m_nFVFSize );
	}

	// Note : 저장 되었던 IB를 로드한다.
	if( bThread )
	{
		DWORD dwSub = 0;
		DWORD dwAdd = 160;	// 12byte * 160 = 1920byte
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		for( DWORD i=0; i<m_dwFace; )
		{
			dwSub = m_dwFace - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pIndices+(i*3), sizeof(WORD)*dwSub*3 );
			i += dwSub;
			Sleep(0);
		}
		m_pIB->Unlock();
	}
	else
	{
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		SFile.ReadBuffer( pIndices, sizeof(WORD)*m_dwFace*3 );
		m_pIB->Unlock();
	}

	// Note : 
	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
		m_pColor = new DIRECTPOINTCOLOR[m_dwVert];
		SFile.ReadBuffer( m_pColor, sizeof(DIRECTPOINTCOLOR)*m_dwVert );
		break;
	};

	Sleep(0);

	// Note : VertexBuffer 생성후 데이터 삽입.
	EnterCriticalSection(&m_pCSLockVB);
	{
		BYTE* pVertices(NULL);
		m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
		memcpy( pVertices, pTempVB, m_dwVert*m_nFVFSize );
		m_pVB->Unlock();
	}
	LeaveCriticalSection(&m_pCSLockVB);

	Sleep(0);

	// Note : 
	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
	case EMLR_DAYNIGHT_OFF:
		SAFE_DELETE_ARRAY( m_pVertSrc );
		m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];
		memcpy( m_pVertSrc, pTempVB, sizeof(VERTEXNORCOLORTEX)*m_dwVert );

		Sleep(0);

		break;
	case EMLR_PIECE:
		D3DXVECTOR3* pPos(NULL);
		D3DXVECTOR3* pNor(NULL);
		D3DXVECTOR2* pTex(NULL);
		SAFE_DELETE_ARRAY( m_pVertSrc );
		m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			pPos = (D3DXVECTOR3*)( pTempVB + i*m_nFVFSize );
			pNor = (D3DXVECTOR3*)( pTempVB + i*m_nFVFSize + 4*3 );	// 4바이트 * (위치)
			pTex = (D3DXVECTOR2*)( pTempVB + i*m_nFVFSize + 4*6 );	// 4바이트 * (위치+노멀)

			m_pVertSrc[i].vPos = *pPos;
			m_pVertSrc[i].vNor = *pNor;
			m_pVertSrc[i].dwColor = 0xffffffff;
			m_pVertSrc[i].vTex = *pTex;
		}

		Sleep(0);

		break;
	};

	// Note : 임시로 만든것을 지워줌.
	SAFE_DELETE_ARRAY( pTempVB );

	return TRUE;
}

BOOL OBJOCTree::OctreeLoad_100( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bThread )
{
	DWORD dwTemp(0L);
	SFile >> m_dwFVF;
	SFile >> m_dwVert;
	SFile >> m_dwFace;
	SFile >> dwTemp;
	m_emLandType = (EM_LAND_TYPE)dwTemp;

	DWORD dwSaveFVFSize = D3DXGetFVFVertexSize( m_dwFVF );
	DWORD dwFVF = (~D3DFVF_TEXCOUNT_MASK) & m_dwFVF;
	DWORD dwInverseTexFVFSize = D3DXGetFVFVertexSize( dwFVF );

	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
	case EMLR_DAYNIGHT_OFF:
		m_dwFVF = VERTEXNORCOLORTEX::FVF;
		m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );
		break;

	case EMLR_PIECE:
		m_nFVFSize = D3DXGetFVFVertexSize( m_dwFVF );
		break;
	};
	

	if( bThread )	Sleep(0);

	// Note : 
	BYTE*	pTempVB(NULL);
	pTempVB = new BYTE[m_dwVert*dwSaveFVFSize];

	EnterCriticalSection(&m_pCSLockVB);
	{		
		SAFE_RELEASE( m_pVB );
		pd3dDevice->CreateVertexBuffer( m_dwVert*m_nFVFSize, D3DUSAGE_WRITEONLY, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, NULL );
		if( !m_pVB )
		{
			// Note : 임시로 만든것을 지워줌.
			SAFE_DELETE_ARRAY( pTempVB );

			LeaveCriticalSection(&m_pCSLockVB);
			return FALSE;
		}
	}
	LeaveCriticalSection(&m_pCSLockVB);

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( !m_pIB )
	{
		// Note : 임시로 만든것을 지워줌.
		SAFE_DELETE_ARRAY( pTempVB );

		EnterCriticalSection(&m_pCSLockVB);
		SAFE_RELEASE( m_pVB );
		LeaveCriticalSection(&m_pCSLockVB);

		return FALSE;
	}

	if( bThread )	Sleep(0);

	// Note : 저장 되었던 VB를 로드한다.
	if( bThread )
	{
		// 기본 데이터를 로드한다.
		DWORD dwSub = 0;
		DWORD dwAdd = 60;	// ??byte * 60 = ????byte
		for( DWORD i=0; i<m_dwVert; )
		{
			dwSub = m_dwVert - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pTempVB+(dwSaveFVFSize*i), dwSaveFVFSize*dwSub );
			i += dwSub;
			Sleep(0);
		}
	}
	else
	{
		SFile.ReadBuffer( pTempVB, m_dwVert*dwSaveFVFSize );
	}

	// Note : 저장 되었던 IB를 로드한다.
	if( bThread )
	{
		DWORD dwSub = 0;
		DWORD dwAdd = 160;	// 12byte * 160 = 1920byte
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		for( DWORD i=0; i<m_dwFace; )
		{
			dwSub = m_dwFace - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pIndices+(i*3), sizeof(WORD)*dwSub*3 );
			i += dwSub;
			Sleep(0);
		}
		m_pIB->Unlock();
	}
	else
	{
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		SFile.ReadBuffer( pIndices, sizeof(WORD)*m_dwFace*3 );
		m_pIB->Unlock();
	}

	// Note : 
	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
		m_pColor = new DIRECTPOINTCOLOR[m_dwVert];
		SFile.ReadBuffer( m_pColor, sizeof(DIRECTPOINTCOLOR)*m_dwVert );
		break;
	};

	Sleep(0);

	// Note : 
	D3DXVECTOR3* pPos(NULL);
	D3DXVECTOR3* pNor(NULL);
	D3DXVECTOR2* pTex(NULL);
	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
	case EMLR_DAYNIGHT_OFF:
		SAFE_DELETE_ARRAY( m_pVertSrc );
		m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			pPos = (D3DXVECTOR3*)( pTempVB + i*dwSaveFVFSize );
			pTex = (D3DXVECTOR2*)( pTempVB + i*dwSaveFVFSize + dwInverseTexFVFSize );	// 4바이트 * (위치+디퓨즈)

			m_pVertSrc[i].vPos = *pPos;
			m_pVertSrc[i].vNor = D3DXVECTOR3(0.f,1.f,0.f);
			m_pVertSrc[i].dwColor = 0xffffffff;
			m_pVertSrc[i].vTex = *pTex;
		}

		//memcpy( m_pVertSrc, pTempVB, sizeof(VERTEXNORCOLORTEX)*m_dwVert );

		Sleep(0);

		break;


	case EMLR_PIECE:
		SAFE_DELETE_ARRAY( m_pVertSrc );
		m_pVertSrc = new VERTEXNORCOLORTEX[m_dwVert];
		for( DWORD i=0; i<m_dwVert; ++i )
		{
			pPos = (D3DXVECTOR3*)( pTempVB + i*m_nFVFSize );
			pNor = (D3DXVECTOR3*)( pTempVB + i*m_nFVFSize + 4*3 );	// 4바이트 * (위치+노멀)
			pTex = (D3DXVECTOR2*)( pTempVB + i*m_nFVFSize + 4*6 );	// 4바이트 * (위치+노멀)

			m_pVertSrc[i].vPos = *pPos;
			m_pVertSrc[i].vNor = *pNor;
			m_pVertSrc[i].dwColor = 0xffffffff;
			m_pVertSrc[i].vTex = *pTex;
		}

		Sleep(0);

		break;
	};

	switch( m_emLandType )
	{
	case EMLR_DAYNIGHT_ON:
	case EMLR_DAYNIGHT_OFF:
		// Note : VertexBuffer 생성후 데이터 삽입.
		EnterCriticalSection(&m_pCSLockVB);
		{
			BYTE* pVertices(NULL);
			m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
			memcpy( pVertices, m_pVertSrc, m_dwVert*m_nFVFSize );
			m_pVB->Unlock();
		}
		LeaveCriticalSection(&m_pCSLockVB);
		break;

	case EMLR_PIECE:
		// Note : VertexBuffer 생성후 데이터 삽입.
		EnterCriticalSection(&m_pCSLockVB);
		{
			BYTE* pVertices(NULL);
			m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
			memcpy( pVertices, pTempVB, m_dwVert*m_nFVFSize );
			m_pVB->Unlock();
		}
		LeaveCriticalSection(&m_pCSLockVB);
		break;
	};
	

	Sleep(0);

	// Note : 임시로 만든것을 지워줌.
	SAFE_DELETE_ARRAY( pTempVB );

	return TRUE;
}

BOOL OBJOCTree::OctreeLoadOLD( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DWORD dwFVF, const BOOL bThread )
{
	HRESULT hr(S_OK);

	SFile >> m_dwVert;
	SFile >> m_dwFace;

	UINT nSIZE = D3DXGetFVFVertexSize( dwFVF );	// dwFVF은 정해지지 않는 것이 들어 오기 대문에 지역 변수로 넣었다.

	if( bThread )	Sleep(0);

	// Note : 로딩 되어있는 데이터 형.
	BYTE*	pTempVB(NULL);
	pTempVB = new BYTE[m_dwVert*nSIZE];

	// Note : 기본적인 VB, IB, DIRECTPOINTCOLOR, 등을 생성한다.
	CreateBASE( pd3dDevice, m_dwVert, m_dwFace );
	if( !m_pVB )	return FALSE;
	if( !m_pIB )	return FALSE;

	if( bThread )	Sleep(0);

	// Note : 저장 되었던 VB를 로드한다.
	if( bThread )
	{
		// 기본 데이터를 로드한다.
		DWORD dwSub = 0;
		DWORD dwAdd = 60;	// 32byte * 60 = 1920byte
		for( DWORD i=0; i<m_dwVert; )
		{
			dwSub = m_dwVert - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pTempVB+(nSIZE*i), nSIZE*dwSub );
			i += dwSub;
			Sleep(0);
		}
	}
	else
	{
		SFile.ReadBuffer( pTempVB, m_dwVert*nSIZE );
	}

	// Note : 저장 되었던 IB를 로드한다.
	if( bThread )
	{
		DWORD dwSub = 0;
		DWORD dwAdd = 160;	// 12byte * 160 = 1920byte
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		for( DWORD i=0; i<m_dwFace; )
		{
			dwSub = m_dwFace - i;
			dwSub = (dwSub>dwAdd) ? dwAdd : dwSub;
			SFile.ReadBuffer( pIndices+(i*3), sizeof(WORD)*dwSub*3 );
			i += dwSub;
			Sleep(0);
		}
		m_pIB->Unlock();
	}
	else
	{
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		SFile.ReadBuffer( pIndices, sizeof(WORD)*m_dwFace*3 );
		m_pIB->Unlock();
	}

	// Note : VertSrc를 생성한다.
	CreateVertSrc( pTempVB, dwFVF, m_dwVert );

	// Note : Normal과 컬러를 가져온다.
	CreateNormalColor( pTempVB, dwFVF, m_dwVert );

	if( bThread )	Sleep(0);
	
	// Note : 렌더에 쓰일 VB를 셋팅한다.
	CopyVertSrcToVB( pTempVB, m_pVertSrc, m_pVB, m_dwVert );

	// Note : 임시로 만든것을 지워줌.
	SAFE_DELETE_ARRAY( pTempVB );

	return TRUE;
}

void OBJOCTree::Load_VER100 ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	HRESULT hr(S_OK);
	CleanUp();

	// Note : 이때는 무조건 Piece가 아니었다. Object의 타입을 설정한다.
	if( DxLightMan::GetInstance()->IsNightAndDay() )	m_emLandType = EMLR_DAYNIGHT_ON;
	else												m_emLandType = EMLR_DAYNIGHT_OFF;

	SFile >> vMax;
	SFile >> vMin;

	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		DWORD dwFVF;
		SFile >> dwFVF;
		SFile >> m_dwVert;
		SFile >> m_dwFace;

		UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );

		// Note : 로딩 되어있는 데이터 형.
		BYTE*	pTempVB(NULL);
		pTempVB = new BYTE[m_dwVert*nSIZE];

		// Note : 기본적인 VB, IB, DIRECTPOINTCOLOR, 등을 생성한다.
		CreateBASE( pd3dDevice, m_dwVert, m_dwFace );

		// Note : 저장 되었던 VB를 로드한다.
		SFile.ReadBuffer( pTempVB, m_dwVert*nSIZE );

		// Note : 저장 되었던 IB를 로드한다.
		WORD* pIndices;
		m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
		SFile.ReadBuffer( pIndices, sizeof(WORD)*m_dwFace*3 );
		m_pIB->Unlock();

		// Note : VertSrc를 생성한다.
		CreateVertSrc( pTempVB, dwFVF, m_dwVert );

		// Note : Normal과 컬러를 가져온다.
		CreateNormalColor( pTempVB, dwFVF, m_dwVert );
			
		// Note : 렌더에 쓰일 VB를 셋팅한다.
		CopyVertSrcToVB( pTempVB, m_pVertSrc, m_pVB, m_dwVert );

		SAFE_DELETE_ARRAY( pTempVB );
	}

	m_bLoad = TRUE;		// 로딩 완료
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		O	p	t	i	m	i	z	e		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD	DxAABBOctree::VERSION = 0x0100;

void DxAABBOctree::Save ( CSerialFile& SFile, BOOL bPiece )
{
	SFile << VERSION;

	SFile.BeginBlock(EMBLOCK_01);
	{
		SFile << m_vMax;
		SFile << m_vMin;

		BOOL bExist;
		bExist = m_pOcTree ? TRUE : FALSE;
		SFile << bExist;
		if ( m_pOcTree )	NSOCTREE::SaveOctree ( SFile, m_pOcTree, bPiece );
	}
	SFile.EndBlock(EMBLOCK_01);
}

void DxAABBOctree::Load ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bDynamicLoad, BOOL bPiece )
{
	CleanUp();

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION )
	{
		SFile >> m_vMax;
		SFile >> m_vMin;

		BOOL bExist;
		SFile >> bExist;
		if ( bExist )
		{
			NSOCTREE::LoadOctree( pd3dDevice, SFile, m_pOcTree, bDynamicLoad, bPiece );
		}
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );
	}
}

void DxAABBOctree::Load_VER100 ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, std::string& szName )
{
	CleanUp();

	SFile >> m_vMax;
	SFile >> m_vMin;
	SFile >> szName;

	BOOL bUse = TRUE;
	SFile >> bUse;
	if ( bUse )
	{
		NSOCTREE::LoadOctreeVER_100( pd3dDevice, SFile, m_pOcTree );
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		S	i	n	g	l	e	T	e	x		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD	DxSingleTexMesh::VERSION = 0x0101;

void DxSingleTexMesh::Save( CSerialFile& SFile, BOOL bPiece )
{
	SFile << VERSION;

	SFile.BeginBlock(EMBLOCK_01);
	{
		SFile << m_dwFlag;
		SFile << m_szTexName;

		// Note : Texture Effect List < Ver.101 >
		{
			DWORD dwSize=0;
			DxTexEffBase* pCur = m_pTexEffList;
			while( pCur )
			{
				++dwSize;
				pCur = pCur->m_pNext;
			}
			SFile << dwSize;

			pCur = m_pTexEffList;
			while( pCur )
			{
				SFile << pCur->GetTypeID();

				pCur->SavePSF( SFile );
				pCur = pCur->m_pNext;
			}
		}
	}
	SFile.EndBlock(EMBLOCK_01);

	BOOL bExist;
	bExist = m_pAABBOctree ? TRUE : FALSE;
	SFile << bExist;
	if ( m_pAABBOctree )	m_pAABBOctree->Save( SFile, bPiece );
}

void DxSingleTexMesh::Load ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bDynamicLoad, BOOL bPiece )
{
	CleanUp();

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION )
	{
		SFile >> m_dwFlag;
		SFile >> m_szTexName;

		// Note : Texture Effect List < Ver.101 >
		{
			DWORD dwSize;
			SFile >> dwSize;

			DxTexEffBase*	pTexEffBase;
			DWORD			dwTypeID;
			std::string		strName;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> dwTypeID;

				// Note : 만들어야지.
				pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

				// Note : Load
				pTexEffBase->LoadPSF( pd3dDevice, SFile );

				// Note : 연결
				pTexEffBase->m_pNext = m_pTexEffList;
				m_pTexEffList = pTexEffBase;
			}
		}
	}
	else if( dwVer==0x0100 )
	{
		SFile >> m_szTexName;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );
	}

	// Note : m_pAABBOctree에서 Version 체크가 일어난다.
	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		m_pAABBOctree = new DxAABBOctree;
		m_pAABBOctree->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
	}

	// Note : Texture Loading		
//	if ( m_szTexName.size() )	TextureManager::LoadTexture ( m_szTexName.c_str(), pd3dDevice, m_pTexture, 0L, 0L );
}

void DxSingleTexMesh::Load_VER100 ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	CleanUp();

	std::string szName;

	m_pAABBOctree = new DxAABBOctree;
	m_pAABBOctree->Load_VER100 ( pd3dDevice, SFile, szName );

	m_szTexName = szName.c_str();
//	if ( m_szTexName.size() )	TextureManager::LoadTexture ( m_szTexName.c_str(), pd3dDevice, m_pTexture, 0L, 0L );
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//														N	S	O	C	T	R	E	E
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSOCTREE
{
	BOOL				g_bDynamicLoad = FALSE;	// Tool 에 따라 변함. Game 사용, Level Tool 미 사용.

	void OneTimeSceneInit()
	{
	}

	void CleanUp()
	{
	}

	BOOL IsDynamicLoad()
	{
		return g_bDynamicLoad;
	}

	void EnableDynamicLoad()
	{
		g_bDynamicLoad = TRUE; 
	}

	BOOL IsFaceFront ( BYTE* pSrc, const D3DXPLANE& sPlane, const UINT nSIZE )
	{
		float fResult;
		D3DXVECTOR3 *pVecter;
		for ( DWORD i=0; i<3; ++i )
		{
			pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*i );
			fResult = D3DXPlaneDotCoord ( &sPlane, pVecter );
			if ( fResult >= 0.f )	return TRUE;
		}
		return FALSE;
	}

	void DivideFace ( BYTE* pSrc, BYTE* pLeft, BYTE* pRight, DWORD dwFace, DWORD& dwLeft, DWORD& dwRight, const D3DXPLANE& sPlane, const UINT nSIZE )
	{
		D3DXVECTOR3 *pVecter;
		D3DXVECTOR3 *pDest;
		for ( DWORD i=0; i<dwFace; ++i )
		{
			// Note : 면이 앞에 있다.		 면 단위 계산
			if ( IsFaceFront ( &pSrc[nSIZE*i*3], sPlane, nSIZE ) )
			{
				pDest = (D3DXVECTOR3*)( pLeft + nSIZE*(dwLeft*3+0) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+0) );
				*pDest = *pVecter;

				pDest = (D3DXVECTOR3*)( pLeft + nSIZE*(dwLeft*3+1) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+1) );
				*pDest = *pVecter;

				pDest = (D3DXVECTOR3*)( pLeft + nSIZE*(dwLeft*3+2) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+2) );
				*pDest = *pVecter;

				memcpy ( pLeft + (nSIZE*3)*dwLeft, pSrc + (nSIZE*3)*i, nSIZE*3 );

				++dwLeft;
			}
			else
			{
				pDest = (D3DXVECTOR3*)( pRight + nSIZE*(dwRight*3+0) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+0) );
				*pDest = *pVecter;

				pDest = (D3DXVECTOR3*)( pRight + nSIZE*(dwRight*3+1) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+1) );
				*pDest = *pVecter;

				pDest = (D3DXVECTOR3*)( pRight + nSIZE*(dwRight*3+2) );
				pVecter = (D3DXVECTOR3*)( pSrc + nSIZE*(i*3+2) );
				*pDest = *pVecter;

				memcpy ( pRight + (nSIZE*3)*dwRight, pSrc + (nSIZE*3)*i, nSIZE*3 );

				++dwRight;
			}
		}
	}

	float DISMAX = 0.f;
	DWORD FACEMAX = 0;

	void MakeOctree1 ( POBJOCTREE& pTree, const UINT nSIZE )
	{
		// Note : 이건 완료된 데이터
		float fDisX = pTree->vMax.x - pTree->vMin.x;
		float fDisY = pTree->vMax.y - pTree->vMin.y;
		float fDisZ = pTree->vMax.z - pTree->vMin.z;

		if ( (fDisX<DISMAX) && (fDisY<DISMAX) && (fDisZ<DISMAX) && (pTree->dwFace<FACEMAX) )
		{
			return;
		}

		// Note : 분할을 하자.
		DWORD	dwLeftFace = 0;
		DWORD	dwRightFace = 0;
		BYTE* pLeftVertex = new BYTE[nSIZE*pTree->dwFace*3];
		BYTE* pRightVertex = new BYTE[nSIZE*pTree->dwFace*3];

		D3DXPLANE sPlane;
		D3DXVECTOR3 vCenter = pTree->vMin;
		vCenter.x += (fDisX*0.5f);
		vCenter.y += (fDisY*0.5f);
		vCenter.z += (fDisZ*0.5f);
		if ( (fDisX>fDisY) && (fDisX>fDisZ) )	// X 길다.
		{
			D3DXVECTOR3	v0 = vCenter;
			D3DXVECTOR3	v1 = vCenter;
			D3DXVECTOR3	v2 = vCenter;
			v1.z += 1.f;
			v2.y += 1.f;
			D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );
		}
		else if (fDisZ>fDisY)									// Z 길다.
		{
			D3DXVECTOR3	v0 = vCenter;
			D3DXVECTOR3	v1 = vCenter;
			D3DXVECTOR3	v2 = vCenter;
			v1.x += 1.f;
			v2.y += 1.f;
			D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );
		}								
		else 
		{
			D3DXVECTOR3	v0 = vCenter;
			D3DXVECTOR3	v1 = vCenter;
			D3DXVECTOR3	v2 = vCenter;
			v1.x += 1.f;
			v2.z += 1.f;
			D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );	
		}

		DivideFace ( pTree->pVertex, pLeftVertex, pRightVertex, pTree->dwFace, dwLeftFace, dwRightFace, sPlane, nSIZE );

		// Note : 
		if( !dwLeftFace || !dwRightFace )
		{
			if( pTree->dwFace>=FACEMAX )	MessageBox( NULL, "Static Mesh (Octree Action) pTree->dwFace>=FACEMAX ", "ERROR", MB_OK );

			// Normal - OK
			SAFE_DELETE_ARRAY ( pLeftVertex );
			SAFE_DELETE_ARRAY ( pRightVertex );
			return;
		}

		// Note : 필요 없어진것 삭제
		SAFE_DELETE_ARRAY ( pTree->pVertex );
		pTree->dwFace = 0;

		// Note : Left 생성
		if ( dwLeftFace )
		{
			D3DXVECTOR3 vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
			D3DXVECTOR3 vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
			for ( DWORD i=0; i<dwLeftFace*3; ++i )
			{
				D3DXVECTOR3 *pVecter = (D3DXVECTOR3*)( pLeftVertex + nSIZE*i );

				if ( vMax.x < pVecter->x )	vMax.x = pVecter->x;
				if ( vMax.y < pVecter->y )	vMax.y = pVecter->y;
				if ( vMax.z < pVecter->z )	vMax.z = pVecter->z;

				if ( vMin.x > pVecter->x )	vMin.x = pVecter->x;
				if ( vMin.y > pVecter->y )	vMin.y = pVecter->y;
				if ( vMin.z > pVecter->z )	vMin.z = pVecter->z;
			}

			// 충돌을 위해서
			vMax += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
			vMin -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

			pTree->pLeftChild = new OBJOCTree;
			pTree->pLeftChild->vMax = vMax;
			pTree->pLeftChild->vMin = vMin;
			pTree->pLeftChild->pVertex = pLeftVertex;
			pTree->pLeftChild->dwFace = dwLeftFace;

			MakeOctree1 ( pTree->pLeftChild, nSIZE );
		}
		else
		{
			SAFE_DELETE_ARRAY ( pLeftVertex );
		}

		// Note : Right 생성
		if ( dwRightFace )
		{
			D3DXVECTOR3 vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
			D3DXVECTOR3 vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
			for ( DWORD i=0; i<dwRightFace*3; ++i )
			{
				D3DXVECTOR3 *pVecter = (D3DXVECTOR3*)( pRightVertex + nSIZE*i );

				if ( vMax.x < pVecter->x )	vMax.x = pVecter->x;
				if ( vMax.y < pVecter->y )	vMax.y = pVecter->y;
				if ( vMax.z < pVecter->z )	vMax.z = pVecter->z;

				if ( vMin.x > pVecter->x )	vMin.x = pVecter->x;
				if ( vMin.y > pVecter->y )	vMin.y = pVecter->y;
				if ( vMin.z > pVecter->z )	vMin.z = pVecter->z;
			}

			// 충돌을 위해서
			vMax += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
			vMin -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

			pTree->pRightChild = new OBJOCTree;
			pTree->pRightChild->vMax = vMax;
			pTree->pRightChild->vMin = vMin;
			pTree->pRightChild->pVertex = pRightVertex;
			pTree->pRightChild->dwFace = dwRightFace;

			MakeOctree1 ( pTree->pRightChild, nSIZE );
		}
		else
		{
			SAFE_DELETE_ARRAY ( pRightVertex );
		}
	}

	void MakeOctree ( POBJOCTREE& pTree, BYTE* pVertex, const DWORD dwFace, float fDisMAX, DWORD dwFaceMAX, const UINT nSIZE )
	{
		DISMAX = fDisMAX;
		FACEMAX = dwFaceMAX;

		D3DXVECTOR3 vMax = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMin = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );

		for ( DWORD i=0; i<dwFace*3; ++i )
		{
			D3DXVECTOR3 *pVecter = (D3DXVECTOR3*)( pVertex + nSIZE*i );

			if ( vMax.x < pVecter->x )	vMax.x = pVecter->x;
			if ( vMax.y < pVecter->y )	vMax.y = pVecter->y;
			if ( vMax.z < pVecter->z )	vMax.z = pVecter->z;

			if ( vMin.x > pVecter->x )	vMin.x = pVecter->x;
			if ( vMin.y > pVecter->y )	vMin.y = pVecter->y;
			if ( vMin.z > pVecter->z )	vMin.z = pVecter->z;
		}

		// 충돌을 위해서
		vMax += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vMin -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

		pTree = new OBJOCTree;
		pTree->vMax = vMax;
		pTree->vMin = vMin;
		pTree->pVertex = new BYTE[nSIZE*dwFace*3];
		memcpy ( pTree->pVertex, pVertex, nSIZE*dwFace*3 );
		pTree->dwFace = dwFace;

		MakeOctree1 ( pTree, nSIZE );
	}




	void MakeOcMesh ( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, const DWORD dwFVF )
	{
		if ( pTree->pVertex )
		{
			SAFE_RELEASE ( pTree->pOcMesh );
			D3DXCreateMeshFVF ( pTree->dwFace, pTree->dwFace*3, D3DXMESH_MANAGED, dwFVF, pd3dDevice, &pTree->pOcMesh );
			if ( !pTree->pOcMesh )	return;

			if ( !pTree->pVertex )	return;

			UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );

			BYTE *pBuffer(NULL);
			pTree->pOcMesh->LockVertexBuffer ( 0L, (VOID**)&pBuffer );
			memcpy ( pBuffer, pTree->pVertex, nSIZE*pTree->dwFace*3 );
			pTree->pOcMesh->UnlockVertexBuffer();

			WORD* pIndices;
			pTree->pOcMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
			for ( DWORD i=0; i<pTree->dwFace*3; ++i )
			{
				pIndices[i] = (WORD)i;
			}
			pTree->pOcMesh->UnlockIndexBuffer();

			SAFE_DELETE_ARRAY ( pTree->pVertex );
			//pTree->dwFace = 0;

			return;
		}

		if ( pTree->pLeftChild )	MakeOcMesh ( pd3dDevice, pTree->pLeftChild, dwFVF );
		if ( pTree->pRightChild )	MakeOcMesh ( pd3dDevice, pTree->pRightChild, dwFVF );
	}




	void MakeOptimizeMesh ( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree )
	{
		if ( pTree->pOcMesh )
		{
			// Note : 메쉬의 최적화
			NSOPTIMIZEMESH::MakeOptimizeMesh( pd3dDevice, pTree->pOcMesh );
			return;
		}

		if ( pTree->pLeftChild )	MakeOptimizeMesh ( pd3dDevice, pTree->pLeftChild );
		if ( pTree->pRightChild )	MakeOptimizeMesh ( pd3dDevice, pTree->pRightChild );
	}



	void MakeCollisionData2 ( LPD3DXMESH pMesh, PDXAABBNODE& pTree )
	{
		DWORD dwFaceNUM = pMesh->GetNumFaces();

		WORD *pLockIndices=NULL;
		PBYTE pbLockPoints = NULL;
		pMesh->LockIndexBuffer( 0L, (VOID**)&pLockIndices );
		pMesh->LockVertexBuffer( 0L, (VOID**)&pbLockPoints );

		WORD *pFaces;
		pFaces = new WORD[dwFaceNUM];
		if ( pFaces == NULL )	return;// E_OUTOFMEMORY;

		for ( WORD i=0; i<dwFaceNUM; i++ )
			pFaces[i] = i;

		D3DXMATRIX	matComb;
		D3DXMatrixIdentity ( &matComb );

		D3DXVECTOR3 vMax, vMin;
		COLLISION::GetSizeNode ( matComb, pLockIndices, pbLockPoints, pMesh->GetFVF(), pFaces, dwFaceNUM, vMax, vMin );

		DxAABBNode *pNode = new DxAABBNode;
		pNode->fMaxX = vMax.x;
		pNode->fMaxY = vMax.y;
		pNode->fMaxZ = vMax.z;
		pNode->fMinX = vMin.x;
		pNode->fMinY = vMin.y;
		pNode->fMinZ = vMin.z;

		COLLISION::MakeAABBNode ( pNode, matComb, pLockIndices, pbLockPoints, pMesh->GetFVF(), pFaces, dwFaceNUM, vMax, vMin );

		pTree = pNode;
		
		pMesh->UnlockIndexBuffer();
		pMesh->UnlockVertexBuffer();

		SAFE_DELETE_ARRAY( pFaces );
	}

	void MakeCollisionData ( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree )
	{
		if( !pTree )	return;

		if ( pTree->pOcMesh )
		{
			MakeCollisionData2 ( pTree->pOcMesh, pTree->pCollsionTREE );
			return;
		}

		if ( pTree->pLeftChild )	MakeCollisionData ( pd3dDevice, pTree->pLeftChild );
		if ( pTree->pRightChild )	MakeCollisionData ( pd3dDevice, pTree->pRightChild );
	}



	void MakeVBIBAndDelMesh( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree )
	{
		if( !pTree )	return;

		// Note : VertexBuffer, IndexBuffer, pNormal을 생성하고 Mesh는 삭제한다.
		//		Mesh는 Normal을 포함하고 있는 경우일 것이다. 분해 후 넣어야 한다.
		if( pTree->pOcMesh )
		{
			// Note : FVF를 얻는다.
			DWORD dwFVF = pTree->pOcMesh->GetFVF();
			DWORD dwVert = pTree->pOcMesh->GetNumVertices();
			DWORD dwFaces = pTree->pOcMesh->GetNumFaces();

			// Note : 기본적인 VB, IB, DIRECTPOINTCOLOR, 등을 생성한다.
			pTree->CreateBASE( pd3dDevice, dwVert, dwFaces );

			// Note : 데이터를 옮긴다.
			BYTE*	pVertices(NULL);
			pTree->pOcMesh->LockVertexBuffer( 0L, (VOID**)&pVertices );
			pTree->CreateVertSrc( pVertices, dwFVF, dwVert );				// VertSrc를 생성한다.
			pTree->CreateNormalColor( pVertices, dwFVF, dwVert );			// NormalColor를 생성한다.

			// Note : 렌더에 쓰일 VB를 셋팅한다.
			pTree->CopyVertSrcToVB( pVertices, pTree->m_pVertSrc, pTree->m_pVB, dwVert );
			pTree->pOcMesh->UnlockVertexBuffer();

			// Note : 렌더에 쓰일 IB를 셋팅한다.
			WORD* pIndexSRC(NULL);
			pTree->pOcMesh->LockIndexBuffer( 0L, (VOID**)&pIndexSRC );
			pTree->CopyMeshToIB( pIndexSRC, pTree->m_pIB, dwFaces );
			pTree->pOcMesh->UnlockIndexBuffer();

			// Note : 마지막으로 삭제한다.
			SAFE_RELEASE( pTree->pOcMesh );

			return;
		}

		if( pTree->pLeftChild )		MakeVBIBAndDelMesh( pd3dDevice, pTree->pLeftChild );
		if( pTree->pRightChild )	MakeVBIBAndDelMesh( pd3dDevice, pTree->pRightChild );
	}

	void DeleteNormalColor( OBJOCTree* pTree )
	{
		if( !pTree )	return;

		SAFE_DELETE_ARRAY( pTree->m_pColor );
		
		if( pTree->pLeftChild )		DeleteNormalColor( pTree->pLeftChild );
		if( pTree->pRightChild )	DeleteNormalColor( pTree->pRightChild );
	}

	void CovertPieceSetting( LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree )
	{
		if( !pTree )	return;

		pTree->CovertPieceSetting( pd3dDevice );
		
		if( pTree->pLeftChild )		CovertPieceSetting( pd3dDevice, pTree->pLeftChild );
		if( pTree->pRightChild )	CovertPieceSetting( pd3dDevice, pTree->pRightChild );
	}


	void MakeLoadCheckOK( OBJOCTree* pTree )
	{
		pTree->m_bLoad = TRUE;

		if ( pTree->pLeftChild )	MakeLoadCheckOK( pTree->pLeftChild );
		if ( pTree->pRightChild )	MakeLoadCheckOK( pTree->pRightChild );
	}




	void InsertColorList( OBJOCTree* pTree, LOADINGDATALIST &listColorData )
	{
		if( !pTree )	return;

		pTree->InsertColorList( listColorData );

		InsertColorList( pTree->pLeftChild, listColorData );
		InsertColorList( pTree->pRightChild, listColorData );
	}




	void RenderOctree ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree )
	{
		if ( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume ( sCV, pTree->vMax, pTree->vMin ) )	return;

		if ( pTree->IsLoad() )
		{
			//pTree->pOcMesh->DrawSubset(0);	// Modify
			pTree->DrawEff( pd3dDevice );
			return;
		}

		if ( pTree->pLeftChild )	RenderOctree ( pd3dDevice, sCV, pTree->pLeftChild );
		if ( pTree->pRightChild )	RenderOctree ( pd3dDevice, sCV, pTree->pRightChild );
	}

	void RenderOctree_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree, DxTexEffFlowUV* pEff )
	{
		if ( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume ( sCV, pTree->vMax, pTree->vMin ) )	return;

		if ( pTree->IsLoad() )
		{
			pd3dDevice->SetIndices ( pTree->m_pIB );			
			pEff->Render( pd3dDevice, pTree->m_dwVert, pTree->m_dwFace, pTree->m_pVertSrc );
			return;
		}

		if ( pTree->pLeftChild )	RenderOctree_FlowUV( pd3dDevice, sCV, pTree->pLeftChild, pEff );
		if ( pTree->pRightChild )	RenderOctree_FlowUV( pd3dDevice, sCV, pTree->pRightChild, pEff );
	}

	void RenderOctree_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree, DxTexEffRotate* pEff )
	{
		if ( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume ( sCV, pTree->vMax, pTree->vMin ) )	return;

		if ( pTree->IsLoad() )
		{
			pd3dDevice->SetIndices ( pTree->m_pIB );
			pEff->Render( pd3dDevice, pTree->m_dwVert, pTree->m_dwFace, pTree->m_pVertSrc );
			return;
		}

		if ( pTree->pLeftChild )	RenderOctree_Rotate( pd3dDevice, sCV, pTree->pLeftChild, pEff );
		if ( pTree->pRightChild )	RenderOctree_Rotate( pd3dDevice, sCV, pTree->pRightChild, pEff );
	}

	void RenderList ( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree )
	{
		if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pTree->DrawEff( pd3dDevice );
			return;
		}

		if ( pTree->pLeftChild )	RenderList( pd3dDevice, pTree->pLeftChild );
		if ( pTree->pRightChild )	RenderList( pd3dDevice, pTree->pRightChild );
	}

	void RenderList_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, DxTexEffFlowUV* pEff )
	{
		if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pd3dDevice->SetIndices ( pTree->m_pIB );
			pEff->Render( pd3dDevice, pTree->m_dwVert, pTree->m_dwFace, pTree->m_pVertSrc );
			return;
		}

		if ( pTree->pLeftChild )	RenderList_FlowUV( pd3dDevice, pTree->pLeftChild, pEff );
		if ( pTree->pRightChild )	RenderList_FlowUV( pd3dDevice, pTree->pRightChild, pEff );
	}

	void RenderList_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, DxTexEffRotate* pEff )
	{
		if ( !pTree )	return;

		if ( pTree->IsLoad() )
		{
			pd3dDevice->SetIndices ( pTree->m_pIB );
			pEff->Render( pd3dDevice, pTree->m_dwVert, pTree->m_dwFace, pTree->m_pVertSrc );
			return;
		}

		if ( pTree->pLeftChild )	RenderList_Rotate( pd3dDevice, pTree->pLeftChild, pEff );
		if ( pTree->pRightChild )	RenderList_Rotate( pd3dDevice, pTree->pRightChild, pEff );
	}



	float	g_fCOLL_LENGTH = FLT_MAX;
	void IsCollision ( DxAABBNode *pAABBCur, OBJOCTree* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, 
						D3DXVECTOR3 &vCollision, D3DXVECTOR3 &vNormal, BOOL& bColl, const BOOL bFrontColl )
	{
		D3DXVECTOR3 vNewP1=vStart, vNewP2=vEnd;

		if ( pAABBCur->IsCollision(vNewP1,vNewP2) )
		{
			if ( pAABBCur->dwFace != AABB_NONINDEX)
			{
				PBYTE pbDataVB;
				PWORD pwIndexB;

				//if( FAILED( pTree->m_pVB->Lock( 0, 0, (VOID**)&pbDataVB, D3DLOCK_READONLY ) ) )
				//	return;
				pbDataVB = (PBYTE)pTree->m_pVertSrc;
				if( FAILED( pTree->m_pIB->Lock( 0, 0, (VOID**)&pwIndexB, D3DLOCK_READONLY ) ) )
				{
					//pTree->m_pVB->Unlock();
					return;
				}

				DWORD fvfsize = sizeof(VERTEXNORCOLORTEX);		// CAUTION : pTree->m_pVertSrc의 자료형이 바뀌었을 시 안 바꿔주면 큰 문제가 생긴다.
				PWORD pwIndex = pwIndexB + pAABBCur->dwFace*3;
				LPD3DXVECTOR3 pvT0 = (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex)));
				LPD3DXVECTOR3 pvT1 = (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex+1)));
				LPD3DXVECTOR3 pvT2 = (D3DXVECTOR3*) (pbDataVB+fvfsize*(*(pwIndex+2)));


				D3DXVECTOR3 vNewCollision;
				D3DXVECTOR3 vNewNormal;
				vNewP1=vStart;
				if ( COLLISION::IsLineTriangleCollision( pvT0, pvT1, pvT2, &vNewP1, &vNewP2, &vNewCollision, &vNewNormal, bFrontColl ) )
				{
					bColl = TRUE;		// 충돌 되었다는 것 확인.

					D3DXVECTOR3 vDxVect;
					vDxVect = vStart - vNewCollision;
					float fNewLength = D3DXVec3LengthSq ( &vDxVect );

					if ( fNewLength < g_fCOLL_LENGTH )
					{
						g_fCOLL_LENGTH = fNewLength;
						vCollision	= vNewCollision;
						vNormal		= vNewNormal;

						vEnd		= vCollision;
					}
				}

				//pTree->m_pVB->Unlock();
				pTree->m_pIB->Unlock();
			}
			else
			{
				if ( pAABBCur->pLeftChild )
					IsCollision ( pAABBCur->pLeftChild, pTree, vStart, vEnd, vCollision, vNormal, bColl, bFrontColl );

				if ( pAABBCur->pRightChild )
					IsCollision ( pAABBCur->pRightChild, pTree, vStart, vEnd, vCollision, vNormal, bColl, bFrontColl );
			}
		}
	}

	void CollisionLine2( OBJOCTree* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
						BOOL& bColl, const BOOL bFrontColl )
	{
		if ( !pTree )	return;

		if ( !COLLISION::IsCollisionLineToAABB( vStart, vEnd, pTree->vMax, pTree->vMin ) )	return;

		if ( pTree->pCollsionTREE )
		{
			IsCollision( pTree->pCollsionTREE, pTree, vStart, vEnd, vColl, vNor, bColl, bFrontColl );
		}

		if ( pTree->pLeftChild )	CollisionLine2( pTree->pLeftChild, vStart, vEnd, vColl, vNor, bColl, bFrontColl );
		if ( pTree->pRightChild )	CollisionLine2( pTree->pRightChild, vStart, vEnd, vColl, vNor, bColl, bFrontColl );
	}

	void CollisionLine ( OBJOCTree* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor,
						BOOL& bColl, const BOOL bFrontColl )
	{
		g_fCOLL_LENGTH	= FLT_MAX;
		CollisionLine2 ( pTree, vStart, vEnd, vColl, vNor, bColl, bFrontColl );
	}




	void SaveOctree ( CSerialFile& SFile, OBJOCTree* pTree, BOOL bPiece )
	{
		if ( pTree )
		{
			SFile << (BOOL)TRUE;
			pTree->Save ( SFile, bPiece );
		}
		else
		{
			SFile << (BOOL)FALSE;
			return;
		}

		SaveOctree ( SFile, pTree->pLeftChild, bPiece );
		SaveOctree ( SFile, pTree->pRightChild, bPiece );
	}

	void LoadOctree ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, POBJOCTREE& pTree, const BOOL bDynamicLoad, BOOL bPiece )
	{
		BOOL bUse = TRUE;
		SFile >> bUse;

		if ( bUse )
		{
			pTree = new OBJOCTree;
			pTree->Load( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
		else
		{
			return;
		}

		LoadOctree( pd3dDevice, SFile, pTree->pLeftChild, bDynamicLoad, bPiece );
		LoadOctree( pd3dDevice, SFile, pTree->pRightChild, bDynamicLoad, bPiece );
	}

	void FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, OBJOCTree* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
	{
		if( !pTree )	return;

		if ( !COLLISION::IsCollisionAABBToAABB( vMax, vMin, pTree->vMax, pTree->vMin ) )	return;

		pTree->DynamicLoad( pd3dDevice, SFile, FALSE );

		FirstLoad( pd3dDevice, SFile, pTree->pLeftChild, vMax, vMin );
		FirstLoad( pd3dDevice, SFile, pTree->pRightChild, vMax, vMin );
	}

	void DynamicLoad ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, OBJOCTree* pTree, const CLIPVOLUME &sCV )
	{
		if( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->vMax, pTree->vMin ) )	return;

		pTree->DynamicLoad( pd3dDevice, SFile, FALSE );

		DynamicLoad ( pd3dDevice, SFile, pTree->pLeftChild, sCV );
		DynamicLoad ( pd3dDevice, SFile, pTree->pRightChild, sCV );
	}

	void ThreadLoad ( OBJOCTree* pTree, const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading )
	{
		if( !pTree )	return;

		if ( !COLLISION::IsCollisionVolume( sCV, pTree->vMax, pTree->vMin ) )	return;

		pTree->ThreadLoad( listLoadingData, CSLockLoading );

		ThreadLoad( pTree->pLeftChild, sCV, listLoadingData, CSLockLoading );
		ThreadLoad( pTree->pRightChild, sCV, listLoadingData, CSLockLoading );
	}

	void LoadOctreeVER_100 ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, POBJOCTREE& pTree )
	{
		BOOL bUse = TRUE;
		SFile >> bUse;

		if ( bUse )
		{
			pTree = new OBJOCTree;
			pTree->Load_VER100 ( pd3dDevice, SFile );
		}
		else
		{
			return;
		}

		LoadOctreeVER_100 ( pd3dDevice, SFile, pTree->pLeftChild );
		LoadOctreeVER_100 ( pd3dDevice, SFile, pTree->pRightChild );
	}
};


