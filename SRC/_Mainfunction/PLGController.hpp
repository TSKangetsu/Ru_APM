#pragma once
#include "UORBMessage.hpp"
#include "../../User/CPP/APSUser.hpp"
#include "../_Excutable/LuaLocal.hpp"
#include "../_Excutable/FlowController.hpp"

enum PLGScriptType
{
	Lua,
	None,
	Python,
};

struct PLGSettings
{
	PLGScriptType ScriptType = None;
};

class PLGController_t : public RuAPSSys::UORBMessage
{
private:
public:
	PLGController_t(PLGSettings);
	~PLGController_t();
};

PLGController_t::PLGController_t(PLGSettings config)
{
}

PLGController_t::~PLGController_t()
{
}
