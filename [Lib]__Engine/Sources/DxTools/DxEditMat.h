#pragma once

#include "DxFrameMesh.h"
#include "DxMethods.h"

enum EDITMAT_TYPE
{
	TRANS_MAT,
	ROTATE_MAT,
	SCALE_MAT
};

class DxEditMat : public DxObject
{
protected:
	HWND			m_hWnd;
	DxFrameMesh		m_CurMesh;

	char			m_szDefaultPath[MAX_PATH];

protected:
	LPDXFRAME		m_pDxFrameSel;
	D3DXMATRIX		m_matCombFrame;

	BOOL			m_bAffineChanged;
	DXAFFINEPARTS	m_AffineParts;
	DXAFFINEMATRIX*	m_pAffineMatrix;

protected:
	EDITMAT_TYPE	m_EditType;

protected:
	BOOL GetKeyMinus ();
	BOOL GetKeyPlus ();

protected:
	void GetWorldMatrix ( D3DXMATRIX &mat );

public:
	void SetType ( EDITMAT_TYPE Type );

	void SetParam ( char *szPath, HWND hWnd ); 
	void SetFrameMatrix ( D3DXMATRIX &matCombFrame );
	void SetAffineMatrix ( DXAFFINEMATRIX* pAffineMatrix );
	
public:
	void GetAffineParts ( LPDXAFFINEPARTS pAffineParts )	{ *pAffineParts = m_AffineParts; }

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEditMat(void);
	virtual ~DxEditMat(void);
};
