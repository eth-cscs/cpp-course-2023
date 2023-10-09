#include <memory>
#include <string>
#include <typeinfo>

// gcc and clang both provide this header
#if __has_include(<cxxabi.h>)
# include <cxxabi.h>
using cxxabi_supported__ = std::true_type;
#else
using cxxabi_supported__ = std::false_type;
// create some dummy function to make the compiler happy in the true_type instantiation
namespace abi {
    template <typename... Ts> char* __cxa_demangle(Ts... ts) { return nullptr; }
}
#endif

// --------------------------------------------------------------------
namespace debug::detail {
    // default : use built-in typeid to get the best info we can
    template <typename T, typename Enabled = std::false_type>
    struct demangle_helper
    {
        char const* type_id() const { return typeid(T).name(); }
    };

    // if available : demangle an arbitrary c++ type using gnu utility
    template <typename T>
    struct demangle_helper<T, std::true_type>
    {
        demangle_helper()
          : demangled_{abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr), std::free}
        {
        }

        char const* type_id() const { return demangled_ ? demangled_.get() : typeid(T).name(); }

    private:
        std::unique_ptr<char, decltype(&std::free)> demangled_;
    };

    template <typename T>
    using cxx_type_id = demangle_helper<T, cxxabi_supported__>;
}

// --------------------------------------------------------------------
// print type information
// usage : std::cout << debug::print_type<args...>("separator")
// separator is appended if the number of types > 1
// --------------------------------------------------------------------
namespace debug {
    template <typename T = void>    // print a single type
    inline std::string print_type(const char* = "")
    {
        return std::string(detail::cxx_type_id<T>().type_id());
    }

    template <>    // fallback for an empty type
    inline std::string print_type<>(char const*)
    {
        return "<>";
    }

    template <typename T, typename... Args>    // print a list of types
    inline std::enable_if_t<sizeof...(Args) != 0, std::string> print_type(const char* delim = "")
    {
        std::string temp(print_type<T>());
        return temp + delim + print_type<Args...>(delim);
    }
}
