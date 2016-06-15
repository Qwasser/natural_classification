#include "gtest/gtest.h"

#include "natclass/dataset.h"
#include "test_data.h"

TEST(dataset_tests, raw_data_constructor_test) {
    SimpleTestData data;

    auto d = data.getData();
    DataSet ds(d);

    EXPECT_EQ(data.getData().size(), ds.nRows());
    EXPECT_EQ(data.getData()[0].size(), ds.nCols());

    Problem p = ds.getProblem();

    for (size_t i = 0; i < ds.nRows(); ++i) {
        for (size_t j = 0; j < ds.nCols(); ++j) {
            EXPECT_EQ(p.encode(j, data.getData()[i][j]), ds.getValue(i, j));
        }
    }
}
