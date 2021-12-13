#include "pch.h"
#include "s_COverlapped.h"
#include "s_CConsoleMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COverlapped::COverlapped(int nStartSize)
{
	int i = 0;	
	for (i=0; i<nStartSize; ++i)
	{
		m_UnUse.AddTail();
	}
}

COverlapped::~COverlapped()
{	
	
}

LPPER_IO_OPERATION_DATA COverlapped::GetFreeOverIO(int nType)
{	
	LPPER_IO_OPERATION_DATA pTemp = NULL;
	int nSize = 0;
	int i     = 0;
	
	LockOn();	

	pTemp = (LPPER_IO_OPERATION_DATA) m_UnUse.Get();
	
	if (pTemp == NULL) 
	{
		LockOff();
		return NULL;
	}
	else
	{
		// pTemp->Reset();
		// ZeroMemory(&pTemp->Overlapped, sizeof(OVERLAPPED));
		pTemp->OperationType = nType;
		pTemp->dwRcvBytes    = 0;
		pTemp->dwSndBytes    = 0;
		pTemp->dwTotalBytes  = 0;
		m_Use.AddTail((MEM_POOLER::NODE<PER_IO_OPERATION_DATA>*) pTemp);
		LockOff();
		return pTemp;
	}
}

void COverlapped::Release(LPPER_IO_OPERATION_DATA pElement)
{
	LockOn();
	if (pElement)
	{
		m_Use.Release  ((MEM_POOLER::NODE<PER_IO_OPERATION_DATA>*) pElement);
		m_UnUse.AddTail((MEM_POOLER::NODE<PER_IO_OPERATION_DATA>*) pElement);
	}
	LockOff();
}

int COverlapped::GetUseCount(void)
{
	return m_Use.GetSize();
}

int COverlapped::GetUnUseCount(void)
{
	return m_UnUse.GetSize();
}