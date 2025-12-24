# Введение
В задании 19 предлагается оптимизировать поразрядную сортировку (Radix sort), используя библиотеку MPI и алгоритм четно-нечетого слияния Бэтчера.
# Постановка задачи
Необходимо написать реализацию Radix sort, а также алгоритм слияния Бэтчера.
Ограничения:
* Входные данные - вектор целых чисел.
* В векторе могут быть отрицательные числа.
* Выходные данные - отсортированный по неубыванию вектор целых чисел.
# Описание алгоритма
## Radix sort
Функция RadixSort находит максимальное число, чтобы знать, сколько максимум разрядов смотреть. Далее в цикле вызывается функция CountingSort.
## CountingSort
* Считает распределение цифр в текущем разряде.
* Преобразует счётчики в префиксные суммы для определения позиций.
* Распределяет элементы по временному вектору в отсортированном порядке по текущему разряду
* Перезаписывает исходный вектор.
## Radix sort с отрицательными числами
Вызывает отдельно RadixSort для отрицательных и положительных элементов вектора. 
* В вектор для отрицательных чисел записываются их модули. 
* Вектор с отсортированными "отрицательными" числами переворачивается, а элементы в нём умножаются на -1.
* Два вектора сливаются в один.
## Алгоритм слияния Бэтчера (Сеть Бэтчера)
Используется алгоритм построения очереди обменов между процессами - Сеть Бэтчера. Для этого используется вектор компараторов. Согласно получившейся очереди, процессы будут обмениваться между собой упорядоченными подмассивами, сливать их в один упорядоченный массив, и один из процессов будет оставлять себе меньшую половину, а второй большую.
Для создания очереди используются функция GenerateBatcherComparators.
## GenerateBatcherComparators
* Используется схема представленная на Википедии(ссылка в источниках)
```
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
```
* p - размер "блока"(уровень рекурсии). На первом уровне (p=1) сортируем пары элементов. На втором (p=2) сливаем отсортированные пары в четверки.
* k - расстояние между сравниваемыми элементами.
* j - начало группы сравнений.
* i используется для выбора пары элементов.
## BactcherMerge
Процессы внутри функции проходят по вектору компараторов, находят партнёров для обмена и осуществляют его. Для обмена используется Sendrecv.
И для RadixSort и для сети Бэтчера написаны дополнительыне вспомогательные функции. Полный код в приложении.
# Схема распараллеливания
* Нулевой процесс получает вектор для сортировки. Изменяет размер, чтобы все процессы получили поровну. Генерирует компараторы. Рассылает другим процессам вектор, размер, компараторы.
* Процессы сортируют подмассивы с использованием RadixSort.
* Процессы проходят по очереди в цикле и выполняют обмены согласно компораторам(BacthcerMerge).
* Полностью отсортированный вектор собирается на нулевом процессе и потом рассылается другим процессам.
# Экспериментальные результаты

| Режим выполнения | Количество процессов | Время выполнения (сек) | Ускорение | Эффективность |
|------------------|---------------------|------------------------|-----------|---------------|
| **Последовательный (SEQ)** | | | | |
| pipeline | 1 | 0.5976 | 1.00x | — |
| task_run | 1 | 0.5949 | 1.00x | — |
| **MPI (2 процесса)** | | | | |
| pipeline | 2 | 0.3672 | 1.63x | 82% |
| task_run | 2 | 0.3696 | 1.61x | 81% |
| **MPI (3 процесса)** | | | | |
| pipeline | 3 | 0.4196 | 1.42x | 47% |
| task_run | 3 | 0.3933 | 1.51x | 50% |
| **MPI (4 процесса)** | | | | |
| pipeline | 4 | 0.3166 | 1.89x | 47% |
| task_run | 4 | 0.2910 | 2.04x | 51% |
| **MPI (5 процессов)** | | | | |
| pipeline | 5 | 0.3105 | 1.93x | 39% |
| task_run | 5 | 0.3530 | 1.69x | 34% |
| **MPI (6 процессов)** | | | | |
| pipeline | 6 | 0.3684 | 1.62x | 27% |
| task_run | 6 | 0.4172 | 1.43x | 24% |

## Тестовые данные

Для тестов на производительность использовались вектора на 10 миллионов элементов, расположенных в обратном порядке.

## Окружение
| Параметр       | Значение                                          |
| -------------- | ------------------------------------------------- |
| **OS**         | Windows 11 Pro 25H2                               |
| **CPU**        | AMD Ryzen 5 5600X (6 cores, 12 threads, 3.70 GHz) |
| **RAM**        | 16 GB DDR4 3400 МГц                               |
| **Компилятор** | MSVC 14.43.34808                                  |
| **Версия MPI** | Microsoft MPI 10.1.12498.52                       |

# Результаты
Успешно реализована гибридная параллельная сортировка на основе Radix Sort и сети Бэтчера. Алгоритм корректно обрабатывает массивы целых чисел, включая отрицательные значения.
# Источники
https://habr.com/ru/articles/275889/?mobile=no - статья на сайте Хабр, на которую опирался. 
https://en.wikipedia.org/wiki/Batcher_odd–even_mergesort#cite_note-4 - статья на Википедии со схемой расчета индексов слияния Бэтчера.
# Приложение

## Код параллельной версии
### RadixSort
```
void GolovanovDRadixSortOddEvenMergeMPI::RadixSort(std::vector<int> &arr) {
  if (arr.empty()) {
    return;
  }

  int max_num = *std::ranges::max_element(arr);

  for (int digit_place = 1; max_num / digit_place > 0; digit_place *= 10) {
    CountingSort(arr, digit_place);
  }
}
```
### RadixSortWithNegatives
```
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
```
### CountingSort
```
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
```

### GetDigit
```
int GolovanovDRadixSortOddEvenMergeMPI::GetDigit(int num, int digit_place) {
  return (num / digit_place) % 10;
}
```

## Сеть Бэтчера

### AddComparator
```
void GolovanovDRadixSortOddEvenMergeMPI::AddComparator(std::vector<std::pair<int, int>> &comparators, int a, int b) {
  if (a < b) {
    comparators.emplace_back(a, b);
  } else if (b < a) {
    comparators.emplace_back(b, a);
  }
}
```
### GenerateBatcherComparators
```
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
```
### MergeTwoSorted
```
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
```
### BatcherMerge
```
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
```