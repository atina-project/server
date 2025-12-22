set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS OFF)
set(BUILD_OBJECT_LIBS OFF)
set(BUILD_TESTING OFF)

message(STATUS "Handling third-party library: boost-ext.di")
set(DI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/di/include"
    CACHE STRING "Boost-ext.di include directory"
    FORCE
)

message(STATUS "Handling third-party library: g3log")
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0077 "NEW")
endif()
set(ADD_FATAL_EXAMPLE OFF)
set(ADD_G3LOG_PERFORMANCE OFF)
set(ADD_G3LOG_UNIT_TEST OFF)
set(INSTALL_G3LOG OFF)
set(USE_DYNAMIC_LOGGING_LEVELS ON)
add_subdirectory(g3log)
set(G3LOG_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/g3log/src"
    "${CMAKE_CURRENT_BINARY_DIR}/g3log/include"
    CACHE STRING "g3log include directory"
    FORCE
)

message(STATUS "Handling third-party library: jsoncpp")
set(JSONCPP_WITH_TESTS OFF)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
set(JSONCPP_WITH_CMAKE_PACKAGE OFF)
set(JSONCPP_NO_INSTALL ON)
add_subdirectory(jsoncpp)
set(JSONCPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp/include"
    CACHE STRING "Jsoncpp include directory"
    FORCE
)

message(STATUS "Handling third-party library: libsodium")
add_subdirectory(libsodium)
set(LIBSODIUM_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/libsodium/src/libsodium/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/libsodium/src/libsodium/include/sodium"
    ${CMAKE_BINARY_DIR}
    CACHE STRING "libsodium include directory"
    FORCE
)

message(STATUS "Handling third-party library: libsqlite3")
add_subdirectory(libsqlite3)
set(SQLITE3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libsqlite3/include"
    CACHE STRING "SQLite3 include directory"
    FORCE
)

message(STATUS "Handling third-party library: libuuid")
add_subdirectory(libuuid)
set(LIBUUID_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libuuid/include"
    CACHE STRING "libuuid include directory"
    FORCE
)

message(STATUS "Handling third-party library: sqlite_orm")
set(SQLITE_ORM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sqlite_orm/include"
    CACHE STRING "sqlite_orm include directory"
    FORCE
)

message(STATUS "Handling third-party library: zlib")
# set(ZLIB_BUILD_SHARED OFF)
set(ZLIB_BUILD_STATIC OFF)
add_subdirectory(zlib)
# zlib is only required by drogon, therefore no include_dir var is needed

message(STATUS "Handling third-party library: drogon")
set(DROGON_BUILD_EXAMPLES OFF)
set(DROGON_BUILD_DOC OFF)
# set(DROGON_BUILD_ORM OFF)
set(DROGON_BUILD_BROTLI OFF)
set(DROGON_BUILD_YAML_CONFIG OFF)
set(DROGON_BUILD_SQLITE ON)
set(DROGON_NO_INSTALL ON)
set(DROGON_USE_SELF_MANAGED_JSONCPP ON)
set(DROGON_SELF_MANAGED_JSONCPP_INCLUDE_DIR ${JSONCPP_INCLUDE_DIR})
set(DROGON_SELF_MANAGED_JSONCPP_LIB jsoncpp_lib)
set(DROGON_USE_SELF_MANAGED_LIBUUID ON)
set(DROGON_SELF_MANAGED_LIBUUID_INCLUDE_DIR ${LIBUUID_INCLUDE_DIR})
set(DROGON_SELF_MANAGED_LIBUUID_LIB libuuid)
set(DROGON_USE_SELF_MANAGED_ZLIB ON)
set(DROGON_SELF_MANAGED_ZLIB_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib
    ${CMAKE_CURRENT_BINARY_DIR}/zlib
)
set(DROGON_SELF_MANAGED_ZLIB_LIB ZLIB::ZLIB)
set(DROGON_USE_SELF_MANAGED_SQLITE ON)
set(DROGON_SELF_MANAGED_SQLITE_INCLUDE_DIR ${SQLITE3_INCLUDE_DIR})
set(DROGON_SELF_MANAGED_SQLITE_LIB sqlite3)
add_subdirectory(drogon)


