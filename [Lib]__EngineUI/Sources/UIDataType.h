#pragma	once

struct UIVERTEX
{
	float		x, y, z;
	float		rhw;
	D3DCOLOR	diffuse;
	float		tu1,tv1;

static	const	DWORD	D3DFVF_UIVERTEX = ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

	UIVERTEX () :
		x ( 0.0f ), y ( 0.0f ), z ( 1.0f ),
		rhw ( 1.0f ),
		tu1 ( 0.0f ), tv1 ( 0.0f ),
		diffuse ( D3DCOLOR_ARGB(255,255,255,255) )
	{
	}

	UIVERTEX ( float _x, float _y, float _z = 1.0f, float _rhw = 0.0f,
		float _tu1 = 0.0f, float _tv1 = 0.0f,
		D3DCOLOR _diffuse = D3DCOLOR_ARGB(255,255,255,255) ) :
		x ( _x ), y ( _y ), z ( _z ),
		rhw ( _rhw ) , tu1 ( _tu1 ), tv1( _tv1 ), diffuse ( _diffuse )
	{
	}

	void operator += ( const D3DXVECTOR2& vPos )
	{
		x += vPos.x;
		y += vPos.y;
	}
	void operator = ( const D3DXVECTOR2& vPos )
	{
		x = vPos.x;
		y = vPos.y;
	}

	void	SetPos ( const D3DXVECTOR2& vPos )
	{
		x = vPos.x;
		y = vPos.y;
	}

	D3DXVECTOR2	GetPos ()
	{
		return D3DXVECTOR2 ( x, y );
	}

	void SetTexturePos (float u, float v )
	{
		tu1 = u;
		tv1 = v;
	}

	D3DXVECTOR2 GetTexturePos ()
	{
		return D3DXVECTOR2 ( tu1, tv1 );
	}

	void SetDiffuse ( const D3DCOLOR& _diffuse )
	{
		diffuse=_diffuse;
	}

	const D3DCOLOR&	GetDiffuse () const
	{
		return diffuse;
	}
};

struct UIRECT
{
	float left, right, top, bottom;
	float sizeX, sizeY;

	UIRECT () :
		left ( 0.0f ), right ( 0.0f ), top ( 0.0f ), bottom ( 0.0f ),
		sizeX ( 0.0f ), sizeY ( 0.0f )
	{
	}

	UIRECT ( float _left, float _top, float _sizeX, float _sizeY )
	{
		left = _left;
		top = _top;
		sizeX = _sizeX;
		sizeY = _sizeY;
		//
		right = left + sizeX;
		bottom = top + sizeY;
	}

	const UIRECT& operator= ( const UIRECT &rcPos )
	{
		if( this == &rcPos )
			return *this;

		left = rcPos.left;
		top = rcPos.top;
		sizeX = rcPos.sizeX;
		sizeY = rcPos.sizeY;
		//
		right = left + sizeX;
		bottom = top + sizeY;
		
		return *this;
	}

	const UIRECT& operator= ( const D3DXVECTOR4 &vecPos )
	{
		left = vecPos.x;
		top = vecPos.y;
		sizeX = vecPos.z;
		sizeY = vecPos.w;

		//
		right = left + sizeX;
		bottom = top + sizeY;
		
		return *this;
	}

	UIRECT operator+ ( const UIRECT &rcPos )
	{
		UIRECT rcTemp;
		rcTemp.left = left + rcPos.left;
		rcTemp.top = top + rcPos.top;
		rcTemp.sizeX = sizeX + rcPos.sizeX;
		rcTemp.sizeY = sizeY + rcPos.sizeY;

		// MEMO : 좌표 계산에 문제가 있는것 같다. +1을 해주는게 맞을거 같다.
		// 하지만 지금 이 좌표대로 CFG가 설정되어있어서 변경 불가능
		rcTemp.right = right + rcPos.right;
		rcTemp.bottom = bottom + rcPos.bottom;

		return rcTemp;
	}

	BOOL operator == ( const UIRECT &rcPos )
	{
		if ( left != rcPos.left )	return FALSE; // MEMO
		if ( top  != rcPos.top  )	return FALSE;
		if ( sizeX!= rcPos.sizeX)	return FALSE;
		if ( sizeY!= rcPos.sizeY)	return FALSE;

		return TRUE;
	}

	void SetPosition( float _left, float _top, float _sizeX, float _sizeY )
	{
		left = _left;
		top = _top;
		sizeX = _sizeX;
		sizeY = _sizeY;

		// MEMO : 좌표 계산에 문제가 있는것 같다. -1을 해주는게 맞을거 같다.
		// 하지만 지금 이 좌표대로 CFG가 설정되어있어서 변경 불가능
		right = left + sizeX;
		bottom = top + sizeY;
	}
};

typedef	int UIGUID;

const UIGUID NO_ID = 0;
const UIGUID FORCE_ID = 10000;