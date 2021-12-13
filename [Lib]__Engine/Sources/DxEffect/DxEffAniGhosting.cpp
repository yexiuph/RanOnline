#include "pch.h"

#include "./editmeshs.h"

#include "./DxEffectMan.h"

#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxSkinChar.h"
#include "./DxWeatherMan.h"

#include "./DxEffAniGhosting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffAniData_Ghosting::TYPEID			= EMEFFANI_GHOSTING;
DWORD		DxEffAniData_Ghosting::VERSION			= 0x0100;
char		DxEffAniData_Ghosting::NAME[64]	= "Ghosting EFFECT";

DxEffAni* DxEffAniData_Ghosting::NEWOBJ( SAnimContainer* pAnimContainer )
{
	DxEffAniGhosting*	pEff = new DxEffAniGhosting;

	pEff->m_dwFlags			= m_dwFlags;
	pEff->m_dwFrame_1		= m_dwFrame_1;
	pEff->m_dwFrame_2		= m_dwFrame_2;
	pEff->m_dwGhostingNum	= m_dwGhostingNum;
	pEff->m_dwAlphaMax		= m_dwAlphaMax;
	pEff->m_dwUnit			= m_dwUnit;
	pEff->m_dwColor			= m_dwColor;

	pEff->Create ( m_pd3dDevice, pAnimContainer );

	return	pEff;
}

HRESULT	DxEffAniData_Ghosting::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize, dwTypeID;

	SFile >> dwTypeID;
	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	if ( pd3dDevice )
	{
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

HRESULT	DxEffAniData_Ghosting::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

BOOL DxEffAniData_Ghosting::CheckEff ( DWORD dwCurKeyTime, DWORD dwPrevFrame )
{
	if ( !m_bCreated )			// 다음에는 현재 효과를 또 생성하지 않는다.
	{
		m_bCreated = TRUE;

		return TRUE;
	}

	return FALSE;		
}

//	-----------------------------------------------	--------------------------------------------------

DxEffAniGhosting::DxEffAniGhosting(void) :
	DxEffAni(),
	m_pd3dDevice(NULL),
	m_pSavedBaseSB(NULL),
	m_pEffectBaseSB(NULL),
	m_nPrevKeyTime(-1)
{
}

DxEffAniGhosting::~DxEffAniGhosting(void)
{
	m_pd3dDevice = NULL;

	m_listGhosting.clear();

	CleanUp ();
}

//DxEffAni* DxEffAniGhosting::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	HRESULT hr;
//	DxEffAniGhosting *pEffAni = new DxEffAniGhosting;
//	pEffAni->SetProperty ( &m_Property );
//
//	hr = pEffAni->Create ( pd3dDevice );
//	if ( FAILED(hr) )
//	{
//		SAFE_DELETE(pEffAni);
//		return NULL;
//	}
//
//	return pEffAni;
//}

HRESULT DxEffAniGhosting::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniGhosting::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniGhosting::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffAniGhosting::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffAniGhosting::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pd3dDevice )	return S_OK;

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	m_pd3dDevice = pd3dDevice;

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

HRESULT DxEffAniGhosting::InvalidateDeviceObjects ()
{
	SAFE_RELEASE( m_pSavedBaseSB );
	SAFE_RELEASE( m_pEffectBaseSB );

	return S_OK;
}

