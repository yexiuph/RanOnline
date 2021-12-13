// ReplaceContainer.h: interface for the CReplaceContainer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <map>
#include <string>
#include "./StlFunctions.h"

#include "DxPieceDefine.h"
#include "./GLLIST.h"

#include "DxSkinMeshContainer9.h"
#include "DxSkinMesh9.h"

#include "DxSkinObject.h"
#include "DxSkinAniMan.h"

#define STRACE_NSIZE	(32)
#define STRACE_IMAGE01 ("IMAGE01")		// 0
#define STRACE_IMAGE02 ("IMAGE02")
#define STRACE_IMAGE11 ("IMAGE11")
#define STRACE_IMAGE12 ("IMAGE12")
#define STRACE_IMAGE21 ("IMAGE21")
#define STRACE_IMAGE22 ("IMAGE22")		//	5

#define STRACE_STRIKE01 ("STRIKE01")	//	6
#define STRACE_STRIKE02 ("STRIKE02")
#define STRACE_STRIKE11 ("STRIKE11")
#define STRACE_STRIKE12 ("STRIKE12")	//	9

#define STRACE_ARROW01 ("ARROW01")		//	10

#define STRACE_RAIN01 ("RAIN01")		//	11
#define STRACE_RAIN02 ("RAIN02")
#define STRACE_RAIN03 ("RAIN03")		//	13

#define STRACE_BLOOD01 ("BLOOD01")		//	14

#define STRACE_ENERGY01 ("ENERGY01")	//	15
#define STRACE_ENERGY02 ("ENERGY02")
#define STRACE_ENERGY03 ("ENERGY03")	//	17

#define MAX_TRACE	(18)
extern char szTRACE_NAME[MAX_TRACE][STRACE_NSIZE];

#ifndef DxEffChar
	class DxEffChar;
#endif

class CSerialFile;

typedef std::vector<DxEffChar*>						GLEFFCHAR_VEC;
typedef GLEFFCHAR_VEC::iterator						GLEFFCHAR_VEC_ITER;
typedef GLEFFCHAR_VEC::reverse_iterator				GLEFFCHAR_VEC_RITER;

struct STRACOR
{
	char			m_szName[STRACE_NSIZE];
	SVERTEXINFLU	m_sVertexInflu;
	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vNormal;
	D3DXMATRIX		m_BoneMatrice;

	STRACOR ();
	STRACOR ( const char* _szName, const SVERTEXINFLU &_sVertexInflu, D3DXVECTOR3 _vPos );
	STRACOR ( const STRACOR& sTracor );

	bool operator== ( const STRACOR& lvalue );
};
typedef std::vector<STRACOR>	VECTRACE;
typedef VECTRACE::iterator		VECTRACE_ITER;

struct STRACOR_OPER
{
	bool operator() ( const STRACOR& lvalue, const STRACOR& rvalue )
	{
		return strcmp(lvalue.m_szName,rvalue.m_szName)<0;
	}

	bool operator() ( const STRACOR& lvalue, const char* szName )
	{
		return strcmp(lvalue.m_szName,szName)<0;
	}
};

struct SMATERIAL_PIECE
{
	enum
	{
		VERSION				= 0x0100,
		FLAG_ALPHATEXTURE	= 0x0001,
	};

	std::string			m_strTexture;
	LPDIRECT3DTEXTUREQ	m_pTexture;

	DWORD				m_dwFlags;
	DWORD				m_dwZBias;
	float				m_fZBias;			// Optimize - m_dwZBias -	Don`t Save Load
	DWORD				m_dwDyVertStart;	// 동적버텍스 렌더시작점-	Don`t Save Load
	DWORD				m_dwTexID;			// 텍스쳐 ID			-	Don`t Save Load

	SMATERIAL_PIECE () :
		m_pTexture(NULL),
		m_dwFlags(NULL),
		m_dwZBias(0),
		m_fZBias(0.f),
		m_dwDyVertStart(0),
		m_dwTexID(0)
	{
	}

	void SaveFile ( CSerialFile &SFile );
	void LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT LoadTexture ( LPDIRECT3DDEVICEQ pd3dDevice );

	SMATERIAL_PIECE ( const SMATERIAL_PIECE &value )
	{
		operator= ( value );
	}

