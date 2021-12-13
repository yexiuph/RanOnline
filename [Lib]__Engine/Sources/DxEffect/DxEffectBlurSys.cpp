#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"
#include "./ShaderConstant.h"
#include "./DxDynamicVB.h"
#include "./DxEnvironment.h"
#include "./DxSurfaceTex.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectBlurSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------[BLURSYS_PROPERTY]
const DWORD	BLURSYS_PROPERTY::VERSION	= 0x0102;
const char	BLURSYS_PROPERTY::NAME[]	= "�ܻ�";

DxEffSingle* BLURSYS_PROPERTY::NEWOBJ ()
{
	//DxEffectBlurSys *pEffect = new DxEffectBlurSys;
	DxEffectBlurSys *pEffect = DxEffectBlurSys::m_pPool->New();

	//	Note : TransPorm Set.		[���뼳��]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : �ð� ����.			[���뼳��]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : ���ư��� ������Ʈ	[���뼳��]
	pEffect->m_bMoveObj		= m_bMoveObj;

	pEffect->m_dwFlag		= m_dwFlag;

	pEffect->m_nBlend		= m_nBlend;

	pEffect->m_fLife		= m_fLife;

	pEffect->m_fLength		= m_fLengthStart;
	pEffect->m_fLengthDelta	= (m_fLengthEnd-m_fLengthStart)/m_fLife;

	pEffect->m_cColor = m_cColorStart;
	pEffect->m_cColorDelta = (m_cColorEnd-m_cColorStart)/m_fLife;

	pEffect->m_cColor.a = m_fAlphaStart;
	pEffect->m_cColorDelta.a = (m_fAlphaEnd-m_fAlphaStart)/m_fLife;

	//	Note : �ؽ��� ����.
	pEffect->m_strTexture = m_szTexture;
	pEffect->m_pTexture = m_pTexture;

	return pEffect;
}

HRESULT BLURSYS_PROPERTY::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : �θ� Ŭ������ ����.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : ����Ʈ�� Property �� ����.
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : ����, �ڽ� ����.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT BLURSYS_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if( dwVer == VERSION )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� Load.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if( dwVer == 0x101 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� Load.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if( dwVer == 0x100 )
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
		PROPERTY_100 pProp;
		SFile.ReadBuffer ( &pProp, sizeof(PROPERTY_100) );

		if ( pProp.m_bAbsolute )	m_dwFlag |= USEABSOLUTE;

		m_nBlend		= pProp.m_nBlend;

		m_fLife			= pProp.m_fLife;

		m_fLengthStart	= pProp.m_fLengthStart;
		m_fLengthEnd	= pProp.m_fLengthEnd;

		m_fAlphaStart	= pProp.m_fAlphaStart;
		m_fAlphaEnd		= pProp.m_fAlphaEnd;

		m_cColorStart	= pProp.m_cColorStart;
		m_cColorEnd		= pProp.m_cColorEnd;

		StringCchCopy( m_szTexture, 256, pProp.m_szTexture );
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

HRESULT BLURSYS_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pTexture, 0, 0, TRUE );

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT BLURSYS_PROPERTY::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [���� �ɹ� ���� �� �ʱ�ȭ] ----------------------------------------------
//
//
const DWORD	DxEffectBlurSys::TYPEID	= EFFSINGLE_BLURSYS;
const DWORD	DxEffectBlurSys::FLAG		= NULL;
const char	DxEffectBlurSys::NAME[]	= "�ܻ�";

LPDIRECT3DSTATEBLOCK9	DxEffectBlurSys::m_pSavedSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectBlurSys::m_pEffectSB = NULL;

CMemPool<DxEffectBlurSys> *DxEffectBlurSys::m_pPool = NULL;

DxEffectBlurSys::VERTEX_UPDOWN_POOL*	DxEffectBlurSys::m_pVertexUpDownPool = NULL;
DxEffectBlurSys::HERMITEPOINT_POOL*		DxEffectBlurSys::m_pHermitePointPool = NULL;
DxEffectBlurSys::BLUR_UPDOWN_POOL*		DxEffectBlurSys::m_pBlurUpDownPool = NULL;

HRESULT DxEffectBlurSys::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		// ��� ���ϴ� �� ( Default�� �����ؾ� ��. )
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectSB );
	}

	return S_OK;
}

HRESULT DxEffectBlurSys::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pEffectSB );

	return S_OK;
}

void DxEffectBlurSys::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectBlurSys>;


	m_pVertexUpDownPool = new CMemPool<VERTEX_UPDOWN>;
	m_pBlurUpDownPool = new CMemPool<BLUR_UPDOWN>;
	m_pHermitePointPool = new CMemPool<HERMITEPOINT>;
}

void DxEffectBlurSys::OnDeleteDevice_STATIC()
{
	SAFE_DELETE( m_pVertexUpDownPool );
	SAFE_DELETE( m_pBlurUpDownPool );
	SAFE_DELETE( m_pHermitePointPool );

	SAFE_DELETE(m_pPool);
}


