// Fill out your copyright notice in the Description page of Project Settings.


#include "PropSpawner.h"

#include "LevelData.h"
#include "PropGenerationPass.h"
#include "Kismet/KismetSystemLibrary.h"

UPropSpawner::UPropSpawner()
{
	SetGenerateOverlapEvents(false);
	SetCollisionProfileName(TEXT("GenerationData"));
	UPrimitiveComponent::SetSimulatePhysics(false);
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UPropSpawner::Generate(ULevelData* Data)
{
	const FVector ScaledExtent = GetScaledBoxExtent();
	const FTransform ComponentTransform = GetComponentTransform();

	const FVector BoxCenter = ComponentTransform.GetLocation();

	const FVector Min = BoxCenter - ScaledExtent;
	const FVector Max = BoxCenter + ScaledExtent;

	BoundingBox = FBox(Min, Max);

	for (const auto& Prop : PropsToSpawn)
	{
		if (DoesPropHaveOverlaps(Prop->GetProp()))continue;
		if (!Prop->Evaluate(Data)) continue;
		SpawnProp(Data, Prop->GetProp());
	}
}

void UPropSpawner::InjectPass(TObjectPtr<UPropGenerationPassComponent> ToSet)
{
	GenerationPass = ToSet;
}

#ifdef WITH_EDITOR
FReply UPropSpawner::OnPreviewButtonPressed(size_t PropIndex)
{
	if (CurrentPreview == nullptr)
	{
		const auto Owner = GetOwner();
		CurrentPreview = NewObject<UChildActorComponent>(Owner, UChildActorComponent::StaticClass(), FName("Preview_" + GetName()));
		CurrentPreview->SetRelativeLocation(FVector::Zero());
		CurrentPreview->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
		const double RotationOffset = FMath::RandRange(-2, 2) * 90.0f;
		auto Rotation = CurrentPreview->GetComponentRotation();
		Rotation.Yaw = RotationOffset;
		CurrentPreview->SetWorldRotation(Rotation);
		CurrentPreview->RegisterComponent();
		Owner->AddInstanceComponent(CurrentPreview);
	}

	if (CurrentPreview == nullptr)return FReply::Unhandled();


	CurrentPreview->SetWorldLocation(GetComponentLocation(), false);
	CurrentPreview->SetChildActorClass(PropsToSpawn[PropIndex]->GetProp());
	CurrentPreview->CreateChildActor();

	return FReply::Handled();
}
#endif

FRotator UPropSpawner::GetRotationForProp() const
{
	auto Rotation = GetComponentRotation();

	if(SetRandomRotation)
	{
		const double RotationOffset = FMath::RandRange(-2, 2) * 90.0f;
		Rotation.Yaw = RotationOffset;
	}

	return Rotation;
}

void UPropSpawner::SpawnProp(ULevelData* Data,const TSubclassOf<AActor>& PropToSpawn) const
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const auto SpawnedActor = GetWorld()->SpawnActor<AActor>(PropToSpawn, GetComponentLocation(), GetRotationForProp(), SpawnInfo);
	Data->Props.Add(SpawnedActor);
	SpawnedActor->Tags.Add("Prop");
}

bool UPropSpawner::DoesPropHaveOverlaps(const TSubclassOf<AActor>& PropToSpawn) const
{
	const auto Box = GenerationPass->GetPropBoxAtPosition(PropToSpawn,GetComponentLocation());
	TArray<FHitResult> HitResults;

	if (!BoundingBox.IsInside(Box))
	{
		UE_LOG(LogTemp, Error, TEXT("Could not spawn %s because it is not within the bounds of the spawner"),*PropToSpawn->GetName())
		return true;
	}

	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), Box.GetCenter(), Box.GetCenter() + FVector::UpVector, Box.GetExtent()/2.0f, FRotator::ZeroRotator, TraceTypeQuery3, false, {}, EDrawDebugTrace::None, HitResults, true);


	for (auto HitResult : HitResults)
	{
		if (HitResult.GetComponent()->ComponentTags.Contains("Prop")) { return true; }
		if (HitResult.GetComponent()->ComponentTags.Contains("Wall")) { return true; }
		if (HitResult.GetComponent()->ComponentTags.Contains("Door")) { return true; }
	}

	return false;
}

