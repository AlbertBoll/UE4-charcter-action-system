// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "ComboWidget.generated.h"

/**
 * 
 */
UCLASS()
class DESTRUCTIBLE_DEMO_API UComboWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UComboWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct()override;
	void UpdateComboCount(int32 value);
	void ResetCombo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* TXTCombo;

	void StoreWidgetAnimations();
	UWidgetAnimation* GetAnimationByName(FName AnimationName)const;

private:
	TMap<FName, UWidgetAnimation*> AnimationMap;
	UWidgetAnimation* ComboFadeAnimation;
	UWidgetAnimation* ComboShakeAnimation;
	
};
