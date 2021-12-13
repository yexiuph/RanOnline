#include "pch.h"
#include "./DxLobyStage.h"

#include "NETCOMMENTS.h"
#include "DxGlobalStage.h"

#include "DxEffectMan.h"
#include "DxEnvironment.h"
#include "DxSurfaceTex.h"
#include "DxGlowMan.h"
#include "DxPostProcess.h"
#include "DxCubeMap.h"
#include "DxSkyMan.h"
#include "DxEffGroupPlayer.h"

#include "DxDynamicVB.h"	// ���� �ؾ��Ѵ�.
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"
#include "../[Lib]__RanClientUI/Sources/InnerUI/ModalCallerID.h"
#include "../[Lib]__RanClientUI/Sources/ControlExUI/ModalWindow.h"
#include "DxRenderStates.h"

#include "../[Lib]__RanClientUI/Sources/OuterUI/SelectCharacterPage.h"
#include "../[Lib]__RanClientUI/Sources/OuterUI/OuterInterface.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLPeriod.h"
#include "../[Lib]__Engine/Sources/DxTools/DxViewPort.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__Engine/Sources/DxTools/DxInputDevice.h"
#include "../[Lib]__Engine/Sources/DxTools/DxLightMan.h"

#include "../[Lib]__EngineSound/Sources/DxSound/BgmSound.h"


//#include "../[Lib]__EngineUI/Sources/UIRenderQueue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note : ĳ���� ����/�� ������
//	���� : ������ ĳ���Ͱ� ���õǴ� ��������
//		   ������ �Է��ؾ��Ѵ�.
namespace
{
	std::string g_strCAMERA[GLCI_NUM+1] =
	{
		"04",	//	����.
		"01",	//	�˵�.
		"02",	//	���.
		"03",	//	�⿹.
        "main",	//	�̼���.
	};

	std::string g_strCAMERAEX[GLCI_NUM_EX+1] =
	{
		"04",	//	����.
		"01",	//	�˵�.
		"02",	//	���.
		"03",	//	�⿹.
		"05",	//	�ذ��γ�
		"06",	//	�ذ��ο�
		"main",	//	�̼���.
	};

	DWORD g_dwCLASS[GLCI_NUM_EX] =
	{
		3,	//	����.
		0,	//	�˵�.
		1,	//	���.
		2,	//	�⿹.
		4,	//	�ذ��γ�
		5,	//	�ذ��ο�
	};
};

/*
namespace HAIRCOLOR
{
	enum { MAXHAIRCOLOR = 8 };

	WORD wHairColor[GLCI_NUM_NEWSEX][MAXHAIRCOLOR] = 
	{ 
		22923, 30719, 32486, 24447, 32258, 32594, 31743, 32767,	// �����γ�
		30719, 22923, 32486, 24447, 32258, 32594, 31743, 32767,	// �˵��γ�
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// ��úο�
		30278, 18008, 31503, 24004, 32488, 27245, 28364, 0,		// �⿹�ο�
		22923, 30719, 32486, 24447, 32258, 32594, 31743, 32767,	// �ذ��γ�
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// �ذ��ο�
		30278, 18008, 31503, 24004, 32488, 27245, 28364, 0,		// �����ο�
		18008, 30278, 31503, 24004, 32488, 27245, 28364, 0,		// �˵��ο�
		30719, 22923, 32486, 24447, 32258, 32594, 31743, 32767,	// ��úγ�
		22923, 30719, 32486, 24447, 32258, 32594, 31743, 32767,	// �⿹�γ�
	};

	WORD GetHairColor ( WORD wClass, WORD wStyle ) { return wHairColor[wClass][wStyle]; }
};

*/

namespace SEXUAL
{
	enum SEX 
	{
		SEX_MAN   = 1,
		SEX_WOMAN = 0
	};

	SEX GetSex ( EMCHARINDEX emIndex ) 
	{
		if( (WORD)emIndex == GLCI_FIGHTER_M || 
			(WORD)emIndex == GLCI_ARMS_M || 
			(WORD)emIndex == GLCI_ARCHER_M || 
			(WORD)emIndex == GLCI_SPIRIT_M || 
			(WORD)emIndex == GLCI_EXTREME_M )
			return SEX_MAN;
		
		return SEX_WOMAN;
	}
}

DxLobyStage::DxLobyStage(void) :
	m_nStartCharNum(0),
	m_nStartCharLoad(0),
	m_nChaSNum(0),
	m_dwCharSelect(MAX_SELECTCHAR),
	m_bGameJoin(FALSE),
	m_nCharStart(0),
	m_fShiftTime(0),
	m_bShiftButton(FALSE),
	m_bShiftLBDown(FALSE),
	m_bShiftRBDown(FALSE),
	m_bUseInterface(TRUE),
	m_bRenderSkip(FALSE),
	m_fRenderSkipTime(0.0f),
	m_bStyle( false ),
	m_vDir ( D3DXVECTOR3( -1.0f,-10.0f,-0.35f) ),
	m_pBackground ( NULL ),
	m_strTextureName( "GUI_001.dds")
{
	
}

DxLobyStage::~DxLobyStage(void)
{
}

HRESULT DxLobyStage::ReSizeWindow ( WORD wWidth, WORD wHeight )
{
	//	Note : �������̽� �ʱ�ȭ.
	//
	COuterInterface::GetInstance().SetResolution (MAKELONG(wHeight,wWidth));

	InvalidateDeviceObjects();
	RestoreDeviceObjects();

	return S_OK;
}

void DxLobyStage::OnInitLogin ()
{
	m_nStartCharLoad = 0;
	m_nChaSNum = 0;
	m_bGameJoin = FALSE;
	m_dwCharSelect = MAX_SELECTCHAR;
	m_nCharStart = 0;
	m_bGameJoin = FALSE;
	m_fShiftTime = 0;
	m_bShiftButton = FALSE;
	m_bShiftLBDown = FALSE;
	m_bShiftRBDown = FALSE;
	m_bUseInterface = TRUE;

	for ( int i=0; i<MAX_SERVERCHAR; i++ ) m_CharInfo[i] = SCHARINFO_LOBBY();
	for ( int i=0; i<MAX_SELECTCHAR; i++ ) m_CharSham[i].ResetCharInfo ();

	m_NewCharSham.ResetCharInfo();
}

SCHARINFO_LOBBY* DxLobyStage::GetSelectCharInfo ()
{
	if ( m_dwCharSelect >= MAX_SELECTCHAR )	
		return NULL;

	return &(m_CharInfo[m_dwCharSelect+m_nCharStart]);
}

