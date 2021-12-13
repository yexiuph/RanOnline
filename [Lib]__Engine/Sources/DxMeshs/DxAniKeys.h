#ifndef DXANIKEYS_H_
#define DXANIKEYS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
//
//	Note : xFile 에니메이션 데이터의 구조체.
//
//
//
//
//

#define UNITANIKEY_PERSEC	(4800.0f)

// X File formation rotate key
struct SRotateKeyXFile
{
	DWORD			dwTime;
	DWORD			dwFloats;	
	float			w;
	float			x;
	float			y;
	float			z;
};

struct SScaleKeyXFile
{
	DWORD		dwTime; 
	DWORD		dwFloats;
	D3DXVECTOR3	vScale;
};

struct SPositionKeyXFile
{
	DWORD		dwTime;
	DWORD		dwFloats;	
	D3DXVECTOR3	vPos;	
};

struct SMatrixKeyXFile
{
	DWORD		dwTime;
	DWORD		dwFloats;	
	D3DXMATRIX	mat;	
};

// in memory versions
struct SRotateKey
{
	DWORD			dwTime;
	D3DXQUATERNION	quatRotate;	
};

struct SPositionKey
{
	DWORD		dwTime;
	D3DXVECTOR3	vPos;	
};

struct SScaleKey
{
	DWORD		dwTime;
	D3DXVECTOR3	vScale;	
};

struct SMatrixKey
{
	DWORD			dwTime;
	D3DXMATRIXA16	mat;

	SMatrixKey() :
		dwTime(0)
	{
		D3DXMatrixIdentity(&mat);
	}

	~SMatrixKey()
	{
	}

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	void* operator new[](size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete[](void* p)
	{
		_mm_free(p);
	}

	bool operator < ( const SMatrixKey &rvalue ) const
	{
		return dwTime < rvalue.dwTime;
	}
};

struct QUATCOMP
{
	DWORD dwFirst;	// x,y
	DWORD dwSecond;	// z,w

	QUATCOMP()	{}
	QUATCOMP( DWORD a, DWORD b ) :
		dwFirst(a),
		dwSecond(dwSecond)
	{
	}
};

struct SQuatPosKey
{
	DWORD			m_dwTime;
	D3DXVECTOR3		m_vScale;
	D3DXVECTOR3		m_vPos;
	QUATCOMP		m_vQuatComp;
//	D3DXQUATERNION	m_vQuat;
	

	SQuatPosKey() :
		m_dwTime(0L),
		m_vScale(0.f,0.f,0.f),
		m_vPos(0.f,0.f,0.f),
		m_vQuatComp(0,0)
//		m_vQuat(0.f,0.f,0.f,1.f)
	{
	}

	~SQuatPosKey()
	{
	}

	bool operator < ( const SQuatPosKey &rvalue ) const
	{
		return m_dwTime < rvalue.m_dwTime;
	}
};

struct SQuatPos
{
	D3DXVECTOR3		m_vScale;
	D3DXVECTOR3		m_vPos;
	D3DXQUATERNION	m_vQuat;

	SQuatPos() :
		m_vScale(1.f,1.f,1.f),
		m_vPos(0.f,0.f,0.f),
		m_vQuat(0.f,0.f,0.f,0.f)
	{
	}

	~SQuatPos()
	{
	}
};

enum EMANI_MAINTYPE
{
	AN_GUARD_N			= 0,	//	경계.
	AN_PLACID			= 1,	//	휴식.
	AN_WALK				= 2,	//	걷기.
	AN_RUN				= 3,	//	뛰기.
	AN_ATTACK			= 4,	//	공격.
	AN_SHOCK			= 5,	//	충격.
	AN_DIE				= 6,	//	죽음.

	AN_CREATE			= 7,	//	생성.
	AN_TALK				= 8,	//	대화.

	AN_SKILL			= 9,	//	스킬.
	AN_GESTURE			= 10,	//	몸짓.
	AN_SHOCK_MIX		= 11,	//	충격,mix용.

	AN_GUARD_L			= 12,	//	경계,SP부족.
	AN_CONFT_WIN		= 13,	//	대련,승리.
	AN_CONFT_LOSS		= 14,	//	대련,패배.

