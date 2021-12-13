// EtcFuncPage.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "EtcFuncPage.h"
#include "SheetWithTab.h"

#include "EtcFunction.h"

#include "DxViewPort.h"

#include "GLGaeaServer.h"
#include "GLLandMan.h"
#include "GLCharDefine.h"
#include ".\etcfuncpage.h"

BOOL CEtcFuncPage::m_bLimitTimeUse = FALSE;
BOOL CEtcFuncPage::m_bCamUse = FALSE;
BOOL CEtcFuncPage::m_bCamTest = FALSE;
BOOL CEtcFuncPage::m_bLandEffUse = FALSE;
// CEtcFuncPage dialog

IMPLEMENT_DYNAMIC(CEtcFuncPage, CPropertyPage)
CEtcFuncPage::CEtcFuncPage()
	: CPropertyPage(CEtcFuncPage::IDD)
	, m_pSheetTab ( NULL )
	, m_bInit ( FALSE )
	, m_vMinLandPos ( 0.f, 0.f )
	, m_vMaxLandPos ( 0.f, 0.f )
	, m_dwSelectNum ( -1 )
{
}

CEtcFuncPage::~CEtcFuncPage()
{
}

void CEtcFuncPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LAND_EFF, m_list_LandEff);
	DDX_Control(pDX, IDC_COMBO_LAND_EFF, m_comob_LandEff);
}


BEGIN_MESSAGE_MAP(CEtcFuncPage, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_LIMIT_TIME_USE, OnBnClickedCheckLimitTimeUse)
	ON_BN_CLICKED(IDC_CHECK_CAM_USE, OnBnClickedCheckCamUse)
	ON_BN_CLICKED(IDC_BUTTON_CAM_SET_CENTER, OnBnClickedButtonCamSetCenter)
	ON_BN_CLICKED(IDC_CHECK_LAND_EFF_USE, OnBnClickedCheckLandEffUse)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LAND_EFF, OnNMClickListLandEff)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LAND_EFF, OnNMDblclkListLandEff)
	ON_BN_CLICKED(IDC_BUTTON_LAND_EFF_ADD, OnBnClickedButtonLandEffAdd)
	ON_BN_CLICKED(IDC_BUTTON_LAND_EFF_DEL, OnBnClickedButtonLandEffDel)
	ON_BN_CLICKED(IDC_BUTTON_ETC_FUNC_SAVE, OnBnClickedButtonEtcFuncSave)
	ON_BN_CLICKED(IDC_CHECK_CAM_TEST, OnBnClickedCheckCamTest)
END_MESSAGE_MAP()


// CEtcFuncPage message handlers
BOOL CEtcFuncPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bInit = TRUE;
	m_comob_LandEff.ResetContent();

	for ( int i = 0; i < (int)EMLANDEFFECT_SIZE; ++i )
	{
		m_comob_LandEff.AddString ( COMMENT::LANDEFFECT_TYPE[i].c_str() );
	}
	
	m_comob_LandEff.SetCurSel ( 0 );

	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_list_LandEff.SetExtendedStyle ( m_list_LandEff.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list_LandEff.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	const int nColumnCount = 3;	
	char* szColumnName1[nColumnCount] = { "EffType", "Rate", "Color" };
	int nColumnWidthPercent1[nColumnCount] = { 60, 20, 20 };

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent1[i] ) / 100;
		m_list_LandEff.InsertColumn (i, &lvColumn );
	}

	SetWin_Text ( this, IDC_EDIT_LAND_EFF_VAL, "0" );

	UpdatePage ();

	return TRUE;
}

