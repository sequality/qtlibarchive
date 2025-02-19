# SPDX-License-Identifier: MIT
# Copyright (C) 2025 sequality software engineering e.U. <office@sequality.at>

cmake_minimum_required(VERSION 3.21)
project(QtLibArchive LANGUAGES CXX C)

option(QTLIBARCHIVE_FETCH_LIBARCHIVE "Fetch libarchive from GitHub" ON)
set(QTLIBARCHIVE_FETCH_LIBARCHIVE_TAG "v3.7.7" CACHE STRING "libarchive version to fetch")
option(QTLIBARCHIVE_BUILD_TESTING "Build QtLibArchive tests" ${PROJECT_IS_TOP_LEVEL})
option(QTLIBARCHIVE_BUILD_SHARED_LIBS "Build QtLibArchive as shared library" ON)

set(CMAKE_AUTOMOC ON)

set(PUBLIC_HEADERS
    include/QtLibArchive/QtLibArchive.h
    include/QtLibArchive/Entry.h
    include/QtLibArchive/Reader.h
    include/QtLibArchive/Writer.h
    include/QtLibArchive/ReaderIterator.h
)
set(PRIVATE_HEADERS)
set(SOURCES
        src/Reader.cpp
        src/Entry.cpp
        src/Writer.cpp
        src/ReaderIterator.cpp
)
set(LIBARCHIVE_TARGET_NAME "")

if (QTLIBARCHIVE_FETCH_LIBARCHIVE)
    include(FetchContent)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

    FetchContent_Declare(LibArchive
        GIT_REPOSITORY https://github.com/libarchive/libarchive.git
        GIT_TAG ${QTLIBARCHIVE_FETCH_LIBARCHIVE_TAG}
        GIT_SHALLOW TRUE
    )

    set(ENABLE_TAR OFF)
    set(ENABLE_CPIO OFF)
    set(ENABLE_CAT OFF)
    set(ENABLE_TEST OFF)
    set(ENABLE_UNZIP OFF)
    FetchContent_MakeAvailable(LibArchive)

    target_compile_options(archive_static PRIVATE -fPIC)
    set(LIBARCHIVE_TARGET_NAME archive_static)
else()
    find_package(LibArchive REQUIRED)
    set(LIBARCHIVE_TARGET_NAME "LibArchive::LibArchive")
endif()


find_package(Qt5 COMPONENTS Core REQUIRED)

if (QTLIBARCHIVE_BUILD_SHARED_LIBS)
    set(QTLIBARCHIVE_LIBRARY_TYPE SHARED)
else()
    set(QTLIBARCHIVE_LIBRARY_TYPE STATIC)
endif()

add_library(qtlibarchive ${QTLIBARCHIVE_LIBRARY_TYPE} ${SOURCES} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS}
    src/QtLibArchive.cpp)
target_link_libraries(qtlibarchive PUBLIC Qt5::Core PRIVATE ${LIBARCHIVE_TARGET_NAME})
target_compile_features(qtlibarchive PUBLIC cxx_std_17)
target_compile_definitions(qtlibarchive PRIVATE QTLIBARCHIVE_LIBRARY)
target_include_directories(qtlibarchive PUBLIC include)
add_library(QtLibArchive ALIAS qtlibarchive)
add_library(Qt::LibArchive ALIAS qtlibarchive)

set_target_properties(qtlibarchive
    PROPERTIES
        ARCHIVE_OUTPUT_NAME QtLibArchive
        LIBRARY_OUTPUT_NAME QtLibArchive
)

if (WIN32)
    set_target_properties(qtlibarchive
        PROPERTIES
            PREFIX ""
    )
endif()

if (QTLIBARCHIVE_BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