	AN_SPEC				= 15,	//	특수.

	//AN_NONE				= 16,
	//AN_TYPE_SIZE		= 16,

	AN_SKILL_B			= 16,	//	스킬.
	AN_SKILL_C			= 17,	//	스킬.
	AN_SKILL_D			= 18,	//	스킬.
	AN_SKILL_E			= 19,	//	스킬. - 극강부에 들어가는 스킬 모음. < EMANI_SUBTYPE 를 10늘이는 것보다 이걸 늘이는 것이 효율적.~! >

	AN_GATHERING		= 20,	//	채집

	AN_NONE				= 21,
	AN_TYPE_SIZE		= 21,
};

enum EMANI_SUBTYPE
{
	AN_SUB_NONE			= 0,	//	맨손.
	AN_SUB_ONEHSWORD	= 1,	//	한손검.
	AN_SUB_TWOHSWORD	= 2,	//	양손검.
	AN_SUB_EITHERSWORD	= 3,	//	쌍검.
	AN_SUB_DAGGER		= 4,	//	단검.
	AN_SUB_SPEAR		= 5,	//	창.
	AN_SUB_BOW			= 6,	//	활.
	AN_SUB_THROW		= 7,	//	투척.
	AN_SUB_MANA			= 8,	//	마법.
	AN_SUB_BIGHSWORD	= 9,	//	양손검 대형.
	AN_SUB_STICK		= 10,	//	몽둥이.
	
	AN_SUB_ONESABER		= 11,	//	한손도.
	AN_SUB_TWOSABER		= 12,	//	양손도.
	AN_SUB_BIGSABER		= 13,	//	양손도 대형.
	AN_SUB_EITHERSABER	= 14,	//	쌍도.
	
	AN_SUB_GWON			= 15,	//	권법.

	AN_SUB_BROOM		= 16,	//	빗자루.
	AN_SUB_HOVERBOARD	= 17,	//	보드

	AN_SUB_SIZE			= 18,

	AN_SUB_00			= 0,
	AN_SUB_01			= 1,
	AN_SUB_02			= 2,
	AN_SUB_03			= 3,
	AN_SUB_04			= 4,
	AN_SUB_05			= 5,
	AN_SUB_06			= 6,
	AN_SUB_07			= 7,
	AN_SUB_08			= 8,
	AN_SUB_09			= 9,
	AN_SUB_10			= 10,
	AN_SUB_11			= 11,
	AN_SUB_12			= 12,
	AN_SUB_13			= 13,
	AN_SUB_14			= 14,
	AN_SUB_15			= 15,
	AN_SUB_16			= 16,
	AN_SUB_17			= 17,
	AN_SUB_18			= 18,
	AN_SUB_19			= 19,

	AN_SUB_20			= 20,
	AN_SUB_21			= 21,
	AN_SUB_22			= 22,
	AN_SUB_23			= 23,
	AN_SUB_24			= 24,
	AN_SUB_25			= 25,
	AN_SUB_26			= 26,
	AN_SUB_27			= 27,
	AN_SUB_28			= 28,
	AN_SUB_29			= 29,
	AN_SUB_30			= 30,
	AN_SUB_31			= 31,
	AN_SUB_32			= 32,
	AN_SUB_33			= 33,
	AN_SUB_34			= 34,
	AN_SUB_35			= 35,
	AN_SUB_36			= 36,
	AN_SUB_37			= 37,
	AN_SUB_38			= 38,
	AN_SUB_39			= 39,

	//AN_SUB_00_SIZE		= 40,

	AN_SUB_40			= 40,
	AN_SUB_41			= 41,
	AN_SUB_42			= 42,
	AN_SUB_43			= 43,
	AN_SUB_44			= 44,
	AN_SUB_45			= 45,
	AN_SUB_46			= 46,
	AN_SUB_47			= 47,
	AN_SUB_48			= 48,
	AN_SUB_49			= 49,

	AN_SUB_00_SIZE		= 50,
};

#endif // DXANIKEYS_H_
