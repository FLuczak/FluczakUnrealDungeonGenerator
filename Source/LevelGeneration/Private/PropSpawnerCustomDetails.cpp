


#include "PropSpawnerCustomDetails.h"

#include "PropSpawnVolume.h"


#if WITH_EDITOR

#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "PropSpawner.h"

#define LOCTEXT_NAMESPACE "PropSpawnerDetails"
TSharedRef<IDetailCustomization> PropSpawnerCustomDetails::MakeInstance()
{
	return MakeShareable(new PropSpawnerCustomDetails);
}


void PropSpawnerCustomDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("PropDebug", FText::GetEmpty(), ECategoryPriority::Important);
	UPropSpawner* PropSpawner = nullptr;
	TArray<TWeakObjectPtr<UObject>> customizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(customizedObjects);

	for (auto Object : customizedObjects)
	{
		if (!Object.IsValid())continue;
		PropSpawner = Cast<UPropSpawner>(Object);
		if (PropSpawner != nullptr)break;
	}

	check(PropSpawner);
	if (PropSpawner->GetOwner() == nullptr)return;
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
