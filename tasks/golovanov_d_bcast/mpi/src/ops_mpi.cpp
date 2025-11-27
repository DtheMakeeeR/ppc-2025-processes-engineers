#include "golovanov_d_bcast//mpi/include/ops_mpi.hpp"



#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include "golovanov_d_bcast//common/include/common.hpp"


namespace golovanov_d_bcast {

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
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int index = std::get<0>(GetInput());
  int n = 0;
  if(rank == index)
  {
    n = std::get<1>(GetInput());
  }
  MY_Bcast(&n, 1, MPI_INT, rank, MPI_COMM_WORLD);
  std::vector<int> v_int(n);
  std::vector<float> v_float(n);
  std::vector<double> v_double(n);
  if(rank == index)
  {
    v_int = std::get<2>(GetInput);
    v_float = std::get<3>(GetInput);
    v_double = std::get<4>(GetInput);
  }
  MY_Bcast(&v_int, n, MPI_INT, rank, MPI_COMM_WORLD);
  MY_Bcast(&v_float, n, MPI_FLOAT, rank, MPI_COMM_WORLD);
  MY_Bcast(&v_double, n, MPI_DOUBLE, rank, MPI_COMM_WORLD);
  
  for(int i = 0; i < n; i++)
  {
    cout << "int proc " << rank << " [" << i << "]: " << v_int[i] << "\n";
    cout << "float proc " << rank << " [" << i << "]: " << v_float[i] << "\n";
    cout << "double proc " << rank << " [" << i << "]: " << v_double[i] << "\n"; 
  }
  
  return true;
}

bool GolovanovDMatrixMaxElemMPI::PostProcessingImpl() {
  return true;
}
int GolovanovDMatrixMaxElemMPI::MY_Bcast(void *buffer, int count, MPI_Datatype datatype,
    int root, MPI_Comm comm){
  int rank_loc = 0;
  MPI_Comm_rank(comm, &rank_loc);
  int world_size;
  MPI_Comm_size(comm, &world_size);
  if(rank_loc == 0)
  {
    rank_loc = root;
  }
  else if(rank_loc == root)
  {
    rank_loc = 0;
  }
  int rank_lvl = 2;
  if(rank_loc == 0)
  {
    MPI_Send(buffer, count, datatype, 1, 0, comm);
  }
  else if(rank_loc == 1)
  {
    MPI_Recv(buffer, count, datatype, 0, 0, comm);
  }
  else
  {
    rank_lvl = static_cast<int>(floor(log2(rank_loc))) + 1;
    int tmp = static_cast<int>pow(2, rank_lvl - 1);
    int rank_parent = rank_loc % tmp;
    MPI_Recv(buffer, count, datatype, rank_parent, 0, comm);
    rank_lvl = static_cast<int>pow(2, rank_lvl);
  }
  for(int i = rank_loc + rank_lvl; i < world_size; i*=2)
  {
    MPI_Send(buffer, count, datatype, i, 0, comm);
  }
  return MPI_SUCCESS;
}
}  // namespace golovanov_d_bcast
