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
	//	Pool �� �ִ°� ��ȯ.
	if ( !m_UnUsed.empty() )
	{
		TYPE* ptr = *m_UnUsed.begin();
		m_UnUsed.pop_front();

		return ptr;
	}

	//	���� ����.
	m_dwNewObj++;
	return new TYPE;
}

//	Note : ��ȯ�Ǵ� ������ �ʱ�ȭ���� �ִ´ٰ� ����.
//
template<typename TYPE>
inline void CMemPool<TYPE>::ReleaseNonInit ( TYPE* ptr )
{
	m_UnUsed.push_back ( ptr );
}

//	Note : ��ȯ�Ǵ� ������ �⺻ �����ڷ� �ʱ�ȭ.
//
template<typename TYPE>
inline void CMemPool<TYPE>::Release ( TYPE* ptr )
{
	*ptr = TYPE();
	m_UnUsed.push_back ( ptr );
}

