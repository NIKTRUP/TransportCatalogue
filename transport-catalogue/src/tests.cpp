#include "include/tests.h"
#include "include/stat_reader.h"
using namespace std;

namespace tc {

    namespace tests {

    using namespace detail;
    using namespace std::filesystem;

        void TestStrToLower(){

            {
            std::string t = " AB fG hcG _ fthFH"s;
            ASSERT_EQUAL(detail::StrToLower(t), " ab fg hcg _ fthfh"s);
            }

        }

        void TestStrConcat(){
            std::vector<std::string_view> t = {"Well"sv, "Hello"sv, "World."sv, "How"sv, "you"sv, "been?"sv};
            std::string res = detail::StrConcat(t, " "s);
            std::string res2 = detail::StrConcat(t);
            ASSERT_EQUAL(res, "Well Hello World. How you been?"s)
            ASSERT_EQUAL(res2, "WellHelloWorld.Howyoubeen?"s)
        }

        void TestLeftTrim(){

            {
            std::string_view t = ""sv;
            LeftTrim(t);
            ASSERT(t.data() == ""s);

            }

            {
            std::string_view t = "abc"sv;
            LeftTrim(t);
            ASSERT(t.data() == "abc"s);
            }

            {
            std::string_view t = "    abc bc  "sv;
            LeftTrim(t);
            ASSERT(t.data() == "abc bc  "s);

            }

            {
            std::string_view t = "abc bc  "sv;
            LeftTrim(t);
            ASSERT(t.data() == "abc bc  "s);
            }
        }

        void TestRightTrim(){
            {
            std::string_view t = ""sv;
            RightTrim(t);
            ASSERT(t.data() == ""s);

            }

            {
            std::string_view t = "abc"sv;
            RightTrim(t);
            ASSERT(t.data() == "abc"s);
            }

            {
            std::string_view t = "   abc bc  "sv;
            RightTrim(t);
            ASSERT(std::string(t) == "   abc bc"s);
            }

            {
            std::string_view t = "  abc bc"sv;
            RightTrim(t);
            ASSERT(t == "  abc bc"sv);
            }
        }

        void TestTrim(){
            {
            std::string_view t = ""sv;
            Trim(t);
            ASSERT(t.data() == ""s);

            }

            {
            std::string_view t = "abc"sv;
            Trim(t);
            ASSERT(t.data() == "abc"s);

            }

            {
            std::string_view t = "    abc  "sv;
            Trim(t);
            ASSERT_EQUAL(t, "abc"sv);
            }

            {
            std::string_view t = "   abc bc  "sv;
            Trim(t);
            ASSERT_EQUAL(t , "abc bc"sv);
            }

            {
            std::string_view t = "abc bc  "sv;
            Trim(t);
            ASSERT(t == "abc bc"sv);
            }
        }

        void TestSplitIntoWords(){
            assert((SplitIntoWords(""s) == vector<string_view>{}));
            assert((SplitIntoWords("     "s) == vector<string_view>{}));
            assert((SplitIntoWords("aaaaaaa"s) == vector{"aaaaaaa"sv}));
            assert((SplitIntoWords("a"s) == vector{"a"sv}));
            assert((SplitIntoWords("a b c"s) == vector{"a"sv, "b"sv, "c"sv}));
            assert((SplitIntoWords("a    bbb   cc"s) == vector{"a"sv, "bbb"sv, "cc"sv}));
            assert((SplitIntoWords("  a    bbb   cc"s) == vector{"a"sv, "bbb"sv, "cc"sv}));
            assert((SplitIntoWords("a    bbb   cc   "s) == vector{"a"sv, "bbb"sv, "cc"sv}));
            assert((SplitIntoWords("  a    bbb   cc   "s) == vector{"a"sv, "bbb"sv, "cc"sv}));

            auto doc0 = ""s;
            auto vec1 = SplitIntoWords(doc0);
            ASSERT_EQUAL(vec1.size() , 0);
            auto doc1 = "раз два три"s;
            auto vec2 = SplitIntoWords(doc1);
            ASSERT(vec2.size() == 3 &&
                   (vec2[0] == "раз"s));
            auto doc2 = "     раз      два      три     "s;
            auto vec3 = SplitIntoWords(doc2);
            ASSERT(vec3.size() == 3 &&
                   vec3[0] == "раз"s);
        }

