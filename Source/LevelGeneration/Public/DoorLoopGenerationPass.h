// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGenerationPass.h"
#include "DoorLoopGenerationPass.generated.h"

class URoomExitComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LEVELGENERATION_API UDoorLoopGenerationPass : public UBaseGenerationPass
{
	GENERATED_BODY()
public:
	virtual void Generate(ULevelData* Data) override;
private:
	static TObjectPtr<URoomExitComponent> GetClosestExit(ULevelData& Data, TObjectPtr<URoomExitComponent> Exit, float& OutDistanceToExit);
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess))
	float MaxDistanceToConnectRooms = 0.45f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float ChanceToCreateLoop = 0.55f;
};
