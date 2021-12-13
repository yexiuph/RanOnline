#include "pch.h"
#include <algorithm>
#include "./StringUtils.h"

#include "./RENDERPARAM.h"
#include "./DxEffChar.h"

#include "DxCharPart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxCharPart::DxCharPart () :
	m_pSkinMesh(NULL),
	m_pSkeleton(NULL),
	m_emType(PIECE_HEAD),
	m_emWeaponWhereBack(EMPEACE_WEAPON_RSLOT),

	m_dwFlag(NULL),
	m_dwFlags(NULL),
	m_pmcMesh(NULL),

	m_dwMaterialNum(0),
	m_pMaterialPiece(NULL),

	m_dwVertexNUM(0),
	m_bRender(TRUE),
	m_bAttackMode(TRUE),
	m_pHandHeld(NULL),
	m_pSlotHeld(NULL),

	m_nGrindLevel(0)
{
	memset( m_szFileName, 0, sizeof(char)*FILE_LENGTH );
	D3DXMatrixIdentity( &m_matCombine );

	m_vecEFFECT.reserve(20);
}

DxCharPart::~DxCharPart ()
{
	ReSetPart ();
}

void DxCharPart::ClearEffList ()
{
	m_dwFlags = NULL;

	std::for_each ( m_vecEFFECT.begin(), m_vecEFFECT.end(), std_afunc::DeleteObject() );
	//m_vecEFFECT.erase ( m_vecEFFECT.begin(), m_vecEFFECT.end() );
	m_vecEFFECT.clear();
}

void DxCharPart::SumEffFlag ()
{
	m_dwFlags = NULL;

	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		m_dwFlags |= m_vecEFFECT[n]->GetFlag();
	}
}

//	Note : 신규 이펙트 생성시.
//
void DxCharPart::AddEffList ( DxEffChar* pEffChar )
{
	GASSERT(pEffChar);

	m_vecEFFECT.push_back( pEffChar );
	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	//	flag가 지정된 효과의 경우 flag를 합산.
	m_dwFlags |= pEffChar->GetFlag();
}

//	Note : 효과 생명이 끝났을때
//
void DxCharPart::DelEffList ( DxEffChar* pEffChar )
{
	if ( !pEffChar )	return;

	DWORD dwFlag = pEffChar->GetFlag();
	DWORD dwOrder = pEffChar->GetStateOrder();

	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), dwOrder, DXEFFCHAR_OPER() );
	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > dwOrder )	break;	//	존제하지 않는다.
		if ( pEFFECT!=pEffChar )					continue;

		//	삭제.
		SAFE_DELETE(pEFFECT);
		m_vecEFFECT.erase(pos);
		break;	// 반복자로 할 경우 삭제시 유효하지 않을 수 있으므로 나가야한다.
	}

	//	flag가 있었던 효과가 삭제될 경우 flag를 다시 합산.
	if ( dwFlag )		SumEffFlag ();
}

//	Note : 강제로 삭제시, 지속효과 제거시 호출.
//
void DxCharPart::DelEffList ( const char* szEffFile )
{
	if ( szEffFile[0] == NULL )		return;
	
	DWORD dwFlag=NULL;

	for ( size_t n=0; n<m_vecEFFECT.size(); )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		if ( pEFFECT->GetFileName() == szEffFile )
		{
			dwFlag = pEFFECT->GetFlag();

			SAFE_DELETE(pEFFECT);
			m_vecEFFECT.erase(m_vecEFFECT.begin()+n);
		}
		else
		{
			++n;
		}
	}

	//	flag가 있었던 효과가 삭제될 경우 flag를 다시 합산.
	if ( dwFlag )	SumEffFlag ();
}

