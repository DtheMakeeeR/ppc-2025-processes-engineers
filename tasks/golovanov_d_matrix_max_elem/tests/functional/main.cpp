#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "golovanov_d_matrix_max_elem//common/include/common.hpp"
#include "golovanov_d_matrix_max_elem//mpi/include/ops_mpi.hpp"
#include "golovanov_d_matrix_max_elem//seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace golovanov_d_matrix_max_elem {

class GolovanovDMatrixMaxElemFuncTest : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return test_param;
  }

 protected:
  void SetUp() override {
    
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_data_ = input_data_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if(output_data)
    {

    }
    return true;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
//ОБРАТИТЬ ВНИМАНИЕ
 private:
  InType input_data_;
};

namespace {

TEST_P(GolovanovDMatrixMaxElemFuncTest, TestTest1) {
  ExecuteTest(GetParam());
}
TEST_P(GolovanovDMatrixMaxElemFuncTest, TestTest2) {
  ExecuteTest(GetParam());
}
const std::vector<double> testVector = {0.1, -1.0};
const std::array<TestType, 1> kTestParam = {};

const auto kTestTasksList =
    std::tuple_cat(ppc::util::AddFuncTask<GolovanovDMatrixMaxElemMPI, InType>(kTestParam, PPC_SETTINGS_golovanov_d_matrix_max_elem),
                   ppc::util::AddFuncTask<GolovanovDMatrixMaxElemSEQ, InType>(kTestParam, PPC_SETTINGS_golovanov_d_matrix_max_elem));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = GolovanovDMatrixMaxElemFuncTest::PrintFuncTestName<GolovanovDMatrixMaxElemFuncTest>;

INSTANTIATE_TEST_SUITE_P(MatrixMaxElemFunTests, GolovanovDMatrixMaxElemFuncTest, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace golovanov_d_matrix_max_elem
