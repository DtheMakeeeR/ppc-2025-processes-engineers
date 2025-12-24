#include "golovanov_d_radix_sort_odd_even_merge/seq/include/ops_seq.hpp"

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

int GolovanovDRadixSortOddEvenMergeSEQ::GetDigit(int num, int digitPlace) {
  return (num / digitPlace) % 10;
}

void GolovanovDRadixSortOddEvenMergeSEQ::CountingSort(std::vector<int> &arr, int digitPlace) {
  int n = static_cast<int>(arr.size());
  const int range = 10;
  std::vector<int> output(n);
  std::vector<int> count(range, 0);

  for (int i = 0; i < n; i++) {
    int digit = GetDigit(arr[i], digitPlace);
    count[digit]++;
  }

  for (int i = 1; i < range; i++) {
    count[i] += count[i - 1];
  }

  for (int i = n - 1; i >= 0; i--) {
    int digit = GetDigit(arr[i], digitPlace);
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

  int maxNum = arr[0];
  for (size_t i = 1; i < arr.size(); i++) {
    if (arr[i] > maxNum) {
      maxNum = arr[i];
    }
  }

  for (int digitPlace = 1; maxNum / digitPlace > 0; digitPlace *= 10) {
    CountingSort(arr, digitPlace);
  }
}

void GolovanovDRadixSortOddEvenMergeSEQ::RadixSortWithNegatives(std::vector<int> &arr) {
  if (arr.empty()) {
    return;
  }

  std::vector<int> negatives;
  std::vector<int> nonNegatives;

  for (size_t i = 0; i < arr.size(); i++) {
    if (arr[i] < 0) {
      negatives.push_back(-arr[i]);
    } else {
      nonNegatives.push_back(arr[i]);
    }
  }

  RadixSort(negatives);
  RadixSort(nonNegatives);

  for (size_t i = 0; i < negatives.size() / 2; i++) {
    int temp = negatives[i];
    negatives[i] = negatives[negatives.size() - 1 - i];
    negatives[negatives.size() - 1 - i] = temp;
  }

  for (size_t i = 0; i < negatives.size(); i++) {
    negatives[i] = -negatives[i];
  }

  arr.clear();
  for (size_t i = 0; i < negatives.size(); i++) {
    arr.push_back(negatives[i]);
  }
  for (size_t i = 0; i < nonNegatives.size(); i++) {
    arr.push_back(nonNegatives[i]);
  }
}

}  // namespace golovanov_d_radix_sort_odd_even_merge