DxLandMan* DxLobyStage::GetCharNewLand ()
{
	switch ( m_sCharNewInfo.m_wSchool )
	{
	case 0: m_LandCharSlt_s01.ActiveMap ();	return &m_LandCharSlt_s01;
	case 1:	m_LandCharSlt_s02.ActiveMap ();	return &m_LandCharSlt_s02;
	case 2:	m_LandCharSlt_s03.ActiveMap ();	return &m_LandCharSlt_s03;

	default:
		GASSERT(0&&"���� ������ �ɸ����� �б������� 0~2 ���� �̳��� ���� �ʽ��ϴ�." );
		break;
	};

	return NULL;
}

WORD DxLobyStage::ShiftCharClass ( EMSHIFT emShift, WORD wClassMax )
{
	WORD nRet(wClassMax);

	switch ( emShift )
	{
	case EM_PREV:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_emIndex;
			if ( (WORD)m_sCharNewInfo.m_emIndex > 0 )			wIndex--;
			else												wIndex = wClassMax;
			SelCharClass ( wIndex );

			nRet = wIndex;
		}
		break;

	case EM_NEXT:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_emIndex;
			if ( (WORD)m_sCharNewInfo.m_emIndex < wClassMax )	wIndex++;
			else												wIndex = 0;
			SelCharClass ( wIndex );

			nRet = wIndex;
		}
		break;
	};

	return nRet;
}

void DxLobyStage::ShiftCharSchool ( EMSHIFT emShift )
{
	switch ( emShift )
	{
	case EM_PREV:
		{
			WORD wIndex = m_sCharNewInfo.m_wSchool;
			if ( wIndex>0 )										wIndex--;
			else												wIndex = (GLCONST_CHAR::wSCHOOLNUM-1);
			SelCharSchool ( wIndex );
		}
		break;

	case EM_NEXT:
		{
			WORD wIndex = m_sCharNewInfo.m_wSchool;
			if ( (WORD)wIndex < (GLCONST_CHAR::wSCHOOLNUM-1) )	wIndex++;
			else												wIndex = 0;
			SelCharSchool ( wIndex );
		}
		break;
	};
}

void DxLobyStage::ShiftCharFace ( EMSHIFT emShift )
{
	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[m_sCharNewInfo.m_emIndex];

	switch ( emShift )
	{
	case EM_PREV:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_wFace;
			if ( wIndex > 0 )					wIndex--;
			else								wIndex = (WORD) (sCONST.dwHEADNUM_SELECT-1);
			
			SelCharFace ( wIndex );
		}
		break;

	case EM_NEXT:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_wFace;
			if ( wIndex < WORD(sCONST.dwHEADNUM_SELECT-1) )	wIndex++;
			else											wIndex = 0;

			SelCharFace ( wIndex );
		}
		break;
	};
}

void DxLobyStage::ShiftCharHair ( EMSHIFT emShift )
{
	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[m_sCharNewInfo.m_emIndex];

	switch ( emShift )
	{
	case EM_PREV:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_wHair;
			if ( wIndex > 0 )					wIndex--;
			else								wIndex = (WORD) (sCONST.dwHAIRNUM_SELECT-1);

			WORD wHairColor = HAIRCOLOR::GetHairColor ( m_sCharNewInfo.m_emIndex, wIndex );
			
			SelCharHair ( wIndex );
			SelCharHairColor ( wHairColor );
		}
		break;

	case EM_NEXT:
		{
			WORD wIndex = (WORD)m_sCharNewInfo.m_wHair;
			if ( wIndex < WORD(sCONST.dwHAIRNUM_SELECT-1) )	wIndex++;
			else											wIndex = 0;

			WORD wHairColor = HAIRCOLOR::GetHairColor ( m_sCharNewInfo.m_emIndex, wIndex );

			SelCharHair ( wIndex );
			SelCharHairColor ( wHairColor );
		}
		break;
	};
}

//void DxLobyStage::ToCameraPos ( int n )
//{
//	ShiftCharClass ( (EMSHIFT)n );
//}

void DxLobyStage::SelCharClass ( WORD wIndex )
{
	int nClassMax = GLCI_NUM;
	if( COuterInterface::GetInstance().IsCreateExtream() )
		nClassMax = GLCI_NUM_EX;

//	GASSERT(wIndex<(nClassMax+1));
	
	//	�ɸ��� ����.
	m_sCharNewInfo.m_emIndex = (EMCHARINDEX) wIndex;

	m_sCharNewInfo.m_wHair = 0;
	m_sCharNewInfo.m_wFace = 0;
	m_sCharNewInfo.m_wHairColor = HAIRCOLOR::GetHairColor ( m_sCharNewInfo.m_emIndex, 0 );

	// ���� ����
	m_sCharNewInfo.m_wSex = SEXUAL::GetSex ( m_sCharNewInfo.m_emIndex );

// ĳ���� ���� Test 
/*
#if !defined( _RELEASED ) && !defined( TW_PARAM ) && !defined( KRT_PARAM ) && !defined( KR_PARAM ) && !defined( VN_PARAM ) && !defined( TH_PARAM ) && !defined( MYE_PARAM ) && !defined( MY_PARAM ) && !defined( ID_PARAM ) && !defined( CH_PARAM ) && !defined( PH_PARAM ) && !defined( HK_PARAM ) && !defined( JP_PARAM ) && !defined( GS_PARAM )
	//	Note : Camera ����.
	//
	GASSERT(m_sCharNewInfo.m_emIndex<(nClassMax+1));
	DxLandMan* pLandMan = GetCharNewLand ();
	if ( !pLandMan )	return;

	DxCamAniMan* pCamAniMan = pLandMan->GetCamAniMan();


	if( COuterInterface::GetInstance().IsCreateExtream() )
	{
		pCamAniMan->ActiveCameraAni ( (char*)::g_strCAMERAEX[m_sCharNewInfo.m_emIndex].c_str() );
	}
	else
	{
		pCamAniMan->ActiveCameraAni ( (char*)::g_strCAMERA[m_sCharNewInfo.m_emIndex].c_str() );
	}
#endif
*/
}

