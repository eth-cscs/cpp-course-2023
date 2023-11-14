---
marp: true
theme: cscs
# class: lead
paginate: true
backgroundColor: #fff
backgroundImage: url('../slides-support/common/4k-slide-bg-white.png')
size: 16:9
---

# **Coroutines in a bit more than a nutshell**
![bg cover](../slides-support/common/title-bg3.png)
<!-- _paginate: skip  -->
<!-- _class: titlecover -->
<!-- _footer: "" -->


#### Péter Kardos

---

# Suspendable functions

<div class="twocolumns">
<div>

### Threads

```c++
std::future<int> compute(std::future<int> input) {
    // Start function on thread A
    int value = input.get(); // Suspend thread A
    // Resume function on thread A
    return std::async([=]{ return f(value); });
}
```

</div>
<div>

### Coroutines

```c++
task<int> compute(task<int> input) {
    // Start coroutine on thread A
    int value = co_await input; // Suspend coroutine
    // Resume coroutine on thread B
    co_return f(value);
}
```

</div>
</div>

- Resumable functions are not new
- Threads do get suspended in the middle of a function call
- They do pick up where they left off once resumed by the OS
- Coroutines do the same thing essentially

---

# Threads vs coroutines: performance

<div class="twocolumns">
<div>

- Threads are expensive:
    - They have 1 MiB stack allocated each
    - Synchonization requires OS kernel calls
    - Suspension involves the OS scheduler
    - Creation and destruction is expensive

</div>
<div>

- Coroutines are cheap:
    - They are stackless
    - Synchronization happens in userspace: uses cheap atomics or spinlocks
    - Suspension is just saving some registers
    - Creation and destruction is just a new/delete

</div>
</div>

Comparison:
- Number of threads my PC can finish in a second: ...
- Number of coroutines my PC can finish in a second: ...

---

# Threads vs coroutines: syntax

<div class="twocolumns">
<div>

### Future-then pattern

```c++
int r = std::async(
        [](int v){ return add(v, 1); },
        1
    )
    .then([](int v){ return add(v, 2); })
    .then([](int v){ return add(v, 3); })
    .then([](int v){ return add(v, 4); })
    .then([](int v){ return add(v, 5); })
    .get();
```

- Async code has no resemblance to the linear code
- Hard to read, write, and debug

</div>
<div>

### Coroutine pattern

```c++
int r = [](int v) -> task<int> {
    int r = v;
    r = co_await add(r, 1);
    r = co_await add(r, 2);
    r = co_await add(r, 3);
    r = co_await add(r, 4);
    r = co_await add(r, 5);
    co_return r;
}(1).get();
```

- Async code is almost the same as the linear code
- Easy to read, write, and debug

</div>
</div>

---

# Implementing `task<T>`

### How to use C++ coroutines?

1. Decide you need coroutines
2. Sit down and code you own coroutine library
3. Write thousands of lines of code
4. Debug concurrency errors
5. ???
6. Profit

### Reason:

- You can't simply use coroutines out of the box
- Downside: `task<T>` and the like have to be implemented by you
- Upside: `task<T>` and the like can be implemented in any way you want it

---

# Defining a coroutine

<div class="twocolumns">
<div>

### Coroutine body:
```c++
// Definition:
task<int> my_first_coro() {
    // Body here...
    co_return 1;
}

// Call
task<int> t = my_first_coro();
```

- This is how your code looks like

</div>
<div>

### Under the hood:
```c++
// Definition:
task<int> my_first_coro(
    typename task<int>::promise_type& promise
) {
    try {
        co_await promise.initial_suspend();
        // Body here...
        promise.return_value(1);
    }
    catch (...) {
        promise.unhandled_exception();
    }
final_suspend:
    co_await promise.final_suspend()
    delete &promise;
}

// Call
auto promise = new typename task<int>::promise_type;
task<int> t = promise->get_return_object();
my_first_coro(*promise);

```

- This is how the compiler interacts with your code
- You only care about this when implementing `task<T>`

</div>
</div>

---

# Making `task<T>` and `promise<T>`

- From the under-the-hood view, we can figure out the layout of both
- These are the minimum methods they have to implement

<div class="twocolumns">
<div>

### The `task<T>`

```c++
template <class T>
class task {
public:
    using promise_type = promise<T>
}
```

</div>
<div>

### The `promise<T>`

```c++
template <class T>
struct promise {
    task<int> get_return_object();
    auto initial_suspend() const noexcept;
    auto final_suspend() const noexcept;
    void return_value(T value) noexcept;
    void unhandled_exception() noexcept;
}
```

</div>
</div>

---

# Defining `promise<T>::get_return_object`

- This method returns the `task<T>` object
- The returned `task` should most likely know about the promise
- Thus we'll pass the `this` pointer to the `task`

```c++
template <class T>
task<T> promise<T>::get_return_object() {
    return task<T>(this);
}
```

---

# Defining `promise<T>::initial_suspend`

- Coroutines can essentially start suspended or start running
- The behavior is determined by `promise::initial_suspend`
- Remember how each coroutine body starts with `co_await promise.initial_suspend()`?
    - This can either suspend or continue the coroutine
- In our case, we want to always suspend the coroutine after starting:

```c++
template <class T>
auto promise<T>::initial_suspend() const noexcept {
    return std::suspend_always{};
}
```

- `std::suspend_always` is a helper class
- It can be `co_await`ed -- we'll soon see what that means

---

# Defining `promise<T>::final_suspend`

- When coroutines finish they have two options:
    - Suspend the coroutine: it's final state can be inspected
    - Continue the coroutine: this also destroys the coroutine
    - The behavior is determined by `promise::final_suspend`
    - This happens immediately after the `co_return` statement
- We want our coroutines to always suspend on finish
- This is because we want to retrieve the results
- If the coroutine is destroyed, we can't get the results anymore
    - Unless the coroutine forwards it before it's destroyed
    - But we won't take that approach for simplicity

```c++
template <class T>
auto promise<T>::final_suspend() const noexcept {
    return std::suspend_always{};
}
```

---

# Defining `promise<T>::return_value`

- The statement `co_return X;`
- Translates into `promise.return_value(X);`
- Essentially `return_value` is our chance to store the value returned by the coroutine
- We'll save it into a field of the `promise` object

```c++
template <class T>
auto promise<T>::return_value(T value) const noexcept {
    m_result = std::move(value);
}
```

For this, we need to modify the promise too by adding the `m_result` field:

```c++
template <class T>
struct promise {
    ...
    T get_result() { return std::move(m_result.value); }
    std::optional<T> m_result;
}
```

---

# Defining `promise<T>::unhandled_exception`

- This is called when instead of `co_return`, we exit the scope because of an exception
- We can call `std::current_exception` to store the exception
- Then later use `std::rethrow_exception` when someone tries to retrieve the results
- But for now, we'll just terminate the application:

```c++
template <class T>
auto promise<T>::unhandled_exception() const noexcept {
    std::terminate();
}
```