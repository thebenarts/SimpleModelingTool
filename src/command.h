#pragma once

// note we shouldn't do this as this can lead to circular dependency, if we want to be safe we should make a seperate include file to include every command, but for now it shoudl be fine
//#include "vec3Command.h"
#include "object.h"
#include "glm/glm.hpp"
#include <iostream>

class ICommand
{
public:
	virtual ~ICommand() {}				// in special cases where the command is responsible for keeping the object alive, it should also be responsible for deleting it
	virtual void Execute() = 0;			// perform the action, add to the undoStack and clear the redoStack
	virtual void Undo() = 0;			// perform the undoAction, remove from the undoStack, add to the redoStack
};

class Vec3Command : public ICommand
{
public:
	Vec3Command(glm::vec3& originalVector, glm::vec3& newVector) :
		targetVector(originalVector),
		desiredVector(newVector),
		originalVector(glm::vec3(originalVector))
	{}

	virtual void Execute() override
	{
		targetVector = desiredVector;
	}

	virtual void Undo() override
	{
		targetVector = originalVector;
	}


private:
	glm::vec3& targetVector;
	glm::vec3 desiredVector;
	glm::vec3 originalVector;
};

class MoveCommand : public ICommand
{
public:
	MoveCommand(Object* target, glm::vec3& newVector) :
		targetObject(target),
		desiredVector(newVector)
	{
		if (targetObject)
			originalVector = targetObject->GetObjectLocation();
	}

	MoveCommand(Object* target, glm::vec3& ogLocation, glm::vec3& newLocation);

	virtual void Execute() override;

	virtual void Undo() override;

private:
	Object* targetObject;
	glm::vec3 desiredVector;
	glm::vec3 originalVector;
};

class ScaleCommand : public ICommand
{
public:
	ScaleCommand(Object* target, glm::vec3& newVector) :
		targetObject(target),
		desiredVector(newVector)
	{
		originalVector = targetObject->GetObjectScale();
	}

	virtual void Execute() override
	{
		if (targetObject)
		{
			targetObject->SetObjectScale(desiredVector);
		}
	}

	virtual void Undo() override
	{
		if (targetObject)
		{
			targetObject->SetObjectScale(originalVector);
		}
	}


private:
	Object* targetObject;
	glm::vec3 desiredVector;
	glm::vec3 originalVector;
};

class RotateCommand : public ICommand
{
public:
	RotateCommand(Object* target, glm::vec3& newVector) :
		targetObject(target),
		desiredVector(newVector)
	{
		originalVector = targetObject->GetObjectRotation();
	}

	virtual void Execute() override
	{
		if (targetObject)
		{
			targetObject->SetObjectRotation(desiredVector);
		}
	}

	virtual void Undo() override
	{
		if (targetObject)
		{
			targetObject->SetObjectRotation(originalVector);
		}
	}


private:
	Object* targetObject;
	glm::vec3 desiredVector;
	glm::vec3 originalVector;
};

class RemoveCommand : public ICommand
{
	RemoveCommand(Object* inObject) :
		objectToRemove(inObject) {}

	virtual ~RemoveCommand();

private:
	Object* objectToRemove;
};