#include "golovanov_d_radix_sort_odd_even_merge/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdio>
#include <utility>
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
  int rank = 0;
  int world_size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  std::vector<int> input_data;
  int original_size = 0;

  if (rank == 0) {
    input_data = GetInput();
    original_size = input_data.size();
  }

  MPI_Bcast(&original_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int padded_size = original_size;
  if (original_size % world_size != 0) {
    padded_size = original_size + (world_size - original_size % world_size);
  }
  int local_size = padded_size / world_size;

  if (rank == 0) {
    input_data.resize(padded_size, INT_MAX);
  }

  std::vector<std::pair<int, int>> comparators;
  if (rank == 0) {
    comparators = GenerateBatcherComparators(world_size);
  }

  size_t comparators_count = 0;
  if (rank == 0) {
    comparators_count = comparators.size();
  }

  MPI_Bcast(&comparators_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank != 0) {
    comparators.resize(comparators_count);
  }

  std::vector<int> flat_comparators(comparators_count * 2);

  if (rank == 0) {
    for (size_t i = 0; i < comparators_count; i++) {
      flat_comparators[2 * i] = comparators[i].first;
      flat_comparators[(2 * i) + 1] = comparators[i].second;
    }
  }

  MPI_Bcast(flat_comparators.data(), comparators_count * 2, MPI_INT, 0, MPI_COMM_WORLD);

  for (size_t i = 0; i < comparators_count; i++) {
    comparators[i] = {flat_comparators[2 * i], flat_comparators[(2 * i) + 1]};
  }

  std::vector<int> local_data(local_size);

  MPI_Scatter(input_data.data(), local_size, MPI_INT, local_data.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);

  RadixSortWithNegatives(local_data);

  BatcherMerge(comparators, rank, local_data);

  std::vector<int> sorted_data;

  if (rank == 0) {
    sorted_data.resize(padded_size);
  }

  MPI_Gather(local_data.data(), local_size, MPI_INT, sorted_data.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    sorted_data.resize(original_size);
  }
  sorted_data.resize(original_size);
  MPI_Bcast(sorted_data.data(), original_size, MPI_INT, 0, MPI_COMM_WORLD);
  GetOutput() = sorted_data;
  return true;
}

bool GolovanovDRadixSortOddEvenMergeMPI::PostProcessingImpl() {
  return true;
}

// radix sort

int GolovanovDRadixSortOddEvenMergeMPI::GetDigit(int num, int digit_place) {
  return (num / digit_place) % 10;
}

void GolovanovDRadixSortOddEvenMergeMPI::CountingSort(std::vector<int> &arr, int digit_place) {
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

void GolovanovDRadixSortOddEvenMergeMPI::RadixSort(std::vector<int> &arr) {
  if (arr.empty()) {
    return;
  }

  int max_num = *std::ranges::max_element(arr);

  for (int digit_place = 1; max_num / digit_place > 0; digit_place *= 10) {
    CountingSort(arr, digit_place);
  }
}

void GolovanovDRadixSortOddEvenMergeMPI::RadixSortWithNegatives(std::vector<int> &arr) {
  if (arr.empty()) {
    return;
  }

  std::vector<int> negatives;
  std::vector<int> non_negatives;

  for (size_t i = 0; i < arr.size(); i++) {
    if (arr[i] < 0) {
      negatives.push_back(-arr[i]);
    } else {
      non_negatives.push_back(arr[i]);
    }
  }

  RadixSort(negatives);
  RadixSort(non_negatives);

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
  for (size_t i = 0; i < non_negatives.size(); i++) {
    arr.push_back(non_negatives[i]);
  }
}

// bathcer net
void GolovanovDRadixSortOddEvenMergeMPI::AddComparator(std::vector<std::pair<int, int>> &comparators, int a, int b) {
  if (a < b) {
    comparators.emplace_back(a, b);
  } else if (b < a) {
    comparators.emplace_back(b, a);
  }
}

std::vector<std::pair<int, int>> GolovanovDRadixSortOddEvenMergeMPI::GenerateBatcherComparators(int num_processes) {
  std::vector<std::pair<int, int>> comparators;
  for (int p = 1; p < num_processes; p *= 2) {
    for (int k = p; k >= 1; k /= 2) {
      for (int j = k % p; j < num_processes - k; j += 2 * k) {
        for (int i = 0; i < std::min(k, num_processes - j - k); i++) {
          if ((j + i) / (p * 2) == (j + i + k) / (p * 2)) {
            comparators.emplace_back(j + i, j + i + k);
          }
        }
      }
    }
  }
  return comparators;
}

std::vector<int> GolovanovDRadixSortOddEvenMergeMPI::MergeTwoSorted(const std::vector<int> &a,
                                                                    const std::vector<int> &b) {
  std::vector<int> result;
  result.reserve(a.size() + b.size());

  size_t i = 0;
  size_t j = 0;

  while (i < a.size() && j < b.size()) {
    if (a[i] <= b[j]) {
      result.push_back(a[i]);
      i++;
    } else {
      result.push_back(b[j]);
      j++;
    }
  }

  while (i < a.size()) {
    result.push_back(a[i]);
    i++;
  }

  while (j < b.size()) {
    result.push_back(b[j]);
    j++;
  }

  return result;
}

void GolovanovDRadixSortOddEvenMergeMPI::BatcherMerge(std::vector<std::pair<int, int>> &comparators, int rank,
                                                      std::vector<int> &local_data) {
  for (size_t i = 0; i < comparators.size(); i++) {
    size_t local_size = local_data.size();
    std::vector<int> received_data(local_size);
    std::vector<int> temp_data(local_size);
    int a = comparators[i].first;
    int b = comparators[i].second;

    if (rank == a || rank == b) {
      int partner = (rank == a) ? b : a;

      MPI_Sendrecv(local_data.data(), local_size, MPI_INT, partner, 0, received_data.data(), local_size, MPI_INT,
                   partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      std::vector<int> merged = MergeTwoSorted(local_data, received_data);

      if (rank == std::min(a, b)) {
        std::copy(merged.begin(), merged.begin() + local_size, temp_data.begin());
      } else {
        std::copy(merged.begin() + local_size, merged.end(), temp_data.begin());
      }

      local_data = temp_data;
    }
  }
}
}  // namespace golovanov_d_radix_sort_odd_even_merge