void DxLobyStage::SelCharSchool ( WORD wIndex )
{
	//	�б� ����.
	GASSERT(wIndex<GLCONST_CHAR::wSCHOOLNUM);
	m_sCharNewInfo.m_wSchool = (WORD)wIndex;

	int nClassMax = GLCI_NUM;
	if( COuterInterface::GetInstance().IsCreateExtream() )
		nClassMax = GLCI_NUM_EX;

/*
// ĳ���� ���� Test 
#if !defined( _RELEASED ) && !defined( TW_PARAM ) && !defined( KRT_PARAM ) && !defined( KR_PARAM ) && !defined( VN_PARAM ) && !defined( TH_PARAM ) && !defined( MYE_PARAM ) && !defined( MY_PARAM ) && !defined( ID_PARAM ) && !defined( CH_PARAM ) && !defined( PH_PARAM ) && !defined(HK_PARAM) && !defined( JP_PARAM ) && !defined( GS_PARAM )
	DxLandMan *pLandMan = GetCharNewLand();
	if ( pLandMan )
	{
		for( int j=GLCI_EXTREME_M; j<GLCI_NUM_EX; ++j )
		{
			CString strSkinObj;
			strSkinObj.Format( "s%02d_%02d.chf", m_sCharNewInfo.m_wSchool+1, g_dwCLASS[j]+1 );

			PLANDSKINOBJ pLandSkinObj = pLandMan->FindLandSkinObjByFileName(strSkinObj.GetString());
			if( pLandSkinObj )		
			{
				if( nClassMax == GLCI_NUM )
					pLandSkinObj->m_bRender = FALSE;
				else
					pLandSkinObj->m_bRender = TRUE;
			}
		}
	}
#endif
*/

	//	���õ� �ɸ��� Ŭ���� ����.
	SelCharClass( (WORD)nClassMax);

	m_sCharNewInfo.m_wHair = 0;
	m_sCharNewInfo.m_wFace = 0;
}

void DxLobyStage::DelChar ( int nCharID )
{
	//	Note : ���� ĳ���� ���� ���Ž�.
	int nIndex = -1;
	for ( int i=0; i<m_nChaSNum; i++ )
	{
		if ( m_nChaIDs[i]== nCharID )	nIndex = i;
	}
	if ( nIndex == -1 )		return;

	//	Note : ĳ���� ������ ���.
	m_dwCharSelect = MAX_SELECTCHAR;

	//	Note : �����Ȱ� ����� ������.
	for (int i=nIndex+1; i<m_nChaSNum; i++ )
	{
		m_nChaIDs[i-1] = m_nChaIDs[i];
		m_CharInfo[i-1] = m_CharInfo[i];
	}
	m_nChaSNum--;

	//	Note : ���÷��� �ɸ��� ���� ����.
	for( int i=0; i<MAX_SELECTCHAR; i++ )
	{
		m_CharSham[i].ResetCharInfo ();
		m_CharSham[i].SetSelect ( FALSE );

		InitRenderSkip();
	}

	if( m_nCharStart >= m_nChaSNum )
	{
		ShiftLBDown();
		return;
	}

	for( int i=m_nCharStart, j=0; i<m_nChaSNum; i++, j++ )
	{
		if( j >= MAX_SELECTCHAR ) 
			break;

		m_CharSham[j].SetCharInfo( m_CharInfo[i], FALSE );
	}
}

BOOL DxLobyStage::ShiftRightChar()
{
	if( (m_nCharStart+MAX_SELECTCHAR) < m_nChaSNum )
	{
		m_nCharStart += MAX_SELECTCHAR;
		return TRUE;
	}

	return FALSE;
}

BOOL DxLobyStage::ShiftLeftChar()
{
	if( (m_nCharStart-MAX_SELECTCHAR) >= 0 )
	{
		m_nCharStart -= MAX_SELECTCHAR;
		return TRUE;
	}

	return FALSE;
}

void DxLobyStage::ShiftCharInfo()
{
	for ( int i=m_nCharStart, j=0; i<m_nChaSNum; i++, j++ )
	{
		if( j >= MAX_SELECTCHAR ) 
			break;

		m_CharSham[j].SetCharInfo( m_CharInfo[i], FALSE );
	}
}

void DxLobyStage::ShiftChar( float fElapsedTime )
{
	DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

	if( !m_bShiftButton )
	{
		if( (dxInputDev.GetKeyState( DIK_LEFT )&DXKEY_DOWN) || m_bShiftLBDown )
		{
			if( ShiftLeftChar() )
			{
				m_bShiftButton = TRUE;
			}
		}
		else if( (dxInputDev.GetKeyState( DIK_RIGHT )&DXKEY_DOWN) || m_bShiftRBDown )
		{
			if( ShiftRightChar() )
			{
				m_bShiftButton = TRUE;
			}
		}

		if( m_bShiftButton )
		{
			//	Note : ĳ���� ������ ���.
			m_dwCharSelect = MAX_SELECTCHAR;

			for ( int i=0; i<MAX_SELECTCHAR; i++ )
			{
				m_CharSham[i].ResetCharInfo ();
				m_CharSham[i].SetSelect ( FALSE );

				InitRenderSkip();
			}
		}
	}

	if( m_bShiftButton )
	{
		m_fShiftTime += fElapsedTime;

		if( m_fShiftTime > 0.5f )
		{
			ShiftCharInfo();

			m_fShiftTime = 0;
			m_bShiftButton = FALSE;
			m_bShiftLBDown = FALSE;
			m_bShiftRBDown = FALSE;

			CSelectCharacterPage * pControl = COuterInterface::GetInstance().GetSelectCharacterPage();
			if( pControl ) pControl->SetPageNum( GetPageNum() );
		}
	}
}

//***********************************************
// �ʱ� ĳ���� �������� �ִϸ��̼��� 1������ ��ŵ
//***********************************************
void DxLobyStage::InitRenderSkip()
{
	m_bRenderSkip = TRUE;
	m_fRenderSkipTime = 0.0f;
}

HRESULT DxLobyStage::OneTimeSceneInit ( HWND hWnd, WORD wWidth, WORD wHeight, const char* szAppPath )
{
	GASSERT(hWnd);

	StringCchCopy ( m_szAppPath, MAX_PATH, szAppPath );
	m_hWnd = hWnd;

	m_dwCharSelect = MAX_SELECTCHAR;
	m_nCharStart = 0;

	return S_OK;
}

