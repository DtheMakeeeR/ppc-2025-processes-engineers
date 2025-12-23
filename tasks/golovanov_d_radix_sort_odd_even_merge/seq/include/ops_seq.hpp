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
  //radix sort
  static int getDigit(int num, int digitPlace);
  static void countingSort(std::vector<int>& arr, int digitPlace);
  static void radixSort(std::vector<int>& arr);
  static void radixSortWithNegatives(std::vector<int>& arr);
};

}  // namespace golovanov_d_radix_sort_odd_even_merge
