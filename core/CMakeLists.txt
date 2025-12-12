project(atina-server-core)

set(ATINA_SERVER_CORE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include"
    CACHE STRING "Atina server core include directory"
    FORCE
)
set(ATINA_SERVER_CORE_SRC_LIST
    ${PROJECT_SOURCE_DIR}/src/builtin/thread_pool.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/crypto.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/time.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/uuid.cpp
    ${PROJECT_SOURCE_DIR}/src/account.cpp
)
add_library(${PROJECT_NAME} SHARED
    ${ATINA_SERVER_CORE_SRC_LIST}
)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${ATINA_SERVER_CORE_INCLUDE_DIR}
    PRIVATE
        ${DI_INCLUDE_DIR}
        ${LIBSODIUM_INCLUDE_DIR}
        ${LIBUUID_INCLUDE_DIR}
        ${SQLITE3_INCLUDE_DIR}
        ${SQLITE_ORM_INCLUDE_DIR}
)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        ATINA_SERVER_USE_SELF_MANAGED_SQLITE
)
target_link_libraries(${PROJECT_NAME}
    drogon
    libsodium
    libuuid
    sqlite3
)
