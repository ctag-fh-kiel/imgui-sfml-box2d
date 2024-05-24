cmake_minimum_required(VERSION 3.16)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

set(SFML_STATIC_LIBRARIES TRUE)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.89.9)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(ImGui-SFML)

option(BOX2D_BUILD_UNIT_TESTS OFF)
FetchContent_Declare(box2d
        GIT_REPOSITORY      https://github.com/erincatto/Box2D.git
        GIT_TAG             v2.4.1)
FetchContent_MakeAvailable(box2d)

add_executable(CMakeSFMLProject src/main.cpp)
target_link_libraries(CMakeSFMLProject PRIVATE sfml-graphics ImGui-SFML::ImGui-SFML box2d)
target_compile_features(CMakeSFMLProject PRIVATE cxx_std_17)

install(TARGETS CMakeSFMLProject DESTINATION bin)

if(WIN32)
    # create install target

    # add all mingw64 system dlls to install target
    # this is overkill, but collecting only the needed dlls is a pain
    set(MINGW_ROOT "c:/msys64/mingw64")
    # install all dlls except those which start with Qt5
    install(DIRECTORY ${MINGW_ROOT}/bin/ DESTINATION bin
            FILES_MATCHING
            PATTERN "*.dll"
            EXCLUDE)


    # make deployment using cpack and nsis
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME "CMakeSFMLProject")
    set(CPACK_PACKAGE_VENDOR "CTAG")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CMakeSFMLProject")
    set(CPACK_PACKAGE_VERSION "1.0.0")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMakeSFMLProject")
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL OFF)
    set(CPACK_NSIS_IGNORE_LICENSE_PAGE ON)
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    #set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico")
    #set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico")
    set(CPACK_NSIS_MENU_LINKS
            "bin/CMakeSFMLProject.exe" "CMakeSFMLProject"
    )

    include(CPack)
endif()

