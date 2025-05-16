
#include "RoomWithSpawnRules.h"
#include "RoomSpawnRules.h"

bool URoomWithSpawnRules::Evaluate(const ULevelData* levelData,FRoomSpawnData& Data)
{
	if (Rules.Num() == 0)return true;
	for (const auto rule : Rules)
	{
		if (rule == nullptr)continue;
		if (!rule->CheckRules(levelData,Data))return false;
	}
	return true;
}

#if WITH_EDITOR

void URoomWithSpawnRules::RefreshLevels()
{
	for (auto& RoomData : RoomsToSpawn)
	{
		RoomData.LevelName = RoomData.LevelToSpawn.ToSoftObjectPath().ToString();
	}
}

void URoomWithSpawnRules::PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph)
{
	RefreshLevels();
}
#endif
