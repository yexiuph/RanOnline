#pragma once

namespace rol_clipboard
{
	bool InitClipboard ();

	HANDLE PutClipboard ( HWND hwnd );

	bool RolHeadCheck ( HWND hwnd );
};