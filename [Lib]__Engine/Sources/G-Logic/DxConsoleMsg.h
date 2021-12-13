#ifndef DXCONSOLEMSG_H_
#define DXCONSOLEMSG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DxConsoleMsg
{
public:
	virtual void Write( const TCHAR* msg, ... ) = 0;
	virtual void Write( int nType, const TCHAR* msg, ... ) = 0;	
};

#endif // DXCONSOLEMSG_H_