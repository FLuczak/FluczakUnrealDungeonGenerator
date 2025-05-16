

#pragma once

#include "BaseGenerationPass.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RoomGenerationPass.generated.h"

class ABaseRoom;
class URoomExitComponent;
class ULevelStreamingDynamic;
class ULevelData;
class URoomWithSpawnRules;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEVELGENERATION_API URoomGenerationPass : public UBaseGenerationPass
{
	GENERATED_BODY()
public:	
	URoomGenerationPass();
	void SpawnRooms(ULevelData* Data);
	void CleanUpTemporaryRoomInstances();
	virtual void Generate(ULevelData* Data) override;
private:
	void SpawnStartRoom(ULevelData*);
	void SpawnSpecialRooms(ULevelData* Data);
	bool DoesRoomOverlap(const TSubclassOf<ABaseRoom>& ObjectToSpawn, const FVector& Vector, const FRotator& Rotator) const;
	void HandleRoomInstanceCreation(TSubclassOf<ABaseRoom> RoomToSpawn);

	TObjectPtr<ABaseRoom> TrySpawnRoom(ULevelData* Data);
	TObjectPtr<ABaseRoom> SpawnRoom(ULevelData* Data, const FString RoomLevelPath, const FVector& Location, const FRotator& Rotation);

	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess= true))
	int NumberOfRoomsToSpawn;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))

	TObjectPtr<URoomWithSpawnRules>  StartRoomLevel=nullptr;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<URoomWithSpawnRules>>  SpecialRooms ={};

	int FailSafeTryLimit= 1000;
	int FailSafeCounter = 0;

	TMap<TSubclassOf<ABaseRoom>, TObjectPtr<ABaseRoom>>RoomInstances = {};
	TArray<TObjectPtr<URoomExitComponent>> TempExits = {  };
};

