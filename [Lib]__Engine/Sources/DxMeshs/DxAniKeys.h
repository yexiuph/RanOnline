#ifndef DXANIKEYS_H_
#define DXANIKEYS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
//
//	Note : xFile ���ϸ��̼� �������� ����ü.
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
	AN_GUARD_N			= 0,	//	���.
	AN_PLACID			= 1,	//	�޽�.
	AN_WALK				= 2,	//	�ȱ�.
	AN_RUN				= 3,	//	�ٱ�.
	AN_ATTACK			= 4,	//	����.
	AN_SHOCK			= 5,	//	���.
	AN_DIE				= 6,	//	����.

	AN_CREATE			= 7,	//	����.
	AN_TALK				= 8,	//	��ȭ.

	AN_SKILL			= 9,	//	��ų.
	AN_GESTURE			= 10,	//	����.
	AN_SHOCK_MIX		= 11,	//	���,mix��.

	AN_GUARD_L			= 12,	//	���,SP����.
	AN_CONFT_WIN		= 13,	//	���,�¸�.
	AN_CONFT_LOSS		= 14,	//	���,�й�.

	AN_SPEC				= 15,	//	Ư��.

	//AN_NONE				= 16,
	//AN_TYPE_SIZE		= 16,

	AN_SKILL_B			= 16,	//	��ų.
	AN_SKILL_C			= 17,	//	��ų.
	AN_SKILL_D			= 18,	//	��ų.
	AN_SKILL_E			= 19,	//	��ų. - �ذ��ο� ���� ��ų ����. < EMANI_SUBTYPE �� 10���̴� �ͺ��� �̰� ���̴� ���� ȿ����.~! >

	AN_GATHERING		= 20,	//	ä��

	AN_NONE				= 21,
	AN_TYPE_SIZE		= 21,
};

enum EMANI_SUBTYPE
{
	AN_SUB_NONE			= 0,	//	�Ǽ�.
	AN_SUB_ONEHSWORD	= 1,	//	�Ѽհ�.
	AN_SUB_TWOHSWORD	= 2,	//	��հ�.
	AN_SUB_EITHERSWORD	= 3,	//	�ְ�.
	AN_SUB_DAGGER		= 4,	//	�ܰ�.
	AN_SUB_SPEAR		= 5,	//	â.
	AN_SUB_BOW			= 6,	//	Ȱ.
	AN_SUB_THROW		= 7,	//	��ô.
	AN_SUB_MANA			= 8,	//	����.
	AN_SUB_BIGHSWORD	= 9,	//	��հ� ����.
	AN_SUB_STICK		= 10,	//	������.
	
	AN_SUB_ONESABER		= 11,	//	�Ѽյ�.
	AN_SUB_TWOSABER		= 12,	//	��յ�.
	AN_SUB_BIGSABER		= 13,	//	��յ� ����.
	AN_SUB_EITHERSABER	= 14,	//	�ֵ�.
	
	AN_SUB_GWON			= 15,	//	�ǹ�.

	AN_SUB_BROOM		= 16,	//	���ڷ�.
	AN_SUB_HOVERBOARD	= 17,	//	����

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
