// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"

AGameHUD::AGameHUD() 
{

}

void AGameHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();
	if (ComboWidgetClass) 
	{
		ComboWidget = CreateWidget<UComboWidget>(GetWorld(), ComboWidgetClass);
		if (ComboWidget) {
			ComboWidget->AddToViewport();
		}
	}
}

void AGameHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameHUD::UpdateComboCount(int32 value)
{
	if (ComboWidget)
	{
		ComboWidget->UpdateComboCount(value);

	}
}

void AGameHUD::ResetCombo()
{
	if (ComboWidget) {
		ComboWidget->ResetCombo();
	}
}
