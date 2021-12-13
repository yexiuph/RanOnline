#include "pch.h"

#include "./DxEffectMan.h"

#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxWeatherMan.h"
#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./EDITMESHS.h"

#include "./DxEffCharSingle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharSingle::TYPEID				= EMEFFCHAR_SINGLE;
DWORD		DxEffCharSingle::VERSION			= 0x0105;
char		DxEffCharSingle::NAME[MAX_PATH]		= "2.�ܵ� ����Ʈ";

DxEffCharSingle::DxEffCharSingle(void) :
	DxEffChar(),

	m_pPropGroup(NULL),
	m_vTransUp(0.f,0.f,0.f),
	m_vPrevUp(0.f,0.f,0.f),
	m_vTransDown(0.f,0.f,0.f),
	m_vBackUp(0.f,0.f,0.f),
	m_fTarDirection(0.f),
	m_vDirection(0,0,0),
	m_fCoolTime(10.f),
	m_fCoolTimeADD(0.f),
	m_vCurPos(0.f,0.f,0.f),
	m_pSkinMesh(NULL),
	m_fPlayTime(0.f)
{
	memset( m_szFileName, 0, sizeof(char)*MAX_PATH );
	memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
	memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );

	m_pSingleGroup = NULL;
}

DxEffCharSingle::~DxEffCharSingle(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharSingle::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharSingle *pEffCharSingle = new DxEffCharSingle;
	pEffCharSingle->SetLinkObj ( pCharPart, pSkinPiece );
	pEffCharSingle->SetProperty ( &m_Property );

	hr = pEffCharSingle->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffCharSingle);
		return NULL;
	}

	return pEffCharSingle;
}

HRESULT DxEffCharSingle::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharSingle::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharSingle::OneTimeSceneInit ()
{

	if ( m_pSingleGroup )		m_pSingleGroup->OneTimeSceneInit ();

	return S_OK;
}

HRESULT DxEffCharSingle::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_pSkinMesh;
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_pSkinMesh;
	}

	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName );
	if ( !m_pPropGroup )	return E_FAIL;

	if ( m_pPropGroup )
	{
		D3DXVECTOR3	vNormal;
	
		SAFE_DELETE(m_pSingleGroup);						// ���� �����ʹ� �����ش�.
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP ();
		if ( !m_pSingleGroup )	return E_FAIL;

		//	Note : Eff Group Create Dev.
		//
		m_pSingleGroup->Create ( pd3dDevice );

		//	Note : Eff Group�� ��Ʈ���� Ʈ������ ����.
		//
		if ( m_szTrace[0]==NULL )	return E_FAIL;
		
		if ( m_pCharPart )
		{
			hr = m_pCharPart->CalculateVertexInflu ( m_szTrace, m_vTransUp, vNormal );
			if ( FAILED(hr) )	return E_FAIL;
			D3DXMatrixTranslation ( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );
		}
		else if ( m_pSkinPiece )
		{
			hr = m_pSkinPiece->CalculateVertexInflu ( m_szTrace, m_vTransUp, vNormal );
			if ( FAILED(hr) )	return E_FAIL;
			D3DXMatrixTranslation ( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );
		}

		m_fPlayTime = 0.f;
		m_pSingleGroup->m_matWorld = m_matTrans;
	}

	if ( m_pSingleGroup )		m_pSingleGroup->InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharSingle::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSingleGroup )		m_pSingleGroup->RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharSingle::InvalidateDeviceObjects ()
{
	if ( m_pSingleGroup )		m_pSingleGroup->InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxEffCharSingle::DeleteDeviceObjects ()
{
	if ( m_pSingleGroup )		m_pSingleGroup->DeleteDeviceObjects ();

	return S_OK;
}

HRESULT DxEffCharSingle::FinalCleanup ()
{
	if ( m_pSingleGroup )		m_pSingleGroup->FinalCleanup();
	SAFE_DELETE(m_pSingleGroup);

	return S_OK;
}

HRESULT DxEffCharSingle::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	if ( !SetWeather() )		return S_OK;

	m_fTime = fTime;
	m_fPlayTime += fElapsedTime;

	if ( m_pSingleGroup )
	{
		m_pSingleGroup->FrameMove ( fTime, fElapsedTime );

		if ( m_dwFlag&USE_REPEAT )
		{
			m_fCoolTimeADD += fElapsedTime;
			if ( m_fCoolTimeADD > m_fCoolTime )
			{
				m_fCoolTimeADD = 0.f;
				m_pSingleGroup->ReStartEff();
			}
		}
		else
		{
			if ( m_pSingleGroup->m_dwAliveCount == 0 )
			{
				SetEnd();
			}
		}
	}

	return S_OK;
}

