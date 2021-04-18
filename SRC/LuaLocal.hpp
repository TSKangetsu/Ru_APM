#include <functional>
#include <initializer_list>
extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class LuaLocal
{
public:
    void LuaLocalInit()
    {
        LuaMainLocation = luaL_newstate();
        luaL_openlibs(LuaMainLocation);
    };

    void LuaLocalLoad(char *filename)
    {
        std::ifstream config(filename);
        std::string content((std::istreambuf_iterator<char>(config)),
                            (std::istreambuf_iterator<char>()));
        LuaRunnerMain = content;
    }

    template <typename T>
    LuaLocal &&LuaLocalVarPush(T PushFunction)
    {
        return std::move(*this);
    }

    template <typename Return>
    LuaLocal &&LuaLocalFunctionPush(std::string FunctionName, std::function<Return()> PushFunction)
    {
        void *p = lua_newuserdata(LuaMainLocation, sizeof(PushFunction));
        luaL_setmetatable(LuaMainLocation, FunctionName.c_str());
        new (p) std::function<Return()>(PushFunction);
        auto Pusher = [](lua_State *L) -> Return {
            //=================================//
            std::function<Return()> CAPSTDFUNC =
                *static_cast<std::function<Return()> *>(lua_touserdata(L, -3));
            //=================================//
            Return result = CAPSTDFUNC();
            lua_pushinteger(L, result);
            return 1;
        };
        lua_register(LuaMainLocation, FunctionName.c_str(), Pusher);
        return std::move(*this);
    }

    bool LuaLocalRun()
    {
        int LuaStatus = luaL_dostring(LuaMainLocation, LuaRunnerMain.c_str());
        if (LuaStatus == LUA_OK)
            return true;
        else
            return false;
    }

private:
    std::string LuaRunnerMain;
    lua_State *LuaMainLocation;
};
