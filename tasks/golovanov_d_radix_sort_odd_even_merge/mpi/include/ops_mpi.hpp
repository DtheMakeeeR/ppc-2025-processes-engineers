#pragma once

#include <mpi.h>

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
  static int GetDigit(int num, int digitPlace);
  static void CountingSort(std::vector<int> &arr, int digitPlace);
  static void RadixSort(std::vector<int> &arr);
  static void RadixSortWithNegatives(std::vector<int> &arr);
  // batcher net
  static void AddComparator(std::vector<std::pair<int, int>> &comparators, int a, int b);
  static void MergeNet(const std::vector<int> &up, const std::vector<int> &down,
                       std::vector<std::pair<int, int>> &comparators);
  static void BatcherSortNet(const std::vector<int> &lines, std::vector<std::pair<int, int>> &comparators);
  static std::vector<std::pair<int, int>> GenerateBatcherComparators(int num_processes);
  static std::vector<int> MergeTwoSorted(const std::vector<int> &a, const std::vector<int> &b);
};

}  // namespace golovanov_d_radix_sort_odd_even_merge
