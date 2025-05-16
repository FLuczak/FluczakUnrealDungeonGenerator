// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "PropWithSpawnRules.h"
#include "PropSpawner.generated.h"

class UPropGenerationPassComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LEVELGENERATION_API UPropSpawner : public UBoxComponent
{
	GENERATED_BODY()
public:
	UPropSpawner();
	void Generate(ULevelData* Data);
	void InjectPass(TObjectPtr<UPropGenerationPassComponent> ToSet);
	TArray<TObjectPtr<UPropWithSpawnRules>> GetProps() { return PropsToSpawn; }

// For prop preview tool used only inside the editor
#ifdef WITH_EDITOR 
	FReply OnPreviewButtonPressed(size_t PropIndex);
#endif

private:
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess))
	bool SetRandomRotation = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<TObjectPtr<UPropWithSpawnRules>> PropsToSpawn;
	FBox BoundingBox{};

	FRotator GetRotationForProp() const;
	void SpawnProp(ULevelData* Data, const TSubclassOf<AActor>& PropToSpawn) const;
	bool DoesPropHaveOverlaps(const TSubclassOf<AActor>& PropToSpawn) const;
	TObjectPtr<UPropGenerationPassComponent> GenerationPass = nullptr;

	// For prop preview tool used only inside the editor
#ifdef WITH_EDITOR 
	TObjectPtr<UChildActorComponent> CurrentPreview = nullptr;
#endif


};
