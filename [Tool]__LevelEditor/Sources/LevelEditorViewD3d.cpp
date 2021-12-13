#include "pch.h"
#include "LevelEditor.h"
#include "LevelEditorDoc.h"
#include "LevelEditorView.h"
#include "MainFrm.h"
#include "SheetWithTab.h"

#include "d3dfont.h"

#include "EtcFuncPage.h"
#include "MobSetPage.h"

#include "EtcFunction.h"
#include "RANPARAM.h"

#include "profile.h"
#include "DxShadowMap.h"
#include "DxInputDevice.h"
#include "EDITMESHS.h"
#include "ShaderConstant.h"
#include "DxPostProcess.h"
#include "DXInputString.h"
#include "DxCustomTypes.h"
#include "Collision.h"
#include "ScanCode.h"
#include "GLDefine.h"
#include "DxResponseMan.h"
#include "GLOGIC.h"
#include "DxEditMat.h"
#include "DxEditBox.h"
#include "DxObjectMRS.h"
#include "DxInputDevice.h"
#include "DxLandMan.h"
#include "DxLandGateMan.h"
#include "DxFrameMesh.h"
#include "NavigationMesh.h"
#include "DxVertexFVF.h"
#include "DxObject.h"
#include "DxCustomTypes.h"
#include "DxEffectFrame.h"
#include "DxOctreeMesh.h"

#include "DxParamSet.h"
#include "DxServerInstance.h"
#include "GLGaeaServer.h"
#include "GLLandMan.h"
#include "GLCrowData.h"

#include "DxFontMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern BOOL				g_bFRAME_LIMIT;
DxLandMan				CLevelEditorView::m_LandMan;