void CEtcFuncPage::SetEnableFunction ( DWORD dwEtcFunc, BOOL bEnable )
{
	switch ( dwEtcFunc )
	{
	case EMETCFUNC_LIMITTIME:
		{
			SetWin_Check ( this, IDC_CHECK_LIMIT_TIME_USE, bEnable );

			SetWin_Enable ( this, IDC_EDIT_LIMIT_TIME_MAX, bEnable );
			SetWin_Enable ( this, IDC_EDIT_LIMIT_TELL_TIME, bEnable );
		}
		break;
	case EMETCFUNC_CONTROLCAM:
		{
			SetWin_Check ( this, IDC_CHECK_CAM_USE, bEnable );

			SetWin_Enable ( this, IDC_EDIT_CAM_UP, bEnable );
			SetWin_Enable ( this, IDC_EDIT_CAM_DOWN, bEnable );

			SetWin_Enable ( this, IDC_EDIT_CAM_LEFT, bEnable );
			SetWin_Enable ( this, IDC_EDIT_CAM_RIGHT, bEnable );

			SetWin_Enable ( this, IDC_EDIT_CAM_FAR, bEnable );
			SetWin_Enable ( this, IDC_EDIT_CAM_NEAR, bEnable );

			SetWin_Enable ( this, IDC_BUTTON_CAM_SET_CENTER, bEnable );
			SetWin_Enable ( this, IDC_CHECK_CAM_TEST, bEnable );
		}
		break;
	case EMETCFUNC_LANDEFFECT:
		{
			SetWin_Check ( this, IDC_CHECK_LAND_EFF_USE, bEnable );

			SetWin_Enable ( this, IDC_LIST_LAND_EFF, bEnable );
			SetWin_Enable ( this, IDC_COMBO_LAND_EFF, bEnable );

			SetWin_Enable ( this, IDC_BUTTON_LAND_EFF_ADD, bEnable );
			SetWin_Enable ( this, IDC_BUTTON_LAND_EFF_DEL, bEnable );
		}
		break;
	case EMETCFUNC_MOBCHASE:
		{
			SetWin_Check ( this, IDC_CHECK_ETC_FUNC_MON_CHASE, bEnable );
		}
		break;
	default:
		return;
	}
}

void CEtcFuncPage::SetEtcFuncData ()
{
	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_ETC_FUNC* pEtcFunc = pGLLandMan->GetLevelEtcFunc();

	if ( GetWin_Check ( this, IDC_CHECK_LIMIT_TIME_USE ) )
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_LIMITTIME] = true;

		pEtcFunc->m_sLimitTime.nLimitMinute = atoi ( GetWin_Text ( this, IDC_EDIT_LIMIT_TIME_MAX ).GetString() );
		pEtcFunc->m_sLimitTime.nTellTermMinute = atoi ( GetWin_Text ( this, IDC_EDIT_LIMIT_TELL_TIME ).GetString() );
	}
	else
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_LIMITTIME] = false;
		pEtcFunc->m_sLimitTime.Init();
	}

	if ( GetWin_Check ( this, IDC_CHECK_CAM_USE ) )
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_CONTROLCAM] = true;

		pEtcFunc->m_sControlCam.fUp = (float)atof ( GetWin_Text ( this, IDC_EDIT_CAM_UP ).GetString() );
		pEtcFunc->m_sControlCam.fDown = (float)atof ( GetWin_Text ( this, IDC_EDIT_CAM_DOWN ).GetString() );

		pEtcFunc->m_sControlCam.fLeft = (float)atof ( GetWin_Text ( this, IDC_EDIT_CAM_LEFT ).GetString() );
		pEtcFunc->m_sControlCam.fRight = (float)atof ( GetWin_Text ( this, IDC_EDIT_CAM_RIGHT ).GetString() );

		pEtcFunc->m_sControlCam.fFar = (float)atof ( GetWin_Text ( this, IDC_EDIT_CAM_FAR ).GetString() );
		pEtcFunc->m_sControlCam.fNear = (float)atof ( GetWin_Text ( this, IDC_EDIT_CAM_NEAR ).GetString() );

		pEtcFunc->m_sControlCam.vCamPos.x = (float)atof ( GetWin_Text ( this, IDC_EDIT_CAM_X_VAL ).GetString() );
		pEtcFunc->m_sControlCam.vCamPos.y = (float)atof ( GetWin_Text ( this, IDC_EDIT_CAM_Y_VAL ).GetString() );
		pEtcFunc->m_sControlCam.vCamPos.z = (float)atof ( GetWin_Text ( this, IDC_EDIT_CAM_Z_VAL ).GetString() );
	}
	else
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_CONTROLCAM] = false;
		pEtcFunc->m_sControlCam.Init();
	}

	if ( GetWin_Check ( this, IDC_CHECK_CAM_USE ) )
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_MOBCHASE] = true;
	}
	else
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_MOBCHASE] = false;
	}

	if ( GetWin_Check ( this, IDC_CHECK_CAM_USE ) )
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_LANDEFFECT] = true;
		pEtcFunc->m_vecLandEffect = m_vecLandEffect;
	}
	else
	{
		pEtcFunc->m_bUseFunction[EMETCFUNC_LANDEFFECT] = false;
		pEtcFunc->m_vecLandEffect.clear();
	}
}

