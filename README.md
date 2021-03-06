# EmeraldEngine

[![CodeFactor](https://www.codefactor.io/repository/github/frideakisab/emeraldengine/badge)](https://www.codefactor.io/repository/github/frideakisab/emeraldengine) [![C/C++ CI](https://github.com/FrideakisAB/EmeraldEngine/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/FrideakisAB/EmeraldEngine/actions/workflows/c-cpp.yml) [![codecov](https://codecov.io/gh/FrideakisAB/EmeraldEngine/branch/main/graph/badge.svg?token=TYVKWPH17K)](https://codecov.io/gh/FrideakisAB/EmeraldEngine)

EmeraldEngine - 3D игровой движок написанный на C++.

## Поддерживаемые платформы

* Windows

### планируются

* Linux
* macOS
* Android
* iOS

## Компиляция

Проект требует компилятор с поддержкой C++17, рекомендуемые инструменты:

* Visual Studio 19 + CMake 3.16 + Conan (latest)

Сборка:

```bash
cmake -DCMAKE_BUILD_TYPE=%build_type% -S .
cmake --build .
```

## Используемые библиотеки

* GLM
* GLEW
* GLFW
* json (nlohmann)
* boost
* Catch2

MIT license
