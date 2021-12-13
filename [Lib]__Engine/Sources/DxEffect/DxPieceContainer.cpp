#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./DebugSet.h"
#include "./SerialFile.h"
#include "./SubPath.h"
#include "./DxMethods.h"

#include "./DxFrameMesh.h"
#include "./Collision.h"
#include "./EDITMESHS.h"
#include "./SerialFile.h"
#include "./DxLandMan.h"
#include "./DxRenderStates.h"
#include "./DxReplaceContainer.h"
#include "./DxStaticMesh.h"
#include "./DxPieceEff.h"
#include "./DxPieceStaticAni.h"
#include "./DxTextureEffMan.h"
#include "./DxPieceTexEff.h"
#include "./DxPieceEffMan.h"

#include "./DxPieceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxPieceContainer& DxPieceContainer::GetInstance()
{
	static DxPieceContainer Instance;
	return Instance;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//											D	x		P	i	e	c	e		E	d	i	t
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD DxPieceEdit::VERSION = 0x0102;

DxPieceEdit::DxPieceEdit() :
	m_pFrameMesh(NULL),
	m_pCollisionMap(NULL),
	m_pStaticAniFrame(NULL),
	m_pPieceEffEDIT(NULL),
	m_pPieceEff(NULL),
	m_pAniTexEff(NULL),
	m_nRefCount(0),
	m_fTime(0.f),
	m_fAniTime(0.f),
	m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
	m_vMin(FLT_MAX,FLT_MAX,FLT_MAX),
	m_vStaticMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
	m_vStaticMin(FLT_MAX,FLT_MAX,FLT_MAX),
	m_vCenter(0.f,0.f,0.f),
	m_fLength(1.f)
{
	m_strFileName = "";
	m_strFrameName = "";
	D3DXMatrixIdentity( &m_matLocal );
}

DxPieceEdit::~DxPieceEdit()
{
	CleanUp();
}

void DxPieceEdit::CleanUp()
{
	SAFE_DELETE( m_pFrameMesh );
	SAFE_DELETE( m_pCollisionMap );
	SAFE_DELETE( m_pStaticAniFrame );
	SAFE_DELETE( m_pPieceEffEDIT );
	SAFE_DELETE( m_pPieceEff );
	SAFE_DELETE( m_pAniTexEff );

	m_strFileName = "";
	m_strFrameName = "";
}

void DxPieceEdit::NewFrameMesh ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
{
	SAFE_DELETE ( m_pFrameMesh );
	m_pFrameMesh = new DxFrameMesh;

	// Note : 읽을 x파일과 버텍스 구조 정보 ( FVF ) 를 준다.
	m_strFrameName = szName;
	std::string strFullName = SUBPATH::APP_ROOT;
	strFullName += SUBPATH::OBJ_FILE_PIECEEDIT;
	strFullName += m_strFrameName.c_str();
	m_pFrameMesh->SetFile( strFullName.c_str() );
	m_pFrameMesh->SetFVF( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );

	// Note : 초기화 한다.
	m_pFrameMesh->Create ( pd3dDevice );

	m_pFrameMesh->UpdateFrames();

	// Note : MAX, MIN, CENTER, LENGTH 를 구한다.
	m_vStaticMax = m_vMax = m_pFrameMesh->GetTreeMax();
	m_vStaticMin = m_vMin = m_pFrameMesh->GetTreeMin();
	m_vCenter = (m_vMax+m_vMin)*0.5f;
	D3DXVECTOR3 vVector = m_vCenter - m_vMax;
	m_fLength = D3DXVec3Length( &vVector );

	// Note : DxAABBCollision 생성
	SAFE_DELETE( m_pCollisionMap );
	m_pCollisionMap = new CCollisionMap;
	m_pCollisionMap->SetDxFrameMesh( m_pFrameMesh );
	//m_pCollisionMap->Import( TRUE );

	// Note : DxPieceEff 생성
	SAFE_DELETE( m_pPieceEffEDIT );
	m_pPieceEffEDIT = new DxPieceEff;

	// Note : DxPieceEff 생성
	SAFE_DELETE( m_pPieceEff );
	m_pPieceEff = new DxPieceEff;

	// Note : DxPieceEff 생성
	SAFE_DELETE( m_pAniTexEff );
	m_pAniTexEff = new DxPieceTexEff;
	m_pAniTexEff->Import( pd3dDevice, m_pFrameMesh );

	// Note : DxStaticAniFrame 생성		< MakeAniMan() 함수는 기본 AniFrame을 생성하기 위해 호출한다. >
	SAFE_DELETE( m_pStaticAniFrame );
	m_pStaticAniFrame = new DxStaticAniFrame;
	m_pStaticAniFrame->MakeAniMan( pd3dDevice, m_pFrameMesh );
}

BOOL DxPieceEdit::IsCollision( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, BOOL bEditor, const DxFrame* pFrame, char* pName )
{
	BOOL bUse = FALSE;
	if( m_pCollisionMap )
	{
		bUse = m_pCollisionMap->IsCollision( vP1, vP2, bEditor, pFrame, pName, TRUE );
	}
	return bUse;
}

DxFrame* const DxPieceEdit::GetCollisionDetectedFrame()
{
	DxFrame*	pFrame = NULL;
	if( m_pCollisionMap )
	{
		pFrame = m_pCollisionMap->GetCollisionDetectedFrame();
	}
	return pFrame;
}

const D3DXVECTOR3 DxPieceEdit::GetCollisionPos ()
{
	D3DXVECTOR3	vPos(0.f,0.f,0.f);
	if( m_pCollisionMap )
	{
		vPos = m_pCollisionMap->GetCollisionPos();
	}
	return vPos;
}

DxFrame* DxPieceEdit::GetFrameRoot()
{ 
	if (m_pFrameMesh)	return m_pFrameMesh->GetFrameRoot(); 
	else				return NULL;
}

BOOL DxPieceEdit::SetEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFrameName, const D3DXVECTOR3& vPos, const char* szEffName, std::string& strEditName )
{
	if( !m_pFrameMesh )		return FALSE;
	if( !m_pPieceEffEDIT )	return FALSE;

	m_pFrameMesh->SetAnimationTimeZero();
	DxFrame* pFrame = m_pFrameMesh->GetAniFrame( szFrameName );
    if( m_pPieceEffEDIT->SetEff( pd3dDevice, pFrame, vPos, szFrameName, szEffName, strEditName ) )	
	{
		return TRUE;
	}

	return FALSE;
}

