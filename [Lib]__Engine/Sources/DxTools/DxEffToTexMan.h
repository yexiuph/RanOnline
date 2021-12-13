
#include <vector>
#include "DxVertexFVF.h"

class DxEffToTexMan
{
protected:
	struct DATA
	{
		LPDIRECT3DSURFACEQ	pSuf;
		LPDIRECT3DTEXTUREQ	pTex;

		DATA();
		~DATA();
	};

protected:
	typedef std::vector<DATA*>		VECDATA;
	typedef VECDATA::iterator		VECDATA_ITER;

protected:
	VECDATA			m_vecData;
	int				m_nTexSize;
	int				m_nTexNumber;
	BOOL			m_bTexRepeat;

protected:
	LPDIRECT3DSURFACEQ	m_pMainSuf;
	LPDIRECT3DTEXTUREQ	m_pMainTex;

protected:
	VERTEXCOLORRHW	m_sVertexF[4];
	VERTEXCOLORRHW	m_sVertexB[4];

public:
	void SetTexSize( int nTexSize )		{ m_nTexSize = nTexSize; }
	void SetTexNumber( int nNumber )	{ m_nTexNumber = nNumber; }
	void SetTexRepeat( BOOL bRepeat )	{ m_bTexRepeat = bRepeat; }

public:
	void InsertData( LPDIRECT3DDEVICEQ pd3dDevice );
	void EndEffToTex( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void EndEffToTex128( LPDIRECT3DDEVICEQ pd3dDevice );
	void EndEffToTex256( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DWORD ComputeAlpha( int nCount);
	void MakeSequenceRepeat( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTex );
	void SequnceMake_00( int nCount, BOOL bFace, int nBaseCol, VERTEXCOLORRHW sVertex[] );	// 偎熱陛 礎熱 橾 唳辦
	void SequnceMake_01( int nCount, BOOL bFace, int nBaseCol, VERTEXCOLORRHW sVertex[] );	// 偎熱陛 �汝� 橾 唳辦

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

public:
	void CleanUp();

public:
	DxEffToTexMan();
	virtual ~DxEffToTexMan();

public:
	static DxEffToTexMan& GetInstance();
};
