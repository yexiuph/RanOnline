#pragma once

#include "../[Lib]__EngineSound/Sources/DxSound/MovableSound.h"

#include <string>
#include <map>

class basestream;
class CSerialFile;
class DxEffSingle;
class DxEffSingleGroup;

#ifndef PDXEFFSINGLE
	typedef DxEffSingle* PDXEFFSINGLE;
#endif // PDXEFFSINGLE


//enum EFFECTFLAG		// 0x00000001 ����
//{
//	EFF_PLY_PLAY		= (0x00010),	//	EFFECT�� Play ��.
//	EFF_PLY_DONE		= (0x00020),	//	EFFECT�� Ȱ���ð��� ������.
//	EFF_PLY_END			= (0x00040),	//	EFFECT�� ���� �� �غ� ��.
//	EFF_PLY_RENDPASS	= (0x00100),	//	EFFECT�� �׷����� �ʰ� ������.
//
//	EFF_CFG_NEVERDIE	= (0x01000),			//	EFFECT�� ���� �ʴ� �Ӽ��� ������.		// üũ 
//	EFF_CFG_TARGETPOS	= (0x02000),			//	EFFECT�� Ÿ�� ��ġ�� ������.			// ����
//	EFF_CFG_TARGETID	= (0x04000),			//	EFFECT�� Ÿ�� 'ID'�� ������.			// ����
//	EFF_CFG_MOVE		= (0x08000),			//	EFFECT�� ���๰ü�� ���� �ٴѴ�.		// ����
//	EFF_CFG_NONE		= (0x0f000),�� �ƴҶ�	//  EFFECT�� ���ڸ����� ����				// ���� ( ����Ʈ )
//
//	EFF_CFG_EVENT		= (0x10000),	//	EFFECT�� event �� �߻��ÿ� Ȱ��ȭ (EFF_SLEEP->EFF_BEGIN).
//
//	EFF_CFG_USEDTARGET	= (EFF_CFG_TARGETPOS|EFF_CFG_TARGETID|EFF_CFG_MOVE),
//	EFF_CFG_NONE		= (EFF_CFG_NEVERDIE|EFF_CFG_TARGETPOS|EFF_CFG_TARGETID|EFF_CFG_MOVE),
//};

enum EFFECTFLAG
{
	EFF_PLY_PLAY		= (0x0000010),	//	EFFECT�� Play ��.
	EFF_PLY_DONE		= (0x0000020),	//	EFFECT�� Ȱ���ð��� ������.
	EFF_PLY_END			= (0x0000040),	//	EFFECT�� ���� �� �غ� ��.
	EFF_PLY_RENDPASS	= (0x0000100),	//	EFFECT�� �׷����� �ʰ� ������.

	EFF_CFG_TARGETLINE	= (0x0000200),	//  EFFECT�� Ÿ�ٱ����� ��

	EFF_CFG_NONE		= (0x0001000),	//  EFFECT�� ���ڸ����� ����				// ���� ( ����Ʈ )
	EFF_CFG_TARGETPOS	= (0x0002000),	//	EFFECT�� Ÿ�� ��ġ�� ������.			// ����
	EFF_CFG_TARGETID	= (0x0004000),	//	EFFECT�� Ÿ�� 'ID'�� ������.			// ����
	EFF_CFG_MOVE		= (0x0008000),	//	EFFECT�� ���๰ü�� ���� �ٴѴ�.		// ���� (�Ⱦ���)

	EFF_CFG_NEVERDIE	= (0x0010000),	//	EFFECT�� ���� �ʴ� �Ӽ��� ������.		// üũ 

	EFF_CFG_AVERAGE		= (0x1000000),	//	EFFECT�� �Ϲ����� �ӵ�
	EFF_CFG_SLOWFAST	= (0x2000000),	//	EFFECT�� ���� ��������.
	EFF_CFG_FASTSLOW	= (0x4000000),	//	EFFECT�� ���� ��������.
	EFF_CFG_ACCEL		= (0xf000000),	//	�ӵ� �Ӽ��� �� �����ٶ� ���δ�.

