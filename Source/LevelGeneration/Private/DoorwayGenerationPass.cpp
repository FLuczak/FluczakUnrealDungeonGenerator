#include "DoorwayGenerationPass.h"

#include "BaseDoor.h"
#include "BaseRoom.h"
#include "LevelData.h"
#include "RoomExitComponent.h"

void UDoorwayGenerationPass::Generate(ULevelData* Data)
{
	Super::Generate(Data);

	for (const auto Exit : Data->Exits)
	{
		FActorSpawnParameters Parameters;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (Exit->SpawnedRoom == nullptr)
		{
			if(DoorCapToSpawn != nullptr)
			{
				auto Cap = GetWorld()->SpawnActor<AActor>(DoorCapToSpawn, Exit->GetComponentLocation(), Exit->GetComponentRotation(), Parameters);
			}
			
			continue;
		}

		auto Door = GetWorld()->SpawnActor<ABaseDoor>(DoorToSpawn, Exit->GetComponentLocation(), Exit->GetComponentRotation(), Parameters);

		if (Door == nullptr)
		{
			continue;
		}

		Door->SetRoom(Cast<ABaseRoom>(Exit->GetOwner()));
		Door->SetRoom(Exit->SpawnedRoom.Get());
		Door->OnRoomsSet();

		Data->Doors.Add(Door);
	}
}
