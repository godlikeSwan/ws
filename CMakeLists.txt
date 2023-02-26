cmake_minimum_required(VERSION 3.12)

project(ws VERSION 1.0)

# set(CMAKE_CXX_FLAGS "-ggdb -fsanitize=address")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG  "${CMAKE_SOURCE_DIR}/lib")

set(src_dir "${CMAKE_SOURCE_DIR}/src")

set(src
    netsocket.cpp
    websocket.cpp
    websocketframe.cpp
    websocketmessage.cpp
)
list(TRANSFORM src PREPEND "${src_dir}/")

# add_executable(g WIN32 "${src}")
add_library(ws "${src}")

add_executable(netsocket_example "${CMAKE_SOURCE_DIR}/examples/netsocket_example.cpp")
add_executable(websocket_example "${CMAKE_SOURCE_DIR}/examples/websocket_example.cpp")

add_executable(queue_test "${CMAKE_SOURCE_DIR}/test/queue_test.cpp")

# for static linking

# set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib;.a;.dll.a")
# target_compile_options(g PUBLIC /MT)
# target_link_options(g PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)

# find_package(OpenSSL REQUIRED)
# find_package(OpenGL REQUIRED)

# target_link_libraries(g opengl32.lib)Ws2_32.lib

if(WIN32)
  target_link_libraries(ws Ws2_32.lib)
endif()

target_include_directories(ws PUBLIC "${CMAKE_SOURCE_DIR}/src/include/")

target_include_directories(netsocket_example PUBLIC "${CMAKE_SOURCE_DIR}/src/include/")
target_include_directories(websocket_example PUBLIC "${CMAKE_SOURCE_DIR}/src/include/")
target_include_directories(queue_test PUBLIC "${CMAKE_SOURCE_DIR}/src/include/")
target_link_libraries(netsocket_example ws)
target_link_libraries(websocket_example ws)

# include_directories("${CMAKE_SOURCE_DIR}/src/include/")

# set_target_properties(ws PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
