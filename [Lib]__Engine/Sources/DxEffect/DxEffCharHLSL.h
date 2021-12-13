#pragma once

namespace NSCHARHLSL
{
	extern ID3DXEffect*		m_pNormalFX;
	extern ID3DXEffect*		m_pSpecularFX;
	extern ID3DXEffect*		m_pCubeFX;
	extern ID3DXEffect*		m_pReflectFX;
	extern ID3DXEffect*		m_pLevel1FX;
	extern ID3DXEffect*		m_pToonFX;
	extern ID3DXEffect*		m_pEdgeFX;

	extern ID3DXEffect*		m_pNormalBaseFX;
	extern ID3DXEffect*		m_pNormalSpecularFX;

	HRESULT CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT ResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LostDevice();
	void	DestroyDevice();

	void FrameMove();
};