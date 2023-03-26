#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include <sol/sol.hpp>

struct ScriptComponent {
	sol::function func;

	ScriptComponent(sol::function func = sol::lua_nil) {
		this->func = func;
	}
};

#endif // SCRIPT_COMPONENT_H
