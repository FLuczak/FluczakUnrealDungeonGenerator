// Fill out your copyright notice in the Description page of Project Settings.


#include "ScatteredEnemySpawner.h"

#include "ActorRoomDetectorComponent.h"
#include "EnemyGenerationPass.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BaseRoom.h"

void UScatteredEnemySpawner::Generate(ULevelData* LevelData,TArray<EnemyData>& Data)
{
	InitializeBoundingBox();

	for (const auto Element : EnemiesToSpawn)
	{
		for(int32 i= 0; i< Element.Amount;i++)
		{
			SpawnEnemyOfRarity(LevelData, Data, Element.Rarity);
		}
	}
}

UScatteredEnemySpawner::UScatteredEnemySpawner()
{
	SetCollisionProfileName(TEXT("GenerationData"));
}

void UScatteredEnemySpawner::InitializeBoundingBox()
{
	const FVector ScaledExtent = GetScaledBoxExtent();
	const FTransform ComponentTransform = GetComponentTransform();

	const FVector BoxCenter = ComponentTransform.GetLocation();

	const FVector Min = BoxCenter - ScaledExtent;
	const FVector Max = BoxCenter + ScaledExtent;

	BoundingBox = FBox(Min, Max);
}

void UScatteredEnemySpawner::InjectRandom(std::mt19937 GeneratorToSet, int SeedToSet)
{
	Generator = GeneratorToSet;
	Seed = SeedToSet;
}

void UScatteredEnemySpawner::InjectPass(TObjectPtr<UEnemyGenerationPass> Pass)
{
	GenerationPass = Pass;
}

TOptional<TSubclassOf<AActor>> UScatteredEnemySpawner::GetRandomEnemyOfRarity(TArray<EnemyData> Data, EEnemyGrade Rarity)
{
	TArray<TSubclassOf<AActor>> AllEnemiesOfRarity = {};
	for(EnemyData Enemy : Data)
	{
		if(Enemy.EnemyGrade != Rarity)
		{
			continue;
		}

		AllEnemiesOfRarity.Add(Enemy.Actor);
	}

	const int32 NumberOfEntries = AllEnemiesOfRarity.Num();
	if (NumberOfEntries == 0)return {};

	std::uniform_int_distribution<int> Uid(0,NumberOfEntries-1);
	return AllEnemiesOfRarity[Uid(Generator)];
}

bool UScatteredEnemySpawner::IsLocationValid(const FVector& Point, const TSubclassOf<AActor>& Actor) const
{
	const auto Box = GenerationPass->GetEnemyBoxAtPosition(Actor, Point);
	TArray<FHitResult> HitResults;

	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), Box.GetCenter(), Box.GetCenter() + FVector::UpVector, Box.GetExtent() / 2.0f, FRotator::ZeroRotator, TraceTypeQuery3, false, {}, EDrawDebugTrace::None, HitResults, true);

	for (auto HitResult : HitResults)
	{
		if (HitResult.GetComponent()->ComponentTags.Contains("Prop"))
		{
			UE_LOG(LogTemp, Error, TEXT("Could not spawn %s because it is not within the bounds of the spawner"), *Actor->GetName())
			return false;
		}
		if (HitResult.GetComponent()->ComponentTags.Contains("Enemy"))
		{
			UE_LOG(LogTemp, Error, TEXT("Could not spawn %s because it is not within the bounds of the spawner"), *Actor->GetName())
			return false;
		}
		if (HitResult.GetComponent()->ComponentTags.Contains("Wall"))
		{
			UE_LOG(LogTemp, Error, TEXT("Could not spawn %s because it is not within the bounds of the spawner"), *Actor->GetName())
			return false;
		}
		if (HitResult.GetComponent()->ComponentTags.Contains("Door"))
		{
			UE_LOG(LogTemp, Error, TEXT("Could not spawn %s because it is not within the bounds of the spawner"), *Actor->GetName())
			return false;
		}
	}

	return true;
}

void UScatteredEnemySpawner::SpawnEnemyOfRarity(ULevelData* LevelData,TArray<EnemyData>& Data, EEnemyGrade Rarity)
{
	auto RandomEnemyOfRarity = GetRandomEnemyOfRarity(Data, Rarity);
	const TSubclassOf<AActor>* EnemyToSpawn = RandomEnemyOfRarity.GetPtrOrNull();

	const FVector Point = GetRandomPointInsideTheBox();


	if (EnemyToSpawn == nullptr)
	{
		return;
	}

	if (IsLocationValid(Point, *EnemyToSpawn))
	{
		auto SpawnedEnemy = GetWorld()->SpawnActor<AActor>(EnemyToSpawn->Get(), Point, FRotator::ZeroRotator, {});
		LevelData->Enemies.Add(SpawnedEnemy);

		if (SpawnedEnemy == nullptr)
		{
			return;
		}

		const auto SpawnedEnemyData = Data.FindByPredicate([EnemyToSpawn](const EnemyData& TempData)
			{
				return TempData.Actor.Get() == EnemyToSpawn->Get();
			});

		const auto RoomDetector = SpawnedEnemy->GetComponentByClass(UActorRoomDetectorComponent::StaticClass());

		if(RoomDetector !=nullptr)
		{
			const auto Casted = Cast<UActorRoomDetectorComponent>(RoomDetector);
			ABaseRoom* OwningRoom = Cast<ABaseRoom>(GetOwner());
			OwningRoom->AddEnemy(SpawnedEnemy);
			Casted->SetInitialRoom(OwningRoom);
		}
	}
}

FVector UScatteredEnemySpawner::GetRandomPointInsideTheBox() const
{
	const auto BoxExtents = GetScaledBoxExtent();
	const auto BoxLocation = GetComponentLocation();
	const FVector WorldExtent = GetComponentRotation().RotateVector(BoxExtents);

	const auto Min = BoxLocation - WorldExtent;
	const auto Max = BoxLocation + WorldExtent;

	auto Pos = FMath::RandPointInBox(FBox(Min, Max));
	Pos.Z = GetComponentLocation().Z;
	return Pos;
}
