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

#include "./DxCharPart.h"

#include "./DxCharAroundEff.h"

DWORD		DxCharAroundEff::TYPEID			= EMEFFCHAR_AROUNDEFFECT;
DWORD		DxCharAroundEff::VERSION			= 0x0001;
char		DxCharAroundEff::NAME[MAX_PATH]	= "2.CharAroundEff<...>"; 

DxCharAroundEff::DxCharAroundEff(void)
{
	m_pTexture = NULL;	
}

DxCharAroundEff::~DxCharAroundEff(void)
{
	CleanUp();
}

DxEffChar* DxCharAroundEff::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	DxCharAroundEff *pEffChar = new DxCharAroundEff;
	pEffChar->SetProperty ( &m_Property );
	for( DWORD i = 0; i < m_vecAroundEffList.size(); i++ )
	{
		pEffChar->InsertAroundEffect( m_vecAroundEffList[i] );
	}

	HRESULT hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}
HRESULT	DxCharAroundEff::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	if ( dwVer == VERSION )
	{
		//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
		SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

		SFile >> dwSize;
		for( DWORD i = 0; i < dwSize; i++ )
		{
			SAROUNDEFF_DATA loadData;
			//SFile.ReadBuffer( &loadData, sizeof(loadData) );
			SFile >> loadData.vPos;
			SFile >> loadData.strBoneName;
			SFile >> loadData.vNormal;
			SFile >> loadData.fMeshDist;

			m_vecAroundEffList.push_back(loadData);
		}

		//	Note : Device 자원을 생성한다.
		hr = Create( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		SFile.SetOffSet( SFile.GetfTell()+dwSize );

		return E_FAIL;
	}

	return S_OK;
}
HRESULT	DxCharAroundEff::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	SFile << m_vecAroundEffList.size();
	for( DWORD i = 0; i < m_vecAroundEffList.size(); i++ )
	{
		SAROUNDEFF_DATA saveData = m_vecAroundEffList[i];
		SFile << saveData.vPos;
		SFile << saveData.strBoneName;
		SFile << saveData.vNormal;
		SFile << saveData.fMeshDist;
		
//		SFile.WriteBuffer( &saveData, sizeof(saveData) );
	}

	return S_OK;
}

void DxCharAroundEff::InsertAroundEffect( SAROUNDEFF_DATA addData, bool bTool /* = FALSE */ )
{
	if( bTool )
	{
		DxBoneTrans*	pBoneTran;
		pBoneTran = m_pSkeleton->FindBone( addData.strBoneName.c_str() );

		if( !pBoneTran ) return;

		addData.vPos.x = addData.vPos.x - pBoneTran->matCombined._41;
		addData.vPos.y = addData.vPos.y - pBoneTran->matCombined._42;
		addData.vPos.z = addData.vPos.z - pBoneTran->matCombined._43;
	}
	addData.CreateParticleData(m_Property);


	addData.fRunTime = RandomNumber( 0.0f, (addData.fRenderLifeTime + addData.fRenderRivalTime) );

	m_vecAroundEffList.push_back(addData);
}



void DxCharAroundEff::DeleteAroundEffect( int nIndex )
{
	if( m_vecAroundEffList.size() < (size_t)nIndex ) return;
	m_vecAroundEffList.erase( m_vecAroundEffList.begin() + nIndex );
}

HRESULT DxCharAroundEff::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}
HRESULT DxCharAroundEff::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}
HRESULT DxCharAroundEff::OneTimeSceneInit ()
{
	return S_OK;
}
HRESULT DxCharAroundEff::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture( m_szTexture, pd3dDevice, m_pTexture, 0, 0, FALSE );

	for( DWORD i = 0; i < m_vecAroundEffList.size(); i++ )
	{	
		m_vecAroundEffList[i].CreateParticleData(m_Property);
		m_vecAroundEffList[i].fRunTime = RandomNumber( 0.0f, (m_vecAroundEffList[i].fRenderLifeTime + m_vecAroundEffList[i].fRenderRivalTime) );
	}

	
	return S_OK;
}
HRESULT DxCharAroundEff::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{

	return S_OK;
}
HRESULT DxCharAroundEff::InvalidateDeviceObjects ()
{
	return S_OK;
}
HRESULT DxCharAroundEff::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );
	return S_OK;
}
HRESULT DxCharAroundEff::FinalCleanup ()
{
	return S_OK;
}

