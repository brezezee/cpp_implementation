#ifndef LAR_MODEL_H
#define LAR_MODEL_H

#include "config.h"
#include <string>
#include <stdint.h>
#include <memory>
#include <vector>

using namespace std;

namespace lar {


struct Args {
//     struct COprArgs {
//         //! for run c opr
//         bool is_run_c_opr = false;
//         bool is_run_c_opr_with_param = false;
//         typedef void (*COPR_PARAM_DEVICE_PTR_MEM_T)(ExternCOprParam* param);
//         typedef void (*COPR_PARAM_DEVICE_PTR_H2D_T)(
//                 ExternCOprParam* param, void* host_ptr,
//                 size_t extern_device_tensor_id);
//         COPR_PARAM_DEVICE_PTR_MEM_T copr_param_device_ptr_malloc = nullptr;
//         COPR_PARAM_DEVICE_PTR_MEM_T copr_param_device_ptr_free = nullptr;
//         COPR_PARAM_DEVICE_PTR_H2D_T copr_param_device_ptr_h2d = nullptr;
//     };

//     COprArgs c_opr_args;

//     bool disable_assert_throw = false;
//     bool share_param_mem = false;
// #if MGB_ENABLE_FASTRUN
//     bool use_full_run = false;
//     bool use_fast_run = false;
// #endif
//     bool reproducible = false;
//     std::string fast_run_cache_path;
//     bool copy_to_host = false;
//     int nr_run = 10;
//     int nr_warmup = 1;
//     int nr_thread = 1;
//     int multithread_number = 1;
//     size_t workspace_limit = SIZE_MAX;
//     std::vector<std::string> data_files;
//     serialization::GraphLoader::LoadResult load_ret;
// #if MGB_ENABLE_JSON
//     std::unique_ptr<GraphProfiler> profiler;
// #endif
//     std::string profiler_output;
//     std::string bin_out_dump;

//     std::unique_ptr<OprIODumpBase> iodump;
//     std::unique_ptr<NumRangeChecker> num_range_checker;
//     std::unique_ptr<CPUDispatchChecker> cpu_dispatch_checker;
//     std::unique_ptr<VarValueChecker> var_value_checker;
//     serialization::GraphLoader::LoadConfig load_config;
//     thin_function<void(size_t)> affinity_cb;
};

class Model {
public:
    Model() {}
    virtual bool run(Args& env);
};

class Modelmdl :public Model {
public:
    Modelmdl();
    bool run(Args& env);
};

class Modellite : public Model {
public:
    Modellite();
    bool run(Args& env);
};

class ModelManager {
public:
    ModelManager(){}
    ModelManager(std::string&path);  // check model type and set model_format

    //Model* read_model(GraphDumpFormat);
    Model* read_model(std::string&);

    bool parse(std::vector<std::vector<std::string> >& );

    bool get_dependence();
    
    bool toposort( ) ;

    void run_before();  // for (auto config : dependence_before)

    void run() ;

    void run_after();


public:
    //GraphDumpFormat model_format;
    std::string model_path;
    Args env;
    std::vector<std::shared_ptr<Config>> inst_config;
    std::vector<std::shared_ptr<Config>> dependence_before;
    std::vector<std::shared_ptr<Config>> dependence_after;
};



};


#endif