	void operator= ( const SMATERIAL_PIECE &value )
	{
		m_strTexture = value.m_strTexture;
		m_pTexture = value.m_pTexture;

		m_dwFlags = value.m_dwFlags;
		m_dwZBias = value.m_dwZBias;
	}
};


struct DxSkinPiece
{
public:
	const static DWORD	VERSION;
	const static char	m_szPIECETYPE[PIECE_SIZE][64];

public:
	char*				m_szFileName;

	DWORD				m_dwRef;
	
	char*				m_szXFileName;
	DxSkinMesh9*		m_pSkinMesh;

	char*				m_szSkeleton;
	DxSkeleton*			m_pSkeleton;

	EMPIECECHAR			m_emType;
	EMPEACEZONEWEAPON	m_emWeaponWhereBack;
	char*				m_szMeshName;
	PSMESHCONTAINER		m_pmcMesh;
	DWORD				m_dwVertexNUM;			// 정점 갯수를 미리.. 넣어 놓는다.

	DWORD				m_dwMaterialNum;
	SMATERIAL_PIECE*	m_pMaterialPiece;

	// Note : 무슨 효과가 들어있는지를 알아내는 Flag 실시간 변함
	//
	DWORD				m_dwFlags;

	// Note : 효과 셋팅, Save, Load
	DWORD				m_dwFlag;

	//	Note : 추적 위치값 들.
	//
	VECTRACE			m_vecTrace;

	//	Note : 스킨 조각에 부착된 효과들.
	//
	GLEFFCHAR_VEC		m_vecEFFECT;

protected:
	void SumEffFlag ();

public:
	void SelectMesh ( PSMESHCONTAINER pmcMesh );

public:
	void AddEffList ( DxEffChar* pEffChar );
	void DelEffList ( DxEffChar* pEffChar );

public:
	HRESULT SetSkinMesh ( const char* szXFileName, const char* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread );

public:
	HRESULT LoadPiece ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread );
	HRESULT SavePiece ( const char* szFile );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();
	void ClearAll ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DxSkeleton* pSkeleton );
	HRESULT MeshEditRender ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DxSkeleton* pSkeleton );
	HRESULT RenderTEST( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DxSkeleton* pSkeleton );

public:
	HRESULT DrawMeshContainer ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOriginDraw=TRUE, BOOL bAlpha=FALSE, BOOL bShadow=FALSE, BOOL bAlphaTex=TRUE );
	HRESULT DrawTraceVert ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DrawSelTraceVert ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szSelVert );

public:
	SVERTEXINFLU* GetTracePos ( std::string strTrace );
	void SetTracePos ( std::string strTrace, SVERTEXINFLU &sVertInflu );
	void DelTracePos ( std::string strTrace );

	//	pBoneMatrice -> (주의) 영향을 받는 본의 갯수가 여러개일 경우에 문제가 생김.
	HRESULT CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice=NULL );

	HRESULT CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	DWORD GetVertexNUM()	{ return m_dwVertexNUM; }	// 정점 갯수.

public:
	void CreateCartoonMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fThickness, float fSizeADD );
	void CreateNormalMapMESH( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxSkinPiece ();
	~DxSkinPiece ();
};
typedef DxSkinPiece*	PDXSKINPIECE;

class DxSkinPieceContainer  
{
private:
	typedef std::map<std::string,DxSkinPiece*>				PIECEMAP;
	typedef std::map<std::string,DxSkinPiece*>::iterator	PIECEMAP_ITER;

protected:
	char			m_szPath[MAX_PATH];

public:
	void			SetPath ( char* szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	char*			GetPath ()					{ return m_szPath; }

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	PIECEMAP			m_mapSkinPiece;

public:
	DxSkinPiece* LoadPiece ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread );
	HRESULT ReleasePiece ( const char* szFile );
	HRESULT DeletePiece ( const char* szFile );
	DxSkinPiece* FindPiece ( const char* szFile );

public:
	HRESULT CleanUp ();

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

protected:
	DxSkinPieceContainer();

public:
	virtual ~DxSkinPieceContainer();

public:
	static DxSkinPieceContainer& GetInstance();
};

