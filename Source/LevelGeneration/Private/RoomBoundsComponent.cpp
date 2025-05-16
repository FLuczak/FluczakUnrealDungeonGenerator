#include "RoomBoundsComponent.h"

#include "BaseRoom.h"

URoomBoundsComponent::URoomBoundsComponent()
{
	SetCollisionProfileName(TEXT("GenerationData"));
	ComponentTags.Add("Room");
}
