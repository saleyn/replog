#ifndef _REPLOG_RAW_CHAR_HPP_
#define _REPLOG_RAW_CHAR_HPP_

#include <boost/static_assert.hpp>
#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <algorithm>
#include <string.h>
#include <replog/endian.hpp>

namespace replog {

/**
 * \brief A character buffer of size N storing data in 
 * big endian format.
 * The buffer allows for easy conversion between big endian and 
 * native data representation.
 */
template <int N>
class raw_char {
    char m_data[N];
public:
    raw_char() {}

    template <typename T>
    raw_char(T a)                       { *this = a; }

    raw_char(const char (&a)[N+1])      { memcpy(m_data, a, N); }
    raw_char(const std::string& a)      { set(a.c_str(), a.size()); }
    raw_char(const char* a, size_t n)   { set(a, n); }

    template <typename T>
    void operator= (T a) {
        BOOST_STATIC_ASSERT(sizeof(T) == N && sizeof(T) <= 8);
        store_be(m_data, a);
    }

    void operator= (const raw_char<N>& a) { memcpy(m_data, a.m_data, N); }
    void operator= (const std::string& a) { set(a.c_str(), a.size()); }

    void set(const char* a, size_t n) {
        size_t m = std::min((size_t)N, n+1);
        memcpy(m_data, a, m);
        fill('\0', m);
    }

    void fill(char a_ch, int a_offset = 0) {
        memset(&m_data[a_offset], a_ch, std::max(0, N - a_offset));
    }

    template <typename T>
    operator T () const {
        BOOST_STATIC_ASSERT(sizeof(T) == N && sizeof(T) <= 8);
        T n; cast_be(m_data, n); return n;
    }

    //operator const () const { BOOST_STATIC_ASSERT(N == 1); return m_data[0]; }
    operator char* () const { return m_data; }
    operator char* ()       { return m_data; }

    std::string to_string() const {
        size_t n = N;
        for(const char* p = &m_data[N-1]; !*p && n > 0; --p, --n);
        return std::string(m_data, n);
    }

    std::ostream& dump(std::ostream& out) {
        bool printable = true;
        const char* end = m_data + N;
        for (const char* p = m_data; p != end; ++p)
            if (*p < ' ' || *p > '~') {
                printable = false; break;
            } else if (p > m_data && *p == '\0') {
                end = ++p; break;
            }

        for (const char* p = m_data; p != end; ++p)
            if (printable)
                out << *p;
            else
                out << (p == m_data ? "" : ",") << (int)*(unsigned char*)p;
    }
};

template <int N>
::std::ostream& operator<< (::std::ostream& out, const raw_char<N>& a) {
    return a.dump(out);
}

} // namespace replog

#endif // _REPLOG_RAW_CHAR_HPP_

