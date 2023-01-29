#include <iostream>
#include <fstream>
#include <filesystem>
#include "include/tests.h"
#include "include/input_reader.h"
#include "include/stat_reader.h"


using namespace std;
using namespace std::filesystem;

using namespace tc;


int main(){

#ifndef NDEBUG
    tests::TestProject();

    std::string out_path = "../test_result.log";
    std::ofstream out_logs(out_path);
    if(out_logs.is_open()){

        TestRunner test;
        auto tsC_case1 = [&](){ tests::Test("../tests/tsC_case/tsC_case1_input.txt"s, "../tests/tsC_case/tsC_case1_output1.txt"s, out_logs); };
        RUN_TEST(test, tsC_case1);
        //auto tsC_case2 = [&](){ Test("../tests/tsC_case/tsC_case1_input.txt"s, "../tests/tsC_case/tsC_case1_output2.txt"s, out_logs); };
        //RUN_TEST(test, tsC_case2);

    }else{
        std::cerr << "Ошибка: Log файл " << out_path << " не открыт " << std::endl;
    }
#endif


    {
        path test_path = "../tests/example.txt"s;
        TransportCatalogue catalogue;

        path result_path = test_path.root_path().string().append("tests_result").append(test_path.stem()).append("_result.log");
         std::vector<std::pair<QueryType, std::string>> queries;
        std::ifstream in(test_path);
        if(in.is_open()){
            detail::ParseTxt(in, catalogue);
            queries = ParseOutputQuery(in);
        }else{
            std::cout << " Файл " << test_path.string() << " не открыт " << std::endl;
        }

        for(auto& [type ,query] : queries){
            PrintQuery(type, query, catalogue);
        }
    }


    TransportCatalogue catalogue;
    ReadByConsole(catalogue);
    auto queries = ParseOutputQuery(std::cin);
    for(auto& [type ,query] : queries){
        PrintQuery(type, query, catalogue);
    }
    return 0;

}
