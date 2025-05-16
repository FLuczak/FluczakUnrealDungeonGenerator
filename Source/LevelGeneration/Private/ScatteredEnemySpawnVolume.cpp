#include "ScatteredEnemySpawnVolume.h"

#include "ActorRoomDetectorComponent.h"
#include "BaseRoom.h"
#include "EnemyGenerationPass.h"
#include "MainGenerator.h"
#include "ScatteredEnemySpawner.h"
#include "Kismet/GameplayStatics.h"



void AScatteredEnemySpawnVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MainGenerator = Cast<AMainGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainGenerator::StaticClass()));

	if (MainGenerator == nullptr)
	{
		return;
	}

	MainGenerator->OnPassStarted.AddUObject(this, &AScatteredEnemySpawnVolume::TryGenerate);
}


void AScatteredEnemySpawnVolume::TryGenerate(UBaseGenerationPass* BaseGenerationPass)
{
	 UEnemyGenerationPass* EnemyGenerationPass = Cast<UEnemyGenerationPass>( BaseGenerationPass);

	if(EnemyGenerationPass == nullptr)
	{
		return;
	}

	InitializeBoundingBox();
	auto SpawnData = EnemyGenerationPass->GetEnemySpawnData(*MainGenerator->GetData());

	for (const auto Element : EnemiesToSpawn)
	{
		for (int32 i = 0; i < Element.Amount; i++)
		{
			SpawnEnemyOfRarity(MainGenerator->GetData(),SpawnData, Element.Rarity,EnemyGenerationPass);
		}
	}
}


void AScatteredEnemySpawnVolume::InitializeBoundingBox()
{
	const FVector ScaledExtent = GetBounds().BoxExtent;
	const FTransform ComponentTransform = GetActorTransform();

	const FVector BoxCenter = ComponentTransform.GetLocation();

	const FVector Min = BoxCenter - ScaledExtent;
	const FVector Max = BoxCenter + ScaledExtent;

	BoundingBox = FBox(Min, Max);
}

TOptional<TSubclassOf<AActor>> AScatteredEnemySpawnVolume::GetRandomEnemyOfRarity(TArray<EnemyData> Data,
	EEnemyGrade Rarity)
{
	TArray<TSubclassOf<AActor>> AllEnemiesOfRarity = {};
	for (EnemyData Enemy : Data)
	{
		if (Enemy.EnemyGrade != Rarity)
		{
			continue;
		}

		AllEnemiesOfRarity.Add(Enemy.Actor);
	}

	const int32 NumberOfEntries = AllEnemiesOfRarity.Num();
	if (NumberOfEntries == 0)return {};

	std::uniform_int_distribution<int> Uid(0, NumberOfEntries - 1);
	return AllEnemiesOfRarity[Uid(Generator)];
}

bool AScatteredEnemySpawnVolume::IsLocationValid(const FVector& Point, const TSubclassOf<AActor>& Actor,UEnemyGenerationPass* GenerationPass) const
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

void AScatteredEnemySpawnVolume::SpawnEnemyOfRarity(ULevelData* LevelData, TArray<EnemyData>& Data,EEnemyGrade Rarity,UEnemyGenerationPass* GenerationPass)
{
	if (GetOwner() == nullptr)
	{
		return;
	}

	auto RandomEnemyOfRarity = GetRandomEnemyOfRarity(Data, Rarity);
	const TSubclassOf<AActor>* EnemyToSpawn = RandomEnemyOfRarity.GetPtrOrNull();

	const FVector Point = GetRandomPointInsideTheBox();


	if (EnemyToSpawn == nullptr)
	{
		return;
	}

	if (IsLocationValid(Point, *EnemyToSpawn,GenerationPass))
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

		if (RoomDetector != nullptr)
		{
			const auto Casted = Cast<UActorRoomDetectorComponent>(RoomDetector);
			const auto OwningRoom = Cast<ABaseRoom>(GetOwner());
			OwningRoom->AddEnemy(SpawnedEnemy);
			Casted->SetInitialRoom(OwningRoom);
		}
	}
}

FVector AScatteredEnemySpawnVolume::GetRandomPointInsideTheBox() const
{
	const auto BoxExtents = GetBounds().GetBox().GetExtent();
	const auto BoxLocation = GetActorLocation();
	const FVector WorldExtent = GetActorRotation().RotateVector(BoxExtents);

	const auto Min = BoxLocation - WorldExtent;
	const auto Max = BoxLocation + WorldExtent;

	auto Pos = FMath::RandPointInBox(FBox(Min, Max));
	Pos.Z = GetActorLocation().Z;
	return Pos;
}