void CEtcFuncPage::UpdateLandEffectList()
{
	m_list_LandEff.DeleteAllItems();

	for ( int i = 0; i < (int)m_vecLandEffect.size(); ++i )
	{
		EMLANDEFFECT_TYPE emType = m_vecLandEffect[i].emLandEffectType;
		m_list_LandEff.InsertItem ( i, COMMENT::LANDEFFECT_TYPE[(int)emType].c_str() );

		CString strTemp;
		strTemp.Format ( "%.3f", m_vecLandEffect[i].fValue );
		m_list_LandEff.SetItemText ( i, 1, strTemp.GetString() );
		
		switch ( m_vecLandEffect[i].emLandEffectType )
		{
		case EMLANDEFFECT_ATK_SPEED:
			{
				m_list_LandEff.SetItemText ( i, 2, "Red" );
			}
			break;
		case EMLANDEFFECT_MOVE_SPEED:
			{
				m_list_LandEff.SetItemText ( i, 2, "Orange" );
			}
			break;
		case EMLANDEFFECT_MP_RATE:
			{
				m_list_LandEff.SetItemText ( i, 2, "Yellow" );
			}
			break;
		case EMLANDEFFECT_HP_RATE:
			{
				m_list_LandEff.SetItemText ( i, 2, "Dark Green" );
			}
			break;
		case EMLANDEFFECT_RECOVER_RATE:
			{
				m_list_LandEff.SetItemText ( i, 2, "Blue" );
			}
			break;
		case EMLANDEFFECT_DAMAGE_RATE:
			{
				m_list_LandEff.SetItemText ( i, 2, "Navy Blue" );
			}
			break;
		case EMLANDEFFECT_DEFENSE_RATE:
			{
				m_list_LandEff.SetItemText ( i, 2, "Purple" );
			}
			break;
		case EMLANDEFFECT_RESIST_RATE:
			{
				m_list_LandEff.SetItemText ( i, 2, "Grey" );
			}
			break;
		case EMLANDEFFECT_CANCEL_ALLBUFF:
			{
				m_list_LandEff.SetItemText ( i, 2, "Black" );
			}
			break;
		case EMLANDEFFECT_SAFE_ZONE:
			{
				m_list_LandEff.SetItemText ( i, 2, "White" );
			}
			break;
		default:
			continue;
		}
	}
}

