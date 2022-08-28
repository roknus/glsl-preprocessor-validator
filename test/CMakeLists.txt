cmake_minimum_required(VERSION 3.19)

find_package(GTest REQUIRED)

enable_testing()

set(src_files
    "main.cpp"  
    )

add_executable(glsl-preprocessor-validator-test ${src_files})

target_compile_features(glsl-preprocessor-validator-test
    PRIVATE
        cxx_std_20
    )

target_link_libraries(glsl-preprocessor-validator-test 
    PUBLIC
        GTest::gtest 
    )
    
# To allow test to include private headers
set_property(
    TARGET glsl-preprocessor-validator-test 
    PROPERTY GLSL_PREPROCESSOR_VALIDATOR_PRIVATE 1
    )

gtest_discover_tests(glsl-preprocessor-validator-test
    TEST_LIST test-list
    )

set_tests_properties(${test-list} PROPERTIES TIMEOUT 10)