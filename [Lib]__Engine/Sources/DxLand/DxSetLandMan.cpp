#include "pch.h"
#include "./DxSetLandMan.h"
#include "./DxLandSkinObj.h"

#include "./SerialFile.h"
#include "./DxLightMan.h"
#include "./DxViewPort.h"
#include "./DxWeatherMan.h"
#include "./TextureSetDXT.h"
#include "../[Lib]__EngineSound/Sources/DxSound/BgmSound.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL	g_bLANDEFF_DSPNAME = FALSE;

////////////////////////////////////////////////////////////////////////////////////
//	$30 2006-02-24 T14:45	[0x31로 버젼 올림. m_PieceManager의 로드방식 때문에.]
//	$30 2005-06-11 T11:10	[FrameMesh에 TextureEff 추가 하면서 저장방식 바뀜]
//	$29 2005-05-12 T11:10	[m_PieceManager 추가]
//	$28 2005-03-24 T13:40	[TextureSetDXT data 추가]
//	$27 2004-03-03 T15:00	[SKY data 추가]
//	$26 2003-12-04 T15:00	[FOG data 추가]
//	$25	2003-04-21 T15:12	[LightMan 수정 및 버전관리 ( 밤낮 변화 추가 ), 옷효과 수정, 그림자 수정. ]
//	$24	2003-04-18 T18:18	[Export 오류 수정 및 치환 오브젝트 버그 수정.]
//	$23	2003-04-18 T16:18	[물효과 수정.]
//	$22	2003-04-17 T21:40	[치환 조각 관련 버그 수정.] 
//	$21	2003-04-08 T15:06	[이것저것 다 수정.]
//	$20	2003-03-25 T20:20	[오류 보고 메세지 추가, 기타기능 수정됨.]
//	$19	2003-03-05 T13:20	[배경음악 클래스 추가됨]
//	$18	2003-03-03 T18:02	[알 수 없음.]
//	$17	2003-02-19 T16:06	[버퍼가 넘었다고 GASSERT문 넣은 것이 동작했음]
//	$16	2003-02-19 T15:20	[알 수 없음,승재선배님 요청후.. 일어나는 버전업임]
//	$15	2003-02-05 T11:34	[네비게이션 메쉬, 찾아가는 길 보여주기]
//	$14	2003-02-03 T11:46	[WorldSoundPage 수정, 버퍼를 0로 만드는 버그 수정]
//	$13	2003-01-29 T22:41	[Land Gate 오류 수정]
//							[RenderState 로드 수정]
//	$12	2003-01-29 T16:26	[새 문서 기능 오류 수정]
//	$11	2003-01-29 T14:33	[치환 오브젝트 효과 위치 수정]
//	$10	2003-01-29 T10:23	[학교 부분 psf파일 로딩 오류 수정]
//	$9	2003-01-22 T11:15	[네비게이션 메쉬, 데이타 무결성 체크 오류 수정
//							디렉토리 로드 경로가 틀렸었음]
//							[구조조정]
//	$8	2003-01-16 T14:10	[짜잘한 버그들 수정]
//	$7	2003-01-10 T00:23	[맵 에디터 사운드 추가시 오류상황 수정(ReleaseWav가 일어
//							나서 다른 곳에서 사용하는 버퍼를 삭제함)]
//	$6.	2003-01-09 T21:04	[치환오브젝트 버전 업/LandGate 기능 추가/사운드 작업
//							인터페이스 수정"락/언락"]
//	$5.	2003-01-04 T11:39	[잔디/홀씨 수정]
//	$4.	2002-12-10 T19:15	[합산 배포]
//	$3.	2002-12-04 T16:31	[맵 사운드 모듈 수정, 버퍼 볼륨추가 및 구조 변경]
//	$2.	2002-11-30 T12:01	[캐릭터 포지션 강제 이동 수정]
//	$1.	2002-11-29 Txx:xx	[최초 설정]
////////////////////////////////////////////////////////////////////////////////////

