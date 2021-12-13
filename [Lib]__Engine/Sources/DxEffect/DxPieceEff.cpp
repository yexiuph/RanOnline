#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./SerialFile.h"
#include "./DxAnimationManager.h"
#include "./DxFrameMesh.h"
#include "./DxPieceEffMan.h"
#include "./DxStaticMesh.h"

#include "./DxPieceEff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//----------------------------------------------------------------------------------------------------------------------
//									D	x		L	a	n	d		E	f	f
//----------------------------------------------------------------------------------------------------------------------
DxLandEff::DxLandEff() :
	m_pFrame(NULL),
	m_bGroudPos(FALSE),
	m_bCheckColl(FALSE),
	m_vCollPos(0.f,0.f,0.f)
{
	m_pPropGroup = NULL;
	m_pSingleGroup = NULL;
	m_strEffName = "";
	m_strEditName = "";
	m_strFrameName = "";

	D3DXMatrixIdentity( &m_matLocal );
	D3DXMatrixIdentity( &m_matLast );
}

DxLandEff::~DxLandEff()
{
	CleanUp();
}

void DxLandEff::CleanUp()
{
	m_bGroudPos = FALSE;
	m_bCheckColl = FALSE;
	m_vCollPos = D3DXVECTOR3(0.f,0.f,0.f);

	D3DXMatrixIdentity( &m_matLocal );
	D3DXMatrixIdentity( &m_matLast );

	m_strEditName = "";
	m_strEffName = "";
	m_strFrameName = "";

	SAFE_DELETE( m_pSingleGroup );

	// Note : 참조를 하기 때문에 삭제는 안됨.
	m_pPropGroup = NULL;
	m_pFrame = NULL;
}

void DxLandEff::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_pSingleGroup )	return;
	m_pSingleGroup->RestoreDeviceObjects( pd3dDevice );
}

void DxLandEff::OnLostDevice()
{
	if( !m_pSingleGroup )	return;
	m_pSingleGroup->InvalidateDeviceObjects();
}

BOOL DxLandEff::CreateEff( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame, const D3DXVECTOR3& vPos, 
							const char* szFrameName, const char* szEffName, const char* szEditName )
{
	m_pFrame = pFrame;

	D3DXMATRIX matWorld;
	if( m_pFrame )
	{
		D3DXVECTOR3 vTemp;
		D3DXMATRIX matInverse;
		D3DXMatrixInverse( &matInverse, NULL, &m_pFrame->matCombined );
		D3DXVec3TransformCoord( &vTemp, &vPos, &matInverse );

		m_matLocal._41 = vTemp.x;
		m_matLocal._42 = vTemp.y;
		m_matLocal._43 = vTemp.z;

		D3DXMatrixMultiply( &matWorld, &m_matLocal, &m_pFrame->matCombined );
		m_matLast = matWorld;
	}
	else
	{
		m_matLocal._41 = vPos.x;
		m_matLocal._42 = vPos.y;
		m_matLocal._43 = vPos.z;

		matWorld = m_matLocal;
	}

	m_strEffName = szEffName;
	m_strEditName = szEditName;
	m_strFrameName = szFrameName;

	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( szEffName );

	if ( m_pPropGroup )
	{	
		SAFE_DELETE(m_pSingleGroup);
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP();
		if( !m_pSingleGroup )	return FALSE;

		// Note : Eff Group Create Dev.
		HRESULT hr = m_pSingleGroup->Create( pd3dDevice );

		// Note : 초기 위치 설정
		m_pSingleGroup->m_matWorld = matWorld;

		if( hr==S_OK )	return TRUE;
		else			return FALSE;
	}

	return FALSE;
}

LPD3DXMATRIX DxLandEff::GetEffMatrix()
{
	if( m_pFrame )	return &m_matLast;
	else			return &m_matLocal;
}

void DxLandEff::ModifyMatrix()
{
	if( !m_pFrame )	return;

	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &m_pFrame->matCombined );
	D3DXMatrixMultiply( &m_matLocal, &m_matLast, &matInverse );	
}

