#include "gmock/gmock.h"

#include "tcframe/spec/io/LinesIOSegment.hpp"

using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::SizeIs;
using ::testing::StrEq;
using ::testing::Test;

namespace tcframe {

class LinesIOSegmentBuilderTests : public Test {
protected:
    vector<int> X;
    vector<int> Y;
    vector<vector<int>> Z;

    LinesIOSegmentBuilder builder;
};

TEST_F(LinesIOSegmentBuilderTests, Building_Successful) {
    LinesIOSegment* segment = builder
            .addVectorVariable(Vector::create(X, "X"))
            .addVectorVariable(Vector::create(Y, "Y"))
            .setSize(3)
            .build();

    ASSERT_THAT(segment->variables(), SizeIs(2));
    EXPECT_TRUE(segment->variables()[0]->equals(Vector::create(X, "X")));
    EXPECT_TRUE(segment->variables()[1]->equals(Vector::create(Y, "Y")));
    EXPECT_THAT(segment->size(), Eq(3));
}

TEST_F(LinesIOSegmentBuilderTests, Building_WithJaggedVector_Successful) {
    LinesIOSegment* segment = builder
            .addVectorVariable(Vector::create(X, "X"))
            .addVectorVariable(Vector::create(Y, "Y"))
            .addJaggedVectorVariable(Matrix::create(Z, "Z"))
            .setSize(4)
            .build();

    ASSERT_THAT(segment->variables(), SizeIs(3));
    EXPECT_TRUE(segment->variables()[0]->equals(Vector::create(X, "X")));
    EXPECT_TRUE(segment->variables()[1]->equals(Vector::create(Y, "Y")));
    EXPECT_TRUE(segment->variables()[2]->equals(Matrix::create(Z, "Z")));
    EXPECT_THAT(segment->size(), Eq(4));
}

TEST_F(LinesIOSegmentBuilderTests, Building_Failed_NoVariables) {
    try {
        builder
                .setSize(4)
                .build();
        FAIL();
    } catch (runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Lines segment must have at least one variable"));
    }
}

TEST_F(LinesIOSegmentBuilderTests, Building_Failed_JaggedVectorNotLast) {
    try {
        builder
                .addVectorVariable(Vector::create(Y, "Y"))
                .addJaggedVectorVariable(Matrix::create(Z, "Z"))
                .addVectorVariable(Vector::create(X, "X"))
                .setSize(5)
                .build();
        FAIL();
    } catch (runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Jagged vector can only be the last variable in a lines segment"));
    }
}

TEST_F(LinesIOSegmentBuilderTests, Building_Failed_SizeNotSet) {
    try {
        builder
                .addVectorVariable(Vector::create(X, "X"))
                .addVectorVariable(Vector::create(Y, "Y"))
                .addJaggedVectorVariable(Matrix::create(Z, "Z"))
                .build();
        FAIL();
    } catch (runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Lines segment must define vector sizes"));
    }
}

}