void DxPieceEdit::DelEff( const char* szEffName )
{
	if( !m_pPieceEffEDIT )		return;
	m_pPieceEffEDIT->DelEff( szEffName );
}

void DxPieceEdit::ModifyEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szEditName, const char* szEffName )
{
	if( !m_pPieceEffEDIT )		return;
	m_pPieceEffEDIT->ModifyEff( pd3dDevice, szEditName, szEffName );
}

LPD3DXMATRIX DxPieceEdit::GetEffMatrix( const char* szEditName )
{
	return m_pPieceEffEDIT->GetEffMatrix( szEditName );
}

void DxPieceEdit::SetCheckCollPoint( const char* szEffName, BOOL bUse )
{
	if( !m_pPieceEffEDIT )		return;
	m_pPieceEffEDIT->SetCheckCollPoint( szEffName, bUse );
}

BOOL DxPieceEdit::GetCheckCollPoint( const char* szEffName )
{
	if( !m_pPieceEffEDIT )		return FALSE;
	return m_pPieceEffEDIT->GetCheckCollPoint( szEffName );
}

const char* DxPieceEdit::GetEffName( const char* pEditName )
{
	if( !m_pPieceEffEDIT )		return NULL;
	return m_pPieceEffEDIT->GetEffName( pEditName );
}

void DxPieceEdit::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_pPieceEffEDIT )		m_pPieceEffEDIT->OnCreateDevice( pd3dDevice );
}

