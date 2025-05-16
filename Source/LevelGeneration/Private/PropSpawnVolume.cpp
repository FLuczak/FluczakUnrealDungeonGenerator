// Fill out your copyright notice in the Description page of Project Settings.


#include "PropSpawnVolume.h"

// Fill out your copyright notice in the Description page of Project Settings.



#include "MainGenerator.h"
#include "PropGenerationPass.h"
#include "PropWithSpawnRules.h"
#include "Kismet/GameplayStatics.h"

void APropSpawnVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if WITH_EDITOR
	if(CurrentPreview != nullptr)
	{
		CurrentPreview->DestroyChildActor();
		CurrentPreview->UnregisterComponent();
	}
#endif

	MainGenerator = Cast<AMainGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainGenerator::StaticClass()));

	if (MainGenerator == nullptr)
	{
		return;
	}

	MainGenerator->OnPassStarted.AddUObject(this, &APropSpawnVolume::TryGenerate);
}

#if WITH_EDITOR
FReply APropSpawnVolume::OnPreviewButtonPressed(size_t PropIndex)
{
	if (PropsToSpawn[PropIndex] == nullptr)return FReply::Unhandled();
	if (CurrentPreview == nullptr)
	{
		CurrentPreview = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass(), FName("Preview_" + GetName()));
		CurrentPreview->SetRelativeLocation(FVector::Zero());
		CurrentPreview->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		const double RotationOffset = FMath::RandRange(-2, 2) * 90.0f;
		auto Rotation = CurrentPreview->GetComponentRotation();
		Rotation.Yaw = RotationOffset;
		CurrentPreview->SetWorldRotation(Rotation);
		CurrentPreview->RegisterComponent();
		AddInstanceComponent(CurrentPreview);
	}

	if (CurrentPreview == nullptr)return FReply::Unhandled();


	CurrentPreview->SetWorldLocation(GetActorLocation(), false);
	CurrentPreview->SetChildActorClass(PropsToSpawn[PropIndex]->GetProp());
	CurrentPreview->CreateChildActor();

	return FReply::Handled();
}
#endif

void APropSpawnVolume::SpawnProp(ULevelData* Data, const TSubclassOf<AActor>& PropToSpawn) const
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const auto SpawnedActor = GetWorld()->SpawnActor<AActor>(PropToSpawn, GetActorLocation(), GetActorRotation(), SpawnInfo);
	Data->Props.Add(SpawnedActor);
	SpawnedActor->Tags.Add("Prop");
}

bool APropSpawnVolume::DoesPropHaveOverlaps(const TSubclassOf<AActor>& PropToSpawn, UPropGenerationPassComponent* GenerationPass)const
{
	const auto Box = GenerationPass->GetPropBoxAtPosition(PropToSpawn, GetActorLocation());
	TArray<FHitResult> HitResults;

	if (!BoundingBox.IsInside(Box))
	{
		UE_LOG(LogTemp, Error, TEXT("Could not spawn %s because it is not within the bounds of the spawner"), *PropToSpawn->GetName())
			return true;
	}

	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), Box.GetCenter(), Box.GetCenter() + FVector::UpVector, Box.GetExtent() / 2.0f, FRotator::ZeroRotator, TraceTypeQuery3, false, {}, EDrawDebugTrace::None, HitResults, true);


	for (auto HitResult : HitResults)
	{
		if (HitResult.GetComponent()->ComponentTags.Contains("Prop")) { return true; }
		if (HitResult.GetComponent()->ComponentTags.Contains("Wall")) { return true; }
		if (HitResult.GetComponent()->ComponentTags.Contains("Door")) { return true; }
	}

	return false;
}

void APropSpawnVolume::TryGenerate(UBaseGenerationPass* BaseGenerationPass)
{
	UPropGenerationPassComponent* PropGenerationPass = Cast<UPropGenerationPassComponent>(BaseGenerationPass);
	if (PropGenerationPass == nullptr)
	{
		return;
	}

	const FVector ScaledExtent = GetBounds().BoxExtent;
	const FTransform ComponentTransform = GetActorTransform();

	const FVector BoxCenter = ComponentTransform.GetLocation();

	const FVector Min = BoxCenter - ScaledExtent;
	const FVector Max = BoxCenter + ScaledExtent;

	BoundingBox = FBox(Min, Max);

	for (const auto& Prop : PropsToSpawn)
	{
		if (DoesPropHaveOverlaps(Prop->GetProp(), PropGenerationPass))continue;
		if (!Prop->Evaluate(MainGenerator->GetData())) continue;
		SpawnProp(MainGenerator->GetData(), Prop->GetProp());
	}
}
