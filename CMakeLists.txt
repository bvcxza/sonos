cmake_minimum_required(VERSION 3.30)
project(sonos CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT MSVC)
	message(STATUS "NOT MSVC")
	add_compile_options("-Wall" "-Wextra" "$<$<CONFIG:DEBUG>:-Werror>" "$<$<CONFIG:RELEASE>:-O3>")
	add_compile_options("$<$<CONFIG:DEBUG>:-Wpedantic>")
	add_compile_options("$<$<CONFIG:DEBUG>:-Wpointer-arith>")
	add_compile_options("$<$<CONFIG:DEBUG>:-Wundef>")
	add_compile_options("$<$<CONFIG:DEBUG>:-Wno-stringop-overflow>")
	add_compile_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")
	add_compile_options("$<$<CONFIG:DEBUG>:-fsanitize=leak>")
	add_compile_options("$<$<CONFIG:DEBUG>:-fsanitize=undefined>")
	add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")
	add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=leak>")
	add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=undefined>")
endif()

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
find_package(Boost 1.73 REQUIRED json url)
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")

find_package(PkgConfig REQUIRED)
pkg_search_module(SECP256K1 REQUIRED libsecp256k1)
message(STATUS "SECP256K1_INCLUDEDIR: ${SECP256K1_INCLUDEDIR}")
message(STATUS "SECP256K1_LIBRARIES: ${SECP256K1_LIBRARIES}")

add_library(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
	${CMAKE_SOURCE_DIR}/external/bech32/ref/c++/bech32.cpp
	src/utils.cpp
	src/key.cpp
	src/pubkey.cpp
	src/keypair.cpp
	src/nostr.cpp
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
	${OPENSSL_INCLUDE_DIR}
	Boost::headers
	${CMAKE_SOURCE_DIR}/external/bech32/ref/c++
	${SECP256K1_INCLUDEDIR}
	${CMAKE_SOURCE_DIR}/external/asio-socks45-client
)
target_link_libraries(${CMAKE_PROJECT_NAME}
	Threads::Threads
	${OPENSSL_LIBRARIES}
	${SECP256K1_LIBRARIES}
	Boost::json
	Boost::url
)

add_executable(main_ex
	src/commands/convert_cmd.cpp
	src/commands/event_cmd.cpp
	src/commands/keypair_cmd.cpp
	src/commands/relays_cmd.cpp
	src/commands/req_cmd.cpp
	src/commands/user_cmd.cpp
	src/main.cpp
)
set_property(TARGET main_ex PROPERTY OUTPUT_NAME ${CMAKE_PROJECT_NAME})
target_include_directories(main_ex PRIVATE
	${CMAKE_SOURCE_DIR}/include
	Boost::headers
	${CMAKE_SOURCE_DIR}/external/bech32/ref/c++
	${SECP256K1_INCLUDEDIR}
	${CMAKE_SOURCE_DIR}/external/asio-socks45-client
)
target_link_libraries(main_ex
	Threads::Threads
	${OPENSSL_LIBRARIES}
	${SECP256K1_LIBRARIES}
	${CMAKE_PROJECT_NAME}
	Boost::json
	Boost::url
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
	${CMAKE_SOURCE_DIR}/include
	${SECP256K1_INCLUDEDIR}
)
target_link_libraries(KeyPairTest
	GTest::GTest
	GTest::Main
	${CMAKE_PROJECT_NAME}
)
gtest_discover_tests(KeyPairTest)
# KeyPairTest <

# NostrTest >
add_executable(NostrTest test/nostr_test.cpp)
target_include_directories(NostrTest PRIVATE
	${CMAKE_SOURCE_DIR}/include
	${SECP256K1_INCLUDEDIR}
)
target_link_libraries(NostrTest
	GTest::GTest
	GTest::Main
	${CMAKE_PROJECT_NAME}
	Boost::json
)
gtest_discover_tests(NostrTest)
# NostrTest <

# Install
install(TARGETS ${CMAKE_PROJECT_NAME} main_ex)

message(STATUS "Summary of build options:
	CXX compiler:                 ${CMAKE_CXX_COMPILER} (${CMAKE_CXX_COMPILER_ID})
	CMAKE_INSTALL_PREFIX          ${CMAKE_INSTALL_PREFIX}
")
