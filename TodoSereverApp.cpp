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
        else if (!method.compare("PUT")) {
            std::cerr << "Update id:" << *(--tokenizer.end()) << std::endl;
            std::cerr << "Update text:" << form.get("text") << std::endl;

        }
        else if (!method.compare("DELETE")) {
            std::cerr << "Delete id:" << *(--tokenizer.end()) << std::endl;
            size_t id = std::stoul(*(--tokenizer.end()));
            TodoServerApp::deleteTodo(id);
        }

        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("application/json");
        std::ostream& out = resp.send();

        std::cerr << TodoServerApp::readTodoList << std::endl;
        out << TodoServerApp::readTodoList << std::endl;
        out.flush();
    }
};

#include <iostream>
#include <fstream>
#include <map>
#include "MimeTypes/MimeTypes.h"

class CFileHandler : public HTTPRequestHandler {
    typedef std::map<const std::string , const std::string> TStrStrMap;
    TStrStrMap CONTENT_TYPE = {
#include "MimeTypes/MimeTypes.h"
    };

    std::string getPath (std::string& path) {
        if (path == "/")
            path = "/index.html";

        path.insert(0 , "./www");
        return path;
    }

    std::string getContentType (std::string& path) {
        std::string contentType ("text/plain");
        Poco::Path p(path);

        TStrStrMap::const_iterator i = CONTENT_TYPE.find(p.getExtension());

        if (i != CONTENT_TYPE.end())
            contentType = i->second;

        if (contentType.find("text/") != std::string::npos)
            contentType += "; charset = utf-8";

        std::cerr << path << " : " << contentType << std::endl;
        return contentType;
    }
public :

    void handleRequest (HTTPServerRequest& req , HTTPServerResponse& resp) {

        std::cerr << "Get static page: ";

        URI uri (req.getURI());
        std::string path(uri.getPath());
        std::ifstream ifs (getPath(path).c_str() , std::ifstream::in);

        if (ifs) {
            resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            resp.setContentType(getContentType(path));
            std::ostream& out = resp.send();

            char c = ifs.get();
            while (ifs.good()) {
                out << c;
                c = ifs.get();
            }
            out.flush();
        }
        else {
            resp.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
            std::ostream& out = resp.send();

            out << "File not found" << std::endl;
            out.flush();
        }
        ifs.close();
    }
};

class TodoRequestHandlerFactory : public HTTPRequestHandlerFactory {
public:
    virtual HTTPRequestHandler* createRequestHandler (const HTTPServerRequest& request) {
        if (!request.getURI().find("/api/"))
            return new CTodoHandler;
        else
            return new CFileHandler;
    }
};

void TodoServerApp::createTodo(CTodo &todo) {
    ScopedLock<Mutex> lock (todoLock);
    todoList.create(todo);
}

CTodoList& TodoServerApp::readTodoList() {
    ScopedLock<Mutex> lock (todoLock);
    return todoList;
}

void TodoServerApp::deleteTodo(size_t id) {
    ScopedLock<Mutex> lock (todoLock);
    todoList.del(id);
}

int TodoServerApp::main(const std::vector<std::string> &) {
    HTTPServerParams* pParams = new HTTPServerParams;

    pParams->setMaxQueued(100);
    pParams->setMaxThreads(16);

    HTTPServer s (new TodoRequestHandlerFactory ,ServerSocket(8000), pParams);

    s.start();
    std::cerr << "Server started" << std::endl;

    waitForTerminationRequest(); // wait for CTRL + C or kill

    std::cerr << "Shutting down ..." << std::endl;
    s.stop();

    return Application::EXIT_OK;
}



