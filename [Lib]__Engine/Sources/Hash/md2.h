#ifndef ___MD2_H___
#define ___MD2_H___

/**
 * \ingroup EngineLib
 *
 * \date 2006-10-25
 *
 * \author Jgkim
 *
 * \par license
 *
 * http://www.codeproject.com/cpp/chash.asp 의 코드를 약간 수정했다. 
 *
 */
// Modified by Dominik Reichl, 2003

namespace HASH
{

class CMD2
{
public:
	CMD2();
	virtual ~CMD2();

	void Init();
	void Update(const unsigned char *buf, unsigned int len);
	void TruncatedFinal(unsigned char *hash, unsigned int size);

private:
	void Transform();
	unsigned char m_X[48], m_C[16], m_buf[16];
	unsigned int m_count;
};

//#include "md2.cpp"

}; // End of namespace HASH

#endif // ___MD2_H___
