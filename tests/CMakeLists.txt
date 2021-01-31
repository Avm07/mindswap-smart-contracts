cmake_minimum_required( VERSION 3.5 )

# project(units)

set(EOSIO_VERSION_MIN "2.0")
set(EOSIO_VERSION_SOFT_MAX "2.0")
#set(EOSIO_VERSION_HARD_MAX "")

find_package(eosio)

### Check the version of eosio
set(VERSION_MATCH_ERROR_MSG "")
EOSIO_CHECK_VERSION(VERSION_OUTPUT "${EOSIO_VERSION}"
        "${EOSIO_VERSION_MIN}"
        "${EOSIO_VERSION_SOFT_MAX}"
        "${EOSIO_VERSION_HARD_MAX}"
        VERSION_MATCH_ERROR_MSG)
if(VERSION_OUTPUT STREQUAL "MATCH")
    message(STATUS "Using eosio version ${EOSIO_VERSION}")
elseif(VERSION_OUTPUT STREQUAL "WARN")
    message(WARNING "Using eosio version ${EOSIO_VERSION} even though it exceeds the maximum supported version of ${EOSIO_VERSION_SOFT_MAX}; continuing with configuration, however build may fail.\nIt is recommended to use eosio version ${EOSIO_VERSION_SOFT_MAX}.x")
else() # INVALID OR MISMATCH
    message(FATAL_ERROR "Found eosio version ${EOSIO_VERSION} but it does not satisfy version requirements: ${VERSION_MATCH_ERROR_MSG}\nPlease use eosio version ${EOSIO_VERSION_SOFT_MAX}.x")
endif(VERSION_OUTPUT STREQUAL "MATCH")

configure_file(${CMAKE_SOURCE_DIR}/contracts.hpp.in ${CMAKE_BINARY_DIR}/contracts.hpp)

include_directories(${CMAKE_BINARY_DIR})

### UNIT TESTING ###
include(CTest) # eliminates DartConfiguration.tcl errors at test runtime
enable_testing()

# build unit test executable
include_directories(
arbitrage
arbitrage/suites
limit
limit/suites
token
mindswap
${CMAKE_BINARY_DIR}
${CMAKE_CURRENT_SOURCE_DIR}
)

# find all unit test suites
file(GLOB ARBITRAGE_UNIT_TESTS
"*.cpp"
"*.hpp"
"./arbitrage/*.cpp"
"./arbitrage/suites/*.hpp"
"./token/*.cpp"
"./token/*.hpp"
"./limit/limit_test_api.cpp"
"./limit/limit_test_api.hpp"
"./mindswap/*.hpp"
)

# build unit tests as one executable
add_eosio_test_executable(arbitrage_unit_test ${ARBITRAGE_UNIT_TESTS})

# mark test suites for execution
foreach(TEST_SUITE ${ARBITRAGE_UNIT_TESTS}) # create an independent target for each test suite
    execute_process(COMMAND bash -c "grep -E 'BOOST_AUTO_TEST_SUITE\\s*[(]' ${TEST_SUITE} | grep -vE '//.*BOOST_AUTO_TEST_SUITE\\s*[(]' | cut -d ')' -f 1 | cut -d '(' -f 2" OUTPUT_VARIABLE SUITE_NAME OUTPUT_STRIP_TRAILING_WHITESPACE) # get the test suite name from the *.cpp file
    if (NOT "" STREQUAL "${SUITE_NAME}") # ignore empty lines
        execute_process(COMMAND bash -c "echo ${SUITE_NAME} | sed -e 's/s$//' | sed -e 's/_test$//'" OUTPUT_VARIABLE TRIMMED_SUITE_NAME OUTPUT_STRIP_TRAILING_WHITESPACE) # trim "_test" or "_tests" from the end of ${SUITE_NAME}
        # to run unit_test with all log from blockchain displayed, put "--verbose" after "--", i.e. "unit_test -- --verbose"
        add_test(NAME arbitrage_${TRIMMED_SUITE_NAME}_unit_test COMMAND arbitrage_unit_test --run_test=${SUITE_NAME} --report_level=detailed --color_output)
    endif()
endforeach(TEST_SUITE)

# find all unit test suites
file(GLOB LIMIT_UNIT_TESTS
"*.cpp"
"*.hpp"
"./limit/*.cpp"
"./limit/suites/*.hpp"
"./token/*.cpp"
"./token/*.hpp"
)

# build unit tests as one executable
add_eosio_test_executable(limit_unit_test ${LIMIT_UNIT_TESTS})

# mark test suites for execution
foreach(TEST_SUITE ${LIMIT_UNIT_TESTS}) # create an independent target for each test suite
    execute_process(COMMAND bash -c "grep -E 'BOOST_AUTO_TEST_SUITE\\s*[(]' ${TEST_SUITE} | grep -vE '//.*BOOST_AUTO_TEST_SUITE\\s*[(]' | cut -d ')' -f 1 | cut -d '(' -f 2" OUTPUT_VARIABLE SUITE_NAME OUTPUT_STRIP_TRAILING_WHITESPACE) # get the test suite name from the *.cpp file
    if (NOT "" STREQUAL "${SUITE_NAME}") # ignore empty lines
        execute_process(COMMAND bash -c "echo ${SUITE_NAME} | sed -e 's/s$//' | sed -e 's/_test$//'" OUTPUT_VARIABLE TRIMMED_SUITE_NAME OUTPUT_STRIP_TRAILING_WHITESPACE) # trim "_test" or "_tests" from the end of ${SUITE_NAME}
        # to run unit_test with all log from blockchain displayed, put "--verbose" after "--", i.e. "unit_test -- --verbose"
        add_test(NAME limit_${TRIMMED_SUITE_NAME}_unit_test COMMAND limit_unit_test --run_test=${SUITE_NAME} --report_level=detailed --color_output)
    endif()
endforeach(TEST_SUITE)