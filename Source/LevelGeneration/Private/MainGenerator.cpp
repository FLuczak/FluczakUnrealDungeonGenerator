


#include "MainGenerator.h"
#include "BaseGenerationPass.h"
#include "BaseRoom.h"

#if WITH_EDITOR
#include "Subsystems/EditorActorSubsystem.h"
#endif

// Sets default values
AMainGenerator::AMainGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMainGenerator::InitializeAndGenerate()
{
	auto PassActorComponents = K2_GetComponentsByClass(UBaseGenerationPass::StaticClass());

	if (GenerationPasses.Num() == 0)
	{
		for (const auto ActorComponent : PassActorComponents)
		{
			UBaseGenerationPass* CastedPass = Cast<UBaseGenerationPass>(ActorComponent);
			GenerationPasses.Add(CastedPass);
			GenerationPasses.Sort([](TObjectPtr<UBaseGenerationPass> lhs, TObjectPtr<UBaseGenerationPass> rhs)
				{
					return lhs->Priority < rhs->Priority;
				});
		}
	}

	Generate();

	if (GetWorld()->IsEditorWorld())
	{
		bGeneratedFromEditor = true;
	}
	else
	{
		bGeneratedFromEditor = false;
	}
}

void AMainGenerator::BeginPlay()
{
	Super::BeginPlay();
	InitializeAndGenerate();
}

void AMainGenerator::Generate()
{
	LevelData = NewObject<ULevelData>();

	CleanupLevelDataRuntime();
#if WITH_EDITOR
	CleanupLevelDataEditor();
#endif

	if(GenerateWithRandomSeed)
	{
		Seed = FMath::RandRange(0, 10000000);
	}

	FMath::SRandInit(Seed);
	LevelData->RandomStream.Initialize(Seed);
	
	if(ShouldSetupOtherGeneratorsSeeds)
	{
		for (const auto Pass : GenerationPasses)
		{
			Pass->SetSeed(Seed);
		}
	}

	for (const auto Pass : GenerationPasses)
	{
		OnPassStarted.Broadcast(Pass);
		Pass->Generate(LevelData);
		OnPassEnded.Broadcast(Pass);
	}
}

void AMainGenerator::CleanupLevelDataRuntime() const
{
	if (!GetWorld()->IsGameWorld())
	{
		return;
	}

	LevelData->Exits.Empty();

	for (const auto Element : LevelData->Rooms)
	{
		Element->Destroy();
	}
	LevelData->Rooms.Empty();

	for (const auto Element : LevelData->Props)
	{
		Element->Destroy();
	}
	LevelData->Props.Empty();

	for (const auto Element : LevelData->Doors)
	{
		Element->Destroy();
	}
	LevelData->Doors.Empty();

	for (const auto Element : LevelData->Enemies)
	{
		Element->Destroy();
	}
	LevelData->Enemies.Empty();
}

#if WITH_EDITOR

void AMainGenerator::CleanupLevelDataEditor() const
{
	if (!GetWorld()->IsEditorWorld() && !bGeneratedFromEditor)
	{
		return;
	}

	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>(); 

	if (!EditorActorSubsystem)
	{
		return;
	}

	LevelData->Exits.Empty();

	for (const auto Element : LevelData->Rooms)
	{
		GEditor->SelectActor(Element, true, true, true);
		EditorActorSubsystem->DestroyActor(Element);
	}
	LevelData->Rooms.Empty();

	for (const auto Element : LevelData->Props)
	{
		GEditor->SelectActor(Element, true, true, true);
		EditorActorSubsystem->DestroyActor(Element);
	}
	LevelData->Props.Empty();

	for (const auto Element : LevelData->Doors)
	{
		GEditor->SelectActor(Element, true, true, true);
		EditorActorSubsystem->DestroyActor(Element);
	}
	LevelData->Doors.Empty();

	for (const auto Element : LevelData->Enemies)
	{
		GEditor->SelectActor(Element, true, true, true);
		EditorActorSubsystem->DestroyActor(Element);
	}

	LevelData->Enemies.Empty();
	GEngine->ForceGarbageCollection(true);
}

#endif
