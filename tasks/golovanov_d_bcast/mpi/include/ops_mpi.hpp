#pragma once

#include "golovanov_d_bcast//common/include/common.hpp"
#include "task/include/task.hpp"
#include <mpi.h>

namespace golovanov_d_bcast {

class GolovanovDMatrixMaxElemMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit GolovanovDMatrixMaxElemMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
  int MY_Bcast(void *buffer, int count, MPI_Datatype datatype,
    int root, MPI_Comm comm)
};

}  // namespace golovanov_d_bcast
