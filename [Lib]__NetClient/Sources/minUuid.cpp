#include "pch.h"
#include <strsafe.h>
#include "minUuid.h"

namespace SERVER_UTIL
{
	
int getUUID(TCHAR* pszUUID, size_t cchUUID)
{
	if (pszUUID == NULL) return UUID_ERROR;

	UUID id;
	if (RPC_S_OK == ::UuidCreate(&id))
	{
		TCHAR* tmpid = NULL;
#ifdef UNICODE
		RPC_STATUS rpcResult = ::UuidToString(&id, reinterpret_cast<unsigned short**>(&tmpid));
		if (RPC_S_OUT_OF_MEMORY != rpcResult)
		{
			StringCchCopy(pszUUID, cchUUID, tmpid);
			::RpcStringFree(reinterpret_cast<unsigned short**>(&tmpid));
			return UUID_SUCCESS;
		}
#else
		RPC_STATUS rpcResult = ::UuidToString(&id, reinterpret_cast<unsigned char**>(&tmpid));
		if (RPC_S_OUT_OF_MEMORY != rpcResult)
		{
			StringCchCopy(pszUUID, cchUUID, tmpid);
			::RpcStringFree(reinterpret_cast<unsigned char**>(&tmpid));
			return UUID_SUCCESS;
		}
#endif // UNICODE
	}	
	return UUID_ERROR;
}

int getUUID(UUID &_uuid)
{
	if (RPC_S_OK == ::UuidCreate(&_uuid)) {
		return UUID_SUCCESS;
	}
	else {
		return UUID_ERROR;
	}
}

int uuidToString(UUID &_uuid, TCHAR* pszUUID)
{
	if (NULL == pszUUID) return UUID_ERROR;	

	TCHAR* tmpid = NULL;
#ifdef UNICODE
	RPC_STATUS rpcResult = ::UuidToString(&_uuid, reinterpret_cast<unsigned short**>(&tmpid));
	if (RPC_S_OUT_OF_MEMORY != rpcResult)
	{
		StringCchCopy(pszUUID, UUID_STR_LENGTH, tmpid);
		::RpcStringFree(reinterpret_cast<unsigned short**>(&tmpid));
		return UUID_SUCCESS;
	}
#else
	RPC_STATUS rpcResult = ::UuidToString(&_uuid, reinterpret_cast<unsigned char**>(&tmpid));
	if (RPC_S_OUT_OF_MEMORY != rpcResult)
	{
		StringCchCopy(pszUUID, UUID_STR_LENGTH, tmpid);
		::RpcStringFree(reinterpret_cast<unsigned char**>(&tmpid));
		return UUID_SUCCESS;
	}
#endif // UNICODE
	return UUID_ERROR;
}

int stringToUUID(TCHAR* pszUUID, UUID &_uuid)
{
#ifdef UNICODE
	if (RPC_S_OK == ::UuidFromString(reinterpret_cast<unsigned short*>(pszUUID), &_uuid))
	{
		return UUID_SUCCESS;
	}
#else
	if (RPC_S_OK == ::UuidFromString(reinterpret_cast<unsigned char*>(pszUUID), &_uuid))
	{
		return UUID_SUCCESS;
	}
#endif
	return UUID_ERROR;	
}

} // End of namesapce uuid 