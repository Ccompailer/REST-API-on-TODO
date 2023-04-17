#include <iostream>
#include <string>
#include "TodoServerApp.h"

Mutex TodoServerApp::todoLock;
CTodoList TodoServerApp::todoList;

std::ostream& operator<< (std::ostream& os , CTodo& todo) {
    os << "{ \"_id\": " << todo.getID() << "\"text\": " << todo.getText() <<"\" }";
    return os;
}

std::ostream& operator<< (std::ostream& os , CTodoList& todoList) {

    std::map<size_t , CTodo> todos = todoList.readList();
    os << "[";
    if (!todos.empty())
        if (todos.size() == 1)
            os << todos.begin()->second;
    else {
        for (auto it = todos.begin();;) {
            os << it->second;
            if (++it != todos.end())
                os << ',';
            else
                break;
            }
    }
    os << "]\n";
    return os;
}

class CTodoHandler : public HTTPRequestHandler {
public :
    void handleRequest (HTTPServerRequest& req , HTTPServerResponse& resp) override {
    URI uri (req.getURI());
    std::string method = req.getMethod();

    std::cerr << "URI: " << uri.toString() << std::endl;
    std::cerr <<"Method: " << req.getMethod() << std::endl;

    StringTokenizer tokenizer (uri.getPath() , "/" , StringTokenizer::TOK_TRIM);
    HTMLForm form (req , req.stream());

        if (!method.compare("POST")) {
            std::cerr << "Create: " << form.get("text") <<std::endl;
            CTodo todo (form.get("text"));
            TodoServerApp::createTodo(todo);
        }

}

};





int main ()
{
    return 0;
}

