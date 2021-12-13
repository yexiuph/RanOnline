// #include "GLCrowTracer.h"
#pragma once
#include "GLDefine.h"

inline D3DXVECTOR3 VERROR()	{ return D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX); }

class DxSkinChar;
class GLCrowTracer
{
public:
	virtual BOOL IsVisibleCV ( const STARGETID & sTargetID ) = 0;
	virtual D3DXVECTOR3 GetTargetPos ( const STARGETID &sTargetID ) = 0;
	virtual DxSkinChar* GetSkinChar ( const STARGETID &sTargetID ) = 0;
	virtual float GetCrowDir ( const STARGETID &sTargetID ) = 0;
};
