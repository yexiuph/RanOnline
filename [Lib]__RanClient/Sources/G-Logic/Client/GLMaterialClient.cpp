#include "pch.h"
#include "./GLMaterialClient.h"
#include "./GLFactEffect.h"
#include "./GLGaeaClient.h"
#include "../[Lib]__Engine/Sources/DxTools/DxViewPort.h"
#include "../[Lib]__Engine/Sources/DxTools/DxShadowMap.h"
#include "../[Lib]__Engine/Sources/DxEffect/DxEffCharData.h"
#include "../[Lib]__Engine/Sources/DxEffect/DxEffGroupPlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLMaterialClient::GLMaterialClient(void) :
	m_pd3dDevice(NULL),
	m_pSkinChar(NULL),

	m_vDir(0,0,-1),
	m_vDirOrig(0,0,-1),
	m_vPos(0,0,0),

	m_vServerPos(0,0,0),

	m_vMaxOrg(8,20,8),
	m_vMinOrg(-8,0,-8),

	m_vMax(8,20,8),
	m_vMin(-8,0,-8),
	m_fHeight(20.f),

	m_dwGlobID(0),
	m_dwCeID(0),
	m_pLandManClient(NULL),
	m_pGlobNode(NULL),
	m_pQuadNode(NULL),
	m_pCellNode(NULL),

	m_fAge(0.0f),
	m_Action(GLAT_IDLE),
	m_dwActState(0),
	m_fIdleTime(0.0f)

{

}

GLMaterialClient::~GLMaterialClient(void)
{
	m_pd3dDevice = NULL;

	m_actorMove.ResetMovedData();
	m_actorMove.Stop();
	m_actorMove.Release();

	m_pLandManClient = NULL;
	m_pGlobNode = NULL;
	m_pQuadNode = NULL;
	m_pCellNode = NULL;
	
	SAFE_DELETE(m_pSkinChar);
}

HRESULT GLMaterialClient::CreateMaterial ( GLLandManClient *pLandManClient, SDROP_MATERIAL *pCrowDrop, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	GASSERT(pLandManClient);

	m_pLandManClient = pLandManClient;
	m_pd3dDevice = pd3dDevice;

	//	Note : 몹의 설정 정보를 가져옴.
	m_pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pCrowDrop->sNativeID );
	if( !m_pCrowData )
	{
		SNATIVEID sMobID = pCrowDrop->sNativeID;
		CDebugSet::ToLogFile ( "GLMaterialClient::CreateMaterial()-GLCrowDataMan::GetCrowData() [%d/%d] 가져오기 실패.", sMobID.wMainID, sMobID.wSubID );
		return E_FAIL;
	}

	m_sNativeID = m_pCrowData->sNativeID;

	//	Note : 이동 제어 초기화.
	m_vPos = pCrowDrop->vPos;
	m_vDir = pCrowDrop->vDir;

	m_actorMove.Create ( m_pLandManClient->GetNaviMesh(), m_vPos, -1 );

	DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_pCrowData->GetSkinObjFile(), m_pd3dDevice, TRUE );
	if ( !pCharData )
	{
		if( strlen( m_pCrowData->GetSkinObjFile() ) )
		{
			CDebugSet::ToLogFile ( "GLCrowClient::CreateCrow()-DxSkinCharDataContainer::LoadData() [%s] 가져오기 실패.", m_pCrowData->GetSkinObjFile() );
		}
		return E_FAIL;
	}

	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;

	m_pSkinChar->SetCharData ( pCharData, pd3dDevice );

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	//	Note : 현정보 적용.
	m_dwGlobID = pCrowDrop->dwGlobID;
	m_dwCeID = pCrowDrop->dwCeID;

	switch ( pCrowDrop->emAction )
	{
	case GLAT_MOVE:
		break;

	case GLAT_FALLING:
		TurnAction ( GLAT_FALLING );
		break;

	case GLAT_DIE:
		TurnAction ( GLAT_DIE );
		break;
	};
	
	//	Note : 생성 효과 발생
	//
	if ( pCrowDrop->dwFLAGS&SDROP_MATERIAL::CROW_GEM )
	{
		D3DXMATRIX matEffect;
		D3DXMatrixTranslation ( &matEffect, m_vPos.x, m_vPos.y, m_vPos.z );

		std::string strGEM_EFFECT = GLCONST_CHAR::strMOB_GEM_EFFECT.c_str();
		if ( !m_pCrowData->m_sAction.m_strBirthEffect.empty() )	strGEM_EFFECT = m_pCrowData->m_sAction.m_strBirthEffect;

		STARGETID sTargetID(m_pCrowData->m_emCrow,m_dwGlobID,m_vPos);
		DxEffGroupPlayer::GetInstance().NewEffGroup
		(
			strGEM_EFFECT.c_str(),
			matEffect,
			&sTargetID
		);
	}

	if ( !m_pSkinChar->GETCURANIMNODE() )
	{
		CDebugSet::ToLogFile ( "ERORR : current animation node null point error [ %s ] [ M %d S %d ]", m_pSkinChar->GetFileName(), m_pSkinChar->GETCURMTYPE(), m_pSkinChar->GETCURSTYPE() );
		m_pSkinChar->DEFAULTANI ();
 	}

	return S_OK;
}

WORD GLMaterialClient::GetBodyRadius ()
{
	return m_pCrowData->m_sAction.m_wBodyRadius;
}

void GLMaterialClient::SetPosition ( D3DXVECTOR3 &vPos )
{
	GASSERT(m_pCrowData);

	m_vPos = vPos;
}


BOOL GLMaterialClient::IsValidBody ()
{
	return ( m_Action < GLAT_FALLING );
}

