#include <functional>
#include <iostream>
#include <numeric>
#include <type_traits>
#include <vector>
#include <utility>

template <typename T> std::vector<T> append(std::vector<T> a, const std::vector<T>& b)
{
	std::copy(b.begin(), b.end(), std::back_inserter(a));
	return a;
}

template <typename T> class Parser
{
public:
	// read a T from std::in or throw
	T read() const
	{
		return m_reader();
	}

	// read any char from stdin
	friend Parser<char> charAny();

	// read a particular char from stdin
	friend Parser<char> charP(char c);

	// Parse with lhs if string begins with the corretct char, otherwise rhs.
	template <typename A> friend Parser<A> either(Parser<A> lhs, Parser<A> rhs);

	// Parse with 'a' as many times as possible
	template <typename A> friend Parser<std::vector<A>> zeroOrMore(Parser<A> a);

public:
	Parser<T>(){};
	std::function<T()> m_reader;
	bool m_consumesNothing;
	std::vector<char> m_startChar;
};

Parser<char> charAny()
{
	Parser<char> anyParser;
	anyParser.m_consumesNothing = false;
	for(int i = 0; i < 128; ++i)
		anyParser.m_startChar.push_back(char(i));
	anyParser.m_reader = []() {
		char c;
		std::cin >> c;
		return c;
	};

	return anyParser;
}

Parser<char> charP(char c)
{
	using namespace std::string_literals;
	Parser<char> cParser;
	cParser.m_consumesNothing = false;
	cParser.m_startChar = {c};
	cParser.m_reader = [c]() {
		char cc;
		std::cin >> cc;
		if(cc != c)
		{
			std::string m = "Expected char '"s;
			m.push_back(c);
			m += "' not found. It was a '"s;
			m.push_back(cc);
			m += "'";
			throw std::runtime_error(m);
		}
		return c;
	};
	return cParser;
}

template <typename A> Parser<A> either(Parser<A> lhs, Parser<A> rhs)
{
	Parser<A> result;
	result.m_consumesNothing = lhs.m_consumesNothing;
	result.m_startChar = append(lhs.m_startChar, rhs.m_startChar);
	result.m_reader = [lhs, rhs]() {
		const char c = std::cin.peek();
		if(std::find(lhs.m_startChar.begin(), lhs.m_startChar.end(), c) != lhs.m_startChar.end())
			return lhs.read();
		else
			return rhs.read();
	};
	return result;
}

// pure operation of Parser<T>
// Reads nothing and always succeeds with the specified value
template <typename T> Parser<T> successP(T value)
{
	Parser<T> result;
	result.m_startChar = {};
	result.m_consumesNothing = true;
	result.m_reader = [value]() { return value; };
	return result;
}

template <typename A> Parser<std::vector<A>> zeroOrMore(Parser<A> a)
{
	Parser<std::vector<A>> result;
	result.m_consumesNothing = a.m_consumesNothing;
	result.m_startChar = a.m_startChar;
	result.m_reader = [a]() {
		std::vector<A> aa;
		while(true)
		{
			const char c = std::cin.peek();
			if(std::find(a.m_startChar.begin(), a.m_startChar.end(), c) != end(a.m_startChar))
			{
				aa.push_back(a.read());
			} else
				break;
		}
		return aa;
	};
	return result;
}

template <typename F, typename T> auto apply(Parser<F> fParser, Parser<T> tParser)
{
	Parser<std::result_of_t<F(T)>> result;
	result.m_consumesNothing = fParser.m_consumesNothing && tParser.m_consumesNothing;

	if(fParser.m_consumesNothing)
		result.m_startChar = tParser.m_startChar;
	else
		result.m_startChar = fParser.m_startChar;

	result.m_reader = [fParser, tParser]() {
		F f = fParser.read();
		T t = tParser.read();
		return f(t);
	};
	return result;
}

//------------------

template <typename A, typename B>
Parser<std::pair<A, B>> operator>>(Parser<A> aParser, Parser<B> bParser)
{
	auto afParser =
	    apply(successP([](A a) { return [a](B b) { return std::make_pair(a, b); }; }), aParser);

	return apply(afParser, bParser);
}

int main()
{
	auto digitToIntParser = successP([](char c) { return static_cast<int>(c - '0'); });
	auto char0Parser = charP('0');
	auto char1Parser = charP('1');
	auto char2Parser = charP('2');
	auto charDigitParser = either(char0Parser, either(char1Parser, char2Parser));
	Parser<int> digitP = apply(digitToIntParser, charDigitParser);

	Parser<int> intP = apply(successP([](std::vector<int> v) {
		                         return std::accumulate(begin(v), end(v), 0,
		                                                [](int x, int y) { return 10 * x + y; });
	                         }),
	                         zeroOrMore(digitP));

	auto twoIntsP = intP >> charP(' ') >> intP;
	auto p = twoIntsP.read();
	auto x = p.first.first;
	auto y = p.second;
	std::cout << "(" << x << ", " << y << ")\n";
}
