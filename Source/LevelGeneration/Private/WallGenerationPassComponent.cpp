// Fill out your copyright notice in the Description page of Project Settings.


#include "WallGenerationPassComponent.h"
#include "LevelData.h"
#include "Curve/PolygonIntersectionUtils.h"
#include "BaseRoom.h"
#include "RoomFloorComponent.h"
#include "WallTile.h"
#include "Kismet/KismetSystemLibrary.h"


void UWallGenerationPassComponent::Generate(ULevelData* Data)
{
	Super::Generate(Data);
	TMap<TObjectPtr<ABaseRoom>, TArray<FVector>> Geometry = InitializeRoomPolygons(*Data);
	
	UE_LOG(LogCore, Warning, TEXT("Started generating polygons"));

	for(auto Pair : Geometry)
	{
		CreateWallComponents(Data, Pair.Key, Pair.Value);
	}

	RemoveOverlappingWallComponents(Data);
}


UE::Geometry::FGeneralPolygon2d UWallGenerationPassComponent::GetPolygonShapeForRoom(TObjectPtr<ABaseRoom> Room)
{
	auto Floors = Room->GetFloors();
	TArray<UE::Geometry::FGeneralPolygon2d> RoomPolygon = {};
	UE_LOG(LogCore, Warning, TEXT("Room floor count: %d"), Floors.Num());

	for (const auto Floor : Floors)
	{
		RoomPolygon.Add(Floor->GetMeshData());
	}

	UE_LOG(LogCore, Warning, TEXT("Room polygon size: %d"),RoomPolygon.Num());
	TArray<UE::Geometry::FGeneralPolygon2d> Result;
	UE::Geometry::PolygonsUnion(RoomPolygon, Result, true);
	if(Result.Num() == 0)
	{
		return {};
	}
	return Result[0];
}

TMap<TObjectPtr<ABaseRoom>, TArray<FVector>> UWallGenerationPassComponent::InitializeRoomPolygons(ULevelData& Data)
{
	TMap<TObjectPtr<ABaseRoom>, TArray<FVector>> Geometry;

	for (auto Room : Data.Rooms)
	{
		auto RoomPolygon = GetPolygonShapeForRoom(Room);
		if (RoomPolygon.GetOuter().VertexCount() == 0.0f)continue;
		for (const auto Point : RoomPolygon.GetOuter().GetVertices())
		{
			if(!Geometry.Contains(Room))
			{
				Geometry.Add(Room);
			}

			Geometry[Room].Add(FVector(Point.X, Point.Y, Room->GetActorLocation().Z));
		}
	}

	return Geometry;
}

TObjectPtr<UWallTileComponent> UWallGenerationPassComponent::SpawnWallTile(
	const TObjectPtr<ABaseRoom>& Room, const FString& ObjectName, UStaticMesh* Mesh, const FVector& SpawnLocation,
	const FRotator& Rotator, float PercentageAlongWall) const
{
	if (Mesh == nullptr)return nullptr;
	UWallTileComponent* tile = NewObject<UWallTileComponent>(Room, UWallTileComponent::StaticClass(), FName(*ObjectName));

	tile->PercentageOnAWall = PercentageAlongWall;
	tile->IsCorner = PercentageAlongWall >= 0.99f || PercentageAlongWall <= 0.001f;
	
	if (!tile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create tile!"));
		return nullptr;
	}

	tile->ComponentTags.Add(FName("Wall"));
	tile->SetStaticMesh(Mesh);
	tile->SetWorldLocation(SpawnLocation);
	tile->SetWorldRotation(Rotator);

	tile->AttachToComponent(Room->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	tile->RegisterComponent();
	Room->AddInstanceComponent(tile);
	tile->SetGenerateOverlapEvents(true);

	tile->SetCollisionObjectType(ECC_WorldStatic);
	tile->SetCollisionResponseToAllChannels(ECR_Block);
	tile->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	return tile;
}

TArray<FHitResult> UWallGenerationPassComponent::GetOverlappingComponentsForWall(const TObjectPtr<UStaticMeshComponent> Wall ) const
{
	TArray<FHitResult> ToReturn = {};
	const auto Start = Wall->Bounds.GetBox().GetCenter();
	const FVector End = Wall->Bounds.GetBox().GetCenter();
	const auto Box = Wall->GetStaticMesh().Get()->GetBounds().GetBox().GetExtent();
	const FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(Box.X * 0.05f, Box.Y * 0.05f, Box.Z));

	UKismetSystemLibrary::BoxTraceMulti(
		GetWorld(),
		Start,
		End,
		BoxShape.GetBox(),
		Wall->GetComponentRotation(),
		TraceTypeQuery3,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		ToReturn,
		false
	);

	return ToReturn;
}

