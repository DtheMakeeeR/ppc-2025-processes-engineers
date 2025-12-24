#pragma once

#include <utility>
#include <vector>

#include "golovanov_d_radix_sort_odd_even_merge/common/include/common.hpp"
#include "task/include/task.hpp"
namespace golovanov_d_radix_sort_odd_even_merge {

class GolovanovDRadixSortOddEvenMergeMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit GolovanovDRadixSortOddEvenMergeMPI(const InType &in);

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
  // batcher net
  static void AddComparator(std::vector<std::pair<int, int>> &comparators, int a, int b);
  static std::vector<std::pair<int, int>> GenerateBatcherComparators(int num_processes);
  static std::vector<int> MergeTwoSorted(const std::vector<int> &a, const std::vector<int> &b);
  static void BatcherMerge(std::vector<std::pair<int, int>> &comparators, int rank, std::vector<int> &local_data);
};

}  // namespace golovanov_d_radix_sort_odd_even_merge
