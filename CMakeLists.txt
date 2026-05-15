cmake_minimum_required(VERSION 3.21)
project(TradeSim VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts)

set(SOURCES
    src/main.cpp
    src/db/Database.cpp
    src/db/Schema.cpp
)

qt_add_executable(TradeSim ${SOURCES})

target_link_libraries(TradeSim PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
)

if(EMSCRIPTEN)
    target_link_options(TradeSim PRIVATE
        -sASYNCIFY
        -sASYNCIFY_STACK_SIZE=65536
        -sWASM=1
        -sALLOW_MEMORY_GROWTH=1
        --bind
        -sEXPORTED_RUNTIME_METHODS=FS
    )
endif()