HRESULT DxEffAniGhosting::DeleteDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffAniGhosting::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxEffAniGhosting::FrameMove ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT	DxEffAniGhosting::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pSkinChar, const D3DXMATRIX& matCurPos )
{
	HRESULT hr = S_OK;
	D3DXVECTOR3 vNormalUp;
	D3DXVECTOR3 vNormalDown;
	D3DXMATRIX	matWorld;
	D3DXMatrixIdentity ( &matWorld );

	DWORD	dwUnit;
	float	fAlpha;
	DWORD	GHOSTMAX	= m_dwGhostingNum*m_dwUnit;
	DWORD	dwColor		= 0xffffffff;
	DWORD	dwUnitTime	= pSkinChar->GETUNITTIME();
	float	fOldCurTime	= pSkinChar->GETCURTIME ();
	DWORD	dwKeyTime	= pSkinChar->GETCURKEYTIME();

	// Note : 렌더할 고스팅이 없다면...
	if( !m_listGhosting.empty() )
	{
		DxSkinAniControl* pSkinAniControl = pSkinChar;			// 이렇게 써도 되는지.. ㅡ,.ㅡ;

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

				m_pEffectBaseSB->Apply();	// State Block 셋팅.

				//	Note : Skin Animation.
				pSkinChar->SETCURTIME ( (float)(iter->m_dwKeyTime*dwUnitTime) );
				pSkinAniControl->Render ( iter->m_matCurAni );

				fAlpha = (float)(GHOSTMAX-iter->m_UseNum);

				dwColor		=	m_dwColor;
				dwColor		+= (DWORD)((fAlpha/GHOSTMAX)*m_dwAlphaMax)<<24;
				pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, dwColor );



				CHARSETTING sCharSetting;
				sCharSetting.pMeshContainerBase = pSkinChar->m_PartArray[PIECE_HEAD].m_pmcMesh;
				sCharSetting.pmtrlPiece			= pSkinChar->m_PartArray[PIECE_HEAD].m_pMaterialPiece;
				sCharSetting.pmtrlSpecular		= NULL;
				sCharSetting.bWorldIdentity		= FALSE;
				sCharSetting.emRDOP				= CTOP_BASE;

				switch( pSkinChar->m_sSkinEffData.m_bNewHeadDRAW )
				{
				case TRUE:
					if( !pSkinChar->m_PartArray[PIECE_HEAD].m_pSkinMesh )	break;
					if( !pSkinChar->m_PartArray[PIECE_HEAD].m_pmcMesh )		break;

					pd3dDevice->SetTexture( 0, pSkinChar->m_sSkinEffData.m_rFaceTex );
					sCharSetting.emRDOP		= CTOP_NEWTEX;
					pSkinChar->m_PartArray[PIECE_HEAD].m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting );
					break;

				case FALSE:
					if( !pSkinChar->m_PartArray[PIECE_HEAD].m_pSkinMesh )	break;
					if( !pSkinChar->m_PartArray[PIECE_HEAD].m_pmcMesh )		break;

					sCharSetting.emRDOP		= CTOP_BASE;
					pSkinChar->m_PartArray[PIECE_HEAD].m_pSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
					pSkinChar->m_PartArray[PIECE_HEAD].m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting );
					break;
				}

				for ( int i=1; i<PIECE_REV01; i++ )
				{
					if ( !pSkinChar->m_PartArray[i].m_pSkinMesh )	continue;
					if ( !pSkinChar->m_PartArray[i].m_pmcMesh )		continue;

					CHARSETTING sCharSetting;
					sCharSetting.pMeshContainerBase = pSkinChar->m_PartArray[i].m_pmcMesh;
					sCharSetting.pmtrlPiece			= pSkinChar->m_PartArray[i].m_pMaterialPiece;
					sCharSetting.pmtrlSpecular		= NULL;
					sCharSetting.bWorldIdentity		= FALSE;
					sCharSetting.emRDOP				= CTOP_BASE;

					pSkinChar->m_PartArray[i].m_pSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
					pSkinChar->m_PartArray[i].m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting );
				}
			}
		}
		m_pSavedBaseSB->Apply();

		pSkinChar->SETCURTIME ( fOldCurTime );
	}

	
	if ( m_nPrevKeyTime != dwKeyTime )
	{
		// Note : Count 를 올려서 한계에 차면 삭제시킴.
		GHOSTINGLIST_ITER	iter = m_listGhosting.begin();
		GHOSTINGLIST_ITER	iter_del;
		for ( ; iter!=m_listGhosting.end(); )
		{
			iter->m_UseNum += 1;

			iter_del = iter++;

			if ( iter->m_UseNum >= GHOSTMAX )
			{
				m_listGhosting.erase ( iter_del );
			}
		}

		m_nPrevKeyTime = dwKeyTime;

		// 새로운 고스팅을 생성해도 될 경우.
		if( m_bFrameMove )
		{
			if ( (m_dwFrame_1<=dwKeyTime) && (dwKeyTime<=m_dwFrame_2) )		// 범위 안에 들어오면 생성
			{
				// 새로운 것을 위한 작업 시작
				GHOSTING_INFORMATION	sGhost_Information;

				sGhost_Information.m_Use		= FALSE;
				sGhost_Information.m_UseNum		= 0;
				sGhost_Information.m_dwKeyTime	= dwKeyTime;
				sGhost_Information.m_matCurAni	= matCurPos;

				m_listGhosting.push_back ( sGhost_Information );
			}
		}
		else
		{
			// 임시 수정판... 이상하게.. 위쪽의 erase가 제대로 안되는 것 같다.
			m_listGhosting.clear();
		}

		// Note : 뿌려야 하는 때를 셋팅한다.
		iter = m_listGhosting.begin();
		for ( ; iter!=m_listGhosting.end(); ++iter )
		{
			dwUnit = iter->m_UseNum + 1;
			dwUnit = dwUnit%m_dwUnit;
			if ( dwUnit )	iter->m_Use = FALSE;
			else			iter->m_Use = TRUE;
		}
	}

	return S_OK;
}