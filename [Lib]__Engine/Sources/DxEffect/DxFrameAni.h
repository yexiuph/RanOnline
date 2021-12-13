#pragma once

#include <map>

#include "./DxTextureEffMan.h"

struct DxFrame;
class DxAnimationMan;
class DxTexEffBase;

//-------------------------------------------------------------------------------------------------------
//	Note : 자신이 속해있는 Frame을 알기 위해 Link를 하고, Attribute 를 사용하여 VB,IB 호출횟수를 줄임.
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
//	Note : Texture 중심으로 만들어 진다. VB를 모았다.
//			Texture 가 같지만 Mesh 가 분리 되어 있는것을 모은것이다.
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
	DWORD		m_dwNumVB;	// 임시
	VERTEX*		m_pArrayVB;	// 임시
	DWORD		m_dwFaces;	// 임시
	WORD*		m_pArrayIB;	// 임시

	DxFrameAttribute*	m_pFrameAttribHead;	// 임시

protected:
	ANIMESH*					m_pAniMesh;		// Main	( VB 너무 클 경우 여러개 있는것을 가정해서 List 형식으로 만듬 )
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

	void CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, const char* pName, DxTextureEffMan* pTexEffMan );	// 찌그러기들이 모인것을 생성.
	void CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice );										// 메쉬 거대화로 인한 현재 모인 메쉬의 생성.

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
//	Note : 1개의 Animation FrameRoot
//			여러개의 DxFrameAni가 있을 수 있다.
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
