#pragma once

#include "./DxEffectFrame.h"

class DxEffectBase;
class DxEffectBase;
class DxLandMan;

struct DXEFFECTREND
{
	DxEffectBase*		m_pEffect;
	DxFrame*			m_pFrame;
	DxLandMan*			m_pLandMan;

	DXEFFECTREND () :
		m_pEffect(NULL),
		m_pFrame(NULL),
		m_pLandMan(NULL)
	{
	}

	DXEFFECTREND ( DxEffectBase* pEffect, DxFrame* pFrame, DxLandMan* pLandMan ) :
		m_pEffect(pEffect),
		m_pFrame(pFrame),
		m_pLandMan(pLandMan)
	{
	}


	HRESULT RENDER ( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		return m_pEffect->Render ( m_pFrame, pd3dDevice, NULL, m_pLandMan );
	}
};

class DxAfterRender
{
public:
	typedef std::multimap<DWORD,DXEFFECTREND>				EFFECT_MMAP;
	typedef std::multimap<DWORD,DXEFFECTREND>::iterator		EFFECT_MMAP_ITER;

private:
	EFFECT_MMAP		m_EffectMMap;

public:
	void Clear ()
	{
		m_EffectMMap.clear();
	}

	void AfterRender ( DWORD dwOrder, DXEFFECTREND &sEffRend )
	{
		m_EffectMMap.insert ( std::make_pair(dwOrder,sEffRend) );
	}

	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		HRESULT hr=S_OK;

		EFFECT_MMAP_ITER iter = m_EffectMMap.begin ();
		EFFECT_MMAP_ITER iter_end = m_EffectMMap.end ();

		for ( ; iter!=iter_end; ++iter )
		{
			DXEFFECTREND &sEffRend = (*iter).second;

			hr = sEffRend.RENDER ( pd3dDevice );
			if ( FAILED(hr) )	return hr;
		}

		return S_OK;
	}

public:
	DxAfterRender ()
	{
	}
	~DxAfterRender ()
	{
	}
};

