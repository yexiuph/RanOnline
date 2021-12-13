#include "pch.h"
#include "minlzo.h"

/* portability layer */
#define WANT_LZO_MALLOC 1
#include "portab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMinLzo* CMinLzo::SelfInstance = NULL;


CMinLzo& CMinLzo::GetInstance()
{
	static CMinLzo Instance;
	return Instance;
}
/*
CMinLzo* CMinLzo::GetInstance()
{
	if (SelfInstance == NULL)
	{
		SelfInstance = new CMinLzo();
	}
	return SelfInstance;
}

void CMinLzo::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		SAFE_DELETE(SelfInstance);
	}
}
*/
CMinLzo::CMinLzo(void)
	: m_pWorkmem(NULL)
	, m_bInit(false)
{
	::InitializeCriticalSection(&m_CriticalSection);
}

CMinLzo::~CMinLzo(void)
{
	if (m_pWorkmem != NULL)
	{
		lzo_free(m_pWorkmem);
		m_pWorkmem = NULL;
	}
	::DeleteCriticalSection(&m_CriticalSection);
}

int CMinLzo::init()
{
	if (m_bInit == true)
	{
		return MINLZO_SUCCESS;
	}

	if (lzo_init() != LZO_E_OK)
	{
		m_strError = "internal error - lzo_init() failed ! \n (this usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable `-DLZO_DEBUG' for diagnostics)";
		m_bInit = false;
		return MINLZO_ERROR;
	}
	else
	{
		// Allocate blocks and the work-memory
		// pIn = (lzo_bytep) lzo_malloc(IN_LEN);
		// pOut = (lzo_bytep) lzo_malloc(OUT_LEN);
		if (m_pWorkmem != NULL)
		{
			lzo_free(m_pWorkmem);
			m_pWorkmem = NULL;
		}
		m_pWorkmem = (lzo_bytep) lzo_malloc(LZO1X_1_MEM_COMPRESS);

		// if (pIn == NULL || pOut == NULL || m_pWorkmem == NULL)
		if (m_pWorkmem == NULL)
		{
			m_strError = "out of memory";
			m_bInit = false;
			return MINLZO_ERROR;
		}
		else
		{
			m_bInit = true;
			return MINLZO_SUCCESS;
		}
	}
}

int CMinLzo::lzoCompress (byte* pInBuffer, 
						  int nInLength, 
						  byte* pOutBuffer, 
						  int &nOutLength)
{
	if (pInBuffer == NULL ||
		nInLength == 0 ||
		pOutBuffer == NULL)
	{
		m_strError = "lzoCompress input data error";
		return MINLZO_INPUT_DATA_ERROR;
	}

	int nReturn = 0;
	
	::EnterCriticalSection(&m_CriticalSection);
	// compress from `in' to `out' with LZO1X-1	
	nReturn = lzo1x_1_compress(pInBuffer,
						nInLength,
						pOutBuffer,
						(lzo_uint*) &nOutLength,
						m_pWorkmem);
					
	if (nReturn == LZO_E_OK)
	{
		if (nOutLength >= nInLength)
		{
			m_strError = "lzoCompress can not compress";			
			::LeaveCriticalSection(&m_CriticalSection);
			return MINLZO_CAN_NOT_COMPRESS;
		}
		else
		{			
			::LeaveCriticalSection(&m_CriticalSection);
			return MINLZO_SUCCESS;
		}
	}
	else
	{
		m_strError = "lzoCompress internal error";		
		::LeaveCriticalSection(&m_CriticalSection);
		return MINLZO_INTERNAL_ERROR;
	}	
}

int CMinLzo::lzoDeCompress (byte* pOutBuffer, 
						    int nOutLength, 
						    byte* pInBuffer, 
						    int &nNewLength)
{
	if (pOutBuffer == NULL ||
		nOutLength == 0 ||
		pInBuffer == NULL)
	{
		m_strError = "lzoDeCompress input data error";
		return MINLZO_INPUT_DATA_ERROR;
	}

	::EnterCriticalSection(&m_CriticalSection);	
	int nReturn = 0;
	// decompress, now going from `out' to `in'	
	nReturn = lzo1x_decompress(pOutBuffer, 
							nOutLength,
							pInBuffer,
							(lzo_uint*) &nNewLength,
							NULL);

	if (nReturn == LZO_E_OK && nNewLength >= nOutLength)
	{		
		::LeaveCriticalSection(&m_CriticalSection);
		return MINLZO_SUCCESS;
	}
	else
	{
		m_strError = "lzoDeCompress can not decompress";		
		::LeaveCriticalSection(&m_CriticalSection);
        return MINLZO_ERROR;
	}
}

std::string& CMinLzo::getErrorString()
{
	return m_strError;
}