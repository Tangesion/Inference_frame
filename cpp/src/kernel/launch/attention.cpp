#include "kernel/launch/attention.h"

namespace toy::kernel::launch
{
    namespace kernel_cpu = toy::kernel::cpu;
    using UniquePtr = toy::runtime::Tensor::UniquePtr;
    using DataType = toy::runtime::Tensor::DataType;

    void attentionForward(UniquePtr &out, UniquePtr &query, UniquePtr &key, UniquePtr &value, UniquePtr &interAttn, const bool isPrefill, const cpu::AttentionType attentionType)
    {
        DataType dataTypeOut = out->getDataType();
        DataType dataTypeQuery = query->getDataType();
        DataType dataTypeKey = key->getDataType();
        DataType dataTypeValue = value->getDataType();
        DataType dataTypeInterAttn = interAttn->getDataType();
        try
        {
            CHECK_WITH_INFO(dataTypeOut == dataTypeQuery, "Data type of output and query must be the same");
            CHECK_WITH_INFO(dataTypeOut == dataTypeKey, "Data type of output and key must be the same");
            CHECK_WITH_INFO(dataTypeOut == dataTypeValue, "Data type of output and value must be the same");
            CHECK_WITH_INFO(dataTypeOut == dataTypeInterAttn, "Data type of output and interAttn must be the same");
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            std::exit(EXIT_FAILURE);
        }

        int64_t B = out->getShape().d[0];
        int64_t NH = out->getShape().d[1];
        int64_t H = key->getShape().d[2];
        int64_t D = out->getShape().d[3];

        switch (dataTypeOut)
        {
        case DataType::kFLOAT:
        {
            auto *outData = static_cast<float *>(out->data());
            auto *queryData = static_cast<float *>(query->data());
            auto *keyData = static_cast<float *>(key->data());
            auto *valueData = static_cast<float *>(value->data());
            auto *interAttnData = static_cast<float *>(interAttn->data());
            switch (attentionType)
            {
            case kernel_cpu::AttentionType::kAttentionOneThread:
                kernel_cpu::attentionForwardOneThread(outData, queryData, keyData, valueData, interAttnData, isPrefill, B, NH, H, D);
                break;
            case kernel_cpu::AttentionType::kAttentionMultiThread:
                kernel_cpu::attentionForwardMultiThread(outData, queryData, keyData, valueData, interAttnData, isPrefill, B, NH, H, D);
                break;
            }
            break;
        }

        default:
            break;
        }

        // auto *outData = toy::func::getData<DataType::kFLOAT>(out);
        // auto *queryData = toy::func::getData<DataType::kFLOAT>(query);
        // auto *keyData = toy::func::getData<DataType::kFLOAT>(key);
        // auto *valueData = toy::func::getData<DataType::kFLOAT>(value);
        // auto *interAttnData = toy::func::getData<DataType::kFLOAT>(interAttn);
        //
    }
} // namespace toy::kernel::launch