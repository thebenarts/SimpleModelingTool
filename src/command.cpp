#include "command.h"
#include "resourcemanager.h"


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
//------------------------------------------------------------------ROTATE COMMAND--------------------------------------------------------------------------------



RemoveCommand::~RemoveCommand()
{
	delete objectToRemove;
}