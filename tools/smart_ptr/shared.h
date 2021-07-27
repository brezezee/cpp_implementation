#pragma once


// #include "count_block.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>

struct count_block {
    size_t shared_count = 0;
    size_t weak_count = 0;
};


template <typename T>
class shared_ptr {
private:
    T* raw_ptr = nullptr;
    count_block* p_count = nullptr;

    template<class Y> friend class shared_ptr;
public:
    shared_ptr() noexcept = default;

    // 原始指针T* 不能隐式转换
    explicit shared_ptr(T* ptr) : raw_ptr(ptr) {
        if (raw_ptr) {
            p_count = new count_block();
        }
    }

    shared_ptr(const shared_ptr& _r) noexcept : raw_ptr(_r.get()), p_count(_r.p_count) {
        if (p_count) {
            ++ p_count->shared_count;
        }
    }

    // 兼容可隐式转换的智能指针操作，比如将子类对象的智能指针赋值给父类智能指针
    template <class Cast_>
    shared_ptr(const shared_ptr<Cast_>& _r) noexcept 
        : raw_ptr(_r.raw_ptr),   
        p_count(_r.p_count) 
    {
        if (p_count) {
            ++ p_count->shared_count;
        }
    }

    // 移动构造
    shared_ptr(shared_ptr&& _r) noexcept {
        swap(_r);
        _r.reset();
    }

    void reset() noexcept {
        dec_count();
        raw_ptr = nullptr;
        p_count = nullptr;
    }

    void reset(T* ptr) {
        dec_count();
        raw_ptr = ptr;
        p_count = new count_block();
    }

    T* get() const {
        return raw_ptr;
    }

    T* operator ->() const noexcept {
        assert(raw_ptr != 0);
        return raw_ptr;
    }

    T& operator *() const noexcept {
        assert(raw_ptr != 0);
        return *raw_ptr;
    }

private:
    void swap(shared_ptr& _r) noexcept {
        using std::swap;
        swap(raw_ptr, _r.raw_ptr);
        swap(p_count, _r.p_count);
    }

    void dec_count() {
        if (!p_count) return ;

        -- p_count->shared_count;

        if (p_count->shared_count <= 0) {
            delete raw_ptr;
            raw_ptr = nullptr;
            if (p_count->weak_count <= 0) {
                delete p_count;
                p_count = nullptr;
            }
        }
    }
};
