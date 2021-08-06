#ifndef REFLECT_H
#define REFLECT_H

#include <cxxabi.h>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>
#include <unordered_map>


namespace lar {

// declear in config.h
class Config;

// 单例类，管理所有注册进来的子类
template <typename... Targs> class ConfigManager {
public:
    virtual ~ConfigManager(){};

    static ConfigManager *Instance() {
        if (nullptr == mp_ConfigManager) {
            mp_ConfigManager = new ConfigManager();
        }
        return (mp_ConfigManager);
    }

    // 注册从名字 到 构造函数的映射  
    bool _register(const std::string &strTypeName,
                std::function<Config *(Targs &&... args)> pFunc) {
        if (nullptr == pFunc) {
            return (false);
        }
        std::string strRealTypeName = strTypeName;
      
        bool success =
            m_mapCreateFunction.insert(std::make_pair(strRealTypeName, pFunc))
                .second;
        return (success);
    }

    Config *Construct(const std::string &strTypeName, Targs &&... args) {
        
        auto iter = m_mapCreateFunction.find(strTypeName);
        if (iter == m_mapCreateFunction.end()) {
          std::cout << "Param: " << strTypeName << " is not registered, please check the input" << std::endl;
          return (nullptr);
        } else {
          return (iter->second(std::forward<Targs>(args)...));  
        }
    }

private:
    ConfigManager() { };
    static ConfigManager<Targs...> *mp_ConfigManager;      // 单例对象指针
    std::unordered_map<std::string, std::function<Config *(Targs &&...)>> m_mapCreateFunction;    // 名称到构造的映射
};

template <typename... Targs>
ConfigManager<Targs...> *ConfigManager<Targs...>::mp_ConfigManager = nullptr;   

// 每一个继承此类的子类都会传入子类和它的构造参数，此类负责将它的构造函数绑定到单例的工厂。
// 用于绑定具体类的构造函数 T ： 需要绑定的类， Targs： T类的构造函数参数
template <typename T, typename... Targs> class ConstructorBind {
public:
  struct Binder {        
      Binder() {
        char *szDemangleName = nullptr;
        std::string strTypeName;  
        szDemangleName =
            abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
        if (nullptr != szDemangleName) {
          strTypeName = szDemangleName;
          std::cout << strTypeName << std::endl;
          free(szDemangleName);
        }

        // 将构造函数注册到单例工厂中
        ConfigManager<Targs...>::Instance()->_register(strTypeName, CreateObject);   // 注册构造函数  
      }

	    inline void active() const {}
  };

    // 构造时 初始化静态成员
    ConstructorBind() { m_binder.active(); }
    virtual ~ConstructorBind() { };

    // 构造具体对象的单例
    static T *CreateObject(Targs &&... args) {  
        return new T(std::forward<Targs>(args)...);  
    }

    static Binder m_binder;
};

template <typename T, typename... Targs>
typename ConstructorBind<T, Targs...>::Binder
    ConstructorBind<T, Targs...>::m_binder;  


// 使用的时候通过构造函数的参数去调用单例Manager，找到该函数的构造并调用
template <typename... Args>
Config *CREATE_CONFIG(const std::string &strTypeName, Args &&... args) {
    Config *p = ConfigManager<Args...>::Instance()->Construct(
        strTypeName, std::forward<Args>(args)...);
    return (p);
}

} // namespace lar


#endif