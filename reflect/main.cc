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
    
    mm.parse(config_list);
    
    mm.toposort();

    mm.run_before();

    return 0;
}