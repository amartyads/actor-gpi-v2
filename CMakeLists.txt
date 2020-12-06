cmake_minimum_required(VERSION 3.13)

project(ActorGPI)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake-Aux/Modules/")

find_package (GPI REQUIRED)

## Get information about the git revision.
# the commit's SHA1, and whether the building workspace was dirty or not
execute_process(COMMAND
    "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_SHA1
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
)

# the date of the commit
execute_process(COMMAND
    "${GIT_EXECUTABLE}" log -1 --format=%ad --date=local
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_DATE
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
)

# the subject of the commit
execute_process(COMMAND
    "${GIT_EXECUTABLE}" log -1 --format=%s
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_COMMIT_SUBJECT
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
)

#set(CMAKE_REQUIRED_DEFINITIONS ${GPI_COMPILE_FLAGS})
set(CMAKE_REQUIRED_INCLUDES ${GPI_INCLUDE_PATH})
set(CMAKE_REQUIRED_LIBRARIES ${GPI_LIBRARIES})

add_library(actorlib)

add_executable(pingpong_example)
add_executable(primes_example)

add_subdirectory(src/actorlib)
add_subdirectory(src/examples/pingpong)
add_subdirectory(src/examples/primes)

#target_link_libraries(actorlib PRIVATE OpenMP::OpenMP_CXX)
message(STATUS "Gaspi at : ${GPI_INCLUDE_PATH}")
target_compile_definitions(actorlib PRIVATE ACTORLIB_GIT_REVISION=${GIT_SHA1})
target_compile_definitions(actorlib PRIVATE ACTORLIB_GIT_DATE=${GIT_DATE})
target_compile_definitions(actorlib PRIVATE ACTORLIB_GIT_COMMIT_MSG=${GIT_COMMIT_SUBJECT})

if (ENABLE_MEMORY_SANITATION AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    message(STATUS "Memory sanitation enabled. Linking appropriate libraries.")
    target_compile_options(actorlib PUBLIC -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined -fsanitize-address-use-after-scope -Wuninitialized -Wall -Wextra -pedantic)
    target_link_libraries(actorlib PUBLIC -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined)
    # For some old Ubuntu machines, you may need to use a different linker. Set the following variable: CMAKE_EXE_LINKER_FLAGS=qq-fuse-ld=gold
elseif(ENABLE_MEMORY_SANITATION)
    message(STATUS "Full Memory sanitation only supported on GNU Environment. Trying to use intel fallback.")
    target_compile_options(actorlib PUBLIC -g -traceback -check-uninit)
    target_link_libraries(actorlib PUBLIC -g -traceback -check-uninit)
    
endif()

target_include_directories(actorlib PUBLIC ${GPI_INCLUDE_PATH})
target_link_libraries(actorlib PUBLIC ${GPI_LIBRARIES})

target_include_directories(pingpong_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(primes_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(actorlib PUBLIC -pthread)
target_link_libraries(actorlib PUBLIC -libverbs)

#set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lGPI2 -lpthread -libverbs")

target_link_libraries(pingpong_example PUBLIC actorlib)
target_link_libraries(primes_example PUBLIC actorlib)

set_property(TARGET actorlib PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET actorlib PROPERTY CXX_STANDARD 14)

set_property(TARGET pingpong_example PROPERTY CXX_STANDARD 14)
set_property(TARGET primes_example PROPERTY CXX_STANDARD 14)

# Export compiler commands for use in YouCompleteMe
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
