//////////////////////////////////////////////////////////////////////////////
/// \file SlangFilter.cpp
/// \author excel96
/// \date 2003.7.15
///
/// ��Ӿ� ���͸��� ���� ����̴�.
//////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "SlangFilter.h"
//#include "FunctionObject.h"

#include <map>

using namespace std;

#define VN_ALPHA 7	// vietnam alphavet num 
#define VN_CODEPAGE 1258 // vitenam code page 


//////////////////////////////////////////////////////////////////////////////
/// \class SlangNode
/// \brief ��Ӿ� Ʈ���� �����ϴ� ��� Ŭ����.
///
/// �ڽ� ����� ������ ��Ʈ������ �̷������.
//////////////////////////////////////////////////////////////////////////////

class SlangNode : public map<size_t, SlangNode*>
{

private:
    bool m_bLeafNode; ///< �� ��尡 �ܾ��� ���� �� �� �ִ°�?


public:
    /// \brief ������
    SlangNode() : m_bLeafNode(false) {}

    /// \brief �Ҹ���
    ~SlangNode() { 		
		iterator pos;

        for ( pos = begin(); pos != end(); ++pos)
		{
			SAFE_DELETE(pos->second);
		}
		clear();
    }


public:
    /// \brief ��Ʈ���� �̿��� �ش��ϴ� �ڽ� ��带 ã�´�.
    /// \param idx ã���� �ϴ� �ڽ� ����� ��Ʈ��
    /// \return SlangNode* �ش��ϴ� �ڽ� ��尡 ������ ��� �� ����� 
    /// �����͸� ��ȯ�ϰ�, �������� ���� ��� NULL�� ��ȯ�Ѵ�.
    SlangNode* findChild(size_t idx) const
    {
        const_iterator itr(find(idx));
        return itr != end() ? itr->second : NULL;
    }

    /// \brief �ش��ϴ� ��Ʈ���� �ڽ� ��带 �߰��Ѵ�.
    /// \param idx �߰��ϰ��� �ϴ� �ڽ� ����� ��Ʈ��
    /// \return SlangNode* ���� ������ �ڽ� ����� ������
    SlangNode* addChild(size_t idx)
    {
        // �ش��ϴ� �ڽ��� ���� ���, ���ο� ��带 �����ؼ� �߰��Ѵ�.
        iterator itr(find(idx));
        if (itr == end()) itr = insert(value_type(idx, new SlangNode)).first;

        return itr->second;
    }

    /// \name �ܾ��� �� ���θ� ��ȯ/����
    /// \{ 
    bool isLeafNode() const { return m_bLeafNode; }
    void setLeafNode(bool value) { m_bLeafNode = value; }
    /// \} 
};


//////////////////////////////////////////////////////////////////////////////
/// \struct SlangFilter::IMPL
/// \brief SlangFilter Ŭ���� ���� ������ ����ü
//////////////////////////////////////////////////////////////////////////////
struct SlangFilter::IMPL
{
    SlangNode* pRoot; ///< �ֻ��� ���

	static const wstring s_Punctuations; ///< ���� ��ȣ��

    IMPL() : pRoot(new SlangNode) { /*AssertPtr(pRoot);*/ }
    ~IMPL() { SAFE_DELETE(pRoot); }
};

/// ���� ��ȣ��
const wstring SlangFilter::IMPL::s_Punctuations = L" `~!@#$%^&*()-_=+\\|[{]};:'\",<.>/?";


//////////////////////////////////////////////////////////////////////////////
/// \brief ������
//////////////////////////////////////////////////////////////////////////////
SlangFilter::SlangFilter()
: m_pImpl(new IMPL)
, m_nCodePage ( CP_ACP )
{
}

//////////////////////////////////////////////////////////////////////////////
/// \brief �Ҹ���
//////////////////////////////////////////////////////////////////////////////
SlangFilter::~SlangFilter()
{
    SAFE_DELETE(m_pImpl);
}