HRESULT CLevelEditorView::FrameMove3DEnvironment()
{
	HRESULT hr=S_OK;

	if ( m_pd3dDevice )
	{
		hr = CD3DApplication::FrameMove3DEnvironment ();
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

HRESULT CLevelEditorView::Render3DEnvironment ()
{
	HRESULT hr=S_OK;

	if ( m_pd3dDevice )
	{
		hr = CD3DApplication::Render3DEnvironment ();
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

void CLevelEditorView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if ( m_bCreated )
	{
		ReSizeWindow(cx,cy);
	}
}

void CLevelEditorView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CLevelEditorApp* pApp = (CLevelEditorApp *)AfxGetApp();
	pApp->SetActive ( bActivate );

	CLevelEditorView::SetActive ( bActivate );

	__super::OnActivateView ( bActivate, pActivateView, pDeactiveView );
}

void CLevelEditorView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	CLevelMainFrame* pFrame = (CLevelMainFrame *)AfxGetApp()->m_pMainWnd;

	RANPARAM::LOAD ( pFrame->m_szAppPath );

	CD3DApplication::Create ( m_hWnd, m_hWnd, AfxGetInstanceHandle() );

	m_bCreated = TRUE;
}

void CLevelEditorView::PostNcDestroy ()
{
	CD3DApplication::Cleanup3DEnvironment();
	__super::PostNcDestroy();
}

HRESULT CLevelEditorView::ConfirmDevice ( D3DCAPSQ* pCaps, DWORD dwBehavior, D3DFORMAT Format )
{
	if ( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelEditorView::OneTimeSceneInit ()
{
	CLevelMainFrame* pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;

	char szFullPath[MAX_PATH] = {0};

	DxResponseMan::GetInstance().OneTimeSceneInit ( pFrame->m_szAppPath, this,
		RANPARAM::strFontType, RANPARAM::dwLangSet, RANPARAM::strGDIFont );

	GMTOOL::Create ( pFrame->m_szAppPath );

	m_pDxEditmat = new DxEditMat;

	strcpy_s ( szFullPath, pFrame->m_szAppPath );
	strcat_s ( szFullPath, "\\Data\\Editor\\" );
	m_pDxEditmat->SetParam ( szFullPath, m_hWnd );

	m_p3dPosMesh = new DxFrameMesh;

	strcpy_s ( szFullPath, pFrame->m_szAppPath );
	strcat_s ( szFullPath, "\\Data\\Editor\\" );
	strcat_s ( szFullPath, "3dPos.x" );
	m_p3dPosMesh->SetFile ( szFullPath );

	strcpy_s ( szFullPath, pFrame->m_szAppPath );
	strcat_s ( szFullPath, "\\Data\\Editor\\" );
	m_EditBox.SetPath ( szFullPath );

	return S_OK;
}

HRESULT CLevelEditorView::CreateObjects()
{
	{ // Note : 로딩시 텍스트를 출력하기 위해서 폰트와 텍스트 부분을 먼저 초기화 했다.

		//	Note : 폰트 메니져 초기화.
		DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
		CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pD3dFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );

		//	Note	:	디버그셋 경로 설정 및 초기화
		CDebugSet::InitDeviceObjects( pD3dFont9 );
	}

	HRESULT hr = S_OK;

	hr = InitDeviceObjects();
    if( FAILED(hr) )	return hr;

	hr = RestoreDeviceObjects();	
    if( FAILED(hr) )	return hr;

	return hr;
}

HRESULT CLevelEditorView::InitDeviceObjects()
{
	DxViewPort::GetInstance().InitDeviceObjects ( m_pd3dDevice, m_hWnd );

	DxResponseMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );

	DxViewPort::GetInstance().SetGameCamera();

	D3DXVECTOR3 vFrompt		= D3DXVECTOR3 ( 0.f, 80.0f, -80.0f );
	D3DXVECTOR3	vLookatpt	= D3DXVECTOR3 ( 0.f, 0.0f, 0.0f );
	D3DXVECTOR3	vUpVec		= D3DXVECTOR3 ( 0.f, 1.0f, 0.0f );
	DxViewPort::GetInstance().SetViewTrans ( vFrompt, vLookatpt, vUpVec );

	GLGaeaServer::GetInstance().Create4Level ( m_pd3dDevice );
	GLGaeaServer::GetInstance().SetUpdate ( FALSE );

	SetCurType ( CUR_SEL_MOBSCH );
	m_LandMan.InitDeviceObjects ( m_pd3dDevice );

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	pGLLandMan->InitDeviceObjects ( m_pd3dDevice );

	m_pDxEditmat->InitDeviceObjects ( m_pd3dDevice );
	m_p3dPosMesh->InitDeviceObjects ( m_pd3dDevice );

	m_EditBox.InitDeviceObjects ( m_pd3dDevice );

	return S_OK;
}

HRESULT CLevelEditorView::RestoreDeviceObjects()
{
	DxResponseMan::GetInstance().RestoreDeviceObjects ();

	LPDIRECT3DSURFACEQ pBackBuffer;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();

	DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
				(float)m_d3dsdBackBuffer.Height, 5.0f, 3000.0f );

	HRESULT hr = DxViewPort::GetInstance().SetViewPort();

	if ( FAILED(hr) )
		return hr;

	m_LandMan.RestoreDeviceObjects( m_pd3dDevice );

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	pGLLandMan->RestoreDeviceObjects ( m_pd3dDevice );

	m_p3dPosMesh->RestoreDeviceObjects ( m_pd3dDevice );
	m_sObjectMRS.OnResetDevice ( m_pd3dDevice );

	return S_OK;
}

HRESULT CLevelEditorView::InvalidateDeviceObjects ()
{
	DxResponseMan::GetInstance().InvalidateDeviceObjects();

	m_pDxEditmat->InvalidateDeviceObjects();
	m_p3dPosMesh->InvalidateDeviceObjects ( m_pd3dDevice );

	m_sObjectMRS.OnLostDevice ( m_pd3dDevice );
	m_LandMan.InvalidateDeviceObjects ();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	pGLLandMan->InitDeviceObjects ( m_pd3dDevice );

	return S_OK;
}

HRESULT CLevelEditorView::DeleteDeviceObjects()
{
	DxResponseMan::GetInstance().DeleteDeviceObjects();

	m_pDxEditmat->DeleteDeviceObjects();
	m_p3dPosMesh->DeleteDeviceObjects();

	m_LandMan.DeleteDeviceObjects();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	pGLLandMan->DeleteDeviceObjects();

	GLGaeaServer::GetInstance().CleanUp();

	GMTOOL::CleanUp();

	return S_OK;
}

