# macro to apply static linking flags and hidden visibility
macro(apply_static_flags TARGET_NAME)
    if(WIN32 AND MINGW)
        target_link_libraries(${TARGET_NAME} PRIVATE
            "-static-libgcc"
            "-static-libstdc++"
            "-Wl,-Bstatic"
            "-lwinpthread"
            "-Wl,-Bdynamic"
        )
    endif()

    if(UNIX AND NOT APPLE)
        target_link_options(${TARGET_NAME} PRIVATE
            -static-libgcc
            -static-libstdc++
        )
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
endmacro()
