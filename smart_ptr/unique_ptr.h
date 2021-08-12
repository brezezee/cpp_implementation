#pragma once 

#include <algorithm>
#include <cassert>

template <typename T>
class unique_ptr {
private:
    T* raw_ptr;

public:
    unique_ptr() noexcept = default;

    explicit unique_ptr(T* ptr) : raw_ptr(ptr) { }

    // 移动构造
    unique_ptr(unique_ptr&& rhs) noexcept : raw_ptr(rhs.raw_ptr) {
        rhs.raw_ptr = nullptr;
    }

    // 移动赋值
    unique_ptr& operator =(unique_ptr&& rhs) noexcept {
        reset(rhs);
        rhs.raw_ptr = nullptr; // 右值的指针置空
        return *this;
    }

    ~unique_ptr() {
        delete raw_ptr;
        raw_ptr = nullptr;
    }

    T* get() const noexcept {
        return raw_ptr;
    }

    T* operator ->() const noexcept {
        assert(raw_ptr);
        return raw_ptr;
    }

    T& operator *() const noexcept {
        assert(*raw_ptr);
        return *raw_ptr;
    }

    explicit operator bool() const noexcept {
        return raw_ptr != nullptr;
    }

    T* release() noexcept {
        T* tmp = raw_ptr;
        raw_ptr = nullptr;
        return tmp;
    }

    void reset(T* ptr = nullptr) noexcept {
        delete raw_ptr;
        raw_ptr = ptr;
    }

private:
    unique_ptr(const unique_ptr& ) = delete;

    unique_ptr& operator =(const unique_ptr& ) = delete;

};
