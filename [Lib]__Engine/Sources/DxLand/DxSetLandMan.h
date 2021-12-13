#ifndef DXSETLANDMAN_H_
#define DXSETLANDMAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxLandDef.h"
#include "./DxLandEff.h"
#include "./DxLandGateMan.h"
#include "./DxLandSkinObj.h"
#include "./CollisionMap.h"

#include "./DxCamAniMan.h"
#include "./DxFogMan.h"
#include "./DxLightMan.h"
#include "./DxSkyMan.h"
#include "./DxWeatherMan.h"
#include "./DxPieceManager.h"
#include "../[Lib]__EngineSound/Sources/DxSound/BgmSoundData.h"
#include "../[Lib]__EngineSound/Sources/DxSound/StaticSoundMan.h"
#include "./DxFrameMesh.h"
#include "../NaviMesh/NavigationMesh.h"

class DxSetLandMan
{
public:
	const static DWORD	VERSION;

protected:
	SNATIVEID		m_MapID;
	char			m_szMapName[MAXLANDNAME];

	DxFrameMesh		m_FrameMesh;

	LPDXFRAME		m_pNaviFrame;
	NavigationMesh	m_NaviMesh;

	CStaticSoundMan m_StaticSoundMan;

	DWORD			m_dwNumLandEff;
	PLANDEFF		m_pLandEffList;
	OBJAABBNode*	m_pLandEffTree;

	DWORD			m_dwNumLandSkinObj;
	PLANDSKINOBJ	m_pLandSkinObjList;
	OBJAABBNode*	m_pLandSkinObjTree;

	DxLightMan		m_LightMan;
	DxWeatherMan	m_WeatherMan;
	DxCamAniMan		m_CameraAniMan;
	DxLandGateMan	m_LandGateMan;

	CCollisionMap	m_CollisionMap;

	FOG_PROPERTY	m_FOG_PROPERTY;
	SKY_PROPERTY	m_SKY_PROPERTY;
	SBGMDATA		m_BGMDATA;

	DxPieceManager	m_PieceManager;

public:
	void SetFogProperty ( FOG_PROPERTY& Property );
	FOG_PROPERTY& GetFogProperty ()		{ return m_FOG_PROPERTY; }

	void SetSkyProperty ( SKY_PROPERTY& Property );
	SKY_PROPERTY& GetSkyProperty ()		{ return m_SKY_PROPERTY; }

public:
	void SetNaviFrame ( DxFrame *pNaviMesh );
	void GetNaviFrame ( DxFrame **pNaviMesh ) { *pNaviMesh = m_pNaviFrame; }

	BOOL CreateNaviMesh ( LPDIRECT3DDEVICE9 pd3dDevice );

public:
	void Clone_MouseShift( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX& pMatrix );
	void SetObjRotate90();

public:
	SNATIVEID GetMapID ()				{ return m_MapID; }
	char* GetMapName ()					{ return m_szMapName; }
	DxFrameMesh* GetFrameMesh ()		{ return &m_FrameMesh; }
	CStaticSoundMan* GetStaticSoundMan(){ return &m_StaticSoundMan; }
	DxLightMan* GetLightMan ()			{ return &m_LightMan; }
	DxWeatherMan* GetWeatherMan ()		{ return &m_WeatherMan; }
	DxCamAniMan* GetCameraAniMan ()		{ return &m_CameraAniMan; }
	DxLandGateMan* GetLandGateMan ()	{ return &m_LandGateMan; }
	NavigationMesh* GetNaviMesh ()		{ return &m_NaviMesh; }
	CCollisionMap* GetCollisionMap ()	{ return &m_CollisionMap; }
	SBGMDATA* GetBGMDATA ()				{ return &m_BGMDATA; }
	DxPieceManager*	GetPieceManager()	{ return &m_PieceManager; }

public:
	void SetMapID ( SNATIVEID &MapID )	{ m_MapID = MapID; }

public:
	DWORD GetNumLandEff ()				{ return m_dwNumLandEff; }
	PLANDEFF GetLandEffList ()			{ return m_pLandEffList; }

	void AddLandEff ( PLANDEFF pLandEff );
	void DelLandEff ( PLANDEFF pLandEff );
	PLANDEFF FindLandEff( char* szName );
	PLANDEFF FindLandEff( D3DXVECTOR3& vCenter, D3DXVECTOR3& vTarget );
	void ReverseListLandEff();

	void BuildSingleEffTree ();

public:
	DWORD GetNumSkinObj ()				{ return m_dwNumLandSkinObj; }
	PLANDSKINOBJ GetLandSkinObjList ()	{ return m_pLandSkinObjList; }

	void AddLandSkinObj ( PLANDSKINOBJ pLandSkinObj );
	void DelLandSkinObj ( PLANDSKINOBJ pLandSkinObj );
	PLANDSKINOBJ FindLandSkinObj ( char* szName );

	void BuildSkinObjTree ();

public:
	HRESULT ActiveMap ();

public:
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

	virtual HRESULT CleanUp( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		InvalidateDeviceObjects( pd3dDevice );

		DeleteDeviceObjects();

		FinalCleanup();

		return S_OK;
	}

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void LoadSet ( LPDIRECT3DDEVICEQ pd3dDevice, const char	*pFileName );
	void SaveSet ( LPDIRECT3DDEVICEQ pd3dDevice, const char	*pFileName );

public:
	DxSetLandMan(void);
	~DxSetLandMan(void);
};

#endif // DXSETLANDMAN_H_