HRESULT DxLobyStage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	m_pd3dDevice = pd3dDevice;

	hr = m_pd3dDevice->GetDeviceCaps ( &m_d3dCaps );
	if ( FAILED(hr) )	return hr;

	WORD wWidth = 800;
	WORD wHeight = 600;

	//	Note : �����ػ� �޾ƿ���.
	//
	LPDIRECT3DSURFACEQ pBackBuffer=NULL;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	if ( pBackBuffer )
	{
		pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
		pBackBuffer->Release();

		wWidth = static_cast<WORD> ( m_d3dsdBackBuffer.Width );
		wHeight = static_cast<WORD> ( m_d3dsdBackBuffer.Height );
	}

	//	Note : �������̽� �ʱ�ȭ.
	//
	COuterInterface::GetInstance().SetResolution ( MAKELONG ( wHeight, wWidth ) );
	COuterInterface::GetInstance().SetResolutionBack ( MAKELONG ( 768, 1024 ) );
	COuterInterface::GetInstance().OneTimeSceneInit ();

	//	Note : ����Ʈ.
	//
	DxViewPort::GetInstance().SetLobyCamera ();
	D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.0f, 70.0f, -70.0f );
	D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );

	//	Note : �������̽� �ʱ�ȭ.
	//
	hr = COuterInterface::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	//	Note : ĳ���� ���� ��� ���� ���� �ʱ�ȭ.
	//
	m_LandCharSlt.InitDeviceObjects ( m_pd3dDevice );
	m_LandCharSlt.LoadFile ( "character_slt.wld", m_pd3dDevice, FALSE );

	m_LandCharSlt_s01.InitDeviceObjects ( m_pd3dDevice );
	m_LandCharSlt_s01.LoadFile ( "character_slt_s01.wld", m_pd3dDevice, FALSE );

	m_LandCharSlt_s02.InitDeviceObjects ( m_pd3dDevice );
	m_LandCharSlt_s02.LoadFile ( "character_slt_s02.wld", m_pd3dDevice, FALSE );

	m_LandCharSlt_s03.InitDeviceObjects ( m_pd3dDevice );
	m_LandCharSlt_s03.LoadFile ( "character_slt_s03.wld", m_pd3dDevice, FALSE );

	m_LandLogin.InitDeviceObjects ( m_pd3dDevice );
	m_LandLogin.LoadFile ( "login.wld", m_pd3dDevice, FALSE );

	// Note : �̸� �ε�..
	D3DXVECTOR3 vMax = D3DXVECTOR3(100000,100000,100000);
	D3DXVECTOR3 vMin = D3DXVECTOR3(-100000,-100000,-100000);
	m_LandCharSlt.LoadBasicPos( m_pd3dDevice, vMax, vMin );
	m_LandCharSlt_s01.LoadBasicPos( m_pd3dDevice, vMax, vMin );
	m_LandCharSlt_s02.LoadBasicPos( m_pd3dDevice, vMax, vMin );
	m_LandCharSlt_s03.LoadBasicPos( m_pd3dDevice, vMax, vMin );
	m_LandLogin.LoadBasicPos( m_pd3dDevice, vMax, vMin );

	m_LandLogin.ActiveMap ();

	//	NOTE
	//		BGM ����, �κ�� ����
	DxBgmSound::GetInstance().SetFile ( ID2GAMEWORD("LOBY_BGM") );
	DxBgmSound::GetInstance().Play ();

	//	Note : ĳ���� ���� ������Ʈ �ʱ�ȭ.
	//
	char szName[MAX_PATH] = {0};
	for ( int i=0; i<MAX_SELECTCHAR; i++ )
	{
		StringCchPrintf ( szName, MAX_PATH, "character[%02d]", i+1 );
		DxLandGateMan* pLandGateMan = m_LandCharSlt.GetLandGateMan();
		PDXLANDGATE pLandGate = pLandGateMan->FindLandGate ( szName );
		GASSERT(pLandGate&&"character[%02d]�� ã�� ���Ͽ����ϴ�." );
		if ( pLandGate )
		{
			D3DXVECTOR3 vPos = (pLandGate->GetMax()+pLandGate->GetMin())/2.0f;
			m_CharSham[i].Create( &vPos, m_pd3dDevice, FALSE, TRUE, i );
		}
	}

	DxSkinCharDataContainer::GetInstance().LoadData( "chs_kt.chf", pd3dDevice, FALSE );
	DxSkinCharDataContainer::GetInstance().LoadData( "chs_ac.chf", pd3dDevice, FALSE );
	DxSkinCharDataContainer::GetInstance().LoadData( "chs_mg.chf", pd3dDevice, FALSE );

	D3DXVECTOR3 vPos = D3DXVECTOR3(-103.0f, -1.0f,-3.0f);

	m_NewCharSham.Create( &vPos, m_pd3dDevice, FALSE, TRUE, MAX_SELECTCHAR );

	return S_OK;
}

HRESULT DxLobyStage::RestoreDeviceObjects ()
{
	HRESULT hr;
	LPDIRECT3DSURFACEQ pBackBuffer;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();

	//	Note : ����Ʈ
	//
	DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
		(float)m_d3dsdBackBuffer.Height, 5.0f, 3000.0f );
	
	hr = DxViewPort::GetInstance().SetViewPort ();
	if ( FAILED(hr) )	return hr;

	COuterInterface::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );

	//	Note : ĳ���� ���� ��� ���� ���� �ʱ�ȭ.
	//
	m_LandCharSlt.RestoreDeviceObjects ( m_pd3dDevice );
	m_LandCharSlt_s01.RestoreDeviceObjects ( m_pd3dDevice );
	m_LandCharSlt_s02.RestoreDeviceObjects ( m_pd3dDevice );
	m_LandCharSlt_s03.RestoreDeviceObjects ( m_pd3dDevice );

	m_LandLogin.RestoreDeviceObjects ( m_pd3dDevice );

	//	Note : ĳ���� ���� ������Ʈ �ʱ�ȭ.
	//
	for ( int i=0; i<MAX_SELECTCHAR; i++ )
	{
		m_CharSham[i].RestoreDeviceObjects ( m_pd3dDevice );
	}

	m_NewCharSham.RestoreDeviceObjects( m_pd3dDevice );

	return S_OK;
}

HRESULT DxLobyStage::InvalidateDeviceObjects ()
{
	COuterInterface::GetInstance().InvalidateDeviceObjects ();

	//	Note : ĳ���� ���� ��� ���� ���� �ʱ�ȭ.
	//
	m_LandCharSlt.InvalidateDeviceObjects ();
	m_LandCharSlt_s01.InvalidateDeviceObjects ();
	m_LandCharSlt_s02.InvalidateDeviceObjects ();
	m_LandCharSlt_s03.InvalidateDeviceObjects ();

	m_LandLogin.InvalidateDeviceObjects ();

	//	Note : ĳ���� ���� ������Ʈ �ʱ�ȭ.
	//
	for ( int i=0; i<MAX_SELECTCHAR; i++ )
	{
		m_CharSham[i].InvalidateDeviceObjects ();
	}

	m_NewCharSham.InvalidateDeviceObjects();
	return S_OK;
}

HRESULT DxLobyStage::DeleteDeviceObjects ()
{
	//	Note : ĳ���� ���� ��� ���� ���� �ʱ�ȭ.
	//
	m_LandCharSlt.DeleteDeviceObjects ();
	m_LandCharSlt_s01.DeleteDeviceObjects ();
	m_LandCharSlt_s02.DeleteDeviceObjects ();
	m_LandCharSlt_s03.DeleteDeviceObjects ();

	m_LandLogin.DeleteDeviceObjects ();

	COuterInterface::GetInstance().DeleteDeviceObjects ();

	DxSkinCharDataContainer::GetInstance().ReleaseData ( "select01.chf" );
	DxSkinCharDataContainer::GetInstance().ReleaseData ( "select02.chf" );

	DxBgmSound::GetInstance().ForceStop ();

	if ( m_pBackground ) TextureManager::ReleaseTexture( m_strTextureName, m_pBackground );

	return S_OK;
}

