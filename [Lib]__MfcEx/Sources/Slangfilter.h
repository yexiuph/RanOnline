#pragma once

#include <string>

//////////////////////////////////////////////////////////////////////////////
/// \file SlangFilter.h
/// \author excel96
/// \date 2003.7.15
///
/// 비속어 필터링을 위한 모듈이다.
///
/// \todo 유니코드를 지원해야하는가...?
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/// \class SlangFilter
/// \brief 비속어 필터링 클래스
/// 
/// 이 클래스는 입력된 비속어를 트리로 만들어 가지고 있는다. 비속어 문자열의 
/// 한 바이트, 한 바이트가 노드를 이루게 된다. 예를 들어, "fuck"이란 단어와 
/// "fucking"이란 단어가 입력되었다면 대강 다음과 같은 모양을 가지게 된다.
///
/// <pre>
/// f(f) - u(f) - c(f) - k(t) - i(f) - n(f) - g(t)
/// </pre>
/// 
/// 옆의 f, t는 SlangNode 클래스의 bLeafNode 멤버 변수와 매치되는 값인데,
/// t일 때, 이 노드가 한 단어의 끝임을 말한다. 즉 fuck이란 단어도 있고,
/// "fucking"이란 단어도 있음을 말한다. 위의 트리에 "fox"란 단어도 비속어로
/// 입력되었다면 다음과 같은 모양이 될 것이다.
///
/// <pre>
/// f(f) + u(f) - c(f) - k(t) - i(f) - n(f) - g(t)
///      |
///      + o(f) - x(t)
/// </pre>
/// 
/// 이런 식으로 트리를 구성해두면, 비속어 데이터베이스가 아무리 커도
/// 비속어 포함 여부를 문자열 길이의 O(N)으로 검사할 수 있다.
//////////////////////////////////////////////////////////////////////////////

class SlangFilter
{
private:
    struct IMPL;
    IMPL* m_pImpl; ///< 내부 데이터 PIMPL


public:
    SlangFilter(); /// 생성자
    ~SlangFilter(); /// 소멸자

	UINT m_nCodePage;


public:
    /// \brief 비속어를 '*'로 치환한 문장을 리턴한다.
    std::wstring filter(const std::wstring& original) const;

    /// \brief 해당하는 문장이 비속어를 포함하고 있는지의 여부를 리턴한다.
    bool hasSlang(const std::wstring& original) const;

    /// \brief 비속어를 추가한다.
    void addSlang(const std::wstring& slang);

	void Init();

	void SetCodePage ( UINT nCodePage ) { m_nCodePage = nCodePage; }

private:
    /// \brief 해당하는 문장의 첫 바이트부터 비속어가 포함되어있는지 검사한다.
    size_t match(const std::wstring& text) const;

    /// \brief 해당하는 글자가 문장 부호인지 검사한다.
    bool isPunctutation(wchar_t c) const;
};

int ToUpperVN( int ch );	// 베트남어도 대문자로 변경해준다.