	EFF_CFG_EVENT		= (0x0100000),	//	EFFECT�� event �� �߻��ÿ� Ȱ��ȭ (EFF_SLEEP->EFF_BEGIN).

	EFF_CFG_AUTOAABBBOX	= (0x10000000),	//	�ڵ������� AABBBOX�� �����.

	EFF_CFG_USEDTARGET	= (EFF_CFG_TARGETPOS|EFF_CFG_TARGETID|EFF_CFG_MOVE|EFF_CFG_TARGETLINE),
};


//	Note : ���� ����Ʈ���� �������� ���� ���ҽ� �ڿ����� ����
//			�� struct�� ���� ����Ʈ�� ���۽ÿ� ��� �޾Ƽ� �̿� ������ ���� �־�� �Ѵ�.
//			���� ��� "struct PARTICLESYS_PROPERTY : public EFF_PROPERTY" �� ���� ���� ���̴�.
//
struct EFF_PROPERTY
{
	BOOL		m_bMoveObj;
	float		m_fGBeginTime;
	float		m_fGLifeTime;
	SMovSound	m_MovSound;	//	����	
	D3DXMATRIX	m_matLocal;

	DWORD	GetSizeBase ();

	EFF_PROPERTY *m_pChild;
	EFF_PROPERTY *m_pSibling;

	virtual DWORD GetTypeID () = 0;
	virtual const char* GetName () = 0;

	virtual DxEffSingle* NEWOBJ () = 0;

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	(pChild,pSibling) ��� ȣ���.
	virtual HRESULT SaveFile ( CSerialFile &SFile );								//	(pChild,pSibling) ��� ȣ���.

	void	CloneEffProp	( LPDIRECT3DDEVICEQ pd3dDevice, EFF_PROPERTY* pSrcProp );

public:
	EFF_PROPERTY () :
		m_bMoveObj(FALSE),
		m_fGBeginTime(0.0f),
		m_fGLifeTime(4.0f),
		m_pChild(NULL),
		m_pSibling(NULL)
	{
		D3DXMatrixIdentity( &m_matLocal );
	}

	virtual ~EFF_PROPERTY()
	{
		CleanUp ();

		SAFE_DELETE(m_pChild);
		SAFE_DELETE(m_pSibling);
	}

	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

	virtual HRESULT Create(LPDIRECT3DDEVICEQ pd3dDevice)
	{
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )
			return E_FAIL;

		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )
			return E_FAIL;

		return S_OK;
	}

	virtual HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();

		DeleteDeviceObjects();

		return S_OK;
	}

	//	Note : ���� ���.
	//
	static EFF_PROPERTY* NEW_PROP ( DWORD TypeID );
};
typedef EFF_PROPERTY* PEFF_PROPERTY;


//	Note : ����Ʈ�� �������� ������ �ִ� "EFF_PROPERTY" ���� Ʈ�� ������ ������ ����
//			�̰��� �ϳ��� ���� ������ SAVE/LOAS �Ǹ� ����Ʈ ��ü�� �����ÿ� �ϳ��� ������ ������
//
struct EFF_PROPGROUP
{
public:
	const static DWORD	VERSION;
	const static DWORD	FILEVERSION;

public:
	std::string			m_strFileName;
	DWORD				m_dwFlag;
	D3DXVECTOR3			m_vLocal;
	D3DXMATRIX			m_matWorld;
	EFF_PROPERTY*		m_pPropRoot;

public:
	D3DXVECTOR3 m_vMax;			//	AABB�� ���.
	D3DXVECTOR3 m_vMin;			//	AABB�� ���.

public:
	HRESULT DeleteProp ( PEFF_PROPERTY &pPropHead, PEFF_PROPERTY pProp );
	HRESULT GetMatCombParent ( PEFF_PROPERTY &pPropHead, PEFF_PROPERTY pProp, D3DXMATRIX &matParent, D3DXMATRIX &matComb );

public:
	HRESULT LoadFile ( const char* szFileName, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT SaveFile ( const char* szFileName );

	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

	HRESULT Create(LPDIRECT3DDEVICEQ pd3dDevice)
	{
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )
			return E_FAIL;

		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )
			return E_FAIL;

