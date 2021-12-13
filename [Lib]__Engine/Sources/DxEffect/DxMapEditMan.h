#pragma once

#include <algorithm>
#include <map>
#include <list>

#include "DxQUADTree.h"

#include "DxMapBlend.h"

#include "DxStaticPisMan.h"

class	CSerialFile;

#define	USE_LENGTH		0x0001

struct MAPEDIT_PROPERTY
{
	DWORD	m_dwFlag;
	DWORD	m_dwGrid;

	MAPEDIT_PROPERTY() :
		m_dwGrid(8)
	{
		m_dwFlag = 0L;
	};
};

class DxMapEditMan
{
public:
	const static DWORD	VERSION;
	const static DWORD	VERSION_DATA;
	const static DWORD	VERSION_TEX;

protected:
	static char			m_szPathData[MAX_PATH];
	static char			m_szPathTex[MAX_PATH];

	static char			m_szFullNameData[MAX_PATH];		// Data Full Name
	static char			m_szFullNameTex[MAX_PATH];		// Tex Full Name

	static char			m_szExtNameMain[MAX_PATH];
	static char			m_szExtNameEdit[MAX_PATH];
	static char			m_szExtNameGame[MAX_PATH];

public:
	static void			SetPathData ( char* szPath )				{ StringCchCopy( m_szPathData, MAX_PATH, szPath ); }
	static char*		GetPathData ()								{ return m_szPathData; }

	static void			SetPathTex ( char* szPath )					{ StringCchCopy( m_szPathTex, MAX_PATH, szPath ); }
	static char*		GetPathTex ()								{ return m_szPathTex; }

	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB;

protected:
	union
	{
		struct
		{
			MAPEDIT_PROPERTY m_sProperty;
		};
		struct
		{
			DWORD	m_dwFlag;
			DWORD	m_dwGrid;		// < Save, Load >
		};
	};

protected:
	char			m_szMapName[128];

public:
	void				SetProperty ( MAPEDIT_PROPERTY& sProperty ) { m_sProperty = sProperty; }
	MAPEDIT_PROPERTY&	GetProperty ()								{ return m_sProperty; }

protected:
	struct VERTEX
	{
		D3DXVECTOR3			vPos;
		D3DXVECTOR3			vNor;
		D3DXVECTOR2			vTex;
		const static DWORD	FVF;
	};

	static const float		fCURVEVALUE;

	char		m_cBaseName[100];
	std::string	m_strBaseName;
	std::string	m_strExtName;

public:
	enum VERTEX_STATE
	{
		EMVS_LOCK	= 0,
		EMVS_NONE	= 1,
	};

	struct EDITDATA
	{
		VERTEX			sVertex;
		VERTEX_STATE	sState;

		EDITDATA()
		{
			sState = EMVS_NONE;
		}
	};

	struct	TILING_EDIT
	{
		std::string cName;
		float		fScale;

		TILING_EDIT() :
			fScale(1.0f)
		{
			cName = "_Eff_detail.bmp"; 
		}
	};

	typedef std::map<std::string,TILING_EDIT>		TILING_EDIT_MAP;
	typedef TILING_EDIT_MAP::iterator				TILING_EDIT_MAP_ITER;

	TILING_EDIT_MAP		m_mapTiles;
	D3DXCOLOR			m_cColor_EDIT[14];

protected:
	struct GRID_XZ
	{
		int x;
		int z;
	};

	struct GRID
	{
		int		nBaseX;		// 타일 번호
		int		nBaseZ;		// 타일 번호
		int		nBigX;		// 큰쪽에서 봤을 경우의 타일 X
		int		nBigZ;		// 큰쪽에서 봤을 경우의 타일 Z
		int		nShortX;	// 작은쪽에서 봤을 경우의 타일 X
		int		nShortZ;	// 작은쪽에서 봤을 경우의 타일 Z

		GRID() :
			nBaseX(0),
			nBaseZ(0),
			nBigX(0),
			nBigZ(0),
			nShortX(0),
			nShortZ(0)
		{
		};
	};