//	Note : 지속효과 관리시 존제하는지 검사후 넣어줄때 사용.
//
BOOL DxCharPart::FindEffList ( const char* szEffFile )
{
	if ( szEffFile[0] == NULL )		return FALSE;

	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		if ( pEFFECT->GetFileName() == szEffFile )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void DxCharPart::SetPart ( DxSkinPiece* pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice, bool bCharaterData )
{
	StringCchCopy ( m_szFileName, FILE_LENGTH, pSkinPiece->m_szFileName );

	m_pSkinMesh = pSkinPiece->m_pSkinMesh;
	m_pSkeleton = pSkinPiece->m_pSkeleton;
	m_emType = pSkinPiece->m_emType;
	m_emWeaponWhereBack = pSkinPiece->m_emWeaponWhereBack;

	m_pmcMesh = pSkinPiece->m_pmcMesh;
	m_dwVertexNUM = pSkinPiece->GetVertexNUM();
	m_dwMaterialNum = pSkinPiece->m_dwMaterialNum;

	if ( pSkinPiece->m_pMaterialPiece )
	{
		SAFE_DELETE_ARRAY(m_pMaterialPiece);
		m_pMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_pMaterialPiece[i] = pSkinPiece->m_pMaterialPiece[i];
		}
	}

	m_vecTrace = pSkinPiece->m_vecTrace;

	ClearEffList ();

	//	효과 복제.
	size_t nSIZE = pSkinPiece->m_vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = pSkinPiece->m_vecEFFECT[n];
		
		DxEffChar* pNEW_EFFECT = pEFFECT->CloneInstance ( pd3dDevice, this, NULL );
		m_vecEFFECT.push_back ( pNEW_EFFECT );
	}

	m_dwFlag = pSkinPiece->m_dwFlag;

	if( bCharaterData )
	{
		UINT i;
		for( i = 0; i < m_vecEFFECT.size(); i++ )
		{
			if( m_vecEFFECT[i]->GetFlag() == EMECF_AMBIENT )
			{			
				return;
			}
		}
		
		DxEffChar* pEffChar;
		pEffChar = DxEffCharMan::GetInstance().AddDefaultAmbient( this, NULL );
		AddEffList( pEffChar );
	}
}

void DxCharPart::ReSetPart ()
{
	ClearEffList ();

	if ( m_szFileName[0]!='\0' )
		DxSkinPieceContainer::GetInstance().ReleasePiece ( m_szFileName );
	m_szFileName[0] = '\0';

	m_pSkinMesh = NULL;
	m_pSkeleton = NULL;
	m_pmcMesh = NULL;
	m_dwVertexNUM = 0;

	SAFE_DELETE_ARRAY(m_pMaterialPiece);
}

HRESULT DxCharPart::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	size_t nSIZE = m_vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		pEFFECT->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxCharPart::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	size_t nSIZE = m_vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		pEFFECT->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxCharPart::InvalidateDeviceObjects ()
{
	size_t nSIZE = m_vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		pEFFECT->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxCharPart::DeleteDeviceObjects()
{
	size_t nSIZE = m_vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		pEFFECT->DeleteDeviceObjects ();
	}

	ClearEffList();

	return S_OK;
}

HRESULT DxCharPart::FrameMove ( float fTime, float fETime, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwKeyTime )
{
	//	Note : 이팩트 업데이트.
	//
	for ( size_t n=0; n<m_vecEFFECT.size(); )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];

		pEFFECT->SetCurrentAni ( MType, SType, dwKeyTime );
		pEFFECT->FrameMove ( fTime, fETime );

		//	Note : 종료시점 도달시에 Effect 제거.
		if ( pEFFECT->IsEnd() )
		{
 			DWORD dwFlag = pEFFECT->GetFlag();
			SAFE_DELETE(pEFFECT);
			m_vecEFFECT.erase ( m_vecEFFECT.begin()+n );

			if ( dwFlag )	SumEffFlag();
		}
		else
		{
			++n;
		}
	}

	SumEffFlag();

	return S_OK;
}