HRESULT CLevelEditorView::FinalCleanup()
{
	DxResponseMan::GetInstance().FinalCleanup();

	HRESULT hr = GLogicData::GetInstance().ClearData();

	if ( FAILED(hr) )
		return hr;

	if ( m_pDxEditmat )
	{
		SAFE_DELETE ( m_pDxEditmat );
	}

	if ( m_p3dPosMesh )
	{
		SAFE_DELETE ( m_p3dPosMesh );
	}

	return S_OK;
}

HRESULT CLevelEditorView::FrameMove ()
{
	if ( DxInputDevice::GetInstance().GetKeyState(DIK_Q)&DXKEY_UP )
		g_bFRAME_LIMIT = g_bFRAME_LIMIT == FALSE;

	CheckControlfp();

	CLevelMainFrame* pFrame = (CLevelMainFrame* ) AfxGetApp()->m_pMainWnd;
	
	int nWVL = pFrame->m_wndToolBar.m_comboMatrix.GetCurSel();
	m_sObjectMRS.SetWVL ( nWVL );

	if ( m_sObjectMRS.IsCollMode() && m_CurType != CUR_SEL_MOBSCH && m_CurType != CUR_EDIT_BOX )
		m_CurType = CUR_SELECT;

	DxResponseMan::GetInstance().FrameMove ( m_fTime, m_fElapsedTime, m_bDefWin );
	DxViewPort::GetInstance().FrameMoveMAX ( m_fElapsedTime, m_vCollisionPos );

	if ( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == (int)ETCFUNCPAGE )
	{
		CEtcFuncPage* pDialog = &pFrame->m_DlgLevelEdit.m_pSheetTab->m_EtcFuncPage;

		if ( CEtcFuncPage::m_bCamUse && CEtcFuncPage::m_bCamTest )
		{
			D3DXVECTOR3 vBaseCamPos, vFromPt, vLookatPt, vUpVec, vMaxPos, vMinPos, vCamPos;
			float fUp, fDown, fLeft, fRight, fFar, fNear;

			fUp		= (float)atof ( GetWin_Text ( pDialog, IDC_EDIT_CAM_UP ) );
			fDown	= (float)atof ( GetWin_Text ( pDialog, IDC_EDIT_CAM_DOWN ) );
			fLeft	= (float)atof ( GetWin_Text ( pDialog, IDC_EDIT_CAM_LEFT ) );
			fRight	= (float)atof ( GetWin_Text ( pDialog, IDC_EDIT_CAM_RIGHT ) );
			fFar	= (float)atof ( GetWin_Text ( pDialog, IDC_EDIT_CAM_FAR ) );
			fNear	= (float)atof ( GetWin_Text ( pDialog, IDC_EDIT_CAM_NEAR ) );

			vBaseCamPos.x = (float)atof ( GetWin_Text ( pDialog, IDC_EDIT_CAM_X_VAL ) );
			vBaseCamPos.y = (float)atof ( GetWin_Text ( pDialog, IDC_EDIT_CAM_Y_VAL ) );
			vBaseCamPos.z = (float)atof ( GetWin_Text ( pDialog, IDC_EDIT_CAM_Z_VAL ) );

			vMaxPos.x = vBaseCamPos.x + fRight;
			vMaxPos.y = vBaseCamPos.y + fUp;
			vMaxPos.z = vBaseCamPos.z + fFar;

			vMinPos.x = vBaseCamPos.x + fLeft;
			vMinPos.y = vBaseCamPos.y + fDown;
			vMinPos.z = vBaseCamPos.z + fNear;

			DxViewPort::GetInstance().GetViewTrans ( vFromPt, vLookatPt, vUpVec );
			
			CDebugSet::ToView ( 7, "vFromPt %.2fx %.2fy %.2fz", vFromPt.x, vFromPt.y, vFromPt.z );

			vCamPos = vFromPt - vLookatPt;

			if ( vCamPos.x > vMaxPos.x )
				vCamPos.x = vMaxPos.x;
			if ( vCamPos.y > vMaxPos.y )
				vCamPos.y = vMaxPos.y;
			if ( vCamPos.z > vMaxPos.z )
				vCamPos.z = vMaxPos.z;

			if ( vCamPos.x < vMinPos.x )
				vCamPos.x = vMinPos.x;
			if ( vCamPos.y < vMinPos.y )
				vCamPos.y = vMinPos.y;
			if ( vCamPos.z < vMinPos.z )
				vCamPos.z = vMinPos.z;

			vFromPt = vCamPos + vLookatPt;
			CDebugSet::ToView ( 8, "vNEwFromPt %.2fx %.2fy %.2fz", vFromPt.x, vFromPt.y, vFromPt.z );

			DxViewPort::GetInstance().SetCameraControl ( TRUE, vBaseCamPos, fUp, fDown, fLeft, fRight, fFar, fNear );
		}
		/*else
		{
			DxViewPort::GetInstance().SetCameraControl ( TRUE );
		}*/
	}

	m_LandMan.FrameMove ( m_fTime, m_fElapsedTime );
	GLGaeaServer::GetInstance().FrameMove ( m_fTime, m_fElapsedTime );

	if ( m_CurType > CUR_SCALE )
	{
		if ( m_CurType == CUR_EDIT_BOX )
			m_EditBox.FrameMove ( m_fTime, m_fElapsedTime );
	}
	else if ( m_CurType >= CUR_MOVE )
	{
		CurMove();
	}
	else if ( m_CurType == CUR_SEL_MOBSCH )
	{
		DWORD dwMKey = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );

		if ( dwMKey & DXKEY_UP )
		{
			GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
			GLMobScheduleMan* pMobSchMan = pGLLandMan->GetMobSchMan();

			D3DXVECTOR3 vTargetPt, vFromPt;
			vFromPt = DxViewPort::GetInstance().GetFromPt ();
			BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );

			if ( bTargetted )
			{
				GLMobSchedule* pMobSch = pMobSchMan->GetCollisionMobSchedule ( vFromPt, vTargetPt );

				if ( pMobSch )
				{
					PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData ( pMobSch->m_CrowID );
					CMobSetPage::m_bMaterial = (BOOL)pCrow->m_sBasic.m_emCrow == CROW_MATERIAL;

					pFrame->m_DlgLevelEdit.m_pSheetTab->SetActivePage ( MOBSETPAGE );
					pFrame->m_DlgLevelEdit.m_pSheetTab->m_MobSetPage.ActiveMobSetEdit ( FALSE );
					pFrame->m_DlgLevelEdit.m_pSheetTab->m_MobSetPage.SetSelectSchedule ( pMobSch );
				}
			}
		}
	}
	else if ( m_CurType == CUR_SELECT )
	{
		CurSelect();
	}

	if ( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == (int)ETCFUNCPAGE && CEtcFuncPage::m_bLandEffUse )
	{
		CurSelect();
	}

	if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_DOWNED &&
		 pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == MOBSETPAGE )
	{
		if ( DxInputDevice::GetInstance().GetKeyState ( DIK_X ) & DXKEY_DOWN )
		{
			pFrame->m_DlgLevelEdit.m_pSheetTab->m_MobSetPage.CutMobSch ();

			return S_OK;
		}
		if ( DxInputDevice::GetInstance().GetKeyState ( DIK_C ) & DXKEY_DOWN )
		{
			pFrame->m_DlgLevelEdit.m_pSheetTab->m_MobSetPage.CopyMobSch ();

			return S_OK;
		}
		if ( DxInputDevice::GetInstance().GetKeyState ( DIK_V ) & DXKEY_DOWN )
		{
			pFrame->m_DlgLevelEdit.m_pSheetTab->m_MobSetPage.PasteMobSch ();

			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CLevelEditorView::Render ()
{
	D3DCOLOR colorClear = D3DCOLOR_XRGB(89,135,179);
	DxFogMan::GetInstance().RenderFogSB ( m_pd3dDevice );
	colorClear = DxFogMan::GetInstance().GetFogColor();

	// Clear the viewport
	HRESULT hr = m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
						colorClear, 1.0f, 0L );

	DxShadowMap::GetInstance().ClearShadow ( m_pd3dDevice );

	//// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );

		//	Note : Light 설정.
		//
		DxLightMan::GetInstance()->Render ( m_pd3dDevice );

		//	Note : 버텍스 쉐이더 콘스탄트 설정.
		//
		DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT, (float*)&Light.m_Light.Direction, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIFFUSE, (float*)&Light.m_Light.Diffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTAMBIENT, (float*)&Light.m_Light.Ambient, 1 );

		D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_CAMERAPOSITION, (float*)&vFromPt, 1 );

		D3DXMATRIX matView = DxViewPort::GetInstance().GetMatView();
		D3DXMATRIX matProj = DxViewPort::GetInstance().GetMatProj();

		m_pd3dDevice->SetVertexShaderConstantF ( VSC_SKIN_DEFAULT, (float*)&D3DXVECTOR4 (1.f, 0.5f, 0.f, 765.01f), 1 );

		D3DXVECTOR3	vLightVector = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
		D3DXVec3TransformNormal ( &vLightVector, &vLightVector, &matView );
		D3DXVec3Normalize ( &vLightVector, &vLightVector);
		vLightVector = -vLightVector;
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT_VIEW, (float*)&vLightVector, 1 );

		D3DXMatrixTranspose( &matView, &matView );
		D3DXMatrixTranspose( &matProj, &matProj );

		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume();

		DxSkyMan::GetInstance().Render ( m_pd3dDevice );

		m_LandMan.Render ( m_pd3dDevice, cv );
		m_LandMan.Render_EFF ( m_pd3dDevice, cv );
		m_LandMan.RenderPickAlpha ( m_pd3dDevice );

		GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

		if ( m_bRENDQUAD )
			pGLLandMan->RendQuad ( m_pd3dDevice );

		GLMobScheduleMan* pMobSchMan = pGLLandMan->GetMobSchMan();
		pMobSchMan->Render ( m_pd3dDevice, cv );

		if ( CMobSetPage::m_bShow )
		{
			CMobSetPage::m_Schedule.RenderGenPos ( m_pd3dDevice );
			CMobSetPage::m_Schedule.Render ( m_pd3dDevice, cv );
		}

		GLGaeaServer::GetInstance().Render ( m_pd3dDevice, cv );

		pGLLandMan->Render ( m_pd3dDevice, cv );
		pGLLandMan->Render_EFF ( m_pd3dDevice, cv );

		DxSkyMan::GetInstance().Render_AFTER ( m_pd3dDevice );

		D3DXMatrixIdentity( &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		DWORD	dwNormal;
		m_pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormal );
		m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

		D3DXVECTOR3 vCollPos = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );

		switch ( m_CurType )
		{
		case CUR_SELECT:
			{
				vCollPos = m_vCollisionPos;
				D3DXMatrixTranslation ( &matWorld, vCollPos.x, vCollPos.y, vCollPos.z );

				D3DXVECTOR3 pV = DxViewPort::GetInstance().GetFromPt();
				pV -= vCollPos;

				float fDist, fScale;
				fDist = D3DXVec3Length ( &pV );
				fScale = DxViewPort::GetInstance().ComputeDistSize ( fDist ) / 800.0f;

				D3DXMATRIX matScale;
				D3DXMatrixScaling ( &matScale, fScale, fScale, fScale );
				D3DXMatrixMultiply ( &matWorld, &matScale, &matWorld );

				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
				m_p3dPosMesh->Render ( m_pd3dDevice, &matWorld, &cv );

				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
				m_p3dPosMesh->Render ( m_pd3dDevice, &matWorld, &cv );
			}
			break;
		case CUR_ROTATE:
		case CUR_MOVE:
		case CUR_SCALE:
			{
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
				m_pDxEditmat->Render ( m_pd3dDevice );

				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
				m_pDxEditmat->Render ( m_pd3dDevice );
			}
			break;
		case CUR_EDIT_BOX:
			{
				m_EditBox.Render ( m_pd3dDevice );
			}
			break;
		default:
			break;
		}

		RenderLandEffectState();

		m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormal );

		if ( DxLandGateMan::m_bREND )
		{
			pGLLandMan->GetLandGateMan().Render ( m_pd3dDevice );
		}

		m_sObjectMRS.Render ( m_pd3dDevice );
		RenderText ();

		// End the scene.
		m_pd3dDevice->EndScene();
	}

	return S_OK;
}

