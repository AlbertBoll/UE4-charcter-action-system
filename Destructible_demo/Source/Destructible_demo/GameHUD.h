// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/WidgetComponent.h"
#include "ComboWidget.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class DESTRUCTIBLE_DEMO_API AGameHUD : public AHUD
{
	GENERATED_BODY()
public:
	AGameHUD();
	virtual void DrawHUD()override;
	virtual void BeginPlay()override;
	virtual void Tick(float DeltaTime)override;

	UFUNCTION()
	void UpdateComboCount(int32 value);

	UFUNCTION()
	void ResetCombo();

	UPROPERTY(EditDefaultsOnly, Category=Widgets)
	TSubclassOf<UUserWidget>ComboWidgetClass;
private:
	UComboWidget* ComboWidget;

};
