#pragma once
#include <vector>

#include "golovanov_d_radix_sort_odd_even_merge/common/include/common.hpp"
#include "task/include/task.hpp"

namespace golovanov_d_radix_sort_odd_even_merge {

class GolovanovDRadixSortOddEvenMergeSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit GolovanovDRadixSortOddEvenMergeSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
  // radix sort
  static int GetDigit(int num, int digit_place);
  static void CountingSort(std::vector<int> &arr, int digit_place);
  static void RadixSort(std::vector<int> &arr);
  static void RadixSortWithNegatives(std::vector<int> &arr);
};

}  // namespace golovanov_d_radix_sort_odd_even_merge
