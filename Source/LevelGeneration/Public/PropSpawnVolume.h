// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "PropSpawnVolume.generated.h"

class ULevelData;
class AMainGenerator;
class UPropGenerationPassComponent;
class UPropWithSpawnRules;
class UBaseGenerationPass;
/**
 * 
 */
UCLASS()
class LEVELGENERATION_API APropSpawnVolume : public AVolume
{
public:
	GENERATED_BODY()
	virtual void PostInitializeComponents() override;
	TArray<TObjectPtr<UPropWithSpawnRules>> GetProps() { return PropsToSpawn; }

#if WITH_EDITOR
	FReply OnPreviewButtonPressed(size_t PropIndex);
	TObjectPtr<UChildActorComponent> CurrentPreview = nullptr;
#endif
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UPropWithSpawnRules>> PropsToSpawn;
private:
	void SpawnProp(ULevelData* Data, const TSubclassOf<AActor>& PropToSpawn) const;
	bool DoesPropHaveOverlaps(const TSubclassOf<AActor>& PropToSpawn, UPropGenerationPassComponent* GenerationPass) const;

	void TryGenerate(UBaseGenerationPass* BaseGenerationPass);
	AMainGenerator* MainGenerator = nullptr;
	FBox BoundingBox{};
};
