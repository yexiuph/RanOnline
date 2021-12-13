#ifndef _MIN_BUG_TRAP_H_
#define _MIN_BUG_TRAP_H_

#pragma once

#include <string>

namespace BUG_TRAP
{
	//! BugTrap 용 Exception handler 를 설치한다.
	//! strAppName : Application Name
	//! bUseEmail : e-mail 리포팅을 받을 것인가?
	//! bUserScreenCapture : Screen Capture 를 첨부해서 받을 것인가?
	void BugTrapInstall(
			std::string& strAppName,
			bool bUseEmail = true,
			bool bUseScreenCapture = true );


}; // End of namespace CORE_COMMON

#endif // _MIN_BUG_TRAP_H_