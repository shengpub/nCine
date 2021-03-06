#ifndef CLASS_NCINE_LUAIAPPEVENTHANDLER
#define CLASS_NCINE_LUAIAPPEVENTHANDLER

#include "common_defines.h"

struct lua_State;

namespace ncine {

class AppConfiguration;

/// Wrapper around the `IAppEventHandler` class
class DLL_PUBLIC LuaIAppEventHandler
{
  public:
	static void onPreInit(lua_State *L, AppConfiguration &config);
	static void onInit(lua_State *L);
	static void onFrameStart(lua_State *L);
	static void onShutdown(lua_State *L);
};

}

#endif
