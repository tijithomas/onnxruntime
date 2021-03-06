// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "precomp.h"

namespace Dml
{

class DmlOperatorGather : public DmlOperator, public GatherHelper
{
public:
    DmlOperatorGather(const MLOperatorKernelCreationContext& kernelCreationContext)
    :   DmlOperator(kernelCreationContext),
        GatherHelper(kernelCreationContext, kernelCreationContext.GetTensorShapeDescription())
    {
        ML_CHECK_VALID_ARGUMENT(kernelCreationContext.GetInputCount() == 2, "Gather expects 2 inputs.");
        ML_CHECK_VALID_ARGUMENT(kernelCreationContext.GetOutputCount() == 1, "Gather expects 1 output.");

        DmlOperator::Initialize(kernelCreationContext);

        std::vector<DML_TENSOR_DESC> inputDescs = GetDmlInputDescs();
        std::vector<DML_TENSOR_DESC> outputDescs = GetDmlOutputDescs();
        assert(inputDescs.size() == 2);
        assert(outputDescs.size() == 1);

        m_inputTensorDescs[1].ForceUnsignedDataType();

        auto outputTensorShapeDescription = kernelCreationContext.GetTensorShapeDescription();;
        std::vector<DimensionType> dataDimensions = outputTensorShapeDescription.GetInputTensorShape(0);
        std::vector<DimensionType> indicesDimensions = outputTensorShapeDescription.GetInputTensorShape(1);
        ML_CHECK_VALID_ARGUMENT(dataDimensions.size() <= OperatorHelper::NchwDimensionCount);
        uint32_t dmlAxis = GetDmlAdjustedAxis(m_axis, kernelCreationContext, m_inputTensorDescs.front().GetDimensionCount());

        DML_GATHER_OPERATOR_DESC operatorDesc = {};
        operatorDesc.InputTensor = &inputDescs[0];
        operatorDesc.IndicesTensor = &inputDescs[1];
        operatorDesc.OutputTensor = outputDescs.data();
        operatorDesc.Axis = dmlAxis;
        operatorDesc.IndexDimensions = gsl::narrow_cast<uint32_t>(indicesDimensions.size());

        DML_OPERATOR_DESC opDesc = { DML_OPERATOR_GATHER, &operatorDesc };
        SetDmlOperatorDesc(opDesc, kernelCreationContext);
    }
};

DML_OP_DEFINE_CREATION_FUNCTION(Gather, DmlOperatorGather);

} // namespace Dml
