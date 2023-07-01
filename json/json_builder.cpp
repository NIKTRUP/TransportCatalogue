#include "../json/json_builder.h"

namespace json{

    using std::string_literals::operator""s;

    Builder::KeyItemContext Builder::Key( const std::string& key ){
        ThrowReadyException();

        if(!nodes_stack_.back()->IsDict()){
            throw std::logic_error("Ошибка: Использование метода Key вне контейнера Dict");
        }else if(key_is_used){
            throw std::logic_error("Ошибка: Использование метода Key подряд");
        }

        const_cast<Dict&>(nodes_stack_.back()->AsDict())[key];
        last_key_ = key;
        key_is_used = true;
        return *this;
    }

    Builder& Builder::Value(json::Node&& value){
        ThrowReadyException();
        if(!nodes_stack_.empty() && nodes_stack_.back()->IsDict() && !key_is_used){
            throw std::logic_error("Вызов Builder::Value в неположенном месте");
        }

        if (root_ == nullptr){
            root_ = value;
        }else{
            if(nodes_stack_.back()->IsArray()){
                const_cast<Array &>(nodes_stack_.back()->AsArray()).emplace_back(value);
            }else if(nodes_stack_.back()->IsDict()){
                if(!last_key_.has_value()){
                    throw std::logic_error("Вызов Builder::Value в неположенном месте");
                }
                const_cast<Dict&>(nodes_stack_.back()->AsDict())[last_key_.value()] = value;
                key_is_used = false;
            }else{
                throw std::logic_error("Вызов Builder::Value в неположенном месте");
            }
        }
        return *this;
    }

     Builder::DictItemContext Builder::StartDict(){
         ThrowReadyException();

         PushStart<Dict>();
         key_is_used = false;
         last_key_ = std::nullopt;
         return *this;
    }

    Builder& Builder::EndDict(){
        ThrowReadyException();
        if(!nodes_stack_.back()->IsDict()){
            throw std::logic_error("Вызов EndDict в контексте другого контейнера");
        }
        nodes_stack_.pop_back();
        key_is_used = false;
        return *this;
    }

    Builder::ArrayItemContext Builder::StartArray(){
        ThrowReadyException();

        PushStart<Array>();
        key_is_used = false;
        return *this;
    }

    Builder& Builder::EndArray(){
        ThrowReadyException();
        if(!nodes_stack_.back()->IsArray()){
            throw std::logic_error("Вызов EndArray в контексте другого контейнера");
        }

        nodes_stack_.pop_back();
        key_is_used = false;
        return *this;
    }

    json::Node Builder::Build(){
        if(!ObjectIsReady()){
            throw std::logic_error("Ошибка: Объект не готов к сборке");
        }
        key_is_used = false;
        return root_;
    }

    bool Builder::ObjectIsReady(){
        return nodes_stack_.empty() && root_ != nullptr;
    }

    void Builder::ThrowReadyException(){
        if(ObjectIsReady()){
            throw std::logic_error("Ошибка: Для готового объекта можно вызвать только метод Build");
        }
    }
}