	typedef std::list<GRID_XZ>				MODIFYLIST;
	typedef std::list<GRID_XZ>::iterator	MODIFYLIST_ITER;

	MODIFYLIST		m_listModify;
	MODIFYLIST		m_listModifyWAIT;

	typedef std::list<GRID>				GRID_LIST;
	typedef std::list<GRID>::iterator	GRID_LIST_ITER;

	GRID_LIST		m_listPOINT_EDIT;			// 브러쉬 크기에 따른 포인트 리스트
	GRID_LIST		m_listTILE_EDIT;			// 브러쉬 크기에 따른 타일 리스트

	typedef std::map<DWORD,GRID_LIST>				GRIDEX_MAP;
	typedef std::map<DWORD,GRID_LIST>::iterator		GRIDEX_MAP_ITER;

	GRIDEX_MAP		m_mapSortPOINT;
	GRIDEX_MAP		m_mapSortTILE;

protected:
	struct BASE
	{
		LPDIRECT3DVERTEXBUFFERQ	pVB;				// Vertex Buffer ( XYZ|NORMAL|TEX1 )	// Color Mesh
		LPDIRECT3DINDEXBUFFERQ	pIB;				// Index Buffer

		DxBlend			sBlend;						// 타일 맵
		DxColor			sDxColor;					// 컬러 맵

		VERTEX*			pVertex;					// [EM_VERTEX_PART][EM_VERTEX_PART];	// EDITING 을 위한 위치 정보
		TILE*			pTile;						// [EM_GRID][EM_GRID];	// 타일 정보

		void		SaveFile ( CSerialFile &SFile, BOOL bThread );
		void		LoadFile ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, BOOL bThread );

		BASE() :
			pVB(NULL),
			pIB(NULL),
			pVertex(NULL),
			pTile(NULL)
		{
		};
		~BASE()
		{
			SAFE_RELEASE ( pVB );
			SAFE_RELEASE ( pIB );

			SAFE_DELETE_ARRAY ( pVertex );
			SAFE_DELETE_ARRAY ( pTile );
		};
	};

	class QuadBase : public OBJQUAD
	{
	public:
		int			nID_X;
		int			nID_Z;
		D3DXVECTOR3	vMax;
		D3DXVECTOR3	vMin;

		BASE*		pBase;

		QuadBase*	pNext;

	public:
		QuadBase() :
			pBase(NULL),
			nID_X(0),
			nID_Z(0),
			vMax(0.f,0.f,0.f),
			vMin(0.f,0.f,0.f)
		{
		};

		~QuadBase()
		{
			SAFE_DELETE(pBase);
			SAFE_DELETE(pNext);
		}

		virtual void GetQUADSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
		{
			_vMax = vMax; _vMin = vMin;
		}

		virtual void GetQUADSize ( int& nX, int& nZ )
		{
			nX = nID_X;	nZ = nID_Z;
		}

		inline virtual BOOL IsDetectDivision ( const D3DXVECTOR3 &vDivMax, const D3DXVECTOR3 &vDivMin )
		{
			return NSQUADTREE::IsWithInPoint ( vDivMax, vDivMin, (vMax+vMin)/2 );
		}

		virtual BOOL IsDetectDivision ( const int &nMaxX, const int &nMaxZ, const int &nMinX, const int &nMinZ )
		{
			return NSQUADTREE::IsCollisionPoint ( nMaxX, nMaxZ, nMinX, nMinZ, nID_X, nID_Z );
		}

		virtual OBJQUAD* GetNext () { return pNext; }

		virtual BOOL IsCollisionLine ( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir, float& fLength );

		virtual HRESULT FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, BOOL bGame );
		virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );

		BASE*	GetBase ( LPDIRECT3DDEVICEQ pd3dDevice );
		BOOL	IsRenderEnalble ();
		BOOL	IsEditEnalble ();

	protected:
		void	CreateBaseData ( LPDIRECT3DDEVICEQ pd3dDevice );

	public:
		void		SaveFile ( CSerialFile &SFile );
		void		LoadFile ( CSerialFile &SFile );

		void		NewFile ( const char* pName );
		void		SaveFile ( const char* pName, BOOL bThread );
		BASE*		LoadFile ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bThread );
	};

	QuadBase*		m_pQuadList;
	OBJQUADNode*	m_pQuadTree;