//	Note : DxSetLandMan, 버전
//
const DWORD DxSetLandMan::VERSION = 0x00000031;

DxSetLandMan::DxSetLandMan (void) :
	m_dwNumLandEff(0),
	m_pLandEffList(NULL),
	m_pLandEffTree(NULL),
	m_dwNumLandSkinObj(NULL),
	m_pLandSkinObjList(NULL),
	m_pLandSkinObjTree(NULL),
	m_pNaviFrame(NULL)
{
	memset( m_szMapName, 0, sizeof(char)*MAXLANDNAME );

	//	Note : DxSetLandMan는 편집용으로 Instance가 단일하게 존재한다는 가정하에
	//			초기화시에 LightMan 의 Active Instance를 지정한다.
	//
	DxLightMan::SetInstance ( &m_LightMan );

	DxWeatherMan::SetInstance ( &m_WeatherMan );

	//	Note : 초기화 이외에 다른 처리가 필요하지 않아,
	//		   생성자에 삽입함.
	m_CollisionMap.SetDxFrameMesh ( &m_FrameMesh );
}

DxSetLandMan::~DxSetLandMan (void)
{

}

HRESULT DxSetLandMan::ActiveMap ()
{
	DxLightMan::SetInstance ( &m_LightMan );

	DxWeatherMan::SetInstance ( &m_WeatherMan );

	DxFogMan::GetInstance().SetProperty ( m_FOG_PROPERTY );

	DxSkyMan::GetInstance().SetProperty ( m_SKY_PROPERTY );

	return S_OK;
}

HRESULT DxSetLandMan::OneTimeSceneInit ()
{
	HRESULT hr = S_OK;

	hr = m_FrameMesh.OneTimeSceneInit ();
	if ( FAILED(hr) )	return hr;	

	return S_OK;
}

HRESULT DxSetLandMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( m_FrameMesh.GetFileName()!=NULL )
	{
		hr = m_FrameMesh.InitDeviceObjects ( pd3dDevice );
		if ( FAILED(hr) )	return hr;

		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->InitDeviceObjects ( pd3dDevice );

			pCur = pCur->m_pNext;
		}
	}

	m_LightMan.InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxSetLandMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	hr = m_FrameMesh.RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->RestoreDeviceObjects ( pd3dDevice );

			pCur = pCur->m_pNext;
		}
	}
	
	{
		PLANDSKINOBJ pCur = m_pLandSkinObjList;
		while ( pCur )
		{
			pCur->RestoreDeviceObjects ( pd3dDevice );

			pCur = pCur->m_pNext;
		}
	}

	return S_OK;
}

HRESULT DxSetLandMan::InvalidateDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	hr = m_FrameMesh.InvalidateDeviceObjects( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->InvalidateDeviceObjects();

			pCur = pCur->m_pNext;
		}
	}

	{
		PLANDSKINOBJ pCur = m_pLandSkinObjList;
		while ( pCur )
		{
			pCur->InvalidateDeviceObjects();

			pCur = pCur->m_pNext;
		}
	}

	return S_OK;
}

HRESULT DxSetLandMan::DeleteDeviceObjects ()
{
	HRESULT hr = S_OK;

	hr = m_FrameMesh.DeleteDeviceObjects ();
	if ( FAILED(hr) )	return hr;

	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->DeleteDeviceObjects ();

		pCur = pCur->m_pNext;
	}

	m_LightMan.DeleteDeviceObjects ();

	CTextureSetDXT::GetInstance().DeleteDeviceObjects();

	return S_OK;
}

HRESULT DxSetLandMan::FinalCleanup ()
{
	HRESULT hr = S_OK;

	hr = m_FrameMesh.FinalCleanup();
	if ( FAILED(hr) )	return hr;

	m_pNaviFrame = NULL;
	m_NaviMesh.Clear ();

	hr = m_StaticSoundMan.FinalCleanup ();
	if ( FAILED(hr) )	return hr;

	m_dwNumLandEff = 0;
	SAFE_DELETE(m_pLandEffList);
	SAFE_DELETE(m_pLandEffTree);

	m_dwNumLandSkinObj = 0;
	SAFE_DELETE(m_pLandSkinObjList);
	SAFE_DELETE(m_pLandSkinObjTree);

	m_LightMan.CleanUp ();
	m_CameraAniMan.Cleanup ();
	m_LandGateMan.CleanUp ();
	m_CollisionMap.CleanUp ();

	m_PieceManager.CleanUp ();

	return S_OK;
}

