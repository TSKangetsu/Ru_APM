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

    template <typename T, int StackOffset>
    LuaLocal &&LuaLocalVarPush(T PushVar)
    {
        return std::move(*this);
    }

    LuaLocal &&LuaLocalFunctionPush(const char *FunctionName, std::function<int(lua_State *)> PushFunction)
    {
        void *p = lua_newuserdata(LuaMainLocation, sizeof(PushFunction));
        new (p) std::function<int(lua_State *)>(PushFunction);
        lua_setglobal(LuaMainLocation, FunctionName);
        //
        lua_pushstring(LuaMainLocation, FunctionName);
        auto Pusher = [](lua_State *L) -> int {
            const char *FUNCGName = lua_tostring(L, lua_upvalueindex(1));
            lua_getglobal(L, FUNCGName);
            std::function<int(lua_State *)> CAPSTDFUNC = *static_cast<std::function<int(lua_State *)> *>(lua_touserdata(L, -1));
            return CAPSTDFUNC(L);
        };
        lua_pushcclosure(LuaMainLocation, Pusher, 1);
        char *FunctionNameLUA;
        strncpy(FunctionNameLUA, FunctionName + 3, 20);
        lua_setglobal(LuaMainLocation, FunctionNameLUA);
        //
        return std::move(*this);
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