HRESULT DxLobyStage::FinalCleanup ()
{
	//	Note : ĳ���� ���� ��� ���� ���� �ʱ�ȭ.
	//
	m_LandCharSlt.CleanUp ();
	m_LandLogin.CleanUp ();

	COuterInterface::GetInstance().FinalCleanup ();

	m_bGameJoin = FALSE;

	return S_OK;
}

HRESULT DxLobyStage::FrameMove ( float fTime, float fElapsedTime )
{
	DxInputDevice &dxInputDev = DxInputDevice::GetInstance();
	DWORD MouseLKey = dxInputDev.GetMouseState ( DXMOUSE_LEFT );

	if( (dxInputDev.GetKeyState( DIK_F12 )&DXKEY_DOWN) )
	{
		m_bUseInterface = !m_bUseInterface;
	}

	if( m_bUseInterface )
	{
		COuterInterface::GetInstance().FrameMove ( m_pd3dDevice, fElapsedTime );
	}

	UIGUID ControlID = COuterInterface::GetInstance().GetRenderStage ();
	
	//	Note : �������� ������ �����Ͽ� 
	//
	static UIGUID ControlIDBack = NO_ID;
	if ( ControlIDBack != ControlID )
	{
		switch ( ControlIDBack )
		{
		case SELECT_CHINA_AREA_PAGE:
		case SELECT_SERVER_PAGE:
		case LOGIN_PAGE:
		case PASS_KEYBOARD:
		case SECPASS_SETPAGE:
		case SECPASS_CHECKPAGE:
			break;

		case SELECT_CHARACTER_PAGE:
		case SELECT_CHARACTER_LEFTPAGE:
			for ( int i=0; i<MAX_SELECTCHAR; i++ )
			{
				m_CharSham[i].SetSelect ( FALSE );
				m_CharSham[i].FrameMove ( 0, 0 );
			}
			break;

		case CREATE_CHARACTER_PAGE:
		case CREATE_CHARACTER_WEB:
			break;
		};

		DxWeatherMan::GetInstance()->ReSetSound ();
//		DxBgmSound::GetInstance().Stop ();

		switch ( ControlID )
		{
		case SELECT_CHINA_AREA_PAGE:
		case SELECT_SERVER_PAGE:
		case LOGIN_PAGE:
		case PASS_KEYBOARD:
		case SECPASS_SETPAGE:
		case SECPASS_CHECKPAGE:
			{
				if( ControlIDBack==SELECT_SERVER_PAGE || 
					ControlIDBack==LOGIN_PAGE ||
					ControlIDBack==PASS_KEYBOARD || 
					ControlIDBack==SELECT_CHINA_AREA_PAGE ||
					ControlIDBack==SECPASS_SETPAGE ||
					ControlIDBack==SECPASS_CHECKPAGE) 
					break;
				
				m_LandLogin.ActiveMap ();
				DxCamAniMan* pCamAniMan = m_LandLogin.GetCamAniMan();
				pCamAniMan->ActiveCameraAni ( "main" );
			}
			break;

		case SELECT_CHARACTER_PAGE:
		case SELECT_CHARACTER_LEFTPAGE:
			{
				if( ControlIDBack==SELECT_CHARACTER_PAGE ||
					ControlIDBack==SELECT_CHARACTER_LEFTPAGE ) 
					break;

				//	���õ� �ɸ��� �ʱ�ȭ.
				m_dwCharSelect = MAX_SELECTCHAR;

				m_LandCharSlt.ActiveMap ();
				DxCamAniMan* pCamAniMan = m_LandCharSlt.GetCamAniMan();
				pCamAniMan->ActiveCameraPos ( "main" );

				InitRenderSkip();

				DxLightMan::GetInstance()->EnableLighting( true );
			}
			break;

		case CREATE_CHARACTER_PAGE:
		case CREATE_CHARACTER_WEB:
			{
				if( ControlIDBack==CREATE_CHARACTER_PAGE ||
					ControlIDBack==CREATE_CHARACTER_WEB )	
					break;

				m_sCharNewInfo.RESET();
				SelCharSchool( m_sCharNewInfo.m_wSchool );
// ĳ���� ���� Test
//#if	defined( _RELEASED ) || defined( TW_PARAM )  || defined( KRT_PARAM ) || defined( KR_PARAM ) || defined( VN_PARAM ) || defined( TH_PARAM ) || defined( MYE_PARAM ) || defined( MY_PARAM ) || defined( ID_PARAM ) || defined( CH_PARAM ) || defined( PH_PARAM ) || defined(HK_PARAM) || defined( JP_PARAM ) || defined ( GS_PARAM ) 
				
				// ī�޶� �� ���� ����				
				D3DXVECTOR3 vFromPt		= DxViewPort::GetInstance().GetFromPt();
				D3DXVECTOR3 vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
				D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
				vFromPt.y = 19.0f;
				
				DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
				DxLightMan::GetInstance()->EnableLighting( false );					
//#endif

			}
			break;
		};

		ControlIDBack = ControlID;
	}

	switch ( ControlID )
	{
	case SELECT_CHINA_AREA_PAGE:
	case SELECT_SERVER_PAGE:
	case LOGIN_PAGE:
	case PASS_KEYBOARD:
	case SECPASS_SETPAGE:
	case SECPASS_CHECKPAGE:
		{
			//	Note : ����Ʈ
			//
			DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
				(float)m_d3dsdBackBuffer.Height, 5.0f, 3000.0f );

			GLPeriod::GetInstance().SetPeriod ( 2003, 180, 1, 0 );
			m_LandLogin.FrameMove ( fTime, fElapsedTime );
		}
		break;

	case SELECT_CHARACTER_PAGE:
	case SELECT_CHARACTER_LEFTPAGE:
		{
			DxCamAniMan* pCamAniMan = m_LandCharSlt.GetCamAniMan();
			pCamAniMan->ActiveCameraAni ( "main" );

			//	Note : ����Ʈ
			//
			DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
				(float)m_d3dsdBackBuffer.Height, 5.0f, 600.0f );

			GLPeriod::GetInstance().SetPeriod ( 2003, 180, 1, 0 );

			//	Note : ĳ���� ���� ��� ���� ����.
			//
			m_LandCharSlt.ActiveMap ();
			m_LandCharSlt.FrameMove ( fTime, fElapsedTime );

			D3DXVECTOR3 vDir = DxViewPort::GetInstance().GetFromPt() - DxViewPort::GetInstance().GetLookatPt();

			//	Note : ĳ���� ���� ������Ʈ.
			//
			for ( int i=0; i<MAX_SELECTCHAR; i++ )
			{
				m_CharSham[i].SetDirection ( vDir );
				m_CharSham[i].FrameMove ( fTime, fElapsedTime );
			}

			BOOL bOnInterface = COuterInterface::GetInstance().IsOnTheInterface();

			DWORD dwCharSelect = MAX_SELECTCHAR;
			if ( !bOnInterface && MouseLKey&(DXKEY_UP|DXKEY_DUP) && m_bGameJoin == FALSE )
			{
				D3DXVECTOR3 vFrom, vLook;
				vFrom = DxViewPort::GetInstance().GetFromPt ();
				BOOL bOk = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vLook );

				if ( bOk )
				{
					//for ( int i=0; i<m_nChaSNum; i++ )
					for ( int i=0; i<MAX_SELECTCHAR; i++ )
					{
						if ( !m_CharSham[i].IsValidData() )		continue;

						D3DXVECTOR3 vMax, vMin;
						m_CharSham[i].GetAABB ( vMax, vMin );

						BOOL bCollision = COLLISION::IsCollisionLineToAABB ( vFrom, vLook, vMax, vMin );
						if ( bCollision )
						{
							dwCharSelect = i;
							DxGlobalStage::GetInstance().GetGameStage()->SetSelectChar ( &m_CharInfo[dwCharSelect+m_nCharStart] );
							break;
						}
					}
				}
			}

			//	Note : Ŭ���� �ɸ��� ������.
			//
			if ( dwCharSelect != MAX_SELECTCHAR )
			{
				//	Note : �ɸ��� ���� ȿ�� ǥ��.
				//
				if( m_dwCharSelect != dwCharSelect )
				{
					m_dwCharSelect = dwCharSelect;

					for ( int i=0; i<MAX_SELECTCHAR; i++ )	
						m_CharSham[i].SetSelect ( FALSE, TRUE );

					m_CharSham[m_dwCharSelect].SetSelect ( TRUE );
				}

				if ( !bOnInterface && MouseLKey&DXKEY_DUP && !m_bGameJoin )
				{
					//	TODO : ���õ� �ɸ����� ���� ����.
					//
					m_bGameJoin = TRUE;

					GLMSG::SNETLOBBY_REQ_GAME_JOIN NetMsgReq;
					COuterInterface::GetInstance().MsgProcess ( &NetMsgReq );
				}
			}

			ShiftChar( fElapsedTime ); // ĳ���� ����Ʈ

			if( m_bRenderSkip )
			{
				m_fRenderSkipTime += fElapsedTime;
			}
		}
		break;

	case CREATE_CHARACTER_PAGE:
	case CREATE_CHARACTER_WEB:
		{
// ĳ���� ���� Test 
/*
#if !defined( _RELEASED ) && !defined( TW_PARAM ) && !defined( KRT_PARAM ) && !defined( KR_PARAM ) && !defined( VN_PARAM ) && !defined( TH_PARAM ) && !defined( MYE_PARAM ) && !defined( MY_PARAM ) && !defined( ID_PARAM ) && !defined( CH_PARAM ) && !defined( PH_PARAM ) && !defined(HK_PARAM) && !defined( JP_PARAM ) && !defined( GS_PARAM ) 
			GLPeriod::GetInstance().SetPeriod ( 2003, 180, 1, 0 );
		
			//	Note : ĳ���� ���� ��� ���� ����.
			//
			DxLandMan *pLandMan = GetCharNewLand();
			if( pLandMan ) pLandMan->FrameMove( fTime, fElapsedTime );
#else 
*/
			if ( m_bStyle )
			{
				m_NewCharSham.SetSelect( FALSE );
				m_NewCharSham.SetDirection( m_vDir );

				m_NewCharSham.m_wFace = m_sCharNewInfo.m_wFace;
				m_NewCharSham.m_wHair = m_sCharNewInfo.m_wHair;
				m_NewCharSham.m_wHairColor = m_sCharNewInfo.m_wHairColor;

				m_NewCharSham.UpdateSuit();
				m_NewCharSham.FrameMove( fTime, fElapsedTime );
				
				
				m_rcVertex = COuterInterface::GetInstance().GetBackImageRect();
				m_Vertices[0] = TEXTUREVERTEX( m_rcVertex.left, m_rcVertex.top, m_rcTexture.left, m_rcTexture.top );
				m_Vertices[1] = TEXTUREVERTEX( m_rcVertex.right, m_rcVertex.top, m_rcTexture.right, m_rcTexture.top );
				m_Vertices[2] = TEXTUREVERTEX( m_rcVertex.left, m_rcVertex.bottom, m_rcTexture.left, m_rcTexture.bottom );
				m_Vertices[3] = TEXTUREVERTEX( m_rcVertex.right, m_rcVertex.bottom, m_rcTexture.right, m_rcTexture.bottom );
			}
//#endif
		}
		break;
	};
	 
	return S_OK;
}