void SAROUNDEFF_DATA::CreateParticleData( EFFCHAR_PROPERTY_CHAR_AROUND property )
{
	fRunTime	   = 0;
	fRunTextureNum = 0;
	bAlive		   = TRUE;

	vRenderSize.x		= RandomNumber( property.m_vMinSize.x,		 property.m_vMaxSize.x );
	vRenderSize.y		= RandomNumber( property.m_vMinSize.y,	     property.m_vMaxSize.y );
	fRenderLifeTime		= RandomNumber( property.m_fMinLifeTime,     property.m_fMaxLifeTime );
	fRenderRivalTime	= RandomNumber( property.m_fMinRivalTime,    property.m_fMaxRivalTime );
	fRunTexAnimateSpeed = RandomNumber( property.m_fMinTextureSpeed, property.m_fMaxTextureSpeed );
	fRenderRotate		= RandomNumber( 0.0f, 360.0f );

}

void SAROUNDEFF_DATA::FrameMove( float fElapsedTime, EFFCHAR_PROPERTY_CHAR_AROUND property )
{
	if( property.m_dwFlag & USE_SEQUENCE )
	{
		fRunTextureNum += (fRunTexAnimateSpeed*fElapsedTime);
		if( fRunTextureNum >= property.m_dwSequenceCol*property.m_dwSequenceRow )
		{
			fRunTexAnimateSpeed = RandomNumber( property.m_fMinTextureSpeed, property.m_fMaxTextureSpeed );
			fRunTextureNum	 = 0.0f;
		}	
	}

	fRunTime += fElapsedTime;

	if( bAlive )
	{
		if( fRunTime > fRenderLifeTime )
		{
			bAlive = FALSE;
		}
	}else{
		if( fRunTime > fRenderLifeTime + fRenderRivalTime )
		{
			CreateParticleData(property);
		}
	}
}


