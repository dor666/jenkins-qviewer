find_package(Threads)
# Enable ExternalProject CMake module
include(ExternalProject)

# Add gmock
ExternalProject_Add(
    googlemock
    SVN_REPOSITORY http://googlemock.googlecode.com/svn/trunk/
    SVN_REVISION -r466
    UPDATE_DISCONNECTED 1
    TIMEOUT 30
    CMAKE_ARGS -Dgtest_force_shared_crt=ON
    # Disable install step
    INSTALL_COMMAND ""
)

# Specify include dir for googlemock and googletest
ExternalProject_Get_Property(googlemock source_dir)
include_directories(${source_dir}/include)
include_directories(${source_dir}/gtest/include)

add_executable(tst_joblistresponseparser tst_joblistresponseparser.cpp)

ExternalProject_Get_Property(googlemock binary_dir)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a) #override todo fix
target_link_libraries(tst_joblistresponseparser
                        ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main${CMAKE_FIND_LIBRARY_SUFFIXES}
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${PROJECT_LIB_NAME}
                     )
