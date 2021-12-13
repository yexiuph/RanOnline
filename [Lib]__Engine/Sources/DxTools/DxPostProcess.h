#pragma once

#include "DxVertexFVF.h"

class DxPostProcess
{
private:
	BOOL	m_bProjectActive;		// 프로젝트에서 동작/미동작
	BOOL	m_bOptionEnable;		// 옵션에서의 활성화
	BOOL	m_bDeviceEnable;		// 장비에서의 체크
	BOOL	m_bEnable;				// DxPostProcess이 사용가능한지 체크

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
	LPDIRECT3DVERTEXBUFFER9 m_pScreenVB;	// 화면으로 옮길 것.
	LPDIRECT3DVERTEXBUFFER9 m_pModulateVB;	// 값의 제곱
	LPDIRECT3DVERTEXBUFFER9 m_pBlurVB[10];	// 이미지 블러

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

	void ImageModulateHLSL( LPDIRECT3DDEVICEQ pd3dDevice );		// 이미지를 강조를 시킴
	void ImageModulateASSEM( LPDIRECT3DDEVICEQ pd3dDevice );	// 이미지를 강조를 시킴
	void ImageBlur( LPDIRECT3DDEVICEQ pd3dDevice );	// 이미지를 뭉겜

private:
	BOOL ValueEDIT();	// 셋팅 변경

protected:
	DxPostProcess();

public:
	virtual ~DxPostProcess();

public:
	static DxPostProcess& GetInstance();
};