HRESULT DxEffectBlurSys::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectBlurSys::InvalidateDeviceObjects ()
{
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [Ŭ���� ��ü] ------------------------------------------------------------

DxEffectBlurSys::DxEffectBlurSys () :
	m_bBaseBlurRender(FALSE),
	m_dwKeyPointCOUNT(0),

	m_pd3dDevice(NULL),
	m_pTexture(NULL)
{
	m_fGLifeTime	= 0.0f;
	m_fTime			= 0.f;
	m_fTime30		= 0.f;
	m_fElapsedTime	= 0.f;

	D3DXMatrixIdentity( &m_matIdentity );
}

DxEffectBlurSys::~DxEffectBlurSys ()
{
	//LPPARTICLE	pPaticle;
	//pPaticle = m_pParticleHead;
	//while ( pPaticle )
	//{
	//	m_pParticleHead = pPaticle->m_pNext;
	//	SAFE_DELETE ( pPaticle );
	//	pPaticle = m_pParticleHead;
	//}

	// Note : Key Data�� clear �Ѵ�.
	ClearKeyFrame();

	// Note : Hermite Data�� clear �Ѵ�.
	ClearHermitePoint();

	// Note : Blur Data�� clear �Ѵ�.
	ClearBlurData();

	CleanUp ();
}

HRESULT DxEffectBlurSys::FinalCleanup()
{
	//	Note : ���� �ڽ� ���.
	//
	DxEffSingle::FinalCleanup ();

	// Note : Key Data�� clear �Ѵ�.
	ClearKeyFrame();

	// Note : Hermite Data�� clear �Ѵ�.
	ClearHermitePoint();

	// Note : Blur Data�� clear �Ѵ�.
	ClearBlurData();

	return S_OK;
}

void DxEffectBlurSys::ReStartEff ()
{
	m_fGAge		= 0.f;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;

	if ( m_dwFlag & USE_LOOP_RESET )
	{
		// Note : Key Data�� clear �Ѵ�.
		ClearKeyFrame();

		// Note : Hermite Data�� clear �Ѵ�.
		ClearHermitePoint();

		// Note : Blur Data�� clear �Ѵ�.
		ClearBlurData();
	}

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectBlurSys::FrameMove ( float fTime, float fElapsedTime )
{
	D3DXVECTOR3		vDeltaGVel;
	D3DXVECTOR3		vLocal;
	D3DXMATRIX		matLocal;

	//	Note : Eff�� ���̸� ���.
	//
	m_fGAge += fElapsedTime;
	m_fTime += fElapsedTime;
	m_fTime30 += fElapsedTime;
	m_fElapsedTime = fElapsedTime;

	//	Note : ��ü���� ���İ��� ���߾��ش�.
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		m_fAlpha = 1.f;
	}
	else
	{
		m_fAlpha = (m_fGAge-m_fGBeginTime)/m_fGLifeTime;		// 0~1
		if ( m_fAlpha < 0.f )	m_fAlpha = 0.f;
		if ( m_fAlpha > 1.f )	m_fAlpha = 1.f;
		m_fAlpha = (m_fAlpha<0.8f) ? 0.f: (m_fAlpha-0.8f)*5.f;	// 0.8f �� ����̴�.
		m_fAlpha = 1.f-m_fAlpha;								// 1�� 80%, 1~0 �� 20%
	}

	//	Note : �ݺ��� �� �ð� �� ���� ���� �ʱ�ȭ �Ѵ�.
	//
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_pThisGroup->m_fGBeginTime+m_pThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;

			if ( m_dwFlag & USE_LOOP_RESET )
			{
				// Note : Key Data�� clear �Ѵ�.
				ClearKeyFrame();

				// Note : Hermite Data�� clear �Ѵ�.
				ClearHermitePoint();

				// Note : Blur Data�� clear �Ѵ�.
				ClearBlurData();
			}
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
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_DONE;

	if ( m_dwRunFlag & EFF_PLY_DONE )
	{
		ClearKeyFrame();

		if( m_deqKeyPoint.empty() )
		{
			m_dwRunFlag |= EFF_PLY_END;
		}
	}


_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

    return S_OK;
}

HRESULT DxEffectBlurSys::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;
	D3DXMATRIX	matWorld, matIdentity, matTrans, matRotate, matInverse, matTemp;

	float		fLength(0.f);
	float		fPer(0.f);
	float		fTEXUV(0.f);
	float		Div_1_DrawPoints(0.f);
	VERTEX		sVertexTEMP[2];
	D3DXVECTOR3	vTemp(0.f,0.f,0.f);

	D3DXVECTOR3		vParent(0.f,0.f,0.f), vDirec(0.f,0.f,0.f);
	D3DXVECTOR3		vCarDirec;
	D3DXVECTOR3		vDirection;
	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3&	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();

	if ( !m_pThisGroup->m_bOut_Particle )	goto _RETURN;
	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

	D3DXMatrixIdentity ( &matTrans );
	D3DXMatrixIdentity ( &m_matResult );
	matTrans._41 = matComb._41;
	matTrans._42 = matComb._42;
	matTrans._43 = matComb._43;
	matRotate = matComb;
	matRotate._41 = 0.f;
	matRotate._42 = 0.f;
	matRotate._43 = 0.f;
	m_matResult._41 = m_matLocal._41*m_pThisGroup->m_fOut_Scale;
	m_matResult._42 = m_matLocal._42*m_pThisGroup->m_fOut_Scale;
	m_matResult._43 = m_matLocal._43*m_pThisGroup->m_fOut_Scale;

	D3DXMatrixMultiply ( &m_matResult, &m_matResult, &matRotate );
	D3DXMatrixMultiply ( &m_matResult, &m_matResult, &matTrans );

	D3DXMatrixIdentity ( &matIdentity );

	//	�θ�༮�� ������� ������ �Ϳ� ���� ����
	//
	if ( m_dwFlag & USEABSOLUTE )
	{
		vParent.x = m_matResult._41;
		vParent.y = m_matResult._42;
		vParent.z = m_matResult._43;

		pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
	}
	else
	{
		m_matResult = m_pThisGroup->m_matOutRotate;		

		//	Note: �ܺο��� ������ �ְ� �̵��� ���.
		if( m_pThisGroup->m_bAutoMove )
		{
			fPer	= (m_fGAge-m_fGBeginTime)/m_fGLifeTime;
			m_matResult._41 = m_pThisGroup->m_vStartPos.x + (m_pThisGroup->m_vDirect.x*fPer);
			m_matResult._42 = m_pThisGroup->m_vStartPos.y + (m_pThisGroup->m_vDirect.y*fPer);
			m_matResult._43 = m_pThisGroup->m_vStartPos.z + (m_pThisGroup->m_vDirect.z*fPer);
			fLength = D3DXVec3Length ( &m_pThisGroup->m_vDirect );
		}

		D3DXMatrixInverse ( &matInverse, NULL, &m_matResult );			// ��
		D3DXMatrixMultiply ( &matWorld, &matComb, &matInverse  );		// �θ�

		vParent.x = m_matLocal._41*m_pThisGroup->m_fOut_Scale;
		vParent.y = m_matLocal._42*m_pThisGroup->m_fOut_Scale;
		vParent.z = m_matLocal._43*m_pThisGroup->m_fOut_Scale;

		D3DXVec3TransformCoord ( &vParent, &vParent, &matWorld );		// �θ��� ȸ���� ������ �޾Ҵ�.

		vParent.y += fLength*fPer;					// ������ ���̸� �ø���.

		if( m_pThisGroup->m_bAutoMove )
		{
			m_matResult._41 = m_pThisGroup->m_vStartPos.x;
			m_matResult._42 = m_pThisGroup->m_vStartPos.y;
			m_matResult._43 = m_pThisGroup->m_vStartPos.z;
		}

		pd3dDevice->SetTransform ( D3DTS_WORLD, &m_matResult );
	}


	// Note : 30fps�� �����Ѵ�... Ǯ�� ������.. ���� ��ȭ���� ���� ����� ���δ�.
	if( m_fTime30 > 0.033f )
	{
		m_fTime30 = 0.f;

		D3DXVECTOR3 vUp(0.f,0.f,0.f);
		D3DXVECTOR3 vDown(0.f,0.f,0.f);

		// Note : �ϴ� �� �����.
		ClearBlurData();

		// Note : ���� ��ġ�� ��´�. ���� ���ٸ� �����Ŷ� �������� �ʴ´�.
		if( !IsUnionKey( vParent ) )	goto _IF_OVER;

		// Note : �ʱ�ȭ.
		m_bBaseBlurRender = FALSE;

		// Note : ó���� ����
		FrameMoveKeyFrame();

		// Note : Hermite ����� ���� FrameMove ��.
		FrameMoveHermitePoint( m_fElapsedTime );

		// 
		if( !(m_dwRunFlag&EFF_PLY_DONE) && !(m_dwRunFlag&EFF_PLY_END) )
		{
			CreateNewKeyFrame( vParent );	// m_deqKeyPoint �� �ִ´�.
			CreateHermite();
		}
		else
		{
			ClearKeyFrame();	// �ִϸ��̼��� �ٲ� ���� ����� Key �����͸� �����Ѵ�.
		}

		CreateBlur();			// �ܻ��� ����.
		FrameMoveBlur();		// Ȱ��.
	}

_IF_OVER:		// ������ �ʿ���� ��� �پ� ����

	if( m_dwFlag & USEREFRACT )
	{
		DxEnvironment::GetInstance().SetRefractPlayON();	// Ȱ��ȭ
		pd3dDevice->SetTexture( 0, DxSurfaceTex::GetInstance().m_pWaveTex );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	}
	else
	{
		// Note : Get Texture
		if( !m_pTexture )	TextureManager::GetTexture( m_strTexture.c_str(), m_pTexture );
		if( !m_pTexture )	goto _RETURN;

		pd3dDevice->SetTexture( 0, m_pTexture );
	}



	m_pSavedSB->Capture();
	m_pEffectSB->Apply();

	//	Note : ���� ��� ����
	switch ( m_nBlend )
	{
	case 1 :
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		break;
	case 2:
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );
		break;
	case 3:
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );
		break;
	case 5:
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );

		break;
	}

	RenderBlur( pd3dDevice );

	m_pSavedSB->Apply();



