#include <sstream>
#include <iostream>
//TODO: May we should add some system nofity?
class StringBuilder
{
public:
    template <typename T>
    inline StringBuilder &operator<<(const T &t)
    {
        mStream << t;
        return *this;
    }
    inline std::string get() const
    {
        return mStream.str();
    }
    inline operator std::string() const
    {
        return get();
    }

private:
    std::stringstream mStream;
};

namespace LOGPublicator
{
    void LogPrintSTDIO(std::string Output)
    {
        std::cout << Output;
        std::cout.flush();
    };
};