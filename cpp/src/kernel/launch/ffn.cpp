#include <iostream>
#include "kernel/cpu/ffn.h"
#include "runtime/tensor.h"
#include "func/func.h"

namespace toy::kernel::launch
{
    namespace kernel_cpu = toy::kernel::cpu;
    using UniquePtr = toy::runtime::Tensor::UniquePtr;
    using DataType = toy::runtime::Tensor::DataType;
    using MemoryType = toy::runtime::MemoryType;

    void ffnForwardOneThread(UniquePtr &out, UniquePtr &inp, UniquePtr &gateProj, UniquePtr &upProj, UniquePtr &downProj)
    {
        DataType dataTypeOut = out->getDataType();
        DataType dataTypeInp = inp->getDataType();
        DataType dataTypeGateProj = gateProj->getDataType();
        DataType dataTypeUpProj = upProj->getDataType();
        DataType dataTypeDownProj = downProj->getDataType();
        try
        {
            CHECK_WITH_INFO(dataTypeOut == dataTypeInp, "Data type of output and input must be the same");
            CHECK_WITH_INFO(dataTypeOut == dataTypeGateProj, "Data type of output and gateProj must be the same");
            CHECK_WITH_INFO(dataTypeOut == dataTypeUpProj, "Data type of output and upProj must be the same");
            CHECK_WITH_INFO(dataTypeOut == dataTypeDownProj, "Data type of output and downProj must be the same");
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            std::exit(EXIT_FAILURE);
        }

        int64_t B = inp->getShape().d[0];
        int64_t H = inp->getShape().d[1];
        int64_t C = inp->getShape().d[2];
        int64_t interSize = gateProj->getShape().d[0];

        switch (dataTypeOut)
        {
        case DataType::kFLOAT:
        {
            auto *outData = static_cast<float *>(out->data());
            auto *inpData = static_cast<float *>(inp->data());
            auto *gateProjData = static_cast<float *>(gateProj->data());
            auto *upProjData = static_cast<float *>(upProj->data());
            auto *downProjData = static_cast<float *>(downProj->data());
            auto outInterGateTensor = toy::func::createTensor({B, H, interSize}, DataType::kFLOAT, MemoryType::kCPU);
            auto outInterUpTensor = toy::func::createTensor({B, H, interSize}, DataType::kFLOAT, MemoryType::kCPU);
            auto *outInterGate = static_cast<float *>(outInterGateTensor->data());
            auto *outInterUp = static_cast<float *>(outInterUpTensor->data());
            // std::cout << "ffnForwardOneThread" << std::endl;

            kernel_cpu::ffnForwardOneThread(outData, inpData, outInterGate, outInterUp, gateProjData, upProjData, downProjData, B, H, C, interSize);
            break;
        }

        default:
            break;
        }
    }

    void ffnForwardMultiThreads(UniquePtr &out, UniquePtr &inp, UniquePtr &gateProj, UniquePtr &upProj, UniquePtr &downProj)
    {
        DataType dataTypeOut = out->getDataType();
        DataType dataTypeInp = inp->getDataType();
        DataType dataTypeGateProj = gateProj->getDataType();
        DataType dataTypeUpProj = upProj->getDataType();
        DataType dataTypeDownProj = downProj->getDataType();
        try
        {
            CHECK_WITH_INFO(dataTypeOut == dataTypeInp, "Data type of output and input must be the same");
            CHECK_WITH_INFO(dataTypeOut == dataTypeGateProj, "Data type of output and gateProj must be the same");
            CHECK_WITH_INFO(dataTypeOut == dataTypeUpProj, "Data type of output and upProj must be the same");
            CHECK_WITH_INFO(dataTypeOut == dataTypeDownProj, "Data type of output and downProj must be the same");
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            std::exit(EXIT_FAILURE);
        }

        int64_t B = inp->getShape().d[0];
        int64_t H = inp->getShape().d[1];
        int64_t C = inp->getShape().d[2];
        int64_t interSize = gateProj->getShape().d[0];

        switch (dataTypeOut)
        {
        case DataType::kFLOAT:
        {
            auto *outData = static_cast<float *>(out->data());
            auto *inpData = static_cast<float *>(inp->data());
            auto *gateProjData = static_cast<float *>(gateProj->data());
            auto *upProjData = static_cast<float *>(upProj->data());
            auto *downProjData = static_cast<float *>(downProj->data());
            auto outInterGateTensor = toy::func::createTensor({B, H, interSize}, DataType::kFLOAT, MemoryType::kCPU);
            auto outInterUpTensor = toy::func::createTensor({B, H, interSize}, DataType::kFLOAT, MemoryType::kCPU);
            auto *outInterGate = static_cast<float *>(outInterGateTensor->data());
            auto *outInterUp = static_cast<float *>(outInterUpTensor->data());
            kernel_cpu::ffnForwardMultiThread(outData, inpData, outInterGate, outInterUp, gateProjData, upProjData, downProjData, B, H, C, interSize);
            break;
        }
        default:
            break;
        }
    }

    void ffnForward(UniquePtr &out, UniquePtr &inp, UniquePtr &gateProj, UniquePtr &upProj, UniquePtr &downProj, const bool isMultiThread = true)
    {
        if (isMultiThread)
        {
            ffnForwardMultiThreads(out, inp, gateProj, upProj, downProj);
        }
        else
        {
            ffnForwardOneThread(out, inp, gateProj, upProj, downProj);
        }
    }

}