_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matComb );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	return hr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//																K	e	y
//------------------------------------------------------------------------------------------------------------------------------------------------
BOOL DxEffectBlurSys::IsUnionKey( const D3DXVECTOR3& vBase )
{
	float fDiff(0.1f);
	if( !m_deqKeyPoint.empty() )
	{
		// ���� ��ġ�� ��� ���� ���� �ʴ´�.
		if( m_deqKeyPoint[0]->sPos.x <= vBase.x+fDiff && m_deqKeyPoint[0]->sPos.x >= vBase.x-fDiff &&
			m_deqKeyPoint[0]->sPos.y <= vBase.y+fDiff && m_deqKeyPoint[0]->sPos.y >= vBase.y-fDiff &&
			m_deqKeyPoint[0]->sPos.z <= vBase.z+fDiff && m_deqKeyPoint[0]->sPos.z >= vBase.z-fDiff )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void DxEffectBlurSys::CreateNewKeyFrame( D3DXVECTOR3& vBase )
{
	// Note : ���� �ִ´�.
	VERTEX_UPDOWN* pNew = m_pVertexUpDownPool->New();
	pNew->sPos = vBase;

	m_deqKeyPoint.push_front( pNew );
	++m_dwKeyPointCOUNT;
}

void DxEffectBlurSys::FrameMoveKeyFrame()
{
	VERTEX_UPDOWN* pVector(NULL);
	while( m_deqKeyPoint.size() >= 4 )
	{
		pVector = m_deqKeyPoint.back();

		m_pVertexUpDownPool->Release( pVector );

		m_deqKeyPoint.pop_back();
		--m_dwKeyPointCOUNT;
	}
}

void DxEffectBlurSys::ClearKeyFrame()
{
	if( !m_pVertexUpDownPool )	return;

	for( DWORD i=0; i<m_deqKeyPoint.size(); ++i )
	{
		m_pVertexUpDownPool->Release( m_deqKeyPoint[i] );
	}
	m_deqKeyPoint.clear();

	m_dwKeyPointCOUNT = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//												H	e	r	m	i	t	e		P	o	i	n	t
//------------------------------------------------------------------------------------------------------------------------------------------------
void DxEffectBlurSys::CreateHermite()
{
	int nNUM(0);
	float fAlpha(1.f);
	D3DXVECTOR3 vLookatCross(0.f,0.f,0.f);
	D3DXVECTOR3 vPos00(0.f,0.f,0.f);
	D3DXVECTOR3 vPos01(0.f,0.f,0.f);

	// Key�� ������ ���� ������ Ʋ���� ��.
	switch( m_dwKeyPointCOUNT ) 
	{
	case 1:
		break;

	case 2:
		m_bBaseBlurRender = TRUE;

		if ( m_dwFlag & USEABSOLUTE )
		{
			GetLookatCross( m_deqKeyPoint[1]->sPos, m_deqKeyPoint[0]->sPos, vLookatCross );
		}
		else
		{
			D3DXVec3TransformCoord( &vPos00, &m_deqKeyPoint[0]->sPos, &m_matResult );
			D3DXVec3TransformCoord( &vPos01, &m_deqKeyPoint[1]->sPos, &m_matResult );

			GetLookatCross( vPos01, vPos00, vLookatCross );
		}
		vLookatCross = vLookatCross * m_fLength;

		m_sBaseBlur[0].vPos = m_deqKeyPoint[0]->sPos + vLookatCross;
		m_sBaseBlur[1].vPos = m_deqKeyPoint[0]->sPos - vLookatCross;
		m_sBaseBlur[2].vPos = m_deqKeyPoint[1]->sPos + vLookatCross;
		m_sBaseBlur[3].vPos = m_deqKeyPoint[1]->sPos - vLookatCross;

		DxInsertHermitePoint( m_deqKeyPoint[1]->sPos );
		break;

	case 3:
		m_bBaseBlurRender = TRUE;

		if ( m_dwFlag & USEABSOLUTE )
		{
			GetLookatCross( m_deqKeyPoint[1]->sPos, m_deqKeyPoint[0]->sPos, vLookatCross );
		}
		else
		{
			D3DXVec3TransformCoord( &vPos00, &m_deqKeyPoint[0]->sPos, &m_matResult );
			D3DXVec3TransformCoord( &vPos01, &m_deqKeyPoint[1]->sPos, &m_matResult );

			GetLookatCross( vPos01, vPos00, vLookatCross );
		}
		vLookatCross = vLookatCross * m_fLength;

		m_sBaseBlur[0].vPos = m_deqKeyPoint[0]->sPos + vLookatCross;
		m_sBaseBlur[1].vPos = m_deqKeyPoint[0]->sPos - vLookatCross;
		m_sBaseBlur[2].vPos = m_deqKeyPoint[1]->sPos + vLookatCross;
		m_sBaseBlur[3].vPos = m_deqKeyPoint[1]->sPos - vLookatCross;

		DxInsertHermitePoint( m_deqKeyPoint[1]->sPos );
		break;

	case 4:
		m_bBaseBlurRender = TRUE;

		if ( m_dwFlag & USEABSOLUTE )
		{
			GetLookatCross( m_deqKeyPoint[1]->sPos, m_deqKeyPoint[0]->sPos, vLookatCross );
		}
		else
		{
			D3DXVec3TransformCoord( &vPos00, &m_deqKeyPoint[0]->sPos, &m_matResult );
			D3DXVec3TransformCoord( &vPos01, &m_deqKeyPoint[1]->sPos, &m_matResult );

			GetLookatCross( vPos01, vPos00, vLookatCross );
		}
		vLookatCross = vLookatCross * m_fLength;

		m_sBaseBlur[0].vPos = m_deqKeyPoint[0]->sPos + vLookatCross;
		m_sBaseBlur[1].vPos = m_deqKeyPoint[0]->sPos - vLookatCross;
		m_sBaseBlur[2].vPos = m_deqKeyPoint[1]->sPos + vLookatCross;
		m_sBaseBlur[3].vPos = m_deqKeyPoint[1]->sPos - vLookatCross;

		// Note : �߰� �ܰ� ����.
		nNUM = SeparateNumOfLine();
		switch( nNUM )
		{
		case 0:
			break;

		case 1:
			DxInsertHermitePoint( 0.5f );
			break;

		case 2:
			DxInsertHermitePoint( 0.66f );
			DxInsertHermitePoint( 0.33f );
			break;

		case 3:
			DxInsertHermitePoint( 0.75f );
			DxInsertHermitePoint( 0.5f );
			DxInsertHermitePoint( 0.25f );
			break;

		case 4:
			DxInsertHermitePoint( 0.8f );
			DxInsertHermitePoint( 0.6f );
			DxInsertHermitePoint( 0.4f );
			DxInsertHermitePoint( 0.2f );
			break;

		case 5:
			DxInsertHermitePoint( 0.83f );
			DxInsertHermitePoint( 0.66f );
			DxInsertHermitePoint( 0.5f );
			DxInsertHermitePoint( 0.33f );
			DxInsertHermitePoint( 0.17f );
			break;

		case 6:
			DxInsertHermitePoint( 0.858f );
			DxInsertHermitePoint( 0.715f );
			DxInsertHermitePoint( 0.572f );
			DxInsertHermitePoint( 0.429f );
			DxInsertHermitePoint( 0.286f );
			DxInsertHermitePoint( 0.143f );
			break;

		case 7:
			fAlpha = 1.f;
			DxInsertHermitePoint( 0.875f );
			DxInsertHermitePoint( 0.75f );
			DxInsertHermitePoint( 0.625f );
			DxInsertHermitePoint( 0.5f );
			DxInsertHermitePoint( 0.375f );
			DxInsertHermitePoint( 0.25f );
			DxInsertHermitePoint( 0.125f );
			break;

		case 15:
			fAlpha = 1.f;					fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertHermitePoint( fAlpha );
			break;

		default:
			CDebugSet::ToLogFile( "SeparateNumOfLine()���� �Ѿ���� �� ����" );
		};

		DxInsertHermitePoint( m_deqKeyPoint[1]->sPos );

		break;
	}
}

int DxEffectBlurSys::SeparateNumOfLine()
{
	D3DXVECTOR3 vLine[2];
	vLine[0] = m_deqKeyPoint[0]->sPos - m_deqKeyPoint[2]->sPos;
	vLine[1] = m_deqKeyPoint[1]->sPos - m_deqKeyPoint[3]->sPos;

	D3DXVec3Normalize( &vLine[0], &vLine[0] );
	D3DXVec3Normalize( &vLine[1], &vLine[1] );

	float fDot = D3DXVec3Dot( &vLine[0], &vLine[1] );

	int nData(0);
	if( fDot > 0.97f )			nData = 0;
	else if( fDot > 0.93f )		nData = 1;
	else if( fDot > 0.89f )		nData = 2;
	else if( fDot > 0.83f )		nData = 3;
	else if( fDot > 0.75f )		nData = 4;
	else if( fDot > 0.55f )		nData = 5;
	else if( fDot > 0.3f )		nData = 6;
	else if( fDot > 0.0f )		nData = 7;
	else						nData = 15;

	// ĳ���� �������� ���ٸ� ������ ���� ���� �ʴ´�.
	if( DxEffectMan::GetInstance().GetSkinDetail() == SKD_SPEC )
	{
		if( nData==0 )			nData = 0;
		else if( nData==1 )		nData = 0;
		else if( nData==2 )		nData = 1;
		else if( nData==3 )		nData = 2;
		else if( nData==4 )		nData = 3;
		else if( nData==5 )		nData = 4;
		else if( nData==6 )		nData = 5;
		else if( nData==7 )		nData = 6;
		else if( nData==15 )	nData = 7;
	}
	else if( DxEffectMan::GetInstance().GetSkinDetail() == SKD_NONE )
	{
		if( nData==0 )			nData = 0;
		else if( nData==1 )		nData = 0;
		else if( nData==2 )		nData = 1;
		else if( nData==3 )		nData = 1;
		else if( nData==4 )		nData = 2;
		else if( nData==5 )		nData = 2;
		else if( nData==6 )		nData = 3;
		else if( nData==7 )		nData = 3;
		else if( nData==15 )	nData = 4;
	}

	return nData;
}

void DxEffectBlurSys::DxInsertHermitePoint( const D3DXVECTOR3& vPos )
{
	// Note : ó�� ���� ������ ���
	if( m_deqHermitePoint.empty() )
	{
		// Note : ������ ����.
		HERMITEPOINT* pNew = m_pHermitePointPool->New();
		pNew->fLife = m_fLife;
		pNew->fLength = m_fLength;
		pNew->cColor = m_cColor;
		pNew->vBasePos = vPos;
		pNew->vLookatCross = D3DXVECTOR3( 0.f, 0.f, 0.f );	// ���̸� �÷��� �þ�� �ʵ��� �Ѵ�.
		m_deqHermitePoint.push_back( pNew );
	}
	else
	{
		// ���� ���� ã�´�.
		HERMITEPOINT* pPrevData = m_deqHermitePoint.back();

		// Note : ������ ����.
		HERMITEPOINT* pNew = m_pHermitePointPool->New();
		pNew->fLife = m_fLife;
		pNew->fLength = m_fLength;
		pNew->cColor = m_cColor;
		pNew->vBasePos = vPos;
		GetLookatCross( pPrevData->vBasePos, pNew->vBasePos, pNew->vLookatCross );
		m_deqHermitePoint.push_back( pNew );
	}
}

void DxEffectBlurSys::DxInsertHermitePoint( float fRate )
{
	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	D3DXVec3CatmullRom( &vPos, &m_deqKeyPoint[0]->sPos, &m_deqKeyPoint[1]->sPos, 
								&m_deqKeyPoint[2]->sPos, &m_deqKeyPoint[3]->sPos, fRate );

	DxInsertHermitePoint( vPos );
}

void DxEffectBlurSys::GetLookatCross( const D3DXVECTOR3& vBase, const D3DXVECTOR3& vTarget, D3DXVECTOR3& vLookatCross )
{
	D3DXVECTOR3 vDirection = vTarget - vBase;
	D3DXVECTOR3 vCarDirec = DxViewPort::GetInstance().GetLookDir();

	D3DXVec3Cross( &vLookatCross, &vDirection, &vCarDirec );
	D3DXVec3Normalize( &vLookatCross, &vLookatCross );
}

void DxEffectBlurSys::FrameMoveHermitePoint( float fElapsedTime )
{
	// �ð��� �긮��,,
	DWORD dwSize = (DWORD)m_deqHermitePoint.size();
	for( DWORD i=0; i<dwSize; ++i )
	{
		m_deqHermitePoint[i]->fLife -= fElapsedTime;
	}

	// �ð� �� �� ���� �����ش�.
	// �� �ð��� �����ϰ� ���� ������ ť�������� �����.
	HERMITEPOINT* pCur(NULL);
	while( !m_deqHermitePoint.empty() )
	{
		pCur = m_deqHermitePoint.front();

		if( pCur->fLife > 0.f )	goto _RETURN;

		m_pHermitePointPool->Release( pCur );
		m_deqHermitePoint.pop_front();
	}

_RETURN:

	float fLengthElapsed = m_fLengthDelta * fElapsedTime;
	D3DXCOLOR cColorElapsed = m_cColorDelta * fElapsedTime;

	// Note : �۾� ����
	dwSize = (DWORD)m_deqHermitePoint.size();
	for( DWORD i=0; i<dwSize; ++i )
	{
		m_deqHermitePoint[i]->fLength += fLengthElapsed;
		m_deqHermitePoint[i]->cColor += cColorElapsed;
	}
}

void DxEffectBlurSys::ClearHermitePoint()
{
	if( !m_pHermitePointPool )	return;

	for( DWORD i=0; i<m_deqHermitePoint.size(); ++i )
	{
		m_pHermitePointPool->Release( m_deqHermitePoint[i] );
	}
	m_deqHermitePoint.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//															B	l	u	r
//------------------------------------------------------------------------------------------------------------------------------------------------
void DxEffectBlurSys::CreateBlur()
{
	// Note : ������ ����� �ִٸ� �۾��� ���Ѵ�.
	if( m_deqHermitePoint.empty() )	return;

	// Note : ����.
	D3DXVECTOR3 vLookatCross(0.f,0.f,0.f);
	DWORD dwColor(0L);
	DWORD dwSize = (DWORD)m_deqHermitePoint.size();

	BLUR_UPDOWN* pNew(NULL);
	for( DWORD i=0; i<dwSize; ++i )
	{
		vLookatCross = m_deqHermitePoint[i]->vLookatCross * m_deqHermitePoint[i]->fLength;
		dwColor = D3DCOLOR_COLORVALUE( m_deqHermitePoint[i]->cColor.r, 
										m_deqHermitePoint[i]->cColor.g, 
										m_deqHermitePoint[i]->cColor.b, 
										m_deqHermitePoint[i]->cColor.a*m_fAlpha);

		pNew = m_pBlurUpDownPool->New();

		pNew->sVertex[0].vPos = m_deqHermitePoint[i]->vBasePos + vLookatCross;
		pNew->sVertex[1].vPos = m_deqHermitePoint[i]->vBasePos - vLookatCross;

		pNew->sVertex[0].dwColor = dwColor;
		pNew->sVertex[1].dwColor = dwColor;

		pNew->sVertex[0].vTex = D3DXVECTOR2( 0.f, 0.f );
		pNew->sVertex[1].vTex = D3DXVECTOR2( 1.f, 0.f );

		m_deqBlurPoint.push_back( pNew );
	}
}

void DxEffectBlurSys::FrameMoveBlur()
{
	D3DVIEWPORT9* pViewPort(NULL);
	D3DXMATRIX* pmatView(NULL);
	D3DXMATRIX* pmatProj(NULL);
	float fOneDivWidth(0.f);
	float fOneDivHeight(0.f);
	float fAddWidth(0.f);
	float fAddHeight(0.f);

	if( m_dwFlag & USEREFRACT )
	{
		pViewPort = &DxViewPort::GetInstance().GetViewPort();
		pmatView = &DxViewPort::GetInstance().GetMatView();
		pmatProj = &DxViewPort::GetInstance().GetMatProj();

		if( pViewPort->Width < 1 )	return;
		if( pViewPort->Height < 1 )	return;

		fOneDivWidth = 1.f / pViewPort->Width;
		fOneDivHeight = 1.f / pViewPort->Height;

		// ȸ��
		float fSin = sinf(m_fTime)*0.01f;
		float fCos = cosf(m_fTime)*0.01f;

		fAddWidth = fCos + fSin;
		fAddHeight = fCos - fSin;
	}

	// Note : 
	DWORD dwSize = (DWORD)m_deqBlurPoint.size();
	float fTemp(0.f);
	float fOneDivSize(0.f);

	if( m_bBaseBlurRender )
	{
		fOneDivSize = 1.f / (dwSize+1);	// Base �ܻ� ������. 1�� �����־���.
	}
	else if( dwSize )
	{
		fOneDivSize = 1.f / dwSize;
	}

	// Note : �⺻ �ܻ�.
	int nCountUV(0);
	if( m_bBaseBlurRender )
	{
		DWORD dwBaseColor = ((DWORD)(m_cColor.r*255)<<16) + ((DWORD)(m_cColor.g*255)<<8) + (DWORD)(m_cColor.b*255);
		DWORD dwAlpha = (DWORD)( 255.f * m_cColor.a * m_fAlpha );

		
		m_sBaseBlur[0].dwColor = 0L;				// �պκ��� �����ϰ� ����..
		m_sBaseBlur[1].dwColor = 0L;
		m_sBaseBlur[2].dwColor = dwAlpha << 24;
		m_sBaseBlur[3].dwColor = dwAlpha << 24;

		m_sBaseBlur[0].dwColor += dwBaseColor;
		m_sBaseBlur[1].dwColor += dwBaseColor;
		m_sBaseBlur[2].dwColor += dwBaseColor;
		m_sBaseBlur[3].dwColor += dwBaseColor;

		if( m_dwFlag & USEREFRACT )
		{
			D3DXVECTOR3 vPos(0.f,0.f,0.f);
			D3DXVec3Project( &vPos, &m_sBaseBlur[0].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[0].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_sBaseBlur[1].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[1].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_sBaseBlur[2].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[2].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_sBaseBlur[3].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[3].vTex = D3DXVECTOR2( vPos.x, vPos.y );
		}
		else
		{
			m_sBaseBlur[0].vTex = D3DXVECTOR2( 0.f, 0.f );
			m_sBaseBlur[1].vTex = D3DXVECTOR2( 1.f, 0.f );
			m_sBaseBlur[2].vTex = D3DXVECTOR2( 0.f, fOneDivSize );
			m_sBaseBlur[3].vTex = D3DXVECTOR2( 1.f, fOneDivSize );
		}

		++nCountUV;	// ���� ���� ���ؼ� �־�д�.
	}

	// Note : ��ǻ�� ���� ��ȭ ��Ų��.
	dwSize = (DWORD)m_deqBlurPoint.size();
	if( m_deqHermitePoint.size() != dwSize )	return;

	for( DWORD i=0; i<dwSize; ++i, ++nCountUV )
	{
		fTemp = m_deqHermitePoint[i]->fLife * m_fOneDiveFullLife *255.f;

		if( m_dwFlag & USEREFRACT )
		{
			D3DXVECTOR3 vPos(0.f,0.f,0.f);
			D3DXVec3Project( &vPos, &m_deqBlurPoint[i]->sVertex[0].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_deqBlurPoint[i]->sVertex[0].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_deqBlurPoint[i]->sVertex[1].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_deqBlurPoint[i]->sVertex[1].vTex = D3DXVECTOR2( vPos.x, vPos.y );
		}
		else
		{
			fTemp = (dwSize-nCountUV+1)*fOneDivSize;
			m_deqBlurPoint[i]->sVertex[0].vTex = D3DXVECTOR2( 0.f, fTemp );
			m_deqBlurPoint[i]->sVertex[1].vTex = D3DXVECTOR2( 1.f, fTemp );
		}
	}
}

void DxEffectBlurSys::ClearBlurData()
{
	if( !m_pBlurUpDownPool )	return;

	for( DWORD i=0; i<m_deqBlurPoint.size(); ++i )
	{
		m_pBlurUpDownPool->Release( m_deqBlurPoint[i] );
	}
	m_deqBlurPoint.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//															R	E	N	D	E	R
//------------------------------------------------------------------------------------------------------------------------------------------------
void DxEffectBlurSys::RenderBlur( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	pd3dDevice->SetIndices ( NULL );

	// Note : �⺻ �ܻ�.
	if( m_bBaseBlurRender )
	{
		DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
		DWORD dwVertexSizeFULL = sizeof(VERTEXCOLORTEX)*4;
		if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
		{
			dwFlag = D3DLOCK_DISCARD; 
			DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
			DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
		}

		VERTEXCOLORTEX *pVertices;
		hr = DxDynamicVB::m_sVB_PDT.pVB->Lock ( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
		if ( FAILED(hr) )	return;

		for( DWORD i=0; i<4; ++i )
		{
			pVertices[i] = m_sBaseBlur[i];
		}

		DxDynamicVB::m_sVB_PDT.pVB->Unlock ();

		pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
		pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PDT.pVB, 0, sizeof(VERTEXCOLORTEX) );

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, DxDynamicVB::m_sVB_PDT.nVertexCount, 2 );

		// Note : ���� ������. 
		DxDynamicVB::m_sVB_PDT.nVertexCount += 4;
		DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;
	}
	
	// Note : ���� �ܻ�.
	if( !m_deqBlurPoint.empty() )
	{
		DWORD dwBlurPointSize = (DWORD)m_deqBlurPoint.size();
		if( dwBlurPointSize <= 1 )	return;

		DWORD dwVerticesNUM = dwBlurPointSize * 2;

		DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
		DWORD dwVertexSizeFULL = sizeof(VERTEXCOLORTEX)*dwVerticesNUM;
		if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
		{
			dwFlag = D3DLOCK_DISCARD; 
			DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
			DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
		}

		VERTEXCOLORTEX *pVertices;
		hr = DxDynamicVB::m_sVB_PDT.pVB->Lock ( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
		if ( FAILED(hr) )	return;

		for( DWORD i=0; i<dwBlurPointSize; ++i )
		{
			pVertices[i*2+0] = m_deqBlurPoint[i]->sVertex[0];
			pVertices[i*2+1] = m_deqBlurPoint[i]->sVertex[1];
		}

		DxDynamicVB::m_sVB_PDT.pVB->Unlock ();

		pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
		pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PDT.pVB, 0, sizeof(VERTEXCOLORTEX) );

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, DxDynamicVB::m_sVB_PDT.nVertexCount, dwVerticesNUM-2 );

		// Note : ���� ������. 
		DxDynamicVB::m_sVB_PDT.nVertexCount += dwVerticesNUM;
		DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;
	}
}
