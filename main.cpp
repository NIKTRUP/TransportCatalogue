
#include <iostream>
#include <fstream>
#include <filesystem>
#include "include/tests.h"
#include "include/stat_reader.h"
#include "svg/svg.h"

using namespace std;
using namespace std::filesystem;
using namespace svg;
using namespace tc;


int main(){

#ifndef NDEBUG

    {
        LOG_DURATION("TestProject");
        tests::TestProject();
    }
    std::string out_path = "../test_difference.log";
    std::filesystem::remove(out_path);
    std::ofstream out_logs(out_path, std::ios::app);
    {

        if(out_logs.is_open()){

            TestRunner test;
            auto tsC_case1 = [&](){ tests::TestTxtFiles("../tests/tsC_case/tsC_case1_input.txt"s, "../tests/tsC_case/tsC_case1_output1.txt"s, out_logs); };
            RUN_TEST(test, tsC_case1);

        }else{
            std::cerr << "Ошибка: Log файл " << out_path << " не открыт " << std::endl;
        }

    }

    {
        if(out_logs.is_open()){
            TestRunner test;
            auto example1 = [&](){ tests::TestJsonFiles("../examples/example1/example1.json"s, "../examples/example1/output1.json"s, out_logs); };
            RUN_TEST(test, example1);

        }else{
            std::cerr << "Ошибка: Log файл " << out_path << " не открыт " << std::endl;
        }
    }

    {
        if(out_logs.is_open()){
            TestRunner test;
            auto example2 = [&](){ tests::TestJsonFiles("../examples/example2/example2.json"s, "../examples/example2/output2.json"s, out_logs); };
            RUN_TEST(test, example2);

        }else{
            std::cerr << "Ошибка: Log файл " << out_path << " не открыт " << std::endl;
        }
    }

    {
        if(out_logs.is_open()){
            TestRunner test;
            auto example3 = [&](){ tests::TestJsonFiles("../examples/example3/example3.json"s, "../examples/example3/output3.json"s, out_logs); };
            RUN_TEST(test, example3);

        }else{
            std::cerr << "Ошибка: Log файл " << out_path << " не открыт " << std::endl;
        }
    }
    out_logs.close();
#endif

    TransportCatalogue catalogue;
    auto [out, settings] = ReadFile("../examples/example1/example1.json"s, catalogue);
    //auto [out, settings] = detail::ParseJson(std::cin, catalogue);
    DictRenderer render(catalogue, settings);
    PrintQuery(out, catalogue, render, std::cout);
    return 0;
}
