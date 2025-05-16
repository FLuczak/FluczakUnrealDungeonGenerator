// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorLoopGenerationPass.h"

#include "BaseRoom.h"

void UDoorLoopGenerationPass::Generate(ULevelData* Data)
{
	Super::Generate(Data);
	for (const auto Exit : Data->Exits)
	{
		if (Exit->SpawnedRoom != nullptr)continue;
		std::uniform_real_distribution Distribution(0.0f, 1.0f);
		const float Chance = Distribution(RandomNumberGenerator);
		if(Chance > ChanceToCreateLoop)continue;
		float Dist = MAX_FLT;
		const TObjectPtr<URoomExitComponent> ClosestExit = GetClosestExit(*Data,Exit,Dist);

		if (ClosestExit->SpawnedRoom != nullptr)continue;
		if (Dist <= MaxDistanceToConnectRooms)
		{
			ClosestExit->SpawnedRoom = Cast<ABaseRoom>(Exit->GetOwner());
			Exit->SpawnedRoom = Cast<ABaseRoom>(ClosestExit->GetOwner());
		}
	}
}

TObjectPtr<URoomExitComponent> UDoorLoopGenerationPass::GetClosestExit(ULevelData& Data,TObjectPtr<URoomExitComponent> Exit,float& OutDistanceToExit)
{
	TObjectPtr<URoomExitComponent> Closest = nullptr;
	float ClosestDistance = MAX_FLT;
	TArray<TObjectPtr<URoomExitComponent>>& RoomExitComponents = Data.Exits;
	for (auto Element : RoomExitComponents)
	{
		if (Element == Exit)continue;
		const float Dist = FVector::DistSquared(Element->GetComponentLocation(), Exit->GetComponentLocation());
		if (Dist < ClosestDistance)
		{
			ClosestDistance = Dist;
			Closest = Element;
		}
	}

	OutDistanceToExit = ClosestDistance;
	return Closest;
}
