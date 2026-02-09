// RUN: %clang_cc1 -std=c++20 -Wunsafe-buffer-usage -verify %s

namespace std {
  typedef __SIZE_TYPE__ size_t;

  template <typename CharT>
  class basic_string_view {
  public:
    basic_string_view(const CharT *s, size_t count);
    basic_string_view(const CharT *s);
    template <typename It>
    basic_string_view(It begin, It end);
  };

  using string_view = basic_string_view<char>;
}

void test_string_view_scenarios(const char *ptr, std::size_t n) {
  // --- UNSAFE CASES ---
  std::string_view sv1(ptr, n);
  // expected-warning@-1 {{the two-parameter std::string_view construction is unsafe as it can introduce mismatch between buffer size and the bound information}}

  // --- SAFE CASES (Should NOT warn for string_view construction) ---
  std::string_view sv2("hello", 5);     // Safe: String literal match
  std::string_view sv3(ptr, 0);          // Safe: Zero size
  std::string_view sv4(ptr);             // Safe: Single parameter

  // Note: 'ptr + 5' triggers general pointer arithmetic warning,
  // but the string_view constructor itself is ignored by your gadget.
  std::string_view sv5(ptr, ptr + 5); // expected-warning {{unsafe pointer arithmetic}} \
                                      // expected-note {{pass -fsafe-buffer-usage-suggestions to receive code hardening suggestions}}
}

void test_constant_array_safety() {
  char buffer[10];
  std::string_view sv6(buffer, 10);      // Safe: Constant array bound match
}
