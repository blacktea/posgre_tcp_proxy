cmake_minimum_required(VERSION 2.8)

project(proxy_db)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED
    system
    )

find_package(Threads REQUIRED)

FILE(GLOB SRC "*.cpp")
FILE(GLOB HEADERS "*.h")

add_executable(${PROJECT_NAME} ${SRC} ${HEADERS})
target_link_libraries(${PROJECT_NAME}  PRIVATE ${Boost_LIBRARIES} Threads::Threads)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE PG_PROXY_DEBUG=1)
endif()
