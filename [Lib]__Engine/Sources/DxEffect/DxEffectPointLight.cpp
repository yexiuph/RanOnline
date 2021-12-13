#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectPointLight.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//---------------------------------------------------------------------[POINTLIGHT_PROPERTY]
const DWORD	POINTLIGHT_PROPERTY::VERSION	= 0x0101;
const char	POINTLIGHT_PROPERTY::NAME[]	= "Point Light";

DxEffSingle* POINTLIGHT_PROPERTY::NEWOBJ ()
{
	//DxEffectPointLight *pEffect = new DxEffectPointLight;
	DxEffectPointLight *pEffect = DxEffectPointLight::m_pPool->New();

	//	Note : TransPorm Set.		[���뼳��]
	pEffect->m_matLocal = m_matLocal;

	//	Note : �ð� ����.			[���뼳��]
	//
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : ���ư��� ������Ʈ	[���뼳��]
	//
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_fPower		= m_fPowerStart;
	pEffect->m_fPowerStart	= m_fPowerStart;

	pEffect->m_fPowerTime1 = m_fGBeginTime + m_fGLifeTime*m_fPowerRate1/100.f;
	pEffect->m_fPowerTime2 = m_fGBeginTime + m_fGLifeTime*m_fPowerRate2/100.f;

	if ( m_fPowerRate1 == 0.f )					pEffect->m_fPowerDelta1	= 0.f;
	else										pEffect->m_fPowerDelta1 = ( m_fPowerMid1 - m_fPowerStart ) / (pEffect->m_fPowerTime1-m_fGBeginTime);
	if ( m_fPowerRate2-m_fPowerRate1 == 0.f )	pEffect->m_fPowerDelta2	= 0.f;
	else										pEffect->m_fPowerDelta2 = ( m_fPowerMid2 - m_fPowerMid1 ) / (pEffect->m_fPowerTime2-pEffect->m_fPowerTime1);
	if ( m_fPowerRate2 == 100.f )				pEffect->m_fPowerDelta3	= 0.f;
	else										pEffect->m_fPowerDelta3 = ( m_fPowerEnd - m_fPowerMid2 - 0.1f ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fPowerTime2);

	pEffect->m_fScale		= m_fScaleStart;
	pEffect->m_fScaleStart	= m_fScaleStart;

	pEffect->m_fScaleTime1 = m_fGBeginTime + m_fGLifeTime*m_fScaleRate1/100.f;
	pEffect->m_fScaleTime2 = m_fGBeginTime + m_fGLifeTime*m_fScaleRate2/100.f;

	if ( m_fScaleRate1 == 0.f )					pEffect->m_fScaleDelta1	= 0.f;
	else										pEffect->m_fScaleDelta1 = ( m_fScaleMid1 - m_fScaleStart ) / (pEffect->m_fScaleTime1-m_fGBeginTime);
	if ( m_fScaleRate2-m_fScaleRate1 == 0.f )	pEffect->m_fScaleDelta2	= 0.f;
	else										pEffect->m_fScaleDelta2 = ( m_fScaleMid2 - m_fScaleMid1 ) / (pEffect->m_fScaleTime2-pEffect->m_fScaleTime1);
	if ( m_fScaleRate2 == 100.f )				pEffect->m_fScaleDelta3	= 0.f;
	else										pEffect->m_fScaleDelta3 = ( m_fScaleEnd - m_fScaleMid2 - 0.1f) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fScaleTime2);

	pEffect->m_cColor = m_cColor;

	return pEffect;
}

HRESULT POINTLIGHT_PROPERTY::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : �θ� Ŭ������ ����.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : ����, �ڽ� ����.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT POINTLIGHT_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if ( dwVer == 0x0100 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� �б�.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice );

	return S_OK;
}

