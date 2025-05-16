


#include "RoomGenerationPass.h"

#include "BaseRoom.h"
#include "EntranceComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelData.h"
#include "RoomWithSpawnRules.h"
#include "Engine/LevelStreamingDynamic.h"


// Sets default values for this component's properties
URoomGenerationPass::URoomGenerationPass()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URoomGenerationPass::SpawnRooms(ULevelData* Data)
{
	if (Data->Rooms.Num() >= NumberOfRoomsToSpawn)
	{
		return;
	}

	if (Data->Rooms.Num() == 0)
	{
		return;
	}

	for (int i = 0; i < NumberOfRoomsToSpawn - 1; i++)
	{
		FailSafeCounter++;
		if (FailSafeCounter >= FailSafeTryLimit)
		{
			return;
		}

		if (Data->Exits.Num() == 0)
		{
			return;
		}

		TObjectPtr<ABaseRoom> SpawnedRoom = TrySpawnRoom(Data);
		if (SpawnedRoom == nullptr)
		{
			i--;
		}
	}
}

void URoomGenerationPass::Generate(ULevelData* Data)
{
	TempExits.Empty();
	RoomInstances.Empty();
	FailSafeCounter = 0;
	FailSafeTryLimit = NumberOfRoomsToSpawn * 25;

	SpawnStartRoom(Data);
	SpawnRooms(Data);
	SpawnSpecialRooms(Data);
	CleanUpTemporaryRoomInstances();
}

void URoomGenerationPass::SpawnStartRoom(ULevelData* Data)
{
	const FVector Location(0.0f, 0.0f, 0.0f);
	const FRotator Rotation(0.0f, 0.0f, 0.0f);
	const auto& StartRoom = StartRoomLevel->RoomsToSpawn[0];
	const auto SpawnedRoom = SpawnRoom(Data, StartRoom.LevelName, Location, Rotation);
	SpawnedRoom->IsStartRoom = true;
	SpawnedRoom->Tags.Add("Start");
	Data->StartRoom = SpawnedRoom;
}

void URoomGenerationPass::SpawnSpecialRooms(ULevelData* Data)
{
	for (auto RoomToSpawn : SpecialRooms)
	{
		if (RoomToSpawn == nullptr)
		{
			continue;
		}

		const FRoomData& RoomData = RoomToSpawn->RoomsToSpawn[0];

		if(RoomData.RoomToSpawn == nullptr)
		{
			continue;
		}

		auto ExitsCopy = TempExits;
		ExitsCopy.Sort([&](const TObjectPtr<URoomExitComponent> A, const TObjectPtr<URoomExitComponent> B)
			{
				const float DistA = FVector::DistSquared(A->GetComponentLocation(), Data->StartRoom->GetActorLocation());
				const float DistB = FVector::DistSquared(B->GetComponentLocation(), Data->StartRoom->GetActorLocation());
				return DistA > DistB; // Sort in descending order (furthest first)
			});

		HandleRoomInstanceCreation(RoomData.RoomToSpawn);

		for (const auto Exit : ExitsCopy)
		{
			FVector SpawnLocation = Exit->GetComponentLocation();
			FRotator SpawnRotation = Exit->GetComponentRotation();
			if (DoesRoomOverlap(RoomData.RoomToSpawn, SpawnLocation, SpawnRotation))
			{
				continue;
			}

			const auto SpawnedRoom = SpawnRoom(Data, RoomData.LevelName, SpawnLocation, SpawnRotation);
			Exit->SpawnedRoom = SpawnedRoom;
			TempExits.Remove(Exit);
			break;
		}
	}
}

bool URoomGenerationPass::DoesRoomOverlap(const TSubclassOf<ABaseRoom>& ObjectToSpawn, const FVector& Vector, const FRotator& Rotator) const
{
	TArray<TObjectPtr<URoomBoundsComponent>> RoomBounds = RoomInstances[ObjectToSpawn]->GetRoomBounds();

	for (const auto Bound : RoomBounds)
	{
		TArray<FHitResult> HitResults;

		FVector RelativeScale3D = Bound->GetComponentScale();
		FQuat RelativeRotation = Bound->GetComponentRotation().Quaternion();

		FTransform BoundTransform;
		BoundTransform.SetScale3D(RelativeScale3D);
		BoundTransform.SetRotation(RelativeRotation);

		FQuat Quat = Rotator.Quaternion();
		FTransform RoomTransform;
		RoomTransform.SetTranslation(Vector + Quat.RotateVector(Bound->GetRelativeLocation() * RelativeScale3D / 2));
		RoomTransform.SetRotation(Rotator.Quaternion());
		RoomTransform.SetScale3D(RelativeScale3D);

		FTransform FinalTransform = BoundTransform * RoomTransform;

		FVector BoundCenter = FinalTransform.GetLocation();
		FQuat BoundRotation = FinalTransform.GetRotation();

		UKismetSystemLibrary::BoxTraceMulti(
			GetWorld(),
			BoundCenter,
			BoundCenter + FVector::UpVector,
			Bound->GetScaledBoxExtent(),
			BoundRotation.Rotator(),
			TraceTypeQuery3,
			false,
			{},
			EDrawDebugTrace::None,
			HitResults,
			true
		);

		// Check if any of the hit results belong to a room
		for (const auto HitResult : HitResults)
		{
			if (HitResult.Component->ComponentTags.Contains("Room"))
			{
				return true;
			}
		}
	}

	return false;
}

