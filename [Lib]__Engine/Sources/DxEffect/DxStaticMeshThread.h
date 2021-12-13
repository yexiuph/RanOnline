// DxStaticMeshCreate.h: interface for the CCollision class.
//
//	class DxStaticMesh_THREAD : 기존의 Thread Loading 부분 또한 이곳으로 옮겨졌음.
//								DxStaticMesh가 Normal이 아닌 Diffuse로 색을 제어하게 되어 Diffuse 계산을 Thread로 하게됨.
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
	DWORD	dwThreadID;			//	스레드 ID
	HANDLE	hThread;			//	스레드 핸들
	HANDLE	hEventTerminate;	//	스레드 종료

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
	DWORD	dwThreadID;			//	스레드 ID
	HANDLE	hThread;			//	스레드 핸들
	HANDLE	hEventTerminate;	//	스레드 종료

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

	BOOL				m_bGetListData;		// List를 얻어도 되는가 ?
	DATA				m_sData;
	CRITICAL_SECTION	m_CSLockColor;		// m_listColorData의 변화가 중복되지 않도록 하기 위함.

public:
	LOADINGDATALIST		m_listColorData;	// 주소 리스트
	BOOL				m_bOctreeMode;		// Octree Mode일 경우만 작업하겠다.

public:
	void CheckListEmpty();		// List가 비워 있는지 체크한다. DxStaticMehs보다 앞에서 작업해야 한다.
	BOOL IsInsertListData()	{ return m_bGetListData; }	// List를 Insert해도 되는가 ?
	void EnterCS()			{ EnterCriticalSection( &m_CSLockColor ); }
	void LeaveCS()			{ LeaveCriticalSection( &m_CSLockColor ); }

	void DeleteList();		// 모든 데이터를 초기화.

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

