//
//	[Bone Collector], JDH
//
//	(2002.12.02),JDH, DxBoneTrans::ResetBone () 메소드 추가. - 에니메이션 키중에 빠진
//			키가 있을 경우 Render 마다 키가 리셋되지 않고 앳날 대이터가 남아있어서
//			문제 발생. 이를 해결하기 위한 것.
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

#define	USE_ANI_ADD			0x0001	// 애니메이션 수치 추가.
#define	USE_ANI_BLENDING	0x0002	// 애니메이션 블렌딩

enum EMBODY
{
	EMBODY_DEFAULT	= 0,	// 전체. 이것이 셋팅되면 다 렌더다. - 일반적인 셋팅.
	EMBODY_UPBODY	= 1,	// 상체								- 굳이 상체를 움직이고 싶을 때만 셋팅한다.
	EMBODY_DOWNBODY	= 2,	// 하체								- 하체를 셋팅하는 것은 없다. < 있다면 많은 수정을 해야함 >
};

struct DxBoneTrans
{
	char*			szName;					// 본 이름.
	EMBODY			m_emBody;				// 신체의 위치
	D3DXMATRIXA16	matRot, matRotOrig;		// 트랜스폼 메트릭스.
	D3DXMATRIXA16	matCombined;

	DWORD			m_dwFlag;
	float			m_fScale;			// 신체부위를 스케일 한다.
	SQuatPos		m_pQuatPosORIG;		// 쿼터니언 값
	SQuatPos		m_pQuatPosCUR;		// 쿼터니언 값
	SQuatPos		m_pQuatPosPREV;
	D3DXQUATERNION	m_vQuatMIX;			// 타격시 믹싱 된다.

	DxBoneTrans		*pBoneTransSibling;		//	형제 노드들.
	DxBoneTrans		*pBoneTransFirstChild;	//	자식 노드들.

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
