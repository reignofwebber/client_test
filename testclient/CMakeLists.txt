# CMakeList.txt : CMake project for testclient, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
find_package(Boost REQUIRED COMPONENTS system thread regex)

message(${Boost_INCLUDE_DIRS})


add_executable(TestClient client.h
                          message.h
						  main.cpp)

target_include_directories(TestClient PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(TestClient PRIVATE ${Boost_LIBRARIES})

target_compile_features(TestClient PRIVATE cxx_std_14)

# TODO: Add tests and install targets if needed.