inline HRESULT DxCharPart::DrawMeshContainer ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOriginDraw, BOOL bAlpha, BOOL bShadow, BOOL bAlphaTex, float fHeight )
{
	if ( !m_pmcMesh )		return E_FAIL;

	HRESULT hr = S_OK;

	if ( m_pSkinMesh )
	{
		m_pSkinMesh->SetDrawState( bOriginDraw, bAlpha, bShadow, bAlphaTex, FALSE, fHeight );

		if( m_pmcMesh->pSkinInfo )	
		{
			CHARSETTING sCharSetting;
			sCharSetting.pMeshContainerBase = m_pmcMesh;
			sCharSetting.pmtrlPiece			= m_pMaterialPiece;
			sCharSetting.pmtrlSpecular		= NULL;
			sCharSetting.bWorldIdentity		= FALSE;
			sCharSetting.emRDOP				= CTOP_BASE;

			m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting );
		}
		else						m_pSkinMesh->DrawFrameMesh( pd3dDevice, m_pmcMesh, m_pMaterialPiece, &m_matCombine );

		return S_OK;
	}

	return E_FAIL;
}

HRESULT DxCharPart::Render ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton, SKINEFFDATA& sSKINEFFDATA, BOOL bShadow, BOOL bEff )
{
	if ( !m_pmcMesh )	return S_OK;
	if ( !m_bRender )	return S_OK;

	BOOL bOriginDraw(TRUE);
	BOOL bAlphaTex(TRUE);
	BOOL bAlpha(FALSE);
	BOOL bCrashRender(FALSE);

	if( (m_dwFlags&EMECF_NORMALMAP) && (m_dwFlags&EMECF_NONHLSL) )	bCrashRender = TRUE;	// 렌더시 충돌이 일어난다.

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
	case EMCRT_VERTEX:
		if( m_dwFlags & EMECF_ORGINNON )	bOriginDraw = FALSE;
		break;
	case EMCRT_PIXEL:
		if( m_dwFlags & EMECF_NORMALMAP )	bOriginDraw = FALSE;
		break;
	}

	if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )	bAlphaTex = FALSE;
	if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )	bAlpha = TRUE;


	//	Note : 일반 메시일 경우 Bone에 링크되어 있는걸 가정.
	//		연결 설정된 본과 다른 곳에 사용된다는걸 가정하여 본을 다시 연결	시켜줌.
	//
	if ( !m_pmcMesh->pSkinInfo )
	{
		//	Note : 영향 받는 본의 matrix 포인터를 다시 찾는다.
		//			m_pmcMesh->m_szBoneNames[0];	//	[LOCAL]
		//			m_pmcMesh->m_szBoneNames[1];	//	[LOCAL]
		//			m_pmcMesh->m_szBoneNames[2];	//	[COMBINED] <-- 이것만 실제로 BONE에서 영향을 받는것.
		m_pHandHeld = pSkeleton->FindBone ( m_pmcMesh->szBoneName );

		switch( m_emWeaponWhereBack )
		{
		case EMPEACE_WEAPON_RSLOT:
		case EMPEACE_WEAPON_WHAL_SLOT:
		case EMPEACE_WEAPON_STICK_RSLOT:
			m_pSlotHeld = pSkeleton->FindBone ( SKINTYPE_STING::m_szWHEREBACKTYPE[m_emWeaponWhereBack] );
			break;
		};

		// Note : 활이 등에 붙는 위치의 이름이 바뀌었는데 .. 예전 이름도 아직 사용중이라서 이렇게 해야한다. < 극강부 들어가면 삭제해야함 >
		if( !m_pSlotHeld && (m_emWeaponWhereBack==EMPEACE_WEAPON_WHAL_SLOT) )
		{
			m_pSlotHeld = pSkeleton->FindBone ( _T("slot_lhand") );
		}

		if ( m_bAttackMode && m_pHandHeld )			m_pmcMesh->ppBoneMatrixPtrs[2] = &m_pHandHeld->matCombined;
		else if ( !m_bAttackMode && m_pSlotHeld )	m_pmcMesh->ppBoneMatrixPtrs[2] = &m_pSlotHeld->matCombined;

		// Note : 일반 메쉬인 경우 매트릭스를 미리 계산해 놓는다.
		D3DXMatrixMultiply( &m_matCombine, m_pmcMesh->ppBoneMatrixPtrs[1], m_pmcMesh->ppBoneMatrixPtrs[2] );
		D3DXMatrixMultiply( &m_matCombine, m_pmcMesh->ppBoneMatrixPtrs[0], &m_matCombine );
	}

	//	Note : Setting State.
	//
	if ( !bShadow )
	{
		size_t nSIZE = m_vecEFFECT.size();
		for ( size_t n=0; n<nSIZE; ++n )
		{
			DxEffChar* pEFFECT = m_vecEFFECT[n];
			if ( pEFFECT->GetStateOrder() > EMEFFSO_RENDERSTATE )	break;

			pEFFECT->SettingState ( pd3dDevice, sSKINEFFDATA );
		}
	}

	//	Note : 캐릭터 텍스쳐 파일에 스펙큘러 맵이 들어 있을 경우 1번만 뿌리기 위해서 기본 조각파일을 뿌리지 않는다.
	//
	if ( bOriginDraw )
	{
		DrawMeshContainer( pd3dDevice, bOriginDraw, bAlpha, bShadow, bAlphaTex );
	}

	//	Note : 기본 캐릭터 출력시 trace point calc, render effect skin 를 행한다.
	//
	if ( !bShadow && bEff )
	{
		size_t nSize = m_vecTrace.size();
		for ( size_t n=0; n<nSize; ++n )
		{
			STRACOR &sTracor = m_vecTrace[n];

			m_pmcMesh->CalculateVertexInfluences ( &sTracor.m_sVertexInflu, sTracor.m_vPos, sTracor.m_vNormal, &sTracor.m_BoneMatrice, FALSE );
		}

		//	Note : render effect skin
		//
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_NORMAL, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_NORMAL )	break;	//	EMEFFSO_NORMAL이 아니면 중단.

			switch( RENDERPARAM::emCharRenderTYPE )
			{
			case EMCRT_SOFTWARE:
			case EMCRT_NORMAL:
			case EMCRT_VERTEX:
				if( pEFFECT->GetFlag() & EMECF_NORMALMAP )	continue;	// Pixel Shader 안 쓴다고 셋팅해 놓으면 작동 안한다.
				break;
			case EMCRT_PIXEL:
				//if( pEFFECT->GetFlag() & EMECF_NORMALMAP )	continue;	// Pixel Shader 안 쓴다고 셋팅해 놓으면 작동 안한다.
				break;
			}

			if( bCrashRender )
			{
				switch( RENDERPARAM::emCharRenderTYPE )
				{
				case EMCRT_SOFTWARE:
				case EMCRT_NORMAL:
				case EMCRT_VERTEX:
					if( pEFFECT->GetFlag() & EMECF_NORMALMAP )	continue;	// Pixel Shader 안 쓴다고 셋팅해 놓으면 작동 안한다.
					break;
				case EMCRT_PIXEL:
					if( pEFFECT->GetFlag() & EMECF_NONHLSL )	continue;	// 충돌이 일어나는 것을 뿌리지 않는다.
					break;
				}
			}

			pEFFECT->Render ( pd3dDevice );
		}
	}

	//	Note : Setting State.
	//
	if ( !bShadow )
	{
		GLEFFCHAR_VEC_RITER pos = std::lower_bound ( m_vecEFFECT.rbegin(), m_vecEFFECT.rend(), EMEFFSO_RENDERSTATE, DXEFFCHAR_DISOPER() );
		for ( ; pos!=m_vecEFFECT.rend(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);

			pEFFECT->RestoreState ( pd3dDevice );
		}
	}

	return S_OK;
}

