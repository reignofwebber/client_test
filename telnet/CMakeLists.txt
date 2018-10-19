cmake_minimum_required (VERSION 3.8)

find_package(Boost REQUIRED COMPONENTS system thread regex)

message(${Boost_INCLUDE_DIRS})


add_executable(TelClient main.cpp telnet.h)
                          

target_include_directories(TelClient PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(TelClient PRIVATE ${Boost_LIBRARIES})

target_compile_features(TelClient PRIVATE cxx_std_14)
