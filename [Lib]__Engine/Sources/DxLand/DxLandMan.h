#ifndef DXLANDMAN_H_
#define DXLANDMAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//	[class DxLandMan]
//
//	Need : ���ĸ� �׸���ÿ� ����ȭ ������ ����. (������), JDH
//
//	Modify : (2002.12.03), JDH, ExportProgress::SetStateString() ���� ��� while()������
//		{}�� �������� ������ ���ѷ����� ������ ���� �߻�. Sleep() �߰���.

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
	//	Note : ġȯ ���� ����.
	DXPIECEOBJ*		m_pPieceObjHead;
	DXPIECEOBJNODE*	m_pPieceObjTree;


protected:
	DxEffectBase*	m_pEffectFrameList;	//	Frame ���� ȿ�� ����.
	DxAfterRender	m_AfterRender;		//	Frame ���� ȿ���� ���߿� �ѷ����°� ����.

	//	Note : Frame ���� ����Ʈ�� (_EFF_REPLACE) �Ӽ��� �ִ� ȿ����.
	//
	DWORD			m_dwEffectCount;
	DxEffectBase*	m_pEffectList;
	OBJAABBNode*	m_pEffectTree;

	//	Note : Frame ���� ����Ʈ�� (_EFF_REPLACE) �Ӽ��� �ִ� ȿ����.
	//
	DWORD			m_dwEffectCount_AFTER;
	DxEffectBase*	m_pEffectList_AFTER;
	OBJAABBNode*	m_pEffectTree_AFTER;

	//	Note : Frame ���� ����Ʈ�� (_EFF_REPLACE) �Ӽ��� �ִ� ȿ����.
	//
	DWORD			m_dwEffectCount_AFTER_1;
	DxEffectBase*	m_pEffectList_AFTER_1;
	OBJAABBNode*	m_pEffectTree_AFTER_1;

	//	Note : Frame ���� ����Ʈ�� <Weather> �Ӽ��� �ִ� ȿ����. <�׸���/��/�ݻ� ��> ���� ���߿� �ѷ��� �Ѵ�.
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
	//	Note : �׺���̼� �޽� ���Ἲ
	BOOL			m_bIntegrity;
	NavigationMesh*	m_pNaviMesh;		// �濡 ���� ������ ���� �޽�

protected:
	//	Note : �� ����
	CStaticSoundMan*m_pStaticSoundMan;
	
protected:
	//	Note : ���̳��� �ε���
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

	//	Note : �浹 ó�� �Լ���.
public:
	void IsCollision ( const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						LPDXFRAME &pDxFrame, const BOOL bFrontColl );

	//  ���� Octree�� �� �Լ��� ������� �� ��.  Piece �浹��.
	void IsCollisionNEW( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						const BOOL bBackColl, const BOOL bPiece );

	void IsCollisionEX ( D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
							LPDXFRAME &pDxFrame, LPCSTR& szName, D3DXVECTOR3 *vNormal = NULL );
};

#endif // DXLANDMAN_H_