void DxPieceEdit::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_pStaticAniFrame )		m_pStaticAniFrame->OnResetDevice( pd3dDevice );
	if( m_pPieceEffEDIT )		m_pPieceEffEDIT->OnResetDevice( pd3dDevice );
}

void DxPieceEdit::OnLostDevice()
{
	if( m_pStaticAniFrame )		m_pStaticAniFrame->OnLostDevice();
	if( m_pPieceEffEDIT )		m_pPieceEffEDIT->OnLostDevice();
}

void DxPieceEdit::OnDestroyDevice()
{
	if( m_pPieceEffEDIT )		m_pPieceEffEDIT->OnDestroyDevice();
}

void DxPieceEdit::FrameMoveFrame( const float fTime, const float fElapsedTime )
{
	if( m_pStaticAniFrame )	m_pStaticAniFrame->FrameMove( fElapsedTime );
	if( m_pFrameMesh )	
	{
		m_pFrameMesh->SetAnimationTimeZero();
		m_pPieceEffEDIT->ModifyMatrix();

		m_pFrameMesh->FrameMove( fTime, fElapsedTime );
	}

	if( m_pPieceEffEDIT )	m_pPieceEffEDIT->FrameMove( fTime, fElapsedTime );
}

void DxPieceEdit::RenderFrame( const LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &sCV )
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	if( m_pFrameMesh )
	{
		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
		m_pFrameMesh->Render( pd3dDevice, &m_matLocal, &sCV );
	}

	if( m_pStaticAniFrame )
	{
		m_pStaticAniFrame->Render_EDIT( pd3dDevice, m_matLocal );
	}

	if( m_pPieceEffEDIT )
	{
		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

		m_pPieceEffEDIT->SetMatrix();	// Frame 모드에서의 Eff를 위한 Matrix 수정
		m_pPieceEffEDIT->Render( pd3dDevice, m_matLocal, NULL, NULL );

		m_pPieceEffEDIT->SetAABBBox( m_vMax, m_vMin );		// Setting Max Min
	}

	// Note : AABB 박스 테스트
	EDITMESHS::RENDERAABB( pd3dDevice, m_vMax, m_vMin );
}

void DxPieceEdit::FrameMove( const float fTime, const float fElapsedTime )
{
	m_fTime += fElapsedTime;
	m_fAniTime += fElapsedTime * UNITANIKEY_PERSEC;
	if( m_fTime > 1.0e15f )		m_fTime = 0.f;
	if( m_fAniTime > 1.0e15f )	m_fAniTime = 0.f;

	if( m_pStaticAniFrame )	m_pStaticAniFrame->FrameMove( fElapsedTime );
	if( m_pPieceEff )		m_pPieceEff->FrameMove( fTime, fElapsedTime );
	if( m_pAniTexEff )		m_pAniTexEff->FrameMove( fElapsedTime );
}

void DxPieceEdit::Render( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const D3DXMATRIX& matWorld )
{
	if( m_pStaticAniFrame )
	{
		m_pStaticAniFrame->Render( pd3dDevice, matWorld, m_fAniTime, m_pAniTexEff->GetMapTexEff() );
		m_pStaticAniFrame->Render_Alpha( pd3dDevice, matWorld, m_pAniTexEff->GetMapTexEff() );
	}
	if( m_pPieceEff )
	{
		m_pPieceEff->SetMatrix();
		m_pPieceEff->Render( pd3dDevice, matWorld, NULL, NULL );
	}
}

