#ifndef DXLANDMAN_H_
#define DXLANDMAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//	[class DxLandMan]
//
//	Need : 알파멥 그리기시에 최적화 여지가 있음. (수정요), JDH
//
//	Modify : (2002.12.03), JDH, ExportProgress::SetStateString() 에서 대기 while()문에서
//		{}가 공백으로 있을때 무한루프에 빠지는 버그 발생. Sleep() 추가함.

#include "./CollisionMap.h"
#include "./DxLandDef.h"
#include "./DxLandEff.h"
#include "./DxLandGateMan.h"
#include "./DxLandSkinObj.h"
#include "./DxOctree.h"

#include "./SerialFile.h"
#include "./Collision.h"
#include "./DxMethods.h"
#include "./DxLightMan.h"
#include "./DxWeatherMan.h"
#include "./DxCamAniMan.h"
#include "./DxFogMan.h"
#include "./DxSkyMan.h"
#include "../[Lib]__EngineSound/Sources/DxSound/BgmSoundData.h"
#include "./DxEffectRender.h"
#include "./DxStaticMesh.h"
#include "./DxPieceManager.h"
#include "./GLDefine.h"
#include "./DxAnimationManager.h"

struct	DXPIECEOBJ;
struct	DXPIECEOBJNODE;
class	NavigationMesh;
class	DxSetLandMan;
class	CStaticSoundMan;


namespace	ExportProgress
{
	extern	int		EndPos;
	extern	int		CurPos;
	extern	BOOL	bValid;	
	extern	char	szState[MAX_PATH];

	void InitialCritical ();
	void DeleteCritical ();

	void GetStateString ( char *szString );
	void SetStateString ( char *szString );
};

class DxLandMan
{
public:
	const static DWORD	VERSION;
	const static char	FILEMARK[128];
	static char			m_szPath[MAX_PATH];

public:
	static void		SetPath ( char* szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	static char*	GetPath ()					{ return m_szPath; }

protected:
	SNATIVEID		m_MapID;
	char			m_szMapName[MAXLANDNAME];

	SLAND_FILEMARK	m_sFILEMARK;

public:
	SNATIVEID GetMapID ()	{ return m_MapID; }
	void	  SetMapID ( SNATIVEID mapID ) { m_MapID = mapID; }

protected:
	DxOctree		m_DxOctree;

protected:
	DxStaticMesh*	m_pStaticMesh;

protected:
	DxAnimationMan*	m_pAniManHead;
	OBJAABBNode*	m_pAniManTree;

protected:
	//	Note : 치환 조각 관리.
	DXPIECEOBJ*		m_pPieceObjHead;
	DXPIECEOBJNODE*	m_pPieceObjTree;


protected:
	DxEffectBase*	m_pEffectFrameList;	//	Frame 종속 효과 관리.
	DxAfterRender	m_AfterRender;		//	Frame 종속 효과중 나중에 뿌려지는것 관리.

	//	Note : Frame 접합 이펙트중 (_EFF_REPLACE) 속성이 있는 효과들.
	//
	DWORD			m_dwEffectCount;
	DxEffectBase*	m_pEffectList;
	OBJAABBNode*	m_pEffectTree;

	//	Note : Frame 접합 이펙트중 (_EFF_REPLACE) 속성이 있는 효과들.
	//
	DWORD			m_dwEffectCount_AFTER;
	DxEffectBase*	m_pEffectList_AFTER;
	OBJAABBNode*	m_pEffectTree_AFTER;

	//	Note : Frame 접합 이펙트중 (_EFF_REPLACE) 속성이 있는 효과들.
	//
	DWORD			m_dwEffectCount_AFTER_1;
	DxEffectBase*	m_pEffectList_AFTER_1;
	OBJAABBNode*	m_pEffectTree_AFTER_1;

	//	Note : Frame 접합 이펙트중 <Weather> 속성이 있는 효과들. <그림자/물/반사 등> 보다 나중에 뿌려야 한다.
	DWORD			m_dwCount_Weather;
	DxEffectBase*	m_pList_Weather;
	OBJAABBNode*	m_pTree_Weather;
	
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
	DxCamAniMan*	GetCamAniMan ()		{ return &m_CameraAniMan; }
	DxLandGateMan*	GetLandGateMan ()	{ return &m_LandGateMan; }
	DxAfterRender*	GetAfterRender ()	{ return &m_AfterRender; }

protected:
	//	Note : 네비게이션 메시 무결성
	BOOL			m_bIntegrity;
	NavigationMesh*	m_pNaviMesh;		// 길에 대한 정보를 가진 메쉬

protected:
	//	Note : 맵 사운드
	CStaticSoundMan*m_pStaticSoundMan;
	
protected:
	//	Note : 다이나믹 로딩용
	CSerialFile		m_SFileMap;

public:
	CSerialFile&	GetSerialFile()		{ return m_SFileMap; }

protected:
	//	Note : Single Effect
	//
	DWORD			m_dwNumLandEff;
	PLANDEFF		m_pLandEffList;
	OBJAABBNode*	m_pLandEffTree;

protected:
	//	Note : Skin Object
	//
	DWORD			m_dwNumLandSkinObj;
	PLANDSKINOBJ	m_pLandSkinObjList;
	OBJAABBNode*	m_pLandSkinObjTree;

public:
	void SetBgmFile ( CString strBGM )	{ m_BGMDATA.SetFile ( strBGM ); }
	const CString& GetBgmFile ()		{ return m_BGMDATA.strFileName; }

public:
	DWORD GetNumLandEff ()				{ return m_dwNumLandEff; }
	PLANDEFF GetLandEffList ()			{ return m_pLandEffList; }

	void AddLandEff ( PLANDEFF pLandEff );
	void DelLandEff ( PLANDEFF pLandEff );
	PLANDEFF FindLandEff ( char* szName );

	void BuildSingleEffTree ();

public:
	DWORD GetNumSkinObj ()				{ return m_dwNumLandSkinObj; }
	PLANDSKINOBJ GetLandSkinObjList ()	{ return m_pLandSkinObjList; }

	void AddLandSkinObj ( PLANDSKINOBJ pLandSkinObj );
	void DelLandSkinObj ( PLANDSKINOBJ pLandSkinObj );
	PLANDSKINOBJ FindLandSkinObj ( char* szName );
	PLANDSKINOBJ FindLandSkinObjByFileName ( const char* szFileName );

	void BuildSkinObjTree ();

public:
	DxOctree* GetOctree () { return &m_DxOctree; }
	NavigationMesh* GetNaviMesh () { return m_pNaviMesh; }

protected:
	BOOL AddPieceObj ( DXPIECEOBJ *pNewPieceObj );
	void AddEffectFrameList ( DxEffectBase* pEffect );

protected:
	HRESULT EffectAdaptToFrame ( DxOctree &OcNode, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
					PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc );

	HRESULT EffectAdaptToList ( DxFrame *pFrameRoot, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
					PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc );

	void SwitchWeatherEffect( PDXEFFECTBASE& pSrc, DWORD& dwSrc, PDXEFFECTBASE& pDest, DWORD& dwDest );

public:
	static HRESULT EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, PDXEFFECTBASE &pEffList );
	static HRESULT EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, PDXEFFECTBASE &pEffList, PDXEFFECTBASE &pEffList_AFTER, 
									DWORD& dwCount, DWORD& dwCount_AFTER );

