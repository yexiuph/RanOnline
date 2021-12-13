#pragma once

class DxDynamicVB
{
private:
	struct BIGVB
	{
		LPDIRECT3DVERTEXBUFFERQ	pVB;
		UINT					nOffsetToLock;
		UINT					nVertexCount;
		UINT					nFullByte;
		DWORD					dwFVF;

		void CreateVB( LPDIRECT3DDEVICEQ pd3dDevice );
		void ReleaseVB();
	};

public:
	static BIGVB	m_sVB;		// Base m_sVB_PDT2	기본값임.
	static BIGVB	m_sVB_PDT;
	static BIGVB	m_sVB_PNT;
	static BIGVB	m_sVB_PT;
	static BIGVB	m_sVB_PWDT;
	static BIGVB	m_sVB_PDT2;
	static BIGVB	m_sVB_CHAR;

	static DWORD					DYNAMIC_MAX_FACE;
	static LPDIRECT3DINDEXBUFFER9	m_pIBuffer;

public:
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects();

	void FrameMove();	// 매 m_pVB 프레임 리셋. !!

protected:
	DxDynamicVB(void);

public:
	~DxDynamicVB(void);

public:
	static DxDynamicVB& GetInstance();
};

#include "CMemPool.h"
#include "DxVertexFVF.h"

class OPTMBase
{
protected:
	struct RenderState
	{
		BOOL	m_bChange;
		DWORD	m_dwValue;

		void SetValue( DWORD dwValue )
		{
			m_bChange = TRUE;
			m_dwValue = dwValue;
		}

		RenderState() :
			m_bChange(FALSE),
			m_dwValue(0L)
		{
		}
	};

	struct SamplerState
	{
		BOOL	m_bChange;
		DWORD	m_dwSampler;
		DWORD	m_dwValue;

		void SetValue( DWORD dwValue )
		{
			m_bChange = TRUE;
			m_dwValue = dwValue;
		}

		SamplerState() :
			m_bChange(FALSE),
			m_dwSampler(0L),
			m_dwValue(0L)
		{
		}
	};

	struct TextureStageState
	{
		BOOL	m_bChange;
		DWORD	m_dwStage;
		DWORD	m_dwValue;

		void SetValue( DWORD dwValue )
		{
			m_bChange = TRUE;
			m_dwValue = dwValue;
		}

		TextureStageState() :
			m_bChange(FALSE),
			m_dwStage(0L),
			m_dwValue(0L)
		{
		}
	};

protected:
	static LPDIRECT3DDEVICEQ		m_pd3dDevice;

protected:
	LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	LPDIRECT3DSTATEBLOCK9	m_pDrawSB;

protected:
	inline void UserMatrix( D3DXMATRIX* pIdentity, D3DXMATRIX* pWorld, BOOL& bChange );
	inline void UserRenderState( D3DRENDERSTATETYPE dwState, RenderState& sBase, const RenderState& sRS );
	inline void UserTextureStageState( D3DTEXTURESTAGESTATETYPE dwType, TextureStageState& sBase, const TextureStageState& sTSS );

protected:
	OPTMBase(void);

public:
	~OPTMBase(void);
};

class OPTMSingleSequence : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류

		BOOL				bChangeMatrix;
		D3DXMATRIX			matWorld;

		RenderState			sCULLMODE;
		RenderState			sDESTBLEND;
		RenderState			sZWRITEENABLE;
		RenderState			sALPHABLENDENABLE;
		TextureStageState	sCOLOROP;

		DATA* pNext;

		DATA() :
			nVertexCount(0),
			nFaceCount(0),
			pTex(NULL),
			pNext(NULL),
			bChangeMatrix(FALSE)
		{
		}
	};

private:
	typedef CMemPool<DATA>	DATAPOOL;
	DATAPOOL*				m_pDataPool;

	DATA*	m_pDataHead;

private:
	BOOL				m_bBaseMatrix;
	RenderState			m_sBaseCullMode;
	RenderState			m_sBaseDestBlend;
	RenderState			m_sBaseZWriteEnable;
	RenderState			m_sBaseAlphaBlendEnable;
	TextureStageState	m_sBaseColorOP;
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->New(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );						// Insert Data

public:
	OPTMSingleSequence(void);
	~OPTMSingleSequence(void);
};

class OPTMSingleGround : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류

		D3DXMATRIX			matWorld;
		DWORD				dwFVF;
		D3DMATERIAL9*		pMaterial;

		RenderState			sLIGHTING;
		RenderState			sDESTBLEND;
		RenderState			sZWRITEENABLE;
		RenderState			sALPHABLENDENABLE;
		RenderState			sALPHAREF;
		TextureStageState	sCOLOROP;

		DATA* pNext;

		DATA() :
			nVertexCount(0),
			nFaceCount(0),
			pTex(NULL),
			pMaterial(NULL),
			pNext(NULL)
		{
		}
	};

