#ifndef DXLANDSTATES_H_
#define DXLANDSTATES_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
//	����. ���� ���� �� �ν��Ͻ��� ���ÿ� �̸� ȣ���ϴ� �Լ���
//		���� �Լ��� ��� ���� ���� ���������� ȣ��Ǹ鼭 �� �����Ͱ�
//		������ �߻��� ������ ����.
//
//
//
//

class DxRenderStates
{
protected:
	//	���ĸ� �Ѹ���. (0x80) ����Ʈ����.
	LPDIRECT3DSTATEBLOCK9 m_pSavedAlphaMapRS;
	LPDIRECT3DSTATEBLOCK9 m_pDrawAlphaMapRS;

	//	���ĸ� �Ѹ���. (0x80) ����Ʈ �ѱ�.
	LPDIRECT3DSTATEBLOCK9 m_pONSavedAlphaMapRS;
	LPDIRECT3DSTATEBLOCK9 m_pONDrawAlphaMapRS;

	//	Soft Alpha
	LPDIRECT3DSTATEBLOCK9 m_pONSavedSoftAlphaSB;
	LPDIRECT3DSTATEBLOCK9 m_pONDrawSoftAlphaSB;

	//	GUI
	LPDIRECT3DSTATEBLOCK9 m_pSavedUIMapRS;
	LPDIRECT3DSTATEBLOCK9 m_pDrawUIMapRS;

	//	3D ��ư
	LPDIRECT3DSTATEBLOCK9 m_pSavedMake3DMapRS;
	LPDIRECT3DSTATEBLOCK9 m_pDrawMake3DMapRS;

	//	3D ��ư �ؽ�ó
	LPDIRECT3DSTATEBLOCK9 m_pSavedRender3DMapRS;
	LPDIRECT3DSTATEBLOCK9 m_pDrawRender3DMapRS;

	//	MiniMap
	LPDIRECT3DSTATEBLOCK9 m_pSavedRenderMiniMapRS;
	LPDIRECT3DSTATEBLOCK9 m_pDrawRenderMiniMapRS;

	//	Land	-	z write
	LPDIRECT3DSTATEBLOCK9 m_pSavedAlpha1LandRS;
	LPDIRECT3DSTATEBLOCK9 m_pDrawAlpha1LandRS;

	//	Land	-	no z write
	LPDIRECT3DSTATEBLOCK9 m_pSavedAlpha2LandRS;
	LPDIRECT3DSTATEBLOCK9 m_pDrawAlpha2LandRS;

	//	Land	-	disable alpha test
	LPDIRECT3DSTATEBLOCK9 m_pSavedAlpha3LandRS;
	LPDIRECT3DSTATEBLOCK9 m_pDrawAlpha3LandRS;

	//	visible:off �ɸ��Ϳ�
	LPDIRECT3DSTATEBLOCK9 m_pSavedAlphaVisibleOff;
	LPDIRECT3DSTATEBLOCK9 m_pDrawAlphaVisibleOff;

	//	���� �׸���
	LPDIRECT3DSTATEBLOCK9 m_pSavedShadowSB;
	LPDIRECT3DSTATEBLOCK9 m_pDrawShadowSB;

	//	ĳ���� �׸���
	LPDIRECT3DSTATEBLOCK9 m_pSavedShadowColorSB;
	LPDIRECT3DSTATEBLOCK9 m_pDrawShadowColorSB;

public:
	void SetAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice );		//	����Ʈ ���� ���ĸ�.
	void ReSetAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetOnAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice );		//	����Ʈ �Ѵ� ���ĸ�.
	void ReSetOnAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetOnSoftAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice );		//	Soft Alpha
	void ReSetOnSoftAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetUIMap( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReSetUIMap( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetMake3DMap( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReSetMake3DMap( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetRender3DMap( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReSetRender3DMap( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetRenderMiniMap( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReSetRenderMiniMap( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetAlphaLand ( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReSetAlphaLand ( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetAlphaNoZWriLand ( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReSetAlphaNoZWriLand ( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetAlphaNoTestLand ( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReSetAlphaNoTestLand ( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetAlphaVisibleOff ( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReSetAlphaVisibleOff ( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetShadow ( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReSetShadow ( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetShadowColor ( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReSetShadowColor ( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetOnAlphaApply( LPDIRECT3DDEVICEQ pd3dDevice )		{ m_pONDrawAlphaMapRS->Apply(); }
	void SetOnSoftAlphaApply( LPDIRECT3DDEVICEQ pd3dDevice )	{ m_pONDrawSoftAlphaSB->Apply(); }

protected:
	void SAFE_RELEASE_BLOCK( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSTATEBLOCK9 pBLOCK );

public:
	virtual HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT InvalidateDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);

protected:
	DxRenderStates(void);

public:
	~DxRenderStates(void);

public:
	static DxRenderStates& GetInstance();
};

#endif // DXLANDSTATES_H_