void CEtcFuncPage::UpdatePage()
{
	if ( !m_bInit )
		return;

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	SLEVEL_ETC_FUNC* pEtcFunc = pGLLandMan->GetLevelEtcFunc();

	for ( DWORD i = 0; i < (DWORD)EMETCFUNC_SIZE; ++i )
		SetEnableFunction ( i, pEtcFunc->m_bUseFunction[i] );

	CString strTemp;

	strTemp.Format ( "%d", pEtcFunc->m_sLimitTime.nLimitMinute );
	SetWin_Text ( this, IDC_EDIT_LIMIT_TIME_MAX, strTemp.GetString() );
	strTemp.Format ( "%d", pEtcFunc->m_sLimitTime.nLimitMinute );
	SetWin_Text ( this, IDC_EDIT_LIMIT_TELL_TIME, strTemp.GetString() );

	strTemp.Format ( "%.2f", pEtcFunc->m_sControlCam.fUp );
	SetWin_Text ( this, IDC_EDIT_CAM_UP, strTemp.GetString() );
	strTemp.Format ( "%.2f", pEtcFunc->m_sControlCam.fDown );
	SetWin_Text ( this, IDC_EDIT_CAM_DOWN, strTemp.GetString() );

	strTemp.Format ( "%.2f", pEtcFunc->m_sControlCam.fLeft );
	SetWin_Text ( this, IDC_EDIT_CAM_LEFT, strTemp.GetString() );
	strTemp.Format ( "%.2f", pEtcFunc->m_sControlCam.fRight );
	SetWin_Text ( this, IDC_EDIT_CAM_RIGHT, strTemp.GetString() );

	strTemp.Format ( "%.2f", pEtcFunc->m_sControlCam.fFar );
	SetWin_Text ( this, IDC_EDIT_CAM_FAR, strTemp.GetString() );
	strTemp.Format ( "%.2f", pEtcFunc->m_sControlCam.fNear );
	SetWin_Text ( this, IDC_EDIT_CAM_NEAR, strTemp.GetString() );

	strTemp.Format ( "%.2f", pEtcFunc->m_sControlCam.vCamPos.x );
	SetWin_Text ( this, IDC_EDIT_CAM_X_VAL, strTemp.GetString() );
	strTemp.Format ( "%.2f", pEtcFunc->m_sControlCam.vCamPos.y );
	SetWin_Text ( this, IDC_EDIT_CAM_Y_VAL, strTemp.GetString() );
	strTemp.Format ( "%.2f", pEtcFunc->m_sControlCam.vCamPos.z );
	SetWin_Text ( this, IDC_EDIT_CAM_Z_VAL, strTemp.GetString() );

	m_vecLandEffect = pEtcFunc->m_vecLandEffect;
	UpdateLandEffectList();
}

void CEtcFuncPage::OnBnClickedCheckLimitTimeUse()
{
	// TODO: Add your control notification handler code here
	m_bLimitTimeUse = GetWin_Check ( this, IDC_CHECK_LIMIT_TIME_USE );
	SetEnableFunction ( EMETCFUNC_LIMITTIME, m_bLimitTimeUse );
}

void CEtcFuncPage::OnBnClickedCheckCamUse()
{
	// TODO: Add your control notification handler code here
	m_bCamUse = GetWin_Check ( this, IDC_CHECK_CAM_USE );
	SetEnableFunction ( EMETCFUNC_CONTROLCAM, m_bCamUse );
}

void CEtcFuncPage::OnBnClickedButtonCamSetCenter()
{
	// TODO: Add your control notification handler code here
	D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 vLookAtPt = DxViewPort::GetInstance().GetLookatPt();

	D3DXVECTOR3 vCamPos = vFromPt - vLookAtPt;
	float fLength = D3DXVec3Length ( &vCamPos );

	if ( fLength <= 200.0f )
	{
		CString strTemp;

		strTemp.Format ( "%.2f", vCamPos.x );
		SetWin_Text ( this, IDC_EDIT_CAM_X_VAL, strTemp.GetString() );

		strTemp.Format ( "%.2f", vCamPos.y );
		SetWin_Text ( this, IDC_EDIT_CAM_Y_VAL, strTemp.GetString() );

		strTemp.Format ( "%.2f", vCamPos.z );
		SetWin_Text ( this, IDC_EDIT_CAM_Z_VAL, strTemp.GetString() );
	}
	else
	{
		MessageBox ( "Camera", "Coution" );
	}
}

