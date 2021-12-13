#pragma once

class DxAnimationMan;
class DxStaticMesh;
class DxFrameMesh;

#include "./DxEffectFrame.h"
#include "DxEffSinglePropGMan.h"
#include "./DxEffSinglePropGMan.h"
#include "DxEffSingle.h"

//----------------------------------------------------------------------------------------------------------------------
//									D	x		L	a	n	d		E	f	f
//----------------------------------------------------------------------------------------------------------------------
class DxLandEff
{
protected:
	// Note : �ʿ��� File Name ����
	std::string			m_strFrameName;

	// Note : ȿ�� ����
	EFF_PROPGROUP*		m_pPropGroup;
	DxEffSingleGroup*	m_pSingleGroup;

	// Note : ��ġ ����
	DxFrame*	m_pFrame;
	D3DXMATRIX	m_matLocal;	// ȿ�� �ڽ��� Matrix		// Save, Load
	D3DXMATRIX	m_matFrame;	// ���� �����ִ� Matrix		// ����
	D3DXMATRIX	m_matLast;	// ���� Matrix				// Edit �ÿ��� ���

protected:
	// Note : ȿ���� �浹 ��ġ�� ���� ����.
	BOOL		m_bGroudPos;	// ������ġ�� �ʿ��Ѱ�?									// SAVE, LOAD
	BOOL		m_bCheckColl;	// 1�� üũ�ϸ� TRUE�� �ٲ��. ���� Collision�� ����.	// NO
	D3DXVECTOR3	m_vCollPos;																// NO

public:
	std::string			m_strEffName;	// Play ȿ�� �̸�
	std::string			m_strEditName;	// Edit ���� ���̴� ȿ�� �̸�

public:
	BOOL CreateEff( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame, const D3DXVECTOR3& vPos, 
					const char* szFrameName, const char* szEffName, const char* szEditName );
	void SetMatrix()	{ if( m_pFrame ) m_matFrame = m_pFrame->matCombined; }
	LPD3DXMATRIX GetEffMatrix();
	void ModifyMatrix();
	BOOL ModifyEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* pEffName );

	void SetCheckCollPoint( BOOL bUse )	{ m_bGroudPos = bUse; }
	BOOL GetCheckCollPoint()			{ return m_bGroudPos; }

public:
	void SetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void CleanUp();

public:
	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh );

public:
	void Save( CSerialFile& SFile );
	void LoadEdit( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAnimation );

	void LoadEdit100( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh );
	void Load100( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAnimation );

public:
	void Export( LPDIRECT3DDEVICEQ pd3dDevice, DxLandEff* pSrcEff, DxAnimationMan* pAniHead );
	void Clone( LPDIRECT3DDEVICEQ pd3dDevice, DxLandEff* pSrcEff );

public:
	DxLandEff();
	~DxLandEff();
};

//----------------------------------------------------------------------------------------------------------------------
//									D	X		P	I	E	C	E		E	F	F
//----------------------------------------------------------------------------------------------------------------------
class DxPieceEff
{
protected:
	static const DWORD VERSION;

	// Note : Replace Eff
protected:
	DWORD			m_dwEffectCount;
	PDXEFFECTBASE	m_pEffectHead;

	// Note : Single Eff
public:
	typedef	std::vector<DxLandEff*>	VECLANDEFF;
	typedef	VECLANDEFF::iterator	VECLANDEFF_ITER;

	VECLANDEFF	m_vecLandEff;

public:
	BOOL SetEff( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame, const D3DXVECTOR3& vPos, 
				const char* szFrameName, const char* szEffName, std::string& strEditName );	// strEditName �� Tool ���� ���� ����.
	void DelEff( const char* szEffName );
	LPD3DXMATRIX GetEffMatrix( const char* szEffName );
	void ModifyMatrix();
	void ModifyEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szEditName, const char* szEffName );

	void SetCheckCollPoint( const char* szEffName, BOOL bUse );
	BOOL GetCheckCollPoint( const char* szEffName );
	const char* GetEffName( const char* pEditName );

protected:
	void CreateEditEffName( const char* pEffName, std::string& strEditName );

public:
	void SetMatrix();
	void SetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );
public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();
	void CleanUp();

public:
	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh );

public:
	void SaveEdt( CSerialFile& SFile, DxFrameMesh* pFrameMesh );
	void LoadEdt( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh );

	void Save( CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAnimation );

public:
	void Export( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceEff* pSrcEff, DxFrameMesh* pFrameMesh, DxAnimationMan* pAniHead );
	void Clone( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceEff* pSrcEff );

protected:
	HRESULT EffectAdaptToList( DxFrame *pFrameRoot, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
								PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc );

	HRESULT EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, PDXEFFECTBASE &pEffList );

public:
	DxPieceEff ();
	~DxPieceEff();
};