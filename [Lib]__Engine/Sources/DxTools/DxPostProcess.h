#pragma once

#include "DxVertexFVF.h"

class DxPostProcess
{
private:
	BOOL	m_bProjectActive;		// ������Ʈ���� ����/�̵���
	BOOL	m_bOptionEnable;		// �ɼǿ����� Ȱ��ȭ
	BOOL	m_bDeviceEnable;		// ��񿡼��� üũ
	BOOL	m_bEnable;				// DxPostProcess�� ��밡������ üũ

public:
	void	SetProjectActiveON()			{ m_bProjectActive=TRUE; }
	void	SetOptionEnable( BOOL bUse )	{ m_bOptionEnable=bUse; }
	BOOL	IsOptionEnable()				{ return m_bOptionEnable; }
	BOOL	IsDeviceEnable()				{ return m_bDeviceEnable; }
	BOOL	IsEnable()						{ return m_bEnable; }

private:
	LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	LPDIRECT3DSTATEBLOCK9	m_pDrawSB;

private:
	LPD3DXEFFECT 			m_pPossasiEFF;
	LPDIRECT3DPIXELSHADER9	m_pPossasiPS;

	D3DXHANDLE	m_pTechnique;

private:
	LPDIRECT3DVERTEXBUFFER9 m_pScreenVB;	// ȭ������ �ű� ��.
	LPDIRECT3DVERTEXBUFFER9 m_pModulateVB;	// ���� ����
	LPDIRECT3DVERTEXBUFFER9 m_pBlurVB[10];	// �̹��� ��

private:
	DWORD				m_nWidth;
	DWORD				m_nHeight;
	LPDIRECT3DTEXTUREQ	m_pTexture;
	LPDIRECT3DSURFACEQ	m_pSurface;
	LPDIRECT3DTEXTUREQ	m_pTexTEMP;
	LPDIRECT3DSURFACEQ	m_pSufTEMP;

public:
	void RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	void InvalidateDeviceObjects();

public:
	void FrameMove();
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	BOOL CreateTexAndClear( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwWidth, DWORD dwHeight, 
							LPDIRECT3DTEXTUREQ&	pTex, LPDIRECT3DSURFACEQ& pSuf );
	void CreateBlurVB( LPDIRECT3DDEVICEQ pd3dDevice, UINT i, float x, float y, float fAlpha );	// i ->Count

	void ImageModulateHLSL( LPDIRECT3DDEVICEQ pd3dDevice );		// �̹����� ������ ��Ŵ
	void ImageModulateASSEM( LPDIRECT3DDEVICEQ pd3dDevice );	// �̹����� ������ ��Ŵ
	void ImageBlur( LPDIRECT3DDEVICEQ pd3dDevice );	// �̹����� ����

private:
	BOOL ValueEDIT();	// ���� ����

protected:
	DxPostProcess();

public:
	virtual ~DxPostProcess();

public:
	static DxPostProcess& GetInstance();
};
