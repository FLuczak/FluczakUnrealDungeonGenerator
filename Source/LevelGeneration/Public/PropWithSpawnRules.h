// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PropWithSpawnRules.generated.h"

class UPropSpawnRules;
class ULevelData;

UCLASS()
class LEVELGENERATION_API UPropWithSpawnRules : public UPrimaryDataAsset
{
public:
	bool Evaluate(const ULevelData* LevelData);
	TSubclassOf<AActor> GetProp() { return PropToSpawn; }
private:
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor>  PropToSpawn{};
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UPropSpawnRules>> Rules{};
};
