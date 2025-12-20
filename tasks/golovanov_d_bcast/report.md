# Введение
В задании 1 предлагается реализовать свою версию функции MPI_Bcast с построением дерева процессов.

# Постановка задачи
Необходимо реализовать функцию широковещательной рассылки с построением дерева процессов.
Ограничения:
* Функция должна уметь передавать int, float, double;
* Должна быть возможность выбора корневого процесса.
# Описание алгоритма
Сначала процессу присваивается внутренний номер, у корневого процесса внутренний корень равен 0.
Далее высчитываются номера родителя и потомков процесса. Для этого алгоритм использует логарифмические и степенные функции. По сути так строится дерево процессов. В функции MPI_Recv, MPI_Send передаются реальные номера процессов.
# Схема распараллеливания
В параллельной программе процесс, который был указан как корневой, есдинственный считывает тестовые данные, остальные процессы только вызывают учебную версию MPI_Bcast и ждут посылки.
Полный код реализации в приложении.
# Экспериментальные результаты
Эксперимент не проводился. В последовательной реализации написана заглушка.

# Результаты

Требуемая функция успешно реализована и проходит функциональные и производительные тесты. Однако сравнение с оригинальной функцией не проводилось.
# Источники
1. https://mpitutorial.com/tutorials/mpi-broadcast-and-collective-communication/ - статья, на которую я опирался.
## Код параллельной версии

```
static int GolovanovDBcastMPI::MyBcast(void *buffer, int count, MPI_Datatype datatype,
    int root, MPI_Comm comm){
  int real_rank = 0;
  MPI_Comm_rank(comm, &real_rank);
  int world_size;
  MPI_Comm_size(comm, &world_size);
  int local_rank = (real_rank - root + world_size) % world_size;
  int rank_lvl = 0;
  //корень отправляет первому
  if(local_rank == 0)
  {
    if(world_size > 1)
    {
      rank_lvl = 1;
      int local_child = 1; 
      int real_child = (root + local_child) % world_size;
      MPI_Send(buffer, count, datatype, real_child, 0, comm);
    }
  }
  //не-корень получает впервые
  else 
  {
    rank_lvl = static_cast<int>(floor(log2(local_rank))) + 1;
    int parent_offset = static_cast<int>(pow(2, rank_lvl - 1));
    int local_parent = local_rank - parent_offset;
    int real_parent = (root + local_parent) % world_size;
    MPI_Recv(buffer, count, datatype, real_parent, 0, comm, MPI_STATUS_IGNORE);
  }
  //расслыка
  int local_child = local_rank + static_cast<int>(pow(2, rank_lvl));
  while(local_child < world_size)
  {
    int real_child = (root + local_child) % world_size;
    MPI_Send(buffer, count, datatype, real_child, 0, comm);
    rank_lvl++;
    local_child = local_rank + static_cast<int>(pow(2, rank_lvl));
  }
  return MPI_SUCCESS;
}

```
