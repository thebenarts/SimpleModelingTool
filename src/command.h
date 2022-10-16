#pragma once

// note we shouldn't do this as this can lead to circular dependency, if we want to be safe we should make a seperate include file to include every command, but for now it shoudl be fine
//#include "vec3Command.h"
#include "object.h"

class ICommand
{
public:
	virtual ~ICommand() {}
	virtual void Execute() = 0;
	virtual void Undo() = 0;
};