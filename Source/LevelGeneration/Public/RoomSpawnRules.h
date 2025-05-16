

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelData.h"
#include "RoomSpawnRules.generated.h"

class URoomExitComponent;

USTRUCT(BlueprintType)
struct FRoomSpawnData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	URoomExitComponent* SpawningDoor = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector PositionToSpawnRoom = FVector(0,0,0);
};

UCLASS( ClassGroup=(Custom) )
class LEVELGENERATION_API URoomSpawnRules : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CheckRules(const ULevelData* LevelData, const FRoomSpawnData& Data);
};
