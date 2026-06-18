# SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

function(latte_apply_relaxed_warning_flags)
    # Keep the Plasma 6 port buildable across distro-patched KDECompilerSettings
    # while preserving a single explicit checklist for future warning cleanup.
    set(LATTE_RELAXED_WARNING_FLAGS
        -Wall
        -Wdeprecated-declarations
        -Wreorder
        -Wunused-variable
        -Wunused-parameter
        -Wsuggest-final-types
    )

    foreach(_latte_warning_flag IN LISTS LATTE_RELAXED_WARNING_FLAGS)
        string(REPLACE "${_latte_warning_flag}" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endforeach()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
    message(STATUS "Latte relaxed warning flags pending cleanup: ${LATTE_RELAXED_WARNING_FLAGS}")
endfunction()
