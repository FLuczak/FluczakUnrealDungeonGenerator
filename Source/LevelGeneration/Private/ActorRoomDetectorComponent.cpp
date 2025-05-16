// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorRoomDetectorComponent.h"

#include "BaseRoom.h"



UActorRoomDetectorComponent::UActorRoomDetectorComponent()
{
	SetCollisionProfileName(TEXT("GenerationData"));
}

void UActorRoomDetectorComponent::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bBFromSweep, const FHitResult& SweepResult)
{
	if (!OtherComp->ComponentTags.Contains("Room"))
	{
		return;
	}

	const auto Room = Cast<ABaseRoom>(OtherComp->GetOwner());

	if (Room == nullptr || Room == CurrentRoom)
	{
		return;
	}

	if(ComponentTags.Contains("Player"))
	{
		if (CurrentRoom != nullptr)
		{
			CurrentRoom->PlayerLeftRoom();

			for (const auto Door : CurrentRoom->GetExits())
			{
				if (Door->SpawnedRoom == nullptr)
				{
					continue;
				}
				Door->SpawnedRoom->PlayerLeftNeighboringRoom.Broadcast();
			}

			for (const auto Door : Room->GetExits())
			{
				if (Door->SpawnedRoom == nullptr)
				{
					continue;
				}
				Door->SpawnedRoom->PlayerEnteredNeighboringRoom.Broadcast();
			}
		}

		Room->PlayerEnteredRoom();
	}

	CurrentRoom = Room;
	OnRoomChanged.Broadcast(CurrentRoom);
}


void UActorRoomDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UActorRoomDetectorComponent::HandleOverlap);

	TArray<UPrimitiveComponent*> Components;
	UpdateOverlaps();
	GetOverlappingComponents(Components);

	for (const auto Component : Components)
	{
		if(Cast<URoomBoundsComponent>(Component))
		{
			SetInitialRoom(Cast<ABaseRoom>(Component->GetOwner()));
		}
	}
}
