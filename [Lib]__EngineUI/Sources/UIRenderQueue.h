#ifndef _UI_RENDER_QUEUE_H
#define _UI_RENDER_QUEUE_H

class CUIControl;
struct UIVERTEX;

//--------------------------------------------------------------------
// UIRenderQueue : √÷¡ÿ«ı
//--------------------------------------------------------------------
class CUIRenderQueue
{
public:
	static BOOL						m_bUsage;

protected:
	static CUIRenderQueue*			m_pInstance;

	LPDIRECT3DDEVICEQ				m_pd3dDevice;
	LPDIRECT3DTEXTUREQ				m_pTexture;
	LPDIRECT3DVERTEXBUFFERQ			m_pVertexBuffer;

	UIVERTEX*						m_pVertex;
	UINT							m_uiOffset;
	UINT							m_uiVertex;
	UINT							m_uiPrimitiv;
	DWORD							m_dwLockFlag;

public:
	CUIRenderQueue();
	virtual ~CUIRenderQueue();

	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID InvalidateDeviceObjects();
	VOID DeleteDeviceObjects();

	BOOL Render( LPDIRECT3DTEXTUREQ pTexture, VOID* pVertex );
	VOID RenderImmediately();

	static CUIRenderQueue* Get();
	static VOID CleanUp();
};

#endif