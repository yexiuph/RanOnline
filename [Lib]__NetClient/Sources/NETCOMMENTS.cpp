#include "pch.h"
#include "netcomments.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace NETCOMMENTS
{
	const char* NET_MSG_CHA_NEW_OK = "캐릭터 생성에 성공하였습니다.";
	const char* NET_MSG_CHA_NEW_DUP = "같은 이름의 캐릭터가 이미 존재합니다.";
	const char* NET_MSG_CHA_NEW_MAX = "더이상 캐릭터를 만들수 없습니다.";
	const char* NET_MSG_CHA_NEW_ERROR = "캐릭터 생성에 실패하였습니다.";
};
