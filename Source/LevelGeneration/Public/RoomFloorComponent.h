
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Curve/GeneralPolygon2.h"
#include "RoomFloorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEVELGENERATION_API URoomFloorComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:	
	URoomFloorComponent();
	virtual void BeginPlay() override;
#if WITH_EDITOR
	/**
	 * @brief SHOULD ONLY RUN IN THE EDITOR. Generate and store the vertex data for the room instance to off-load the work from runtime to in-editor.
	 * @param OuterInstanceGraph - an argument required for PostLoadSubObjects- not required for the function to run
	 */
	virtual void PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph) override;
#endif
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", ToolTip = "Vertices that are lower than VerticalBias inside the mesh want be taken into account in the mesh calculation, used to avoid minimal differences in vertex vertical positions"))
	double VerticalBias = 0.15f;
	static void SortVerticesClockwise(TArray<FVector>& OutVertices);
	static FVector CalculateCentroid(const TArray<FVector>& InVertices);
	UE::Geometry::FGeneralPolygon2d GetMeshData();
private:
	UE::Geometry::FGeneralPolygon2d MeshData = {};
	void GenerateVertexData();
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<FVector> Vertices;
};
