#ifndef APP_EXCEPTION_H
#define APP_EXCEPTION_H

#include <exception>
#include <string>

// Исключение программы. Хранит текст, поясняющий возникшую ситуацию.
class AppException : public std::exception {
private:
    std::string message;

public:
    explicit AppException(const std::string& message);
    const char* what() const noexcept override;
};

#endif
