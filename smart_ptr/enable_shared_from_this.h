#pragma once

#include <algorithm>

template <typename T>
class enable_shared_from_this {
public:
    shared_ptr<T> shared_from_this() {
        shared_ptr<T> p(weak_this_);
        assert(p.get() == this);
        return p;
    }

    shared_ptr<T const> shared_from_this() {
        shared_ptr<T const> p(weak_this_);
        assert(p.get() == this);
        return p;
    }

    // shared ptr 调用
    template<class X, class Y> void _internal_accept_owner( shared_ptr<X> const * sppx, Y * py ) const
    {
        if( weak_this_.expired() )
        {
            weak_this_ = shared_ptr<T>( *sppx, py );
        }
    }

protected:
    enable_shared_from_this() {}

    enable_shared_from_this(enable_shared_from_this const&) {}

    enable_shared_from_this& operator =(enable_shared_from_this const&) {
        return *this;
    }

    ~enable_shared_from_this() {}

private:
    mutable weak_ptr<T> weak_this_;
};


