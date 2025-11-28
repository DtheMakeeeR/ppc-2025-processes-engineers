#include <gtest/gtest.h>

#include "golovanov_d_bcast//common/include/common.hpp"
#include "golovanov_d_bcast//mpi/include/ops_mpi.hpp"
#include "golovanov_d_bcast//seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace golovanov_d_bcast {

class GolovanovDBcastPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
 private:
  InType input_data_{};
  double maximum = 100000;

 public:
  void SetUp() override {
    
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if(output_data) return true;
    return false;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(GolovanovDBcastPerfTest, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, GolovanovDBcastMPI, GolovanovDBcastMPI>(
    PPC_SETTINGS_golovanov_d_bcast);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = GolovanovDBcastPerfTest::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(BcastPerfTests, GolovanovDBcastPerfTest, kGtestValues, kPerfTestName);

}  // namespace golovanov_d_bcast
