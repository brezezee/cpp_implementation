#pragma once

#include <algorithm>

template <typename T>
class enable_shared_from_this {
private:
    mutable weak_ptr<T> _weak_ptr;

protected:
    enable_shared_from_this() {}

    enable_shared_from_this(enable_shared_from_this const&) {}

    enable_shared_from_this& operator =(enable_shared_from_this const&) {
        return *this;
    }

    ~enable_shared_from_this() {}

public:
    shared_ptr<T> shared_from_this() {
        shared_ptr<T> p(_weak_ptr);
        assert(p.get() == this);
        return this;
    }

    shared_ptr<T const> shared_from_this() const {

    }

public:

};



https://blog.csdn.net/Z1042300201/article/details/105640691https://blog.csdn.net/Z1042300201/article/details/105640691