        void TestCheckNumberLines(){
            {
                std::stringstream ss;
                ss << " 100 \n";
                ss << " 0 \n";
                ss << " -100 \n";
                ASSERT_EQUAL(detail::CheckNumberLines(ss), 100);
                ASSERT_EQUAL(detail::CheckNumberLines(ss), 0);
                ASSERT_THROWS(detail::CheckNumberLines(ss), std::out_of_range);
            }
        }

        void TestParseTypeQueryAndName(){
            {
            std::string str = "    Stop    Tolstopaltsevo:"s;
            auto [type, name] = detail::ParseTypeQueryAndName(str);
            ASSERT_EQUAL(type, "Stop"s);
            ASSERT_EQUAL(name, "Tolstopaltsevo"s);
            }

            {
            std::string str = "    Stop    Tolstopaltsevo    :"s;
            auto [type, name] = detail::ParseTypeQueryAndName(str);
            ASSERT_EQUAL(type, "Stop"s);
            ASSERT_EQUAL(name, "Tolstopaltsevo"s);
            }

            {
            std::string str = "   Stop    Biryulyovo     Tovarnaya   :"s;
            auto [type, name] = detail::ParseTypeQueryAndName(str);
            ASSERT_EQUAL(type, "Stop"s);
            ASSERT_EQUAL(name, "Biryulyovo     Tovarnaya"s);
            }

            {
                std::string str = "Bus tUDvCLK83e652  FUYRgRXc:"s;
                auto [type, name] = detail::ParseTypeQueryAndName(str);
                ASSERT_EQUAL(type, "Bus"s);
                ASSERT_EQUAL(name, "tUDvCLK83e652  FUYRgRXc"s);
            }
        }

        void TestParseStopQuery(){
            {
            std::string str = "   55.611087, 37.208290  "s;
            auto [_, coord] = detail::ParseStopQuery(str);
            ASSERT(coord.lat - 55.611087 < 1e-6);
            ASSERT(coord.lng - 37.208290 < 1e-6);
            }

            {
            std::string str = "55.611087   ,  37.208290 "s;
            auto [_, coord] = detail::ParseStopQuery(str);
            ASSERT(coord.lat - 55.611087 < 1e-6);
            ASSERT(coord.lng - 37.208290 < 1e-6);
            }

            {
            std::string str = "55.611087   ,  37.208290"s;
            auto [_, coord] = detail::ParseStopQuery(str);
            ASSERT(coord.lat - 55.611087 < 1e-6);
            ASSERT(coord.lng - 37.208290 < 1e-6);
            }

            {
            std::string str = "55.611087,37.208290"s;
            auto [_, coord] = detail::ParseStopQuery(str);
            ASSERT(coord.lat - 55.611087 < 1e-6);
            ASSERT(coord.lng - 37.208290 < 1e-6);
            }

            {
            std::string str = "-91,60"s;
            ASSERT_THROWS(detail::ParseStopQuery(str), std::out_of_range);
            }

            {
            std::string str = "91,60"s;
            ASSERT_THROWS(detail::ParseStopQuery(str), std::out_of_range);
            }

            {
            std::string str = "50, 181"s;
            ASSERT_THROWS(detail::ParseStopQuery(str), std::out_of_range);
            }

            {
            std::string str = "50,-181"s;
            ASSERT_THROWS(detail::ParseStopQuery(str), std::out_of_range);
            }


            {
            std::string str = "90,180"s;
            ASSERT_DOESNT_THROW(detail::ParseStopQuery(str));
            }

            {
            std::string str = "-90,-180"s;
            ASSERT_DOESNT_THROW(detail::ParseStopQuery(str));
            }
        }