BOOL DxLandEff::ModifyEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* pEffName )
{
	SAFE_DELETE( m_pSingleGroup );
	DxEffSinglePropGMan::GetInstance().DeleteEffectGProp( m_strEffName.c_str() );
	m_strEffName = pEffName;

	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_strEffName.c_str() );
	if ( m_pPropGroup )
	{	
		SAFE_DELETE(m_pSingleGroup);
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP();
		if( !m_pSingleGroup )	return FALSE;

		// Note : Eff Group Create Dev.
		HRESULT hr = m_pSingleGroup->Create( pd3dDevice );

		// Note : 초기 위치 설정
		m_pSingleGroup->m_matWorld = m_matLocal;

		if( hr==S_OK )	return TRUE;
		else			return FALSE;
	}
	else return FALSE;
}

void DxLandEff::SetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	if( !m_pSingleGroup )	return;

	D3DXVECTOR3 vEffMax, vEffMin;
	m_pSingleGroup->GetAABBSize( vEffMax, vEffMin );

	D3DXMATRIX matWorld;
	if( m_pFrame )	D3DXMatrixMultiply( &matWorld, &m_matLocal, &m_pFrame->matCombined );	
	else			matWorld = m_matLocal;

	D3DXMatrixAABBTransform( vEffMax, vEffMin, matWorld );

	vMax.x = ( vMax.x < vEffMax.x ) ? vEffMax.x : vMax.x;
	vMax.y = ( vMax.y < vEffMax.y ) ? vEffMax.y : vMax.y;
	vMax.z = ( vMax.z < vEffMax.z ) ? vEffMax.z : vMax.z;

	vMin.x = ( vMin.x > vEffMin.x ) ? vEffMin.x : vMin.x;
	vMin.y = ( vMin.y > vEffMin.y ) ? vEffMin.y : vMin.y;
	vMin.z = ( vMin.z > vEffMin.z ) ? vEffMin.z : vMin.z;
}

void DxLandEff::FrameMove( const float fTime, const float fElapsedTime )
{
	if( !m_pSingleGroup )	return;
	m_pSingleGroup->FrameMove( fTime, fElapsedTime );
}

void DxLandEff::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh )
{
	if( !m_pSingleGroup )	return;

	if( m_pFrame )
	{
		D3DXMATRIX matTemp;
		D3DXMatrixMultiply( &matTemp, &m_matLocal, &m_matFrame );
		m_pSingleGroup->m_matWorld = matTemp;
	}
	else
	{
		D3DXMATRIX matTemp;
		D3DXMatrixMultiply( &matTemp, &m_matLocal, &matWorld );
		m_pSingleGroup->m_matWorld = matTemp;
	}

	// Note : Collision 체크된 효과
	if( m_bGroudPos )
	{
		if( !m_bCheckColl )	// Collision은 1번만 한다.
		{
			if( pStaticMesh )
			{
				D3DXVECTOR3 vStart, vEnd, vNor;
				vStart.x = m_pSingleGroup->m_matWorld._41;
				vStart.y = m_pSingleGroup->m_matWorld._42;
				vStart.z = m_pSingleGroup->m_matWorld._43;

				vEnd = vStart;
				vEnd.y -= 1000000.f;

				// Note : 충돌 되었을 때만.
				//			bFrontColl = TRUE
				if( pStaticMesh->IsCollisionLine( vStart, vEnd, m_vCollPos, vNor, TRUE ) )	m_bCheckColl = TRUE;
			}
			if( pFrameMesh )
			{
				//D3DXVECTOR3 vStart, vEnd;
				//vStart.x = m_pSingleGroup->m_matWorld._41;
				//vStart.y = m_pSingleGroup->m_matWorld._42;
				//vStart.z = m_pSingleGroup->m_matWorld._43;

				//vEnd = vStart;
				//vEnd.y -= 1000000.f;

				//char szName[256];
				//if( pFrameMesh->IsCollision( vStart, vEnd, szName ) )
				//{
				//	m_vCollPos = pFrameMesh->GetCollisionPos();

				//	m_bCheckColl = TRUE;
				//}
			}

			// 체크 할 때는 뿌리지 않는다.
			return;
		}
		else				// 위치의 변화를 줌.
		{
			m_pSingleGroup->m_matWorld._41 = m_vCollPos.x;
			m_pSingleGroup->m_matWorld._42 = m_vCollPos.y;
			m_pSingleGroup->m_matWorld._43 = m_vCollPos.z;			
		}
	}
	
	m_pSingleGroup->Render( pd3dDevice );
}