void DxLobyStage::UpdateSelSkinChar ( DxSkinChar* pSkinObj, EMCHARINDEX emIndex, WORD wFace, WORD wHair, WORD wHairColor )
{
	if ( !pSkinObj )	return;

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	if ( sCONST.dwHEADNUM > wFace )
	{
		std::string strHEAD_CPS = sCONST.strHEAD_CPS[wFace];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = pSkinObj->GetPiece(PIECE_HEAD);		//	���� ���� ��Ų.

		if ( !pCharPart || !pCharPart->GetFileName() || strcmp(strHEAD_CPS.c_str(),pCharPart->GetFileName()) )
		{
			pSkinObj->SetPiece( strHEAD_CPS.c_str(), m_pd3dDevice );
		}
	}

	if ( sCONST.dwHAIRNUM > wHair )
	{
		std::string strHAIR_CPS = sCONST.strHAIR_CPS[wHair];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = pSkinObj->GetPiece(PIECE_HAIR);		//	���� ���� ��Ų.

		if ( !pCharPart || !pCharPart->GetFileName() || strcmp(strHAIR_CPS.c_str(),pCharPart->GetFileName()) )
		{
			pSkinObj->SetPiece( strHAIR_CPS.c_str(), m_pd3dDevice );
		}
	}

	// ����÷� ����
	pSkinObj->SetHairColor( wHairColor );
}

HRESULT DxLobyStage::Render ()
{
	CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume ();

	UIGUID ControlID = COuterInterface::GetInstance().GetRenderStage ();

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )	
	{

// ĳ���� ���� Test 
//#if	defined( _RELEASED ) || defined( TW_PARAM )  || defined( KRT_PARAM ) || defined( KR_PARAM ) || defined( VN_PARAM ) || defined( TH_PARAM ) || defined( MYE_PARAM ) || defined( MY_PARAM ) || defined( ID_PARAM ) || defined( CH_PARAM ) || defined( PH_PARAM ) || defined(HK_PARAM) || defined( JP_PARAM ) || defined ( GS_PARAM ) 
		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );
