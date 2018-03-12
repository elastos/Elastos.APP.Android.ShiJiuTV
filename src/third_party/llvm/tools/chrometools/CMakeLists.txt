# Since tools/clang isn't actually a subdirectory, use the two argument version
# to specify where build artifacts go. CMake doesn't allow reusing the same
# binary dir for multiple source dirs, so the build artifacts have to go into a
# subdirectory...
add_subdirectory(${CHROMIUM_TOOLS_SRC} ${CMAKE_CURRENT_BINARY_DIR}/a)
