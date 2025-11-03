#include "golovanov_d_matrix_max_elem//mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>

#include <float.h>

#include "golovanov_d_matrix_max_elem//common/include/common.hpp"
#include "util/include/util.hpp"

namespace golovanov_d_matrix_max_elem {

GolovanovDMatrixMaxElemMPI::GolovanovDMatrixMaxElemMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool GolovanovDMatrixMaxElemMPI::ValidationImpl() {
  size_t columns = std::get<0>(GetInput());
  size_t strokes = std::get<1>(GetInput());
  return (columns > 0) && (strokes > 0) && (std::get<2>(GetInput()).size() == strokes * columns) && (GetOutput()==0);
}

bool GolovanovDMatrixMaxElemMPI::PreProcessingImpl() {
  return true;
}

bool GolovanovDMatrixMaxElemMPI::RunImpl() {
  int rank, processes;
  size_t n = 0;
  if(MPI_Comm_rank(MPI_COMM_WORLD, &rank) != MPI_SUCCESS)
  {
    std::cout << "Comm_rank Error!";
    return false;
  };
  if(MPI_Comm_size(MPI_COMM_WORLD, &processes) != MPI_SUCCESS)
  {
    std::cout << "Comm_size Error!";
    return false;
  };
  std::vector<double> elems;
  double answer;
  if(rank == 0)
  {
    auto columns = std::get<0>(GetInput());
    auto strokes = std::get<1>(GetInput());
    elems = std::get<2>(GetInput());
    auto count = columns*strokes;
    n = count/processes;
    if (count%processes != 0)
    {
       for (size_t i = 0; i < processes - count%processes; i++)
       {
        elems.push_back(elems[0]);
       } 
       n++;
    }
    
  }
  if(MPI_Bcast(&n, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD)!= MPI_SUCCESS)
  {
    std::cout << "Bcast Error!";
    return false;
  }
  std::vector<double> workVector(n); 
  if(MPI_Scatter(elems.data(), n, MPI_DOUBLE, workVector.data(), n, MPI_DOUBLE, 0, MPI_COMM_WORLD)!= MPI_SUCCESS)
  {
    std::cout << "Scatter Error!";
    return false;
  }
  double max = DBL_MIN;
  for(size_t i = 0; i < n; i++)
  { 
    if(workVector[i] > max)
    {
      max = workVector[i];
    }
  }
  if(MPI_Reduce(&max, &answer, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD)!= MPI_SUCCESS)
  {
    std::cout << "Reduce Error!";
    return false;
  }
  if(rank == 0) 
  {
    GetOutput() = answer;
  }
  return true;
}

bool GolovanovDMatrixMaxElemMPI::PostProcessingImpl() {
  return true;
}

}  // namespace golovanov_d_matrix_max_elem
