#include "command.h"
#include "resourcemanager.h"
#include <memory>


// basic functionality that has to be implemented by the Command classes


// virtual ~ICommand() {}				// in special cases where the command is responsible for keeping the object alive, it should also be responsible for deleting it
//virtual void Execute() = 0;			// perform the action,		responsibility delegated to the CONSTRUCTOR	add to the undoStack and clear the redoStack
//virtual void Undo() = 0;			// perform the undoAction, 		responsibility delegated to ResourceManager:: remove from the undoStack, add to the redoStack	
//virtual void Redo() = 0;			// perform the redoAction, 		responsibility delegated to ResourceManager:: remove from the redoStack, add to the undoStack

//------------------------------------------------------------------MOVE COMMAND--------------------------------------------------------------------------------
MoveCommand::MoveCommand(Object* target, glm::vec3& ogLocation, glm::vec3& newLocation) : targetObject(target), originalVector(glm::vec3(ogLocation)), desiredVector(glm::vec3(newLocation))
{
	Execute();
	ResourceManager::RegisterNewCommand(this);
}

void MoveCommand::Execute()
{
	if (targetObject)
	{
		std::cout << targetObject->objectName << " OG" << originalVector.x << originalVector.y << originalVector.z << " new" << desiredVector.x << desiredVector.y << desiredVector.z << std::endl;
		targetObject->SetObjectLocation(desiredVector);
	}
}

void MoveCommand::Undo()
{
	if (targetObject)
	{
		std::cout << targetObject->objectName << " OG" << originalVector.x << originalVector.y << originalVector.z << " new" << desiredVector.x << desiredVector.y << desiredVector.z << std::endl;
		targetObject->SetObjectLocation(originalVector);
	}
}

//------------------------------------------------------------------CREATE COMMAND--------------------------------------------------------------------------------
CreateCommand::CreateCommand(Object* inObject) : createdObject(inObject)
{
	if (!inObject)
		return;

	Execute();
	ResourceManager::RegisterNewCommand(this);
}

CreateCommand::~CreateCommand()
{
	if (!createdObject)
		return;

	// resourceManager check if the objects ID is nullptr if so delete the object
	if (!ResourceManager::GetObject(createdObject->objectID) && !createdObject->bRemoved)
	{
		delete createdObject;
	}
}

void CreateCommand::Execute()
{
	if (!createdObject)
		return;

	createdObject->AddID();
}

void CreateCommand::Undo()
{
	if (!createdObject)
		return;

	createdObject->RemoveID();
}

//------------------------------------------------------------------REMOVE COMMAND--------------------------------------------------------------------------------

RemoveCommand::RemoveCommand(Object* inObject) : objectToRemove(inObject)
{
	if (objectToRemove)
	{
		removeID = objectToRemove->objectID;
		Execute();
		ResourceManager::RegisterNewCommand(this);
	}
}

RemoveCommand::RemoveCommand(int objectID) : removeID(objectID)
{
	objectToRemove = ResourceManager::GetObject(removeID);
	if (objectToRemove)
	{
		Execute();
		ResourceManager::RegisterNewCommand(this);
	}
}

RemoveCommand::RemoveCommand()
{
	objectToRemove = ResourceManager::GetSelectedObject();

	if (objectToRemove)
	{
		removeID = objectToRemove->objectID;
		Execute();
		ResourceManager::RegisterNewCommand(this);
	}
}

void RemoveCommand::Execute()
{
	if (!objectToRemove)
		return;
	objectToRemove->bRemoved = true;
	objectToRemove->RemoveID();
}

void RemoveCommand::Undo()
{
	if (!objectToRemove)
		return;

	objectToRemove->AddID();
}

RemoveCommand::~RemoveCommand()
{
	if (!objectToRemove)
		return;

	if (Object* other = ResourceManager::GetObject(objectToRemove->objectID))
	{
			return;
	}

	objectToRemove->FreeID();
	delete objectToRemove;
}