HRESULT DxCharPart::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton, float fHeight )
{
	if ( !m_pmcMesh )				return S_OK;
	if ( !m_bRender )				return S_OK;
	if( !(m_dwFlag&EMCF_SHADOW) )	return S_OK;	// 그림자 옵션이 없으면 끈다.

	DrawMeshContainer ( pd3dDevice, TRUE, FALSE, TRUE, TRUE, fHeight );

	return S_OK;
}

void DxCharPart::RenderGlow ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton )
{
	if ( !m_pmcMesh )	return;
	if ( !m_bRender )	return;

	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_GLOW, DXEFFCHAR_OPER() );
	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > EMEFFSO_GLOW )	break;	//	EMEFFSO_GLOW이 아니면 중단.

		pEFFECT->Render ( pd3dDevice );
	}
}

void DxCharPart::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton, const float fScale )
{
	if( !m_pmcMesh )					return;
	if( !m_bRender )					return;
	if( !m_pmcMesh->ppBoneMatrixPtrs )	return;

	// Note : 일반 메쉬이면 계산 한 것을 가지고 뿌려준다.
	LPD3DXMATRIX pMatrix(NULL);
	if( !m_pmcMesh->pSkinInfo )	pMatrix = &m_matCombine;

	// Note : Render
	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_SINGLE_EFF, DXEFFCHAR_OPER() );
	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > EMEFFSO_SINGLE_EFF )	break;	//	EMEFFSO_SINGLE_EFF이 아니면 중단.

		pEFFECT->SetSkeleton( pSkeleton );
		pEFFECT->RenderEff( pd3dDevice, pMatrix, fScale );
	}
}

