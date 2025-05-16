// Copyright Epic Games, Inc. All Rights Reserved.

#include "LevelGeneration.h"

#define LOCTEXT_NAMESPACE "FLevelGenerationModule"

void FLevelGenerationModule::StartupModule()
{
#if WITH_EDITOR
	FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	propertyModule.RegisterCustomClassLayout("PropSpawner", FOnGetDetailCustomizationInstance::CreateStatic(&PropSpawnerCustomDetails::MakeInstance));
	propertyModule.RegisterCustomClassLayout("PropSpawnVolume", FOnGetDetailCustomizationInstance::CreateStatic(&PropSpawnVolumeCustomDetails::MakeInstance));
#endif
}

void FLevelGenerationModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLevelGenerationModule, LevelGeneration)