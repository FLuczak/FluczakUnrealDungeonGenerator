

#pragma once

#include "CoreMinimal.h"
#include "BaseGenerationPass.h"
#include "Components/SceneComponent.h"
#include "PropGenerationPass.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEVELGENERATION_API UPropGenerationPassComponent : public UBaseGenerationPass
{
public:
	GENERATED_BODY()
	virtual void Generate(ULevelData* Data) override;
	FBox GetPropBoxAtPosition(TSubclassOf<AActor> Prop, FVector Position);
private:
	void RegisterProp(const TSubclassOf<AActor>& Prop);
	void SpawnProps(ULevelData* Data);
	void ClearTemporarySpawningData();
	TMap<TSubclassOf<AActor>, TObjectPtr<AActor>>PropInstances = {};
	TMap<TSubclassOf<AActor>, FBox> PropBounds = {};
};
