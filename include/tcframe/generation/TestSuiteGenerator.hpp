#pragma once

#include <functional>
#include <map>
#include <set>
#include <string>

#include "TestSuiteGenerationListener.hpp"
#include "TestCaseNameCreator.hpp"
#include "TestCaseGenerationResult.hpp"
#include "TestCaseGenerator.hpp"
#include "TestSuiteGenerationResult.hpp"
#include "tcframe/config.hpp"
#include "tcframe/os.hpp"
#include "tcframe/testcase.hpp"

using std::function;
using std::map;
using std::set;
using std::string;

namespace tcframe {

class TestSuiteGenerator {
private:
    TestCaseGenerator* testCaseGenerator_;
    OperatingSystem* os_;
    TestSuiteGenerationListener* generationListener_;

public:
    virtual ~TestSuiteGenerator() {}

    TestSuiteGenerator(
            TestCaseGenerator* testCaseGenerator,
            OperatingSystem* os,
            TestSuiteGenerationListener* generationListener)
            : testCaseGenerator_(testCaseGenerator)
            , os_(os)
            , generationListener_(generationListener)
    {}

    TestSuiteGenerationResult generate(
            const TestSuite& testSuite,
            const ProblemConfig& problemConfig,
            const GeneratorConfig& generatorConfig) {

        generationListener_->onIntroduction();

        os_->forceMakeDir(generatorConfig.testCasesDir());

        map<string, TestCaseGenerationResult> testCaseGenerationResultByTestCaseName;
        generateTestCases(testSuite.testCases(), problemConfig, generatorConfig, testCaseGenerationResultByTestCaseName);
        return TestSuiteGenerationResult(testCaseGenerationResultByTestCaseName);
    }

private:
    void generateTestCases(
            const vector<TestGroup>& testCases,
            const ProblemConfig& problemConfig,
            const GeneratorConfig& generatorConfig,
            map<string, TestCaseGenerationResult>& testCaseGenerationResultByTestCaseName) {

        for (const TestGroup& testGroup : testCases) {
            generateTestGroup(testGroup, problemConfig, generatorConfig, testCaseGenerationResultByTestCaseName);
        }
    }

    void generateTestGroup(
            const TestGroup& testGroup,
            const ProblemConfig& problemConfig,
            const GeneratorConfig& generatorConfig,
            map<string, TestCaseGenerationResult>& testCaseGenerationResultByTestCaseName) {

        generationListener_->onTestGroupIntroduction(testGroup.id());

        for (int testCaseId = 1; testCaseId <= testGroup.testCases().size(); testCaseId++) {
            TestCase testCase = testGroup.testCases()[testCaseId - 1];
            TestCaseData testCaseData = TestCaseDataBuilder()
                    .setName(TestCaseNameCreator::createTestCaseName(problemConfig.slug(), testGroup.id(), testCaseId))
                    .setDescription(testCase.description())
                    .setConstraintGroupIds(testGroup.constraintGroupIds())
                    .build();

            generateTestCase(testCaseData, testCase.closure(), generatorConfig, testCaseGenerationResultByTestCaseName);
        }
    }

    TestCaseGenerationResult generateTestCase(
            const TestCaseData& testCaseData,
            const function<void()> testCaseClosure,
            const GeneratorConfig& generatorConfig,
            map<string, TestCaseGenerationResult>& testCaseGenerationResultByTestCaseName) {

        generationListener_->onTestCaseIntroduction(testCaseData.name());

        TestCaseGenerationResult result = testCaseGenerator_->generate(testCaseData, testCaseClosure, generatorConfig);
        testCaseGenerationResultByTestCaseName[testCaseData.name()] = result;

        generationListener_->onTestCaseGenerationResult(testCaseData.description(), result);

        return result;
    }
};

}