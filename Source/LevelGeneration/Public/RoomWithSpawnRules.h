

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RoomWithSpawnRules.generated.h"


class ULevelData;
class URoomSpawnRules;
class ABaseRoom;
struct FRoomSpawnData;

USTRUCT()
struct FRoomData
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere)
	FString LevelName = "";
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<ABaseRoom> RoomToSpawn{};
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UWorld> LevelToSpawn = nullptr;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEVELGENERATION_API URoomWithSpawnRules : public UPrimaryDataAsset
{
public:
	GENERATED_BODY()
	bool Evaluate(const ULevelData* levelData, FRoomSpawnData& Data);
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<FRoomData> RoomsToSpawn{};
#if WITH_EDITOR
	UFUNCTION(CallInEditor)
	void RefreshLevels();
	/**
	 * @brief SHOULD ONLY RUN IN THE EDITOR. Generate and store the vertex data for the room instance to off-load the work from runtime to in-editor.
	 * @param OuterInstanceGraph - an argument required for PostLoadSubObjects- not required for the function to run
	 */
	virtual void PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph) override;
#endif
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<URoomSpawnRules>> Rules{};
};