HRESULT DxCharPart::RenderGhosting ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinAniControl* pSkinAniControl, const D3DXMATRIX &_matCurAni )
{
	//	Note : render effect skin
	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_GHOSTING, DXEFFCHAR_OPER() );
	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > EMEFFSO_GHOSTING )	break;	//	EMEFFSO_GHOSTING이 아니면 중단.

		pEFFECT->SetAniControl ( pSkinAniControl, _matCurAni );
		pEFFECT->Render ( pd3dDevice );
	}

	return S_OK;
}

SVERTEXINFLU* DxCharPart::GetTracePos ( std::string strTrace )
{
	VECTRACE_ITER found = std::lower_bound ( m_vecTrace.begin(), m_vecTrace.end(), strTrace.c_str(),STRACOR_OPER() );
	if ( found == m_vecTrace.end() )						return NULL;

	if ( !strcmp((*found).m_szName,strTrace.c_str()) )		return &(*found).m_sVertexInflu;
	return NULL;
}

STRACOR* DxCharPart::GetTracor ( std::string strTrace )
{
	VECTRACE_ITER found = std::lower_bound ( m_vecTrace.begin(), m_vecTrace.end(), strTrace.c_str(),STRACOR_OPER() );
	if ( found == m_vecTrace.end() )						return NULL;

	if ( !strcmp((*found).m_szName,strTrace.c_str()) )		return &(*found);

	return NULL;
}

//	pBoneMatrice -> (주의) 영향을 받는 본의 갯수가 여러개일 경우에 문제가 생김.
//	랜더시 미리 계산되어 있으므로 값을 꺼내서 반환.
HRESULT DxCharPart::CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice )
{
	if ( strTrace.empty() ) return E_FAIL;

	STRACOR* pTracor = GetTracor(strTrace);
	if ( !pTracor )		return E_FAIL;
	if ( !m_pmcMesh )	return E_FAIL;

	vVert = pTracor->m_vPos;
	vNormal = pTracor->m_vNormal;
	if ( pBoneMatrice )	*pBoneMatrice = pTracor->m_BoneMatrice;

	return S_OK;
}
