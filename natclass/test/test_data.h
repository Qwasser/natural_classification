#ifndef TEST_DATA_H
#define TEST_DATA_H

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
        data.push_back({"a", "b", "d", "e", "e"});
        data.push_back({"a", "c", "e", "e", "e"});
        data.push_back({"b", "c", "f", "f", "e"});
        data.push_back({"b", "b", "d", "e", "e"});
    }
};

#endif
