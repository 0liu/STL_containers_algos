#ifndef __EXCEPTION_H_
#define __EXCEPTION_H_

#include <cstring>
#include <string>
#include <exception>
#include <stdexcept>  // std::runtime_error


namespace ADT {

    using std::runtime_error;
    using std::string;

    // Derive base exception from std::exception and override virtual const char* std::exception::what()
    class Exception: public std::exception {
        public:
            Exception(const string& msg): _msg(msg) {}
            virtual const char* what() const noexcept override
            {
                return _msg.c_str();
            }
        private:
            string _msg;
    };

    // --- Derive exceptions directly from std::runtime_error,
    // --- which already implements virtual const char* std::exception::what().

    // Error attempting to access an element from an empty ADT.
    class Empty: public runtime_error {
        public:
            Empty(const char* msg): runtime_error(msg) {}
            Empty(const string& msg): runtime_error(msg) {}
    };

    // Error attempting to delete an element from an empty container.
    class Underflow: public runtime_error {
        public:
            Underflow(const char* msg): runtime_error(msg) {}
            Underflow(const string& msg): runtime_error(msg) {}
    };

    // Error attempting to insert an element to a full container.
    class Overflow: public runtime_error {
        public:
            Overflow(const char* msg): runtime_error(msg) {}
            Overflow(const string& msg): runtime_error(msg) {}
    };

    
}


#endif // __EXCEPTION_H_
