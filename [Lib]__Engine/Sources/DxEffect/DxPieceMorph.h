#pragma once

#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
//									D	x		P	i	e	c	e		M	o	r	p	h
//----------------------------------------------------------------------------------------------------------------------
class DxPieceMorph
{
protected:
	static const DWORD VERSION;

	typedef std::vector<std::string>	VECXFILE;
	typedef VECXFILE::iterator			VECXFILE_ITER;
	VECXFILE	m_vecXFile;

public:
	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );

public:
	DxPieceMorph();
	~DxPieceMorph();
};