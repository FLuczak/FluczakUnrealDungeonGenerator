// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "ActorRoomDetectorComponent.generated.h"

class ABaseRoom;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoomChanged, ABaseRoom*, Room);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEVELGENERATION_API UActorRoomDetectorComponent : public UBoxComponent
{
	GENERATED_BODY()
public:	
	UActorRoomDetectorComponent();
	void SetInitialRoom(TObjectPtr<ABaseRoom> RoomToSet) { CurrentRoom = RoomToSet; }
protected:
	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay")
	FRoomChanged OnRoomChanged{};

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bIsPlayer = false;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABaseRoom> CurrentRoom = nullptr;
};