//#else
//		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, TRUE );
//#endif
	}

	//	Note : �ϴ� ������..!!
	DxSkyMan::GetInstance().Render ( m_pd3dDevice );

	//	Note : ĳ���� ���� ��� ���� ����.
	//
	switch ( ControlID )
	{
	case SELECT_CHINA_AREA_PAGE:
	case SELECT_SERVER_PAGE:
	case LOGIN_PAGE:
	case PASS_KEYBOARD:
	case SECPASS_SETPAGE:
	case SECPASS_CHECKPAGE:
		{
			PROFILE_BEGIN2("Environment Render");
			DxEnvironment::GetInstance().Render( m_pd3dDevice, NULL, &m_LandLogin );
			PROFILE_END2("Environment Render");

			m_LandLogin.Render ( m_pd3dDevice, CV );
			m_LandLogin.Render_EFF ( m_pd3dDevice, CV );
			m_LandLogin.RenderPickAlpha( m_pd3dDevice );
			DxWeatherMan::GetInstance()->Render ( m_pd3dDevice );
		}
		break;

	case SELECT_CHARACTER_PAGE:
	case SELECT_CHARACTER_LEFTPAGE:
		{
			PROFILE_BEGIN2("Environment Render");
			DxEnvironment::GetInstance().Render( m_pd3dDevice, NULL, &m_LandCharSlt );
			PROFILE_END2("Environment Render");

			m_LandCharSlt.Render ( m_pd3dDevice, CV );

			//	Note : ĳ���� ���� ������Ʈ.
			//
			if( m_fRenderSkipTime > 1.0f )
			{
				for ( int i=0; i<MAX_SELECTCHAR; i++ )
				{
					m_CharSham[i].Render ( m_pd3dDevice, CV, FALSE );
					m_CharSham[i].RenderShadow ( m_pd3dDevice );
				}

				m_bRenderSkip = FALSE;
			}

			m_LandCharSlt.Render_EFF ( m_pd3dDevice, CV );
			m_LandCharSlt.RenderPickAlpha( m_pd3dDevice );

			DxWeatherMan::GetInstance()->Render ( m_pd3dDevice );
		}
		break;

	case CREATE_CHARACTER_PAGE:
	case CREATE_CHARACTER_WEB:
		{
/*
// ĳ���� ���� Test 
//#if !defined( _RELEASED ) && !defined( TW_PARAM ) && !defined( KRT_PARAM ) && !defined( KR_PARAM ) && !defined( VN_PARAM ) && !defined( TH_PARAM ) && !defined( MYE_PARAM ) && !defined( MY_PARAM ) && !defined( ID_PARAM ) && !defined( CH_PARAM ) && !defined( PH_PARAM ) && !defined(HK_PARAM) && !defined( JP_PARAM ) && !defined( GS_PARAM ) 
			DxLandMan *pLandMan = GetCharNewLand();
			if ( pLandMan )
			{
				PROFILE_BEGIN2("Environment Render");
				DxEnvironment::GetInstance().Render( m_pd3dDevice, NULL, pLandMan );
				PROFILE_END2("Environment Render");

				for ( int i=0; i<GLCI_NUM_EX; ++i )
				{
					CString strSkinObj;
					strSkinObj.Format ( "s%02d_%02d.chf", m_sCharNewInfo.m_wSchool+1, g_dwCLASS[i]+1 );

					PLANDSKINOBJ pLandSkinObj = pLandMan->FindLandSkinObjByFileName(strSkinObj.GetString());
					if( pLandSkinObj )		
					{
						UpdateSelSkinChar ( pLandSkinObj->GetSkinChar(), 
											(EMCHARINDEX)i, 
											0, 
											0, 
											HAIRCOLOR::wHairColor[i][0] );
					}
				}

				if ( m_sCharNewInfo.m_emIndex < GLCI_NUM_EX )
				{
					CString strSkinObj;
					strSkinObj.Format ( "s%02d_%02d.chf", m_sCharNewInfo.m_wSchool+1, g_dwCLASS[m_sCharNewInfo.m_emIndex]+1 );

					PLANDSKINOBJ pLandSkinObj = pLandMan->FindLandSkinObjByFileName(strSkinObj.GetString());
					if( pLandSkinObj )
					{
						UpdateSelSkinChar ( pLandSkinObj->GetSkinChar(), 
											m_sCharNewInfo.m_emIndex, 
											m_sCharNewInfo.m_wFace, 
											m_sCharNewInfo.m_wHair, 
											m_sCharNewInfo.m_wHairColor );
					}
				}

				pLandMan->Render ( m_pd3dDevice, CV );
				pLandMan->Render_EFF ( m_pd3dDevice, CV );
				pLandMan->RenderPickAlpha( m_pd3dDevice );
			}

			DxWeatherMan::GetInstance()->Render ( m_pd3dDevice );
#else 
*/

			m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,	D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

			if ( m_bStyle )
			{
				m_pd3dDevice->SetTexture ( 0, m_pBackground );
				m_pd3dDevice->SetFVF( TEXTUREVERTEXFVF );
				m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_Vertices, sizeof(TEXTUREVERTEX) );

				m_pd3dDevice->SetTexture ( 0, NULL );
				m_NewCharSham.Render ( m_pd3dDevice, CV, FALSE );
			}
//#endif
		}
		break;

	};

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );

	PROFILE_BEGIN("DxEffGroupPlayer::Render");
	DxEffGroupPlayer::GetInstance().Render ( m_pd3dDevice );
	PROFILE_END("DxEffGroupPlayer::Render");

	// Note : ����ȭ�� Eff
	OPTMManager::GetInstance().Render( m_pd3dDevice );

	//	Note : ť�� �ؽ��� ������..!!
	DxCubeMap::GetInstance().Render ( m_pd3dDevice );

	//	Note : �۷ο� ó���� �ϱ����� ��� �Ѵ�.
	//
	DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );
	DxGlowMan::GetInstance().Render ( m_pd3dDevice );
	DxGlowMan::GetInstance().RenderBURN( m_pd3dDevice );

	//	Note : Wave �Ǿ� �ϴ°��� ������ ���������� �Ѹ���.
	//
	PROFILE_BEGIN("DxEnvironment::UserToRefraction()");
	DxEnvironment::GetInstance().UserToRefraction ( m_pd3dDevice );
	PROFILE_END("DxEnvironment::UserToRefraction()");

	PROFILE_BEGIN("DxEnvironment::RenderWave()");
	DxEnvironment::GetInstance().RenderWave ( m_pd3dDevice, DxEffectMan::GetInstance().GetBumpTexture() );
	PROFILE_END("DxEnvironment::RenderWave()");

	// Note : ��ó��
	DxPostProcess::GetInstance().Render( m_pd3dDevice );

	PROFILE_BEGIN("COuterInterface::Render");
	if( m_bUseInterface )
	{
		COuterInterface::GetInstance().Render ( m_pd3dDevice );
	}
	PROFILE_END("COuterInterface::Render");

	return S_OK;
}

