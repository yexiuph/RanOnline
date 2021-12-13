//
//	NEED : DxAnimation::LoadFile () ���� ����� ������ ���� �������.
//
//
//
//
//
//
//
//
#ifndef	__DX_ANIMATION_INFO__
#define	__DX_ANIMATION_INFO__

#include "./DxAniKeys.h"
#include "Collision.h" //../[Lib]__Engine/Sources/DxCommon
#include "./DxEffectFrame.h"

class	DxOctree;
struct	DxFrame;
class	CSerialFile;
struct	CLIPVOLUME;
class	DxAnimationMan;

enum DXANIOPTION
{
	DXANI_MOD		= 0x0001,
};

struct DxAnimation
{
	char*			szName;				//	���ϸ��̼ǵ� ������ �̸�.
	DxFrame			*pframeToAnimate;	//	���ϸ��̼� ������ �Ǵ� �������� ����Ŵ.

	SPositionKey	*m_pPositionKeys;	//	�̵� ����Ű.
	UINT			m_cPositionKeys;
	
	SRotateKey		*m_pRotateKeys;		//	ȸ�� ����Ű.
	UINT			m_cRotateKeys;
	
	SScaleKey		*m_pScaleKeys;		//	ũ�� ����Ű.
	UINT			m_cScaleKeys;
	
	SMatrixKey		*m_pMatrixKeys;		//	��Ʈ���� ����Ű.
	UINT			m_cMatrixKeys;

	DxAnimation		*pAnimNext;			//	���ϸ��̼� ����Ʈ ��� ����.

	DxAnimation () :
		szName(NULL),
		pframeToAnimate(NULL),
		m_pPositionKeys(NULL),
		m_cPositionKeys(0),
		m_pRotateKeys(NULL),
		m_cRotateKeys(0),
		m_pScaleKeys(NULL),
		m_cScaleKeys(0),
		m_pMatrixKeys(NULL),
		m_cMatrixKeys(0),
		pAnimNext(NULL)
	{
	}

	~DxAnimation ()
	{
		SAFE_DELETE_ARRAY(szName);
		SAFE_DELETE_ARRAY(m_pPositionKeys);
		SAFE_DELETE_ARRAY(m_pRotateKeys);
		SAFE_DELETE_ARRAY(m_pScaleKeys);
		SAFE_DELETE_ARRAY(m_pMatrixKeys);

		SAFE_DELETE(pAnimNext);
		// do NOT delete pframeToAnimate
	}

	LPD3DXMATRIX SetTime ( float fGlobalTime, DWORD dwFlag=DXANI_MOD );
	float GetAnimTimeLine ();

public:
	void operator= ( DxAnimation& rValue );

public:
	BOOL SaveFile ( CSerialFile &SFile );
	BOOL LoadFile ( CSerialFile &SFile, DxAnimationMan *pDxAnimation=NULL );
};

enum ANIMATETYPE { ANT_LOOP, ANT_RND, ANT_HIT };
class DxAnimationMan : public OBJAABB
{
public:
	ANIMATETYPE		m_AniType;

protected:
	float			fCurTime;
	D3DXMATRIX		m_matWorld;
	DxFrame*		m_pDxFrameRoot;
	DxFrame*		m_pDxAlphaFrameHead;
	
	float			m_fUNITTIME;
	DxAnimation*	m_pDxAnimationHead;

	PDXEFFECTBASE	m_pEffectHead;

public:
	DxAnimationMan*	m_pNext;

public:
	void InsertDxAnimation ( DxAnimation *pDxAnimation );
	void SetDxFrameRoot ( DxFrame *pDxFrameRoot ) { m_pDxFrameRoot = pDxFrameRoot; }
	void SetAnimationHead ( DxAnimation* pAnimHead ) { m_pDxAnimationHead = pAnimHead; }
	DxFrame* GetDxFrameRoot () { return m_pDxFrameRoot; }
	DxFrame* FindFrame( const char* szName );
	
	void SetUnitTIme ( float fUnitTime ) { m_fUNITTIME = fUnitTime; }
	float GetUnitTime ()	{ return m_fUNITTIME; }

public:
	void SetCurTime( float _fCurTime )		{ fCurTime = _fCurTime*UNITANIKEY_PERSEC; }
	void SetTime( const float fAniTime )	{ fCurTime = fAniTime; }
	void FrameMove ( float fTime, float fElapsedTime );

protected:
	void SetTime ();
	void UpdateFrames( DxFrame *pframeCur, const D3DXMATRIX &matCur );
	void UpdateFrames_EDIT( DxFrame *pframeCur, const D3DXMATRIX &matCur );
	HRESULT DrawFrames ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const D3DXMATRIX& matParent, const float fAlpha );
	HRESULT DrawFramesAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const float fAlpha );
	HRESULT DrawFramesSTOP( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const D3DXMATRIX& matParent, const float fAlpha );
	HRESULT DrawFramesAlphaSTOP( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const float fAlpha );

public:
	BOOL SaveFile ( CSerialFile &SFile );
	BOOL LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DWORD	GetSaveSetSize ();
	void	SaveSet ( CSerialFile &SFile );

public:
	DxAnimationMan ();
	~DxAnimationMan();

	//	Note : OBJAABB
	//
public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
	virtual OBJAABB* GetNext () { return m_pNext; }

public:
	void SetWorldMatrix ( D3DXMATRIX &mCur ) {	m_matWorld = mCur; }
	virtual HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const float fAlpha );
	void Render_EDIT( LPDIRECT3DDEVICEQ pd3dDevice, const float fAlpha );
	void UpdateFrames( const D3DXMATRIX &matCur );
	void UpdateFrames_EDIT( const D3DXMATRIX &matCur );


	//	Note : ���� ������ - EDITER, DEBUG, SAVESET ��.
	//
public:
	static DWORD	m_dwAniTypeNum;
	static char		m_szAniType[3][24];
	static DWORD	m_dwAniManCount;
	static DWORD	m_dwVERSION;
};

typedef	DxAnimationMan*	LPDXANIMATIONMAN;

#endif	//	__DX_ANIMATION_INFO__