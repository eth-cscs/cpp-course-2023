#include <cassert>
#include <cstddef>
#include <memory>

// Abstraction around a generic memory buffer. As always, be careful managing
// buffers like these, but if you're going to use them at least ensure that
// resources are freed correctly by using RAII and rule of zero/five!
class buffer {
    using buffer_type = char[];

public:
    // Construct a buffer that is backed by memory
    buffer(std::size_t n) : p(
        // What's the difference between these?
        // 1.
        std::make_unique<buffer_type>(n)
        // 2.
        // new char[n]
        //
        // 3.
        // new char[n]()
        //
        // See overload 2) of
        // https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique and
        // 2.2 of https://en.cppreference.com/w/cpp/language/new.
        //
        // Also see https://quick-bench.com/q/RjAoY0EgtzOOiiajSQUuiB4rn74.
    ) {}


    // All of the following special member functions can be omitted (rule of
    // zero), or may be explicitly defaulted/deleted (rule of five).

    // Default construction results in an empty buffer
    buffer() = default;

    // Move construction empties the source buffer
    buffer(buffer&&) = default;
    // Move assignment empties the source buffer
    buffer& operator=(buffer&&) = default;

    // Copy construction is disabled thanks to unique_ptr having a deleted copy constructor
    buffer(buffer const&) = default;
    // Copy assignment is deleted thanks to unique_ptr having a deleted copy assignment operator
    buffer& operator=(buffer const&) = default;

    // Destructor will release the buffer, if p contains one
    ~buffer() = default;

    bool valid() const noexcept {
        return bool(p);
    }

private:
    std::unique_ptr<buffer_type> p;
};

int main() {
    buffer b(1024);
    assert(b.valid());

    buffer b2;
    assert(!b2.valid());

    b2 = std::move(b);
    assert(!b.valid());
    assert(b2.valid());

    // use buffer...

    // b2 will release the underlying buffer, b will not release anything
    // because the contained unique_ptr is empty. This all happens without
    // explicitly writing a single move/copy constructor/assignment operator or
    // destructor.
}
