#include "pch.h"
#include "./UITextControl.h"

namespace	NS_UITEXTCONTROL
{
	DWORD	GetEvaluateColor ( BOOL bValue )
	{
		if ( bValue )	return ENABLE;
		else			return DISABLE;
	}
};