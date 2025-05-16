// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyGenerationPass.h"

#include "BaseRoom.h"
#include "EnemySpawnRule.h"
#include "EnemyTableRow.h"
#include "LevelData.h"
#include "ScatteredEnemySpawner.h"

void UEnemyGenerationPass::Generate(ULevelData* Data)
{
	Super::Generate(Data);
	TArray<EnemyData> SpawnData = GetEnemySpawnData(*Data);

	if (EnemiesDataTable == nullptr) 
	{
		return;
	}

	for (const auto Room : Data->Rooms)
	{
		auto Spawners = Room->K2_GetComponentsByClass(UScatteredEnemySpawner::StaticClass());
		for (const auto ActorComponent : Spawners)
		{
			const auto Casted = Cast<UScatteredEnemySpawner>(ActorComponent);

			Casted->InjectRandom(RandomNumberGenerator, Seed);
			Casted->InjectPass(this);

			Casted->Generate(Data,SpawnData);
		}
	}

	for (const auto Instance : EnemyInstances)
	{
		Instance.Value->Destroy();
	}

	EnemyInstances.Empty();
	EnemyBounds.Empty();
}

FBox UEnemyGenerationPass::GetEnemyBoxAtPosition(const TSubclassOf<AActor>& Enemy, const FVector& Position)
{
	if (!EnemyInstances.Contains(Enemy))
	{
		RegisterProp(Enemy);
	}

	FBox ToReturn = EnemyBounds[Enemy];
	ToReturn = FBox(ToReturn.MoveTo(Position));

	return ToReturn;
}

void UEnemyGenerationPass::RegisterProp(const TSubclassOf<AActor>& Enemy)
{
	if (EnemyInstances.Contains(Enemy))
	{
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const auto Instance = GetWorld()->SpawnActor<AActor>(Enemy, FVector(0, 0, 100000), FRotator::ZeroRotator, SpawnInfo);
	Instance->SetActorEnableCollision(false);
	EnemyInstances.Add(Enemy, Instance);

	FVector Center;
	FVector Bounds;
	EnemyInstances[Enemy].Get()->GetActorBounds(false, Center, Bounds, true);

	EnemyBounds.Add(Enemy, FBox(Center - Bounds * 0.5f, Center + Bounds * 0.5f));
}

TArray<EnemyData> UEnemyGenerationPass::GetEnemySpawnData(const ULevelData& Data) const
{
	TArray<EnemyData>  ToReturn;

	TArray<FEnemyTableRow*> Rows{};
	EnemiesDataTable.Get()->GetAllRows(FString("context"), Rows);

	for (const auto Row : Rows)
	{
		EnemyData Temp = { Row->ActorToSpawn,Row->EnemyGrade };
		ToReturn.Add(Temp);
	}

	return ToReturn;
}
