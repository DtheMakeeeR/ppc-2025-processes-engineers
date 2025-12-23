#include "golovanov_d_radix_sort_odd_even_merge/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cmath>
#include <cstdio>
#include <vector>

#include "golovanov_d_radix_sort_odd_even_merge/common/include/common.hpp"

namespace golovanov_d_radix_sort_odd_even_merge {

GolovanovDRadixSortOddEvenMergeMPI::GolovanovDRadixSortOddEvenMergeMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool GolovanovDRadixSortOddEvenMergeMPI::ValidationImpl() {
  return true;
}

bool GolovanovDRadixSortOddEvenMergeMPI::PreProcessingImpl() {
  return true;
}

bool GolovanovDRadixSortOddEvenMergeMPI::RunImpl() {
  
  return true;
}

bool GolovanovDRadixSortOddEvenMergeMPI::PostProcessingImpl() {
  return true;
}

//radix sort

int GolovanovDRadixSortOddEvenMergeMPI::getDigit(int num, int digitPlace) {
    return (num / digitPlace) % 10;
}

void GolovanovDRadixSortOddEvenMergeMPI::countingSort(std::vector<int>& arr, int digitPlace) {
    int n = static_cast<int>(arr.size());
    const int range = 10; 
    std::vector<int> output(n);
    std::vector<int> count(range, 0);
    
    for (int i = 0; i < n; i++) {
        int digit = getDigit(arr[i], digitPlace);
        count[digit]++;
    }
    
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }
    
    for (int i = n - 1; i >= 0; i--) {
        int digit = getDigit(arr[i], digitPlace);
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }
    
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}

void GolovanovDRadixSortOddEvenMergeMPI::radixSort(std::vector<int>& arr) {
    if (arr.empty()) return;
    
    int maxNum = arr[0];
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] > maxNum) {
            maxNum = arr[i];
        }
    }
    
    for (int digitPlace = 1; maxNum / digitPlace > 0; digitPlace *= 10) {
        countingSort(arr, digitPlace);
    }
}

void GolovanovDRadixSortOddEvenMergeMPI::radixSortWithNegatives(std::vector<int>& arr) {
    if (arr.empty()) return;
    
    std::vector<int> negatives;
    std::vector<int> nonNegatives;
    
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] < 0) {
            negatives.push_back(-arr[i]);
        } else {
            nonNegatives.push_back(arr[i]);
        }
    }
    
    radixSort(negatives);
    radixSort(nonNegatives);
    
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
