// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackStartNotifyState.h"
#include "Destructible_demoCharacter.h"
#include "Engine.h"

void UAttackStartNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Cyan, __FUNCTION__);
	if (MeshComp && MeshComp->GetOwner()) {
		ADestructible_demoCharacter* main = Cast<ADestructible_demoCharacter>(MeshComp->GetOwner());
		if (main) {
			main->AttackStart();
		}
	}
}

void UAttackStartNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (MeshComp && MeshComp->GetOwner()) {
		ADestructible_demoCharacter* main = Cast<ADestructible_demoCharacter>(MeshComp->GetOwner());
		if (main) {
			if(main->GetCurrentAttackType()==EAttackType::MELEE_KICK)
				main->SetIsKeyboardEnabled(false);
		}
	}
}

void UAttackStartNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Cyan, __FUNCTION__);
	if (MeshComp && MeshComp->GetOwner()) {
		ADestructible_demoCharacter* main = Cast<ADestructible_demoCharacter>(MeshComp->GetOwner());
		if (main) {
			main->AttackEnd();
			main->SetIsKeyboardEnabled(true);
		}
	}
}
