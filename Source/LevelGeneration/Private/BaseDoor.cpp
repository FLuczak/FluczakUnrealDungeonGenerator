


#include "BaseDoor.h"


// Sets default values
ABaseDoor::ABaseDoor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseDoor::OnRoomsSet_Implementation()
{
}

void ABaseDoor::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