HRESULT POINTLIGHT_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT POINTLIGHT_PROPERTY::DeleteDeviceObjects ()
{
	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

const DWORD	DxEffectPointLight::TYPEID		= EFFSINGLE_POINTLIGHT;
const DWORD	DxEffectPointLight::FLAG		= NULL;
const char	DxEffectPointLight::NAME[]		= "Point Light";

CMemPool<DxEffectPointLight> *DxEffectPointLight::m_pPool = NULL;

//	Note :
//
DxEffectPointLight::DxEffectPointLight(void) :
	m_vGLocation(0,0,0),

	m_fTime(0.0f),

	m_pd3dDevice(NULL)
{
}

DxEffectPointLight::~DxEffectPointLight(void)
{

}

HRESULT DxEffectPointLight::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }
HRESULT DxEffectPointLight::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }

void DxEffectPointLight::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectPointLight>;
}

void DxEffectPointLight::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
}

HRESULT DxEffectPointLight::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectPointLight::InvalidateDeviceObjects ()
{
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

void DxEffectPointLight::ReStartEff ()
{
	m_fGAge		= 0.f;
	m_fPower	= m_fPowerStart;
	m_fScale	= m_fScaleStart;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectPointLight::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff�� ���̸� ���.
	//
	m_fGAge += fElapsedTime;
	m_fTime += fElapsedTime;

	//	Note : �ݺ��� �� �ð� �� ���� ���� �ʱ�ȭ �Ѵ�.
	//
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_pThisGroup->m_fGBeginTime+m_pThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_fPower	= m_fPowerStart;
			m_fScale	= m_fScaleStart;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : ���۵��� �ʾҴ��� �˻�, ���� �ð��̸� Ȱ��ȭ ��Ŵ.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : ���� ������ �Ǿ����� �˻�.
	//
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_END;

	//	Note : �� ����
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fPowerTime1)
		m_fPower += (m_fPowerDelta1*fElapsedTime)/2;
	else if ( m_fGAge >= m_fPowerTime1 && m_fGAge < m_fPowerTime2)	
		m_fPower += (m_fPowerDelta2*fElapsedTime)/2;
	else														
		m_fPower += (m_fPowerDelta3*fElapsedTime)/2;

	if ( m_fPower < 0.f )		m_fPower = 0.f;

	//	Note : ������ ����
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fScaleTime1)
		m_fScale += (m_fScaleDelta1*fElapsedTime)/2;
	else if ( m_fGAge >= m_fScaleTime1 && m_fGAge < m_fScaleTime2)	
		m_fScale += (m_fScaleDelta2*fElapsedTime)/2;
	else														
		m_fScale += (m_fScaleDelta3*fElapsedTime)/2;

	if ( m_fScale < 0.f )		m_fScale = 0.f;


_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT	DxEffectPointLight::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld;

	D3DXMatrixMultiply ( &matWorld, &m_matLocal, &matComb );
	D3DXVECTOR3	vPos(matWorld._41,matWorld._42,matWorld._43);

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

	D3DLIGHTQ	Light;

	Light.Type			= D3DLIGHT_POINT;
	Light.Position		= vPos;
	Light.Diffuse.r		= m_cColor.r * m_fPower;
	Light.Diffuse.g		= m_cColor.g * m_fPower;
	Light.Diffuse.b		= m_cColor.b * m_fPower;
	Light.Ambient.r		= 0.f;
	Light.Ambient.g		= 0.f;
	Light.Ambient.b		= 0.f;
	Light.Specular.r	= 0.f;
	Light.Specular.g	= 0.f;
	Light.Specular.b	= 0.f;
	Light.Range			= m_fScale;
	Light.Attenuation0	= 1.0f;
	Light.Attenuation1	= 0.1f;
	Light.Attenuation2	= 0.0f;

	DxLightMan::GetInstance()->SetDPLight ( Light );


_RETURN:
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� ���.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	return S_OK;
}

HRESULT DxEffectPointLight::FinalCleanup()
{
	//	Note : ���� �ڽ� ���.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}
