
IF (USE_JSON)
    IF(IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/json")
        MESSAGE(STATUS "JSON source found")
        
        #include_directories (
        #"${CMAKE_CURRENT_LIST_DIR}/json"
        #)
        include("${CMAKE_CURRENT_LIST_DIR}/json/CMakeLists.txt")
    ELSE()
        MESSAGE(STATUS "JSON source not found found, deactivating")
        set (USE_JSON false)
    ENDIF()
ENDIF()

include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(libsddl
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/basics.h"
        "${CMAKE_CURRENT_LIST_DIR}/common_data_types.h"
        "${CMAKE_CURRENT_LIST_DIR}/common_structs.h"
        "${CMAKE_CURRENT_LIST_DIR}/common.h"
        "${CMAKE_CURRENT_LIST_DIR}/progid.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/astx_000.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_001.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_002.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_003.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_004.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_008.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_010.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_011.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_016.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_017.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_019.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_020.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_021.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_023.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_030.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_031.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_032.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_034.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_048.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_062.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_063.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_065.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_221.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_247.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/astx_252.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/chk_ass.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/do_asx.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/do_frame.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/do_ioss.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/do_netto.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/do_rec.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/do_rff.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/errors.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/fillup.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_aid.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_alt.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_ano.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_dsi.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/lister.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_ftm.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_idt.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_msa.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_mtp.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_num.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_pos.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_sno.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/list_tod.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/options.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/process.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/proc_inp.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/utils.cpp"
)

add_executable ( sddl "${CMAKE_CURRENT_LIST_DIR}/main.cpp")
target_link_libraries ( sddl libsddl)

#set_property(TARGET sddl PROPERTY C_STANDARD 11)


