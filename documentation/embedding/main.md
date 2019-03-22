# Embedding Kafe

## Requirements

* C++17 compiler
* CMake 3.8 (if you want to use the given CMakeLists.txt)

## Basic setup

Saying you have this configuration:
```
|__project/
   |__kafe/
   |  |__CMakeLists.txt
   |  |__...
   |__source/
   |  |__...
   |__include/
   |  |__...
   |__CMakeLists.txt
```

with your code in `source` and `include`.

You will need to modify your CMakeLists.txt to look like this:

```cmake
cmake_minimum_required(VERSION 3.8)

project(MyProject CXX)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/source/*.cpp)

# building Kafe and adding it to the include dirs
add_subdirectory(kafe)
include_directories(${KAFE_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# don't forget to link with libKafe !
target_link_libraries(${PROJECT_NAME} PRIVATE Kafe)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)
```

Then, in your C++ code:

```cpp
#include <kafe/kafe.hpp>
#include <string>
#include <iostream>

const std:string g_code =
    "cst x : int = 12\n"
    "fun main(argc: int, argv: string) -> int\n"
    "    print(x)\n"
    "    ret 0\n"
    "end"
;

int main()
{
    kafe::Parser p(g_code);
    p.parse();

    // print ast
    p.ASTtoString(std::cout);

    // transform code into bytecode
    auto bytecode = p.generateBytecode();

    // launch the bytecode
    kafe::VM vm;
    vm.feed(bytecode);
    vm.exec();

    auto x = vm.get<int>("x");
    auto ret = vm.call<int>(1, std::string("hello"));

    return 0;
}
```