void CEtcFuncPage::OnBnClickedCheckLandEffUse()
{
	// TODO: Add your control notification handler code here
	m_bLandEffUse = GetWin_Check ( this, IDC_CHECK_LAND_EFF_USE );
	SetEnableFunction ( EMETCFUNC_LANDEFFECT, m_bLandEffUse );
}

void CEtcFuncPage::OnNMClickListLandEff(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CEtcFuncPage::OnNMDblclkListLandEff(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	POSITION pos = m_list_LandEff.GetFirstSelectedItemPosition();
	int nItem = m_list_LandEff.GetNextSelectedItem ( pos );

	if ( nItem != -1 )
	{
		D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();
		D3DXVECTOR3 vLookAtPt = DxViewPort::GetInstance().GetLookatPt();

		D3DXVECTOR3 vNewLoockatPt = D3DXVECTOR3 ( 0.f, 1.0f, 0.f );
		SLANDEFFECT pLandEff = m_vecLandEffect[nItem];

		vNewLoockatPt.x = ( pLandEff.vMaxPos.x - pLandEff.vMinPos.x ) / 2.0f + pLandEff.vMinPos.x;
		vNewLoockatPt.z = ( pLandEff.vMaxPos.y - pLandEff.vMinPos.y ) / 2.0f + pLandEff.vMinPos.y;

		m_dwSelectNum = (DWORD)nItem;

		DxViewPort::GetInstance().CameraJump ( vNewLoockatPt );
	}
}

void CEtcFuncPage::OnBnClickedButtonLandEffAdd()
{
	// TODO: Add your control notification handler code here
	if ( m_vMinLandPos == D3DXVECTOR2 ( 0.f, 0.f ) &&
		 m_vMaxLandPos == D3DXVECTOR2 ( 0.f, 0.f ) )
	{
		MessageBox ( "Zone must be set.", "Coution" );
	}
	else
	{
		float fAddValue = (float)atof ( GetWin_Text ( this, IDC_EDIT_LAND_EFF_VAL ) );

		if ( fAddValue == 0.f )
		{
			MessageBox ( "% Will be 0.", "Coution" );
		}
		else
		{
			SLANDEFFECT landEffect;

			landEffect.emLandEffectType = (EMLANDEFFECT_TYPE)m_comob_LandEff.GetCurSel ();
			landEffect.fValue = fAddValue;

			D3DXVECTOR2 vMin, vMax;
			vMin.x = min( m_vMinLandPos.x, m_vMaxLandPos.x );
			vMin.y = min( m_vMinLandPos.y, m_vMaxLandPos.y );
			vMax.x = max( m_vMinLandPos.x, m_vMaxLandPos.x );
			vMax.y = max( m_vMinLandPos.y, m_vMaxLandPos.y );

			landEffect.vMinPos = vMin;
			landEffect.vMaxPos = vMax;

			m_vecLandEffect.push_back ( landEffect );

			m_dwSelectNum = -1;

			UpdateLandEffectList ();

			m_vMinLandPos = D3DXVECTOR2 ( 0.f, 0.f );
			m_vMaxLandPos = D3DXVECTOR2 ( 0.f, 0.f );
		}
	}
}

void CEtcFuncPage::OnBnClickedButtonLandEffDel()
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_list_LandEff.GetFirstSelectedItemPosition();
	int nItem = m_list_LandEff.GetNextSelectedItem ( pos );

	if ( nItem != -1 )
	{
		m_vecLandEffect.erase ( m_vecLandEffect.begin() + nItem );

		UpdateLandEffectList();

		if ( m_dwSelectNum == nItem )
			m_dwSelectNum = -1;

		m_list_LandEff.GetSelectedCount ();
	}
}

void CEtcFuncPage::OnBnClickedButtonEtcFuncSave()
{
	// TODO: Add your control notification handler code here
	SetEtcFuncData();
}

void CEtcFuncPage::OnBnClickedCheckCamTest()
{
	// TODO: Add your control notification handler code here
	m_bCamTest = GetWin_Check ( this, IDC_CHECK_CAM_TEST );
}
