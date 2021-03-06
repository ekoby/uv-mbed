CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

execute_process(
        COMMAND git describe --tags HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_INFO
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(${GIT_INFO} MATCHES "^v([0-9]+\\.[0-9]+\\.[0-9]+)$")
    set(ver ${CMAKE_MATCH_1})
elseif(${GIT_INFO} MATCHES "^v([0-9]+\\.[0-9]+\\.[0-9]+)-([0-9]+)-[^-]*")
    string(JOIN "." ver ${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
endif()

PROJECT("uv-mbed"
        LANGUAGES C
        VERSION ${ver}
        )

message("Project: ${CMAKE_PROJECT_NAME}@v${CMAKE_PROJECT_VERSION}")

include(GNUInstallDirs)

#sample is off by default
option(ENABLE_UM_SAMPLES "Build samples." OFF)
option(ENABLE_UM_TESTS "Build tests." ON)

option(USE_OPENSSL "use OpenSSL engine" OFF)

option(HAVE_LIBUV "use installed system-wide libuv" OFF)

if(NOT USE_OPENSSL)
    message("using mbedTLS libraries")
    set(USE_MBEDTLS true)
    set(tlsImpl mbedtls)
else()
    set(tlsImpl openssl)
endif()

add_subdirectory(deps)

if (USE_MBEDTLS)
set(p11_sources
        src/p11/p11_mbedtls/mbed_p11.c
        src/p11/p11_mbedtls/p11_ecdsa.c
        src/p11/p11_mbedtls/p11_rsa.c)
set(tls_engine_files
        src/engine_mbedtls.c)
elseif(USE_OPENSSL)
set(tls_engine_files
        src/engine_openssl.c)
endif()

set(uv_mbed_sources
        src/uv_mbed.c
        src/bio.c
        src/http.c
        src/tcp_src.c
        src/um_debug.c
        src/um_debug.h
        src/websocket.c
        src/http_req.c
        src/tls_link.c
        src/base64.c
        src/tls_engine.c
        )


add_library(uv_mbed STATIC
        ${uv_mbed_sources}
        ${tls_engine_files}
        ${p11_sources}
        )

set_property(TARGET uv_mbed PROPERTY C_STANDARD 99)

target_include_directories(uv_mbed
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/p11
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/libuv/src
        )

if(HAVE_LIBUV)
    set(LIBUV_NAME uv)
else()
    set(LIBUV_NAME uv_a)
endif()

target_link_libraries(uv_mbed
        PUBLIC http-parser
        PUBLIC ${LIBUV_NAME}
        PUBLIC uv_link
        )
TARGET_COMPILE_DEFINITIONS(uv_mbed PRIVATE UV_MBED_VERSION=v${CMAKE_PROJECT_VERSION})

if (USE_MBEDTLS)
    target_compile_definitions(uv_mbed PRIVATE USE_MBEDTLS)
    target_link_libraries(uv_mbed
            PRIVATE mbedtls
            )
elseif(USE_OPENSSL)
    find_package(OpenSSL REQUIRED)
    target_compile_definitions(uv_mbed PRIVATE USE_OPENSSL)
    target_link_libraries(uv_mbed PRIVATE OpenSSL::SSL)
endif()

if (WIN32)
    target_link_libraries(uv_mbed
            PUBLIC crypt32)

endif()

install(DIRECTORY include/uv_mbed DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS uv_mbed
        ARCHIVE DESTINATION lib)

if(ENABLE_UM_SAMPLES)
add_subdirectory(sample)
endif()

if(ENABLE_UM_TESTS)
ENABLE_TESTING()
add_subdirectory(tests)
endif()