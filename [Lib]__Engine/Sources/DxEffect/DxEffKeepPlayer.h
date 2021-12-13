#pragma once

#include <map>

class DxEffKeepMain;
class DxEffKeepDataMain;

class DxEffKeepPlayer
{
public:
	typedef std::map<TSTRING,DxEffKeepMain*>	MAP_EFFKEEPMAIN;
	typedef MAP_EFFKEEPMAIN::iterator			MAP_EFFKEEPMAIN_ITER;

	MAP_EFFKEEPMAIN	m_mapEff;

public:
	void CreateEff( DxEffKeepDataMain* pEff );	// Eff ����.~!
	void DeleteEff( const TCHAR* pName );		// �Ⱦ� ��� Eff�� ���ش�.
	void Reset();

	void CleanUp();

public:
	void FrameMove( float fElapsedTime, SKINEFFDATA& sSKINEFFDATA );
	void Render( const LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffKeepPlayer();
	~DxEffKeepPlayer();
};