// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <random>

#include "CoreMinimal.h"
#include "LevelData.h"
#include "Components/BoxComponent.h"
#include "ScatteredEnemySpawner.generated.h"


struct EnemyData;
enum class EEnemyGrade : uint8;

USTRUCT()
struct FAmountPerEnemyRarity
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	int Amount = 0;
	UPROPERTY(EditAnywhere)
	EEnemyGrade Rarity;
};

class UEnemyGenerationPass;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LEVELGENERATION_API UScatteredEnemySpawner : public UBoxComponent
{
	GENERATED_BODY()
public:
	UScatteredEnemySpawner();
	void InitializeBoundingBox();
	void Generate(ULevelData* LevelData, TArray<EnemyData>& Data);
	void InjectRandom(std::mt19937 GeneratorToSet, int SeedToSet);
	void InjectPass(TObjectPtr<UEnemyGenerationPass> Pass);
private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	TArray<FAmountPerEnemyRarity> EnemiesToSpawn;
	TOptional<TSubclassOf<AActor>> GetRandomEnemyOfRarity(TArray<EnemyData> Data, EEnemyGrade Rarity);
	bool IsLocationValid(const FVector& Point, const TSubclassOf<AActor>& Actor) const;
	void SpawnEnemyOfRarity(ULevelData* LevelData, TArray<EnemyData>& Data, EEnemyGrade Rarity);
	FVector GetRandomPointInsideTheBox() const;

	TObjectPtr<UEnemyGenerationPass> GenerationPass;
	FBox BoundingBox{};
	std::mt19937 Generator;
	int Seed = 0;
};