void DxSetLandMan::SetFogProperty ( FOG_PROPERTY& Property )
{
	m_FOG_PROPERTY = Property;
	DxFogMan::GetInstance().SetProperty ( m_FOG_PROPERTY );
}

void DxSetLandMan::SetSkyProperty ( SKY_PROPERTY& Property )
{
	m_SKY_PROPERTY = Property;
	DxSkyMan::GetInstance().SetProperty ( m_SKY_PROPERTY );
}

HRESULT DxSetLandMan::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	hr = m_FrameMesh.FrameMove ( fTime, fElapsedTime );
	if ( FAILED(hr) )	return hr;

	//	Note : 카메라 방향에 따라 소리변화 반영
	//		   고정 사운드 클리핑
	//
	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	
	PROFILE_BEGIN("StaticSound");
	m_StaticSoundMan.FrameMove ( vLookatPt, fElapsedTime );
	PROFILE_END("StaticSound");

	DXLANDEFF::FrameMove ( fTime, fElapsedTime );
	DXLANDSKINOBJ::FrameMove ( fTime, fElapsedTime );

	m_PieceManager.FrameMove( fTime, fElapsedTime );

	m_CameraAniMan.FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT DxSetLandMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();
	hr = m_FrameMesh.Render ( pd3dDevice, NULL, &cv );
	if ( FAILED(hr) )	return hr;

	// Note : 조각 파일 렌더링.
	m_PieceManager.Render( pd3dDevice, cv );

	//	Note : 고정 사운드 영역 그리기
	m_StaticSoundMan.Render ( pd3dDevice );

	if ( m_pLandSkinObjTree )
	{
		COLLISION::dwRendAABB = 0;
		hr = COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandSkinObjTree, FALSE );
		if ( FAILED(hr) )	return hr;

		CDebugSet::ToView ( 8, "COLLISION::dwRendAABB,m_pLandSkinObjTree = %d", COLLISION::dwRendAABB );
	}
	else
	{
		PLANDSKINOBJ pCur = m_pLandSkinObjList;
		while ( pCur )
		{
			pCur->Render ( pd3dDevice, &cv );

			pCur = pCur->m_pNext;
		}
	}

	if ( DxLandGateMan::m_bREND )	m_LandGateMan.Render ( pd3dDevice );

	return S_OK;
}

HRESULT DxSetLandMan::Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();
	hr = m_FrameMesh.Render_EFF ( pd3dDevice, NULL, &cv );
	if ( FAILED(hr) )	return hr;

	// Note : 조각 파일 렌더링.
	m_PieceManager.RenderAlpha( pd3dDevice, NULL, &m_FrameMesh );

	if ( m_pLandEffTree )
	{
		//COLLISION::dwRendAABB = 0;
		hr = COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandEffTree, FALSE );
		if ( FAILED(hr) )	return hr;

		//CDebugSet::ToView ( 8, "COLLISION::dwRendAABB = %d", COLLISION::dwRendAABB );
	}
	else
	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->Render ( pd3dDevice, &cv );

			pCur = pCur->m_pNext;
		}
	}

	if( g_bLANDEFF_DSPNAME )
	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->RenderName( pd3dDevice );
			pCur = pCur->m_pNext;
		}
	}

	// Note : Camera Pick Object    AlphaBlending
	m_PieceManager.RenderPickAlpha( pd3dDevice, NULL, &m_FrameMesh );

	return S_OK;
}

