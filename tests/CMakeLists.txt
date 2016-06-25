find_package(Threads)
# Enable ExternalProject CMake module
include(ExternalProject)

# Add gmock
ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 0a43962
    UPDATE_DISCONNECTED 1
    TIMEOUT 30
    CMAKE_ARGS -Dgtest_force_shared_crt=ON
    # Disable install step
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

# Specify include dir for googletest and googlemock
ExternalProject_Get_Property(googletest source_dir)
include_directories(${source_dir}/googlemock/include)
include_directories(${source_dir}/googletest/include)

add_executable(tst_joblistresponseparser tst_joblistresponseparser.cpp)

add_dependencies(tst_joblistresponseparser googletest)

ExternalProject_Get_Property(googletest binary_dir)

set(CMAKE_FIND_LIBRARY_SUFFIXES .a) #override todo fix
target_link_libraries(tst_joblistresponseparser
                        ${binary_dir}/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main${CMAKE_FIND_LIBRARY_SUFFIXES}
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${PROJECT_LIB_NAME}
                     )
