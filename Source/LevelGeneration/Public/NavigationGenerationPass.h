// Filln out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGenerationPass.h"
#include "NavigationGenerationPass.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LEVELGENERATION_API UNavigationGenerationPass : public UBaseGenerationPass
{
	GENERATED_BODY()
public:
	virtual void Generate(ULevelData* Data) override;
};
