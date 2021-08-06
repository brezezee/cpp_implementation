#include "config.h"
#include "model.h"

using namespace std;

int main() {

    vector<vector<string>> config_list= {
        {"multithread"},
        {"test"},
        {"profile", "tmppath"}
    };

    lar::ModelManager mm;
    // lar::Config *p1 = lar::CREATE_CONFIG(std::string("lar::multithread"));
    
    // p1->put();
    // lar::Config *p2 = lar::CREATE_CONFIG(std::string("lar::profile"), std::string("iampath"));
    // p2->put();
    // lar::Config *p3 = lar::CREATE_CONFIG(std::string("lar::test"));
    // cout << endl;

    // mm.inst_config.push_back(std::shared_ptr<lar::Config>(p1));
    // mm.inst_config.push_back(std::shared_ptr<lar::Config>(p2));
    // mm.inst_config.push_back(std::shared_ptr<lar::Config>(p3));
    
    mm.parse(config_list);
    mm.toposort();

    cout << "sort finish\n" << endl;

    mm.run_before();

    return 0;
}