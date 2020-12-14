add_contract(arbitrage arbitrage ${CMAKE_CURRENT_SOURCE_DIR}/src/arbitrage.cpp)
target_include_directories(arbitrage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(arbitrage PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_options( arbitrage PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )