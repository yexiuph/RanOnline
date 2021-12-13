#pragma once

#ifndef MIN_TEA_H
#define MIN_TEA_H

#define _CRT_SECURE_DEPRECATE_MEMORY
#include <memory.h>
#include <assert.h>

/**
 * \ingroup tea3
 *
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2005-11-04
 *
 * \author jgkim
 *
 * \par license
 *
 * ac_array.h - version 1.0, June 25h, 2003
 *
 * acCArray is a templated dynamic array.
 *
 * Copyright (c) 2003 Andreas J?sson
 *
 * This software is provided 'as-is', without any form of
 * warranty. In no case will the author be held responsible
 * for any damage caused by its use.
 *
 * Permission is granted to anyone to use the software for
 * for any purpose, including commercial. It is also allowed
 * to modify the software and redistribute it free of charge.
 * The permission is granted with the following restrictions:
 *
 * 1.The origin of the software may not be misrepresented.
 *   It must be plainly understandable who is the author of
 *   the original software.
 * 2.Altered versions must not be misrepresented as the
 *   original software, i.e. must be plainly marked as
 *   altered.
 * 3.This notice may not be removed or altered from any
 *   distribution of the software, altered or not.
 *
 * Andreas J?sson
 *  andreas@angelcode.com
 * 
 * \todo 
 *
 * \bug
 */
template <class T> 
class acCArray
{
public:
	acCArray();
	acCArray(const acCArray<T> &);
	~acCArray();

	void Allocate(int numElements, bool keepData);
	int  GetCapacity() const;

	void PushLast(T element);
	T    PopLast();

	void SetLength(int numElements);
	int  GetLength() const;

	void Copy(const T*, int count);
	acCArray<T> &operator =(const acCArray<T> &);

	T &operator [](int index) const;
	T *AddressOf();

protected:
	T   *array;
	int  length;
	int  maxLength;
};

// Implementation

template <class T>
T *acCArray<T>::AddressOf()
{
	return array;
}

template <class T>
acCArray<T>::acCArray(void)
{
	array     = 0;
	length    = 0;
	maxLength = 0;
}

template <class T>
acCArray<T>::acCArray(const acCArray<T> &copy)
{
	array     = 0;
	length    = 0;
	maxLength = 0;

	*this = copy;
}

template <class T>
acCArray<T>::~acCArray(void)
{
	if( array )
	{
		delete[] array;
		array = 0;
	}
}

template <class T>
int acCArray<T>::GetLength() const
{
	return length;
}

template <class T>
T &acCArray<T>::operator [](int index) const
{
	assert(index >= 0);
	assert(index < maxLength);

	return array[index];
}

template <class T>
void acCArray<T>::PushLast(T element)
{
	if( length == maxLength )
		Allocate(int(maxLength*1.5f) + 1, true);

	array[length++] = element;
}

template <class T>
T acCArray<T>::PopLast()
{
	assert(length > 0);

	return array[--length];
}

template <class T>
void acCArray<T>::Allocate(int numElements, bool keepData)
{
	assert(numElements >= 0);

	T *tmp = new T[numElements];

	if( array )
	{
		if( keepData )
		{
			if( length > numElements )
				length = numElements;

			memcpy(tmp, array, length*sizeof(T));
		}
		else
			length = 0;

		delete[] array;
	}

	array = tmp;
	maxLength = numElements;
}

template <class T>
int acCArray<T>::GetCapacity() const
{
	return maxLength;
}

template <class T>
void acCArray<T>::SetLength(int numElements)
{
	assert(numElements >= 0);

	if( numElements > maxLength )
		Allocate(numElements, true);

	length = numElements;
}

template <class T>
void acCArray<T>::Copy(const T *data, int count)
{
	if( maxLength < count )
		Allocate(count, false);

	memcpy(array, data, count*sizeof(T));
}

template <class T>
acCArray<T> &acCArray<T>::operator =(const acCArray<T> &copy)
{
	Copy(copy.array, copy.length);

	return *this;
}

/**
 * \ingroup tea3
 *
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2005-11-04
 *
 * \author jgkim
 *
 * \par license
 * The TEA encryption algorithm was invented by 
 * David Wheeler & Roger Needham at Cambridge 
 * University Computer Lab
 *   http://www.cl.cam.ac.uk/ftp/papers/djw-rmn/djw-rmn-tea.html (1994)
 *   http://www.cl.cam.ac.uk/ftp/users/djw3/xtea.ps (1997)
 *   http://www.cl.cam.ac.uk/ftp/users/djw3/xxtea.ps (1998)
 *
 * This code was originally written in JavaScript by 
 * Chris Veness at Movable Type Ltd
 *   http://www.movable-type.co.uk
 *
 * It was adapted to C++ by Andreas Jonsson 
 *   http://www.angelcode.com
 *
 * Modified by Jgkim
 * 2005-11-03
 * 
 * \todo 
 * 현재는 종료문자가 있는 문자열만 암호화 가능하다.
 * 추후에는 모든 데이터에 대해서 암호하 가능하도록 수정해야 한다.
 *
 * \bug 
 *
 */
#define TEA_KEY_LENGTH 16 /// TEA 알고리듬에서 사용하는 암호키의 길이...

class minTea
{	
public:
	minTea(void);
	minTea(const char* szKey);
	~minTea(void);	

	/// The length of the data should be at least 8 
	/// bytes, else the algorithm will not work.
	/// The key should be 128 bit, or 16 bytes
	/// The memory pointed to by data will be modified by 
	/// the functions.
	/// void TeaEncrypt(char* szData);	
	bool encrypt (char* szData, int nMaxLength);
	bool decrypt (char* szData, int nMaxLength);
	bool setKey (const char* szKey); /// 암호키를 설정한다.

protected:
	void encrypt (char* szData, acCArray<char>& encryptedText, int nMaxLength);
	void decrypt (char* szData, acCArray<char>& decryptedText, int nMaxLength);

	int encrypt (UINT *data, UINT dataLen, UINT *key);
	int decrypt (UINT *data, UINT dataLen, UINT *key);

protected:
	char realKey[TEA_KEY_LENGTH]; /// 암호키
};

#endif // MIN_TEA_H