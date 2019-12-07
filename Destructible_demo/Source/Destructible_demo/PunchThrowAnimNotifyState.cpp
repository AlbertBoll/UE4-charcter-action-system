// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine.h"
#include "Destructible_demoCharacter.h"
#include "PunchThrowAnimNotifyState.h"

void UPunchThrowAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Red, __FUNCTION__);
	if (MeshComp && MeshComp->GetOwner()) {
		ADestructible_demoCharacter* main = Cast<ADestructible_demoCharacter>(MeshComp->GetOwner());
		if (main && !main->PunchThrowAudioComponent->IsPlaying()) {
			main->PunchThrowAudioComponent->SetPitchMultiplier(FMath::RandRange(1.f, 1.3f));
			main->PunchThrowAudioComponent->Play(0.f);
		}
	}
}


void UPunchThrowAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Green, __FUNCTION__);
}

void UPunchThrowAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Blue, __FUNCTION__);
}
