#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// reference additional headers your program requires here
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include <afx.h>
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.

//	dx 관련과 기타 필수 인클루드 및 설정.
#include "dxstdafx.h"

// Standard
#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>
//#include <strstream>
#include <vector>
#include <map>
#include <set>
#include <list>

// MS
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <wincrypt.h>
#include <rtcapi.h>
#include <imm.h>
#include <io.h>
#include <malloc.h>

#include <functional>