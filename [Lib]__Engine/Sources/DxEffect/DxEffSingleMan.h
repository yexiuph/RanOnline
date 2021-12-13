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

//	Note : �̱��� �������� �Ǿ� �ִ�. (���� ��ü�� �����Ѵ�. )
//			����Ʈ ������ ����ؼ� ��ϵ� ����Ʈ���� �̸���, TYPE_ID�� ����Ʈ�� ������
//			����Ʈ�� ���� �ڿ����� �ε��ϰ� �����Ѵ�. ( static ��� �Լ����� ȣ���Ͽ� �̼ҽ��� �о 
//			�ʱ�ȭ �Ͽ� static��� ������ �����صθ�, ���� �Լ��� ȣ���Ͽ� ���ҽ��� �����. )
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