void DxSetLandMan::SetNaviFrame ( DxFrame *pNaviMesh )
{
	GASSERT ( pNaviMesh && "네비게이션 메쉬 지정되지 않음" );

	if ( m_pNaviFrame ) m_pNaviFrame->bFlag &= ~DXFRM_NAVIFRAME;
	
	m_pNaviFrame = pNaviMesh;
	m_pNaviFrame->bFlag |= DXFRM_NAVIFRAME;
}

BOOL DxSetLandMan::CreateNaviMesh ( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;

	if ( m_pNaviFrame )
	{
		hr = m_NaviMesh.CreateNaviMesh ( m_pNaviFrame, pd3dDevice );
		if ( FAILED(hr) )	goto _ERROR;
		
		hr = m_NaviMesh.MakeAABBTree ();
		if ( FAILED(hr) )	goto _ERROR;
		
		m_NaviMesh.LinkCells ();
	}

	return TRUE;

_ERROR:
	m_NaviMesh.Clear();

	return FALSE;
}

void DxSetLandMan::BuildSingleEffTree ()
{
	PLANDEFF pCur = m_pLandEffList;
	while( pCur )
	{
		pCur->ReSetAABBBox();
		pCur = pCur->m_pNext;
	}
	COLLISION::MakeAABBTree ( m_pLandEffTree, m_pLandEffList );
}

PLANDEFF DxSetLandMan::FindLandEff ( char* szName )
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

PLANDEFF DxSetLandMan::FindLandEff( D3DXVECTOR3& vCenter, D3DXVECTOR3& vTarget )
{
	D3DXVECTOR3 vColl, vNor;
	POBJAABB pObj = NULL;
	COLLISION::CollisionLineToTree( m_pLandEffTree, vCenter, vTarget, pObj, vColl, vNor );

	PLANDEFF pEff = (PLANDEFF)pObj;
	return pEff;
}

void DxSetLandMan::AddLandEff ( PLANDEFF pLandEff )
{
	m_dwNumLandEff++;
	pLandEff->m_pNext = m_pLandEffList;
	m_pLandEffList = pLandEff;
}

void DxSetLandMan::DelLandEff ( PLANDEFF pLandEff )
{
	GASSERT(pLandEff);
	PLANDEFF pCur, pBack;

	if ( m_pLandEffList==pLandEff )
	{
		pCur = m_pLandEffList;
		m_pLandEffList = m_pLandEffList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandEff--;
		return;
	}

	pCur = m_pLandEffList;
	while ( pCur )
	{
		if ( pCur==pLandEff )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandEff--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

void DxSetLandMan::ReverseListLandEff()
{
	PLANDEFF pNewRoot = NULL;
	PLANDEFF pChange = NULL;
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pChange = pCur;
		pCur = pCur->m_pNext;

		pChange->m_pNext = pNewRoot;
		pNewRoot = pChange;
	}

	m_pLandEffList = pNewRoot;
}

void DxSetLandMan::Clone_MouseShift( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX& pMatrix )
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		DWORD dwBuffer = (DWORD)&pCur->m_matWorld;
		if( dwBuffer == (DWORD)pMatrix )
		{
			DXLANDEFF *pLandEff;
			pLandEff = new DXLANDEFF;

			char szNewName[DXLANDEFF::LANDEFF_MAXSZ] = "";
			
			int i=0;
			do
			{
				//sprintf( szNewName, "%s[%d]", pCur->m_szFileName, i++ );
				StringCchPrintf( szNewName, DXLANDEFF::LANDEFF_MAXSZ,"%s[%d]", pCur->m_szFileName, i++ );
			} while( FindLandEff(szNewName) );
			pLandEff->SetEffect( szNewName, pCur->m_matWorld, pCur->m_szFileName, pd3dDevice );

			pMatrix = &pLandEff->m_matWorld;	// DxObjectMRS 커서를 바꿔준다. < 중요 >
			AddLandEff( pLandEff );

			BuildSingleEffTree ();

			return;
		}
		pCur = pCur->m_pNext;
	}

	DxLightMan::GetInstance()->Clone_MouseShift( pMatrix );

	m_StaticSoundMan.Clone_MouseShift( pMatrix );
}

