// Fill out your copyright notice in the Description page of Project Settings.


#include "PropSpawnVolumeCustomDetails.h"


#if WITH_EDITOR
#include "DetailCategoryBuilder.h"
#include "PropSpawnVolume.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#define LOCTEXT_NAMESPACE "PropSpawnVolumeDetails"

TSharedRef<IDetailCustomization> PropSpawnVolumeCustomDetails::MakeInstance()
{
	return MakeShareable(new PropSpawnVolumeCustomDetails);
}

void PropSpawnVolumeCustomDetails::ForceRefresh() const
{
	if (CachedBuilder)
	{
		CachedBuilder->ForceRefreshDetails();
	}
}

void PropSpawnVolumeCustomDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	CachedBuilder = &DetailBuilder;
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("PropSpawnVolumeDetails", FText::GetEmpty(), ECategoryPriority::Important);
	APropSpawnVolume* PropSpawner = nullptr;
	TArray<TWeakObjectPtr<UObject>> customizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(customizedObjects);

	for (auto Object : customizedObjects)
	{
		if (!Object.IsValid())continue;
		PropSpawner = Cast<APropSpawnVolume>(Object);
		if (PropSpawner != nullptr)break;
	}

	TSharedRef<IPropertyHandle> ArrayPropertyHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(APropSpawnVolume,PropsToSpawn ));
	if (ArrayPropertyHandle->AsArray().IsValid())
	{
		uint32 NumElements = 0;
		ArrayPropertyHandle->AsArray()->GetNumElements(NumElements);

		for (uint32 i = 0; i < NumElements; ++i)
		{
			TSharedRef<IPropertyHandle> ElementHandle = ArrayPropertyHandle->AsArray()->GetElement(i);
			ElementHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &PropSpawnVolumeCustomDetails::ForceRefresh));
		}

		// Also refresh when array size changes
		ArrayPropertyHandle->AsArray()->SetOnNumElementsChanged(FSimpleDelegate::CreateSP(this, &PropSpawnVolumeCustomDetails::ForceRefresh));
	}

	check(PropSpawner);
	auto Props = PropSpawner->GetProps();

	for (size_t i = 0; i < Props.Num(); i++)
	{
		FText PropName = FText::FromString(Props[i].GetName());
		FText IndexToString = FText::FromString(FString::FromInt(i));

		Category.AddCustomRow(FText(PropName), false).NameContent()
			[
				SNew(STextBlock).Text(PropName).
					Font(IDetailLayoutBuilder::GetDetailFont())
			].
			ValueContent()
					[
						SNew(SButton).Text(FText::FromString("Preview")).HAlign(HAlign_Center).OnClicked_Lambda([i, PropSpawner]()
							{
								PropSpawner->OnPreviewButtonPressed(i);
								return FReply::Handled();
							})
					];
	}
}
#endif
#undef LOCTEXT_NAMESPACE
