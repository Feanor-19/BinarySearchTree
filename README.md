# Двоичное поисковое дерево

## Установка

### Виртуальное окружение

Все последующие шаги, включая установку conan, можно делать в [виртуальном окружении](https://docs.python.org/3/library/venv.html):

```
python3 -m venv .venv
source .venv/bin/activate
pip3 install conan
```

### Необходимые сторонние инструменты

- cmake >=3.11
- conan >= 2.7.1

Если `conan` используется первый раз, требуется также исполнить: 
```
conan profile detect
```

### Клонирование репозитория

```
git clone https://github.com/Feanor-19/BinarySearchTree
cd BinarySearchTree
```

### Подготовка зависимостей

Release configure preset:
```
conan install . --output-folder=build/Release --build=missing --settings=build_type=Release
```

Debug configure preset:
```
conan install . --output-folder=build/Debug --build=missing --settings=build_type=Debug
```

### Сборка с cmake

Release:
```
cd build/Release
cmake ../.. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Debug:
```
cd build/Debug
cmake ../.. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```