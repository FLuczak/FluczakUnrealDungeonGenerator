// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGenerationPass.h"
#include "EnemyGenerationPass.h"
#include "PropGenerationPass.h"
#include "GameFramework/Volume.h"
#include "ScatteredEnemySpawnVolume.generated.h"

class AMainGenerator;
class ULevelData;
class UPropGenerationPassComponent;
class UPropWithSpawnRules;
class UBaseGenerationPass;
class ABaseRoom;
struct EnemyData;
struct FAmountPerEnemyRarity;

/**
 * 
 */
UCLASS()
class LEVELGENERATION_API AScatteredEnemySpawnVolume : public AVolume
{
public:
	GENERATED_BODY()
	void TryGenerate(UBaseGenerationPass* BaseGenerationPass);
	virtual void PostInitializeComponents() override;
private:
	void InitializeBoundingBox();
	TOptional<TSubclassOf<AActor>> GetRandomEnemyOfRarity(TArray<EnemyData> Data, EEnemyGrade Rarity);
	bool IsLocationValid(const FVector& Point, const TSubclassOf<AActor>& Actor, UEnemyGenerationPass* GenerationPass) const;
	void SpawnEnemyOfRarity(ULevelData* LevelData, TArray<EnemyData>& Data, EEnemyGrade Rarity, UEnemyGenerationPass* GenerationPass);
	FVector GetRandomPointInsideTheBox() const;

	AMainGenerator* MainGenerator = nullptr;
	FBox BoundingBox{};
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<FAmountPerEnemyRarity> EnemiesToSpawn;
	std::mt19937 Generator;
	int Seed = 0;
};
