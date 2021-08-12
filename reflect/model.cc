#include "model.h"
#include <queue>
#include <unordered_map>
#include <memory>

namespace lar {


Model* ModelManager::read_model(std::string& modelpath) {
    
}

bool ModelManager::parse(std::vector<std::vector<std::string> >& config_list) {
    std::string prefix = "lar::";
    for (const auto& param : config_list) { 
        if (param.size() == 1) {
            inst_config.push_back(
                std::shared_ptr<Config>(lar::CREATE_CONFIG(prefix + param[0]))
            );
        }else if (param.size() == 2) {
            inst_config.push_back(
                std::shared_ptr<Config>(lar::CREATE_CONFIG(prefix + param[0], param[1]))
            );
        }
    }
    return true;
}

bool ModelManager::toposort() {
    std::unordered_map<std::string, std::vector<std::string>> graph;
    std::unordered_map<std::string, int> degree;
    std::queue<std::string> que;
    std::unordered_map<std::string, std::shared_ptr<Config>> str2inst;

    for (auto item : inst_config) {
        std::string vecStrname 
            = abi::__cxa_demangle(typeid(*item).name(), nullptr, nullptr, nullptr);
        vecStrname = vecStrname.substr(5);
        str2inst[vecStrname] = item;

        auto dependences = item->get_dependence();
        degree[vecStrname] = dependences.size();  // 
        //cout << vecStrname << ": " ;
        for (const std::string &depend : dependences) {
            graph[depend].push_back(vecStrname);
            //cout << depend << ' ' << degree[depend] << ' ';
        }
    }
    
    for (auto iter = degree.begin(); iter != degree.end(); ++ iter) {
        cout << iter->first << ':' << iter->second << endl;
        if (!iter->second) que.push(iter->first);
    }

    while (que.size()) {
        std::string cur = que.front();
        cout << cur << endl;
        que.pop();
        dependence_before.push_back(str2inst[cur]);

        for (auto vetex : graph[cur]) {
            -- degree[vetex];
            if (!degree[vetex]) que.push(vetex);
        }
    }
    return true;
}


void ModelManager::run_before() {
    for (auto it : dependence_before) {
        it->put();
    }
}





} // namespace lar


