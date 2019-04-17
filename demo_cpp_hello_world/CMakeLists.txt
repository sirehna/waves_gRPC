cmake_minimum_required(VERSION 2.8)

project(HelloWorld C CXX)

SET(THIRDPARTY_PREFIX "" CACHE STRING "String prefix to indicate third_party location. Use .. whith local compilation")

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    add_definitions(-D_WIN32_WINNT=0x600)
endif()

if(GRPC_AS_SUBMODULE)
    # One way to build a projects that uses gRPC is to just include the
    # entire gRPC project tree via "add_subdirectory".
    # This approach is very simple to use, but the are some potential
    # disadvantages:
    # * it includes gRPC's CMakeLists.txt directly into your build script
    #   without and that can make gRPC's internal setting interfere with your
    #   own build.
    # * depending on what's installed on your system, the contents of submodules
    #   in gRPC's third_party/* might need to be available (and there might be
    #   additional prerequisites required to build them). Consider using
    #   the gRPC_*_PROVIDER options to fine-tune the expected behavior.
    #
    # A more robust approach to add dependency on gRPC is using
    # cmake's ExternalProject_Add (see cmake_externalproject/CMakeLists.txt).

    # Include the gRPC's cmake build (normally grpc source code would live
    # in a git submodule called "third_party/grpc", but this example lives in
    # the same repository as gRPC sources, so we just look a few directories up)
    add_subdirectory(../../.. ${CMAKE_CURRENT_BINARY_DIR}/grpc EXCLUDE_FROM_ALL)
    message(STATUS "Using gRPC via add_subdirectory.")

    # After using add_subdirectory, we can now use the grpc targets directly from
    # this build.
    set(_PROTOBUF_LIBPROTOBUF libprotobuf)
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
    set(_GRPC_GRPCPP_UNSECURE grpc++_unsecure)
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
else()
    # This branch assumes that gRPC and all its dependencies are already installed
    # on this system, so they can be located by find_package().

    # Find Protobuf installation
    # Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
    set(Protobuf_DEBUG ON)
    set(protobuf_MODULE_COMPATIBLE TRUE)
    # find_package(Protobuf CONFIG REQUIRED)
    message(STATUS "Using protobuf ${protobuf_VERSION}")

    set(_PROTOBUF_LIBPROTOBUF /usr/lib/x86_64-linux-gnu/libprotobuf.a) #protobuf::libprotobuf)
    set(_PROTOBUF_PROTOC protoc ) # GJ $<TARGET_FILE:protobuf::protoc>)

    # Find gRPC installation
    # Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
    # find_package(gRPC CONFIG REQUIRED)
    message(STATUS "Using gRPC ${gRPC_VERSION}")

    set(_GRPC_GRPCPP_UNSECURE
        /usr/lib/x86_64-linux-gnu/libgrpc++_unsecure.a
        /usr/lib/x86_64-linux-gnu/libgrpc.a
        /usr/lib/x86_64-linux-gnu/libcares.a
        /usr/lib/x86_64-linux-gnu/libssl.a
        /usr/lib/x86_64-linux-gnu/libcrypto.a
        z
        dl)
        #/usr/lib/x86_64-linux-gnu/libdl.a) # gRPC::grpc++_unsecure)
        #/usr/lib/x86_64-linux-gnu/libgnutls.a
    set(_GRPC_CPP_PLUGIN_EXECUTABLE /usr/bin/grpc_cpp_plugin)
    # set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

# Proto file
get_filename_component(hw_proto "helloworld.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

# Targets greeter_[async_](client|server)
foreach(_target
    greeter_sync_client greeter_sync_server
    greeter_async_client greeter_async_server)
    add_executable(${_target} "${_target}.cc"
        ${hw_proto_srcs}
        ${hw_grpc_srcs})
    target_link_libraries(${_target}
        ${_GRPC_GRPCPP_UNSECURE}
        ${_PROTOBUF_LIBPROTOBUF}
        pthread)
endforeach()

MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
# add_executable(my_test
#     ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/google_test/googletest/src/gtest-all.cc
#     ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/google_test/googletest/src/gtest_main.cc
#     )
# target_include_directories(my_test SYSTEM PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/google_test/googletest/include)
add_executable(my_test
    tests.cpp
    ${THIRDPARTY_PREFIX}/third_party/google_test/googletest/src/gtest-all.cc
    ${THIRDPARTY_PREFIX}/third_party/google_test/googletest/src/gtest_main.cc
)
target_include_directories(my_test SYSTEM PRIVATE
    ${THIRDPARTY_PREFIX}/third_party/google_test/googletest
    ${THIRDPARTY_PREFIX}/third_party/google_test/googletest/include)
target_link_libraries(my_test pthread)
