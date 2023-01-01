#ifndef StringsHelper_H
#define StringsHelper_H

#include <string>


class StringsHelper
{

protected:

    StringsHelper();

    ~StringsHelper();

public:

    template<typename ... Args>
    static std::string string_format(const std::string& format, Args ... args);

};

#endif