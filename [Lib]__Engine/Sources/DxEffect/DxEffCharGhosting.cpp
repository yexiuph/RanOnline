#include "pch.h"
#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxViewPort.h"
#include "./DxEffectMan.h"

#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./DxEffCharGhosting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharGhosting::TYPEID			= EMEFFCHAR_GHOSTING;
DWORD		DxEffCharGhosting::VERSION			= 0x0101;
char		DxEffCharGhosting::NAME[MAX_PATH]	= "1.고스팅";

LPDIRECT3DSTATEBLOCK9		DxEffCharGhosting::m_pSavedBaseSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharGhosting::m_pEffectBaseSB		= NULL;

DxEffCharGhosting::DxEffCharGhosting(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_dwPrevKeyTime(0)
{
}

DxEffCharGhosting::~DxEffCharGhosting(void)
{
	m_listGhosting.clear();
}

DxEffChar* DxEffCharGhosting::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharGhosting *pEffChar = new DxEffCharGhosting;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharGhosting::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR,		0xffffffff );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedBaseSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectBaseSB );
	}

	return S_OK;
}

HRESULT DxEffCharGhosting::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedBaseSB );
	SAFE_RELEASE( m_pEffectBaseSB );

	return S_OK;
}

HRESULT DxEffCharGhosting::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_pSkinMesh;
		m_pmcMesh = m_pCharPart->m_pmcMesh;
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_pSkinMesh;
		m_pmcMesh = m_pSkinPiece->m_pmcMesh;
	}

	return S_OK;
}

HRESULT DxEffCharGhosting::DeleteDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharGhosting::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	if ( m_CurAniMType != m_AniMType || m_CurAniSType != m_AniSType )
	{
		m_listGhosting.clear();
		SetEnd();
	}

	return S_OK;
}

void DxEffCharGhosting::Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	if ( !m_pmcMesh )						return;
	if ( !m_pSkinMesh )						return;

	SMATERIAL_PIECE*	pMaterialPiece = NULL;
	if ( m_pCharPart )			pMaterialPiece = m_pCharPart->m_pMaterialPiece;
	else if ( m_pSkinPiece )	pMaterialPiece = m_pSkinPiece->m_pMaterialPiece;

	GASSERT(m_pSkinAniControl&&"스킨에니메이션 콘트롤러가 초기화 되지 않고 호출되었습니다.");

	DWORD	dwUnit;
	float	fAlpha;
	DWORD	GHOSTMAX	= m_dwGhostingNum*m_dwUnit;
	DWORD	dwColor		= 0xffffffff;
	DWORD	dwUnitTime	= m_pSkinAniControl->GETUNITTIME();
	float	fOldCurTime	= m_pSkinAniControl->GETCURTIME ();

	//	Render 시작
	m_pSavedBaseSB->Capture();
	
	{
		if ( m_dwColor == 0x00ffffff )		// 흰색일 경우 색의 변화는 주지 않는다.
		{
			pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );
		}

		GHOSTINGLIST_ITER iter = m_listGhosting.begin();
		for ( ; iter!=m_listGhosting.end(); ++iter )
		{
			if ( !(iter->m_Use) )		continue;

			m_pEffectBaseSB->Apply();	// StateBlock을 셋팅 한다.

			//	Note : Skin Animation.
			m_pSkinAniControl->SETCURTIME ( (float)(iter->m_dwKeyTime*dwUnitTime) );
			m_pSkinAniControl->Render ( iter->m_matCurAni );

			fAlpha = (float)(GHOSTMAX-iter->m_UseNum);

			dwColor		=	m_dwColor;
			dwColor		+= (DWORD)((fAlpha/GHOSTMAX)*m_dwAlphaMax)<<24;
			pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, dwColor );

			CHARSETTING sCharSetting;
			sCharSetting.pMeshContainerBase = m_pmcMesh;
			sCharSetting.pmtrlPiece			= pMaterialPiece;
			sCharSetting.pmtrlSpecular		= NULL;
			sCharSetting.bWorldIdentity		= FALSE;
			sCharSetting.emRDOP				= CTOP_BASE;

			m_pSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
			m_pSkinMesh->DrawMeshContainer ( pd3dDevice, sCharSetting );
		}
	}
	m_pSavedBaseSB->Apply();

	m_pSkinAniControl->SETCURTIME ( fOldCurTime );




	if ( m_dwPrevKeyTime != m_dwKeyTime )
	{
		GHOSTINGLIST_ITER	iter = m_listGhosting.begin();
		for ( ; iter!=m_listGhosting.end(); )
		{
			iter->m_UseNum += 1;

			GHOSTINGLIST_ITER	iter_del = iter;
			++iter;

			if ( iter->m_UseNum == GHOSTMAX )
			{
				m_listGhosting.erase ( iter_del );
			}
		}

		// 새로운 것을 위한 작업 시작
		GHOSTING_INFORMATION	sGhost_Information;

		sGhost_Information.m_Use		= FALSE;
		sGhost_Information.m_UseNum		= 0;
		sGhost_Information.m_dwKeyTime	= m_dwKeyTime;
		sGhost_Information.m_matCurAni	= m_matCurAni;

		m_listGhosting.push_back ( sGhost_Information );

		m_dwPrevKeyTime = m_dwKeyTime;

		iter = m_listGhosting.begin();
		for ( ; iter!=m_listGhosting.end(); ++iter )
		{
			dwUnit = iter->m_UseNum + 1;
			dwUnit = dwUnit%m_dwUnit;
			if ( dwUnit )	iter->m_Use = FALSE;
			else			iter->m_Use = TRUE;
		}
	}
}

HRESULT	DxEffCharGhosting::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0100 )
	{
		EFFCHAR_PROPERTY_GHOSTING_100	sProp;
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &sProp, sizeof(EFFCHAR_PROPERTY_GHOSTING_100) );

		m_Property.m_dwFlag		= sProp.m_dwFlag;
		m_Property.m_dwGhostingNum = sProp.m_dwGhostingNum;
		m_Property.m_dwUnit		= sProp.m_dwUnit;
		m_Property.m_dwAlphaMax = sProp.m_dwAlphaMax;
		m_Property.m_dwColor	= sProp.m_dwColor;

		m_Property.m_AniMType	= AN_SKILL;
		m_Property.m_AniSType	= AN_SUB_NONE;

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharGhosting::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}