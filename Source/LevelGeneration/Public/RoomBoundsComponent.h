

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "RoomBoundsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEVELGENERATION_API URoomBoundsComponent : public UBoxComponent
{
	GENERATED_BODY()
public:
	URoomBoundsComponent();
};
