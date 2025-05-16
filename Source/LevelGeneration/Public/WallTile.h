

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WallTile.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEVELGENERATION_API UWallTileComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	bool IsCorner = false;
	UPROPERTY(VisibleAnywhere)
	float PercentageOnAWall = 0.0f;
	UPROPERTY(VisibleAnywhere)
	FVector Normal;
	UWallTileComponent();
};
