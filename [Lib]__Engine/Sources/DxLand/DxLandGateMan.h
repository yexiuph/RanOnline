#pragma once
#include "./DxCustomTypes.h"
#include "./Collision.h"
#include "./GLDefine.h"

class CSerialFile;
class basestream;
class NavigationMesh;
class DxLandGate : public OBJAABB
{
public:
	enum
	{
		VERSION		= 0x0100,
		MAX_SZNAME	= 65,

		GATE_IN		= 0x0001,
		GATE_OUT	= 0x0002,

		GEN_NULL	= -1,

		GATE_ALL	= (GATE_IN|GATE_OUT),
	};

	enum EMGENPOS_SEL
	{
		GEN_FIRST	= 1,
		GEN_RENDUM	= 2,
	};

	struct PROPERTY
	{
		char			m_szName[MAX_SZNAME];
		DWORD			m_dwGeteFlags;
		DWORD			m_GateID;

		SNATIVEID		m_ToMapID;
		DWORD			m_ToGateID;
		D3DXVECTOR2		m_vDiv;

		D3DXVECTOR3		m_vMax;
		D3DXVECTOR3		m_vMin;

		DWORD			m_dwStartPNum;

		PROPERTY()
		{
			memset( m_szName, 0, sizeof(char)*MAX_SZNAME );
		}
	};

protected:
	union
	{
		struct
		{
			PROPERTY		m_Prop;
		};
		struct
		{
			char			m_szName[MAX_SZNAME];
			DWORD			m_dwGeteFlags;
			DWORD			m_GateID;

			SNATIVEID		m_ToMapID;
			DWORD			m_ToGateID;
			D3DXVECTOR2		m_vDiv;

			D3DXVECTOR3		m_vMax;
			D3DXVECTOR3		m_vMin;

			DWORD			m_dwStartPNum;
		};
	};

public:
	LPD3DXVECTOR3	m_pvStartPos;

public:
	DWORD			m_dwLastUsedPos;

public:
	void SetName ( const char* szName )	{ StringCchCopy( m_szName, MAX_SZNAME, szName ); }
	void SetGate ( DWORD dwFlags, DWORD GateID, SNATIVEID ToMapID, DWORD ToGateID, const D3DXVECTOR2 &vDiv );

	void SetGateID ( DWORD GateID )		{ m_GateID = GateID; }
	void SetPos ( const D3DXVECTOR3 &vPos );
	void SetBox ( const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin );
	BOOL MakeStartPos ( NavigationMesh* pNaviMesh );

	char* GetName ()						{ return m_szName; }
	DWORD GetFlags ()						{ return m_dwGeteFlags; }
	DWORD GetGateID ()						{ return m_GateID; }
	SNATIVEID GetToMapID ()					{ return m_ToMapID; }
	DWORD GetToGateID ()					{ return m_ToGateID; }
	D3DXVECTOR2 GetDiv ()					{ return m_vDiv; }
	DWORD GetStartPNum ()					{ return m_dwStartPNum; }
	LPD3DXVECTOR3 GetStartPos ()			{ return m_pvStartPos; }

public:
	DxLandGate*		m_pNext;

	//	Note : OBJAABB
	//
public:
	void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		vMax = m_vMax; vMin = m_vMin;
	}

	D3DXVECTOR3 GetMax ()	{ return m_vMax; }
	D3DXVECTOR3 GetMin ()	{ return m_vMin; }

	BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, vPos );
	}

	BOOL IsDetectDivision ( D3DXVECTOR3 vPos )
	{
		return COLLISION::IsWithInPoint ( m_vMax, m_vMin, vPos );
	}

	OBJAABB* GetNext ()		{ return m_pNext; }

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxLandGate () :
		m_dwGeteFlags(GATE_ALL),
		m_GateID(0),
		
		m_ToGateID(0),
		m_vDiv(12.0f,12.0f),
		m_vMax(40,40,40),
		m_vMin(0,0,0),

		m_dwStartPNum(0),
		m_pvStartPos(NULL),

		m_dwLastUsedPos(0),

		m_pNext(NULL)
	{
		memset( m_szName, 0, sizeof(char)*MAX_SZNAME );
	}
	~DxLandGate ()
	{
		SAFE_DELETE_ARRAY(m_pvStartPos);

		SAFE_DELETE(m_pNext);
	}

public:
	D3DXVECTOR3 GetGenPos ( DxLandGate::EMGENPOS_SEL emGenSel );
	void SetObjRotate90();

public:
	HRESULT Load ( basestream	&SFile );
	HRESULT Save ( CSerialFile	&SFile );

public:
	void operator= ( DxLandGate &LandGate );
};
typedef DxLandGate* PDXLANDGATE;

class DxLandGateMan
{
public:
	static BOOL		m_bREND;

protected:
	DWORD			m_dwNumLandGate;
	PDXLANDGATE		m_pLandGateList;
	OBJAABBNode*	m_pLandGateTree;

public:
	void AddLandGate ( PDXLANDGATE pLandGate );
	void DelLandGate ( char* szName );

public:
	PDXLANDGATE FindLandGate ( const char* szName );
	PDXLANDGATE FindLandGate ( DWORD dwGateID );
	void BuildTree ();
	void SetObjRotate90();

public:
	PDXLANDGATE DetectGate ( const D3DXVECTOR3 &vPos );

public:
	PDXLANDGATE GetListLandGate ()		{ return m_pLandGateList; }
	DWORD		GetNumLandGate	()		{ return m_dwNumLandGate; }

public:
	HRESULT Load ( basestream	&SFile );
	HRESULT Save ( CSerialFile	&SFile );

	static VOID SaveCsvHead( std::fstream &SFile );
	VOID SaveCsv( std::fstream &SFile );
	VOID LoadCsv( CStringArray &StrArray );

public:
	void CleanUp ();

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxLandGateMan(void);
	~DxLandGateMan(void);

public:
	DxLandGateMan& operator= ( DxLandGateMan &LandGateMan );
};