void DxPieceEdit::SavePEF( const char* szName )
{
	CSerialFile	SFile;

	BOOL bOpened = SFile.OpenFile( FOT_WRITE, szName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return;
	}

	SFile << VERSION;

	SFile.BeginBlock();
	{
		SFile << m_vMin;
		SFile << m_vMax;
		SFile.WriteBuffer( &m_matLocal, sizeof(D3DXMATRIX) );
		SFile << m_strFrameName;

		// Frame 효과를 저장한다.
		m_pFrameMesh->SaveFrameEff( SFile );

		// Note : TextureEffMan을 저장 시킨다.
		m_pFrameMesh->GetTextureEffMan()->SavePSF( SFile );
	}
	SFile.EndBlock();


	BOOL bExist;
	bExist = m_pPieceEffEDIT ? TRUE : FALSE;
	SFile << bExist;
	if( m_pPieceEffEDIT )	m_pPieceEffEDIT->SaveEdt( SFile, m_pFrameMesh );

	SFile.CloseFile();
}

HRESULT DxPieceEdit::LoadPEF( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
{
	CleanUp();		// 초기화

	if ( !strcmp(szName,"") )	return E_FAIL;
	
	CSerialFile	SFile;
	BOOL bOpened = SFile.OpenFile( FOT_READ, szName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return E_FAIL;
	}

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION || dwVer==0x0101 )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile.ReadBuffer( &m_matLocal, sizeof(D3DXMATRIX) );
		SFile >> m_strFrameName;

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str() );

		// Frame의 효과를 로드한다.
		LoadFrameEff( pd3dDevice, SFile, m_pFrameMesh );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF( pd3dDevice, SFile );

		SAFE_DELETE( m_pAniTexEff );
		m_pAniTexEff = new DxPieceTexEff;
		m_pAniTexEff->Import( pd3dDevice, m_pFrameMesh );
	}
	else if( dwVer==0x0100 )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile.ReadBuffer( &m_matLocal, sizeof(D3DXMATRIX) );
		SFile >> m_strFrameName;

		// EDIT 시에만 FrameMesh를 로드한다.
		NewFrameMesh( pd3dDevice, m_strFrameName.c_str() );

		// Note : TextureEffMan을 로드 시킨다.
		m_pFrameMesh->GetTextureEffMan()->LoadPSF( pd3dDevice, SFile );

		SAFE_DELETE( m_pAniTexEff );
		m_pAniTexEff = new DxPieceTexEff;
		m_pAniTexEff->Import( pd3dDevice, m_pFrameMesh );
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );
	}

	// Note : m_pPieceEffEDIT 내부에서 Version 체크가 일어난다.
	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		SAFE_DELETE( m_pPieceEffEDIT );
		m_pPieceEffEDIT = new DxPieceEff;
		m_pPieceEffEDIT->LoadEdt( pd3dDevice, SFile, m_pFrameMesh );
	}

	SFile.CloseFile();

	return S_OK;
}

void DxPieceEdit::LoadFrameEff( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh )
{
	// Note : Replace Effect
	BOOL	bResult;
	SFile >> bResult;
	while ( bResult )
	{
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
		BYTE	*pProp = NULL;

		DWORD	dwVer = 0;

		SFile >> dwVer;
		SFile >> dwSize;
		if ( dwSize )
		{
			pProp = new BYTE [ dwSize ];
			SFile.ReadBuffer ( pProp, sizeof ( BYTE ) * dwSize );
		}

		LPDXAFFINEPARTS	pAffineParts = NULL;
		SFile >> bResult;		
		if ( bResult )
		{
			pAffineParts = new DXAFFINEPARTS;
			SFile.ReadBuffer ( pAffineParts, sizeof ( DXAFFINEPARTS ) );
		}
		
		DxFrame	*pDxFrame=NULL;
		if ( szName ) pDxFrame = pFrameMesh->FindFrame( szName );

		if ( szName && pDxFrame )
		{
			DxEffectBase *pEffect = DxPieceEffMan::GetInstance().AdaptToDxFrame( TypeID, pFrameMesh, pDxFrame, pd3dDevice,
																				pProp, dwSize, dwVer, pAffineParts );

			if ( !pEffect )
			{
				DWORD NODESIZE;
				SFile >> NODESIZE;
				SFile.SetOffSet ( SFile.GetfTell() + NODESIZE );

				CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %d] 인스턴스 생성에 실패.", TypeID );
				CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %d] NunNamed Effect Buffer Skip..", TypeID );
			}
			else
			{
				CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %x] 인스턴스 생성.", TypeID );
			}

			//	Note : LoadBuffer(), 특정 Effect 에서만 사용되는 메모리 버퍼를 읽는다.
			pEffect->LoadBufferSet ( SFile, dwVer, pd3dDevice );

			pEffect->CleanUp ();
			pEffect->Create ( pd3dDevice );
		}
		else
		{
			DWORD NODESIZE;
			SFile >> NODESIZE;
			SFile.SetOffSet ( SFile.GetfTell() + NODESIZE );

			if ( szName )	CDebugSet::ToFile ( "LoadSet.log", "[EFF TYPEID %d] DxFrame for Adapt [%s]...Not Found", TypeID, szName );
			else			CDebugSet::ToFile ( "LoadSet.log", "   [EFF TYPEID %d] NunNamed Effect Buffer Skip..", TypeID );
		}

		SAFE_DELETE_ARRAY ( pProp );
		SAFE_DELETE ( pAffineParts );
		SAFE_DELETE_ARRAY ( szName );

		SFile >> bResult;
	}
}

