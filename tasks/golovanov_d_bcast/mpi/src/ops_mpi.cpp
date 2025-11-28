#include "golovanov_d_bcast//mpi/include/ops_mpi.hpp"



#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include "golovanov_d_bcast//common/include/common.hpp"


namespace golovanov_d_bcast {

GolovanovDBcastMPI::GolovanovDBcastMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = true;
}

bool GolovanovDBcastMPI::ValidationImpl() {
  //int index = std::get<0>(GetInput());
  int n = std::get<1>(GetInput());
  return (n > -1) && ((std::get<2>(GetInput()).size() == std::get<3>(GetInput()).size()) == (std::get<4>(GetInput()).size() == static_cast<size_t>(n))) &&
         (GetOutput() == true);
}

bool GolovanovDBcastMPI::PreProcessingImpl() {
  return true;
}

bool GolovanovDBcastMPI::RunImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int main_proc = std::get<0>(GetInput());
  int n = 0;
  if(rank == main_proc)
  {
    std::cout << "proc " << rank << " is main " << "\n"; 
    n = std::get<1>(GetInput());
  }
  std::cout << "proc " << rank << " is alive" << "\n"; 
  MY_Bcast(&n, 1, MPI_INT, main_proc, MPI_COMM_WORLD);
  std::vector<int> v_int(n);
  std::vector<float> v_float(n);
  std::vector<double> v_double(n);
  if(rank == main_proc)
  {
    v_int = std::get<2>(GetInput());
    v_float = std::get<3>(GetInput());
    v_double = std::get<4>(GetInput());
  }
  MY_Bcast(v_int.data(), n, MPI_INT, main_proc, MPI_COMM_WORLD);
  MY_Bcast(v_float.data(), n, MPI_FLOAT, main_proc, MPI_COMM_WORLD);
  MY_Bcast(v_double.data(), n, MPI_DOUBLE, main_proc, MPI_COMM_WORLD);
  
  for(int i = 0; i < n; i++)
  {
    std::cout << "int proc " << rank << " [" << i << "]: " << v_int[i] << "\n";
    std::cout << "float proc " << rank << " [" << i << "]: " << v_float[i] << "\n";
    std::cout << "double proc " << rank << " [" << i << "]: " << v_double[i] << "\n"; 
  }
  GetOutput() = true;
  std::cout << "proc " << rank << " exit task" << "\n";
  return true;
}

bool GolovanovDBcastMPI::PostProcessingImpl() {
  return true;
}
int GolovanovDBcastMPI::MY_Bcast(void *buffer, int count, MPI_Datatype datatype,
    int root, MPI_Comm comm){
  int rank_loc = 0;
  MPI_Comm_rank(comm, &rank_loc);
  std::cout << "proc " << rank_loc << " enter Bcast" << "\n"; 
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
    std::cout << "proc " << rank_loc << " to " << 1 << "\n"; 
    MPI_Send(buffer, count, datatype, 1, 0, comm);
    std::cout << "proc " << rank_loc << " sended to " << 1 << "\n"; 
  }
  else if(rank_loc == 1)
  {
    std::cout << "proc " << rank_loc << " from " << 1 << "\n"; 
    MPI_Recv(buffer, count, datatype, 0, 0, comm, MPI_STATUS_IGNORE);
    std::cout << "proc " << rank_loc << " getted from " << 1 << "\n"; 
  }
  else
  {
    rank_lvl = static_cast<int>(floor(log2(rank_loc))) + 1;
    int tmp = static_cast<int>(pow(2, rank_lvl - 1));
    int rank_parent = rank_loc % tmp;
    if(rank_parent == 0) 
    {
      rank_parent = root;
    }
    else if(rank_parent == root)
    {
      rank_parent = 0;
    }
    std::cout << "proc " << rank_loc << " from " << rank_parent << "\n"; 
    MPI_Recv(buffer, count, datatype, rank_parent, 0, comm, MPI_STATUS_IGNORE);
    std::cout << "proc " << rank_loc << " getted from " << rank_parent << "\n"; 
    rank_lvl = static_cast<int>(pow(2, rank_lvl));
  }
  for(int i = rank_loc + rank_lvl; i < world_size; i*=2)
  {
    std::cout << "proc " << rank_loc << " to " << i << "\n"; 
    MPI_Send(buffer, count, datatype, i, 0, comm);
    std::cout << "proc " << rank_loc << " sended to " << i << "\n"; 
  }
  std::cout << "proc " << rank_loc << " exit Bcast "<< "\n"; 
  return MPI_SUCCESS;
}
}  // namespace golovanov_d_bcast
