//	COMMENT : 게임 겍체 기본 메소드 정의 클레스.
//
//	최초 작성자 : 정동헌.
//	최초 작성일 : 02.03.07
//
//	변경[BY/00.00.00] : 
//
//
#if !defined(_DXOBJECT_H__INCLUDED_)
#define _DXOBJECT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class DxObject
{
public:
	virtual HRESULT OneTimeSceneInit()									{ return S_OK; }
	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)		{ return S_OK; }
	virtual HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)	{ return S_OK; }

public:
	virtual HRESULT InvalidateDeviceObjects()							{ return S_OK; }
	virtual HRESULT DeleteDeviceObjects()								{ return S_OK; }
	virtual HRESULT FinalCleanup()										{ return S_OK; }

public:
	virtual HRESULT FrameMove(float fTime, float fElapsedTime)			{ return S_OK; }
	virtual HRESULT Render(LPDIRECT3DDEVICEQ pd3dDevice)				{ return S_OK; }

public:
	virtual HRESULT Create(LPDIRECT3DDEVICEQ pd3dDevice)
	{
		if ( FAILED(OneTimeSceneInit()) )
			return E_FAIL;

		if ( FAILED(InitDeviceObjects(pd3dDevice)) )
			return E_FAIL;

		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )
			return E_FAIL;

		return S_OK;
	}

	virtual HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();

		DeleteDeviceObjects();

		FinalCleanup();

		return S_OK;
	}
};


#endif // !defined(_APPGAME_H__INCLUDED_)