public:
	HRESULT Import ( int MaxTriangles, int MaxSubdivisions, DxSetLandMan *pSetLandMan, LPDIRECT3DDEVICEQ pd3dDevice, char *szDebugFullDir );

public:
	HRESULT ActiveMap ();

public:
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);

	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();	

protected:
	HRESULT DrawAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, BOOL bReflect=FALSE );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	HRESULT Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	void	RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT	Render_ReflectOLD( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &pCV );
	HRESULT	Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );

	HRESULT Render_NOSKIN ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );

public:
	void CleanUp ();

public:
	BOOL IsNaviMeshIntegrity()	{ return m_bIntegrity; }

public:
	BOOL SaveFile ( const char *szFile );
	BOOL SavePieceFile ( const char *szFile );

public:
	BOOL LoadFile ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad = FALSE );
	BOOL LoadBasicPos( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 &vPos );
	BOOL LoadBasicPos( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin );

protected:
	BOOL LoadFile_VER108 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad );
	BOOL LoadFile_VER109 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );
	BOOL LoadFile_VER110 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );
	BOOL LoadFile_VER112 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName );	// Ver 111~112

public:
	DxLandMan ();
	~DxLandMan();		

	//	Note : 충돌 처리 함수들.
public:
	void IsCollision ( const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						LPDXFRAME &pDxFrame, const BOOL bFrontColl );

	//  예전 Octree는 이 함수를 사용하지 못 함.  Piece 충돌함.
	void IsCollisionNEW( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						const BOOL bBackColl, const BOOL bPiece );

	void IsCollisionEX ( D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
							LPDXFRAME &pDxFrame, LPCSTR& szName, D3DXVECTOR3 *vNormal = NULL );
};

#endif // DXLANDMAN_H_