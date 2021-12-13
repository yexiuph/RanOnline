#include "pch.h"
#include "DamageDisplayMan.h"
#include "DamageDisplay.h"
#include "../[Lib]__Engine/Sources/DxTools/TextureManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDamageDisplayMan::CDamageDisplayMan () :
	m_nStartIndex ( 0 ),
	m_pTextureDUMMY ( NULL )
{
}

CDamageDisplayMan::~CDamageDisplayMan ()
{
}

HRESULT CDamageDisplayMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_strNumberTEX = "number.dds";
	TextureManager::LoadTexture( m_strNumberTEX.c_str(), pd3dDevice, m_pTextureDUMMY, 0, 0 );

	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

HRESULT CDamageDisplayMan::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture ( m_strNumberTEX.c_str(), m_pTextureDUMMY );

	return CUIGroup::DeleteDeviceObjects ();
}

void CDamageDisplayMan::CreateSubControl ()
{
	for ( int i = 0; i < nMAX_DAMAGE_DISPLAY; i++ )
	{
		CDamageDisplay* pDamageDisplay = new CDamageDisplay;
		pDamageDisplay->CreateSub ( this, "DAMAGE_DISPLAY" );
		pDamageDisplay->CreateSubControl ();
		pDamageDisplay->SetVisibleSingle ( FALSE );
		RegisterControl ( pDamageDisplay );
		m_pDamageDisplay[i] = pDamageDisplay;
	}
}

void CDamageDisplayMan::SetDamage ( int nPosX, int nPosY, int nDamage, DWORD dwDamageFlag, BOOL bAttack )
{
	CDamageDisplay* pDamageDisplay = m_pDamageDisplay[m_nStartIndex];
	pDamageDisplay->SetVisibleSingle ( TRUE );
	pDamageDisplay->SetDamage ( nDamage, dwDamageFlag, bAttack );
	pDamageDisplay->SetGlobalPos ( D3DXVECTOR2 ( float(nPosX), float(nPosY) ) );

	m_nStartIndex++;
	if ( nMAX_DAMAGE_DISPLAY <= m_nStartIndex )
	{
		m_nStartIndex = 0;
	}
}