#ifndef TESTS_H
#define TESTS_H

#include "filesystem"
#include "../include/log_duration.h"
#include "../include/test_framework.h"

namespace transport {

    namespace tests {

        void TestStrToLower();

        void TestLeftTrim();

        void TestRightTrim();

        void TestTrim();

        void TestSplitIntoWords();

        void TestCheckNumberLines();

        void TestParseTypeQueryAndName();

        void TestParseStopQuery();

        void TestParseRouteQuery();

        void TestParseOutputQuery();

        void TestParseDistances();

        void TestSetDistance();

        void TestJSON();

        void TestTxtFiles(const std::filesystem::path& test_path, const std::filesystem::path& standard_path, std::ostream& out_logs = std::cout);

        void TestJsonFiles(const std::filesystem::path& test_path, const std::filesystem::path& standard_path, std::ostream& out_logs = std::cout);

        void TestSvgFiles(const std::filesystem::path& test_path, const std::filesystem::path& standard_path, std::ostream& out_logs);

        std::vector<double> LoadRouteTime(std::istream& in);

        std::vector<double> LoadRouteTime(const std::filesystem::path& test_path);

        void CheckFindingOptimalRoute(const std::filesystem::path& test_in, const std::filesystem::path& standard_out, const std::filesystem::path& out_log);

        void TestFindingOptimalRoute();

        void TestProject();

        void TestExamples();
    }
}

#endif // TESTS_H
