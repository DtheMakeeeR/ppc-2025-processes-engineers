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

#include "golovanov_d_bcast//common/include/common.hpp"
#include "golovanov_d_bcast//mpi/include/ops_mpi.hpp"
#include "golovanov_d_bcast//seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace golovanov_d_bcast {

class GolovanovDBcastFuncTest : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    std::string s1 = std::to_string(std::get<0>(test_param)) + "_";
    std::string s2 = std::to_string(std::get<1>(test_param)) + "_";
    std::string s3;
    if (std::get<2>(test_param) < 0) {
      s3 = "minus" + std::to_string((int)-std::get<2>(test_param));
    } else {
      s3 = std::to_string((int)std::get<2>(test_param));
    }
    return s1 + s2 + s3;
  }

 protected:
  double maximum;
  int maxPos;

  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());

    std::vector<double> tmpVector(0);
    int n = std::get<0>(params), m = std::get<1>(params);
    maximum = std::get<2>(params);
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        tmpVector.push_back(RandomDouble(-1000, maximum));
      }
    }
    maxPos = RandomInt(0, m * n);
    tmpVector[maxPos] = maximum;
    input_data_ = std::tuple<int, int, std::vector<double>>(n, m, tmpVector);
  }

  double RandomDouble(double min, double max) {
    return min + (double)rand() / RAND_MAX * (max - min);
  }
  int RandomInt(int min, int max) {
    return min + rand() % max;
  }
  bool CheckTestOutputData(OutType &output_data) final {
    if (output_data == maximum) {
      return true;
    }
    return false;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
  // ОБРАТИТЬ ВНИМАНИЕ
 private:
  InType input_data_;
};

namespace {

TEST_P(GolovanovDBcastFuncTest, TestTest1) {
  ExecuteTest(GetParam());
}
const std::array<TestType, 3> kTestParam = {TestType(5, 5, 10.0), TestType(5, 5, -10.0), TestType(5, 5, 0)};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<GolovanovDMatrixMaxElemMPI, InType>(kTestParam, PPC_SETTINGS_golovanov_d_bcast),
    ppc::util::AddFuncTask<GolovanovDMatrixMaxElemSEQ, InType>(kTestParam, PPC_SETTINGS_golovanov_d_bcast));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = GolovanovDBcastFuncTest::PrintFuncTestName<GolovanovDBcastFuncTest>;

INSTANTIATE_TEST_SUITE_P(MatrixMaxElemFunTests, GolovanovDBcastFuncTest, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace golovanov_d_bcast
