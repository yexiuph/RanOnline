//
//	[Bone Collector], JDH
//
//	(2002.12.02),JDH, DxBoneTrans::ResetBone () �޼ҵ� �߰�. - ���ϸ��̼� Ű�߿� ����
//			Ű�� ���� ��� Render ���� Ű�� ���µ��� �ʰ� �ܳ� �����Ͱ� �����־
//			���� �߻�. �̸� �ذ��ϱ� ���� ��.
//
#pragma once

#include <map>
#include <list>
#include <string>

#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE

// Must Install Platform SDK
// Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
#include "strsafe.h" // Safe string function


#include "DxAniKeys.h"

#ifndef CHR_ID_LENGTH
#define CHR_ID_LENGTH	33
#endif

#define	USE_ANI_ADD			0x0001	// �ִϸ��̼� ��ġ �߰�.
#define	USE_ANI_BLENDING	0x0002	// �ִϸ��̼� ����

enum EMBODY
{
	EMBODY_DEFAULT	= 0,	// ��ü. �̰��� ���õǸ� �� ������. - �Ϲ����� ����.
	EMBODY_UPBODY	= 1,	// ��ü								- ���� ��ü�� �����̰� ���� ���� �����Ѵ�.
	EMBODY_DOWNBODY	= 2,	// ��ü								- ��ü�� �����ϴ� ���� ����. < �ִٸ� ���� ������ �ؾ��� >
};

struct DxBoneTrans
{
	char*			szName;					// �� �̸�.
	EMBODY			m_emBody;				// ��ü�� ��ġ
	D3DXMATRIXA16	matRot, matRotOrig;		// Ʈ������ ��Ʈ����.
	D3DXMATRIXA16	matCombined;

	DWORD			m_dwFlag;
	float			m_fScale;			// ��ü������ ������ �Ѵ�.
	SQuatPos		m_pQuatPosORIG;		// ���ʹϾ� ��
	SQuatPos		m_pQuatPosCUR;		// ���ʹϾ� ��
	SQuatPos		m_pQuatPosPREV;
	D3DXQUATERNION	m_vQuatMIX;			// Ÿ�ݽ� �ͽ� �ȴ�.

	DxBoneTrans		*pBoneTransSibling;		//	���� ����.
	DxBoneTrans		*pBoneTransFirstChild;	//	�ڽ� ����.

	DxBoneTrans () :
		szName(NULL),
		m_emBody(EMBODY_DEFAULT),
		m_dwFlag(0L),
		m_fScale(1.f),
		m_vQuatMIX(0.f,0.f,0.f,1.f),
		pBoneTransSibling(NULL),
		pBoneTransFirstChild(NULL)
	{
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matRotOrig);

		D3DXMatrixIdentity(&matCombined);
	}
	
	~DxBoneTrans()
	{
		SAFE_DELETE_ARRAY(szName);
		SAFE_DELETE(pBoneTransSibling);
		SAFE_DELETE(pBoneTransFirstChild);
	}

	void AddBoneTrans ( DxBoneTrans *pBoneTrans )
	{
		if ( pBoneTransFirstChild == NULL )
		{
			pBoneTransFirstChild = pBoneTrans;
		}
		else
		{
			pBoneTrans->pBoneTransSibling = pBoneTransFirstChild->pBoneTransSibling;
			pBoneTransFirstChild->pBoneTransSibling = pBoneTrans;
		}
	}

	void ResetBone ();

	void SetName( char* _szname )
	{
		if ( _szname )
		{
			size_t nStrLen = strlen(_szname)+1;
			szName = new char[ nStrLen ];
			StringCchCopy( szName, nStrLen, _szname );
		}
		else
		{
			size_t nStrLen = strlen("[Unnamed]")+1;
			szName = new char[ nStrLen ];
			StringCchCopy( szName, nStrLen, "[Unnamed]" );
		}
	}

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}
};
typedef DxBoneTrans*	PDXBONETRANS;

struct DxSkeleton
{
	static	float	g_fWeight;

	typedef std::map<std::string,DxBoneTrans*>					BONETRANSMAP;
	typedef std::map<std::string,DxBoneTrans*>::iterator		BONETRANSMAP_ITER;
	typedef std::map<std::string,DxBoneTrans*>::const_iterator	BONETRANSMAP_CITER;

	char*			szXFileName;
	
	DxBoneTrans*	pBoneRoot;
	BONETRANSMAP	BoneMap;

	DWORD			dwRefCount;

	HRESULT LoadBoneTrans ( PDXBONETRANS &pBone, LPDIRECTXFILEDATA pxofobjCur, EMBODY emBody );
	HRESULT LoadFile ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice );

	DxBoneTrans* FindBone ( const char *szName ) const
	{
		BONETRANSMAP_CITER iter = BoneMap.find ( std::string(szName) );
		if ( BoneMap.end()!=iter )	return (*iter).second;

		return NULL;
	}

	void UpdateBones( DxBoneTrans *pBoneCur, const D3DXMATRIX &matCur );

	void CheckSphere( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );
	void EditLineSphere( LPDIRECT3DDEVICEQ pd3dDevice );
	void EditLineSphere( LPDIRECT3DDEVICEQ pd3dDevice, DxBoneTrans *pBoneCur );

	BOOL IsCollision( D3DXVECTOR3& vFrom, D3DXVECTOR3& vLook, std::string& strName, const float fLength );
	void CheckCollision( D3DXVECTOR3& vFrom, D3DXVECTOR3& vLook, std::string& strName, const float fLength, float fBestDis, DxBoneTrans *pBoneCur );

	DxSkeleton () :
		szXFileName(NULL),
		pBoneRoot(NULL),
		dwRefCount(0)
	{
	}
	~DxSkeleton ()
	{
		SAFE_DELETE_ARRAY(szXFileName);
		SAFE_DELETE(pBoneRoot);
	}
};

class DxBoneCollector
{
private:
	typedef std::map<std::string,DxSkeleton*>			BONEMAP;
	typedef std::map<std::string,DxSkeleton*>::iterator	BONEMAP_ITER;

	typedef std::list<DxSkeleton*>						BONELIST;
	typedef std::list<DxSkeleton*>::iterator			BONELIST_ITER;

protected:
	char		m_szPath[MAX_PATH];

public:
	char*		GetPath () { return m_szPath; }

protected:
	BONEMAP		m_MapBone;
	BONEMAP		m_PreLoadBoneMap;

public:
	DxSkeleton* Find ( const char* szFile );
	DxSkeleton* Load ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL PreLoad ( const char* szBoneList, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void OneTimeSceneInit ( char* szPath );

	void CleanUp ();
	HRESULT DoInterimClean ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DxBoneCollector(void);

public:
	~DxBoneCollector(void);

public:
	static DxBoneCollector& GetInstance();
};