BOOL GLMaterialClient::IsDie ()
{
	if ( IsACTION(GLAT_DIE) )		return TRUE;

	return FALSE;
}

BOOL GLMaterialClient::IsHaveVisibleBody ()
{
	return ( m_pCrowData->GetSkinObjFile() && (strlen(m_pCrowData->GetSkinObjFile())!=0) );
}

void GLMaterialClient::TurnAction ( EMACTIONTYPE toAction )
{
	//	Note : 이전 액션 취소(쓰러질때와 죽을때는 제외)
	//
	switch ( m_Action )
	{
	case GLAT_IDLE:
		break;

	case GLAT_FALLING:
		if ( toAction==GLAT_FALLING )	return;
		break;
		
	case GLAT_DIE:
		if ( toAction==GLAT_FALLING || toAction==GLAT_DIE )	return;
		break;

	default:
		break;
	};

	//	Note : 액션 초기화.
	//
	m_Action = toAction;

	switch ( m_Action )
	{
	case GLAT_IDLE:
		m_fIdleTime = 0.0f;
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	case GLAT_FALLING:
		break;

	case GLAT_DIE:
		{
			D3DXVECTOR3		vNORMAL;
			D3DXVECTOR3		vSTRIKE;

			bool bBLOOD_TRACE(false);
			for ( int i=0; i<PIECE_REV01; ++i )
			{
				PDXCHARPART pCharPart = m_pSkinChar->GetPiece(i);
				if ( !pCharPart )	continue;

				if ( pCharPart->GetTracePos(STRACE_BLOOD01) )
				{
					bBLOOD_TRACE = true;
					pCharPart->CalculateVertexInflu ( STRACE_BLOOD01, vSTRIKE, vNORMAL );
				}
			}

			if ( !bBLOOD_TRACE )		vSTRIKE = m_vPos;

			D3DXMATRIX matEffect;
			D3DXMatrixTranslation ( &matEffect, vSTRIKE.x, vSTRIKE.y, vSTRIKE.z );

			STARGETID sTargetID(m_pCrowData->m_emCrow,m_dwGlobID,m_vPos);
			DxEffGroupPlayer::GetInstance().NewEffGroup
			(
				GLCONST_CHAR::strMOB_DIE_BLOOD.c_str(),
				matEffect,
				&sTargetID
			);

			DxEffcharDataMan::GetInstance().PutEffect
			(
				m_pSkinChar,
				GLCONST_CHAR::strERASE_EFFECT.c_str(), &m_vDir
			);
		}
		break;

	default:
		GASSERT("GLCharacter::TurnAction() 준비되지 않은 ACTION 이 들어왔습니다.");
		break;
	};

}


HRESULT GLMaterialClient::UpateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	BOOL bLowSP(false);

	BOOL bFreeze = FALSE;
	switch ( m_Action )
	{
	case GLAT_IDLE:
		{
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		}
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	case GLAT_SHOCK:
		break;

	case GLAT_PUSHPULL:
		break;

	case GLAT_FALLING:
		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_DIE );
		break;

	case GLAT_DIE:
		bFreeze = TRUE;
		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		m_pSkinChar->TOENDTIME();
		break;
	case GLAT_GATHERING:
		break;
	};

	//	Note : Mob 스킨 업데이트.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( m_Action )
	{
	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		break;
	};

	m_pSkinChar->SetPosition ( m_vPos );
	m_pSkinChar->FrameMove ( fTime, fSkinAniElap, TRUE, bFreeze );

	return S_OK;
}

BOOL GLMaterialClient::IsCollisionVolume ()
{
	const CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume();
	if ( COLLISION::IsCollisionVolume ( CV, m_vMax, m_vMin ) )	return TRUE;

	return FALSE;
}

HRESULT GLMaterialClient::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;
	GASSERT(m_pCrowData);
	GASSERT(m_pLandManClient);

	//	Note : 에니메이션, 스킨 업데이트.
	//
	UpateAnimation ( fTime, fElapsedTime );

	switch ( m_Action )
	{
	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	case GLAT_PUSHPULL:
		break;
	};

	// 현재 위치 업데이트
	m_vPos = m_actorMove.Position();

	
	//	Note : 현재 위치와 방향으로 Transform 메트릭스 계산.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	m_matTrans = matYRot * matTrans;

	//	Note : AABB 계산.
	//
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;
	

	return S_OK;
}

float GLMaterialClient::GetDirection ()
{
	return DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
}

HRESULT GLMaterialClient::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{
	HRESULT hr;
	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	return S_OK;

	DxEffcharDataMan::GetInstance().OutEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );

	hr = m_pSkinChar->Render ( pd3dDevice, m_matTrans );
	if ( FAILED(hr) )	return hr;

	#ifdef _SYNC_TEST
	{
		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vServerPos, 2.0f );
	}
	#endif

	return S_OK;
}

HRESULT GLMaterialClient::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr;
	if ( IsACTION(GLAT_DIE) )			return S_FALSE;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	return S_OK;
	
	//	Note : 그림자 랜더링.
	//
	hr = DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matTrans, pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT GLMaterialClient::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	return S_OK;
}

HRESULT GLMaterialClient::InvalidateDeviceObjects ()
{
	m_pSkinChar->InvalidateDeviceObjects ();
	return S_OK;
}

void GLMaterialClient::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_ACTION_BRD:
		{
			GLMSG::SNET_ACTION_BRD *pNetMsg = (GLMSG::SNET_ACTION_BRD *)nmg;
			TurnAction ( pNetMsg->emAction );
		}
		break;

	default:
		CDebugSet::ToListView ( "GLCrowClient::MsgProcess() 분류되지 않은 메시지 수신. TYPE[%d/%d]", nmg->nType, nmg->nType-NET_MSG_GCTRL );
		break;
	};
}