void DxLandEff::Export( LPDIRECT3DDEVICEQ pd3dDevice, DxLandEff* pSrcEff, DxAnimationMan* pAniHead )
{
	CleanUp();

	m_strEffName = pSrcEff->m_strEffName.c_str();
	m_strFrameName = pSrcEff->m_strFrameName.c_str();

	m_matLocal = pSrcEff->m_matLocal;
	m_matLast = pSrcEff->m_matLast;
	m_bGroudPos = pSrcEff->m_bGroudPos;

	// Note : Frame* 찾기
	DxFrame *pRoot, *pFrame;
	DxAnimationMan* pCur = pAniHead;
	while( pCur )
	{
		pRoot = pCur->GetDxFrameRoot();
		if( pRoot )
		{
			pFrame = pRoot->FindFrame( m_strFrameName.c_str() );
			if( pFrame )
			{
				m_pFrame = pFrame;
				goto _RETURN;
			}
		}
		pCur = pCur->m_pNext;
	}

_RETURN:
	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_strEffName.c_str() );

	if ( m_pPropGroup )
	{	
		SAFE_DELETE(m_pSingleGroup);
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP();
		if( !m_pSingleGroup )	return;

		// Note : Eff Group Create Dev.
		HRESULT hr = m_pSingleGroup->Create( pd3dDevice );

		D3DXMATRIX matWorld;
		if( m_pFrame )
		{
			D3DXMatrixMultiply( &matWorld, &m_matLocal, &m_pFrame->matCombined );	
		}
		else
		{
			matWorld = m_matLocal;
		}
		// Note : 초기 위치 설정
		m_pSingleGroup->m_matWorld = matWorld;
	}
}

void DxLandEff::Save( CSerialFile& SFile )
{
	SFile << m_strEditName;
	SFile << m_strEffName;
	SFile << m_strFrameName;
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	SFile.WriteBuffer( m_matLast, sizeof(D3DXMATRIX) );
	
	// Note : 새로 추가 된 것
	SFile << m_bGroudPos;
}

void DxLandEff::LoadEdit( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh )
{
	SFile >> m_strEditName;
	SFile >> m_strEffName;
	SFile >> m_strFrameName;
	SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	SFile.ReadBuffer( m_matLast, sizeof(D3DXMATRIX) );

	// Note : 새로 추가 된 것
	SFile >> m_bGroudPos;

	// Note : Frame 을 맞춘다.
	m_pFrame = pFrameMesh->GetAniFrame( m_strFrameName.c_str() );

	// Note : 효과 생성
	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_strEffName.c_str() );

	if( m_pPropGroup )
	{	
		SAFE_DELETE(m_pSingleGroup);
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP();
		if( !m_pSingleGroup )	return;

		// Note : Eff Group Create Dev.
		m_pSingleGroup->Create( pd3dDevice );
	}
}

void DxLandEff::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAnimation )
{
	SFile >> m_strEditName;
	SFile >> m_strEffName;
	SFile >> m_strFrameName;
	SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	SFile.ReadBuffer( m_matLast, sizeof(D3DXMATRIX) );

	// Note : 새로 추가 된 것
	SFile >> m_bGroudPos;

	// Note : Frame 을 맞춘다.
	m_pFrame = pAnimation->FindFrame( m_strFrameName.c_str() );

	// Note : 효과 생성
	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_strEffName.c_str() );

	if( m_pPropGroup )
	{	
		SAFE_DELETE(m_pSingleGroup);
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP();
		if( !m_pSingleGroup )	return;

		// Note : Eff Group Create Dev.
		m_pSingleGroup->Create( pd3dDevice );
	}
}

