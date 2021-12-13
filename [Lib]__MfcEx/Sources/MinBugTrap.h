#ifndef _MIN_BUG_TRAP_H_
#define _MIN_BUG_TRAP_H_

#pragma once

#include <string>

namespace BUG_TRAP
{
	//! BugTrap �� Exception handler �� ��ġ�Ѵ�.
	//! strAppName : Application Name
	//! bUseEmail : e-mail �������� ���� ���ΰ�?
	//! bUserScreenCapture : Screen Capture �� ÷���ؼ� ���� ���ΰ�?
	void BugTrapInstall(
			std::string& strAppName,
			bool bUseEmail = true,
			bool bUseScreenCapture = true );


}; // End of namespace CORE_COMMON

#endif // _MIN_BUG_TRAP_H_