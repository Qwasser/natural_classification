#include <set>
#include "gtest/gtest.h"

#include <natclass/problem_builder.h>

//! Checks if each value of given feature has unique code
void checkValues(Problem p, size_t feature_id, std::vector<std::string> values) {
    std::set<int> code_set;
    for (auto val : values) {
        int code = p.encode(feature_id, val);
        if (code_set.find(code) != code_set.end()) {
            FAIL() << "Value " << val << " has not unique code in feature " << feature_id;
        }
        code_set.insert(code);
    }
}

//! Class for wrapping test dataset
class TestData {
public:
    std::vector<std::vector<std::string>> getData() {
        return data;
    }

    //! Returns all distict values for the feature
    std::vector<std::string> getValues(size_t feature_id) {
        std::set<std::string> val_set;
        std::vector<std::string> values;

        for (size_t i = 0; i < data.size(); ++i) {
            std::string val = data[i][feature_id];
            if(val_set.find(val) == val_set.end()) {
                values.push_back(val);
                val_set.insert(val);
            }
        }

        return values;
    }

    //! Returns all distinct values in dataset
    std::vector<std::string> getAllValues() {
        std::set<std::string> val_set;
        std::vector<std::string> values;

        for (auto vec : data) {
            for (auto val : vec) {
                if(val_set.find(val) == val_set.end()) {
                    values.push_back(val);
                    val_set.insert(val);
                }
            }
        }

        return values;
    }

protected:
    std::vector<std::vector<std::string>> data;
};

//! Hardcoded test data
class SimpleTestData : public TestData {
public:
    SimpleTestData () {
        data.push_back({"a", "b", "d", "e"});
        data.push_back({"a", "c", "e", "e"});
        data.push_back({"b", "c", "f", "f"});
        data.push_back({"b", "b", "d", "e"});
    }
};


TEST(problem_builder_test, add_feature_value_test) {
    ProblemBuilder pb;

    EXPECT_EQ(pb.getFeatureCount(), 0);
    pb.addFeature();
    EXPECT_EQ(pb.getFeatureCount(), 1);

    pb.addValue(0, "a");
    pb.addValue(0, "a");
    pb.addValue(0, "b");

    Problem p = pb.getResult();
    std::vector<std::string> vals = {"a", "b"};
    checkValues(p, 0, vals);

    pb.addFeatureWithValues(vals);
    p = pb.getResult();
    checkValues(p, 0, vals);

    pb.removeFeature(0);
    p = pb.getResult();
    EXPECT_EQ(p.getFeatureCount(), 1);
}

TEST(problem_builder_test, non_rectangular_dataset_test) {
    std::vector<std::vector<std::string>> data;
    data.push_back({"a", "b"});
    data.push_back({"a"});

    try {
       ProblemBuilder pb;
       pb.fromData(data);
       FAIL() << "Expected std::logic_error";
    }

    catch(std::logic_error const & err) {
       EXPECT_EQ(err.what(),std::string("Dataset is not rectangular!"));
    }
}

TEST(problem_builder_test, from_data_separate_test) {
    TestData data = SimpleTestData();

    ProblemBuilder pb;
    pb.fromData(data.getData(), ProblemBuilder::SEPARATE_VALUES);

    Problem p = pb.getResult();

    EXPECT_EQ(p.getFeatureCount(), data.getData()[0].size());

    for (size_t i = 0; i < p.getFeatureCount(); ++i) {
        checkValues(p, i, data.getValues(i));

        try {
           checkValues(p, i, data.getAllValues());
           FAIL() << "Expected std::out_of_range";
        }
        catch(std::out_of_range const & err) {

        }
        catch(std::exception const & err) {
            FAIL() << err.what();
        }
    }
}

TEST(problem_builder_test, from_data_common_test) {
    TestData data = SimpleTestData();

    ProblemBuilder pb;
    pb.fromData(data.getData(), ProblemBuilder::COMMON_VALUES);

    Problem p = pb.getResult();

    EXPECT_EQ(p.getFeatureCount(), data.getData()[0].size());

    for (size_t i = 0; i < p.getFeatureCount(); ++i) {
        checkValues(p, i, data.getAllValues());
    }
}

TEST(problem_builder_test, result_constancy_test) {
    TestData data = SimpleTestData();

    ProblemBuilder pb;
    pb.fromData(data.getData(), ProblemBuilder::COMMON_VALUES);

    Problem p1 = pb.getResult();
    Problem p2 = pb.getResult();

    EXPECT_TRUE(p1 == p2);

    pb.addValue(0, "new_val");
    if (!p2.containsValue(0, "new_val")) {
        EXPECT_FALSE(p2 == pb.getResult());
    }
}

