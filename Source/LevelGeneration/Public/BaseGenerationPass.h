

#pragma once

#include <random>
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseGenerationPass.generated.h"

class ULevelData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEVELGENERATION_API UBaseGenerationPass : public UActorComponent
{
	GENERATED_BODY()
public:
	UBaseGenerationPass();
	virtual  ~UBaseGenerationPass() override;
	//Base function for level generation, takes in level data and modifies it, along with the world state
	virtual void Generate(ULevelData* Data);
	void SetSeed(const int32 NewSeed);
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess = true))
	int Priority = 0;
protected:
	void SetupRNG();
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Generation", meta = (allowPrivateAccess = "true", ToolTip = "Seed that the generation will be based on"))
	int32 Seed = 1234567;
	//Generator that will generate numbers for this pass
	std::mt19937 RandomNumberGenerator{};
};
