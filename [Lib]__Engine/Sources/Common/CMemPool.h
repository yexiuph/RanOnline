#pragma once

#include <GASSERT.h>
#include <list>
#include <algorithm>

template<typename TYPE>
class CMemPool
{
private:
	struct DeleteObject
	{
		template<typename TYPE>
			void operator() ( const TYPE* ptr ) const
		{
			delete ptr;
		}
	};

	typedef typename std::list<TYPE*>	LIST;
	typedef LIST						LIST_ITER;

private:
	DWORD	m_dwNewObj;
	LIST	m_UnUsed;

public:
	CMemPool ();
	~CMemPool ();

public:
	TYPE* New ();
	void Release ( TYPE* ptr );
	void ReleaseNonInit ( TYPE* ptr );

public:
	DWORD GetAmount ()		{ return static_cast<DWORD>(m_UnUsed.size()); }
};

template<typename TYPE>
inline CMemPool<TYPE>::CMemPool () :
	m_dwNewObj(0)
{
}

template<typename TYPE>
inline CMemPool<TYPE>::~CMemPool ()
{
	std::for_each ( m_UnUsed.begin(), m_UnUsed.end(), DeleteObject() );
}

template<typename TYPE>
inline TYPE* CMemPool<TYPE>::New ()
{
	//	Pool 에 있는것 반환.
	if ( !m_UnUsed.empty() )
	{
		TYPE* ptr = *m_UnUsed.begin();
		m_UnUsed.pop_front();

		return ptr;
	}

	//	새로 생성.
	m_dwNewObj++;
	return new TYPE;
}

//	Note : 반환되는 변수는 초기화된후 넣는다고 가정.
//
template<typename TYPE>
inline void CMemPool<TYPE>::ReleaseNonInit ( TYPE* ptr )
{
	m_UnUsed.push_back ( ptr );
}

//	Note : 반환되는 변수는 기본 생성자로 초기화.
//
template<typename TYPE>
inline void CMemPool<TYPE>::Release ( TYPE* ptr )
{
	*ptr = TYPE();
	m_UnUsed.push_back ( ptr );
}

