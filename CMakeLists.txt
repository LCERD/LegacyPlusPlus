cmake_minimum_required(VERSION 3.26)
project(LegacyPlusPlus VERSION 1.1.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

file(GLOB_RECURSE LegacyPlusPlusSrc CONFIGURE_DEPENDS
        "src/*.h"
        "src/*.cpp"
        "src/*.rc"
)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Concurrent)
qt_standard_project_setup()

# libLCE
option(LOCAL_LIBLCE "use local libLCE" OFF)

message(STATUS "use local libLCE: ${LOCAL_LIBLCE}")

if(NOT LOCAL_LIBLCE)
    FetchContent_Declare(
            LCE
            GIT_REPOSITORY https://github.com/DexrnZacAttack/libLCE.git
            GIT_TAG        main
    )
    FetchContent_MakeAvailable(LCE)
else()
    add_subdirectory("lib/libLCE")

    include_directories(LegacyPlusPlus PRIVATE "lib/libLCE/include")
endif()

include(FetchContent)
FetchContent_Declare(
        BIO
        GIT_REPOSITORY https://github.com/DexrnZacAttack/libBIO.git
        GIT_TAG main
)
FetchContent_MakeAvailable(BIO)

include_directories("src")

qt_add_executable(LegacyPlusPlus
        ${LegacyPlusPlusSrc}
)

target_link_libraries(${PROJECT_NAME} PRIVATE LCE BIO Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Concurrent)

if(APPLE)
    set_target_properties(LegacyPlusPlus PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_GUI_IDENTIFIER "me.dexrn.LegacyPlusPlus"

            MACOSX_BUNDLE_BUNDLE_NAME "Legacy++"

            MACOSX_BUNDLE_COPYRIGHT "DexrnZacAttack, Legacy Console Edition Research & Development"
    )
    install(TARGETS LCE LIBRARY DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/LegacyPlusPlus.app/Contents/Frameworks")
endif()

target_compile_definitions(LegacyPlusPlus PRIVATE
        LPP_VERSION="${PROJECT_VERSION}"
        LPP_RC_VERSION=${PROJECT_VERSION_MAJOR},${PROJECT_VERSION_MINOR},${PROJECT_VERSION_PATCH}
)
