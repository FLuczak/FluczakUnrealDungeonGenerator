

#pragma once

#include "CoreMinimal.h"
#include "LevelData.h"
#include "Components/ActorComponent.h"
#include "WallTileWithSpawnRules.generated.h"


class UWallSpawnRules;

USTRUCT(BlueprintType, Blueprintable)
struct LEVELGENERATION_API FWallTileSpawnData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector SpawnPosition{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector WallNormal{};
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WallPercentage =0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsCorner = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AngleToNextWall = 0.0f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEVELGENERATION_API UWallTileWithSpawnRules : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	bool Evaluate(const ULevelData* levelData, FWallTileSpawnData& spawnData);
	TObjectPtr<UStaticMesh> GetWall() { return MeshToSpawn; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMesh> MeshToSpawn{};
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UWallSpawnRules>> Rules{};
};
