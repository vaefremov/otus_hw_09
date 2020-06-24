cmake_minimum_required(VERSION 3.5)

if("$ENV{TRAVIS_BUILD_NUMBER}" STREQUAL "")
  message("Warning: Could not find TRAVIS_BUILD_NUMBER environment variable")
  set(VERSION_NUMBER "0.0.42")
else()
  set(VERSION_NUMBER "0.0.$ENV{TRAVIS_BUILD_NUMBER}")
endif()

project(bayan VERSION ${VERSION_NUMBER})

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

add_library(utilities src/files_comparator.cpp src/scanner.cpp)
target_include_directories(utilities
    PRIVATE 
    "include"
)


add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)
target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE 
    "include"
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
    utilities
    ${CONAN_LIBS}
)

add_executable(test_scanner tests/test_scanner.cpp)
target_include_directories(test_scanner
    PRIVATE 
    "${CMAKE_HOME_DIRECTORY}/include"
    "${GTEST_INCLUDE_DIR}"
)
target_link_libraries(test_scanner
    PRIVATE
    utilities
    ${GTEST_LIBRARIES}
    # ${GTEST_MAIN_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CONAN_LIBS}
)

if (MSVC)
  set(WARNINGS_FLAGS /W4)
else ()
  set(WARNINGS_FLAGS -Wall -Wextra -pedantic -Werror)
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

enable_testing()
# add_test(test_writer bin/test_writer)

install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT vefremov@gmail.com)

include(CPack)

configure_file(bintray-descriptor.in.json ${CMAKE_BINARY_DIR}/bintray-descriptor.json)