void SlangFilter::Init()
{
	SAFE_DELETE(m_pImpl);

	m_pImpl = new IMPL;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief ��Ӿ '*'�� ġȯ�� ������ �����Ѵ�.
/// 
/// \param original ���� ����
/// \return std::string ��Ӿ '*'�� ġȯ�� ����
//////////////////////////////////////////////////////////////////////////////
std::wstring SlangFilter::filter(const std::wstring& original) const
{
    std::wstring text(original);
	std::wstring text_upper(original);

	if ( m_nCodePage != VN_CODEPAGE )
		transform(text_upper.begin(), text_upper.end(), text_upper.begin(), toupper);
	else
		transform(text_upper.begin(), text_upper.end(), text_upper.begin(), ToUpperVN);
	

    for (size_t i=0; i<text_upper.size();)
    {
        size_t size = match(text_upper.substr(i, text_upper.size() - i));
        if (size > 0)
        {
            text.replace(i, size, std::wstring(size, '*'));
            i += size;
        }
        else
        { 
			++i;
        }
    }

    return text;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief �ش��ϴ� ������ ��Ӿ �����ϰ� �ִ����� ���θ� �����Ѵ�.
/// 
/// \param original �����ϰ����ϴ� ����
/// \return bool ��Ӿ �����ϰ� ���� ��� true�� ��ȯ�Ѵ�.
//////////////////////////////////////////////////////////////////////////////
bool SlangFilter::hasSlang(const std::wstring& original) const
{
    for (size_t i=0; i<original.size(); i++)
    {
        if (match(original.substr(i, original.size() - i)) > 0)
            return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief ��Ӿ �߰��Ѵ�.
/// \param slang ��Ӿ�� ���� ��ȣ�� ���ԵǾ� ���� �ʾƾ� �Ѵ�.
//////////////////////////////////////////////////////////////////////////////
void SlangFilter::addSlang(const std::wstring& slang)
{
    // �ܾ��� ���̴� 256����Ʈ�� ����. Ư���� ������ ����. �׳� 256����Ʈ�� 
    // �Ѿ�� ���� �Է� ��ü�� ���� ���� �Ŷ�� �����߱� �����̴�.
    // ���� �ܾ �����ȣ�� ���ԵǾ� �ִ� ��쿡�� �߰����� �ʴ´�.
    // match() �Լ��� ����, �˰����� ���� ��ȣ�� �� ������� ������� �ʱ�
    // ���ؼ��̴�. (���� ��� "��...��" ���� ���� �����ϱ� ����!)
    if (slang.size() > 256 ||
        slang.find_first_of(IMPL::s_Punctuations) != std::wstring::npos)
        return;

    // char�� size_t�� �ٷ� ��ȯ��Ű�� �������� ��� �ᰡ�� ��ȯ�� �Ͼ��.
    // ��¿ �� ���� unsigned char�� ���� ��ȯ�� �ڿ� size_t�� ��ȯ��Ų��.
    wchar_t buf[256+1] = {0, };
    _snwprintf_s(buf, sizeof(buf) - 1, L"%s", slang.c_str());
    buf[256] = 0;

    // �ܾ��� ��� ����Ʈ�� iteration�ϸ鼭, �׿� ���� Ʈ���� �����Ѵ�.
    SlangNode* pCurrent = m_pImpl->pRoot;
    for (size_t i=0; i<slang.size(); i++)
    {
        // �ڽ� ��带 �߰�
        pCurrent = pCurrent->addChild((size_t)buf[i]);
    }

    // �ܾ��� ���� �Ǵ� ���� flag�� ǥ�ø� �صξ���Ѵ�.
    pCurrent->setLeafNode(true);
}

//////////////////////////////////////////////////////////////////////////////
/// \brief �ش��ϴ� ������ ù ����Ʈ���� ��Ӿ ���ԵǾ��ִ��� �˻��Ѵ�.
/// \param text �˻��ϰ��� �ϴ� ����.
/// \return size_t ��Ӿ ���ԵǾ� ���� ��쿡�� �� ��Ӿ��� ���̸� 
/// �����Ѵ�. ���ԵǾ� ���� ���� ��쿡�� 0�� �����Ѵ�.
//////////////////////////////////////////////////////////////////////////////
size_t SlangFilter::match(const std::wstring& text) const
{
    if (text.empty()) return 0;
    if (isPunctutation(text[0])) return 0;

    SlangNode* pCurrent = m_pImpl->pRoot;
    size_t i = 0;
	int collect = -1;

    while (i < text.size())
    {
        // ���� ����Ʈ�� ���� ��ȣ�� ��쿡�� �׳� continue�Ѵ�.
        // ��Ӿ� ���̻��̿� ���� ��ȣ�� ���� ��츦 �˻��ϱ� ���ؼ��̴�.
        if (isPunctutation(text[i]))
        {
            i++;
            continue;
        }

        // �ڽ� ��� �߿� ���� ����Ʈ�� ��ġ�ϴ� ������ �̾����� ���� ã�´�.
        size_t idx = (size_t)((wchar_t)text[i]);
        pCurrent = pCurrent->findChild(idx);

        // �� �̻� �̾����� ��尡 ���ٴ� ���� ������ ����Ʈ��� ��ġ�ϴ�
        // �ܾ ��� Ʈ�� �� �������� �ʴ´ٴ� ���̴�.
        if (pCurrent == NULL) return collect+1;

        // ���͸��ؾ��ϴ� �ܾ��!
        if (pCurrent->isLeafNode()) 
		{
			if( i == text.size()-1 )	return i + 1;
			collect = static_cast<int>(i);
		}	

        i++;
    }

	return collect + 1;

}

//////////////////////////////////////////////////////////////////////////////
/// \brief �ش��ϴ� ���ڰ� ���� ��ȣ���� �˻��Ѵ�.
/// \param c �˻��Ϸ��� ����
/// \return bool �ش��ϴ� ���ڰ� ���� ��ȣ�� ��쿡�� true�� ��ȯ�Ѵ�.
//////////////////////////////////////////////////////////////////////////////
bool SlangFilter::isPunctutation(wchar_t c) const
{
    return IMPL::s_Punctuations.find(c) != std::string::npos;
}

int ToUpperVN( int ch )	// ��Ʈ��� �빮�ڷ� �������ش�.
{
	static const int LowVN[VN_ALPHA] = { 0x0103, 0x00E2, 0x0111, 0x00EA, 0x00F4,
										  0x01A1, 0x01B0 };

	static const int UpperVN[VN_ALPHA] = { 0x0102, 0x00C2, 0x0110, 0x00CA, 0x00D4,
											0x01A0, 0x01AF } ;

	if( ch >= 'a' && ch <= 'z' )
	{
		ch += ( 'A' - 'a' );

		return ch;
	}
	else
	{
		for ( int i = 0; i < VN_ALPHA; ++i )
		{
			if ( ch == LowVN[i] )
			{
				ch = UpperVN[i];
				return ch;
			}
		}
	}

	return ch;
}