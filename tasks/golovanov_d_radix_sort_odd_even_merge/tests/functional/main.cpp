#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <array>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "golovanov_d_radix_sort_odd_even_merge/common/include/common.hpp"
#include "golovanov_d_radix_sort_odd_even_merge/mpi/include/ops_mpi.hpp"
#include "golovanov_d_radix_sort_odd_even_merge/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace golovanov_d_radix_sort_odd_even_merge {

class GolovanovDRadixSortOddEvenMergeFuncTest : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    std::string s1 = std::get<0>(test_param);
    return s1;
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    res_ = std::vector(std::get<1>(params));
    input_data_ = res_;
    std::sort(res_.begin(), res_.end());
  }
  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == res_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  std::vector<int> res_;
};

namespace {

TEST_P(GolovanovDRadixSortOddEvenMergeFuncTest, TestTest1) {
  ExecuteTest(GetParam());
}
const std::array<TestType, 2> kTestParam = {std::make_tuple("some_test", std::vector<int>{1, -2, 13, -65}),
                                            std::make_tuple("another_test", std::vector<int>{1, -2, -31, 4})};
const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<GolovanovDRadixSortOddEvenMergeMPI, InType>(
                                               kTestParam, PPC_SETTINGS_golovanov_d_radix_sort_odd_even_merge),
                                           ppc::util::AddFuncTask<GolovanovDRadixSortOddEvenMergeSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_golovanov_d_radix_sort_odd_even_merge));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    GolovanovDRadixSortOddEvenMergeFuncTest::PrintFuncTestName<GolovanovDRadixSortOddEvenMergeFuncTest>;

INSTANTIATE_TEST_SUITE_P(BcastFunTests, GolovanovDRadixSortOddEvenMergeFuncTest, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace golovanov_d_radix_sort_odd_even_merge