void	DxEffCharSingle::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )
{
	HRESULT hr = S_OK;
	D3DXVECTOR3 vNormalUp;
	D3DXVECTOR3 vNormalDown;
	D3DXMATRIX matVertex;

	if ( !SetWeather() )		return;

	if ( m_dwFlag&(USE_1POINT | USE_MATRIX) )
	{
		if ( m_pCharPart )
		{
			hr = m_pCharPart->CalculateVertexInflu ( m_szTrace, m_vTransUp, vNormalUp, &matVertex );
			if ( FAILED(hr) )	return;
		}
		else if ( m_pSkinPiece )
		{
			hr = m_pSkinPiece->CalculateVertexInflu ( m_szTrace, m_vTransUp, vNormalUp, &matVertex );
			if ( FAILED(hr) )	return;
		}
	}
	else if ( m_dwFlag&USE_2POINT )
	{
		if ( m_pCharPart )
		{
			hr = m_pCharPart->CalculateVertexInflu ( m_szTrace, m_vTransUp, vNormalUp, &matVertex );
			if ( FAILED(hr) )	return;

			hr = m_pCharPart->CalculateVertexInflu ( m_szTraceBack, m_vTransDown, vNormalDown, &matVertex );
			if ( FAILED(hr) )	return;
		}
		else if ( m_pSkinPiece )
		{
			hr = m_pSkinPiece->CalculateVertexInflu ( m_szTrace, m_vTransUp, vNormalUp, &matVertex );
			if ( FAILED(hr) )	return;

			hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceBack, m_vTransDown, vNormalDown, &matVertex );
			if ( FAILED(hr) )	return;
		}
	}
	

	BOOL bUse = TRUE;
	if ( (m_nAniMType==1) && (m_CurAniMType!=AN_ATTACK) )		bUse = FALSE;		// ���ݽ� ����
	else if ( (m_nAniMType==2) && (m_CurAniMType==AN_ATTACK) )	bUse = FALSE;	// ���� �ܿ� ����

	if( m_dwFlag&USE_1POINT )
	{
		m_pSingleGroup->m_bOut_Particle = bUse;

		if ( m_vBackUp != m_vTransUp )
		{
			m_vPrevUp = m_vBackUp;
			m_vBackUp = m_vTransUp;
		}


		D3DXMATRIX	matRotate;	

		D3DXMatrixRotationY ( &matRotate, m_fTarDirection+(D3DX_PI*0.5f) );

		m_vTransUp = m_vTransUp - (vNormalUp*m_fDepthUp);
		D3DXMatrixTranslation ( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );

		D3DXMatrixMultiply( &m_pSingleGroup->m_matWorld, &matRotate, &m_matTrans );
		
		m_pSingleGroup->m_fOut_Scale = m_fScale;				// ������

		if ( m_pSingleGroup )
			m_pSingleGroup->Render ( pd3dDevice );
	}
	else if( m_dwFlag&USE_2POINT )
	{
		m_pSingleGroup->m_bOut_Particle = bUse;

		if ( m_vBackUp != m_vTransUp )
		{
			m_vPrevUp = m_vBackUp;
			m_vBackUp = m_vTransUp;
		}

		m_pSingleGroup->m_vTargetPos	= m_vTransDown - (vNormalDown*m_fDepthDown);					// ��ǥ ��ġ
		m_pSingleGroup->m_vStartPos		= m_vTransUp - (vNormalUp*m_fDepthUp);							// ���� ��ġ
		m_pSingleGroup->m_vDirect		= m_pSingleGroup->m_vTargetPos - m_pSingleGroup->m_vStartPos;	// ����

		if( m_dwFlag&USE_REPEATMOVE )
		{
			m_vCurPos = m_pSingleGroup->m_vStartPos + ( m_pSingleGroup->m_vDirect * (sinf(m_fTime*m_fRepeatSpeed)+1.f)*0.5f);
		}
		else
		{
			if ( m_dwFlag&USE_AUTOMOVE )	m_vCurPos = m_pSingleGroup->m_vStartPos + ( m_pSingleGroup->m_vDirect * (m_fPlayTime*m_fRepeatSpeed));
			else							m_vCurPos = m_pSingleGroup->m_vStartPos;
		}
		
		m_pSingleGroup->m_matWorld._41 = m_vCurPos.x;
		m_pSingleGroup->m_matWorld._42 = m_vCurPos.y;
		m_pSingleGroup->m_matWorld._43 = m_vCurPos.z;

		if ( m_dwFlag&USE_AUTOMOVE )	m_pSingleGroup->m_bAutoMove = TRUE;
		else							m_pSingleGroup->m_bAutoMove = FALSE;

		m_pSingleGroup->m_bOut_Direct	= TRUE;			// ������� �־� �ش�.

		m_pSingleGroup->m_fOut_Scale	= m_fScale;		// ������

		m_pSingleGroup->m_matOutRotate	= DxBillboardLookAtDir ( &m_pSingleGroup->m_vDirect );			// �ܺ��� ��Ʈ����

		if ( m_pSingleGroup )
			m_pSingleGroup->Render ( pd3dDevice );
	}
	else if( m_dwFlag&USE_MATRIX )
	{
		// �ܵ� ����Ʈ ���⼺�� ������ �ʿ��Ҷ� ���
		m_pSingleGroup->m_bOut_Particle = bUse;

		if ( m_vBackUp != m_vTransUp )
		{
			m_vPrevUp = m_vBackUp;
			m_vBackUp = m_vTransUp;
		}


		D3DXMATRIX	matRotate, matRotateCombine ;
		D3DXMatrixIdentity( &matRotate );

		if ( m_pCharPart )
		{
			if( !m_pCharPart->m_pmcMesh->pSkinInfo )
				matRotateCombine = m_pCharPart->GetMatCombine();
			else
				matRotateCombine = matVertex;

			D3DXMatrixRotationY ( &matRotate, m_fTarDirection+(D3DX_PI*0.5f) );

			D3DXMatrixMultiply( &matRotate, &matRotate,  &matRotateCombine );
			
			matRotate._41 = 0.0f;
			matRotate._42 = 0.0f;
			matRotate._43 = 0.0f;
		}
		else
		{
			D3DXMatrixRotationY ( &matRotate, m_fTarDirection+(D3DX_PI*0.5f) );
		}
		
		m_vTransUp = m_vTransUp - (vNormalUp*m_fDepthUp);
		D3DXMatrixTranslation ( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );

		D3DXMatrixMultiply( &m_pSingleGroup->m_matWorld, &matRotate, &m_matTrans );
		
		m_pSingleGroup->m_fOut_Scale = m_fScale;				// ������

		if ( m_pSingleGroup )
			m_pSingleGroup->Render ( pd3dDevice );
	}

	m_vBackUp = m_vTransUp;
}