void DxPieceEdit::SavePIE( const char* szName )
{
	CSerialFile	SFile;

	BOOL bOpened = SFile.OpenFile( FOT_WRITE, szName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
		return;
	}

	SFile << VERSION;

	SFile.BeginBlock();
	{
		SFile << m_vMin;
		SFile << m_vMax;
		SFile << m_vStaticMin;
		SFile << m_vStaticMax;
		SFile << m_vCenter;
		SFile << m_fLength;
		SFile.WriteBuffer( &m_matLocal, sizeof(D3DXMATRIX) );
	}
	SFile.EndBlock();

	BOOL bExist;
	bExist = m_pStaticAniFrame ? TRUE : FALSE;
	SFile << bExist;
	if( m_pStaticAniFrame )	m_pStaticAniFrame->Save( SFile );

	bExist = m_pPieceEff ? TRUE : FALSE;
	SFile << bExist;
	if( m_pPieceEff )	m_pPieceEff->Save( SFile );

	bExist = m_pAniTexEff ? TRUE : FALSE;
	SFile << bExist;
	if( m_pAniTexEff )	m_pAniTexEff->Save( SFile );
	
	// 더미
	SFile << (BOOL)FALSE;

	SFile.CloseFile();
}

HRESULT DxPieceEdit::LoadPIE( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
{
	CleanUp();		// 초기화

	if ( !strcmp(szName,"") )	return E_FAIL;
	
	CSerialFile	SFile;
	BOOL bOpened = SFile.OpenFile( FOT_READ, szName );
	if ( !bOpened )
	{
		CDebugSet::ToLogFile( "piece file load fail : %s", szName );
		return E_FAIL;
	}

	DWORD dwVer;
	DWORD dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile >> m_vStaticMin;
		SFile >> m_vStaticMax;
		SFile >> m_vCenter;
		SFile >> m_fLength;
		SFile.ReadBuffer( &m_matLocal, sizeof(D3DXMATRIX) );
	}
	else if( dwVer==0x0101 )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile.ReadBuffer( &m_matLocal, sizeof(D3DXMATRIX) );

		m_vStaticMin = m_vMin;
		m_vStaticMax = m_vMax;
		m_vCenter = (m_vMax+m_vMin)*0.5f;
		D3DXVECTOR3 vVector = m_vCenter - m_vMax;
		m_fLength = D3DXVec3Length( &vVector );
	}
	else if( dwVer==0x0100 )
	{
		SFile >> m_vMin;
		SFile >> m_vMax;
		SFile.ReadBuffer( &m_matLocal, sizeof(D3DXMATRIX) );

		m_vStaticMin = m_vMin;
		m_vStaticMax = m_vMax;
		m_vCenter = (m_vMax+m_vMin)*0.5f;
		D3DXVECTOR3 vVector = m_vCenter - m_vMax;
		m_fLength = D3DXVec3Length( &vVector );
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet( dwCur+dwBufferSize );
	}

	// Note : 내부에서 Version 체크가 일어난다.
	BOOL bExist;
	SFile >> bExist;
	if ( bExist )
	{
		SAFE_DELETE( m_pStaticAniFrame );
		m_pStaticAniFrame = new DxStaticAniFrame;
		m_pStaticAniFrame->Load( pd3dDevice, SFile );
	}

	SFile >> bExist;
	if ( bExist )
	{
		SAFE_DELETE( m_pPieceEff );
		m_pPieceEff = new DxPieceEff;
		m_pPieceEff->Load( pd3dDevice, SFile, m_pStaticAniFrame->GetAniManHead() );
	}

	SFile >> bExist;
	if ( bExist )
	{
		SAFE_DELETE( m_pAniTexEff );
		m_pAniTexEff = new DxPieceTexEff;
		m_pAniTexEff->Load( pd3dDevice, SFile );
	}

	// 더미
	SFile >> bExist;

	SFile.CloseFile();

	return S_OK;
}

