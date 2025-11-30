set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC_LIBS OFF)
set(BUILD_OBJECT_LIBS OFF)
set(BUILD_TESTING OFF)

message(STATUS "Handling third-party library: jsoncpp")
set(JSONCPP_WITH_TESTS OFF)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
set(JSONCPP_WITH_CMAKE_PACKAGE OFF)
# set(JSONCPP_INSTALL_LIB_ONLY ON)
# set(JSONCPP_INSTALL_LIB_PATH ${ATINA_SERVER_INSTALL_LIB_DIR})
set(JSONCPP_LIB_NO_SOVERSION ON)
add_subdirectory(jsoncpp)
set(JSONCPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp/include"
    CACHE STRING "Jsoncpp include directory"
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

message(STATUS "Handling third-party library: zlib")
# set(ZLIB_BUILD_SHARED OFF)
set(ZLIB_BUILD_STATIC OFF)
add_subdirectory(zlib)
# zlib is only required by drogon, therefore no include_dir var is needed

message(STATUS "Handling third-party library: drogon")
set(DROGON_BUILD_EXAMPLES OFF)
set(DROGON_BUILD_DOC OFF)
set(DROGON_BUILD_BROTLI OFF)
set(DROGON_BUILD_YAML_CONFIG OFF)
set(DROGON_BUILD_SQLITE ON)
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