void DxLandEff::LoadEdit100( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh )
{
	SFile >> m_strEditName;
	SFile >> m_strEffName;
	SFile >> m_strFrameName;
	SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	SFile.ReadBuffer( m_matLast, sizeof(D3DXMATRIX) );

	// Note : Frame 을 맞춘다.
	m_pFrame = pFrameMesh->GetAniFrame( m_strFrameName.c_str() );

	// Note : 효과 생성
	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_strEffName.c_str() );

	if( m_pPropGroup )
	{	
		SAFE_DELETE(m_pSingleGroup);
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP();
		if( !m_pSingleGroup )	return;

		// Note : Eff Group Create Dev.
		m_pSingleGroup->Create( pd3dDevice );
	}
}

void DxLandEff::Load100( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAnimation )
{
	SFile >> m_strEditName;
	SFile >> m_strEffName;
	SFile >> m_strFrameName;
	SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	SFile.ReadBuffer( m_matLast, sizeof(D3DXMATRIX) );

	// Note : Frame 을 맞춘다.
	m_pFrame = pAnimation->FindFrame( m_strFrameName.c_str() );

	// Note : 효과 생성
	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_strEffName.c_str() );

	if( m_pPropGroup )
	{	
		SAFE_DELETE(m_pSingleGroup);
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP();
		if( !m_pSingleGroup )	return;

		// Note : Eff Group Create Dev.
		m_pSingleGroup->Create( pd3dDevice );
	}
}

void DxLandEff::Clone( LPDIRECT3DDEVICEQ pd3dDevice, DxLandEff* pSrcEff )
{
	// Note : Data Clear
	CleanUp();

	// Note : Clone
	m_strEffName = pSrcEff->m_strEffName;
	m_strFrameName = pSrcEff->m_strFrameName;	// Frame은 필요 없는거 같기도 하다.
	m_pFrame = pSrcEff->m_pFrame;
	m_matLocal = pSrcEff->m_matLocal;
	m_bGroudPos = pSrcEff->m_bGroudPos;

	// Note : 효과 생성
	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_strEffName.c_str() );

	if( m_pPropGroup )
	{	
		SAFE_DELETE(m_pSingleGroup);
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP();
		if( !m_pSingleGroup )	return;

		// Note : Eff Group Create Dev.
		m_pSingleGroup->Create( pd3dDevice );
	}
}


//----------------------------------------------------------------------------------------------------------------------
//									D	X		R	E	P	L	A	C	E		E	F	F
//----------------------------------------------------------------------------------------------------------------------
const DWORD DxPieceEff::VERSION = 0x0102;

DxPieceEff::DxPieceEff() :
	m_dwEffectCount(0L),
	m_pEffectHead(NULL)
{
}

DxPieceEff::~DxPieceEff()
{
	CleanUp();
}

void DxPieceEff::CleanUp()
{
	m_dwEffectCount = 0L;
	SAFE_DELETE(m_pEffectHead);

	std::for_each ( m_vecLandEff.begin(), m_vecLandEff.end(), std_afunc::DeleteObject() );
	m_vecLandEff.clear();
}

void DxPieceEff::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{}

void DxPieceEff::OnDestroyDevice()
{
	CleanUp();
}

void DxPieceEff::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		(*iter)->OnResetDevice( pd3dDevice );
	}

	PDXEFFECTBASE pCur = m_pEffectHead;
	while( pCur )
	{
		pCur->RestoreDeviceObjects( pd3dDevice );
		pCur = pCur->pEffectNext;
	}
}

void DxPieceEff::OnLostDevice()
{
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		(*iter)->OnLostDevice();
	}

	PDXEFFECTBASE pCur = m_pEffectHead;
	while( pCur )
	{
		pCur->InvalidateDeviceObjects();
		pCur = pCur->pEffectNext;
	}
}

