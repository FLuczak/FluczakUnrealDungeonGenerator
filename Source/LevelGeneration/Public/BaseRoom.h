

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "RoomBoundsComponent.h"
#include "RoomExitComponent.h"
#include "RoomFloorComponent.h"
#include "WallTileWithSpawnRules.h"

#include "EntranceComponent.h"

#include "BaseRoom.generated.h"

class UWallTileComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerEnteredRoom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerLeftRoom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerEnteredNeighboringRoom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerLeftNeighboringRoom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartRoomCombat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndRoomCombat);

UCLASS()
class LEVELGENERATION_API ABaseRoom : public AActor
{
	GENERATED_BODY()
public:
	ABaseRoom();
	const TArray<TObjectPtr<URoomBoundsComponent>>& GetRoomBounds();
	TObjectPtr <URoomEntranceComponent> GetEntrance() const { return Entrance; }

	TArray<TObjectPtr<UWallTileComponent>> WallInstances{};

	TArray<TObjectPtr<URoomExitComponent>> GetExits() const;
	TArray<TObjectPtr<URoomFloorComponent>> GetFloors() const { return Floors; }
	TArray<TObjectPtr<UWallTileWithSpawnRules>> GetWallsToGenerate()const{ return WallsWithRules; }

	UPROPERTY(BlueprintAssignable, Category = "Gameplay")
	FPlayerEnteredRoom PlayerEnteredRoomEvent;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay")
	FPlayerEnteredRoom PlayerLeftRoomEvent;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay")
	FStartRoomCombat OnRoomCombatStarted;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay")
	FEndRoomCombat OnRoomCombatEnded;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay")
	FPlayerEnteredNeighboringRoom PlayerEnteredNeighboringRoom{};
	UPROPERTY(BlueprintAssignable, Category = "Gameplay")
	FPlayerLeftNeighboringRoom PlayerLeftNeighboringRoom{};

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite, Category = "Gameplay")
	AActor* Teleporter = nullptr;

	UFUNCTION(BlueprintNativeEvent)
	void PlayerEnteredRoom();

	UFUNCTION(BlueprintNativeEvent)
	void PlayerLeftRoom();
	void InitializeComponents();
	UFUNCTION(BlueprintCallable)
	void AddEnemy(AActor* Enemy);
	UPROPERTY(EditAnywhere, Category = "Tools | WallSpawning")
	TObjectPtr<UStaticMesh> WallToSpawn = nullptr;
	UFUNCTION(CallInEditor)
	void SpawnWalls();

#if WITH_EDITOR
	TArray<TObjectPtr<AStaticMeshActor>> WallInstancesActors{};
	TArray<FVector> InitializeRoomPolygons() ;
	UE::Geometry::FGeneralPolygon2d GetPolygonShapeForRoom() ;
	TObjectPtr<AStaticMeshActor> SpawnWallTile(
		const FString& ObjectName, UStaticMesh* Mesh, const FVector& SpawnLocation,
		const FRotator& Rotator, float PercentageAlongWall) ;
	TArray<FHitResult> GetOverlappingComponentsForWall(const TObjectPtr<UStaticMeshComponent> Wall) ;
	void RemoveOverlappingWallComponents() ;
	void CreateWallsBasedOnPolygon(TArray<FVector> Polygon) ;
#endif

	UFUNCTION(BlueprintCallable)
	void RegisterEnemyDead(AActor* Enemy);

	int RoomTileHeight = 1;
	bool IsStartRoom = false;
	bool IsRoomCompleted = true;
	bool IsDeadEnd = true;
private:
	virtual void BeginPlay() override;
	TArray<TObjectPtr<URoomFloorComponent>> Floors{};
	TArray<TObjectPtr<URoomBoundsComponent>> RoomBounds{};
	TSet<TObjectPtr<AActor>> EnemiesAlive{};

	UPROPERTY(EditDefaultsOnly,meta=(AllowPrivateAccess=true))
	TArray<TObjectPtr<UWallTileWithSpawnRules>> WallsWithRules{};
	TObjectPtr<URoomEntranceComponent> Entrance = nullptr;
	TArray<TObjectPtr <URoomExitComponent>> Exits {};
	
	
};
