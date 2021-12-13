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
		MINLZO_SUCCESS          =  0, //! ����
		MINLZO_ERROR            = -1, //! ����
		MINLZO_INPUT_DATA_ERROR = -2, //! �Էµ����� ����
		MINLZO_INTERNAL_ERROR   = -3, //! ���� ������ ����
		MINLZO_CAN_NOT_COMPRESS = -4, //! ���� �� �� ����
	};

public:
	static CMinLzo& GetInstance();

	// static CMinLzo* GetInstance();
	// static void ReleaseInstance();
private:
	//! ������
	CMinLzo(void);
	~CMinLzo(void);

public:
	//! ������̺귯���� �ʱ�ȭ �Ѵ�.
	int init();

	//! �����͸� �����Ѵ�.
	//! \param pInBuffer ����� ���� ������
	//! \param nInLength ����� ���� �������� ũ��
	//! \param pOutBuffer ����� �����Ͱ� ����� ����
	//! \param nOutLength ���������Ͱ� ����� ���� ũ��
	//! \return MINLZO_SUCCESS ���� \n
	//!         ������ �ƴҰ�� getErrorString() ȣ��
	int lzoCompress(byte* pInBuffer, 
				    int nInLength, 
					byte* pOutBuffer, 
					int &nOutLength);

	//! ������ ������ �����Ѵ�
	//! \param pOutBuffer ����� �����Ͱ� ����� ����
	//! \param pOutLength ����� �������� ũ��
	//! \param pInBuffer ���������� �����Ͱ� ����� ����	
	//! \param nNewLength ���������� �������� ũ��
	//! \return MINLZO_SUCCESS ���� \n
	//!         ������ �ƴҰ�� getErrorString() ȣ��
	int lzoDeCompress(byte* pOutBuffer,
					  int nOutLength, 
					  byte* pInBuffer, 
					  int &nNewLength);

	//! �����߻��� �߻��� ������ ���� ������ ��ȯ�Ѵ�.
	//! \return �����޽���
	std::string& getErrorString();

protected:
	lzo_bytep m_pWorkmem; //! ���������� ����/������ ����� �޸� ������
	bool m_bInit; //! ���̺귯���� �ʱ�ȭ �Ǿ����� ����
	std::string m_strError; //! �����߻��� ���ڿ��� ����
	CRITICAL_SECTION		m_CriticalSection; // criticalsection object
};