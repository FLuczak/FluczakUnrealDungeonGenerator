// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR
#include "UObject/NoExportTypes.h"
#include "IDetailCustomization.h"

/**
 * 
 */
class LEVELGENERATION_API PropSpawnVolumeCustomDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	void ForceRefresh() const;
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
private:
	IDetailLayoutBuilder* CachedBuilder = nullptr;
};
#endif