void DxPieceEff::FrameMove( const float fTime, const float fElapsedTime )
{
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		(*iter)->FrameMove( fTime, fElapsedTime );
	}

	PDXEFFECTBASE pCur = m_pEffectHead;
	while( pCur )
	{
		pCur->FrameMove( fTime, fElapsedTime );
		pCur = pCur->pEffectNext;
	}
}

void DxPieceEff::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh )
{
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		(*iter)->Render( pd3dDevice, matWorld, pStaticMesh, pFrameMesh );
	}

	PDXEFFECTBASE pCur = m_pEffectHead;
	while( pCur )
	{
		(*pCur->m_pmatLocal) = matWorld;	// Note : World Matrix 음.. 어렵네.

		pCur->Render( NULL, pd3dDevice );
		pCur = pCur->pEffectNext;
	}
}

void DxPieceEff::SetMatrix()
{
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		(*iter)->SetMatrix();
	}
}

void DxPieceEff::SetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		(*iter)->SetAABBBox( vMax, vMin );
	}
}

void DxPieceEff::ModifyMatrix()
{
	// Note : 매트릭스의 수정
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		(*iter)->ModifyMatrix();
	}
}

void DxPieceEff::ModifyEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szEditName, const char* szEffName )
{
	// Note : Eff 파일의 수정
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		if( !strcmp( (*iter)->m_strEditName.c_str(), szEditName ) )
		{
			CreateEditEffName( szEffName, (*iter)->m_strEditName );	// Note : EditName을 변화시킨다.
			(*iter)->ModifyEff( pd3dDevice, szEffName );			// Note : 효과를 수정한다.

			return;
		}
	}
}

BOOL DxPieceEff::SetEff( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame, const D3DXVECTOR3& vPos, 
						const char* szFrameName, const char* szEffName, std::string& strEditName )
{
	CreateEditEffName( szEffName, strEditName );

	DxLandEff* pEff = new DxLandEff;
	if( pEff->CreateEff( pd3dDevice, pFrame, vPos, szFrameName, szEffName, strEditName.c_str() ) )
	{
		m_vecLandEff.push_back( pEff );
		return TRUE;
	}
	else
	{
		SAFE_DELETE( pEff );
		return FALSE;
	}
}

void DxPieceEff::DelEff( const char* szEffName )
{
	DxLandEff* pDel;
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		if( !strcmp( (*iter)->m_strEditName.c_str(), szEffName ) )
		{
			pDel = (*iter);
			SAFE_DELETE( pDel );
			m_vecLandEff.erase( iter );
			
			return;
		}
	}
}

LPD3DXMATRIX DxPieceEff::GetEffMatrix( const char* szEffName )
{
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		if( !strcmp( (*iter)->m_strEditName.c_str(), szEffName ) )
		{
			return (*iter)->GetEffMatrix();
		}
	}

	return NULL;
}

void DxPieceEff::SetCheckCollPoint( const char* szEffName, BOOL bUse )
{ 
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		if( !strcmp( (*iter)->m_strEditName.c_str(), szEffName ) )
		{
			(*iter)->SetCheckCollPoint( bUse );
			return;
		}
	}
}

BOOL DxPieceEff::GetCheckCollPoint( const char* szEffName )		
{ 
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		if( !strcmp( (*iter)->m_strEditName.c_str(), szEffName ) )
		{
			return (*iter)->GetCheckCollPoint();
		}
	}
	return FALSE;
}

const char* DxPieceEff::GetEffName( const char* pEditName )
{
	VECLANDEFF_ITER iter = m_vecLandEff.begin();
	for( ; iter!=m_vecLandEff.end(); ++iter )
	{
		if( !strcmp( (*iter)->m_strEditName.c_str(), pEditName ) )
		{
			return (*iter)->m_strEffName.c_str();
		}
	}
	return NULL;
}

