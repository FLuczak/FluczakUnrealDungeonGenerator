

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseDoor.generated.h"


class ABaseRoom;

UCLASS()
class LEVELGENERATION_API ABaseDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseDoor();
	virtual void Tick(float DeltaTime) override;
	void SetRoom(TObjectPtr<ABaseRoom> Room)
	{
		MyRooms.Add(Room);
	}

	TArray<TObjectPtr<ABaseRoom>> GetRooms() { return MyRooms; }
	UFUNCTION(BlueprintNativeEvent)
	void OnRoomsSet();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess=true))
	TArray<TObjectPtr<ABaseRoom>> MyRooms{};
};
