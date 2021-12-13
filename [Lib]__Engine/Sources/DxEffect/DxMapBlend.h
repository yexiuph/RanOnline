#pragma once

#include <algorithm>
#include <map>
#include <list>

#define	USE_LENGTH		0x0001

enum	DEFAULT_MAP_SETTING
{	
	EM_TEXSIZE		= 128,							// �ؽ��� ������
	EM_TEXSIZE_2	= EM_TEXSIZE-2,					// �ؽ��� ������
	EM_TEXSIZE_4	= EM_TEXSIZE-4,					// �ؽ��� ������
	EM_TEXSIZE_4_A	= EM_TEXSIZE_4+EM_TEXSIZE_4,	// �ؽ��� ������
	EM_TEXSIZE_EDIT	= EM_TEXSIZE+EM_TEXSIZE-2,		// Edit���� ���
	EM_GRID			= 31,							// ���� ��
	EM_QURD			= EM_GRID*EM_GRID,				// ���� ��
	EM_FACE			= EM_QURD*2,					// ���� ��
	EM_VERTEX_PART	= 32,								// ���ؽ� ��
	EM_VERTEX_ALL	= EM_VERTEX_PART*EM_VERTEX_PART,	// ���ؽ� �� ���μ��� ��ü
	EM_VERTEX_EDIT	= EM_VERTEX_PART+EM_VERTEX_PART-1,	// Edit �Ҷ� ���̴� ���ؽ� ��
	EM_QURD_VERTEX	= EM_QURD*4,							// �簢������ ������ ���� ���� ����
};

struct TILE_TEX
{
	std::string	szTexture;
};

struct VERTEX_XT2
{
	D3DXVECTOR3			vPos;
	D3DXVECTOR2			vTex1;
	D3DXVECTOR2			vTex2;
	const static DWORD	FVF;
};

struct VERTEX_XNT2
{
	D3DXVECTOR3			vPos;
	D3DXVECTOR3			vNor;
	D3DXVECTOR2			vTex1;
	D3DXVECTOR2			vTex2;
	const static DWORD	FVF;
};

struct TILE
{
	VERTEX_XNT2	sVertex[4];	// ���� ����
};

class DxBlendData
{
protected:
	BOOL		m_bEnable;

protected:
	DWORD					m_dwGrid;		// ���� ��.						< Edit, Game >
	DWORD					m_dwFaces;		// �� ��.						< Edit, Game >
	DWORD					m_dwVertices;	// �� ��.						< Edit, Game >
	LPDIRECT3DVERTEXBUFFERQ	m_pVB;			// VB.							< Edit, Game >
	LPDIRECT3DINDEXBUFFERQ	m_pIB;			// IB.							< Edit, Game >
	LPDIRECT3DTEXTUREQ		m_pTex;			// ����ڰ� ���� Ÿ�� ��.		< Edit, Game >
	LPDIRECT3DTEXTUREQ		m_pAlphaTex;	// ���α׷��� ���� ���� ��.		< Edit, Game >
	BOOL*					m_pTileUSE;		// IB ������ ���ؼ� �ʿ�.		< Edit>			[EM_GRID][EM_GRID]
	BOOL*					m_pAlphaUSE;	// AphaTex ������ ���ؼ� �ʿ�.	< Edit>			[EM_TEXSIZE][EM_TEXSIZE]

	std::string				m_szBaseTex;

public:
	BOOL	IsEnable()	{ return m_bEnable; }

public:
	void	InitData ( LPDIRECT3DDEVICEQ pd3dDevice, TILE* pTile );			// �⺻ �ʱ�ȭ
	void	InitData ( LPDIRECT3DDEVICEQ pd3dDevice, const char* cName );	// New �ʱ�ȭ

	void	ModifyAlphaTex ( int _X, int _Y, BOOL bUSE );					// ���� �ؽ��� ������

	BOOL	CheckTileUSE ();												// VB, IB ������
	void	ModifyVB ( LPDIRECT3DDEVICEQ pd3dDevice,TILE* pTile );			// VB, IB ������
	void	ModifyIB ( LPDIRECT3DDEVICEQ pd3dDevice );						// VB, IB ������

	void	Modify_Height_PART ( TILE* pTile );								// ���� ������ ������

	void	Render ( LPDIRECT3DDEVICEQ pd3dDevice );						// ����

protected:
	void	InitAlphaTex ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwColor );	// ���� �ʱ�ȭ
	BOOL	IsTileGrid ( int initX, int initZ );							// CheckTileUSE() ���� Ÿ���� ���̴��� �˾Ƴ��� ���� �Լ�

public:
	void	SaveFile_Edit ( CSerialFile &SFile );
	void	LoadFile_Edit ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, const char* pName );
	void	SaveFile_Game ( CSerialFile &SFile );
	void	LoadFile_Game ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, const char* pName );

public:
	DxBlendData ();
	~DxBlendData ();
};

typedef std::map<std::string,DxBlendData*>	BLENDMAP;
typedef BLENDMAP::iterator					BLENDMAP_ITER;

class DxBlend
{
protected:
	BLENDMAP		m_mapBlend;		// BLENDING �ϱ����� ������.	< Edit, Game >
	TILE_TEX*		m_pTileTex;		// �� ������ Ÿ���̸�			< Edit >		[EM_TEXSIZE][EM_TEXSIZE]

public:
	BOOL	IsEnable();
	void	Create ( LPDIRECT3DDEVICEQ pd3dDevice, TILE* pTile );
	void	Modify ( LPDIRECT3DDEVICEQ pd3dDevice, TILE* pTile );
	void	ModifyTex ( LPDIRECT3DDEVICEQ pd3dDevice, const char* strName, int nTempX, int nTempY );
	void	Modify_Height ( TILE* pTile );
	void	Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	void	SaveFile_Edit ( CSerialFile &SFile );
	void	LoadFile_Edit ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile );
	void	SaveFile_Game ( CSerialFile &SFile );
	void	LoadFile_Game ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile );

public:
	DxBlend ();
	~DxBlend ();
};

class DxColor
{
protected:
	BOOL		m_bEnable;

protected:
	LPDIRECT3DTEXTUREQ		m_pTex;				// ���� ��� ��	< Edit, Game >
	D3DXCOLOR*				m_pColor;			// ���� �÷�	< Edit >		[EM_TEXSIZE][EM_TEXSIZE]

public:
	BOOL	IsEnable()		{ return	m_bEnable; }
	void	Create ( LPDIRECT3DDEVICEQ pd3dDevice );
	void	ColorChange ( D3DXCOLOR _cColor, int _X, int _Y );

	LPDIRECT3DTEXTUREQ GetTexture()		{ return m_pTex; }

	void	SaveFile_Edit ( CSerialFile &SFile );
	void	LoadFile_Edit ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile );
	void	SaveFile_Game ( CSerialFile &SFile );
	void	LoadFile_Game ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile );

public:
	DxColor ();
	~DxColor ();
};