void DxPieceEdit::Export( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_pFrameMesh )	return;

	if( m_pStaticAniFrame )
	{
		m_pStaticAniFrame->Export( pd3dDevice, m_pFrameMesh );
	}
	if( m_pPieceEffEDIT )
	{
		m_pPieceEff->Export( pd3dDevice, m_pPieceEffEDIT, m_pFrameMesh, m_pStaticAniFrame->GetAniManHead() );
	}
	if( m_pAniTexEff )
	{
		m_pAniTexEff->Import( pd3dDevice, m_pFrameMesh );
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//							D	x			S	t	a	t	i	c	M	e	s	h			C	o	n	t	a	i	n	e	r
// -----------------------------------------------------------------------------------------------------------------------------------------
char DxPieceContainer::m_szPath[] = "";

DxPieceContainer::DxPieceContainer()
{
}

DxPieceContainer::~DxPieceContainer()
{
	CleanUp ();
}

void DxPieceContainer::CleanUp ()
{
	std::for_each ( m_mapStaticPis.begin(), m_mapStaticPis.end(), std_afunc::DeleteMapObject() );
	m_mapStaticPis.clear ();
}

DxPieceEdit* DxPieceContainer::FindPiece ( const char *szFile )
{
	MAPSTATICPIS_ITER iter = m_mapStaticPis.find ( szFile );
	if ( iter!=m_mapStaticPis.end() )	return (*iter).second;

	return NULL;
}

DxPieceEdit* DxPieceContainer::LoadPiece( LPDIRECT3DDEVICEQ pd3dDevice, const char *szFile, const char *szDir )
{
	HRESULT hr=S_OK;

	DxPieceEdit* pPieceEdit;
	if ( pPieceEdit = FindPiece ( szFile ) )
	{
		pPieceEdit->m_nRefCount++;
		return pPieceEdit;
	}

	std::string szFullName;
	szFullName = m_szPath;
	szFullName += szFile;

	DxPieceEdit* pNewPieceEdit = new DxPieceEdit;
	hr = pNewPieceEdit->LoadPIE( pd3dDevice, szFullName.c_str() );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pNewPieceEdit);
		return NULL;
	}

	pNewPieceEdit->m_nRefCount++;

	//	Note : 리스트에 등록.
	//
	m_mapStaticPis.insert ( std::make_pair(szFile,pNewPieceEdit) );

	return pNewPieceEdit;
}

void DxPieceContainer::ReleasePiece ( const char *szFile )
{
	MAPSTATICPIS_ITER iter = m_mapStaticPis.find ( szFile );
	if ( iter==m_mapStaticPis.end() )	return;

	(*iter).second->m_nRefCount--;
	if ( (*iter).second->m_nRefCount <= 0 )
	{
		delete (*iter).second;
		m_mapStaticPis.erase ( iter );
	}

	return;
}
