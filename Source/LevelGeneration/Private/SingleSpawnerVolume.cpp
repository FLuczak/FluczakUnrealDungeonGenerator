// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleSpawnerVolume.h"

#include "MainGenerator.h"
#include "PropGenerationPass.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASingleSpawnerVolume::ASingleSpawnerVolume()
{
	PrimaryActorTick.bCanEverTick = true;
	ChildActor = CreateDefaultSubobject<UChildActorComponent>("Preview", false);
}

void ASingleSpawnerVolume::TryGenerate(UBaseGenerationPass* BaseGenerationPass) 
{
	ChildActor->DestroyChildActor();
	if(ActorToSpawn == nullptr)
	{
		return;
	}

	const UPropGenerationPassComponent* PropGenerationPass = Cast<UPropGenerationPassComponent>(BaseGenerationPass);
	if (PropGenerationPass == nullptr)
	{
		return;
	}

	const FTransform ComponentTransform = GetActorTransform();
	const FActorSpawnParameters Parameters{};
	const FVector SpawnLocation = ComponentTransform.GetLocation();
	const FRotator SpawnRotation = ComponentTransform.GetRotation().Rotator();
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ActorToSpawn.Get(), SpawnLocation, SpawnRotation, Parameters);

	if(Spawned)
	{
		Destroy();
	}
}

void ASingleSpawnerVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MainGenerator = Cast<AMainGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainGenerator::StaticClass()));

	if (MainGenerator == nullptr)
	{
		return;
	}

	MainGenerator->OnPassStarted.AddUObject(this, &ASingleSpawnerVolume::TryGenerate);
}

#if WITH_EDITOR
void ASingleSpawnerVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(ActorToSpawn != nullptr)
	{
		ChildActor->SetChildActorClass(ActorToSpawn);
		ChildActor->CreateChildActor();
		return;
	}
}
#endif



