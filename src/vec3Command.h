#pragma once

#include "command.h"
#include "glm/glm.hpp"
#include <iostream>


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

	MoveCommand(Object* target, glm::vec3& ogLocation, glm::vec3& newLocation) : targetObject(target), originalVector(glm::vec3(ogLocation)) , desiredVector(glm::vec3(newLocation))
	{
		Execute();
	}

	virtual void Execute() override
	{
		if (targetObject)
		{
			std::cout << targetObject->objectName << " OG" << originalVector.x << originalVector.y << originalVector.z << " new" << desiredVector.x << desiredVector.y << desiredVector.z << std::endl;
			targetObject->SetObjectLocation(desiredVector);
		}
	}

	virtual void Undo() override
	{
		if (targetObject)
		{
			std::cout << targetObject->objectName << " OG" << originalVector.x << originalVector.y << originalVector.z << " new" << desiredVector.x << desiredVector.y << desiredVector.z << std::endl;
			targetObject->SetObjectLocation(originalVector);
		}
	}


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