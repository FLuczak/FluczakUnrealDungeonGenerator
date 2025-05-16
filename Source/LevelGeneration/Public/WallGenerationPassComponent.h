// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGenerationPass.h"
#include "WallTile.h"
#include "Curve/GeneralPolygon2.h"
#include "WallGenerationPassComponent.generated.h"

class ABaseRoom;


/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LEVELGENERATION_API UWallGenerationPassComponent : public UBaseGenerationPass
{
public:
	GENERATED_BODY()
	virtual void Generate(ULevelData* Data) override;
	static UE::Geometry::FGeneralPolygon2d GetPolygonShapeForRoom(TObjectPtr<ABaseRoom> Room);

private:
	static TMap<TObjectPtr<ABaseRoom>, TArray<FVector>> InitializeRoomPolygons(ULevelData& Data);
	TObjectPtr<UWallTileComponent> SpawnWallTile(const TObjectPtr<ABaseRoom>& Room, const FString& ObjectName,
	                                             UStaticMesh* Mesh, const FVector& SpawnLocation,
	                                             const FRotator& Rotator, float PercentageAlongWall) const;

	void RemoveOverlappingWallComponents(ULevelData* LevelData) const;
	void CreateWallComponents(const ULevelData* Data, const TObjectPtr<ABaseRoom>& Room, TArray<UE::Math::TVector<double>> Polygon) const;
	TArray<FHitResult> GetOverlappingComponentsForWall(const TObjectPtr<UStaticMeshComponent> Wall)const;
	//When checking for overlaps sometimes there might be a situation where a wall overlaps with a wall above it, this determines what vertical distance can 2 walls have to be considered overlapping
	int MaxVerticalTileDistance = 30;
};