void DxSetLandMan::SetObjRotate90()
{
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*0.5f );

	// Note : 이펙트
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		D3DXMatrixMultiply( &pCur->m_matWorld, &pCur->m_matWorld, &matRotate );
		pCur = pCur->m_pNext;
	}
	BuildSingleEffTree();

	// Note : 스킨 오브젝브
	PLANDSKINOBJ pSkin = m_pLandSkinObjList;
	while( pSkin )
	{
		D3DXMatrixMultiply( &pSkin->m_matWorld, &pSkin->m_matWorld, &matRotate );
		pSkin = pSkin->m_pNext;
	}
	BuildSkinObjTree();

	// Note : 라이트
	DXLIGHT* pLight = DxLightMan::GetInstance()->GetLightHead();
	while( pLight )
	{
		D3DXMatrixMultiply( &pLight->m_matWorld, &pLight->m_matWorld, &matRotate );
		pLight = pLight->pNext;
	}
	DxLightMan::GetInstance()->MakeAABBTree();

	// Note : 사운드
	m_StaticSoundMan.SetObjRotate90();

	// Note : 게이트의 변환
	m_LandGateMan.SetObjRotate90();
}

void DxSetLandMan::AddLandSkinObj ( PLANDSKINOBJ pLandSkinObj )
{
	m_dwNumLandSkinObj++;
	pLandSkinObj->m_pNext = m_pLandSkinObjList;
	m_pLandSkinObjList = pLandSkinObj;
}

void DxSetLandMan::DelLandSkinObj ( PLANDSKINOBJ pLandSkinObj )
{
	GASSERT(pLandSkinObj);
	PLANDSKINOBJ pCur, pBack;

	if ( m_pLandSkinObjList==pLandSkinObj )
	{
		pCur = m_pLandSkinObjList;
		m_pLandSkinObjList = m_pLandSkinObjList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandSkinObj--;
		return;
	}

	pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		if ( pCur==pLandSkinObj )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandSkinObj--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

PLANDSKINOBJ DxSetLandMan::FindLandSkinObj ( char* szName )
{
	PLANDSKINOBJ pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

void DxSetLandMan::BuildSkinObjTree ()	
{
	COLLISION::MakeAABBTree ( m_pLandSkinObjTree, m_pLandSkinObjList );
}

void DxSetLandMan::LoadSet ( LPDIRECT3DDEVICEQ pd3dDevice, const char *pFileName )
{
	CDebugSet::ClearFile ( "LoadSet.log" );

	BOOL	bResult = FALSE;
	
	CSerialFile	SFile;
	BOOL bOpened = SFile.OpenFile ( FOT_READ, pFileName );
	if ( !bOpened )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. error File Open..." );
	}

	char szFileType[FILETYPESIZE] = "";
	DWORD dwFileVer;
	SFile.GetFileType( szFileType, FILETYPESIZE, dwFileVer );

	if ( dwFileVer > DxSetLandMan::VERSION )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. ERROR 맵 설정 파일(PSF) 이 현제 툴의 버전보다 상위의 버전입니다." );
		return;
	}

	//	Note : Light
	//
	CDebugSet::ToFile ( "LoadSet.log", "-. [LightMan Loading] - Start" );
	m_LightMan.Load ( SFile, dwFileVer, TRUE );
	CDebugSet::ToFile ( "LoadSet.log", "-. [LightMan Loading] - End" );

	//	Note : FrameMesh
	//
	CDebugSet::ToFile ( "LoadSet.log", "-. [FrameMesh Loading] - Start" );
	if ( dwFileVer >= 0x00000030 )
	{
		if ( !m_FrameMesh.LoadSet_VER100( SFile, pd3dDevice ) )
		{
			CDebugSet::ToFile ( "LoadSet.log", "-. 프레임 메쉬로드에 실패하였습니다." );
			return;
		}
	}
	else
	{
		if ( !m_FrameMesh.LoadSet( SFile, pd3dDevice ) )
		{
			CDebugSet::ToFile ( "LoadSet.log", "-. 프레임 메쉬로드에 실패하였습니다." );
			return;
		}
	}
	CDebugSet::ToFile ( "LoadSet.log", "-. [FrameMesh Loading] - End" );

	SFile >> bResult;
	if ( bResult )
	{
		int StrLength = 0;
		char* szNaviName = NULL;
		
		SFile >> StrLength;
		szNaviName = new char [ StrLength ];
		SFile.ReadBuffer ( szNaviName, StrLength );

		DxFrame *pNaviFrame = NULL;
        pNaviFrame = m_FrameMesh.FindFrame ( szNaviName );
		if ( pNaviFrame )	SetNaviFrame ( pNaviFrame );
		else
		{
			CDebugSet::ToFile ( "LoadSet.log", "   [%s]...Not Found Navigation Mesh..", szNaviName );
		}

		SAFE_DELETE_ARRAY ( szNaviName );
	}

	//	Note : ViewPort
	//
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [CameraAniMan Loading] - Start" );
		m_CameraAniMan.LoadSet ( SFile, &m_FrameMesh );
		CDebugSet::ToFile ( "LoadSet.log", "-. [CameraAniMan Loading] - End" );
	}

	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxSoundMan Loading] - Start" );
	
		//	Note : Sound Log
