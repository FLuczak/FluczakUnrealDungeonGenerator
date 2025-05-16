// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomFloorComponent.h"

#include "Misc/MapErrors.h"

URoomFloorComponent::URoomFloorComponent()
{
    this->ComponentTags.Add("Floor");
}

void URoomFloorComponent::BeginPlay()
{
	Super::BeginPlay();
    MeshData = UE::Geometry::FPolygon2d(Vertices);
}

#if WITH_EDITOR
void URoomFloorComponent::PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph)
{
	Super::PostLoadSubobjects(OuterInstanceGraph);
    if (GetStaticMesh() == nullptr)
    {
	    return;
    }

    //Already generated data, no need to do it again
    if (Vertices.Num() > 0 && MeshData.GetOuter().VertexCount() > 0)
    {
	    return;
    }

    Vertices.Empty();
    MeshData = {};

    GetStaticMesh()->bAllowCPUAccess = true;
    GenerateVertexData();
}
#endif

FVector URoomFloorComponent::CalculateCentroid(const TArray<FVector>& InVertices)
{
	FVector Centroid(0, 0, 0);
	const int32 ArraySize = InVertices.Num();

	for (const FVector& Vertex : InVertices)
	{
		Centroid += Vertex;
	}

	Centroid /= ArraySize;
    return Centroid;
}

UE::Geometry::FGeneralPolygon2d URoomFloorComponent::GetMeshData()
{
    MeshData = {};
    auto Temp = TArray<FVector>();
    const FTransform Transform = GetComponentTransform();
    ;
    for (auto Vertex : Vertices)
    {
        auto worldSpaceVertex = GetComponentLocation() + Transform.TransformVector(Vertex);
        Temp.Add(worldSpaceVertex);
    }

    SortVerticesClockwise(Temp);
    MeshData = UE::Geometry::FPolygon2d(Temp);
    return MeshData;
}

void URoomFloorComponent::SortVerticesClockwise(TArray<FVector>& OutVertices)
{
    auto Centroid = CalculateCentroid(OutVertices);

    OutVertices.Sort([Centroid](const FVector& A, const FVector& B)
        {
            if (A.X - Centroid.X >= 0 && B.X - Centroid.X < 0)
                return true;
            if (A.X - Centroid.X < 0 && B.X - Centroid.X >= 0)
                return false;
            if (A.X - Centroid.X == 0 && B.X - Centroid.X == 0) {
                if (A.Y - Centroid.Y >= 0 || B.Y - Centroid.Y >= 0)
                    return A.Y > B.Y;
                return B.Y > A.Y;
            }

            // compute the cross product of vectors (centroid -> a) X (centroid -> b)
            const int Det = (A.X - Centroid.X) * (B.Y - Centroid.Y) - (B.X - Centroid.X) * (A.Y - Centroid.Y);
            if (Det < 0)
                return true;
            if (Det > 0)
                return false;


            const int D1 = (A.X - Centroid.X) * (A.X - Centroid.X) + (A.Y - Centroid.Y) * (A.Y - Centroid.Y);
            const int D2 = (B.X - Centroid.X) * (B.X - Centroid.X) + (B.Y - Centroid.Y) * (B.Y - Centroid.Y);
            return D1 > D2;
        });
}

/**
 * @brief Set Vertices to highest vertices of the floor mesh.
 */
void URoomFloorComponent::GenerateVertexData()
{
	TSet<FVector> TempVertices = TSet<FVector>();

    if (GetStaticMesh() == nullptr)
    {
        return;
    }

    if (GetStaticMesh()->GetRenderData() == nullptr)
    {
        return;
    }

    if (GetStaticMesh()->GetRenderData()->LODResources.Num() == 0)
    {
	    return;
    }

    FPositionVertexBuffer* VertexBuffer = &GetStaticMesh()->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;

	if (VertexBuffer == nullptr)
    {
        return;
    }

    double MaxZPosition = DBL_MIN;
    const int32 VertexCount = VertexBuffer->GetNumVertices();
    const auto Transform = GetComponentTransform();

    for (int32 Index = 0; Index < VertexCount; Index++)
    {
        const FVector3f VertexPosition = VertexBuffer->VertexPosition(Index);
        auto Temp = FVector(VertexPosition.X, VertexPosition.Y, VertexPosition.Z);
        const FVector WorldSpaceVertexLocation = GetComponentLocation() + Transform.TransformVector(Temp);
        MaxZPosition = FMath::Max(WorldSpaceVertexLocation.Z, MaxZPosition);

        TempVertices.Add(WorldSpaceVertexLocation);
    }

    Vertices = {};
    for (auto Position : TempVertices)
    {
        Vertices.Add(Position);
    }


    Vertices.RemoveAll([this,MaxZPosition](const FVector& Lp)
    {
        return Lp.Z + VerticalBias < MaxZPosition;
    });

    SortVerticesClockwise(Vertices);
}
