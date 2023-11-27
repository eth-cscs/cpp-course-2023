#include <coroutine>
#include <iostream>
#include <optional>


template <class T>
class task;


template <class T>
struct promise {
    task<T> get_return_object() noexcept {
        return task<T>(this);
    }

    constexpr auto initial_suspend() const noexcept {
        return std::suspend_always{};
    }

    constexpr auto final_suspend() const noexcept {
        return std::suspend_always{};
    }

    void return_value(T value) noexcept {
        m_result = std::move(value);
    }

    void unhandled_exception() noexcept {
        std::terminate();
    }

    T get_result() noexcept {
        return std::move(m_result.value());
    }

    auto handle() noexcept {
        return std::coroutine_handle<promise>::from_promise(*this);
    }

private:
    std::optional<T> m_result;
};


template <class T>
struct awaitable {
    promise<T>* m_promise;

    bool await_ready() const noexcept {
        m_promise->handle().resume();
        return true;
    }

    void await_suspend(std::coroutine_handle<>) const noexcept {}

    T await_resume() const noexcept {
        return m_promise->get_result();
    }
};


template <class T>
class [[nodiscard]] task {
public:
    using promise_type = promise<T>;

    task(promise_type* promise) : m_promise(promise) {}
    
    ~task() {
        m_promise->handle().destroy();
    }

    T get() noexcept {
        m_promise->handle().resume();
        return m_promise->get_result();
    }

    auto operator co_await() noexcept {
        return awaitable<T>(m_promise);
    }

private:
    promise_type* m_promise = nullptr;
};


task<int> my_first_coro() {
    co_return 1;
}

task<int> my_second_coro() {
    const int value = co_await my_first_coro();
    co_return value;
}

int main() {
    auto result = my_second_coro();
    const auto value = result.get();
    std::cout << value << std::endl;
}