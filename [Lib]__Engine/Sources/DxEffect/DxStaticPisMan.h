#pragma once

#include <map>

#include "Collision.h"

#include "DxPieceContainer.h"

class DxAABBPieceOBJ : public OBJAABB
{
public:
	D3DXVECTOR3		vMax;		// matWorld �� ���� ������ Max
	D3DXVECTOR3		vMin;		// matWorld �� ���� ������ Min

	std::string		szSrcName;		// �����͸� �޾ƾ���.
	D3DXMATRIX		matWorld;
	DxPieceEdit*	pStaticPisEdit;

	DxAABBPieceOBJ*	pNext;

public:
	virtual void GetAABBSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
	{
		_vMax = vMax; _vMin = vMin;
	}
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, (vMax+vMin)/2 );
	}
	virtual OBJAABB* GetNext () { return pNext; }

	virtual BOOL IsCollisionLine ( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor );

public:
	virtual HRESULT Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const char* szName )
	{
//		pStaticPis->Render ( pd3dDevice, sCV, matWorld, szName );
		return S_OK;
	}

public:
	void Create ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szDir=NULL );

protected:
	void ModifyMaxMin ( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin, const D3DXMATRIX& matWorld );

public:
	void Save ( CSerialFile& SFile );
	void Load ( CSerialFile& SFile );

public:
	DxAABBPieceOBJ();
	~DxAABBPieceOBJ();
};

class DxAABBPiece : public OBJAABB
{
public:
	D3DXVECTOR3		vMax;		// ��ü �ʿ����� Max			( �� Material�� ���Ͽ�.. )
	D3DXVECTOR3		vMin;		// ��ü �ʿ����� Max			( �� Material�� ���Ͽ�.. )

	DxAABBPieceOBJ*	pAABBList;
	OBJAABBNode*	pAABBTree;

public:
	DxAABBPiece*	pNext;		// DxPieceMan ���� Tree �� ����� ���� ��.

public:
	virtual void GetAABBSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
	{
		_vMax = vMax; _vMin = vMin;
	}
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, (vMax+vMin)/2 );
	}
	virtual OBJAABB* GetNext () { return pNext; }

	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, POBJAABB& pObj );

public:
	virtual HRESULT Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

public:
	DxAABBPiece();
	~DxAABBPiece();
};
typedef DxAABBPiece* PDXAABBPIECE;
typedef std::map<std::string,DxAABBPiece*>				MAPAABBPIECE;
typedef std::map<std::string,DxAABBPiece*>::iterator	MAPAABBPIECE_ITER;

class DxPieceMan
{
protected:
	static const DWORD	VERSION;

protected:
	std::string		m_szName;

protected:
	MAPAABBPIECE	m_mapAABBPiece;		// Piece �������� ��� ���Ҵ�.
	OBJAABBNode*	m_pAABBPieceTree;

	// EDIT��
public:
	std::string			m_szNamePis;
	static D3DXVECTOR3	m_vPOINT;
	static D3DXVECTOR3	m_vROTATE;
	static D3DXVECTOR3	m_vSTARTPOS;
	static D3DXVECTOR3	m_vENDPOS;
	static D3DXMATRIX	m_matWorld;
	OBJAABBNode*		m_pSelectOBJ;

protected:
	struct LOADING_THREAD
	{
		DWORD	dwThreadID;			//	������ ID
		HANDLE	hThread;			//	������ �ڵ�
		HANDLE	hEventTerminate;	//	������ ����

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

		BOOL	IsActive ()		{ return sData.bDelete ? FALSE : TRUE; }

		static unsigned int WINAPI LoadTileThread( LPVOID pData );
	};

	LOADING_THREAD	m_sLoadingThread;

public:
	BOOL SetPiece ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, D3DXMATRIX& matWorld );

public:
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT DeleteDeviceObjects();

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );
	HRESULT Render_SELECT ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );
	HRESULT Render_EDIT ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );

public:
	void SetPickRay ( D3DXVECTOR3& vPickRayOrig, D3DXVECTOR3& vPickRayDir );
	void SetMousePoint ( D3DXVECTOR3& vPos )				{ m_vPOINT = vPos; }
	void SetRotate ( D3DXVECTOR3& vRotate )					{ m_vROTATE = vRotate; }
	void SetPisName ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );
	void SetLButtonClick ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void Save ( const char* szName );
	void Load ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const char* szDir=NULL );

protected:
	void CleanUp();

public:
	DxPieceMan();
	~DxPieceMan();
};


namespace NSMATERIALMESH
{
//	typedef std::map<DxMaterialMAP*,BOOL>			MAPMATERIALLOAD;
//	typedef std::map<DxMaterialMAP*,BOOL>::iterator	MAPMATERIALLOAD_ITER;

//	extern MAPMATERIALLOAD	m_mapLOADING;		// ������ �ε��� �ʿ��ϴ�.

//	void MakeMaterialList ( DxAABBPieceOBJ*	pHead, MAPMATERIALROOT& mapData, MAPMATERIALROOT& mapAlpha );
//	void MakeBaseTree ( MAPMATERIALROOT& mapData );

	void ConvertMAPtoList ( PDXAABBPIECE& pHead, MAPAABBPIECE& mapData );
}