protected:
	DxPieceMan	m_sPieceMan;
public:
	DxPieceMan*	GetPieceMan()		{ return &m_sPieceMan; }

protected:
	struct LOADING_THREAD
	{
		DWORD	dwThreadID;			//	스레드 ID
		HANDLE	hThread;			//	스레드 핸들
		HANDLE	hEventTerminate;	//	스레드 종료

		LOADING_THREAD () :
			dwThreadID ( UINT_MAX ),
			hThread ( NULL ),
			hEventTerminate ( NULL )
		{
		};

		struct DATA
		{
			LPDIRECT3DDEVICEQ	pd3dDevice;
			BOOL				bEnable;
			BOOL				bDelete;
			DATA() :
				bEnable(FALSE),
				bDelete(FALSE),
				pd3dDevice(NULL)
			{
			};
		};

		DATA	sData;

		BOOL	StartThread ( LPDIRECT3DDEVICEQ pd3dDevice );
		void	EndThread ();

		static unsigned int WINAPI LoadTileThread( LPVOID pData );
	};

	LOADING_THREAD	m_sLoadingThread;

protected:
	static D3DXVECTOR3	m_vMin;			// < Save Load >
	static D3DXVECTOR3	m_vMax;			// < Save Load >
	static int			m_nTILE_X;			// 선택된 타일 X
	static int			m_nTILE_Z;			// 선택된 타일 Z
	static int			m_nPOINT_X;			// 선택된 점 X
	static int			m_nPOINT_Z;			// 선택된 점 Z
	static int			m_nTILE_X_S;		// 선택된 타일 X ( Short )
	static int			m_nTILE_Z_S;		// 선택된 타일 Z ( Short )
	static int			m_nPOINT_X_S;		// 선택된 점 X ( Short )
	static int			m_nPOINT_Z_S;		// 선택된 점 Z ( Short )
	static D3DXVECTOR3	m_vPOINT;			// 선택된 점 위치

protected:
	DWORD		m_dwGrid_Length;	// Grid * Length
	int			m_nBASE_X;			// 선택된 타일 큰것
	int			m_nBASE_Z;			// 선택된 타일 큰것
	BOOL		m_bFINDTILE;		// 현재 파일이 선택 되었나 ?

	//EDIT
public:
	static DWORD	m_dwLength_EDIT;	// 1개 폴리곤의 길이 - Default : 40		< Save, Load >

public:
	BOOL	m_bBlushCircle_EDIT;	// Edit 용	// 브러쉬 원,네모
	int		m_nBlushScale_EDIT;		// Edit 용

	float	m_fHeightValue_EDIT;	// Edit 용
	BOOL	m_bAbsolute_EDIT;		// Edit 용	// 절대높이		<-> #define	USE_RELATIVE
	BOOL	m_bUneven_EDIT;			// Edit 용	// 울퉁불퉁하게	<-> 평평하게
	BOOL	m_bUp_EDIT;				// Edit 용	// 올린다.		<->	내린다.

	BOOL	m_bTILE_DRAW;			// Edit 용	// 타일을 그림.
	BOOL	m_bCOLOR_DRAW;			// Edit 용	// 컬러를 그림.

	float	m_fBright_EDIT;			// Edit 용	// 밝기 조절
	float	m_fRandom_EDIT;			// Edit 용	// 랜덤 조절
	int		m_nColor_EDIT;			// Edit 용	// 현재 쓰이는 컬러를 얻는다.

	std::string	m_szTileName_USE;	// Use		// 현재 쓰이는 타일을 알고 있는다.
	std::string m_szDefaultFileName;

public:
	HRESULT ActiveMap ();

