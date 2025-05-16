


#include "WallTileWithSpawnRules.h"
#include "LevelData.h"
#include "WallSpawnRules.h"

bool UWallTileWithSpawnRules::Evaluate(const ULevelData* levelData,FWallTileSpawnData& spawnData)
{
	if (Rules.Num() == 0)return true;
	for (const auto rule : Rules)
	{
		if (rule == nullptr)continue;
		const bool bEvaluationResult = rule->CheckRules(levelData,spawnData);
		if (!bEvaluationResult)return false;
	}
	return true;
}
