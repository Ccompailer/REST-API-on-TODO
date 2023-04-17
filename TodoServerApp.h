#pragma once

#include <Poco/Mutex.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Path.h>
#include <Poco/ScopedLock.h>
#include <Poco/StringTokenizer.h>
#include <Poco/URI.h>
#include <Poco/Util/ServerApplication.h>

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

class CTodo {
    size_t id;
    std::string text;

public:
    CTodo(std::string text) : text(text){}
    size_t getID() {return id;}
    void setID(size_t id) { this->id = id;}
    std::string getText() {return text;}
};

class CTodoList {
    size_t id;
    std::map<size_t , CTodo> todos;

public:
    CTodoList() : id(0){}
    void create(CTodo& todo) {todo.setID(++id); todos.insert(std::pair<size_t , CTodo> (id , todo));}
    std::map<size_t , CTodo>& readList() {return todos;}
    void del(size_t id) {todos.erase(id);}
};

class TodoServerApp : public ServerApplication {
public:
    static void createTodo(CTodo& todo);
    static CTodoList& readTodoList();
    static void deleteTodo(size_t id);
protected:
    int main(const std::vector<std::string>&);
    static Mutex todoLock;
    static CTodoList todoList;
};
