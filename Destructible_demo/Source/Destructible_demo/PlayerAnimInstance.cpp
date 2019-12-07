// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Destructible_demoCharacter.h"
#include "Engine.h"


UPlayerAnimInstance::UPlayerAnimInstance()
{
	IsInAir = false;
	IsAnimationBlended = true;
	speed = 0.f;
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = TryGetPawnOwner();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!Owner) {
		return;
	}

	if (Owner->IsA(ADestructible_demoCharacter::StaticClass())) {
		ADestructible_demoCharacter* Player = Cast<ADestructible_demoCharacter>(Owner);
		if (Player) {
			IsInAir = Player->GetMovementComponent()->IsFalling();
			IsAnimationBlended = Player->GetIsAnimationBlended();
			speed = Player->GetVelocity().Size();

			/**Debug Purpose**/
			/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "IsInAir " + FString(IsInAir ? "true" : "false"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "IsAnimationBlended " + FString(IsAnimationBlended? "true" : "false"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Speed " + FString::SanitizeFloat(speed));*/

		}
	}
	
}
