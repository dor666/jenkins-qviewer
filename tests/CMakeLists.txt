find_package(Threads)
# Enable ExternalProject CMake module
include(ExternalProject)

set(gtest_install_dir ${CMAKE_BINARY_DIR}/install/gtest)

# Add gmock
ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG d406cb18c3d73995cec51fc6b39a2aea821b68c1
    UPDATE_DISCONNECTED 1
    TIMEOUT 30
    INSTALL_DIR ${gtest_install_dir}
    CMAKE_ARGS -Dgtest_force_shared_crt=ON -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

include_directories(${gtest_install_dir}/include)

add_executable(tst_joblistresponseparser tst_joblistresponseparser.cpp)
add_dependencies(tst_joblistresponseparser googletest)

set(CMAKE_FIND_LIBRARY_SUFFIXES .a) #override todo fix
target_link_libraries(tst_joblistresponseparser
                        ${gtest_install_dir}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main${CMAKE_FIND_LIBRARY_SUFFIXES}
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${PROJECT_LIB_NAME}
                     )
