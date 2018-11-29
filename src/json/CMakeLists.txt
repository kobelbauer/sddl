
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(libsddl
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/json.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/json_conv.h"
        "${CMAKE_CURRENT_LIST_DIR}/json_conv.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/jsonwriter.h"
        "${CMAKE_CURRENT_LIST_DIR}/jsonwriter.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/jsonfilewritetask.h"
)


