#pragma once

struct EFF_PROPERTY;
struct EFF_PROPGROUP;
class DxEffSingleGroup;

#include <list>
#include <map>
#include "GLDefine.h"

//	Note : "class DxEffSingleGroup"�ν��Ͻ����� (��, ���� �����ϰ� �ִ� "����Ʈ �׷�"��ü���� ���� ��Ƽ� �����ϰ�
//			�̸� ���۽�Ű�� ������ �Ѵ�. ( Update & Render ) ���� �̵��� �Ҹ���������� üũ���� �Ҹ�ǰ� �Ѵ�.
//
class DxEffGroupPlayer
{
protected:
	typedef std::multimap<STARGETID,DxEffSingleGroup*>		EFFSGMAP;
	typedef EFFSGMAP::iterator								EFFSGMAP_ITER;
	typedef std::pair<EFFSGMAP_ITER,EFFSGMAP_ITER>			EFFSGMAP_PAIR;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	float				m_fTimer;
	EFFSGMAP			m_mapESG;

protected:
	void	NewAfterEff ( DxEffSingleGroup* pEffSG );

public:
	DWORD	GetAmount ()				{ return DWORD(m_mapESG.size()); }
	
	void	SetTime ( float fTimer )	{ m_fTimer = fTimer; }
	float	GetTime ()					{ return m_fTimer; }

public:
	D3DXVECTOR3 GetTargetIDPos ( STARGETID *pTargetID );

public:
	HRESULT PassiveEffect ( const char* szFileName, const D3DXMATRIX &matTrans, const STARGETID &sTargetID );
	HRESULT DeletePassiveEffect ( const char* szFileName, const STARGETID &sTargetID );
	HRESULT DeletePassiveEffect ( const STARGETID &sTargetID );
	BOOL FindPassiveEffect ( const char* szFileName, const STARGETID &sTargetID );

public:
	DxEffSingleGroup* NewEffGroup ( const char* szFileName, const D3DXMATRIX &matTrans, const STARGETID* pTargetID=NULL );
	DxEffSingleGroup* NewEffGroup ( EFF_PROPGROUP* pPropGroup, const D3DXMATRIX &matTrans, const STARGETID* pTargetID=NULL );
	HRESULT NewEffBody ( const char* szFileName, const STARGETID* pTargetID, D3DXVECTOR3 *pDir );

	//	Note : ���� ����Ʈ�� ȭ�鿡 ������ �ϴ� �޼ҵ�. ��� ����Ʈ ������ REND ����.
	HRESULT NewEffGroup ( EFF_PROPGROUP* pPropGroup, EFF_PROPERTY* pTargetProp, const D3DXMATRIX &matTrans );

	////	Note : ���� ����Ʈ�� ȭ�鿡 ������ �ϴ� �޼ҵ�.
	//HRESULT NewEffSingle ( EFF_PROPERTY* pTargetProp, const D3DXMATRIX &matTrans );

	HRESULT RemoveAllEff ();
	HRESULT DeleteEff ( const char* szFile );
	HRESULT DeleteAllEff ( const char* szFile );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

protected:
	DxEffGroupPlayer (void);

public:
	~DxEffGroupPlayer (void);

public:
	static DxEffGroupPlayer& GetInstance();
};
