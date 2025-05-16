

#pragma once

#include "CoreMinimal.h"
#include "BaseGenerationPass.h"
#include "Components/ActorComponent.h"
#include "DoorwayGenerationPass.generated.h"

class ABaseDoor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEVELGENERATION_API UDoorwayGenerationPass : public UBaseGenerationPass
{
	GENERATED_BODY()
public:
	virtual void Generate(ULevelData* Data) override;
private:
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess= true))
	TSubclassOf<ABaseDoor> DoorToSpawn = nullptr;
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess= true))
	TSubclassOf<AActor> DoorCapToSpawn = nullptr;
};
