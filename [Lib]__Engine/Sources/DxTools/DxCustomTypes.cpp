#include "pch.h"

#include "./DxCustomTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD	OCTREEVERTEX::FVF = (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1);
const DWORD	DIFFUSEVERTEX::FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE);