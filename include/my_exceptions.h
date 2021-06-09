#ifndef THEOPGAME_MY_EXCEPTIONS_H
#define THEOPGAME_MY_EXCEPTIONS_H
#include <exception>
#include <string>

class MyException: public std::exception{
    const char* error;
public:
    MyException(const char* e) : error(e) {}
    const char* what() const throw () {
        return error;
    }
};

#endif //THEOPGAME_MY_EXCEPTIONS_H