void DxPieceEff::CreateEditEffName( const char* pEffName, std::string& strEditName )
{
	int nCount = 0;
	while(1)
	{
		// Note : 이름을 짓는다.
		std::string strNewName = pEffName;
		char c100 = (char) (nCount / 100);
		char c10 = (char) ((nCount - c100*100) / 10);
		char c1 = (char) (nCount%10);
		c100 = (c100+48);
		c10 = (c10+48);
		c1 = (c1+48);
		strNewName += "[";
		strNewName += c100;
		strNewName += c10;
		strNewName += c1;
		strNewName += "]";

		VECLANDEFF_ITER iter = m_vecLandEff.begin();
		for( ; iter!=m_vecLandEff.end(); ++iter )
		{
			// Note : 같은게 있다면
			if( !strcmp( (*iter)->m_strEditName.c_str(), strNewName.c_str() ) )
			{
				goto _RETURN;
			}
		}

		// 이름 같은 것이 없다. < 성공 >
		strEditName = strNewName.c_str();
		return;
		
_RETURN:
		++nCount;
	}
}

void DxPieceEff::SaveEdt( CSerialFile& SFile, DxFrameMesh* pFrameMesh )
{
	SFile << VERSION;

	SFile.BeginBlock( EMBLOCK_00 );
	{
		// Note : Single Eff
		DWORD dwSize = (DWORD)m_vecLandEff.size();
		SFile << dwSize;

		VECLANDEFF_ITER iter = m_vecLandEff.begin();
		for( ; iter!=m_vecLandEff.end(); ++iter )
		{
			(*iter)->Save( SFile );
		}
	}
	SFile.EndBlock( EMBLOCK_00 );
}

void DxPieceEff::LoadEdt( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh )
{
	// Note : Clear
	CleanUp();

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( VERSION==dwVer )
	{
		// Note : Single Eff
		DWORD dwCount;
		SFile >> dwCount;

		for( DWORD i=0; i<dwCount; ++i )
		{
			DxLandEff* pEff = new DxLandEff;
			pEff->LoadEdit( pd3dDevice, SFile, pFrameMesh );
			m_vecLandEff.push_back( pEff );
		}
	}
	else if( 0x0101==dwVer )
	{
		// Note : Single Eff
		DWORD dwCount;
		SFile >> dwCount;

		for( DWORD i=0; i<dwCount; ++i )
		{
			DxLandEff* pEff = new DxLandEff;
			pEff->LoadEdit100( pd3dDevice, SFile, pFrameMesh );
			m_vecLandEff.push_back( pEff );
		}
	}
	else if( 0x0100==dwVer )
	{
		// Note : Replace Effect
		BOOL	bResult;
		SFile >> bResult;

		// Note : Single Eff
		DWORD dwCount;
		SFile >> dwCount;

		for( DWORD i=0; i<dwCount; ++i )
		{
			DxLandEff* pEff = new DxLandEff;
			pEff->LoadEdit( pd3dDevice, SFile, pFrameMesh );
			m_vecLandEff.push_back( pEff );
		}
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );
	}
}

void DxPieceEff::Save( CSerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock();
	{
		// Note : Replace Effect
		{
			DxEffectBase* pEffCur;
			DWORD dwEffCount = 0;
			
			pEffCur = m_pEffectHead;
			while ( pEffCur )
			{
				dwEffCount++;
				pEffCur = pEffCur->pEffectNext;
			}

			SFile << dwEffCount;

			pEffCur = m_pEffectHead;
			while ( pEffCur )
			{
				pEffCur->SaveFile( SFile );
				pEffCur = pEffCur->pEffectNext;
			}
		}

		// Note : Single Eff
		DWORD dwSize = (DWORD)m_vecLandEff.size();
		SFile << dwSize;

		VECLANDEFF_ITER iter = m_vecLandEff.begin();
		for( ; iter!=m_vecLandEff.end(); ++iter )
		{
			(*iter)->Save( SFile );
		}
	}
	SFile.EndBlock();
}