BOOL	DxEffCharSingle::SetWeather ()
{
	if ( m_nWeather == 1 )		// ������
	{
		if ( DxWeatherMan::GetInstance()->GetRain()->UseRain() ) return FALSE;
	}
	else if ( m_nWeather == 2 )		// ����³�
	{
		if ( !DxWeatherMan::GetInstance()->GetRain()->UseRain() ) return FALSE;
	}

	return TRUE;
}

void DxEffCharSingle::ConvertTracePoint()
{
	if( strcmp( m_szTrace, _T("ENERGY02") ) == 0 )
	{
		StringCchCopy( m_szTrace, STRACE_NSIZE, _T("ENERGY01") );
	}
	if( strcmp( m_szTraceBack, _T("ENERGY02") ) == 0 )
	{
		StringCchCopy( m_szTraceBack, STRACE_NSIZE, _T("ENERGY01") );
	}

	if( strcmp( m_szTrace, _T("IMAGE02") ) == 0 )
	{
		StringCchCopy( m_szTrace, STRACE_NSIZE, _T("IMAGE01") );
	}
	if( strcmp( m_szTraceBack, _T("IMAGE02") ) == 0 )
	{
		StringCchCopy( m_szTraceBack, STRACE_NSIZE, _T("IMAGE01") );
	}
}

