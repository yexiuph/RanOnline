//////////////////////////////////////////////////////////////////////////////
/// \file SlangFilter.cpp
/// \author excel96
/// \date 2003.7.15
///
/// 비속어 필터링을 위한 모듈이다.
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
/// \brief 비속어 트리를 구성하는 노드 클래스.
///
/// 자식 노드의 구분은 비트값으로 이루어진다.
//////////////////////////////////////////////////////////////////////////////

class SlangNode : public map<size_t, SlangNode*>
{

private:
    bool m_bLeafNode; ///< 이 노드가 단어의 끝이 될 수 있는가?


public:
    /// \brief 생성자
    SlangNode() : m_bLeafNode(false) {}

    /// \brief 소멸자
    ~SlangNode() { 		
		iterator pos;

        for ( pos = begin(); pos != end(); ++pos)
		{
			SAFE_DELETE(pos->second);
		}
		clear();
    }


public:
    /// \brief 비트값을 이용해 해당하는 자식 노드를 찾는다.
    /// \param idx 찾고자 하는 자식 노드의 비트값
    /// \return SlangNode* 해당하는 자식 노드가 존재할 경우 그 노드의 
    /// 포인터를 반환하고, 존재하지 않을 경우 NULL을 반환한다.
    SlangNode* findChild(size_t idx) const
    {
        const_iterator itr(find(idx));
        return itr != end() ? itr->second : NULL;
    }

    /// \brief 해당하는 비트값의 자식 노드를 추가한다.
    /// \param idx 추가하고자 하는 자식 노드의 비트값
    /// \return SlangNode* 새로 생성한 자식 노드의 포인터
    SlangNode* addChild(size_t idx)
    {
        // 해당하는 자식이 없을 경우, 새로운 노드를 생성해서 추가한다.
        iterator itr(find(idx));
        if (itr == end()) itr = insert(value_type(idx, new SlangNode)).first;

        return itr->second;
    }

    /// \name 단어의 끝 여부를 반환/설정
    /// \{ 
    bool isLeafNode() const { return m_bLeafNode; }
    void setLeafNode(bool value) { m_bLeafNode = value; }
    /// \} 
};


//////////////////////////////////////////////////////////////////////////////
/// \struct SlangFilter::IMPL
/// \brief SlangFilter 클래스 내부 데이터 구조체
//////////////////////////////////////////////////////////////////////////////
struct SlangFilter::IMPL
{
    SlangNode* pRoot; ///< 최상위 노드

	static const wstring s_Punctuations; ///< 문장 부호들

    IMPL() : pRoot(new SlangNode) { /*AssertPtr(pRoot);*/ }
    ~IMPL() { SAFE_DELETE(pRoot); }
};

/// 문장 부호들
const wstring SlangFilter::IMPL::s_Punctuations = L" `~!@#$%^&*()-_=+\\|[{]};:'\",<.>/?";


//////////////////////////////////////////////////////////////////////////////
/// \brief 생성자
//////////////////////////////////////////////////////////////////////////////
SlangFilter::SlangFilter()
: m_pImpl(new IMPL)
, m_nCodePage ( CP_ACP )
{
}

//////////////////////////////////////////////////////////////////////////////
/// \brief 소멸자
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
/// \brief 비속어를 '*'로 치환한 문장을 리턴한다.
/// 
/// \param original 원래 문장
/// \return std::string 비속어가 '*'로 치환된 문장
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
/// \brief 해당하는 문장이 비속어를 포함하고 있는지의 여부를 리턴한다.
/// 
/// \param original 조사하고자하는 문장
/// \return bool 비속어를 포함하고 있을 경우 true를 반환한다.
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
/// \brief 비속어를 추가한다.
/// \param slang 비속어에는 문장 부호가 포함되어 있지 않아야 한다.
//////////////////////////////////////////////////////////////////////////////
void SlangFilter::addSlang(const std::wstring& slang)
{
    // 단어의 길이는 256바이트로 제한. 특별한 이유는 없다. 그냥 256바이트를 
    // 넘어가는 욕은 입력 자체가 뭔가 꼬인 거라고 생각했기 때문이다.
    // 또한 단어에 문장부호가 포함되어 있는 경우에는 추가하지 않는다.
    // match() 함수를 보면, 알겠지만 문장 부호는 비교 대상으로 취급하지 않기
    // 위해서이다. (예를 들어 "바...보" 같은 욕을 검출하기 위해!)
    if (slang.size() > 256 ||
        slang.find_first_of(IMPL::s_Punctuations) != std::wstring::npos)
        return;

    // char를 size_t로 바로 변환시키면 음수값일 경우 콩가루 변환이 일어난다.
    // 어쩔 수 없이 unsigned char로 먼저 변환한 뒤에 size_t로 변환시킨다.
    wchar_t buf[256+1] = {0, };
    _snwprintf_s(buf, sizeof(buf) - 1, L"%s", slang.c_str());
    buf[256] = 0;

    // 단어의 모든 바이트를 iteration하면서, 그에 따른 트리를 생성한다.
    SlangNode* pCurrent = m_pImpl->pRoot;
    for (size_t i=0; i<slang.size(); i++)
    {
        // 자식 노드를 추가
        pCurrent = pCurrent->addChild((size_t)buf[i]);
    }

    // 단어의 끝이 되는 노드는 flag로 표시를 해두어야한다.
    pCurrent->setLeafNode(true);
}

//////////////////////////////////////////////////////////////////////////////
/// \brief 해당하는 문장의 첫 바이트부터 비속어가 포함되어있는지 검사한다.
/// \param text 검사하고자 하는 문장.
/// \return size_t 비속어가 포함되어 있을 경우에는 그 비속어의 길이를 
/// 리턴한다. 포함되어 있지 않을 경우에는 0을 리턴한다.
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
        // 현재 바이트가 문장 부호일 경우에는 그냥 continue한다.
        // 비속어 사이사이에 문장 부호를 넣은 경우를 검색하기 위해서이다.
        if (isPunctutation(text[i]))
        {
            i++;
            continue;
        }

        // 자식 노드 중에 현재 바이트와 일치하는 값으로 이어지는 것을 찾는다.
        size_t idx = (size_t)((wchar_t)text[i]);
        pCurrent = pCurrent->findChild(idx);

        // 더 이상 이어지는 노드가 없다는 말은 현재의 바이트들과 일치하는
        // 단어가 노드 트리 상에 존재하지 않는다는 말이다.
        if (pCurrent == NULL) return collect+1;

        // 필터링해야하는 단어다!
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
/// \brief 해당하는 글자가 문장 부호인지 검사한다.
/// \param c 검사하려는 글자
/// \return bool 해당하는 글자가 문장 부호일 경우에는 true를 반환한다.
//////////////////////////////////////////////////////////////////////////////
bool SlangFilter::isPunctutation(wchar_t c) const
{
    return IMPL::s_Punctuations.find(c) != std::string::npos;
}

int ToUpperVN( int ch )	// 베트남어도 대문자로 변경해준다.
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