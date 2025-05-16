

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelData.h"
#include "MainGenerator.generated.h"

class UBaseGenerationPass;
DECLARE_MULTICAST_DELEGATE_OneParam(FGeneratorProgressDelegate, UBaseGenerationPass*);

UCLASS()
class LEVELGENERATION_API AMainGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AMainGenerator();
	UFUNCTION(CallInEditor)
	void InitializeAndGenerate();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void Generate();
	ULevelData* GetData() const { return LevelData; }
	FGeneratorProgressDelegate OnPassStarted;
	FGeneratorProgressDelegate OnPassEnded;
private:
	void CleanupLevelDataRuntime() const;
	UPROPERTY(meta=(allowPrivateAccess = true))
	bool bGeneratedFromEditor = false;

#if WITH_EDITOR
	UFUNCTION(CallInEditor,meta=(allowPrivateAccess = true))
	void CleanupLevelDataEditor() const;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (allowPrivateAccess = "true", ToolTip = "Seed that the generation will be based on"))
	int32 Seed = 1234567;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (allowPrivateAccess = "true", ToolTip = "During generation: should we set every pass's seed to the main seed"))
	bool ShouldSetupOtherGeneratorsSeeds = true;

	UPROPERTY(EditAnywhere)
	bool GenerateWithRandomSeed = true;

	UPROPERTY(VisibleAnywhere)
	ULevelData* LevelData=nullptr;
	TArray<TObjectPtr<UBaseGenerationPass>> GenerationPasses{};
};
