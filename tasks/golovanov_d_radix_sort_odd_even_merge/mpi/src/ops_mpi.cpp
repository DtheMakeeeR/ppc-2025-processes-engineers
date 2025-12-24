#include "golovanov_d_radix_sort_odd_even_merge/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <climits>
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
  int rank, world_size;
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

  int comparators_count = 0;
  if (rank == 0) {
    comparators_count = comparators.size();
  }

  MPI_Bcast(&comparators_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank != 0) {
    comparators.resize(comparators_count);
  }

  std::vector<int> flat_comparators(comparators_count * 2);

  if (rank == 0) {
    for (int i = 0; i < comparators_count; i++) {
      flat_comparators[2 * i] = comparators[i].first;
      flat_comparators[2 * i + 1] = comparators[i].second;
    }
  }

  MPI_Bcast(flat_comparators.data(), comparators_count * 2, MPI_INT, 0, MPI_COMM_WORLD);

  for (int i = 0; i < comparators_count; i++) {
    comparators[i] = {flat_comparators[2 * i], flat_comparators[2 * i + 1]};
  }

  std::vector<int> local_data(local_size);

  MPI_Scatter(input_data.data(), local_size, MPI_INT, local_data.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);

  RadixSortWithNegatives(local_data);

  std::vector<int> received_data(local_size);
  std::vector<int> temp_data(local_size);

  for (const auto &comparator : comparators) {
    int a = comparator.first;
    int b = comparator.second;

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

int GolovanovDRadixSortOddEvenMergeMPI::GetDigit(int num, int digitPlace) {
  return (num / digitPlace) % 10;
}

void GolovanovDRadixSortOddEvenMergeMPI::CountingSort(std::vector<int> &arr, int digitPlace) {
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

void GolovanovDRadixSortOddEvenMergeMPI::RadixSort(std::vector<int> &arr) {
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

void GolovanovDRadixSortOddEvenMergeMPI::RadixSortWithNegatives(std::vector<int> &arr) {
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

// bathcer net
void GolovanovDRadixSortOddEvenMergeMPI::AddComparator(std::vector<std::pair<int, int>> &comparators, int a, int b) {
  if (a < b) {
    comparators.push_back({a, b});
  } else if (b < a) {
    comparators.push_back({b, a});
  }
}

void GolovanovDRadixSortOddEvenMergeMPI::MergeNet(const std::vector<int> &up, const std::vector<int> &down,
                                                  std::vector<std::pair<int, int>> &comparators) {
  int total_size = up.size() + down.size();

  if (total_size <= 1) {
    return;
  }

  if (total_size == 2) {
    AddComparator(comparators, up[0], down[0]);
    return;
  }

  std::vector<int> up_odd, down_odd, up_even, down_even;

  for (size_t i = 0; i < up.size(); i++) {
    if (i % 2 == 0) {
      up_odd.push_back(up[i]);
    } else {
      up_even.push_back(up[i]);
    }
  }

  for (size_t i = 0; i < down.size(); i++) {
    if (i % 2 == 0) {
      down_odd.push_back(down[i]);
    } else {
      down_even.push_back(down[i]);
    }
  }

  MergeNet(up_odd, down_odd, comparators);
  MergeNet(up_even, down_even, comparators);

  std::vector<int> all_lines;
  all_lines.reserve(up.size() + down.size());

  for (int line : up) {
    all_lines.push_back(line);
  }

  for (int line : down) {
    all_lines.push_back(line);
  }

  for (size_t i = 1; i + 1 < all_lines.size(); i += 2) {
    AddComparator(comparators, all_lines[i], all_lines[i + 1]);
  }
}

void GolovanovDRadixSortOddEvenMergeMPI::BatcherSortNet(const std::vector<int> &lines,
                                                        std::vector<std::pair<int, int>> &comparators) {
  int size = lines.size();

  if (size <= 1) {
    return;
  }

  int up_size = size / 2;
  int down_size = size - up_size;

  std::vector<int> up(up_size);
  std::vector<int> down(down_size);

  for (int i = 0; i < up_size; i++) {
    up[i] = lines[i];
  }

  for (int i = 0; i < down_size; i++) {
    down[i] = lines[up_size + i];
  }

  BatcherSortNet(up, comparators);
  BatcherSortNet(down, comparators);

  MergeNet(up, down, comparators);
}

std::vector<std::pair<int, int>> GolovanovDRadixSortOddEvenMergeMPI::GenerateBatcherComparators(int num_processes) {
  std::vector<std::pair<int, int>> comparators;
  std::vector<int> process_ids(num_processes);

  for (int i = 0; i < num_processes; i++) {
    process_ids[i] = i;
  }

  BatcherSortNet(process_ids, comparators);
  return comparators;
}

std::vector<int> GolovanovDRadixSortOddEvenMergeMPI::MergeTwoSorted(const std::vector<int> &a,
                                                                    const std::vector<int> &b) {
  std::vector<int> result;
  result.reserve(a.size() + b.size());

  size_t i = 0, j = 0;

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

}  // namespace golovanov_d_radix_sort_odd_even_merge
