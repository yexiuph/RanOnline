#include "pch.h"

#include "./DxEffectMan.h"

#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./DxEffCharAttribute.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffCharAttribute::VERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
DWORD		DxEffCharAttribute::TYPEID			= EMEFFCHAR_ATTRIBUTE;
DWORD		DxEffCharAttribute::VERSION			= 0x0100;
char		DxEffCharAttribute::NAME[MAX_PATH]	= "2.�Ӽ�-��,����,��?";

LPDIRECT3DSTATEBLOCK9	DxEffCharAttribute::m_pSavedStateBlock = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffCharAttribute::m_pEffectStateBlock = NULL;

DxEffCharAttribute::DxEffCharAttribute(void) :
	DxEffChar(),
	m_vTransUP(0.f,0.f,0.f),
	m_vTransDOWN(0.f,0.f,0.f),
	m_vPrevUP(0.f,0.f,0.f),
	m_vPrevDOWN(0.f,0.f,0.f),
	m_fTime(0.f),
	m_fThisTime(0.f),
	m_fFPSSumTime(0.f),
	m_fElapsedTime(0.0f),
	m_pLeftVB(NULL),
	m_pRightVB(NULL),
	m_pddsTexture(NULL)
{
	memset( m_szTraceUP, 0, sizeof(char)*STRACE_NSIZE );
	memset( m_szTraceDOWN, 0, sizeof(char)*STRACE_NSIZE );
}

DxEffCharAttribute::~DxEffCharAttribute(void)
{
	SAFE_RELEASE ( m_pLeftVB );
	SAFE_RELEASE ( m_pRightVB );

	CleanUp ();
}

DxEffChar* DxEffCharAttribute::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharAttribute *pEffChar = new DxEffCharAttribute;
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

HRESULT DxEffCharAttribute::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	D3DCAPSQ	d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() ����
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );		// ���� CCW ������CW
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );

		//	Note : SetTextureStageState() ����
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, 0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_BORDER );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharAttribute::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharAttribute::OneTimeSceneInit ()
{

	return S_OK;
}

HRESULT DxEffCharAttribute::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	D3DXVECTOR3	vNormal;

	//	Note : Eff Group�� ��Ʈ���� Ʈ������ ����.
	//
	if ( m_szTraceUP[0] == NULL )		return E_FAIL;
	if ( m_szTraceDOWN[0] == NULL )		return E_FAIL;
	
	if ( m_pCharPart )
	{
		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return E_FAIL;

		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
	}
	else if ( m_pSkinPiece )
	{
		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return E_FAIL;

		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
	}

	//	Note : �ؽ��� �ε�
	//
	TextureManager::LoadTexture( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0, TRUE );

	CreateBlurVB ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharAttribute::CreateBlurVB( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	SAFE_RELEASE ( m_pLeftVB );
	SAFE_RELEASE ( m_pRightVB );

	VERTEX*			pVertex;
	pd3dDevice->CreateVertexBuffer ( ((m_dwFaces*2)+2)*sizeof(VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,
										VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pLeftVB, NULL  );
	pd3dDevice->CreateVertexBuffer ( ((m_dwFaces*2)+2)*sizeof(VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,
										VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pRightVB, NULL  );

	hr = m_pLeftVB->Lock ( 0, 0, (VOID**)&pVertex, D3DLOCK_DISCARD );
	if ( FAILED(hr) )	return E_FAIL;

	for( DWORD i=0; i<m_dwFaces+1; i++ )
	{
		pVertex->p = m_vTransUP;
		pVertex->d = m_vColorUP;
		pVertex->t = D3DXVECTOR2( 0.0f, (float)i/(float)m_dwFaces );
		pVertex++;

		pVertex->p = m_vTransDOWN;
		pVertex->d = m_vColorDOWN;
		pVertex->t = D3DXVECTOR2( 1.0f, (float)i/(float)m_dwFaces );
		pVertex++;
	}

	m_pLeftVB->Unlock();

	hr = m_pRightVB->Lock ( 0, 0, (VOID**)&pVertex, D3DLOCK_DISCARD );
	if ( FAILED(hr) )	return E_FAIL;

	for( DWORD i=0; i<m_dwFaces+1; i++ )
	{
		pVertex->p = m_vTransUP;
		pVertex->d = m_vColorUP;
		pVertex->t = D3DXVECTOR2( 0.0f, (float)i/(float)m_dwFaces );
		pVertex++;

		pVertex->p = m_vTransDOWN;
		pVertex->d = m_vColorDOWN;
		pVertex->t = D3DXVECTOR2( 1.0f, (float)i/(float)m_dwFaces );
		pVertex++;
	}

	m_pRightVB->Unlock();

	return S_OK;
}

HRESULT DxEffCharAttribute::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffCharAttribute::InvalidateDeviceObjects ()
{
	m_pd3dDevice = NULL;

	return S_OK;
}

HRESULT DxEffCharAttribute::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	return S_OK;
}

HRESULT DxEffCharAttribute::FinalCleanup ()
{

	return S_OK;
}

HRESULT DxEffCharAttribute::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fTime = fTime;
	m_fElapsedTime = fElapsedTime;

	return S_OK;
}

