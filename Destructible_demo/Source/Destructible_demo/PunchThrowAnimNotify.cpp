// Fill out your copyright notice in the Description page of Project Settings.


#include "PunchThrowAnimNotify.h"
#include "Destructible_demoCharacter.h"
#include "Engine.h"

void UPunchThrowAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Orange, __FUNCTION__);
	/*if (MeshComp && MeshComp->GetOwner()) {
		ADestructible_demoCharacter* main = Cast<ADestructible_demoCharacter>(MeshComp->GetOwner());
		if (main&& !main->PunchThrowAudioComponent->IsPlaying()) {
			main->PunchThrowAudioComponent->Play(0.f);
		}
	}*/
}
