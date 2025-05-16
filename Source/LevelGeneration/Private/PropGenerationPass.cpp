
#include "PropGenerationPass.h"

#include "BaseRoom.h"
#include "LevelData.h"
#include "PropSpawner.h"

void UPropGenerationPassComponent::SpawnProps(ULevelData* Data)
{
	for (const auto Room : Data->Rooms)
	{
		auto PropSpawners = Room->K2_GetComponentsByClass(UPropSpawner::StaticClass());
		for (auto PropSpawner : PropSpawners)
		{
			const auto Casted = Cast<UPropSpawner>(PropSpawner);
			Casted->InjectPass(this);
			Casted->Generate(Data);
		}
	}
}

void UPropGenerationPassComponent::ClearTemporarySpawningData()
{
	for (const auto Pair: PropInstances)
	{
		Pair.Value->Destroy();
	}

	PropInstances.Empty();
	PropBounds.Empty();
}

void UPropGenerationPassComponent::Generate(ULevelData* Data)
{
	Super::Generate(Data);

	SpawnProps(Data);
	ClearTemporarySpawningData();
}

void UPropGenerationPassComponent::RegisterProp(const TSubclassOf<AActor>& Prop)
{
	if (PropInstances.Contains(Prop))
	{
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const auto Instance = GetWorld()->SpawnActor<AActor>(Prop, FVector(0, 0, 100000), FRotator::ZeroRotator, SpawnInfo);
	Instance->SetActorEnableCollision(false);
	PropInstances.Add(Prop, Instance);

	FVector Center;
	FVector Bounds;
	PropInstances[Prop].Get()->GetActorBounds(false, Center, Bounds, true);

	PropBounds.Add(Prop, FBox(Center - Bounds * 0.5f, Center + Bounds * 0.5f));
}

FBox UPropGenerationPassComponent::GetPropBoxAtPosition(const TSubclassOf<AActor> Prop, const FVector Position)
{
	if(!PropInstances.Contains(Prop))
	{
		RegisterProp(Prop);
	}

	FBox ToReturn = PropBounds[Prop];
	ToReturn = FBox(ToReturn.MoveTo(Position));

	return ToReturn;
}
