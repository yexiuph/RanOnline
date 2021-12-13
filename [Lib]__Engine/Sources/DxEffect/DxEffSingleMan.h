#pragma once

#include "./DxEffectSingleType.h"

#include "./DxLandMan.h"

struct DxEffSingleType
{
	DWORD	TYPEID;
	char	NAME[MAX_PATH];

	DxEffSingleType *pNext;

	DxEffSingleType() :
		TYPEID(0xffffffff),
		pNext(NULL)
	{
		memset( NAME, 0, sizeof(char)*MAX_PATH );
	}

	~DxEffSingleType ()
	{
		SAFE_DELETE(pNext);
	}
};

//	Note : 싱글콘 패턴으로 되어 있다. (당일 개체로 존재한다. )
//			이펙트 종류를 등록해서 등록된 이펙트들의 이름과, TYPE_ID를 리스트로 관리함
//			이펙트의 정적 자원들을 로드하고 해제한다. ( static 멤버 함수르르 호출하여 이소스를 읽어서 
//			초기화 하여 static멤버 변수에 저장해두며, 해제 함수를 호출하여 리소스를 지운다. )
//
class DxEffSingleMan
{
protected:
	void RegistType ( const DWORD TypeID, const char *Name );

protected:
	DxEffSingleType	*pEffectTypeList;

public:
	DxEffSingleType* GetEffectList ()	{ return pEffectTypeList; }

protected:
	DxLandMan*	m_pLandMan;

public:
	void SetLandMan ( DxLandMan* pLandMan )		{ m_pLandMan = pLandMan; }
	DxLandMan* GetLandMan ()					{ return m_pLandMan; }

public:
	HRESULT OneTimeSceneInit();
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT FinalCleanup();

protected:
	DxEffSingleMan(void);

public:
	~DxEffSingleMan(void);

public:
	static DxEffSingleMan& GetInstance();
};

//DxEffSingleMan::GetInstance().GetLandMan()->GetOctree();