add_contract(demo demo ${CMAKE_CURRENT_SOURCE_DIR}/src/demo.cpp)
target_include_directories(demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_options( demo PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )