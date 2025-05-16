
#include "BaseRoom.h"

#include "RoomBoundsComponent.h"
#include "RoomExitComponent.h"
#include "WallTile.h"
#include "Curve/PolygonIntersectionUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetSystemLibrary.h"

ABaseRoom::ABaseRoom()
{
}

const TArray<TObjectPtr<URoomBoundsComponent>>& ABaseRoom::GetRoomBounds()
{
	if (RoomBounds.Num() == 0)
	{
		GetComponents<URoomBoundsComponent>(RoomBounds);
	}

	 return RoomBounds; 
}

void ABaseRoom::PlayerLeftRoom_Implementation()
{
	PlayerLeftRoomEvent.Broadcast();
}

void ABaseRoom::InitializeComponents()
{
	GetComponents<URoomBoundsComponent>(RoomBounds);
	GetComponents<URoomFloorComponent>(Floors);
	Exits = GetExits();
	Entrance = GetComponentByClass<URoomEntranceComponent>();
}

void ABaseRoom::AddEnemy(AActor* Enemy)
{
	EnemiesAlive.Add(Enemy);
}

#if not WITH_EDITOR
void ABaseRoom::SpawnWalls()
{
	
}
#endif

#if WITH_EDITOR

void ABaseRoom::SpawnWalls()
{
	if(GetWorld()->IsGameWorld())
	{
		return;
	}

	for (auto Wall : WallInstancesActors)
	{
		GEditor->GetEditorWorldContext().World()->DestroyActor(Wall);
	}
	WallInstancesActors.Empty();

	const auto Geometry = InitializeRoomPolygons();
	UE_LOG(LogCore, Warning, TEXT("Started generating polygons"));
	CreateWallsBasedOnPolygon(Geometry);
	RemoveOverlappingWallComponents();
}

TArray<FVector> ABaseRoom::InitializeRoomPolygons() 
{
	TArray<FVector>  Geometry{};

	const auto RoomPolygon = GetPolygonShapeForRoom();
	if (RoomPolygon.GetOuter().VertexCount() == 0.0f)return Geometry;
	for (const auto Point : RoomPolygon.GetOuter().GetVertices())
	{
		Geometry.Add(FVector(Point.X, Point.Y,GetActorLocation().Z));
	}

	return Geometry;
}

UE::Geometry::FGeneralPolygon2d ABaseRoom::GetPolygonShapeForRoom() 
{
	auto TempFloors = K2_GetComponentsByClass(URoomFloorComponent::StaticClass());
	TArray<UE::Geometry::FGeneralPolygon2d> RoomPolygon = {};
	UE_LOG(LogCore, Warning, TEXT("Room floor count: %d"), TempFloors.Num());

	for (const auto Floor : TempFloors)
	{
		RoomPolygon.Add(Cast<URoomFloorComponent>(Floor)->GetMeshData());
	}

	UE_LOG(LogCore, Warning, TEXT("Room polygon size: %d"), RoomPolygon.Num());
	TArray<UE::Geometry::FGeneralPolygon2d> Result;
	UE::Geometry::PolygonsUnion(RoomPolygon, Result, true);
	if (Result.Num() == 0)
	{
		return {};
	}
	return Result[0];
}

TObjectPtr<AStaticMeshActor> ABaseRoom::SpawnWallTile( const FString& ObjectName,
	UStaticMesh* Mesh, const FVector& SpawnLocation, const FRotator& Rotator, float PercentageAlongWall) 
{
	if (Mesh == nullptr)return nullptr;
	AStaticMeshActor* Wall = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnLocation, Rotator);

	if (!Wall)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create tile!"));
		return nullptr;
	}

	Wall ->Tags.Add(FName("Wall"));
	Wall->GetStaticMeshComponent()->SetStaticMesh(Mesh);
	Wall->GetStaticMeshComponent()->SetGenerateOverlapEvents(true);

	Wall->GetStaticMeshComponent()->SetCollisionObjectType(ECC_WorldStatic);
	Wall->GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	Wall->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WallInstancesActors.Add(Wall);
	return Wall;
}

TArray<FHitResult> ABaseRoom::GetOverlappingComponentsForWall(const TObjectPtr<UStaticMeshComponent> Wall) 
{
	TArray<FHitResult> ToReturn = {};
	/*if(Wall == nullptr)
	{
		return{};
	}

	if(Wall->GetStaticMesh() == nullptr)
	{
		return {};
	}*/

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
		EDrawDebugTrace::ForDuration,
		ToReturn,
		false
	);

	return ToReturn;
}

void ABaseRoom::RemoveOverlappingWallComponents() 
{
	TArray<AActor*> Destroyed{};
	TArray<AActor*> Kept{};

	for (const auto Wall : WallInstancesActors)
	{
		TArray<FHitResult> HitResults = GetOverlappingComponentsForWall(Wall->GetStaticMeshComponent());

		for (const FHitResult& Hit : HitResults)
		{
			const auto Component = Hit.GetComponent();
			if (!Component->ComponentTags.Contains("Door")) { continue; }
			if (Destroyed.Contains(Wall)) { continue; }
			Destroyed.Add(Wall);
		}
	}

	for (const auto ToDestroy : Destroyed)
	{
		if (ToDestroy == nullptr)continue;
		GEditor->GetEditorWorldContext().World()->DestroyActor(ToDestroy);
	}
}

void ABaseRoom::CreateWallsBasedOnPolygon(TArray<FVector> Polygon) 
{	
	if(WallToSpawn == nullptr)
	{
		return;
	}

	int Total = 0;
	const FBox Bounds = WallToSpawn->GetBoundingBox();
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

			for (int h = 0; h < RoomTileHeight; h++)
			{
				FString ObjectName = FString::Printf(TEXT("Wall_%d"), Total);
				TilePosition.Z = StartPosition.Z + (FVector::UpVector * h * Bounds.GetExtent().Z * 2).Z;
				auto Tile = SpawnWallTile( ObjectName, WallToSpawn, TilePosition, TileRotation, PercentageAlongWall);
				if (Tile == nullptr)continue;
				Total++;
			}
		}
	}
}

#endif

void ABaseRoom::RegisterEnemyDead(AActor* Enemy)
{
	EnemiesAlive.Remove(Enemy);
	if(EnemiesAlive.Num() == 0)
	{
		OnRoomCombatEnded.Broadcast();
		IsRoomCompleted = true;
	}
}

void ABaseRoom::BeginPlay()
{
	Super::BeginPlay();
}

TArray<TObjectPtr<URoomExitComponent>> ABaseRoom::GetExits() const
{
	TArray<TObjectPtr<URoomExitComponent>> ChildrenToReturn{};
	TArray<UActorComponent*> TempChildren = K2_GetComponentsByClass(URoomExitComponent::StaticClass());

	for (UActorComponent* SceneComponentChild : TempChildren)
	{
		auto Exit = Cast<URoomExitComponent>(SceneComponentChild);
		ChildrenToReturn.Add(Exit);
	}

	return ChildrenToReturn;
}

void ABaseRoom::PlayerEnteredRoom_Implementation()
{
	PlayerEnteredRoomEvent.Broadcast();
	if(EnemiesAlive.Num() > 0)
	{
		IsRoomCompleted = false;
		OnRoomCombatStarted.Broadcast();
	}
	else
	{
		IsRoomCompleted = true;
	}
}
