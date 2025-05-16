

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallSpawnRules.generated.h"

UCLASS()
class LEVELGENERATION_API UWallSpawnRules : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CheckRules(const ULevelData* LevelData,FWallTileSpawnData spawnData);
};