void DxEffCharAttribute::Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	if ( !m_pLeftVB )	return;
	if ( !m_pRightVB )	return;

	HRESULT			hr = S_OK;
	D3DXVECTOR3		vNormal;

	//	Note : �ʱ�ȭ �ؾ� �� ��Ȳ.. 1.�� ó�� ���϶�, 2.������ ������� �ٽ� ���϶�..
	//
	ResetVB ();

	if ( m_pCharPart )
	{
		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return;

		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return;
	}
	else if ( m_pSkinPiece )
	{
		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
		if ( FAILED(hr) )	return;

		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
		if ( FAILED(hr) )	return;
	}

	//	Note : ��ġ ���� �Ǿ��� �� !!
	//
	if ( (m_vTransUP!=m_vPrevUP) ||	(m_vTransDOWN!=m_vPrevDOWN) )	UpdatePos ();

	//	Note : Render ( �ܻ� )
	//
	D3DXMATRIX matIdentity, matWorld;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	DWORD					dwFVFSize;
	DWORD					dwVerts, dwFaces;

	
	dwFVFSize = VERTEX::FVF;
	dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

	dwVerts = (m_dwFaces*2)+2;
	dwFaces = (m_dwFaces*2);

	pd3dDevice->SetFVF ( VERTEX::FVF );

	// Note : Get Texture
	if( !m_pddsTexture )	TextureManager::GetTexture( m_szTexture, m_pddsTexture );

	pd3dDevice->SetTexture ( 0, m_pddsTexture );


	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );
	DWORD	dwSrcBlend, dwDestBlend, dwColorOP, dwZWriteEnable, dwAlphaBlendEnable,
			dwAlpgaTestEnable, dwAlphaRef, dwAlphaFunc;

	pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
	pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHATESTENABLE,		&dwAlpgaTestEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAREF,			&dwAlphaRef );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAFUNC,			&dwAlphaFunc );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,	&dwColorOP );

	//	Note : ���� ��� ����
	//
	switch ( m_nBlend )
	{
	case 1 :
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		break;
	case 2:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

		break;
	case 3:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );

		break;			
	case 4:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		break;
	case 5:
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}
		break;
	}


	pd3dDevice->SetStreamSource ( 0, m_pLeftVB, 0, dwFVFSize );
	pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, dwFaces );

	pd3dDevice->SetStreamSource ( 0, m_pRightVB, 0, dwFVFSize );
	pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, dwFaces );
	

	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE,		dwAlpgaTestEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC,			dwAlphaFunc );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	dwColorOP );

	m_pSavedStateBlock->Apply();

	m_vPrevUP = m_vTransUP;
	m_vPrevDOWN = m_vTransDOWN;
}

HRESULT	DxEffCharAttribute::ResetDiffuse ()
{
	HRESULT hr;

	VERTEX*			pVertex;

	hr = m_pLeftVB->Lock ( 0, 0, (VOID**)&pVertex, D3DLOCK_DISCARD );
	if ( FAILED(hr) )	return E_FAIL;

	for( DWORD i=0; i<m_dwFaces+1; i++ )
	{
		pVertex->d = m_vColorUP;
		pVertex++;

		pVertex->d = m_vColorDOWN;
		pVertex++;
	}

	m_pLeftVB->Unlock();

	hr = m_pRightVB->Lock ( 0, 0, (VOID**)&pVertex, D3DLOCK_DISCARD );
	if ( FAILED(hr) )	return E_FAIL;

	for( DWORD i=0; i<m_dwFaces+1; i++ )
	{
		pVertex->d = m_vColorUP;
		pVertex++;

		pVertex->d = m_vColorDOWN;
		pVertex++;
	}

	m_pRightVB->Unlock();

	return S_OK;
}

HRESULT	DxEffCharAttribute::ResetVB ()
{
	HRESULT	hr;

	D3DXVECTOR3	vDeltaTrans;
	D3DXVECTOR3	vNormal;

	if ( m_fThisTime != 0.f )	m_fThisTime += m_fElapsedTime;

	if ( m_fTime - m_fThisTime > 0.3f || m_fThisTime == 0.f)
	{
		if ( m_pCharPart )
		{
			hr = m_pCharPart->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
			if ( FAILED(hr) )	return E_FAIL;

			hr = m_pCharPart->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
			if ( FAILED(hr) )	return E_FAIL;
		}
		else if ( m_pSkinPiece )
		{
			hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceUP, m_vTransUP, vNormal );
			if ( FAILED(hr) )	return E_FAIL;

			hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceDOWN, m_vTransDOWN, vNormal );
			if ( FAILED(hr) )	return E_FAIL;
		}

		//	���� ������ �� �ֱ�
		m_vPrevUP = m_vTransUP;
		m_vPrevDOWN = m_vTransDOWN;

		vDeltaTrans = (m_vTransDOWN - m_vTransUP)/(float)m_dwFaces;

		VERTEX*	pVertex;
		hr = m_pLeftVB->Lock ( 0, 0, (VOID**)&pVertex, D3DLOCK_DISCARD );
		if ( FAILED(hr) )	return E_FAIL;
		for( DWORD i=0; i<m_dwFaces+1; i++ )
		{
			pVertex->p = m_vTransUP + (vDeltaTrans*(float)i);
			pVertex++;

			pVertex->p = m_vTransUP + (vDeltaTrans*(float)i);										// ���� ���� �־ �þ�� ȿ���� �ش�.
			pVertex++;
		}
		m_pLeftVB->Unlock();

		hr = m_pRightVB->Lock ( 0, 0, (VOID**)&pVertex, D3DLOCK_DISCARD );
		if ( FAILED(hr) )	return E_FAIL;
		for( DWORD i=0; i<m_dwFaces+1; i++ )
		{
			pVertex->p = m_vTransUP + (vDeltaTrans*(float)i);
			pVertex++;

			pVertex->p = m_vTransUP + (vDeltaTrans*(float)i);										// ���� ���� �־ �þ�� ȿ���� �ش�.
			pVertex++;
		}
		m_pRightVB->Unlock();

		m_fThisTime = m_fTime;
	}

	return S_OK;
}

