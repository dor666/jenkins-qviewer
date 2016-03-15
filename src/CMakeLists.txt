set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(QT_PACKAGES Core Network Qml Quick)
foreach(package ${QT_PACKAGES})
    find_package(Qt5${package} REQUIRED)
endforeach(package)

set(QML_FILES
    Main.qml
    MainForm.qml
)

set(RESOURCES
    qml.qrc
)
qt5_add_resources(RESOURCE_ADDED ${RESOURCES})

add_library(${PROJECT_LIB_NAME} STATIC
    connection.cpp
    connection.h
    joblistresponseparser.cpp
    joblistresponseparser.h
    qtutils.h
    ${RESOURCE_ADDED}
    ${QML_FILES}
)
target_compile_features(${PROJECT_LIB_NAME} PUBLIC
    cxx_nullptr
    cxx_nonstatic_member_init
)

foreach(package ${QT_PACKAGES})
    target_link_libraries(${PROJECT_LIB_NAME} Qt5::${package})
endforeach(package)

target_include_directories(${PROJECT_LIB_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

add_executable(${CMAKE_PROJECT_NAME} WIN32 main.cpp)

target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_generalized_initializers)

target_link_libraries(${CMAKE_PROJECT_NAME}
    ${PROJECT_LIB_NAME}
)
