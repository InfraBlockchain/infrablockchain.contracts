add_contract(sys.tokenabi sys.tokenabi ${CMAKE_CURRENT_SOURCE_DIR}/src/sys.tokenabi.cpp)

target_include_directories(sys.tokenabi
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(sys.tokenabi
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

#configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/sys.tokenabi.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/sys.tokenabi.contracts.md @ONLY )

#target_compile_options( sys.tokenabi PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