void UWallGenerationPassComponent::RemoveOverlappingWallComponents(ULevelData* LevelData) const
{
	TArray<UActorComponent*> Destroyed;
	TArray<UActorComponent*> Kept;

	for (const auto Room : LevelData->Rooms)
	{
		for (const auto Wall : Room->WallInstances)
		{
			TArray<FHitResult> HitResults = GetOverlappingComponentsForWall(Wall);

			for (const FHitResult& Hit : HitResults)
			{
				const auto Component = Hit.GetComponent();
				if (FMath::Abs(Component->GetComponentLocation().Z - Wall->GetComponentLocation().Z) > MaxVerticalTileDistance)
				{
					continue;
				}

				if (Component->ComponentTags.Contains("Door"))
				{
					if(Cast<URoomExitComponent>(Component)->SpawnedRoom != nullptr)
					{
						Destroyed.Add(Wall);
					}
				}

				if (!Component->ComponentTags.Contains("Wall")) { continue; }

				auto CastedToWall = Cast<UWallTileComponent>(Component);
				if (CastedToWall->IsCorner && Wall->IsCorner)continue;
				if (Component->GetOwner() == Wall->GetOwner()) { continue; }
				if (Destroyed.Contains(Component)) { continue; }
				if (Destroyed.Contains(Wall)) { continue; }
				if (Kept.Contains(Component)) { continue; }
				if (Kept.Contains(Wall)) { continue; }

				Kept.Add(Component);
				Destroyed.Add(Wall);
				break;
			}
		}
	}

	for (const auto ToDestroy : Destroyed)
	{
		if (ToDestroy == nullptr)continue;
		ToDestroy->UnregisterComponent();
		ToDestroy->DestroyComponent();
	}
}

void UWallGenerationPassComponent::CreateWallComponents(const ULevelData* Data, const TObjectPtr<ABaseRoom>& Room, TArray<FVector> Polygon) const
{
	TArray<TObjectPtr<UWallTileWithSpawnRules>> RoomAvailableWalls = Room->GetWallsToGenerate();
	if (RoomAvailableWalls.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WallTile is not set!"));
		return;
	}

	int Total = 0;
	TObjectPtr<UStaticMesh> WallTileMesh = RoomAvailableWalls[RoomAvailableWalls.Num() - 1]->GetWall();
	const FBox Bounds = WallTileMesh->GetBoundingBox();
	const float TileExtent = Bounds.GetExtent().X * 2;

	for (int32 i = 0; i < Polygon.Num(); i++)
	{
		FVector ToNextPoint = Polygon[(i + 1) % Polygon.Num()] - Polygon[i];
		const int32 NumTilesToSpawn = FMath::RoundToInt(ToNextPoint.Size() / TileExtent);
		FVector Direction = ToNextPoint.GetSafeNormal();
		FVector StartPosition = Polygon[i];
		const FRotator TileRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

		for (int32 j = 0; j < NumTilesToSpawn; j++)
		{
			FVector TilePosition = StartPosition + Direction * TileExtent * j;

			float PercentageAlongWall = 0;
			PercentageAlongWall = NumTilesToSpawn != 1 ? static_cast<float>(j) / static_cast<float>(NumTilesToSpawn - 1) : 1.0f;
			FVector Normal{};

			WallTileMesh = nullptr;
			for (const auto TileWithRules : RoomAvailableWalls)
			{
				Normal = ((TilePosition)-Room->GetActorLocation()).GetSafeNormal();
				FWallTileSpawnData WallTileSpawnData{ TilePosition, Normal,PercentageAlongWall,PercentageAlongWall == 0.0f ||PercentageAlongWall >= 0.99f };
				if (TileWithRules == nullptr)continue;
				bool bResult = TileWithRules->Evaluate(Data, WallTileSpawnData);
				if (!bResult)continue;
				WallTileMesh = TileWithRules->GetWall();
				break;
			}

			if (WallTileMesh == nullptr)continue;
			for (int h = 0; h < Room->RoomTileHeight; h++)
			{
				FString ObjectName = FString::Printf(TEXT("Wall_%d"), Total);
				TilePosition.Z = StartPosition.Z + (FVector::UpVector * h * Bounds.GetExtent().Z * 2).Z;
				auto Tile = SpawnWallTile(Room, ObjectName, WallTileMesh, TilePosition, TileRotation,PercentageAlongWall);

				if (Tile == nullptr)continue;

				Tile->Normal = Normal;
				Room->WallInstances.Add(Tile);
				Total++;
			}
		}
	}
}

