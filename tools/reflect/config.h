#ifndef LAR_CONFIG_H
#define LAR_CONFIG_H

#include "reflect.h"
#include <vector>

namespace lar {

class Config {
public:
    Config():vaild(false) {}
    virtual ~Config(){};
    virtual void put() {}
    bool isvaild() {return vaild;}
    void setvaild(bool val) { vaild = val;}
    const virtual std::vector<std::string>& get_dependence() {
        return dependence;
    }
protected:
    bool vaild;
    std::vector<std::string> dependence;
};



// config 
class multithread : public Config, public ConstructorBind<multithread> {
public:
    multithread() {std::cout << "multithread created." << std::endl;}
    void put() {
        std::cout << "multithread exectued, dependence: " << std::endl;
    }

    //static const std::vector<std::string> dependence;
};

//const std::vector<std::string> multithread::dependence = {};


class test : public Config, public ConstructorBind<test> {
public:
    test() {
        std::cout << "test created." << std::endl;
        dependence =  {"multithread", "profile"};
    }
    void put() {
        std::cout << "test executed, dependence: multithread, profile" << std::endl;
    }
    const virtual std::vector<std::string>& get_dependence() {
        return dependence;
    }
    //static const std::vector<std::string> dependence;
};

//const std::vector<std::string> test::dependence = {"multithread", "profile"};


class profile : public Config, public ConstructorBind<profile, const std::string&> {
public:
    
    profile(const std::string &inpath) : path(inpath){
        dependence = {"multithread"};
        std::cout << "profile created." << std::endl;
        //std::cout << "path: " << path << std::endl;
    }
    void put() {
        std::cout << "profile executed, dependence: multithread" << std::endl;
    }

    
private:
    std::string path;
     //const std::vector<std::string> dependence = {"multithread"};
};

//const std::vector<std::string> profile::dependence = {"multithread"};




} // namespace lar


#endif