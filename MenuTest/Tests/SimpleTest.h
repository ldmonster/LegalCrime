#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

namespace SimpleTest {
    
    struct TestResult {
        std::string testName;
        bool passed;
        std::string errorMessage;
    };
    
    class TestRunner {
    private:
        static inline std::vector<std::pair<std::string, std::function<TestResult()>>> tests;
        static inline std::vector<TestResult> results;
        
    public:
        static void Register(const std::string& name, std::function<TestResult()> test) {
            tests.push_back({name, test});
        }
        
        static int RunAll() {
            std::cout << "\n=== TileMap Test Suite ===\n\n";
            
            for (const auto& [name, test] : tests) {
                auto result = test();
                results.push_back(result);
                
                if (result.passed) {
                    std::cout << "[PASS] " << result.testName << "\n";
                } else {
                    std::cout << "[FAIL] " << result.testName << "\n";
                    std::cout << "       Error: " << result.errorMessage << "\n";
                }
            }
            
            // Summary
            int passed = 0;
            int failed = 0;
            for (const auto& result : results) {
                if (result.passed) passed++;
                else failed++;
            }
            
            std::cout << "\n=== Test Summary ===\n";
            std::cout << "Passed: " << passed << "\n";
            std::cout << "Failed: " << failed << "\n";
            std::cout << "Total:  " << (passed + failed) << "\n\n";
            
            return failed == 0 ? 0 : 1;
        }
    };
    
    #define ASSERT_EQUAL(actual, expected) \
        if ((actual) != (expected)) { \
            return SimpleTest::TestResult{__FUNCTION__, false, \
                "Expected " + std::to_string(expected) + ", got " + std::to_string(actual)}; \
        }
    
    #define ASSERT_TRUE(condition) \
        if (!(condition)) { \
            return SimpleTest::TestResult{__FUNCTION__, false, "Condition is false"}; \
        }
    
    #define ASSERT_FALSE(condition) \
        if ((condition)) { \
            return SimpleTest::TestResult{__FUNCTION__, false, "Condition is true"}; \
        }
    
    #define ASSERT_NOT_NULL(ptr) \
        if ((ptr) == nullptr) { \
            return SimpleTest::TestResult{__FUNCTION__, false, "Pointer is null"}; \
        }
    
    #define ASSERT_NULL(ptr) \
        if ((ptr) != nullptr) { \
            return SimpleTest::TestResult{__FUNCTION__, false, "Pointer is not null"}; \
        }

    #define TEST_CASE(name) \
        SimpleTest::TestResult name##_impl(); \
        namespace { static struct name##_register { \
            name##_register() { \
                SimpleTest::TestRunner::Register(#name, &name##_impl); \
            } \
        } name##_reg; } \
        SimpleTest::TestResult name##_impl()

}

