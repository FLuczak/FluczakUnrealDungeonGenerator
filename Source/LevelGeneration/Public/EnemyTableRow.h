// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyTableRow.generated.h"


UENUM(BlueprintType)
enum class EEnemyGrade: uint8
{
	LIGHT UMETA(DisplayName = "Light"),
	MEDIUM   UMETA(DisplayName = "Medium"),
	HEAVY      UMETA(DisplayName = "Heavy"),
	ELITE   UMETA(DisplayName = "Elite")
};

class UEnemySpawnRule;

USTRUCT(BlueprintType, Blueprintable)
struct LEVELGENERATION_API FEnemyTableRow: public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawn = nullptr;

	UPROPERTY(EditAnywhere)
	EEnemyGrade EnemyGrade = EEnemyGrade::LIGHT;
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
	virtual void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName,TArray<FString>& OutCollectedImportProblems) override;
};
