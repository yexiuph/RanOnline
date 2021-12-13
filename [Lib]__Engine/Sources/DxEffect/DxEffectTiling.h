// Terrain0.h: interface for the DxEffectWater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DXEFFECTTILING__INCLUDED__)
#define __DXEFFECTTILING__INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxEffectFrame.h"
#include "Collision.h"

#include <list>
#include <map>
#include <algorithm>

#include "NsOCTree.h"

#define USE_TEX_DRAW		0x0001
#define USE_COLOR_DRAW		0x0002

struct DxMeshes;
struct DXMATERIAL;

struct TILING_PROPERTY					// Ver.102
{
	DWORD			m_dwFlag;

	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;

	DWORD			m_dwVertices;
	DWORD			m_dwFaces;

	DWORD			m_dwUNIQUE_NUMBER;
	DWORD			m_dwMaterials_B;

	D3DXMATRIX		m_matFrameComb;

	TILING_PROPERTY () :
		m_vMax(0.f,0.f,0.f),
		m_vMin(0.f,0.f,0.f),
		m_dwVertices(0),
		m_dwFaces(0),
		m_dwMaterials_B(0),
		m_dwUNIQUE_NUMBER(1)
	{
		m_dwFlag = USE_TEX_DRAW;
	};
};

class DxEffectTiling : public DxEffectBase
{
	//	Note : ����Ʈ Ÿ�� ����.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

public:
	void SetProperty ( TILING_PROPERTY& Property )
	{
		m_Property = Property;
	}
	TILING_PROPERTY& GetProperty () { return m_Property; }

	//	Note : �Ӽ�.
	//
protected:
	union
	{
		struct
		{
			TILING_PROPERTY		m_Property;
		};

		struct
		{
			DWORD			m_dwFlag;

			D3DXVECTOR3		m_vMax;
			D3DXVECTOR3		m_vMin;

			DWORD			m_dwVertices;
			DWORD			m_dwFaces;

			DWORD			m_dwUNIQUE_NUMBER;
			DWORD			m_dwMaterials_B;

			D3DXMATRIX		m_matFrameComb;
		};
	};

	//	Note : ������ ������
	//
protected:
	static	LPDIRECT3DSTATEBLOCK9		m_pSavedSB;
	static	LPDIRECT3DSTATEBLOCK9		m_pDrawSB;

	static	LPDIRECT3DSTATEBLOCK9		m_pSavedSB_Select;
	static	LPDIRECT3DSTATEBLOCK9		m_pDrawSB_Select;

protected:
	struct VERTEXCOLOR
	{
		D3DXVECTOR3			vPos;
		D3DXVECTOR3			vNor;
		D3DCOLOR			dwColor;
		D3DXVECTOR2			vTex;
		const static DWORD	FVF;
	};


	struct POINT
	{
		D3DXVECTOR3		vPos;			// ��ġ
		D3DXVECTOR3		vNor;			// Normal
		D3DXVECTOR2		vTex;			// Tex ��ǥ
		DWORD			dwFace_NUM;		// �ڽ��� ���� �ִ� Face Number
		DWORD			dwPointEx;		// �༮�� ��...�θ�� ����Ʈ
	};

	struct POINTA
	{
		DWORD	dwIndex;
		POINTA*	pNext;

		POINTA() :
			pNext(NULL)
		{
		}

		~POINTA()
		{
			SAFE_DELETE ( pNext );
		}
	};

	struct POINTEX
	{
		POINTA*		pPoint;		// ���� Point
		D3DXVECTOR3	vPos;		// ��ġ
		DWORD		dwMaterial;
		DWORD		dwColor;

		POINTEX() :
			pPoint(NULL),
			vPos(0.f,0.f,0.f),
			dwMaterial(0),
			dwColor(0x00ffffff)
		{
		}

		~POINTEX()
		{
			SAFE_DELETE ( pPoint );
		}
	};

protected:
	struct SEPARATEOBJ
	{
		VERTEXCOLOR	sVertex[3];
	};

	struct TRIANGLE
	{
		BOOL		bBase;			// �⺻ �ﰢ���� �� ���ΰ� ?
		DWORD		dwMaterial;		// ���� ������ �ؽ��ĸ� ������ �˱⸸ �ϸ� �ȴ�.

		TRIANGLE() :
			bBase(TRUE)
		{
		}
	};

	typedef std::list<DWORD>			TRIANGLELIST;
	typedef TRIANGLELIST::iterator		TRIANGLELIST_ITER;

	typedef std::list<SEPARATEOBJ>		FACELIST;
	typedef FACELIST::iterator			FACELIST_ITER;

	struct MATERIALEX
	{
		LPDIRECT3DTEXTUREQ	pTexture;				//
		std::string			strTexture;				//	SAVE

		TRIANGLELIST		listTriangle;			//
		FACELIST			listFace;

		BOOL				m_bAlpha;
		DxSingleTexMesh*	m_pMeshList;			// Save

		MATERIALEX () :
			pTexture(NULL),
			m_bAlpha(FALSE),
			m_pMeshList(NULL)
		{
		}

