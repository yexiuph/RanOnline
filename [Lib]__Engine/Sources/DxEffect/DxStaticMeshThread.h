// DxStaticMeshCreate.h: interface for the CCollision class.
//
//	class DxStaticMesh_THREAD : ������ Thread Loading �κ� ���� �̰����� �Ű�����.
//								DxStaticMesh�� Normal�� �ƴ� Diffuse�� ���� �����ϰ� �Ǿ� Diffuse ����� Thread�� �ϰԵ�.
//
//	Name : Sung-Hwan Han
//	Begin :2005/10/12
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "SerialFile.h"
#include "DxCustomTypes.h"

class DxStaticMesh_THREAD
{
protected:
	DWORD	dwThreadID;			//	������ ID
	HANDLE	hThread;			//	������ �ڵ�
	HANDLE	hEventTerminate;	//	������ ����

protected:
	struct DATA
	{
		LPDIRECT3DDEVICEQ	pd3dDevice;
		BOOL				bEnable;
		BOOL				bDelete;
		CSerialFile*		pSFile;
		LOADINGDATALIST*	plistLoadingData;
		CRITICAL_SECTION*	pCSLockLoading;
		DATA() :
			bEnable(FALSE),
			bDelete(FALSE),
			pd3dDevice(NULL),
			pSFile(NULL),
			plistLoadingData(NULL),
			pCSLockLoading(NULL)
		{
		};
	};

	DATA	m_sData;

public:
	BOOL	StartThread ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile* pSFile, LOADINGDATALIST* plistLoadingData, CRITICAL_SECTION* pCSLockLoading );
	void	EndThread ();

protected:
	static unsigned int WINAPI LoadThread( LPVOID pData );

public:
	DxStaticMesh_THREAD() :
		dwThreadID ( UINT_MAX ),
		hThread ( NULL ),
		hEventTerminate ( NULL )
	{
	};
	~DxStaticMesh_THREAD();
};

class DxStaticMeshColor_THREAD
{
protected:
	DWORD	dwThreadID;			//	������ ID
	HANDLE	hThread;			//	������ �ڵ�
	HANDLE	hEventTerminate;	//	������ ����

protected:
	struct DATA
	{
		LPDIRECT3DDEVICEQ	pd3dDevice;
		BOOL				bEnable;
		BOOL				bDelete;
		LOADINGDATALIST*	plistLoadingData;
		CRITICAL_SECTION*	pCSLockVB;
		BOOL				bProgressInit;
		DATA() :
			bEnable(FALSE),
			bDelete(FALSE),
			pd3dDevice(NULL),
			plistLoadingData(NULL),
			pCSLockVB(NULL),
			bProgressInit(FALSE)
		{
		};
	};

	BOOL				m_bGetListData;		// List�� �� �Ǵ°� ?
	DATA				m_sData;
	CRITICAL_SECTION	m_CSLockColor;		// m_listColorData�� ��ȭ�� �ߺ����� �ʵ��� �ϱ� ����.

public:
	LOADINGDATALIST		m_listColorData;	// �ּ� ����Ʈ
	BOOL				m_bOctreeMode;		// Octree Mode�� ��츸 �۾��ϰڴ�.

public:
	void CheckListEmpty();		// List�� ��� �ִ��� üũ�Ѵ�. DxStaticMehs���� �տ��� �۾��ؾ� �Ѵ�.
	BOOL IsInsertListData()	{ return m_bGetListData; }	// List�� Insert�ص� �Ǵ°� ?
	void EnterCS()			{ EnterCriticalSection( &m_CSLockColor ); }
	void LeaveCS()			{ LeaveCriticalSection( &m_CSLockColor ); }

	void DeleteList();		// ��� �����͸� �ʱ�ȭ.

public:
	BOOL StartThread( LPDIRECT3DDEVICEQ pd3dDevice );
	void EndThread();

protected:
	static unsigned int WINAPI LoadThread( LPVOID pData );

public:
	DxStaticMeshColor_THREAD();
	~DxStaticMeshColor_THREAD();

public:
	static DxStaticMeshColor_THREAD& GetInstance();
};