        void TestParseRouteQuery(){
            {
            std::string str = "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"s;
            auto [type, res] = detail::ParseRouteQuery(str);

            ASSERT(type == RouteType::CIRCLE);
            ASSERT_EQUAL(res.size(), 6);
            ASSERT_EQUAL(res[0].name, "Biryulyovo Zapadnoye"s);
            ASSERT_EQUAL(res[1].name, "Biryusinka"s);
            ASSERT_EQUAL(res[3].name, "Biryulyovo Tovarnaya"s);
            ASSERT_EQUAL(res[5].name, "Biryulyovo Zapadnoye"s);
            }

            {
            std::string str = "  Biryulyovo    Zapadnoye>Biryusinka>Universam>Biryulyovo    Tovarnaya>Biryulyovo     Passazhirskaya>Biryulyovo    Zapadnoye    "s;
            auto [type, res] = detail::ParseRouteQuery(str);

            ASSERT(type == RouteType::CIRCLE);
            ASSERT_EQUAL(res.size(), 6);
            ASSERT_EQUAL(res[0].name, "Biryulyovo    Zapadnoye"s);
            ASSERT_EQUAL(res[1].name, "Biryusinka"s);
            ASSERT_EQUAL(res[3].name, "Biryulyovo    Tovarnaya"s);
            ASSERT_EQUAL(res[5].name, "Biryulyovo    Zapadnoye"s);
            }

            {
            std::string str = "Biryulyovo Zapadnoye - Biryusinka - Universam - Biryulyovo Tovarnaya - Biryulyovo Passazhirskaya"s;
            auto [type, res] = detail::ParseRouteQuery(str);

            ASSERT(type == RouteType::LINEAR);
            ASSERT_EQUAL(res.size(), 5);
            ASSERT_EQUAL(res[0].name, "Biryulyovo Zapadnoye"s);
            ASSERT_EQUAL(res[1].name, "Biryusinka"s);
            ASSERT_EQUAL(res[3].name, "Biryulyovo Tovarnaya"s);
            ASSERT_EQUAL(res[4].name, "Biryulyovo Passazhirskaya"s);
            }

            {
            std::string str = "Biryulyovo Zapadnoye . Biryusinka . Universam . Biryulyovo Tovarnaya . Biryulyovo Passazhirskaya"s;
            ASSERT_THROWS(detail::ParseRouteQuery(str), std::invalid_argument);
            }
        }

        void TestParseOutputQuery(){
            {
            std::stringstream ss;
            ss << " 4 \n";
            ss << " Bus   256 \n"s;
            ss << " Bus   750 \n"s;
            ss << " Bus   751 \n"s;

            ss << " Bus   751 - 26  \n"s;
            auto res = ParseOutputQuery(ss);
            ASSERT_EQUAL(res.size(), 4);
            ASSERT_EQUAL(res[0].second, "256"s);
            ASSERT_EQUAL(res[1].second, "750"s);
            ASSERT_EQUAL(res[2].second, "751"s);
            ASSERT_EQUAL(res[3].second, "751 - 26"s);

            ASSERT(res[0].first == QueryType::Route);
            ASSERT(res[1].first == QueryType::Route);
            ASSERT(res[2].first == QueryType::Route);
            ASSERT(res[3].first == QueryType::Route);
            }

            {
                std::stringstream ss;
                ss << "1 \n";
                ss << "Dus 256\n";
                ASSERT_THROWS(ParseOutputQuery(ss), std::invalid_argument);
            }
        }

