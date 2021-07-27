#pragma once

#include <algorithm>

#include "count_block.h"

template <typename T>
class weak_ptr {
private:
    T* raw_ptr = nullptr;
    count_block* p_count = nullptr;

    template <class Y> friend class shared_ptr;

public:
    weak_ptr() noexcept = default;

    weak_ptr(const weak_ptr& rhs) noexcept 
        : raw_ptr(rhs.raw_ptr)
        , p_count(rhs.p_count) 
    {
        if (p_count) {
            ++ p_count->weak_count;
        }
    }

    weak_ptr(const shared_ptr<T>& s_ptr) noexcept 
        : raw_ptr(s_ptr.raw_ptr)
        , p_count(s_ptr.p_count) 
    {
        if (p_count) {
            ++ p_count->weak_count;
        }
    }

    weak_ptr(weak_ptr&& rhs) noexcept {
        swap(rhs);
        rhs.reset();
    }

    ~weak_ptr() {
        dec_count();
    }

    weak_ptr& operator= (const weak_ptr& rhs) noexcept {
        weak_ptr tmp(rhs);
        swap(tmp);
        return *this;
    }

    weak_ptr& operator= (weak_ptr&& rhs) noexcept {
        swap(rhs);
        rhs.reset();
        return *this;
    }

    shared_ptr<T> lock() const {
        if (!p_count || p_count->shared_count <= 0) {
            return shared_ptr<T>(nullptr);
        }
        return shared_ptr<T>(*this);  // 用weak_ptr对象去构造
    }

    size_t use_count() {
        assert(p_count);
        return p_count->weak_count;
    }

    void reset() noexcept {
        dec_count();
        raw_ptr = nullptr;
        p_count = nullptr;
    }
private:
    void swap(weak_ptr& rhs) noexcept {
        using std::swap;
        swap(raw_ptr, rhs.raw_ptr);
        swap(p_count, rhs.p_count);
    }

    void dec_count() {
        if (!p_count) return ;

        -- p_count->weak_count;

        if (p_count->shared_count <= 0 && p_count->weak_count <= 0) {
            delete p_count;
            p_count = nullptr;
        }
    }
};
