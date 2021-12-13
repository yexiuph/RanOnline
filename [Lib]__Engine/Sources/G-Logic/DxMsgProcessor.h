#pragma once

#include "./CList.h"

class DxMsgProcessor
{
public:
	virtual void MsgProcess ( NET_MSG_GENERIC* nmg ) = 0;
};
