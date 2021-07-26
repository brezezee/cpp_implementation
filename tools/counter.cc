#include <iostream>
#include <stdio.h>

template <typename T>
class Counter {
public:
    Counter() { ++ _count; }
        
    Counter(const Counter&) { ++ _count; }

    // 并未定义为virtual，因为它是一个空类且目的仅用于计数，继承时使用private可防止使用该基类指针去操作子类
    // 非virtual可使得编译器产生空类优化代码，不会给子类带来内存和效率负担
    ~Counter() { -- _count; }         

    static size_t getCount() {
        return _count;
    }
private:
    static size_t _count;
};

template<typename T>
size_t 
Counter<T>::_count = 0;


// private继承 防止使用计数类指针
class Base : private Counter<Base> {
public:
    using Counter<Base>::getCount;   // 需前向申明Counter的计数接口

    Base() {}
    virtual ~Base() {}

};

class Derived : public Base, 
                private Counter<Derived> {
public:
    using Counter<Derived>::getCount;

    Derived() {}
    ~Derived() {}
};


using namespace std;

int main() {
    // Counter<Base>* p = new Base;        // 由private继承，屏蔽了该操作，不用担心错误使用
    // cout << p->getCount() << endl;
    // delete p;
    Base a;
    cout << a.getCount() << endl;
    Derived d;
    cout << d.getCount() << endl;

    cout << sizeof(Counter<Base>) << endl;  // Counter类本身是一个空类，它具有一个占位字节
    cout << sizeof(Base) << endl;           // Base继承了Counter，由空类优化，Counter的内存不会影响Base的大小
    cout << sizeof(Derived) << endl;
    getchar();
    return 0;
}