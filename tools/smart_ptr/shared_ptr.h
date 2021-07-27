#pragma once


#include "count_block.h"

#include <algorithm>
#include <cassert>

// 前向申明
template <class T> class weak_ptr;

template <typename T>
class shared_ptr {
private:
    T* raw_ptr = nullptr;
    count_block* p_count = nullptr;

    template<class Cast_> friend class shared_ptr;  // 
    template<class Y> friend class weak_ptr;
public:
    shared_ptr() noexcept = default;

    // 原始指针T* 不能隐式转换
    explicit shared_ptr(T* ptr) : raw_ptr(ptr) {
        if (raw_ptr) {
            p_count = new count_block();
        }
    }

    shared_ptr(const shared_ptr& rhs) noexcept : raw_ptr(rhs.raw_ptr), p_count(rhs.p_count) {
        if (p_count) {
            ++ p_count->shared_count;
        }
    }

    // 兼容可隐式转换的智能指针操作，比如将子类对象的智能指针赋值给父类智能指针
    template <class Cast_>
    shared_ptr(const shared_ptr<Cast_>& rhs) noexcept 
        : raw_ptr(rhs.raw_ptr),   
        p_count(rhs.p_count) 
    {
        if (p_count) {
            ++ p_count->shared_count;
        }
    }

    explicit shared_ptr(const weak_ptr<T>& w_ptr) noexcept 
        : raw_ptr(w_ptr.raw_ptr)
        , p_count(w_ptr.p_count)
    {
        if (p_count) {
            ++ p_count->shared_count;
        }    
    }

    // 移动构造
    shared_ptr(shared_ptr&& rhs) noexcept {
        swap(rhs);
        rhs.reset();
    }

    ~shared_ptr() {
        printf("~shared\n");
        dec_count();
    }

    // 赋值
    shared_ptr& operator= (const shared_ptr& rhs) noexcept {
        shared_ptr tmp(rhs);
        swap(tmp);
        return *this;
    }

    // 移动赋值
    shared_ptr& operator= (shared_ptr&& rhs) noexcept {
        swap(rhs);
        rhs.reset();
        return *this;
    }

    T* operator-> () const noexcept {
        assert(raw_ptr != 0);
        return raw_ptr;
    }

    T& operator* () const noexcept {
        assert(raw_ptr != 0);
        return *raw_ptr;
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

    size_t use_count() {
        assert(p_count);
        return p_count->shared_count;
    }


private:
    void swap(shared_ptr& rhs) noexcept {
        using std::swap;
        swap(raw_ptr, rhs.raw_ptr);
        swap(p_count, rhs.p_count);
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


template<class T, class U> inline bool operator ==(shared_ptr<T> const& a, shared_ptr<U> const& b) noexcept {
    return a.get() == b.get();
}