void DxPieceEff::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAnimation )
{
	HRESULT hr = S_OK;

	// Note : Clear
	CleanUp();

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( VERSION==dwVer )
	{
		// Note : Replace Effect
		{
			DWORD dwMaxCount = 0;
			SFile >> dwMaxCount;
			for ( DWORD i=0; i<dwMaxCount; i++ )
			{
				hr = EffectLoadToList( pd3dDevice, SFile, m_pEffectHead );
				if ( FAILED(hr) )
					MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

				m_dwEffectCount++;
			}
		}

		// Note : Single Eff
		DWORD dwCount;
		SFile >> dwCount;

		for( DWORD i=0; i<dwCount; ++i )
		{
			DxLandEff* pEff = new DxLandEff;
			pEff->Load( pd3dDevice, SFile, pAnimation );
			m_vecLandEff.push_back( pEff );
		}
	}
	else if( (0x0101==dwVer) || (0x0100==dwVer) )
	{
		// Note : Replace Effect
		{
			DWORD dwMaxCount = 0;
			SFile >> dwMaxCount;
			for ( DWORD i=0; i<dwMaxCount; i++ )
			{
				hr = EffectLoadToList( pd3dDevice, SFile, m_pEffectHead );
				if ( FAILED(hr) )
					MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

				m_dwEffectCount++;
			}
		}

		// Note : Single Eff
		DWORD dwCount;
		SFile >> dwCount;

		for( DWORD i=0; i<dwCount; ++i )
		{
			DxLandEff* pEff = new DxLandEff;
			pEff->Load100( pd3dDevice, SFile, pAnimation );
			m_vecLandEff.push_back( pEff );
		}
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );
	}
}

void DxPieceEff::Export( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceEff* pSrcEff, DxFrameMesh* pFrameMesh, DxAnimationMan* pAniHead )
{
	// Note : Clear
	CleanUp();

	// Note : Replace ^^
	DxEffectBase* pEffCur = pFrameMesh->GetEffectList();
	while ( pEffCur )
	{
		if( pEffCur->GetEachRender() )
		{
			DWORD TypeID = pEffCur->GetTypeID();
			char* szFrameName = pEffCur->GetAdaptFrameName();

			PBYTE pProperty;
			DWORD dwSize;
			DWORD dwVer;
			pEffCur->GetProperty ( pProperty, dwSize, dwVer );
			LPDXAFFINEPARTS pAffineParts = pEffCur->m_pAffineParts;

			DxFrame* pFrame = pFrameMesh->GetFrameRoot();
			EffectAdaptToList( pFrame, TypeID, szFrameName, pd3dDevice, pProperty, dwSize, dwVer, pAffineParts, pEffCur );
		}

		pEffCur = pEffCur->pEffectNext;
	}

	// Note : Single Eff
	VECLANDEFF_ITER iter = pSrcEff->m_vecLandEff.begin();
	for( ; iter!=pSrcEff->m_vecLandEff.end(); ++iter )
	{
		// Note : DxLandEff - Export
		DxLandEff* pEff = new DxLandEff;
		pEff->Export( pd3dDevice, (*iter), pAniHead );

		// Note : 기본적으로 넣어줌.
		m_vecLandEff.push_back( pEff );
	}
}

HRESULT DxPieceEff::EffectAdaptToList( DxFrame *pFrameRoot, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
									PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc )
{
	HRESULT hr;

	DxFrame *pDxFrame = pFrameRoot->FindFrame( szFrame );

	DxEffectBase *pEffect = DxPieceEffMan::GetInstance().CreateEffInstance( TypeID );
	if ( !pEffect ) return NULL;

	hr = pEffect->AdaptToEffList( pDxFrame, pd3dDevice );

	if ( hr == S_FALSE )
	{
		char szMsg[256] = "";
		StringCchPrintf( szMsg, 256, "단독 효과(%s)의 Adapt 메소드가 존재하지 않습니다.", pEffect->GetName() );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
		return E_FAIL;
	}

	if ( FAILED(hr) )
	{
		char szMsg[256] = "";
		StringCchPrintf( szMsg, 256, "단독 효과(%s)의 Adapt 도중 오류를 발생하였습니다.", pEffect->GetName() );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
		return E_FAIL;
	}

	//	Note : Property, Affine  값을 대입.
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )		pEffect->SetAffineValue ( pAffineParts );

	//	Note : 대량의 버퍼를 복제하기 위한 메소드.
	if ( pEffectSrc )	pEffect->CloneData ( pEffectSrc, pd3dDevice );

	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);
		return E_FAIL;
	}

	m_dwEffectCount++;
	pEffect->pEffectNext = m_pEffectHead;
	m_pEffectHead = pEffect;

	return S_OK;
}

