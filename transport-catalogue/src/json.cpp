#include "include/json.h"

using namespace std;

namespace json {

namespace {

    using Number = std::variant<int, double>;

    Node LoadNode(istream& input);

    Node LoadArray(istream& input) {
        Array result;
        char c = '!';
        for (; input >> c && c != ']';) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }

        if (c != ']') {
            throw ParsingError("пропуск ']' в массиве ");
        }
        return Node(move(result));
    }

    Node LoadNumber(std::istream& input) {
        using namespace std::literals;

        std::string parsed_num;

        // Считывает в parsed_num очередной символ из input
        auto read_char = [&parsed_num, &input] {
            parsed_num += static_cast<char>(input.get());
            if (!input) {
                throw ParsingError(" Ошибка чтения числа"s);
            }
        };

        // Считывает одну или более цифр в parsed_num из input
        auto read_digits = [&input, read_char] {
            if (!std::isdigit(input.peek())) {
                throw ParsingError(" Ожидалось число "s);
            }
            while (std::isdigit(input.peek())) {
                read_char();
            }
        };

        if (input.peek() == '-') {
            read_char();
        }
        // Парсим целую часть числа
        if (input.peek() == '0') {
            read_char();
            // После 0 в JSON не могут идти другие цифры
        } else {
            read_digits();
        }

        bool is_int = true;
        // Парсим дробную часть числа
        if (input.peek() == '.') {
            read_char();
            read_digits();
            is_int = false;
        }

        // Парсим экспоненциальную часть числа
        if (int ch = input.peek(); ch == 'e' || ch == 'E') {
            read_char();
            if (ch = input.peek(); ch == '+' || ch == '-') {
                read_char();
            }
            read_digits();
            is_int = false;
        }

        try {
            if (is_int) {
                // Сначала пробуем преобразовать строку в int
                try {
                    return Node(std::stoi(parsed_num));
                } catch (...) {
                    // В случае неудачи, например, при переполнении,
                    // код ниже попробует преобразовать строку в double
                }
            }
            return Node(std::stod(parsed_num));
        } catch (...) {
            throw ParsingError(" Ошибка преобразования "s + parsed_num + " в число "s);
        }
    }

    // Считывает содержимое строкового литерала JSON-документа
    // Функцию следует использовать после считывания открывающего символа ":
    Node LoadString(std::istream& input) {
        using namespace std::literals;

        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        std::string s;
        while (true) {
            if (it == end) {
                // Поток закончился до того, как встретили закрывающую кавычку?
                throw ParsingError("String parsing error");
            }
            const char ch = *it;
            if (ch == '"') {
                // Встретили закрывающую кавычку
                ++it;
                break;
            } else if (ch == '\\') {
                // Встретили начало escape-последовательности
                ++it;
                if (it == end) {
                    // Поток завершился сразу после символа обратной косой черты
                    throw ParsingError("String parsing error");
                }
                const char escaped_char = *(it);
                // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                }
            } else if (ch == '\n' || ch == '\r') {
                // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                throw ParsingError("Unexpected end of line"s);
            } else {
                // Просто считываем очередной символ и помещаем его в результирующую строку
                s.push_back(ch);
            }
            ++it;
        }

        return Node(s);
    }

    Node LoadDict(istream& input) {
        Dict result;
        char c; // Initialize to compare with '}'
        for (; input >> c && c != '}';) {
            if (c == ',') {
                input >> c;
            }
            string key = LoadString(input).AsString();
            input >> c;
            result.insert({move(key), LoadNode(input)});
        }
        if (c != '}') {
            throw ParsingError("Miss '}' at the end of dictionary");
        }
        return Node(move(result));
    }

    std::string GetSymbols(istream& input, size_t n){
        char ch;
        std::string out;
        out.reserve(n);
        for (size_t i = 0; i < n && input >> ch; ++i, out += ch){}
        return out;
    }

    Node LoadNull(istream& input) {

        static const std::string STRING_NULL = "null"s;
        auto res = GetSymbols(input, STRING_NULL.size());

        if (res == STRING_NULL) {
            return Node(nullptr);
        }else{
            throw ParsingError(" Ошибка чтения \"Null\""s);
        }

        return Node{};
    }

    Node LoadBool(istream& input) {
        std::string result;

        static const std::string STRING_TRUE = "true"s;
        static const std::string STRING_FALSE = "false"s;

        if (input.peek() == 't') {
            result = GetSymbols(input, STRING_TRUE.size());
        } else {
            result = GetSymbols(input, STRING_FALSE.size());
        }

        if (result == STRING_TRUE) {
            return Node(true);
        } else if (result == STRING_FALSE) {
            return Node(false);
        } else {
            throw ParsingError(" Ошибка чтения \"Bool\" ");
        }
    }

