#pragma once

#include <map>

#include "./DxTextureEffMan.h"

struct DxFrame;
class DxAnimationMan;
class DxTexEffBase;

//-------------------------------------------------------------------------------------------------------
//	Note : �ڽ��� �����ִ� Frame�� �˱� ���� Link�� �ϰ�, Attribute �� ����Ͽ� VB,IB ȣ��Ƚ���� ����.
//-------------------------------------------------------------------------------------------------------
class DxFrameAttribute
{
protected:
	DxFrame*	m_pFrameLink;
	DWORD		m_dwVertexStart;
	DWORD		m_dwVertexCount;
	DWORD		m_dwFaceStart;
	DWORD		m_dwFaceCount;

public:
	DxFrameAttribute* m_pNext;

public:
	void SetFrameAttribute( DxFrame* pFrame, const DWORD& dwVertexStart, const DWORD& dwVertexCount, 
											const DWORD& dwFaceStart, const DWORD& dwFaceCount );

public:
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void Save( CSerialFile& SFile );
	void Load( CSerialFile& SFile, DxAnimationMan* pAniManHead );
	void CleanUp();

public:
	DxFrameAttribute();
	~DxFrameAttribute();
};

//------------------------------------------------------------------------------------------------
//	Note : Texture �߽����� ����� ����. VB�� ��Ҵ�.
//			Texture �� ������ Mesh �� �и� �Ǿ� �ִ°��� �������̴�.
//------------------------------------------------------------------------------------------------
class DxFrameAniVBTEX
{
protected:
	struct VERTEX
	{
		D3DXVECTOR3 vPos; 
		D3DXVECTOR3 vNor; 
		D3DXVECTOR2 vTex;
		static const DWORD FVF;
	};

	struct ANIMESH
	{
		DWORD					m_dwVertices;	// Main
		LPDIRECT3DVERTEXBUFFERQ	m_pVB;			// Main
		DWORD					m_dwFaces;		// Main
		LPDIRECT3DINDEXBUFFERQ	m_pIB;			// Main
		DxFrameAttribute*		m_pFrameAttrib;	// Main

		ANIMESH*	m_pNext;

		void CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, VERTEX* pArrayVB, const DWORD& dwVertices, 
						WORD* pArrayIB, const DWORD& dwFaces, DxFrameAttribute* pAttribute );

		void Save( CSerialFile& SFile );
		void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAniManHead );
		void CleanUp();

		ANIMESH();
		~ANIMESH();
	};

protected:
	static const DWORD	VERSION;

protected:
	DWORD		m_dwNumVB;	// �ӽ�
	VERTEX*		m_pArrayVB;	// �ӽ�
	DWORD		m_dwFaces;	// �ӽ�
	WORD*		m_pArrayIB;	// �ӽ�

	DxFrameAttribute*	m_pFrameAttribHead;	// �ӽ�

protected:
	ANIMESH*					m_pAniMesh;		// Main	( VB �ʹ� Ŭ ��� ������ �ִ°��� �����ؼ� List �������� ���� )
	char*						m_pTexName;		// Main
	LPDIRECT3DTEXTUREQ			m_pTex;			// Main
	TextureManager::EM_TEXTYPE	m_emType;
	DWORD						m_dwFlag;		// Save, Load

public:
	void AddCloneMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame, LPD3DXMESH pMesh, 
						const DWORD& dwVertexStart, const DWORD& dwVertexCount, 
						const DWORD& dwFaceStart, const DWORD& dwFaceCount );
	void AddCloneMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB,
						const DWORD& dwVertexStart, const DWORD& dwVertexCount, const DWORD& dwFaceStart, const DWORD& dwFaceCount );

	void CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, const char* pName, DxTextureEffMan* pTexEffMan );	// ��׷������ ���ΰ��� ����.
	void CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice );										// �޽� �Ŵ�ȭ�� ���� ���� ���� �޽��� ����.

public:
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderTexEff( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pTexEffList );
	void RenderOnly( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void Save( CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAniManHead, const char* pName );
	void CleanUp();

public:
	DxFrameAniVBTEX();
	~DxFrameAniVBTEX();
};

//------------------------------------------------------------------------------------------------
//	Note : 1���� Animation FrameRoot
//			�������� DxFrameAni�� ���� �� �ִ�.
//------------------------------------------------------------------------------------------------
class DxFrameAni
{
protected:
	static const DWORD	VERSION;

protected:
	typedef std::map<std::string,DxFrameAniVBTEX*>	MAPTEXFRAMEANI;
	typedef MAPTEXFRAMEANI::iterator				MAPTEXFRAMEANI_ITER;

	MAPTEXFRAMEANI	m_mapFrameAni;

public:
	void SetFrameLink( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame );
	void CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEffMan );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pTexEffList );
	void Render_EDIT( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void Save( CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAniManHead );

public:
	DxFrameAni();
	~DxFrameAni();
};
