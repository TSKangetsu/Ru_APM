#pragma once
#include <unistd.h>
#include "../ToolsInclude/M_Tools.hpp"
#include "../RPiSingleAPM/src/SingleAPM.hpp"
#include <libmill.h>

namespace APMMain
{
	class APMControl : SingleAPMAPI::RPiSingleAPM
	{

	};
	
	class APMNetWorkControl
	{
		
		APMNetWorkControl();
		int APMHostSearch();
	};
}
