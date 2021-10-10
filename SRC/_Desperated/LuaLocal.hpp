#include <string>
#include <fstream>
#include <functional>
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

    int LuaLocalVarGet(const char *VARNAME)
    {
        lua_getglobal(LuaMainLocation, VARNAME);
        if (lua_isnumber(LuaMainLocation, -1))
        {
            return lua_tonumber(LuaMainLocation, -1);
        }
        return -1;
    }

    template <typename T>
    LuaLocal &&LuaLocalVarPush(T PushVar)
    {
        return std::move(*this);
    }

    void LuaLocalFunctionPush(const char *FunctionName, std::function<int(lua_State *)> PushFunction)
    {
        void *p = lua_newuserdata(LuaMainLocation, sizeof(PushFunction));
        new (p) std::function<int(lua_State *)>(PushFunction);
        lua_setglobal(LuaMainLocation, FunctionName);
        //
        lua_pushstring(LuaMainLocation, FunctionName);
        auto Pusher = [](lua_State *L) -> int
        {
            const char *FUNCGName = lua_tostring(L, lua_upvalueindex(1));
            lua_getglobal(L, FUNCGName);
            std::function<int(lua_State *)> CAPSTDFUNC = *static_cast<std::function<int(lua_State *)> *>(lua_touserdata(L, -1));
            return CAPSTDFUNC(L);
        };
        lua_pushcclosure(LuaMainLocation, Pusher, 1);
        std::string TMP = FunctionName;
        int size = TMP.size();
        lua_setglobal(LuaMainLocation, TMP.substr(3, size).c_str());
    }

    bool LuaLocalCall(const char *FunctionName, int args, int ReturnARGS)
    {
        lua_getglobal(LuaMainLocation, FunctionName);
        if (lua_isfunction(LuaMainLocation, -1))
        {
            lua_pcall(LuaMainLocation, args, ReturnARGS, 0);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool LuaLocalRun()
    {
        int LuaStatus = luaL_dostring(LuaMainLocation, LuaRunnerMain.c_str());
        if (LuaStatus == LUA_OK)
            return true;
        else
        {
            lua_error(LuaMainLocation);
            return false;
        }
    }

private:
    std::string LuaRunnerMain;
    lua_State *LuaMainLocation;
};
