#include "golovanov_d_radix_sort_odd_even_merge/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

#include "golovanov_d_radix_sort_odd_even_merge/common/include/common.hpp"
// #include "util/include/util.hpp" clang-tidy попросил отключить

namespace golovanov_d_radix_sort_odd_even_merge {

GolovanovDRadixSortOddEvenMergeSEQ::GolovanovDRadixSortOddEvenMergeSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool GolovanovDRadixSortOddEvenMergeSEQ::ValidationImpl() {
  return true;
}

bool GolovanovDRadixSortOddEvenMergeSEQ::PreProcessingImpl() {
  return true;
}
bool GolovanovDRadixSortOddEvenMergeSEQ::RunImpl() {
  std::vector<int> working_vector = GetInput();
  RadixSortWithNegatives(working_vector);
  GetOutput() = working_vector;
  return true;
}

bool GolovanovDRadixSortOddEvenMergeSEQ::PostProcessingImpl() {
  return true;
}

// radix sort

int GolovanovDRadixSortOddEvenMergeSEQ::GetDigit(int num, int digit_place) {
  return (num / digit_place) % 10;
}

void GolovanovDRadixSortOddEvenMergeSEQ::CountingSort(std::vector<int> &arr, int digit_place) {
  int n = static_cast<int>(arr.size());
  const int range = 10;
  std::vector<int> output(n);
  std::vector<int> count(range, 0);

  for (int i = 0; i < n; i++) {
    int digit = GetDigit(arr[i], digit_place);
    count[digit]++;
  }

  for (int i = 1; i < range; i++) {
    count[i] += count[i - 1];
  }

  for (int i = n - 1; i >= 0; i--) {
    int digit = GetDigit(arr[i], digit_place);
    output[count[digit] - 1] = arr[i];
    count[digit]--;
  }

  for (int i = 0; i < n; i++) {
    arr[i] = output[i];
  }
}

void GolovanovDRadixSortOddEvenMergeSEQ::RadixSort(std::vector<int> &arr) {
  if (arr.empty()) {
    return;
  }

  int max_num = *std::ranges::max_element(arr);

  for (int digit_place = 1; max_num / digit_place > 0; digit_place *= 10) {
    CountingSort(arr, digit_place);
  }
}

void GolovanovDRadixSortOddEvenMergeSEQ::RadixSortWithNegatives(std::vector<int> &arr) {
  if (arr.empty()) {
    return;
  }

  std::vector<int> negatives;
  std::vector<int> non_negatives;

  for (int value : arr) {
    if (value < 0) {
      negatives.push_back(-value);
    } else {
      non_negatives.push_back(value);
    }
  }

  RadixSort(negatives);
  RadixSort(non_negatives);

  for (size_t i = 0; i < negatives.size() / 2; i++) {
    int temp = negatives[i];
    negatives[i] = negatives[negatives.size() - 1 - i];
    negatives[negatives.size() - 1 - i] = temp;
  }

  for (int &negative : negatives) {
    negative = -negative;
  }

  arr.clear();
  for (int negative : negatives) {
    arr.push_back(negative);
  }
  for (int non_negative : non_negatives) {
    arr.push_back(non_negative);
  }
}

}  // namespace golovanov_d_radix_sort_odd_even_merge