HRESULT	DxEffCharSingle::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		//	Note : Device �ڿ��� �����Ѵ�.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;

	}
	else if( dwVer == 0x0104 )
	{
		EFFCHAR_PROPERTY_SINGLE_104 	Property;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_104) );

		m_dwPlaySlngle	= Property.m_dwPlaySlngle;
		m_nAniMType		= Property.m_nAniMType;
		m_nWeather		= Property.m_nWeather;
		m_fScale		= Property.m_fScale;

		//	Ver.103 ���� �߰�
		m_fDepthUp		= Property.m_fDepthUp;
		m_fDepthDown	= Property.m_fDepthDown;

		//	Ver.104 ���� �߰�
		m_dwFlag		= Property.m_dwFlag;
		m_fCoolTime		= Property.m_fCoolTime;

		//	Ver.105 ���� �߰�
		m_fRepeatSpeed	= 1.f;
		if ( Property.m_nType )	
		{
			m_dwFlag |= USE_2POINT;
			m_dwFlag &= ~USE_1POINT;

			m_dwFlag |= USE_AUTOMOVE;
		}
		else
		{
			m_dwFlag |= USE_1POINT;
			m_dwFlag &= ~USE_2POINT;
		}

		StringCchCopy(m_szFileName,		MAX_PATH, Property.m_szFileName);	
		StringCchCopy(m_szTrace,		STRACE_NSIZE, Property.m_szTrace);	
		StringCchCopy(m_szTraceBack,	STRACE_NSIZE, Property.m_szTraceBack);

		//	Note : Device �ڿ��� �����Ѵ�.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0103 )
	{
		EFFCHAR_PROPERTY_SINGLE_103 	Property;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_103) );

		m_dwPlaySlngle	= Property.m_dwPlaySlngle;
		m_nAniMType		= Property.m_nAniMType;
		m_nWeather		= Property.m_nWeather;
		m_fScale		= Property.m_fScale;

		//	Ver.103 ���� �߰�
		m_fDepthUp		= Property.m_fDepthUp;
		m_fDepthDown	= Property.m_fDepthDown;

		//	Ver.104 ���� �߰�
		m_dwFlag		= 0L;
		m_fCoolTime		= 10.f;

		//	Ver.105 ���� �߰�
		m_fRepeatSpeed	= 1.f;
		if ( Property.m_nType )	
		{
			m_dwFlag |= USE_2POINT;
			m_dwFlag &= ~USE_1POINT;

			m_dwFlag |= USE_AUTOMOVE;
		}
		else
		{
			m_dwFlag |= USE_1POINT;
			m_dwFlag &= ~USE_2POINT;
		}

		StringCchCopy(m_szFileName,		MAX_PATH, Property.m_szFileName);	
		StringCchCopy(m_szTrace,		STRACE_NSIZE, Property.m_szTrace);	
		StringCchCopy(m_szTraceBack,	STRACE_NSIZE, Property.m_szTraceBack);

		//	Note : Device �ڿ��� �����Ѵ�.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0102 )
	{
		EFFCHAR_PROPERTY_SINGLE_102		Property;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_102) );

		m_dwPlaySlngle	= Property.m_dwPlaySlngle;
		m_nAniMType		= Property.m_nAniMType;
		m_nWeather		= Property.m_nWeather;
		m_fScale		= Property.m_fScale;

		//	Ver.103 ���� �߰�
		m_fDepthUp		= 0.5f;
		m_fDepthDown	= 0.5f;

		//	Ver.104 ���� �߰�
		m_dwFlag		= 0L;
		m_fCoolTime		= 10.f;

		//	Ver.105 ���� �߰�
		m_fRepeatSpeed	= 1.f;
		if ( Property.m_nType )	
		{
			m_dwFlag |= USE_2POINT;
			m_dwFlag &= ~USE_1POINT;

			m_dwFlag |= USE_AUTOMOVE;
		}
		else
		{
			m_dwFlag |= USE_1POINT;
			m_dwFlag &= ~USE_2POINT;
		}

		StringCchCopy(m_szFileName,		MAX_PATH, Property.m_szFileName);	
		StringCchCopy(m_szTrace,		STRACE_NSIZE, Property.m_szTrace);	
		StringCchCopy(m_szTraceBack,	STRACE_NSIZE, Property.m_szTraceBack);

		//	Note : Device �ڿ��� �����Ѵ�.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0101 )
	{
		EFFCHAR_PROPERTY_SINGLE_101		Property;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_101) );

		m_nWeather	= Property.m_nWeather;						//	����..
		m_nAniMType = 0;					//	���� Ÿ��
		m_fScale = 1.f;

		//	Ver.103 ���� �߰�
		m_fDepthUp		= 0.5f;
		m_fDepthDown	= 0.5f;

		//	Ver.104 ���� �߰�
		m_dwFlag		= 0L;
		m_fCoolTime		= 10.f;

		//	Ver.105 ���� �߰�
		m_fRepeatSpeed	= 1.f;
		if ( Property.m_nType )	
		{
			m_dwFlag |= USE_2POINT;
			m_dwFlag &= ~USE_1POINT;

			m_dwFlag |= USE_AUTOMOVE;
		}
		else
		{
			m_dwFlag |= USE_1POINT;
			m_dwFlag &= ~USE_2POINT;
		}

		StringCchCopy(m_szFileName,		MAX_PATH, Property.m_szFileName);	
		StringCchCopy(m_szTrace,		STRACE_NSIZE, Property.m_szTrace);	
		StringCchCopy(m_szTraceBack,	STRACE_NSIZE, Property.m_szTraceBack);

		//	Note : Device �ڿ��� �����Ѵ�.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x0100 )
	{
		EFFCHAR_PROPERTY_SINGLE_100		Property;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_100) );

		m_nWeather	= Property.m_nWeather;						//	����..
		m_nAniMType = 0;					//	���� Ÿ��
		m_fScale = 1.f;

		//	Ver.103 ���� �߰�
		m_fDepthUp		= 0.5f;
		m_fDepthDown	= 0.5f;

		//	Ver.104 ���� �߰�
		m_dwFlag		= 0L;
		m_fCoolTime		= 10.f;

		//	Ver.105 ���� �߰�
		m_fRepeatSpeed	= 1.f;
		if ( Property.m_nType )	
		{
			m_dwFlag |= USE_2POINT;
			m_dwFlag &= ~USE_1POINT;

			m_dwFlag |= USE_AUTOMOVE;
		}
		else
		{
			m_dwFlag |= USE_1POINT;
			m_dwFlag &= ~USE_2POINT;
		}

		StringCchCopy(m_szFileName,		MAX_PATH, Property.m_szFileName);	
		StringCchCopy(m_szTrace,		STRACE_NSIZE, Property.m_szTrace);	
		StringCchCopy(m_szTraceBack,	STRACE_NSIZE, Property.m_szTraceBack);

		//	Note : Device �ڿ��� �����Ѵ�.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharSingle::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

