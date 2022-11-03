/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// These tests are similar to test/test_short_func.h
// They test that functions compile with a custom allocator and nothing else.

// Custom allocator for the following tests
template<class T>
class alloc_func
{
public:
    using value_type = T;

    alloc_func() = default;
    T* allocate(std::size_t n)
    {
        T* p = static_cast<T*>(std::malloc(n * sizeof(T)));
        //std::cout << "Alloc  : " << n << " bytes at " << static_cast<void*>(p) << '\n';
        return p;
    }
    void deallocate(T* p, std::size_t n)
    {
        (void)n;
        //std::cout << "Dealloc: " << n << " bytes at " << static_cast<void*>(p) << '\n';
        std::free(static_cast<void*>(p));
    }
};

// REMINDER: Custom memory resourse
#if 0
#include <memory_resource>

class memory_resource_func : public std::pmr::memory_resource
{
    void* do_allocate(std::size_t bytes, std::size_t alignment) override
    {
        void* p = std::pmr::new_delete_resource()->allocate(bytes, alignment);
        std::cout << "Alloc  : " << bytes << " bytes at " << p << '\n';
        return p;
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
    {
        std::cout << "Dealloc: " << bytes << " bytes at " << p << '\n';
        return std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        return std::pmr::new_delete_resource()->is_equal(other);
    }
};
#endif

void test_alloc_func_conv()
{
    alloc_func<char> alloc;
    alloc_func<char16_t> alloc16;
    alloc_func<char32_t> alloc32;

    std::basic_string<char, std::char_traits<char>, decltype(alloc)> str = "123";
    std::basic_string<char16_t, std::char_traits<char16_t>, decltype(alloc16)> u16str = u"123";
    std::basic_string<char32_t, std::char_traits<char32_t>, decltype(alloc32)> u32str = U"123";

    TESTX((uni::utf8to16<char, char16_t>(str, alloc16) == u16str));
    TESTX((uni::utf8to32<char, char32_t>(str, alloc32) == u32str));
    TESTX((uni::utf16to8<char16_t, char>(u16str, alloc) == str));
    TESTX((uni::utf32to8<char32_t, char>(u32str, alloc) == str));
    TESTX((uni::utf16to32<char16_t, char32_t>(u16str, alloc32) == u32str));
    TESTX((uni::utf32to16<char32_t, char16_t>(u32str, alloc16) == u16str));

    uni::error error;

    TESTX((uni::strict::utf8to16<char, char16_t>(str, error, alloc16) == u16str));
    TESTX((uni::strict::utf8to32<char, char32_t>(str, error, alloc32) == u32str));
    TESTX((uni::strict::utf16to8<char16_t, char>(u16str, error, alloc) == str));
    TESTX((uni::strict::utf32to8<char32_t, char>(u32str, error, alloc) == str));
    TESTX((uni::strict::utf16to32<char16_t, char32_t>(u16str, error, alloc32) == u32str));
    TESTX((uni::strict::utf32to16<char32_t, char16_t>(u32str, error, alloc16) == u16str));

    // REMINDER: Custom memory resourse
#if 0
    //char buffer[64] = {};
    //std::pmr::monotonic_buffer_resource mr{std::data(buffer), std::size(buffer)};
    memory_resource_func mr;
    std::pmr::polymorphic_allocator<char> pa{&mr};
    std::pmr::string result = uni::utf16to8<char16_t, char>(u"123", pa);
    TESTX(result == "123");
#endif
}

void test_alloc_func_case()
{
    alloc_func<char> alloc;
    alloc_func<char16_t> alloc16;

    std::basic_string<char, std::char_traits<char>, decltype(alloc)> str = "123";
    std::basic_string<char16_t, std::char_traits<char16_t>, decltype(alloc16)> u16str = u"123";

    TESTX(uni::cases::to_lowercase_utf8<char>(str, alloc) == str);
    TESTX(uni::cases::to_uppercase_utf8<char>(str, alloc) == str);
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::to_lowercase_utf8<char>(str, uni::locale{}, alloc) == str);
    TESTX(uni::cases::to_uppercase_utf8<char>(str, uni::locale{}, alloc) == str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::to_casefold_utf8<char>(str, alloc) == str);
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::to_titlecase_utf8<char>(str, alloc) == str);
    TESTX(uni::cases::to_titlecase_utf8<char>(str, uni::locale{}, alloc) == str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_BREAK_WORD

    TESTX(uni::cases::to_lowercase_utf16<char16_t>(u16str, alloc16) == u16str);
    TESTX(uni::cases::to_uppercase_utf16<char16_t>(u16str, alloc16) == u16str);
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::to_lowercase_utf16<char16_t>(u16str, uni::locale{}, alloc16) == u16str);
    TESTX(uni::cases::to_uppercase_utf16<char16_t>(u16str, uni::locale{}, alloc16) == u16str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::to_casefold_utf16<char16_t>(u16str, alloc16) == u16str);
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::to_titlecase_utf16<char16_t>(u16str, alloc16) == u16str);
    TESTX(uni::cases::to_titlecase_utf16<char16_t>(u16str, uni::locale{}, alloc16) == u16str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_BREAK_WORD
}

void test_alloc_func_norm()
{
    alloc_func<char> alloc;
    alloc_func<char16_t> alloc16;

    std::basic_string<char, std::char_traits<char>, decltype(alloc)> str = "123";
    std::basic_string<char16_t, std::char_traits<char16_t>, decltype(alloc16)> u16str = u"123";

    TESTX(uni::norm::to_nfc_utf8<char>(str, alloc) == str);
    TESTX(uni::norm::to_nfd_utf8<char>(str, alloc) == str);
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(uni::norm::to_nfkc_utf8<char>(str, alloc) == str);
    TESTX(uni::norm::to_nfkd_utf8<char>(str, alloc) == str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
    TESTX(uni::norm::to_unaccent_utf8<char>(str, alloc) == str);
#endif // UNI_ALGO_DISABLE_PROP

    TESTX(uni::norm::to_nfc_utf16<char16_t>(u16str, alloc16) == u16str);
    TESTX(uni::norm::to_nfd_utf16<char16_t>(u16str, alloc16) == u16str);
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(uni::norm::to_nfkc_utf16<char16_t>(u16str, alloc16) == u16str);
    TESTX(uni::norm::to_nfkd_utf16<char16_t>(u16str, alloc16) == u16str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
    TESTX(uni::norm::to_unaccent_utf16<char16_t>(u16str, alloc16) == u16str);
#endif // UNI_ALGO_DISABLE_PROP
}
