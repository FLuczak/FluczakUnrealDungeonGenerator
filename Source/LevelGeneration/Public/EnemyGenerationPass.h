// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGenerationPass.h"
#include "EnemyGenerationPass.generated.h"

enum class EEnemyGrade : uint8;

struct EnemyData
{
	TSubclassOf<AActor> Actor;
	EEnemyGrade EnemyGrade;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LEVELGENERATION_API UEnemyGenerationPass : public UBaseGenerationPass
{
	GENERATED_BODY()

public:
	virtual void Generate(ULevelData* Data) override;
	FBox GetEnemyBoxAtPosition(const TSubclassOf<AActor>& Enemy, const FVector& Position);
	TArray<EnemyData> GetEnemySpawnData(const ULevelData& Data) const;
private:
	void RegisterProp(const TSubclassOf<AActor>& Class);

	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	TObjectPtr<UDataTable> EnemiesDataTable = nullptr;
	TMap<TSubclassOf<AActor>, TObjectPtr<AActor>>EnemyInstances = {};
	TMap<TSubclassOf<AActor>, FBox> EnemyBounds = {};
};