public:
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT InvalidateDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

	virtual HRESULT CleanUp ()
	{
		//InvalidateDeviceObjects();

		DeleteDeviceObjects();

		FinalCleanup();

		return S_OK;
	};

	//		static Method 들..
protected:
	static void			RenderColor ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB );
	static void			ModifyNormal ( VERTEX* pVertex, DWORD x, DWORD z );
	static D3DXVECTOR3	ModifyNormalPart ( VERTEX* pVertex, int x, int z, int x1, int y1, int x2, int y2 );
	static void			ChangeBig2Short ( D3DXVECTOR3 vPos, int ID_X, int ID_Z, int x, int z );

	//		타일,점 찾기
public:
	void		FindTiles ( D3DXVECTOR3& vPickRayOrig, D3DXVECTOR3& vPickRayDir );

protected:
	void		FindPointList ();
	void		FindTileList ();
	BOOL		IsCircle ( float x1, float z1, float x2, float z2, float _fLength );
	BOOL		IsPointEX ( int& EX_X, int& EX_Z, int& x, int& z );
	BOOL		IsTileEX ( int& EX_X, int& EX_Z, int& x, int& z );
	void		MakePointMap ();
	void		MakeTileMap ();

	//		Grid
public:
	void		ModifyGrid ( LPDIRECT3DDEVICEQ pd3dDevice );		// 크기를 변화 시킨다.

protected:
	void		CreateQuadList ( int x, int z );

	//		높이 조절
public:
	void		ModifyHeight_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice );
	void		ModifyHeight_RENDER ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	GRID_XZ		GetCloneRange ( GRID sGrid );
	void		CloneSrc2Dest ( LPDIRECT3DDEVICEQ pd3dDevice, EDITDATA* pEditData, GRID_XZ& sGrid );
	void		CloneDest2Src ( LPDIRECT3DDEVICEQ pd3dDevice, EDITDATA* pEditData, GRID_XZ& sGrid );
	void		SetEditDataNormal ( EDITDATA* pEditData );
	D3DXVECTOR3	GetEditDataNormal_Part ( D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2 );
	void		Set_Tile_VB_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice, GRID_XZ& sGrid );
	void		Set_Tile_VB ( BASE* pBase, D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

	void		ModifyVertexList_WAIT ( int x, int z );
	void		CloneVertexList ( EDITDATA* pEditData );
	void		SetVertexCurve_MAIN ( EDITDATA* pEditData );
	void		ModifyVertex ( EDITDATA* pEditData, GRID_XZ& sGrid );
	BOOL		ModifyVertex ( D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2 );

	//		 타일 셋팅
public:
	void		ModifyTexUV_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice );
	void		ModifyTexUV_RENDER ( LPDIRECT3DDEVICEQ pd3dDevice );
	void		ModifyTexUV_RENDER_PART ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3* vPos, D3DXVECTOR3* vDir, int x1, int z1, int x2, int z2 );

	void		SetTileData ( const char* pName );
	void		DelTileData ( const char* pName );

protected:
	void		ColorChange_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice, GRID_XZ& sGrid, int x, int z );
	D3DXCOLOR	GetEditColor ();

	void		TileColorChange_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice, GRID_XZ& sGrid, int x, int z );
	void		CheckTileUSE_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice, GRID_XZ& sGrid );

	//		 Piece 셋팅
public:
	void		SetPiece ( D3DXVECTOR3& vPickRayOrig, D3DXVECTOR3& vPickRayDir );
	void		SetPiece ( LPDIRECT3DDEVICEQ pd3dDevice );

	//		ETC
public:
	void		SaveTileData ( const char* pName );
	void		LoadTileData ( const char* pName );
	void		NewTileData ();

	void		EditMode ();

	//		Main
public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bGame );
	HRESULT Render_PieceEDIT ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void LoadSet ( char	*pFileName, LPDIRECT3DDEVICEQ pd3dDevice );
	void SaveSet ( char	*pFileName );

public:
	DxMapEditMan(void);
	~DxMapEditMan(void);
};