HRESULT CLevelEditorView::RenderText ()
{
	D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,255);
	D3DCOLOR fontWarningColor = D3DCOLOR_ARGB(255,0,255,255);
	TCHAR szMsg[MAX_PATH] = TEXT("");

	// Output display stats
	CD3DFontPar* pD3dFont = DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	if ( !pD3dFont )	return S_OK;

	FLOAT fNextLine = 40.0f; 

	lstrcpy( szMsg, m_strDeviceStats );
	fNextLine -= 20.0f;
	pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

	lstrcpy( szMsg, m_strFrameStats );
	fNextLine -= 20.0f;
	pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

	CDebugSet::Render ();

	return S_OK;
}

void CLevelEditorView::RenderLandEffectState ()
{
	CLevelMainFrame* pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	if ( !pFrame ) return;

	CsheetWithTab* pSheetTab = pFrame->m_DlgLevelEdit.m_pSheetTab;
	if ( !pSheetTab ) return;

	CEtcFuncPage* pDialog = &pSheetTab->m_EtcFuncPage;

	if ( CEtcFuncPage::m_bLandEffUse )
	{
		if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL )&DXKEY_DOWNED &&
			 DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT )&DXKEY_DOWNED )
		{
			if ( DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT )&DXKEY_DOWN )
			{
				pDialog->m_vMinLandPos.x = m_vCollisionPos.x;
				pDialog->m_vMinLandPos.y = m_vCollisionPos.z;
			}
			else
			{
				pDialog->m_vMaxLandPos.x = m_vCollisionPos.x;
				pDialog->m_vMaxLandPos.y = m_vCollisionPos.z;
			}
		}

		float fYDist1 = 100.0f;
		float fYDist2 = 100.0f;

		D3DXVECTOR3 vCollision, vFromPt, vTargetPt;
		DWORD dwCollisionID;
		BOOL bCollision;

		vTargetPt.y = -1000.0f;
		vFromPt.y	= 1000.0f;

		vTargetPt.x = vFromPt.x = pDialog->m_vMinLandPos.x;
		vTargetPt.z = vFromPt.z = pDialog->m_vMinLandPos.y;

		m_LandMan.GetNaviMesh()->IsCollision ( vFromPt, vTargetPt, vCollision, dwCollisionID, bCollision );

		if ( bCollision )
			fYDist1 = vCollision.y;

		vTargetPt.y = -1000.0f;
		vFromPt.y	= 1000.0f;

		vTargetPt.x = vFromPt.x = pDialog->m_vMaxLandPos.x;
		vTargetPt.z = vFromPt.z = pDialog->m_vMaxLandPos.y;

		m_LandMan.GetNaviMesh()->IsCollision ( vFromPt, vTargetPt, vCollision, dwCollisionID, bCollision );

		if ( bCollision )
			fYDist2 = vCollision.y;

		if ( ( pDialog->m_vMinLandPos.x != 0.0 ) || ( pDialog->m_vMinLandPos.y != 0.0 ) )
		{
			if ( ( pDialog->m_vMaxLandPos.x != 0.0 ) || ( pDialog->m_vMaxLandPos.y != 0.0 ) )
			{
				D3DXVECTOR2 vMax, vMin;

				vMin.x = min ( pDialog->m_vMinLandPos.x, pDialog->m_vMaxLandPos.x );
				vMin.y = min ( pDialog->m_vMinLandPos.y, pDialog->m_vMaxLandPos.y );
				vMax.x = max ( pDialog->m_vMinLandPos.x, pDialog->m_vMaxLandPos.x );
				vMax.y = max ( pDialog->m_vMinLandPos.y, pDialog->m_vMaxLandPos.y );

				D3DXVECTOR3 vertex[4];

				float fYDistMax;
				fYDistMax = max ( fYDist1, fYDist2 );

				vertex[3].y = fYDistMax + 1.0f;
				vertex[2].y = vertex[3].y;
				vertex[1].y = vertex[3].y;
				vertex[0].y = vertex[3].y;

				vertex[0].x = vMin.x;
				vertex[0].z = vMin.y;
				vertex[1].x = vMax.x;
				vertex[1].z = vMin.y;

				vertex[2].x = vMin.x;
				vertex[2].z = vMax.y;
				vertex[3].x = vMax.x;
				vertex[3].z = vMax.y;

				EDITMESHS::RENDERPLANE ( m_pd3dDevice, vertex, -1 );
			}
		}

		for ( int i = 0; i < (int)pDialog->m_vecLandEffect.size(); ++i )
		{
			SLANDEFFECT landEffect = pDialog->m_vecLandEffect[i];

			D3DXVECTOR3 vFromPt1, vTargetPt1;
			
			vTargetPt1.y = -1000.0f;
			vFromPt1.y	= 1000.0f;

			vTargetPt1.x = vFromPt1.x = landEffect.vMinPos.x;
			vTargetPt1.z = vFromPt1.z = landEffect.vMinPos.y;

			m_LandMan.GetNaviMesh()->IsCollision ( vFromPt1, vTargetPt1, vCollision, dwCollisionID, bCollision );

			if ( bCollision )
				fYDist1 = vCollision.y;

			vTargetPt1.y = -1000.0f;
			vFromPt1.y	= 1000.0f;

			vTargetPt1.x = vFromPt1.x = landEffect.vMaxPos.x;
			vTargetPt1.z = vFromPt1.z = landEffect.vMaxPos.y;

			m_LandMan.GetNaviMesh()->IsCollision ( vFromPt1, vTargetPt1, vCollision, dwCollisionID, bCollision );

			if ( bCollision )
				fYDist2 = vCollision.y;

			D3DXVECTOR3 vertex1[4];

			float fYDistMax;
			fYDistMax = max ( fYDist1, fYDist2 );

			vertex1[3].y = fYDistMax + 1.0f;
			vertex1[2].y = vertex1[3].y;
			vertex1[1].y = vertex1[3].y;
			vertex1[0].y = vertex1[3].y;

			vertex1[0].x = landEffect.vMinPos.x;
			vertex1[0].z = landEffect.vMinPos.y;
			vertex1[1].x = landEffect.vMaxPos.x;
			vertex1[1].z = landEffect.vMinPos.y;

			vertex1[2].x = landEffect.vMinPos.x;
			vertex1[2].z = landEffect.vMaxPos.y;
			vertex1[3].x = landEffect.vMaxPos.x;
			vertex1[3].z = landEffect.vMaxPos.y;

			DWORD dwAlpha, dwColor;

			if ( pDialog->m_dwSelectNum == i )
			{
				dwAlpha = 0x80;
			}
			else
			{
				dwAlpha = 0xFF;
			}

			switch ( landEffect.emLandEffectType )
			{
			case EMLANDEFFECT_ATK_SPEED:
				dwColor = 0xFF0000 | ( dwAlpha << 24 );
				break;
			case EMLANDEFFECT_MOVE_SPEED:
				dwColor = 0xFF0000 | ( dwAlpha << 24 ) | 0xA500;
				break;
			case EMLANDEFFECT_MP_RATE:
				dwColor = 0xFF0000 | ( dwAlpha << 24 ) | 0xFF00;
				break;
			case EMLANDEFFECT_HP_RATE:
				dwColor = ( dwAlpha << 24 ) | 0x8000;
				break;
			case EMLANDEFFECT_RECOVER_RATE:
				dwColor = ( dwAlpha << 24 ) | 0xFF;
				break;
			case EMLANDEFFECT_DAMAGE_RATE:
				dwColor = ( dwAlpha << 24 ) | 0x82;
				break;
			case EMLANDEFFECT_DEFENSE_RATE:
				dwColor = 0xDD0000 | ( dwAlpha << 24 ) | 0xA0DD;
				break;
			case EMLANDEFFECT_RESIST_RATE:
				dwColor = 0x800000 | ( dwAlpha << 24 ) | 0x8080;
				break;
			case EMLANDEFFECT_CANCEL_ALLBUFF:
				dwColor = dwAlpha << 24;
				break;
			case EMLANDEFFECT_SAFE_ZONE:
				dwColor = 0xFFFFFF | ( dwAlpha << 24 );
				break;
			default:
				break;
			}

			EDITMESHS::RENDERPLANE ( m_pd3dDevice, vertex1, dwColor );
		}

		CDebugSet::ToView ( 3, "vMin %.2fx %.2fy", pDialog->m_vMinLandPos.x, pDialog->m_vMinLandPos.y );
		CDebugSet::ToView ( 4, "vMax %.2fx %.2fy", pDialog->m_vMaxLandPos.x, pDialog->m_vMaxLandPos.y );
		CDebugSet::ToView ( 5, "vCollisionPos %.2fx %.2fy %.2fz", m_vCollisionPos.x, m_vCollisionPos.y, m_vCollisionPos.z );
	}
}

