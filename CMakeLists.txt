cmake_minimum_required (VERSION 4.0.0)


#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d" CACHE INTERNAL "" FORCE)
project(SimpleSwitcher VERSION 1.0 LANGUAGES CXX)
# Глобальные настройки
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_EXTENSIONS          OFF)
#set(CMAKE_CXX_MODULE_STD          ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)



find_package(PkgConfig REQUIRED)
pkg_check_modules(XKBCOMMON REQUIRED IMPORTED_TARGET xkbcommon)
pkg_check_modules(SYSTEMD REQUIRED IMPORTED_TARGET libsystemd)


file(GLOB_RECURSE src  "src/*.cpp" )
add_executable (${PROJECT_NAME} ${src}) 
target_precompile_headers(${PROJECT_NAME} PRIVATE src/project.h)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::XKBCOMMON PkgConfig::SYSTEMD)

#glaze
block()
    include(FetchContent)
    FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(glaze) 
    target_link_libraries(${PROJECT_NAME} PRIVATE glaze::glaze)  
endblock()




















