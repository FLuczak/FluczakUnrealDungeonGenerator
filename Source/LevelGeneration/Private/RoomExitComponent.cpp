#include "RoomExitComponent.h"
#include "RoomWithSpawnRules.h"
URoomExitComponent::URoomExitComponent()
{
	SetCollisionProfileName(TEXT("GenerationData"));
	this->ComponentTags.Add("Door");
}

TObjectPtr<URoomWithSpawnRules> URoomExitComponent::GetRoomBasedOnRules(const ULevelData* Data, FRoomSpawnData& RoomData) const
{
	if (RoomsWithRules.Num() == 0)return nullptr;
	for (const auto RoomWithRules : RoomsWithRules)
	{
		if (RoomWithRules == nullptr)continue;
		if (RoomWithRules->Evaluate(Data,RoomData))return RoomWithRules;
	}

	return nullptr;
}

bool URoomExitComponent::HasSpawnedRoom()
{
	return (SpawnedRoom != nullptr);
}

