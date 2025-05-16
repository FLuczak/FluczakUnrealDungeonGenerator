// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PropSpawnRules.generated.h"

/**
 * 
 */
UCLASS()
class LEVELGENERATION_API UPropSpawnRules : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CheckRules(const ULevelData* LevelData);
};
