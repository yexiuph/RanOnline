#pragma once

#include <string>

//////////////////////////////////////////////////////////////////////////////
/// \file SlangFilter.h
/// \author excel96
/// \date 2003.7.15
///
/// ��Ӿ� ���͸��� ���� ����̴�.
///
/// \todo �����ڵ带 �����ؾ��ϴ°�...?
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/// \class SlangFilter
/// \brief ��Ӿ� ���͸� Ŭ����
/// 
/// �� Ŭ������ �Էµ� ��Ӿ Ʈ���� ����� ������ �ִ´�. ��Ӿ� ���ڿ��� 
/// �� ����Ʈ, �� ����Ʈ�� ��带 �̷�� �ȴ�. ���� ���, "fuck"�̶� �ܾ�� 
/// "fucking"�̶� �ܾ �ԷµǾ��ٸ� �밭 ������ ���� ����� ������ �ȴ�.
///
/// <pre>
/// f(f) - u(f) - c(f) - k(t) - i(f) - n(f) - g(t)
/// </pre>
/// 
/// ���� f, t�� SlangNode Ŭ������ bLeafNode ��� ������ ��ġ�Ǵ� ���ε�,
/// t�� ��, �� ��尡 �� �ܾ��� ������ ���Ѵ�. �� fuck�̶� �ܾ �ְ�,
/// "fucking"�̶� �ܾ ������ ���Ѵ�. ���� Ʈ���� "fox"�� �ܾ ��Ӿ��
/// �ԷµǾ��ٸ� ������ ���� ����� �� ���̴�.
///
/// <pre>
/// f(f) + u(f) - c(f) - k(t) - i(f) - n(f) - g(t)
///      |
///      + o(f) - x(t)
/// </pre>
/// 
/// �̷� ������ Ʈ���� �����صθ�, ��Ӿ� �����ͺ��̽��� �ƹ��� Ŀ��
/// ��Ӿ� ���� ���θ� ���ڿ� ������ O(N)���� �˻��� �� �ִ�.
//////////////////////////////////////////////////////////////////////////////

class SlangFilter
{
private:
    struct IMPL;
    IMPL* m_pImpl; ///< ���� ������ PIMPL


public:
    SlangFilter(); /// ������
    ~SlangFilter(); /// �Ҹ���

	UINT m_nCodePage;


public:
    /// \brief ��Ӿ '*'�� ġȯ�� ������ �����Ѵ�.
    std::wstring filter(const std::wstring& original) const;

    /// \brief �ش��ϴ� ������ ��Ӿ �����ϰ� �ִ����� ���θ� �����Ѵ�.
    bool hasSlang(const std::wstring& original) const;

    /// \brief ��Ӿ �߰��Ѵ�.
    void addSlang(const std::wstring& slang);

	void Init();

	void SetCodePage ( UINT nCodePage ) { m_nCodePage = nCodePage; }

private:
    /// \brief �ش��ϴ� ������ ù ����Ʈ���� ��Ӿ ���ԵǾ��ִ��� �˻��Ѵ�.
    size_t match(const std::wstring& text) const;

    /// \brief �ش��ϴ� ���ڰ� ���� ��ȣ���� �˻��Ѵ�.
    bool isPunctutation(wchar_t c) const;
};

int ToUpperVN( int ch );	// ��Ʈ��� �빮�ڷ� �������ش�.