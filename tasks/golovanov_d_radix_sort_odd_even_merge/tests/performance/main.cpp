#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "golovanov_d_radix_sort_odd_even_merge/common/include/common.hpp"
#include "golovanov_d_radix_sort_odd_even_merge/mpi/include/ops_mpi.hpp"
#include "golovanov_d_radix_sort_odd_even_merge/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace golovanov_d_radix_sort_odd_even_merge {

class GolovanovDRadixSortOddEvenMergePerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
 private:
  InType input_data_;
  std::vector<int> res_;
  int count_ = 10000001;

 public:
  void SetUp() override {
    std::vector<int> v_int;
    v_int.reserve(static_cast<size_t>(count_));

    for (int i = count_ - 1; i >= 0; --i) {
      v_int.emplace_back(i);
    }

    std::vector<int> sorted_copy(v_int);
    std::ranges::sort(sorted_copy);

    res_.swap(sorted_copy);

    input_data_ = std::move(v_int);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == res_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(GolovanovDRadixSortOddEvenMergePerfTest, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, GolovanovDRadixSortOddEvenMergeMPI, GolovanovDRadixSortOddEvenMergeSEQ>(
        PPC_SETTINGS_golovanov_d_radix_sort_odd_even_merge);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = GolovanovDRadixSortOddEvenMergePerfTest::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(BcastPerfTests, GolovanovDRadixSortOddEvenMergePerfTest, kGtestValues, kPerfTestName);

}  // namespace golovanov_d_radix_sort_odd_even_merge
