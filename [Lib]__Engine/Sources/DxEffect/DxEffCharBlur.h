#pragma once

#include <queue>

#include "CMemPool.h"

#include "DxEffChar.h"

#include "./TextureManager.h"
#include "DxMethods.h"
#include "DxVertexFVF.h"

struct EFFCHAR_PROPERTY_BLUR_100 : public EFFCHAR_PROPERTY
{
	BOOL			m_bUse;
	BOOL			m_bBezier;
	BOOL			m_bAlphaVel;
	float			m_fAlphaVel;
	int				m_nBlend;
	DWORD			m_dwTraceIndexUP;			//	���� ������ �ε���.
	DWORD			m_dwTraceIndexDOWN;			//	���� ������ �ε���.
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_BLUR_100()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_BLUR_101 : public EFFCHAR_PROPERTY
{
	BOOL			m_bUse;						// ���̺� �� �ʿ�� ���� ���δ�.. ���� �������� ������ ������.
	BOOL			m_bBezier;
	int				m_nBlend;
	DWORD			m_dwTraceIndexUP;			//	���� ������ �ε���.
	DWORD			m_dwTraceIndexDOWN;			//	���� ������ �ε���.
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;
	BOOL			m_bIndexMID;
	float			m_fRateMID;
	D3DCOLOR		m_vColorMID;

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_BLUR_101()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_BLUR_102 : public EFFCHAR_PROPERTY
{
	BOOL			m_bBezier;
	int				m_nBlend;
	DWORD			m_dwTraceIndexUP;			//	���� ������ �ε���.
	DWORD			m_dwTraceIndexDOWN;			//	���� ������ �ε���.
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	BOOL			m_bIndexMID;				//	�߰� �÷�
	float			m_fRateMID;					//
	D3DCOLOR		m_vColorMID;				//

	BOOL			m_bAllMType;				//	Ÿ�̹� �� �ʿ��� �κп� �ִ� ��
	BOOL			m_bAllSType;				//
	BOOL			m_bAllTime;					//
	DWORD			m_dwKeyStartTime;			//
	DWORD			m_dwKeyEndTime;				//
	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_BLUR_102()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_BLUR_103 : public EFFCHAR_PROPERTY
{
	BOOL			m_bBezier;
	int				m_nBlend;
	char			m_szTraceUP[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	���� ������ �ε���.
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	BOOL			m_bIndexMID;				//	�߰� �÷�
	float			m_fRateMID;					//
	D3DCOLOR		m_vColorMID;				//

	BOOL			m_bAllMType;				//	Ÿ�̹� �� �ʿ��� �κп� �ִ� ��
	BOOL			m_bAllSType;				//
	BOOL			m_bAllTime;					//
	DWORD			m_dwKeyStartTime;			//
	DWORD			m_dwKeyEndTime;				//
	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_BLUR_103()
	{
		memset( m_szTraceUP, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceDOWN, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_BLUR_104 : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	char			m_szTraceUP[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	���� ������ �ε���.
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	float			m_fRateMID;					//
	D3DCOLOR		m_vColorMID;				//

	BOOL			m_bAllMType;				//	Ÿ�̹� �� �ʿ��� �κп� �ִ� ��
	BOOL			m_bAllSType;				//
	BOOL			m_bAllTime;					//
	DWORD			m_dwKeyStartTime;			//
	DWORD			m_dwKeyEndTime;				//
	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_BLUR_104()
	{
		memset( m_szTraceUP, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceDOWN, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

#define		NOUSEORIGIN		0x0001
#define		USEREFRACT		0x0002
#define		NOUSE_BEZIER	0x0004	// ���� ��� ���� �ʴ´�.
#define		NOUSE_MIDDLE	0x0008	// ���� ��� ���� �ʴ´�.
#define		USEALLACTION	0x0010	// ��� �Ϳ� �� ���´�.

#define		TEXTURE_LENGTH	128

struct EFFCHAR_PROPERTY_BLUR : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	float			m_fFullLifeTemp;
	TCHAR			m_szTraceUP[STRACE_NSIZE];		//	���� ������ �ε���.
	TCHAR			m_szTraceDOWN[STRACE_NSIZE];	//	���� ������ �ε���.
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	TCHAR			m_szTexture[ TEXTURE_LENGTH ];

	EFFCHAR_PROPERTY_BLUR () :
		m_nBlend(1), 
		m_fFullLifeTemp(0.25f),
		m_vColorUPTemp(D3DCOLOR_ARGB(255,255,255,255)),
		m_vColorDOWNTemp(D3DCOLOR_ARGB(0,255,255,255)),
		m_SetAniMType(AN_ATTACK),
		m_SetAniSType(AN_SUB_NONE)
	{
		m_dwFlag = 0L;

		StringCchCopy( m_szTexture, TEXTURE_LENGTH, "1d_Lighting.bmp" );

		StringCchCopy( m_szTraceUP,		STRACE_NSIZE, STRACE_IMAGE01 );
		StringCchCopy( m_szTraceDOWN,	STRACE_NSIZE, STRACE_IMAGE02 );
	}
};

class DxEffCharBlur : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_BLUR m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;

			int				m_nBlend;
			float			m_fFullLifeTemp;
			TCHAR			m_szTraceUP[STRACE_NSIZE];		//	���� ������ �ε���.
			TCHAR			m_szTraceDOWN[STRACE_NSIZE];	//	���� ������ �ε���.
			D3DCOLOR		m_vColorUPTemp;
			D3DCOLOR		m_vColorDOWNTemp;

			EMANI_MAINTYPE	m_SetAniMType;				//
			EMANI_SUBTYPE	m_SetAniSType;				//

			TCHAR			m_szTexture[ TEXTURE_LENGTH ];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_BLUR*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	LPDIRECT3DTEXTUREQ	m_pddsTexture;

private:
	float		m_fTime;
	float		m_fElapsedTime;
	D3DXMATRIX	m_matIdentity;

	// Note : �ִϸ��̼ǿ� ���� ������ ��.
private:
	struct VERTEX_UPDOWN
	{
		D3DXVECTOR3	sPos[2];
	};

	typedef std::deque<VERTEX_UPDOWN*>	DEQ_STOREPOINT;

	DWORD			m_dwKeyPointCOUNT;
	DEQ_STOREPOINT	m_deqKeyPoint;

	typedef CMemPool<VERTEX_UPDOWN>	VERTEX_UPDOWN_POOL;
    static VERTEX_UPDOWN_POOL*		m_pVertexUpDownPool;


	// Note : �ܻ��� ����� ���ؼ� ��� ���,�����Ǵ� ��.
private:
	struct BLUR_UPDOWN
	{
		VERTEXCOLORTEX	sVertex[2];

		float			fLife;
	};

	typedef std::deque<BLUR_UPDOWN*>	DEQ_BLURPOINT;
	typedef DEQ_BLURPOINT::iterator		DEQ_BLURPOINT_ITER;

	DEQ_BLURPOINT	m_deqBlurPoint;

	typedef CMemPool<BLUR_UPDOWN>	BLUR_UPDOWN_POOL;
    static BLUR_UPDOWN_POOL*		m_pBlurUpDownPool;


	// Note : Į�� �پ� �ִ� �ܻ�
private:
	BOOL			m_bBaseBlurRender;
	VERTEXCOLORTEX	m_sBaseBlur[4];

private:
	float		m_fOneDiveFullLife;	// 1.f / FullLife
	DWORD		m_dwPrevKeyFrame;	// ���� Ű �������� ���� ���� ���´�.

private:
	float			m_fFullLife;
	D3DCOLOR		m_vColorUP;
	D3DCOLOR		m_vColorDOWN;

	float	m_fAlpha[2];	// ���õ� ���İ�
	DWORD	m_dwColor[2];	// ���õ� �÷� ��

private:
	BOOL IsUnionKey( D3DXVECTOR3& vUp, D3DXVECTOR3& vDown );
	void CreateNewKeyFrame( D3DXVECTOR3& vUp, D3DXVECTOR3& vDown );
	void FrameMoveKeyFrame();
	void ClearKeyFrame();
	void ClearBlurDara();
	void CreateBlur();
	void FrameMoveBlur();

private:
	int SeparateNumOfLine();				// Line�� ���� ����
	void DxInsertBlurPoint( float fRate );

private:
	void RenderBlur( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnCreateDevice_STATIC();
	static void OnDestroyDevice_STATIC();

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender=FALSE );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharBlur(void);
	~DxEffCharBlur(void);
};
