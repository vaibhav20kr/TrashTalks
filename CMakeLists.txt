cmake_minimum_required(VERSION 3.16)
project(TrashTalk VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

set(SOURCES
    src/main.cpp
    src/BinManager.cpp
    src/MainWindow.cpp
)

set(HEADERS
    src/Bin.h
    src/HistoryRecord.h
    src/HashTable.h
    src/MaxHeap.h
    src/Sorting.h
    src/Queue.h
    src/LinkedList.h
    src/BinManager.h
    src/MainWindow.h
)

add_executable(TrashTalk ${SOURCES} ${HEADERS})

target_link_libraries(TrashTalk PRIVATE Qt6::Widgets Qt6::Charts)

if(MSVC)
    target_compile_options(TrashTalk PRIVATE /W4)
else()
    target_compile_options(TrashTalk PRIVATE -Wall -Wextra)
endif()