TObjectPtr<ABaseRoom> URoomGenerationPass::TrySpawnRoom(ULevelData* Data)
{
	if (Data->Exits.Num() == 0)return nullptr;

	for(const auto exit : TempExits)
	{
		const auto SpawnLocation = exit->GetComponentLocation();
		const auto SpawnRotation = exit->GetComponentRotation();

		FRoomSpawnData RoomSpawnData{ exit,SpawnLocation };

		const auto RoomExitComponent = Cast<URoomExitComponent>(exit);
		const auto RoomToSpawn = RoomExitComponent->GetRoomBasedOnRules(Data, RoomSpawnData);

		if (RoomToSpawn == nullptr)break;
		std::uniform_int_distribution Distribution(0, RoomToSpawn->RoomsToSpawn.Num()-1);
		int Index = Distribution(RandomNumberGenerator);

		auto& RoomData = RoomToSpawn->RoomsToSpawn[Index];
		if (RoomToSpawn == nullptr)return nullptr;
		HandleRoomInstanceCreation(RoomData.RoomToSpawn);

		if (DoesRoomOverlap(RoomData.RoomToSpawn, SpawnLocation, SpawnRotation))continue;

		auto SpawnedRoom = SpawnRoom(Data, RoomData.LevelName, SpawnLocation, SpawnRotation);

		exit->SpawnedRoom = SpawnedRoom;
		Cast<ABaseRoom>(exit->GetOwner())->IsDeadEnd = false;
		TempExits.Remove(exit);

		for (auto Tag : SpawnedRoom->Tags)
		{
			TMap<FName, FRoomsWithTag>& RoomsWithTags = Data->RoomsWithTags;
			if(!RoomsWithTags.Contains(Tag))
			{
				FRoomsWithTag RoomsTemp = FRoomsWithTag({ SpawnedRoom });
				RoomsWithTags.Add(Tag, RoomsTemp);
			}
			else
			{
				RoomsWithTags[Tag].Rooms.Add(SpawnedRoom);
			}
		}

		return SpawnedRoom;
	}
	return nullptr;
}

TObjectPtr<ABaseRoom> URoomGenerationPass::SpawnRoom(ULevelData* Data, const FString RoomLevelPath, const FVector& Location, const FRotator& Rotation)
{
	if (RoomLevelPath == "")
	{
		return nullptr;
	}

	bool Success = false;
	ULevelStreamingDynamic* LevelStream = ULevelStreamingDynamic::LoadLevelInstance(
		GetWorld(),
		RoomLevelPath,
		Location,
		Rotation,
		Success
	);

	GetWorld()->FlushLevelStreaming();
	const auto LevelInstance = LevelStream->GetLoadedLevel();
	const auto Found = LevelInstance->Actors.FindByPredicate([](AActor* Actor)
	{
		return Actor->IsA(ABaseRoom::StaticClass());
	});

	const auto SpawnedRoom = Cast<ABaseRoom>(Found->Get());

	for (auto Actor : LevelInstance->Actors)
	{
		if (Actor == SpawnedRoom)continue;
		Actor->SetOwner(SpawnedRoom);
	}

	if (SpawnedRoom == nullptr)
	{
		return nullptr;
	}

	SpawnedRoom->InitializeComponents();
	TObjectPtr<URoomEntranceComponent> Entrance = SpawnedRoom->GetEntrance();

	if (Entrance != nullptr)
	{
		auto EntranceLocation = Entrance->GetComponentLocation();
		EntranceLocation.Z = Location.Z;
		const auto Offset = Location - EntranceLocation;
		SpawnedRoom->SetActorLocation(SpawnedRoom->GetActorLocation() + Offset);
	}

	Data->Exits.Append(SpawnedRoom->GetExits());
	TempExits.Append(SpawnedRoom->GetExits());
	Data->Rooms.Add(SpawnedRoom);
	return SpawnedRoom;
}


#pragma region temporary rooms
/**
 * @brief Creates a temporary room instance in a place far far away in the world for the generation purposes, it's an optimization
 * for room spawning and overlap detection
 * @param RoomToSpawn - Room that we want to place in the world
 */
void URoomGenerationPass::HandleRoomInstanceCreation(const TSubclassOf<ABaseRoom> RoomToSpawn)
{
	if (RoomToSpawn == nullptr)return;
	if (!RoomInstances.Contains(RoomToSpawn))
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const auto Instance = GetWorld()->SpawnActor<AActor>(RoomToSpawn, FVector(0, 0, 100000), FRotator::ZeroRotator, SpawnInfo);
		Instance->SetActorEnableCollision(false);
		RoomInstances.Add(RoomToSpawn, Cast<ABaseRoom>(Instance));
	}
}

void URoomGenerationPass::CleanUpTemporaryRoomInstances()
{
	for (const auto& Element : RoomInstances)
	{
		Element.Value->Destroy();
	}
}
#pragma endregion