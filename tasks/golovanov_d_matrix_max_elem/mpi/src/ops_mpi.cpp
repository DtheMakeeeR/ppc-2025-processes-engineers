#include "golovanov_d_matrix_max_elem//mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <vector>
#include <iostream>
#include "golovanov_d_matrix_max_elem//common/include/common.hpp"

namespace golovanov_d_matrix_max_elem {

GolovanovDMatrixMaxElemMPI::GolovanovDMatrixMaxElemMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 1234;
}

bool GolovanovDMatrixMaxElemMPI::ValidationImpl() {
  int columns = std::get<0>(GetInput());
  int strokes = std::get<1>(GetInput());
  return (columns > 0) && (strokes > 0) && (std::get<2>(GetInput()).size() == static_cast<size_t>(strokes * columns)) &&
         (GetOutput() == 1234);
}

bool GolovanovDMatrixMaxElemMPI::PreProcessingImpl() {
  return true;
}

bool GolovanovDMatrixMaxElemMPI::RunImpl() {
  int rank = 0;
  int processes = 0;
  int n = 0;
  if (MPI_Comm_rank(MPI_COMM_WORLD, &rank) != MPI_SUCCESS) {
    std::cout << "Comm_rank Error!\n";
    return false;
  }
  if (MPI_Comm_size(MPI_COMM_WORLD, &processes) != MPI_SUCCESS) {
    std::cout << "Comm_size Error!\n";
    return false;
  }
  std::vector<double> elems;
  double answer = -1234;
  if (rank == 0) {
    auto columns = std::get<0>(GetInput());
    auto strokes = std::get<1>(GetInput());
    elems = std::get<2>(GetInput());
    auto count = columns * strokes;

    n = count / processes;
    if (count % processes != 0) {
      for (int i = 0; i < processes - count % processes; i++) {
        elems.push_back(elems[0]);
      }
      n++;
    }
    std::cout << "\ncount: " << count << " n: " << n << "\n";
  }
  if (MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD) != MPI_SUCCESS) {
    std::cout << "Bcast Error!\n";
    return false;
  }
  std::vector<double> work_vector(n);
  if (MPI_Scatter(elems.data(), n, MPI_DOUBLE, work_vector.data(), n, MPI_DOUBLE, 0, MPI_COMM_WORLD) != MPI_SUCCESS) {
    std::cout << "Scatter Error!\n";
    return false;
  }
  double max = *std::max_element(work_vector.begin(), work_vector.end());
  std::cout << "proc: " << rank << " localMax: " << max << "\n";
  if (MPI_Allreduce(&max, &answer, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD) != MPI_SUCCESS) {
    std::cout << "Reduce Error!";
    return false;
  }
  GetOutput() = answer;
  std::cout << "proc: " << rank << " globalMax after Reduce: " << answer << " GetOutput: " << GetOutput() << "\n";
  return true;
}

bool GolovanovDMatrixMaxElemMPI::PostProcessingImpl() {
  return true;
}

}  // namespace golovanov_d_matrix_max_elem
