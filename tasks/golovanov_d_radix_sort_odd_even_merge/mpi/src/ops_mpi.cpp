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
  GetOutput() = false;
}

bool GolovanovDRadixSortOddEvenMergeMPI::ValidationImpl() {
  int n = std::get<1>(GetInput());
  auto size = static_cast<size_t>(n);
  return ((n > -1) && (std::get<2>(GetInput()).size() == size) && (std::get<3>(GetInput()).size() == size) &&
          (std::get<4>(GetInput()).size() == size) && !GetOutput());
}

bool GolovanovDRadixSortOddEvenMergeMPI::PreProcessingImpl() {
  int &index = std::get<0>(GetInput());
  int world_size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  if (index >= world_size || index < 0) {
    index = 0;
  }
  return true;
}

bool GolovanovDRadixSortOddEvenMergeMPI::RunImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int main_proc = std::get<0>(GetInput());
  int n = 0;
  if (rank == main_proc) {
    n = std::get<1>(GetInput());
  }
  MyBcast(&n, 1, MPI_INT, main_proc, MPI_COMM_WORLD);
  std::vector<int> v_int(n);
  std::vector<float> v_float(n);
  std::vector<double> v_double(n);
  if (rank == main_proc) {
    v_int = std::get<2>(GetInput());
    v_float = std::get<3>(GetInput());
    v_double = std::get<4>(GetInput());
  }
  MyBcast(v_int.data(), n, MPI_INT, main_proc, MPI_COMM_WORLD);
  MyBcast(v_float.data(), n, MPI_FLOAT, main_proc, MPI_COMM_WORLD);
  MyBcast(v_double.data(), n, MPI_DOUBLE, main_proc, MPI_COMM_WORLD);

  GetOutput() = true;
  return true;
}

bool GolovanovDRadixSortOddEvenMergeMPI::PostProcessingImpl() {
  return true;
}

int GolovanovDRadixSortOddEvenMergeMPI::MyBcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
 
}

int GolovanovDRadixSortOddEvenMergeMPI::getDigit(int num, int digitPlace) {
    return (num / digitPlace) % 10;
}

void GolovanovDRadixSortOddEvenMergeMPI::countingSort(std::vector<int>& arr, int digitPlace) {
    int n = arr.size();
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
    for (int i = 1; i < arr.size(); i++) {
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
    
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] < 0) {
            negatives.push_back(-arr[i]);
        } else {
            nonNegatives.push_back(arr[i]);
        }
    }
    
    radixSort(negatives);
    radixSort(nonNegatives);
    
    for (int i = 0; i < negatives.size() / 2; i++) {
        int temp = negatives[i];
        negatives[i] = negatives[negatives.size() - 1 - i];
        negatives[negatives.size() - 1 - i] = temp;
    }
    
    for (int i = 0; i < negatives.size(); i++) {
        negatives[i] = -negatives[i];
    }
    
    arr.clear();
    for (int i = 0; i < negatives.size(); i++) {
        arr.push_back(negatives[i]);
    }
    for (int i = 0; i < nonNegatives.size(); i++) {
        arr.push_back(nonNegatives[i]);
    }
}

}  // namespace golovanov_d_radix_sort_odd_even_merge