		~MATERIALEX();
	};

protected:
	void CreateAABBOctree ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALEX* pMaterialEX );

	struct VERTEXDATA
	{
		BOOL		bEqual;
		DWORD		dwNUM;
		DWORD		dwNUM_EX;
	};

protected:
	POINT*			m_pPoint;

	DWORD			m_dwPointEX;
	POINTEX*		m_pPointEX;

	DWORD			m_dwPointEX_Load;
	POINTEX*		m_pPointEX_Load;

	TRIANGLE*		m_pTriangle;

	DWORD			m_dwMaterials;
	DXMATERIAL*		m_pMaterials;

protected:
	DWORD			m_dwCurPOINT;			// ���� ������ �� ��ȣ
	DWORD			m_dwCurPOINT_PREV;		// ������ ���õ� �� ��ȣ
	TRIANGLELIST	m_listSelectTriangle;	// ���õ� �ﰢ���� ǥ�� ������ ���ֱ� ���ؼ�.

protected:
	DWORD			m_dwCurTILE;			// ���� ������ Ÿ�� ��ȣ (�ؽ��� ����)
	D3DXCOLOR		m_cCurCOLOR;			// ���� ������ �� ��
	D3DXCOLOR		m_cCurRANDOM;			// ���� ������ �� ���� �������� �ֱ� ���ؼ�.
	BOOL			m_bSelectDRAW;			// ������ �� ���ΰ� ?

public:
	typedef std::map<DWORD,MATERIALEX*>				MATERIALMAP;
	typedef MATERIALMAP::iterator					MATERIALMAP_ITER;

	MATERIALMAP		m_mapMaterials;		// ������ ���̴� �ﰢ��

protected:
	struct FINDEX
	{
		DWORD		dwEX;
		D3DXVECTOR3 vPos;
	};
	typedef std::multimap<float,FINDEX>			FINDEXMAP;
	typedef FINDEXMAP::iterator					FINDEXMAP_ITER;

	FINDEXMAP			m_mapFindEX;

protected:
	virtual HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );
public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

	HRESULT Render_Select ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bCenter );

protected:
	//		��	��	��	��
	void	SetBaseQuad( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs );

	void	SetPointData ( VERTEX* pVertices, WORD* pIndices );
	void	SetTriangleData ( DxMeshes *pmsMeshs );

	BOOL	DxFindPoint ( D3DXVECTOR3 vPos, DWORD _dwIndex );

	BOOL	CheckTriangleUSE ( MATERIALMAP_ITER& iter, TRIANGLELIST_ITER &iterTriangle, DWORD dwFaceNUM );

	void	CreateTiles ( LPDIRECT3DDEVICEQ pd3dDevice );
	void	CreateTile ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALMAP_ITER& iterMaterial );

	void	OneSrcTriangle ( VERTEXDATA* pData, FACELIST& pRoot );
	void	TwoSrcTriangle ( VERTEXDATA* pData, FACELIST& pRoot );
	void	ThrSrcTriangle ( VERTEXDATA* pData, FACELIST& pRoot );
	void	SetSeparateList ( FACELIST& pRoot, const VERTEXCOLOR& sVertex_0, const VERTEXCOLOR& sVertex_1, const VERTEXCOLOR& sVertex_2 );

	void	BaseTileSetting ();

	void	BlendTileSetting ();

	void	RenderTiles ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALMAP& mapMaterials );
	void	RenderTiles ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALEX* pMaterialEX );

public:
	BOOL	IsTriangle ( D3DXVECTOR3 vPickRayOrig, D3DXVECTOR3 vPickRayDir );
	void	SetListBox ( LPDIRECT3DDEVICEQ pd3dDevice );

	void	TileEdit ( D3DXVECTOR3 vPickRayOrig, D3DXVECTOR3 vPickRayDir );
	void	DxLeftMouseClick ();
	void	DeleteClick ();

public:
	char*	GetStrList( DWORD i );
	BOOL	SetStrList( LPDIRECT3DDEVICEQ pd3dDevice, const char* cName );

	void		SetCurMaterial ( DWORD i )				{ m_dwCurTILE=i; }			// ���õ� Material 
	DWORD		GetCurMaterial ()						{ return m_dwCurTILE; }		//
	void		SetCurColor ( D3DXCOLOR cColor )		{ m_cCurCOLOR=cColor; }		// ������ ĥ�ϱ�. 
	D3DXCOLOR	GetCurColor ()							{ return m_cCurCOLOR; }		//
	void		SetColorRandom ( D3DXCOLOR cRandom )	{ m_cCurRANDOM=cRandom; }	// �� ������
	D3DXCOLOR	GetColorRandom ()						{ return m_cCurRANDOM; }	//
	void		SetSelectDraw ( BOOL bUse )				{ m_bSelectDRAW=bUse; }		// ���õ� Material �����ϱ�.
	BOOL		GetSelectDraw ()						{ return m_bSelectDRAW; }	//

	void DeleteColorALL();

	
public:
	DxEffectTiling ();
	virtual ~DxEffectTiling ();

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh �� ȿ���� DxLandMan �� ���� �� �� �뷮�� ���� ����Ÿ��
	//		�����ϱ� ���ؼ� ����Ѵ�.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(__DXEFFECTTILING__INCLUDED__)