void CLevelEditorView::SetActive ( BOOL bActive )
{
	DxInputDevice::GetInstance().OnActivate ( bActive );
}

void CLevelEditorView::ActiveEditBox ( D3DXVECTOR3 vMax, D3DXVECTOR3 vMin )
{
	m_CurType = CUR_EDIT_BOX;
	m_EditBox.SetBox ( vMax, vMin );
}

void CLevelEditorView::ActiveEditMatrix ( DXAFFINEMATRIX* pAffineMatrix )
{
	m_CurType = CUR_MOVE;

	m_pDxEditmat->SetAffineMatrix ( pAffineMatrix );
	m_sObjectMRS.SetMatrix ( NULL );
}

void CLevelEditorView::ActiveEditMatrix2 ( D3DXMATRIX* matWorld )
{
	m_CurType = CUR_MOVE;

	if ( matWorld )
		m_sObjectMRS.SetMatrix ( matWorld );
	else
		m_sObjectMRS.SetMatrix ( NULL );

	m_pDxEditmat->SetAffineMatrix ( NULL );
}

void CLevelEditorView::DeActiveEditMatrix ()
{
	m_CurType = CUR_SELECT;

	m_pDxEditmat->SetAffineMatrix ( NULL );
	m_sObjectMRS.SetMatrix ( NULL );
}