		return S_OK;
	}

	HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();

		DeleteDeviceObjects();

		return S_OK;
	}

	EFF_PROPGROUP () :
		m_pPropRoot(NULL),
		m_vMax(15,30,15),
		m_vMin(-15,0,-15),
		m_vLocal(0.f,0.f,0.f)
	{
		m_dwFlag = 0L;
		m_dwFlag |= EFF_CFG_NONE;
		m_dwFlag |= EFF_CFG_AVERAGE;
		m_dwFlag |= EFF_CFG_AUTOAABBBOX;
		
		D3DXMatrixIdentity( &m_matWorld );
	}

	~EFF_PROPGROUP ()
	{
		CleanUp ();

		SAFE_DELETE(m_pPropRoot);
	}

public:
	BOOL RESET_MOVEOBJ_PROP ( PEFF_PROPERTY &pPropHead, EFF_PROPERTY &pCurEffProp );

protected:
	BOOL NEWEFFSINGLE ( PEFF_PROPERTY &pCurEffProp, PDXEFFSINGLE &pCurEffSingle, DxEffSingleGroup* pThisGroup );

	//	Note : ���� ����Ʈ�� ȭ�鿡 ������ �ϴ� �޼ҵ�. ��� ����Ʈ ������ REND ����.
	BOOL NEWEFFSINGLE ( PEFF_PROPERTY &pCurEffProp, PDXEFFSINGLE &pCurEffSingle, DxEffSingleGroup* pThisGroup, EFF_PROPERTY* pTargetProp );

public:
	DxEffSingleGroup* NEWEFFGROUP ();
	
	//	Note : ���� ����Ʈ�� ȭ�鿡 ������ �ϴ� �޼ҵ�. ��� ����Ʈ ������ REND ����.
	DxEffSingleGroup* NEWEFFGROUP ( EFF_PROPERTY* pTargetProp );

	//	Note : ���� ����Ʈ�� ȭ�鿡 ������ �ϴ� �޼ҵ�.
	DxEffSingleGroup* NEWEFFSINGLE ( EFF_PROPERTY* pTargetProp );
};


//	Note : �̱��� �������� �Ǿ� �ִ�. (���� ��ü�� �����Ѵ�.)
//			����Ʈ �׷��� ���Ͽ��� �о ����Ʈ�� �����ϸ� �� ���� ����Ʈ �׷���� �޸𸮿� �ø��� ������ ���� �����Ѵ�.
//			���ϴ� ����Ʈ �׷��� ã�Ƽ� �̸� ���ϴ� ���� ������ �ش�.
//
class DxEffSinglePropGMan
{
public:
	typedef std::map<std::string,EFF_PROPGROUP*>			MAPPROPGROUP;
	typedef MAPPROPGROUP::iterator							MAPPROPGROUP_ITER;

protected:
	char m_szPath[MAX_PATH];

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	MAPPROPGROUP		m_mapPROPGROUP;

public:
	void SetPath ( const char *szPath )
	{
		StringCchCopy( m_szPath, MAX_PATH, szPath );
	}

	const char* GetPath ()	{ return m_szPath; }

public:
	EFF_PROPGROUP* FindEffGProp ( const char* szFileName );
	EFF_PROPGROUP* LoadEffectGProp ( const char* szFileName );
	HRESULT DeleteEffectGProp ( const char* szFileName );

public:
	HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();

		DeleteDeviceObjects();

		return S_OK;
	}

public:
	HRESULT OneTimeSceneInit ( const char *szPath );
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

protected:
	DxEffSinglePropGMan (void);

public:
	~DxEffSinglePropGMan (void);

public:
	static DxEffSinglePropGMan& GetInstance();
};
