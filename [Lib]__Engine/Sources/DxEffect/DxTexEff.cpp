#include "pch.h"

#include "./DxTexEff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxTexEffBase::DxTexEffBase() :
	m_pNext(NULL)
{
}

DxTexEffBase::~DxTexEffBase()
{
	SAFE_DELETE( m_pNext );
}