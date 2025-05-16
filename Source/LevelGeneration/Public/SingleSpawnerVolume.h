// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGenerationPass.h"
#include "GameFramework/Actor.h"
#include "SingleSpawnerVolume.generated.h"

class AMainGenerator;

UCLASS()
class LEVELGENERATION_API ASingleSpawnerVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASingleSpawnerVolume();
	void TryGenerate(UBaseGenerationPass* BaseGenerationPass);
	virtual void PostInitializeComponents() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UChildActorComponent* ChildActor = nullptr;
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess = true))
	TSubclassOf<AActor> ActorToSpawn = nullptr;
	AMainGenerator* MainGenerator = nullptr;
};
