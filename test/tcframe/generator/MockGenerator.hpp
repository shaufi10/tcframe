#pragma once

#include "gmock/gmock.h"

#include "tcframe/generator/Generator.hpp"

namespace tcframe {

class MockGenerator : public Generator {
public:
    MockGenerator()
            : Generator(nullptr, nullptr, nullptr, nullptr, nullptr) {}

    MOCK_METHOD2(generate, bool(const TestSuite&, const GeneratorConfig&));
};

class MockGeneratorFactory : public GeneratorFactory {
public:
    MOCK_METHOD5(create, Generator*(SeedSetter*, TestCaseGenerator*, Verifier*, OperatingSystem*, GeneratorLogger*));
};

}