HRESULT DxCharAroundEff::FrameMove ( float fTime, float fElapsedTime )
{
	SAROUNDEFF_DATA data;
	for( DWORD i = 0; i < m_vecAroundEffList.size(); i++ )
	{
		m_vecAroundEffList[i].FrameMove( fElapsedTime, m_Property );
	}
	
	return S_OK;
}
void DxCharAroundEff::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )
{
	D3DXVECTOR3	vPos, vBonePos, vEffectPos, vTemp, vLocal;

	DxBoneTrans*	pBoneTran;
	SAROUNDEFF_DATA	data;
	D3DXVECTOR2		vStartUV;
	D3DXVECTOR2		vEndUV;
	D3DXMATRIX		matRotate;
	int				nCount = 0; 

	ZeroMemory( m_Vertices, sizeof(m_Vertices) );


	for( DWORD i = 0; i < m_vecAroundEffList.size(); i++ )
	{	

		data = m_vecAroundEffList[i];

		if( data.bAlive == FALSE )
			continue;

		if( m_Property.m_dwFlag & USE_SEQUENCE )
		{
			DWORD dwAnimateNum = (DWORD)data.fRunTextureNum;
			DWORD dwAnimateX   = dwAnimateNum % m_Property.m_dwSequenceCol;
			DWORD dwAnimateY   = dwAnimateNum / m_Property.m_dwSequenceCol;

			vStartUV.x = (1.0f/m_Property.m_dwSequenceCol) * (float)dwAnimateX; 
			vStartUV.y = (1.0f/m_Property.m_dwSequenceRow) * (float)dwAnimateY; 
			vEndUV.x   = (1.0f/m_Property.m_dwSequenceCol) * (float)(dwAnimateX+1); 
			vEndUV.y   = (1.0f/m_Property.m_dwSequenceRow) * (float)(dwAnimateY+1); 
		}

		nCount++;

		pBoneTran = m_pSkeleton->FindBone( data.strBoneName.c_str() );

		if( pBoneTran )
		{
			VERTEXCOLORTEX2 vertices[4];
			vBonePos = D3DXVECTOR3( pBoneTran->matCombined._41, pBoneTran->matCombined._42, pBoneTran->matCombined._43 );
			vEffectPos = data.vPos + vBonePos;
			D3DXVec3Add( &vPos, &vEffectPos, D3DXVec3Scale( &vTemp, &data.vNormal, data.fMeshDist ) );

			D3DXMATRIX	matLocal = DxBillboardLookAt( &vPos, DxViewPort::GetInstance().GetMatView() );
			D3DXMatrixRotationZ( &matRotate, D3DXToRadian(data.fRenderRotate) );
			matLocal = matRotate * matLocal;

			// Note : 점들의 적용			
			vLocal = D3DXVECTOR3( -data.vRenderSize.x,  data.vRenderSize.y, 0.0f );
			D3DXVec3TransformCoord( &vertices[0].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3(  data.vRenderSize.x,  data.vRenderSize.y, 0.0f );
			D3DXVec3TransformCoord( &vertices[1].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( -data.vRenderSize.x, -data.vRenderSize.y, 0.0f );
			D3DXVec3TransformCoord( &vertices[2].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3(  data.vRenderSize.x, -data.vRenderSize.y, 0.0f );
			D3DXVec3TransformCoord( &vertices[3].vPos, &vLocal, &matLocal );


			// Note : 시퀀스를 사용할 경우와 아닐 경우
			if( m_Property.m_dwFlag & USE_SEQUENCE )
			{
				vertices[0].vTex01 = D3DXVECTOR2(vStartUV.x, vStartUV.y);
				vertices[1].vTex01 = D3DXVECTOR2(vEndUV.x,	 vStartUV.y);
				vertices[2].vTex01 = D3DXVECTOR2(vStartUV.x, vEndUV.y);
				vertices[3].vTex01 = D3DXVECTOR2(vEndUV.x,	 vEndUV.y);
			}
			else
			{
				vertices[0].vTex01 = D3DXVECTOR2( 0.f, 0.f );
				vertices[1].vTex01 = D3DXVECTOR2( 1.f, 0.f );
				vertices[2].vTex01 = D3DXVECTOR2( 0.f, 1.f );
				vertices[3].vTex01 = D3DXVECTOR2( 1.f, 1.f );
			}

			vertices[0].vTex02 = D3DXVECTOR2( 0.f, 0.f );
			vertices[1].vTex02 = D3DXVECTOR2( 1.f, 0.f );
			vertices[2].vTex02 = D3DXVECTOR2( 0.f, 1.f );
			vertices[3].vTex02 = D3DXVECTOR2( 1.f, 1.f );

			
			vertices[0].dwColor = vertices[1].dwColor = vertices[2].dwColor = vertices[3].dwColor = m_Property.m_dwColor;

			memcpy( &m_Vertices[i*4], vertices, sizeof(vertices) );
			
		}
		
	}

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
		pData->bAlphaTex = FALSE;
		pData->nVertexCount = DxDynamicVB::m_sVB.nVertexCount;
		pData->nFaceCount = nCount;

		// Note : 데이터 복사
		VERTEXCOLORTEX2 *pVertices;
		DxDynamicVB::m_sVB.pVB->Lock( DxDynamicVB::m_sVB.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
		memcpy( pVertices, m_Vertices, dwVertexSizeFULL );
		DxDynamicVB::m_sVB.pVB->Unlock ();

		// Note : 값을 더해줌. 
		DxDynamicVB::m_sVB.nVertexCount += nCount*4;
		DxDynamicVB::m_sVB.nOffsetToLock += dwVertexSizeFULL;

		// Note : 나중에 뿌리도록 셋팅 해 놓는다.
		OPTMManager::GetInstance().m_sCharParticle.InsertData( pData );
	}
}
void DxCharAroundEff::RenderCountText( DWORD nCount, const D3DXVECTOR3& vPos )
{
	D3DXVECTOR3 vOut;
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	D3DXVec3Project( &vOut, &vPos, &DxViewPort::GetInstance().GetViewPort(), &DxViewPort::GetInstance().GetMatProj(), 
		&DxViewPort::GetInstance().GetMatView(), &matIdentity );

	CDebugSet::ToPos( vOut.x, vOut.y, "%u", nCount );
}
void DxCharAroundEff::RenderEDIT( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )
{
	D3DXVECTOR3	vPos(0.f,0.f,0.f);

	DxBoneTrans*	pBoneTran;
	SAROUNDEFF_DATA	data;

	for( DWORD i = 0; i < m_vecAroundEffList.size(); i++ )
	{	

		data = m_vecAroundEffList[i];

		pBoneTran = m_pSkeleton->FindBone( data.strBoneName.c_str() );

		if( pBoneTran )
		{
			D3DXVECTOR3 vBonePos = D3DXVECTOR3( pBoneTran->matCombined._41, pBoneTran->matCombined._42, pBoneTran->matCombined._43 ), vLocal;
			vPos = data.vPos + vBonePos;		
			RenderCountText( i, vPos );
		}
	}
}