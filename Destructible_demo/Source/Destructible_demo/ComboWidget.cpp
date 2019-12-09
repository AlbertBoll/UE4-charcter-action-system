// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboWidget.h"

#include "Engine.h"

UComboWidget::UComboWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {}

void UComboWidget::NativeConstruct()
{
	Super::NativeConstruct();
	StoreWidgetAnimations();
	ComboFadeAnimation = GetAnimationByName(TEXT("ComboFade"));
	ComboShakeAnimation = GetAnimationByName(TEXT("ComboShake"));

}

void UComboWidget::UpdateComboCount(int32 value)
{
	if (TXTCombo && value > 1) {
		//turn textbox visible if it already hidden
		if(TXTCombo->Visibility==ESlateVisibility::Hidden)
			TXTCombo->SetVisibility(ESlateVisibility::Visible);
		//Generate Output combo
		TXTCombo->SetText(FText::FromString(FString::FromInt(value)+"x Combo"));
		if (ComboFadeAnimation)
		{
			PlayAnimation(ComboFadeAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
		}
		if (ComboShakeAnimation)
		{
			PlayAnimation(ComboShakeAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
		}
	}
}

void UComboWidget::ResetCombo()
{
	if (TXTCombo)
	{
		TXTCombo->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UComboWidget::StoreWidgetAnimations()
{
	AnimationMap.Empty();
	UProperty* Prop = GetClass()->PropertyLink;
	while (Prop)
	{
		if (Prop->GetClass() == UObjectProperty::StaticClass()) {
			UObjectProperty* ObjProp = Cast<UObjectProperty>(Prop);
			//Only get widget animation
			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass()) {
				UObject* obj = ObjProp->GetObjectPropertyValue_InContainer(this);
				UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(obj);
				if (WidgetAnimation && WidgetAnimation->MovieScene) {
					FName AniName = WidgetAnimation->MovieScene->GetFName();
					GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Orange, AniName.ToString());
					AnimationMap.Add(AniName, WidgetAnimation);
				}
			}
		}
		Prop = Prop->PropertyLinkNext;
	}
}

UWidgetAnimation* UComboWidget::GetAnimationByName(FName AnimationName)const
{
	UWidgetAnimation* const* WidgetAnimation = AnimationMap.Find(AnimationName);
	if (WidgetAnimation)
	{
		return *WidgetAnimation;
	}
	return nullptr;
}
