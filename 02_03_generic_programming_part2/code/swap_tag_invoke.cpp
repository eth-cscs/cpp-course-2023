
/////////////////////////////////////////////////////////////////////////////////////////
// simple tag invoke implementation by Eric Niebler
// https://gist.github.com/ericniebler/056f5459cf259da526d9ea2279c386bb
// requires c++20

#include <utility>
#include <type_traits>

namespace _tag_invoke {
  void tag_invoke();

  struct _fn {
    template <typename CPO, typename... Args>
    constexpr auto operator()(CPO cpo, Args&&... args) const
        noexcept(noexcept(tag_invoke((CPO &&) cpo, (Args &&) args...)))
        -> decltype(tag_invoke((CPO &&) cpo, (Args &&) args...)) {
      return tag_invoke((CPO &&) cpo, (Args &&) args...);
    }
  };

  template <typename CPO, typename... Args>
  using tag_invoke_result_t = decltype(
      tag_invoke(std::declval<CPO>(), std::declval<Args>()...));

  using yes_type = char;
  using no_type = char(&)[2];

  template <typename CPO, typename... Args>
  auto try_tag_invoke(int) //
      noexcept(noexcept(tag_invoke(
          std::declval<CPO>(), std::declval<Args>()...)))
      -> decltype(static_cast<void>(tag_invoke(
          std::declval<CPO>(), std::declval<Args>()...)), yes_type{});

  template <typename CPO, typename... Args>
  no_type try_tag_invoke(...) noexcept(false);

  template <template <typename...> class T, typename... Args>
  struct defer {
    using type = T<Args...>;
  };

  struct empty {};
}  // namespace _tag_invoke

namespace _tag_invoke_cpo {
  inline constexpr _tag_invoke::_fn tag_invoke{};
}
using namespace _tag_invoke_cpo;

template <auto& CPO>
using tag_t = std::remove_cvref_t<decltype(CPO)>;

using _tag_invoke::tag_invoke_result_t;

template <typename CPO, typename... Args>
inline constexpr bool is_tag_invocable_v =
    (sizeof(_tag_invoke::try_tag_invoke<CPO, Args...>(0)) ==
     sizeof(_tag_invoke::yes_type));

template <typename CPO, typename... Args>
struct tag_invoke_result
  : std::conditional_t<
        is_tag_invocable_v<CPO, Args...>,
        _tag_invoke::defer<tag_invoke_result_t, CPO, Args...>,
        _tag_invoke::empty> 
{};

template <typename CPO, typename... Args>
using is_tag_invocable = std::bool_constant<is_tag_invocable_v<CPO, Args...>>;

template <typename CPO, typename... Args>
inline constexpr bool is_nothrow_tag_invocable_v =
    noexcept(_tag_invoke::try_tag_invoke<CPO, Args...>(0));

template <typename CPO, typename... Args>
using is_nothrow_tag_invocable =
    std::bool_constant<is_nothrow_tag_invocable_v<CPO, Args...>>;

template <typename CPO, typename... Args>
concept tag_invocable =
    (sizeof(_tag_invoke::try_tag_invoke<CPO, Args...>(0)) ==
     sizeof(_tag_invoke::yes_type));

/////////////////////////////////////////////////////////////////////////////////////////


#include <utility>
#include <iostream>

namespace std2 {
    namespace hidden {
        struct swap_helper {
            template<class A, class B>
            auto operator()(A& a, B& b) const -> tag_invoke_result_t<swap_helper, A&, B&> {
                return tag_invoke(*this, a, b);
            }
        };
    }
    inline constexpr hidden::swap_helper swap_;

    // use the CPO
    template<typename A, typename B>
        requires std::invocable<decltype(swap_), A&, B&>
    void my_swap(A& a, B& b) {
        // Just call the CPO like an ordinary function
        swap_(a, b);
    }
}


namespace ns {

struct swap1 {

    swap1(int i) : data{i} {}

    friend void swap(tag_t<std2::swap_>, swap1& a, swap1& b) {
        b.data = std::exchange(a.data, b.data);
    }

    int data;
};

}
    
template<typename T>
void print(T const& a, T const& b) {
    std::cout << a.data << " " << b.data << "\n";
}

int main() {

    ns::swap1 a{1}, b{2};
    print(a, b);
    //swap(a, b);
    print(a, b);
    std2::swap(a, b);
    print(a, b);



    return 0;
}