private:
	typedef CMemPool<DATA>	DATAPOOL;
	DATAPOOL*				m_pDataPool;

	DATA*	m_pDataHead;

private:
	RenderState			m_sBaseALPHAREF;
	RenderState			m_sBaseLighting;
	RenderState			m_sBaseDestBlend;
	RenderState			m_sBaseZWriteEnable;
	RenderState			m_sBaseAlphaBlendEnable;
	TextureStageState	m_sBaseColorOP;
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->New(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );						// Insert Data

public:
	OPTMSingleGround(void);
	~OPTMSingleGround(void);
};

class OPTMSingleBlurSys : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류

		D3DXMATRIX*			pmatWorld;

		RenderState			sDESTBLEND;
		RenderState			sZWRITEENABLE;
		RenderState			sALPHABLENDENABLE;
		TextureStageState	sCOLOROP;

		DATA* pNext;

		DATA() :
			nVertexCount(0),
			nFaceCount(0),
			pTex(NULL),
			pmatWorld(NULL),
			pNext(NULL)
		{
		}
	};

private:
	typedef CMemPool<DATA>	DATAPOOL;
	DATAPOOL*				m_pDataPool;

	DATA*	m_pDataHead;

private:
	BOOL				m_bMatrixChange;
	RenderState			m_sBaseDestBlend;
	RenderState			m_sBaseZWriteEnable;
	RenderState			m_sBaseAlphaBlendEnable;
	TextureStageState	m_sBaseColorOP;
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->New(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );						// Insert Data

public:
	OPTMSingleBlurSys(void);
	~OPTMSingleBlurSys(void);
};

class OPTMSingleParticle : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류

		BOOL				bChangeMatrix;
		D3DXMATRIX			matWorld;

		RenderState			sCULLMODE;
		RenderState			sSRCBLEND;
		RenderState			sDESTBLEND;
		RenderState			sZWRITEENABLE;
		RenderState			sALPHABLENDENABLE;
		TextureStageState	sCOLOROP;

		DATA* pNext;

		DATA() :
			nVertexCount(0),
			nFaceCount(0),
			pTex(NULL),
			pNext(NULL),
			bChangeMatrix(FALSE)
		{
		}
	};

private:
	typedef CMemPool<DATA>	DATAPOOL;
	DATAPOOL*				m_pDataPool;

	DATA*	m_pDataHead;

private:
	BOOL				m_bBaseMatrix;
	RenderState			m_sBaseCullMode;
	RenderState			m_sBaseDestBlend;
	RenderState			m_sBaseZWriteEnable;
	RenderState			m_sBaseAlphaBlendEnable;
	TextureStageState	m_sBaseColorOP;
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->New(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );						// Insert Data

public:
	OPTMSingleParticle(void);
	~OPTMSingleParticle(void);
};

//--------------------------------------------------------------------------------------------------------------------------
// Note : DxEffCharParticle, DxEffCharBonePosEff, DxCharBoneListEff	적용
//--------------------------------------------------------------------------------------------------------------------------
class OPTMCharParticle : public OPTMBase
{
public:
	struct DATA
	{
		UINT				nVertexCount;	// 시작 점
		UINT				nFaceCount;		// 면 갯수

		LPDIRECT3DTEXTUREQ	pTex;			// Texture 종류
		BOOL				bAlphaTex;		// 알파 텍스쳐 사용.

		DATA* pNext;

		DATA() :
			nVertexCount(0),
			nFaceCount(0),
			pTex(NULL),
			bAlphaTex(FALSE),
			pNext(NULL)
		{
		}
	};

private:
	typedef CMemPool<DATA>	DATAPOOL;
	DATAPOOL*				m_pDataPool;

	DATA*	m_pDataHead;

private:
	LPDIRECT3DTEXTUREQ	m_pBaseTexture;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	// Interface : GetData() 로 DATA를 얻은 후 수정한 다음 
	//				InsertData()로 삽입을 감행한다.
public:
	DATA* GetData()		{ return m_pDataPool->New(); }	// 사용할 DATA를 얻는다.
	void InsertData( DATA* pData );						// Insert Data

public:
	OPTMCharParticle(void);
	~OPTMCharParticle(void);
};


class OPTMManager
{
public:
	OPTMSingleParticle	m_sSingleParticle;
	OPTMSingleSequence	m_sSingleSequence;
	OPTMSingleGround	m_sSingleGround;
	OPTMSingleBlurSys	m_sSingleBlurSys;
	OPTMCharParticle	m_sCharParticle;

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void FrameMove();						// 매 m_pVB 프레임 리셋. !!
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	OPTMManager(void);

public:
	~OPTMManager(void);

public:
	static OPTMManager& GetInstance();
};