void CLevelEditorView::SetCurEditType ( DWORD EditType )
{
	m_pDxEditmat->SetType ( (EDITMAT_TYPE)EditType );

	switch ( EditType )
	{
	case TRANS_MAT:
		m_sObjectMRS.SetMode ( 1 );
		break;
	case ROTATE_MAT:
		m_sObjectMRS.SetMode ( 2 );
		break;
	case SCALE_MAT:
		m_sObjectMRS.SetMode ( 3 );
		break;
	}
}

void CLevelEditorView::CurMove ()
{
	m_pDxEditmat->FrameMove ( m_fTime, m_fElapsedTime );
}

void CLevelEditorView::CurSelect()
{
	DWORD dwKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );

	if ( dwKeyState & DXKEY_DOWNED )
	{
		D3DXVECTOR3 vTargetPt, vFromPt;
		vFromPt = DxViewPort::GetInstance().GetFromPt ();
		BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );

		if ( bTargetted )
		{
			if ( m_LandMan.GetNaviMesh() )
			{
				LPDXFRAME pDxFrame;
				pDxFrame = NULL;

				D3DXVECTOR3 vCollision;
				BOOL		bCollision;
				DWORD		dwCollisionID;

				m_LandMan.GetNaviMesh()->IsCollision ( vFromPt, vTargetPt, vCollision, dwCollisionID, bCollision );

				if ( bCollision )
				{
					m_vCollisionPos = vCollision;
				}
			}
		}
	}

	m_p3dPosMesh->FrameMove ( m_fTime, m_fElapsedTime );
}