#ifdef	_DEBUG
		CDebugSet::ToFile ( "UseSoundList.txt", "" );
		CDebugSet::ToFile ( "UseSoundList.txt", "[-------------------------------------------------------------]" );
		CDebugSet::ToFile ( "UseSoundList.txt", "[!.%s]", pFileName );	
#endif	//	_DEBUG

		DxSoundMan::GetInstance().LoadSet ( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxSoundMan Loading] - End" );
	}

	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [StaticSoundMan Loading] - Start" );
		m_StaticSoundMan.LoadSet ( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [StaticSoundMan Loading] - End" );
	}

	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDEFF Loading] - Start" );

		bResult = TRUE;
		while ( bResult )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( SFile, pd3dDevice );
			AddLandEff ( pLandEff );
			
			SFile >> bResult;
		}
	
		ReverseListLandEff();
		BuildSingleEffTree ();

		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDEFF Loading] - End" );
	}

	//	Note : 맵 ID 읽기.
	//
	SFile >> bResult;
	if ( bResult )
	{
		SFile >> m_MapID.dwID;
	}

	SFile >> bResult;
	if ( bResult )
	{
		SFile.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );
	}

	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDSKINOBJ Loading] - Start" );

		bResult = TRUE;
		while ( bResult )
		{
			PLANDSKINOBJ pLandSkinObj = new DXLANDSKINOBJ;
			pLandSkinObj->Load ( SFile, pd3dDevice );
			AddLandSkinObj ( pLandSkinObj );
			
			SFile >> bResult;
		}
	
		BuildSkinObjTree ();

		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDSKINOBJ Loading] - End" );
	}

	SFile >> bResult;
	if ( bResult )
	{
		m_LandGateMan.Load ( SFile );
	}

	//	WeatherMan
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [WeatherMan Loading] - Start" );
		m_WeatherMan.LoadSet ( SFile, pd3dDevice );
		CDebugSet::ToFile ( "LoadSet.log", "-. [WeatherMan Loading] - End" );
	}

	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxBgmSound Loading] - Start" );
		m_BGMDATA.LoadSet ( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxBgmSound Loading] - End" );
	}

	//	DxFogMan
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [FOG_PROPERTY Loading] - Start" );
		m_FOG_PROPERTY.LoadSet ( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [FOG_PROPERTY Loading] - End" );
	}
	else return;

	//	Sky
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [SKY_PROPERTY Loading] - Start" );
		m_SKY_PROPERTY.LoadSet ( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [SKY_PROPERTY Loading] - End" );
	}
	else return;

	//	TextureSetDXT
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [TextureSetDXT Loading] - Start" );
		CTextureSetDXT::GetInstance().LoadSet( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [TextureSetDXT Loading] - End" );
	}
	else return;

	//	DxPieceManager
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxPieceManager Loading] - Start" );
		if ( 0x30 >= dwFileVer )
		{
			m_PieceManager.LoadPSF_100( pd3dDevice, SFile, &m_FrameMesh );	// 
		}
		else
		{
			m_PieceManager.LoadPSF( pd3dDevice, SFile, &m_FrameMesh );	// 0x31 되면서 바뀌었다.
		}
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxPieceManager Loading] - End" );
	}
	else return;
}