void DxLobyStage::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	CString str;

	switch ( nmg->nType )
	{
	case NET_MSG_LOGIN_FB:
	case NET_MSG_PASSCHECK_FB:
	case JAPAN_NET_MSG_UUID:
		{
			COuterInterface::GetInstance().MsgProcess ( nmg );
		}
		break;

	case NET_MSG_CHA_BAINFO:
		{
			NET_CHA_BBA_INFO* ncbi = (NET_CHA_BBA_INFO*) nmg;

			m_nStartCharNum = m_nChaSNum = ncbi->nChaSNum;

			for ( int i=0; i<m_nChaSNum; i++ )
			{
				m_nChaIDs[i] = ncbi->nChaNum[i];
				DxGlobalStage::GetInstance().GetNetClient()->SndChaBasicInfo ( m_nChaIDs[i] );
			}
		}
		break;

	case NET_MSG_LOBBY_CHAR_SEL:
		{
			GLMSG::SNETLOBBY_CHARINFO* snci = (GLMSG::SNETLOBBY_CHARINFO*) nmg;
		
			BOOL bInserted = FALSE;

			//	Note : ���� ĳ���� ���� ���Ž�.
			for ( int i=0; i<m_nChaSNum; i++ )
			{
				if ( m_nChaIDs[i] == snci->Data.m_dwCharID )
				{
					bInserted = TRUE;
					m_CharInfo[i] = snci->Data;

					if( i < MAX_SELECTCHAR )
						m_CharSham[i].SetCharInfo( m_CharInfo[i], FALSE );

					++m_nStartCharLoad;
					break;
				}
			}

			//	Note : ������ ������ ĳ���� ���� ���Ž�.
			if ( !bInserted )
			{
				if ( m_nChaSNum < MAX_SERVERCHAR )
				{
					m_nChaIDs[m_nChaSNum] = snci->Data.m_dwCharID;
					m_CharInfo[m_nChaSNum] = snci->Data;
					m_CharSham[m_nChaSNum%MAX_SELECTCHAR].SetCharInfo( m_CharInfo[m_nChaSNum], FALSE );

					++m_nChaSNum;

					ShiftRBDown();
				}
			}
		}
		break;

	case NET_MSG_CHA_DEL_FB_OK:
		{
			COuterInterface::GetInstance().SetLobyStage( this );
		}
	case NET_MSG_CHA_DEL_FB_CLUB:
	case NET_MSG_CHA_DEL_FB_ERROR:
	case NET_MSG_CHA_NEW_FB:
	case NET_MSG_LOBBY_CHAR_JOIN_FB:
	case NET_MSG_LOBBY_CHINA_ERROR:
		{
			m_bGameJoin = FALSE;

			COuterInterface::GetInstance().MsgProcess ( nmg );

			// ������ ��� ������ ����
			if( nmg->nType == NET_MSG_LOBBY_CHAR_JOIN_FB )
			{
				GLMSG::SNETLOBBY_CHARJOIN_FB *pNetMsg = (GLMSG::SNETLOBBY_CHARJOIN_FB *) nmg;
				if( pNetMsg->emCharJoinFB == EMCJOIN_FB_ERROR )
					m_bGameJoin = TRUE;
			}
			
		}
		break;

	default:
		CDebugSet::ToListView ( "DxLobyStage::MsgProcess() �з����� ���� �޽��� ����. TYPE[%d]", nmg->nType );
		break;
	};
}

void  DxLobyStage::SetStyleStep( bool bStyle, CString& strTextureName, UIRECT& rcSize, UIRECT& rcTextureSize )
{
	m_bStyle = bStyle;

	if ( m_bStyle )
	{
		SCHARINFO_LOBBY sCharInfo;
		sCharInfo.m_emClass = CharIndexToClass( m_sCharNewInfo.m_emIndex ); // ����
	    sCharInfo.m_wSchool = m_sCharNewInfo.m_wSchool; // �п�.
	    sCharInfo.m_wHair = m_sCharNewInfo.m_wHair; // ��Ÿ��
		sCharInfo.m_wFace = m_sCharNewInfo.m_wFace;
		sCharInfo.m_wHairColor = m_sCharNewInfo.m_wHairColor;

		SCHARDATA2& sCharData = GLCONST_CHAR::GET_CHAR_DATA2(m_sCharNewInfo.m_wSchool,m_sCharNewInfo.m_emIndex);

		sCharInfo.m_PutOnItems[SLOT_UPPER].Assign( sCharData.m_PutOnItems[SLOT_UPPER] );
		sCharInfo.m_PutOnItems[SLOT_LOWER].Assign( sCharData.m_PutOnItems[SLOT_LOWER] );		

		m_NewCharSham.SetCharInfo( sCharInfo, FALSE );
		
		if ( m_pBackground ) TextureManager::ReleaseTexture( m_strTextureName, m_pBackground );

		m_strTextureName = strTextureName;
		
		TextureManager::LoadTexture( m_strTextureName, m_pd3dDevice, m_pBackground,0,0,FALSE );

		m_rcVertex = rcSize;
		m_rcTexture = rcTextureSize;

		m_Vertices[0] = TEXTUREVERTEX( m_rcVertex.left, m_rcVertex.top, m_rcTexture.left, m_rcTexture.top );
		m_Vertices[1] = TEXTUREVERTEX( m_rcVertex.right, m_rcVertex.top, m_rcTexture.right, m_rcTexture.top );
		m_Vertices[2] = TEXTUREVERTEX( m_rcVertex.left, m_rcVertex.bottom, m_rcTexture.left, m_rcTexture.bottom );
		m_Vertices[3] = TEXTUREVERTEX( m_rcVertex.right, m_rcVertex.bottom, m_rcTexture.right, m_rcTexture.bottom );

		m_vDir = D3DXVECTOR3( -1.0f,-1.0f,-0.35f);

	}
	else
	{
		m_NewCharSham.ResetCharInfo();
	}
}

void  DxLobyStage::RotateChar( bool bLeft )
{
	if ( bLeft )
	{
		m_vDir.x -= 0.065f;
		m_vDir.z += 0.1f;

		if ( m_vDir.x < -1.65f ) m_vDir.x = -1.65f;
		if ( m_vDir.z > 0.65f ) m_vDir.z = 0.65f;

	}
	else
	{
		m_vDir.x += 0.065f;
		m_vDir.z -= 0.1f;		

		if ( m_vDir.x > -0.35f ) m_vDir.x = -0.35f;
		if ( m_vDir.z < -1.35f ) m_vDir.z = -1.35f;
	}	
}