find_package(Qt6 REQUIRED COMPONENTS Test Sql)

set(TEST_SOURCES
  ${CMAKE_SOURCE_DIR}/src/db/Database.cpp
  ${CMAKE_SOURCE_DIR}/src/db/Schema.cpp
  ${CMAKE_SOURCE_DIR}/src/db/UserRepository.cpp
  ${CMAKE_SOURCE_DIR}/src/db/StockRepository.cpp
  ${CMAKE_SOURCE_DIR}/src/db/PortfolioRepository.cpp
  ${CMAKE_SOURCE_DIR}/src/db/TradeRepository.cpp
  ${CMAKE_SOURCE_DIR}/src/db/PriceHistoryRepository.cpp
  ${CMAKE_SOURCE_DIR}/src/engine/TradeEngine.cpp)

add_executable(test_models
   test_models.cpp
   ${TEST_SOURCES})

target_link_libraries(test_models PRIVATE
   Qt6::Test
   Qt6::Sql
)

target_include_directories(test_models PRIVATE
   ${CMAKE_SOURCE_DIR}/src
)

add_test(NAME test_models COMMAND test_models)

add_executable(test_tradeengine
    test_tradeengine.cpp
    ${TEST_SOURCES})

target_link_libraries(test_tradeengine PRIVATE Qt6::Test Qt6::Sql)
target_include_directories(test_tradeengine PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME test_tradeengine COMMAND test_tradeengine)