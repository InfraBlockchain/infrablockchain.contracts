add_contract(infra.system infra.system
        ${CMAKE_CURRENT_SOURCE_DIR}/src/infra.system.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/native.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/infrasys_block_producer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/infrasys_system_token.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/infrasys_transaction_fee.cpp
)

target_include_directories(infra.system
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(infra.system
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

#configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/infra.system.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/infra.system.contracts.md @ONLY )

#target_compile_options( infra.system PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