    Node LoadNode(istream& input) {
        char c;
        input >> c;

        if (c == '[') {
            return LoadArray(input);
        } else if (c == '{') {
            return LoadDict(input);
        } else if (c == '"') {
            return LoadString(input);
        }else if (c == 'n') {
            input.putback(c);
            return LoadNull(input);
        } else if (c == 't' || c == 'f') {
            input.putback(c);
            return LoadBool(input);
        } else {
            input.putback(c);
            return LoadNumber(input);
        }
    }
}  // namespace

//------------AS-Functions-----------------------
const Array &Node::AsArray() const {
    if (!IsArray()) {
        throw logic_error(" Это не \" Array \" ");
    }
    return get<Array>(*this);
}

const Dict &Node::AsMap() const {
    if (!IsMap()) {
        throw logic_error(" Это не \" Dict \" ");
    }
    return get<Dict>(*this);
}

const std::string &Node::AsString() const {
    if (!IsString()) {
        throw logic_error(" Это не \" string \" ");
    }
    return get<string>(*this);
}

int Node::AsInt() const {
    if (!IsInt()) {
        throw logic_error(" Это не \" int \" ");
    }
    return get<int>(*this);
}

double Node::AsDouble() const {
    if (!IsDouble()) {
        throw std::logic_error(" Это не \" double \" "s);
    }
    return IsPureDouble() ? std::get<double>(*this) : AsInt();
}

bool Node::AsBool() const {
    if (!IsBool()) {
        throw logic_error(" Это не \" bool \" "s);
    }
    return get<bool>(*this);
}

//------------IS-Functions-----------------------
bool Node::IsInt() const {
    return std::holds_alternative<int>(*this);
}

bool Node::IsDouble() const {
    return IsPureDouble() || IsInt();
}

bool Node::IsPureDouble() const {
    return std::holds_alternative<double>(*this);
}

bool Node::IsString() const {
    return std::holds_alternative<std::string>(*this);
}

bool Node::IsBool() const {
    return std::holds_alternative<bool>(*this);
}

bool Node::IsNull() const {
    return std::holds_alternative<nullptr_t>(*this);
}

bool Node::IsArray() const {
    return std::holds_alternative<Array>(*this);
}

bool Node::IsMap() const {
    return std::holds_alternative<Dict>(*this);
}
//-----------------------------------------------


const Node::Value& Node::GetValue() const { return *this; }

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void PrintNode(const Node& node, std::ostream& out);

struct PrintingNode {
       std::ostream& out;

       void operator()(int value) const {
           out << value;
       }

       void operator()(double value) const {
           out << value;
       }

       void operator()(bool value) const {
           if (value) {
               out << "true"s;
           } else {
               out << "false"s;
           }
       }

       void operator()(const std::string& str) const {

           static const std::unordered_map<char, std::string> symbols = {
               {'\n', "\\n"s}, {'\r' ,"\\r"s}, {'\"', "\\\""s}, {'\\' ,"\\\\"s }};

           out << '"';
           for (const auto& symbol : str) {
               auto it = symbols.find(symbol);
               if (it != symbols.end() ) {
                   out << it->second;
               } else {
                   out << symbol;
               }
           }
           out << '"';
       }

       void operator()(Array array) const {
           bool first = true;
           out << '[';
           for (const Node& node : array) {
               if(first){
                   PrintNode(node, out);
                   first = false;
                   continue;
               }
               out << ',';
               PrintNode(node, out);
           }
           out << ']';
       }

       void operator()(Dict value) const {
           bool first = true;
           out << '{';
           for (const auto& [key, node] : value) {
               if(first){
                   out << '"' << key << "\": ";
                   PrintNode(node, out);

                   first = false;
                   continue;
               }
               out << ',';
               out << '"' << key << "\": ";
               PrintNode(node, out);
           }
           out << '}';
       }

       void operator()(std::nullptr_t) const {
           out << "null"s;
       }
   };


void PrintNode(const Node& node, std::ostream& out) {
      std::visit(PrintingNode{out}, node.GetValue());
}

void Print(const Document& doc, std::ostream& output) {
  PrintNode(doc.GetRoot(), output);
}

bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}

bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.GetValue() == rhs.GetValue();
}

bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

}  // namespace json