HRESULT DxPieceEff::EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, PDXEFFECTBASE &pEffList )
{
	BOOL	bResult = FALSE;
	HRESULT	hr = E_FAIL;
	char	*szName = NULL;
	int		StrLength;

	SFile >> bResult;
	if ( bResult )
	{
		SFile >> StrLength;
		szName = new char [ StrLength ];
		SFile.ReadBuffer ( szName, StrLength );
	}

    DWORD	TypeID;
	SFile >> TypeID;
	
	DWORD	dwSize = 0;
	BYTE	*pProperty = NULL;
	DWORD	dwVer = 0;

	SFile >> dwVer;
	SFile >> dwSize;
	if ( dwSize )
	{
		pProperty = new BYTE [ dwSize ];
		SFile.ReadBuffer ( pProperty, sizeof ( BYTE ) * dwSize );
	}

	LPDXAFFINEPARTS	pAffineParts = NULL;
	SFile >> bResult;
	if ( bResult )
	{
		pAffineParts = new DXAFFINEPARTS;
		SFile.ReadBuffer ( pAffineParts, sizeof ( DXAFFINEPARTS ) );
	}

	DxEffectBase *pEffect = DxPieceEffMan::GetInstance().CreateEffInstance ( TypeID );
	if ( !pEffect ) return E_FAIL;

	//	Note : Property, Affine  값을 대입.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

	//	Note : LoadBuffer(), 특정 Effect 에서만 사용되는 메모리 버퍼를 읽는다.
	//
	pEffect->LoadBuffer ( SFile, dwVer, pd3dDevice );

	//	Note : Create Device.
	//
	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);
		return E_FAIL;
	}

    SAFE_DELETE_ARRAY ( pProperty );
	SAFE_DELETE ( pAffineParts );
	SAFE_DELETE_ARRAY ( szName );

	//	Note : Insert To List.
	//
	pEffect->pEffectNext = pEffList;
	pEffList = pEffect;

	// Note : 값 뒤집기	< List 순서를 유지하기 위해서 >
	DxEffectBase* pTemp = NULL;
	DxEffectBase* pCur	= pEffList;
	DxEffectBase* pThis = pEffList;
	while ( pCur )
	{
		pThis = pCur;
		pCur = pCur->pEffectNext;

		pThis->pEffectNext = pTemp;
		pTemp = pThis;
	}
	pEffList = pTemp;

	return S_OK;
}

void DxPieceEff::Clone( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceEff* pSrcEff )
{
	// Note : Data Clear
	CleanUp();

	// Note : FrameEff 복제
	m_dwEffectCount = pSrcEff->m_dwEffectCount;

	DxEffectBase* pSrc = pSrcEff->m_pEffectHead;
	for( DWORD i=0; i<m_dwEffectCount; ++i )
	{
		DxEffectBase *pDest = DxPieceEffMan::GetInstance().CreateEffInstance ( pSrc->GetTypeID() );

		pDest->CloneData( pSrc, pd3dDevice );

		pSrc = pSrc->pEffectNext;
		pDest->pEffectNext = m_pEffectHead;
		m_pEffectHead = pDest;
	}

	// Note : LandEff 복제
	VECLANDEFF_ITER iter = pSrcEff->m_vecLandEff.begin();
	for( ; iter!=pSrcEff->m_vecLandEff.end(); ++iter )
	{
		DxLandEff* pNew = new DxLandEff;
		pNew->Clone( pd3dDevice, (*iter) );
		m_vecLandEff.push_back( pNew );
	}
}
