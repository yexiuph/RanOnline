#pragma once

#include "lzoconf.h"
#include "lzo1x.h"

/**
 * \ingroup lzoTest
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
 * \date 2005-09-08
 *
 * \author Jgkim
 *
 * \par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 * http://www.oberhumer.com/opensource/lzo/
 *
 * \todo 
 * 
 * \bug 
 *
 */
#pragma comment (lib, "lzo2.lib") // lzo compress lib

class CMinLzo
{
public:
	enum
	{
		MINLZO_SUCCESS          =  0, //! 성공
		MINLZO_ERROR            = -1, //! 실패
		MINLZO_INPUT_DATA_ERROR = -2, //! 입력데이터 오류
		MINLZO_INTERNAL_ERROR   = -3, //! 내부 데이터 오류
		MINLZO_CAN_NOT_COMPRESS = -4, //! 압축 할 수 없음
	};

public:
	static CMinLzo& GetInstance();

	// static CMinLzo* GetInstance();
	// static void ReleaseInstance();
private:
	//! 생성자
	CMinLzo(void);
	~CMinLzo(void);

public:
	//! 압축라이브러리를 초기화 한다.
	int init();

	//! 데이터를 압축한다.
	//! \param pInBuffer 압축될 원본 데이터
	//! \param nInLength 압축될 원본 데이터의 크기
	//! \param pOutBuffer 압축된 데이터가 저장될 버퍼
	//! \param nOutLength 원본데이터가 압축된 후의 크기
	//! \return MINLZO_SUCCESS 성공 \n
	//!         성공이 아닐경우 getErrorString() 호출
	int lzoCompress(byte* pInBuffer, 
				    int nInLength, 
					byte* pOutBuffer, 
					int &nOutLength);

	//! 데이터 압축을 해제한다
	//! \param pOutBuffer 압축된 데이터가 저장된 버퍼
	//! \param pOutLength 압축된 데이터의 크기
	//! \param pInBuffer 압축해제된 데이터가 저장될 버퍼	
	//! \param nNewLength 압축해제된 데이터의 크기
	//! \return MINLZO_SUCCESS 성공 \n
	//!         성공이 아닐경우 getErrorString() 호출
	int lzoDeCompress(byte* pOutBuffer,
					  int nOutLength, 
					  byte* pInBuffer, 
					  int &nNewLength);

	//! 에러발생시 발생된 에러에 대한 설명을 반환한다.
	//! \return 에러메시지
	std::string& getErrorString();

protected:
	lzo_bytep m_pWorkmem; //! 내부적으로 압축/해제에 사용할 메모리 포인터
	bool m_bInit; //! 라이브러리가 초기화 되었는지 여부
	std::string m_strError; //! 에러발생시 문자열을 저장
	CRITICAL_SECTION		m_CriticalSection; // criticalsection object
};