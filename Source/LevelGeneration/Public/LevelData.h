#pragma once

#include "CoreMinimal.h"
#include "Curve/GeneralPolygon2.h"
#include "LevelData.generated.h"

class ABaseRoom;
class URoomExitComponent;

USTRUCT(BlueprintType)
struct FRoomsWithTag
{
	GENERATED_BODY()
    FRoomsWithTag(){};
    FRoomsWithTag(const TArray<TObjectPtr<ABaseRoom>>& RoomsToSet) : Rooms(RoomsToSet){}
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<TObjectPtr<ABaseRoom>> Rooms{};
};

UCLASS(BlueprintType)
class ULevelData : public UObject
{
public:
    GENERATED_BODY()
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<ABaseRoom> StartRoom = nullptr;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
    TArray<TObjectPtr<ABaseRoom>> Rooms{};
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<TObjectPtr<URoomExitComponent>> Exits{};
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<TObjectPtr<AActor>> Props{};
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<TObjectPtr<AActor>> Enemies{};
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<TObjectPtr<AActor>> Doors{};
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TMap<FName, FRoomsWithTag> RoomsWithTags{};
    UPROPERTY(BlueprintReadOnly)
    FRandomStream RandomStream{};
};