        void TestParseDistances(){
            TransportCatalogue catalogue;
            std::unordered_map<string, string> stop_to_distances;

            auto add_stop = [&](std::string& name, std::string& coord){
                auto [distance_stops , coordinate] = detail::ParseStopQuery(coord);
                if(!distance_stops.empty()){
                     stop_to_distances[name] = std::move(distance_stops);
                }
                catalogue.AddStop({std::move(name), std::move(coordinate)});
            };

            {
                std::string name1 = "Biryulyovo Zapadnoye"s;
                std::string coordinate1 = " 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam"s;
                add_stop(name1, coordinate1);

                std::string name2 = "Rossoshanskaya ulitsa"s;
                std::string coordinate2 = " 55.595579, 37.605757, 2500m to Biryulyovo Zapadnoye "s;
                add_stop(name2, coordinate2);


                std::string name3 = "Biryusinka"s;
                std::string coordinate3 = "  55.581065, 37.64839, 750m to Universam"s;
                add_stop(name3, coordinate3);


                std::string name4 = "Universam"s;
                std::string coordinate4 = " 55.587655, 37.645687, 250m to Biryusinka"s;
                add_stop(name4, coordinate4);

                ASSERT_EQUAL(catalogue.GetAmountStops(), 4);
                ASSERT_EQUAL(stop_to_distances.size() , 4);

                ASSERT_DOESNT_THROW(stop_to_distances.at("Biryulyovo Zapadnoye"s));
                ASSERT((catalogue.FindStop("Biryulyovo Zapadnoye")->name == "Biryulyovo Zapadnoye"s) );
                ASSERT((catalogue.FindStop("Biryusinka")->name == "Biryusinka"s) );
                ASSERT((catalogue.FindStop("Universam")->name == "Universam"s) );
                ASSERT((catalogue.FindStop("Rossoshanskaya ulitsa")->name == "Rossoshanskaya ulitsa"s) );

                std::string distanses =  "7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam"s;
                detail::ParseDistances(catalogue, stop_to_distances);

                ASSERT_EQUAL(catalogue.GetStopsDistances("Biryulyovo Zapadnoye"s, "Rossoshanskaya ulitsa"s), 7500);
                ASSERT_EQUAL(catalogue.GetStopsDistances("Rossoshanskaya ulitsa"s, "Biryulyovo Zapadnoye"s), 2500);

                ASSERT_EQUAL(catalogue.GetStopsDistances("Biryulyovo Zapadnoye"s, "Biryusinka"s), 1800);
                ASSERT_EQUAL(catalogue.GetStopsDistances("Biryusinka"s, "Biryulyovo Zapadnoye"s), 1800);

                ASSERT_EQUAL(catalogue.GetStopsDistances("Biryusinka"s, "Universam"s), 750);
                ASSERT_EQUAL(catalogue.GetStopsDistances("Universam"s, "Biryusinka"s), 250);

                ASSERT_EQUAL(catalogue.GetStopsDistances("Biryulyovo Zapadnoye"s, "Universam"s), 2400);
                ASSERT_EQUAL(catalogue.GetStopsDistances("Universam"s, "Biryulyovo Zapadnoye"s), 2400);

            }

        }

        void Test(path test_path, path standard_path, std::ostream& out_logs){
            LOG_DURATION(standard_path.filename().string());

            using namespace std::literals;

            TransportCatalogue catalogue;

            path result_path = test_path.parent_path().
                    append(test_path.stem().string()).string().
                    append("_my_result.log"s);
            std::vector<std::pair<QueryType, std::string>> queries;
            std::ifstream in(test_path);
            {
                LOG_DURATION(standard_path.filename().string() + " read data ");
                if(in.is_open()){
                    detail::ParseTxt(in, catalogue);
                    queries = ParseOutputQuery(in);
                }else{
                    std::cout << " Файл тестовых данных " << test_path.string() << " не открыт " << std::endl;
                }
            }
            in.close();
            std::ofstream out(result_path.string());
            if(out.is_open()){
                for(auto& [type ,query] : queries){
                    PrintQuery(type, query, catalogue, out);
                }
            }else{
                std::cerr << "Ошибка: файл для вывода результатов " << result_path.string() << " не открыт " << std::endl;
            }
            out.close();

            EqualLogFiles(result_path, standard_path, out_logs);
        }


        void TestProject(){
            TestRunner test;
            RUN_TEST(test, TestStrToLower);
            RUN_TEST(test, TestStrConcat);
            RUN_TEST(test, TestLeftTrim);
            RUN_TEST(test, TestRightTrim);
            RUN_TEST(test, TestTrim);
            RUN_TEST(test, TestSplitIntoWords);
            RUN_TEST(test, TestCheckNumberLines);
            RUN_TEST(test, TestParseTypeQueryAndName);
            RUN_TEST(test, TestParseStopQuery);
            RUN_TEST(test, TestParseRouteQuery);
            RUN_TEST(test, TestParseOutputQuery);
            RUN_TEST(test, TestParseDistances);
        }
    }
}