void DxSetLandMan::SaveSet ( LPDIRECT3DDEVICEQ pd3dDevice, const char *pFileName )
{
	CSerialFile	SFile;

	char szFileType[FILETYPESIZE] = "";
	SFile.SetFileType ( szFileType, DxSetLandMan::VERSION );

	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, pFileName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
	}

	//	Note : Light
	//
	m_LightMan.Save ( SFile );

	//	Note : FrameMesh
	//
	m_FrameMesh.SaveSet ( SFile );

	//	Note : 내비게이션 메시.
	//
	if ( m_pNaviFrame )		SFile << BOOL ( TRUE );
	else					SFile << BOOL ( FALSE );

	if( m_pNaviFrame && m_pNaviFrame->szName )
	{
		int	StrLength = (int)strlen(m_pNaviFrame->szName)+1;
		SFile << StrLength;
		SFile.WriteBuffer ( m_pNaviFrame->szName, StrLength );
	}

	//	Note : ViewPort
	//
	SFile << BOOL ( TRUE );
	m_CameraAniMan.SaveSet ( SFile );

	SFile << BOOL ( TRUE );
	DxSoundMan::GetInstance().SaveSet ( SFile );

	SFile << BOOL ( TRUE );
	m_StaticSoundMan.SaveSet ( SFile );

	if ( m_pLandEffList )	SFile << BOOL ( TRUE );
	else					SFile << BOOL ( FALSE );

	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->Save ( SFile );

		pCur = pCur->m_pNext;
		if ( pCur )		SFile << BOOL ( TRUE );
		else			SFile << BOOL ( FALSE );
	}

	//	Note : 맵 ID 저장.
	//
	SFile << BOOL ( TRUE );
	SFile << m_MapID.dwID;

	SFile << BOOL ( TRUE );
	SFile.WriteBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );


	//	Note : Land 에 삽입된 SkinObject
	//
	if ( m_pLandSkinObjList )	SFile << BOOL ( TRUE );
	else						SFile << BOOL ( FALSE );

	PLANDSKINOBJ pLandSkinObj = m_pLandSkinObjList;
	while ( pLandSkinObj )
	{
		pLandSkinObj->Save ( SFile );
			
		pLandSkinObj = pLandSkinObj->m_pNext;
		if ( pLandSkinObj )		SFile << BOOL ( TRUE );
		else					SFile << BOOL ( FALSE );
	}

	SFile << BOOL ( TRUE );
	m_LandGateMan.Save ( SFile );


	//	Note : Weather
	//
	SFile << BOOL ( TRUE );
	m_WeatherMan.SaveSet ( SFile );

	//	Note : BGM 파일 이름 [ 1개만 존재 ]
	SFile << BOOL ( TRUE );
	m_BGMDATA.SaveSet ( SFile );	

	//	Note : Fog
	SFile << BOOL ( TRUE );
	m_FOG_PROPERTY.SaveSet ( SFile );

	//	Note : Sky
	SFile << BOOL ( TRUE );
	m_SKY_PROPERTY.SaveSet ( SFile );

	//	Note : TextureSetDXT
	SFile << BOOL ( TRUE );
	CTextureSetDXT::GetInstance().SaveSet( SFile );

	//	Note : DxPieceManager
	SFile << BOOL ( TRUE );
	m_PieceManager.SavePSF( pd3dDevice, SFile, &m_FrameMesh );

	//	Note : 다음에 추가될 데이터를 위해서 마킹 ------
	//
	SFile << BOOL ( FALSE );
}

