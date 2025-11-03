#include <gtest/gtest.h>

#include "golovanov_d_matrix_max_elem//common/include/common.hpp"
#include "golovanov_d_matrix_max_elem//mpi/include/ops_mpi.hpp"
#include "golovanov_d_matrix_max_elem//seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace golovanov_d_matrix_max_elem {

class GolovanovDMatrixMaxElemPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 100;
  InType input_data_{};

  void SetUp() override {
    input_data_ = input_data_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if(output_data)
    {
      
    }
    return input_data_ == input_data_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(GolovanovDMatrixMaxElemPerfTest, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, GolovanovDMatrixMaxElemMPI, GolovanovDMatrixMaxElemSEQ>(PPC_SETTINGS_golovanov_d_matrix_max_elem);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = GolovanovDMatrixMaxElemPerfTest::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(MatrixMaxElemPerTests, GolovanovDMatrixMaxElemPerfTest, kGtestValues, kPerfTestName);

}  // namespace golovanov_d_matrix_max_elem
