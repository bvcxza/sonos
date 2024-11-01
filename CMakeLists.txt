cmake_minimum_required(VERSION 3.30)
project(sonos CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "Build environment:
	System processor: ${CMAKE_SYSTEM_PROCESSOR}
	CXX compiler:     ${CMAKE_CXX_COMPILER} (${CMAKE_CXX_COMPILER_ID})
")

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
	if(UNIX)
		set(OPENSSL_LIBRARIES "${OPENSSL_LIBRARIES};${CMAKE_DL_LIBS};${CMAKE_THREAD_LIBS_INIT}")
	endif()
	message(STATUS "Using OpenSSL include dir at ${OPENSSL_INCLUDE_DIR}")
	message(STATUS "Using OpenSSL libs: ${OPENSSL_LIBRARIES}")
endif()

find_package(Threads REQUIRED)

cmake_policy(SET CMP0167 NEW)
set(Boost_VERBOSE ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.73 REQUIRED)
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")

add_library(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
	external/bech32/ref/c++/bech32.cpp
	src/utils.cpp
	src/key.cpp
	src/keypair.cpp
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
	${OPENSSL_INCLUDE_DIR}
	Boost::headers
	${CMAKE_SOURCE_DIR}/external/bech32/ref/c++
	${CMAKE_SOURCE_DIR}/external/secp256k1/include
)
target_link_libraries(${CMAKE_PROJECT_NAME}
	Threads::Threads
	${OPENSSL_LIBRARIES}
	${CMAKE_SOURCE_DIR}/build_secp256k1/lib/libsecp256k1.a
)

add_executable(main_ex src/main.cpp)
set_property(TARGET main_ex PROPERTY OUTPUT_NAME ${CMAKE_PROJECT_NAME})
target_include_directories(main_ex PRIVATE
	include
	Boost::headers
	${CMAKE_SOURCE_DIR}/external/bech32/ref/c++
	${CMAKE_SOURCE_DIR}/external/secp256k1/include
)
target_link_libraries(main_ex
	Threads::Threads
	${OPENSSL_LIBRARIES}
	${CMAKE_SOURCE_DIR}/build_secp256k1/lib/libsecp256k1.a
	${CMAKE_PROJECT_NAME}
)

# Tests

find_package(GTest REQUIRED)
include(GoogleTest)
enable_testing()

# KeyTest >
add_executable(KeyTest test/key_test.cpp)
target_include_directories(KeyTest PRIVATE include)
target_link_libraries(KeyTest
	GTest::GTest
	GTest::Main
	${CMAKE_PROJECT_NAME}
)
gtest_discover_tests(KeyTest)
# KeyTest <

# KeyPairTest >
add_executable(KeyPairTest test/keypair_test.cpp)
target_include_directories(KeyPairTest PRIVATE
	include
	${CMAKE_SOURCE_DIR}/external/secp256k1/include
)
target_link_libraries(KeyPairTest
	GTest::GTest
	GTest::Main
	${CMAKE_PROJECT_NAME}
#	${CMAKE_SOURCE_DIR}/build_secp256k1/lib/libsecp256k1.a
)
gtest_discover_tests(KeyPairTest)
# KeyPairTest <

message(STATUS "Summary of build options:
	CXX compiler:                 ${CMAKE_CXX_COMPILER} (${CMAKE_CXX_COMPILER_ID})
	C_FLAGS:                      ${CMAKE_C_FLAGS}
	CXX_FLAGS:                    ${CMAKE_CXX_FLAGS}
	CMAKE_EXE_LINKER_FLAGS:       ${CMAKE_EXE_LINKER_FLAGS}
	CMAKE_THREAD_LIBS_INIT        ${CMAKE_THREAD_LIBS_INIT}
")
