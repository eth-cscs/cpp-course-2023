#include <cassert>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <variant>

int main() {
    {
        // The default state of a variant is the first alternative
        std::variant<std::monostate, std::string, double> v;
        assert(std::holds_alternative<std::monostate>(v));
        assert(!std::holds_alternative<std::string>(v));
        assert(!std::holds_alternative<double>(v));
    }

    {
        // The default state of a variant is the first alternative
        std::variant<std::string, double, std::monostate> v;
        assert(std::holds_alternative<std::string>(v));
        assert(!std::holds_alternative<double>(v));
        assert(!std::holds_alternative<std::monostate>(v));
    }

    {
        // Moving a variant empties the contained type and does not reset the
        // variant to the default state
        std::variant<std::monostate, std::string, double> v{ "hello" };
        assert(!std::holds_alternative<std::monostate>(v));
        assert(std::holds_alternative<std::string>(v));
        assert(!std::holds_alternative<double>(v));

        auto v2 = std::move(v);
        assert(!std::holds_alternative<std::monostate>(v));
        assert(std::holds_alternative<std::string>(v));
        assert(!std::holds_alternative<double>(v));
        assert(!std::holds_alternative<std::monostate>(v2));
        assert(std::holds_alternative<std::string>(v2));
        assert(!std::holds_alternative<double>(v2));

        // The string in v is left in a valid, but unspecified state
        std::cout << "string is: \"" << std::get<std::string>(v) << "\"\n";
    }

    // optional behaves similarly to variant, moving the optional does not reset
    // the optional to the empty state
    {
        // The default state of an optional is the empty state (std::nullopt)
        std::optional<int> o;
        assert(!o.has_value());
        assert(o == std::nullopt);
    }

    {
        // Moving an optional empties the contained type and does not reset the
        // optional to the default state
        std::optional<int> o{ 42 };
        assert(o.has_value());
        // Checked, throws an exception if there is no value
        assert(o.value() == 42);
        // Unsafe
        assert(*o == 42);

        auto o2 = std::move(o);
        assert(o.has_value());
        assert(o2.has_value());

        std::cout << "optional int is: " << o.value() << '\n';

        // We can explicitly reset the source optional
        auto o3 = std::exchange(o2, std::nullopt);
        assert(o.has_value());
        assert(!o2.has_value());
        assert(o3.has_value());
    }
}
