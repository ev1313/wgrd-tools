// from https://stackoverflow.com/questions/21456926/how-do-i-convert-a-string-in-utf-16-to-utf-8-in-c
#include <string>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <cstdint>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <clocale>
#endif

#define ASSERT_MSG(cond, msg) { if (!(cond)) throw std::runtime_error("Assertion (" #cond ") failed at line " + std::to_string(__LINE__) + "! Msg: " + std::string(msg)); }
#define ASSERT(cond) ASSERT_MSG(cond, "")

template <typename U8StrT = std::string>
inline static U8StrT Utf32To8(std::u32string const & s) {
    static_assert(sizeof(typename U8StrT::value_type) == 1, "Char byte-size should be 1 for UTF-8 strings!");
    typedef typename U8StrT::value_type VT;
    typedef uint8_t u8;
    U8StrT r;
    for (auto c: s) {
        size_t nby = c <= 0x7FU ? 1 : c <= 0x7FFU ? 2 : c <= 0xFFFFU ? 3 : c <= 0x1FFFFFU ? 4 : c <= 0x3FFFFFFU ? 5 : c <= 0x7FFFFFFFU ? 6 : 7;
        r.push_back(VT(
            nby <= 1 ? u8(c) : (
                (u8(0xFFU) << (8 - nby)) |
                u8(c >> (6 * (nby - 1)))
            )
        ));
        for (size_t i = 1; i < nby; ++i)
            r.push_back(VT(u8(0x80U | (u8(0x3FU) & u8(c >> (6 * (nby - 1 - i)))))));
    }
    return r;
}

template <typename U8StrT>
inline static std::u32string Utf8To32(U8StrT const & s) {
    static_assert(sizeof(typename U8StrT::value_type) == 1, "Char byte-size should be 1 for UTF-8 strings!");
    typedef uint8_t u8;
    std::u32string r;
    auto it = (u8 const *)s.c_str(), end = (u8 const *)(s.c_str() + s.length());
    while (it < end) {
        char32_t c = 0;
        if (*it <= 0x7FU) {
            c = *it;
            ++it;
        } else {
            ASSERT((*it & 0xC0U) == 0xC0U);
            size_t nby = 0;
            for (u8 b = *it; (b & 0x80U) != 0; b <<= 1, ++nby) {(void)0;}
            ASSERT(nby <= 7);
            ASSERT((end - it) >= nby);
            c = *it & (u8(0xFFU) >> (nby + 1));
            for (size_t i = 1; i < nby; ++i) {
                ASSERT((it[i] & 0xC0U) == 0x80U);
                c = (c << 6) | (it[i] & 0x3FU);
            }
            it += nby;
        }
        r.push_back(c);
    }
    return r;
}


template <typename U16StrT = std::u16string>
inline static U16StrT Utf32To16(std::u32string const & s) {
    static_assert(sizeof(typename U16StrT::value_type) == 2, "Char byte-size should be 2 for UTF-16 strings!");
    typedef typename U16StrT::value_type VT;
    typedef uint16_t u16;
    U16StrT r;
    for (auto c: s) {
        if (c <= 0xFFFFU)
            r.push_back(VT(c));
        else {
            ASSERT(c <= 0x10FFFFU);
            c -= 0x10000U;
            r.push_back(VT(u16(0xD800U | ((c >> 10) & 0x3FFU))));
            r.push_back(VT(u16(0xDC00U | (c & 0x3FFU))));
        }
    }
    return r;
}

template <typename U16StrT>
inline static std::u32string Utf16To32(U16StrT const & s) {
    static_assert(sizeof(typename U16StrT::value_type) == 2, "Char byte-size should be 2 for UTF-16 strings!");
    typedef uint16_t u16;
    std::u32string r;
    auto it = (u16 const *)s.c_str(), end = (u16 const *)(s.c_str() + s.length());
    while (it < end) {
        char32_t c = 0;
        if (*it < 0xD800U || *it > 0xDFFFU) {
            c = *it;
            ++it;
        } else if (*it >= 0xDC00U) {
            ASSERT_MSG(false, "Unallowed UTF-16 sequence!");
        } else {
            ASSERT(end - it >= 2);
            c = (*it & 0x3FFU) << 10;
            if ((it[1] < 0xDC00U) || (it[1] > 0xDFFFU)) {
                ASSERT_MSG(false, "Unallowed UTF-16 sequence!");
            } else {
                c |= it[1] & 0x3FFU;
                c += 0x10000U;
            }
            it += 2;
        }
        r.push_back(c);
    }
    return r;
}


template <typename StrT, size_t NumBytes = sizeof(typename StrT::value_type)> struct UtfHelper;
template <typename StrT> struct UtfHelper<StrT, 1> {
    inline static std::u32string UtfTo32(StrT const & s) { return Utf8To32(s); }
    inline static StrT UtfFrom32(std::u32string const & s) { return Utf32To8<StrT>(s); }
};
template <typename StrT> struct UtfHelper<StrT, 2> {
    inline static std::u32string UtfTo32(StrT const & s) { return Utf16To32(s); }
    inline static StrT UtfFrom32(std::u32string const & s) { return Utf32To16<StrT>(s); }
};
template <typename StrT> struct UtfHelper<StrT, 4> {
    inline static std::u32string UtfTo32(StrT const & s) {
        return std::u32string((char32_t const *)(s.c_str()), (char32_t const *)(s.c_str() + s.length()));
    }
    inline static StrT UtfFrom32(std::u32string const & s) {
        return StrT((typename StrT::value_type const *)(s.c_str()),
            (typename StrT::value_type const *)(s.c_str() + s.length()));
    }
};
template <typename StrT> inline static std::u32string UtfTo32(StrT const & s) {
    return UtfHelper<StrT>::UtfTo32(s);
}
template <typename StrT> inline static StrT UtfFrom32(std::u32string const & s) {
    return UtfHelper<StrT>::UtfFrom32(s);
}
template <typename StrToT, typename StrFromT> inline static StrToT UtfConv(StrFromT const & s) {
    return UtfFrom32<StrToT>(UtfTo32(s));
}