HRESULT	DxEffCharAttribute::UpdatePos ()
{
	HRESULT hr;

	D3DXVECTOR3		vDeltaTrans;
	D3DXVECTOR3		vDeltaPrevTrans;
	D3DXVECTOR3		vCarDirec, vDirection, vLookatCross, vFromPtToPos, vTransAVG;
	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3&	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();

	vTransAVG	= (m_vTransUP+m_vTransDOWN)*0.5f;
	vFromPtToPos = vFromPt - vTransAVG;
	D3DXVec3Normalize ( &vFromPtToPos, &vFromPtToPos );

	vDeltaTrans = (m_vTransDOWN - m_vTransUP)/(float)m_dwFaces;

	vCarDirec = vTransAVG - vFromPt;
	vDirection = vDeltaTrans;	
	D3DXVec3Normalize ( &vDirection, &vDirection );	
	D3DXVec3Cross ( &vLookatCross, &vDirection, &vCarDirec );
	D3DXVec3Normalize ( &vLookatCross, &vLookatCross );

	vDeltaPrevTrans = (m_vPrevUP - m_vTransUP)*m_fBlurRate;

	VERTEX*	pVertex;

	hr = m_pLeftVB->Lock ( 0, 0, (VOID**)&pVertex, D3DLOCK_DISCARD );
	if ( FAILED(hr) )	return E_FAIL;
	for( DWORD i=0; i<m_dwFaces+1; i++ )
	{
		pVertex->p = m_vTransUP + (vDeltaTrans*(float)i);
		if ( m_bFlarePos )	pVertex->p		+= vFromPtToPos*m_fFlarePos;		// ������ ��������..
		pVertex++;

		pVertex->p = m_vTransUP + (vDeltaTrans*(float)i);
		pVertex->p -= vLookatCross*m_fLength;									// �������� ��
		pVertex->p.x += (rand()%100)*m_fRandLength*0.01f - m_fRandLength*0.5f;	// ���� X
		pVertex->p.y += (rand()%100)*m_fRandLength*0.01f - m_fRandLength*0.5f;	// ���� Y
		pVertex->p.z += (rand()%100)*m_fRandLength*0.01f - m_fRandLength*0.5f;	// ���� Z
		pVertex->p += vDeltaPrevTrans;											// ���� ���� �־ �þ�� ȿ���� �ش�.
		if ( m_bGravity )	pVertex->p.y	+= m_fRandLength*m_fGravity;		// �߷��� ��
		pVertex++;
	}
	m_pLeftVB->Unlock();

	hr = m_pRightVB->Lock ( 0, 0, (VOID**)&pVertex, D3DLOCK_DISCARD );
	if ( FAILED(hr) )	return E_FAIL;
	for( DWORD i=0; i<m_dwFaces+1; i++ )
	{
		pVertex->p = m_vTransUP + (vDeltaTrans*(float)i);
		if ( m_bFlarePos )	pVertex->p		+= vFromPtToPos*m_fFlarePos;		// ������ ��������..
		pVertex++;

		pVertex->p = m_vTransUP + (vDeltaTrans*(float)i);
		pVertex->p += vLookatCross*m_fLength;
		pVertex->p.x += (rand()%100)*m_fRandLength*0.01f - m_fRandLength*0.5f;
		pVertex->p.y += (rand()%100)*m_fRandLength*0.01f + m_fRandLength*0.5f;
		pVertex->p.z += (rand()%100)*m_fRandLength*0.01f - m_fRandLength*0.5f;
		pVertex->p += vDeltaPrevTrans;
		if ( m_bGravity )	pVertex->p.y	+= m_fRandLength*m_fGravity;		// �߷��� ��
		pVertex++;
	}
	m_pRightVB->Unlock();

	return S_OK;
}

HRESULT	DxEffCharAttribute::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharAttribute::SaveFile ( basestream &SFile )
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