# Реализация линейного слоя
Реализация линейного слоя для нейронных сетей при помощи C++ Extensions и Pytorch на языке C++.
Написано 3 версии для вычисления линейного преобразования: на обычном C++, при помощи SSE и при помощи AVX.

## Запуск программы
По умолчанию будет запускаться SSE версия линейного слоя. Для выбора другой функции необходимо изменить параметр функции flag_immin в файле cpp/dense.py в функции forward класса DenseFunction.
  - 0 - на обычном C++
  - 1 - SSE
  - 2 - AVX


Чтобы запустить программу:
  - Необходимо зайти в директорию cpp/ и ввести команду python3 setup.py install (возможно, перед придётся написать команду sudo -s)
  - Для JIT-компиляции нужно зайти в директорию cpp/ и ввести команду python3 jit.py(убедитесь, что у вас установлено ninja)
  - Для проверки коррктности выполнения программы необходимо ввести комманду python3 grad_check.py {py, cpp}
  - Для тестирования производительности программы необходимо ввести комманду python3 benchmark.py -m {py, cpp}
 
 ## Тестирование 
 Было произведено несколько тестов производительности работы, написанных функций и Linear, встроенным в pytorch. 
 Более подробные результаты в time.txt.
 
 По оси X - тест из time.txt, ось Y - время, затраченное на выполнение.
 
  - График работы для 3 слоев:
  
 ![alt text](https://github.com/stupakdm/Training/blob/main/CPP_extensions_linear_SSE_AVX/3_layers.png)
 
 - График работы для 4 слоев:
 
 ![alt text](https://github.com/stupakdm/Training/blob/main/CPP_extensions_linear_SSE_AVX/4_layers.png)
 
 - График работы для 5 слоев:
 
 ![alt text](https://github.com/stupakdm/Training/blob/main/CPP_extensions_linear_SSE_AVX/5_layers.png)
 
