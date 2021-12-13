//#include "StlFunctions.h"
//
//

#pragma once
#include <map>

namespace std_afunc
{
	class CCompare
	{
	public:
		D3DXVECTOR3 vCenter;

	public:
		CCompare ( D3DXVECTOR3 vCen ) : vCenter(vCen) {}

	public:
		bool operator() ( const D3DXVECTOR3 &vA, const D3DXVECTOR3 &vB )
		{
			D3DXVECTOR3 vLengA = vA-vCenter;
			D3DXVECTOR3 vLengB = vB-vCenter;
			float fLengA = D3DXVec3Length ( &vLengA );
			float fLengB = D3DXVec3Length ( &vLengB );
			return fLengA < fLengB;
		}
	};

	struct DeleteMapObject
	{
		template<typename KEY, typename VALUE>
			void operator() ( std::pair<KEY,VALUE> MapPair ) const
		{
			SAFE_DELETE(MapPair.second);
		}
	};

	struct ReleaseMapObject
	{
		template<typename KEY, typename VALUE>
			void operator() ( std::pair<KEY,VALUE> MapPair ) const
		{
			SAFE_RELEASE(MapPair.second);
		}
	};

	struct DeleteObject
	{
		template<typename VALUE>
			void operator() ( VALUE* &ptr ) const
		{
			delete ptr;
			ptr = NULL;
		}
	};

	struct ReleaseObject
	{
		template<typename VALUE>
			void operator() ( VALUE* &ptr ) const
		{
			SAFE_RELEASE(ptr);
		}
	};

	struct DeleteArray
	{
		template<typename VALUE>
			void operator() ( const VALUE* ptr ) const
		{
			delete[] ptr;
		}
	};

	//	Note : const 형으로 받는 pair 의 value 값을 바꿀려고함.
	//		강제로 상수성을 제거하게 됨. 상수성을 강제로 제거하지만
	//		KEY 부분을 바꾸는게 아니라 괜찬을 걸로 사료됨.
	//
	template<typename KEY, typename VALUE>
	struct SETVALUE
	{
		VALUE m_setvalue;

		explicit SETVALUE ( VALUE value ) :
			m_setvalue(value)
		{
		}

		void operator() ( const std::pair<KEY,VALUE> &MapPair ) const
		{
			const_cast<VALUE&> (MapPair.second) = m_setvalue;
		}
	};
};