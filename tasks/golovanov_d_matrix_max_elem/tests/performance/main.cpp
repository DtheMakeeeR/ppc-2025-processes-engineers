#include <gtest/gtest.h>

#include "golovanov_d_matrix_max_elem//common/include/common.hpp"
#include "golovanov_d_matrix_max_elem//mpi/include/ops_mpi.hpp"
#include "golovanov_d_matrix_max_elem//seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace golovanov_d_matrix_max_elem {

class GolovanovDMatrixMaxElemPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
 private:
  InType input_data_{};
  double maximum = 100000;

 public:
  void SetUp() override {
    std::vector<double> tmpVector(0);
    int n = 2000, m = 2000;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        double value = i + j;
        tmpVector.push_back(value);
      }
    }
    tmpVector[10] = maximum;
    input_data_ = std::tuple<int, int, std::vector<double>>(n, m, tmpVector);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if (output_data == maximum) {
      return true;
    }
    std::cout << "output data " << output_data;
    return false;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(GolovanovDMatrixMaxElemPerfTest, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, GolovanovDMatrixMaxElemMPI, GolovanovDMatrixMaxElemSEQ>(
    PPC_SETTINGS_golovanov_d_matrix_max_elem);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = GolovanovDMatrixMaxElemPerfTest::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(MatrixMaxElemPerTests, GolovanovDMatrixMaxElemPerfTest, kGtestValues, kPerfTestName);

}  // namespace golovanov_d_matrix_max_elem
