// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Destructible_demoCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sound/SoundCue.h"
#include "Engine.h"
#include "GameHUD.h"

//////////////////////////////////////////////////////////////////////////
// ADestructible_demoCharacter

ADestructible_demoCharacter::ADestructible_demoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	static ConstructorHelpers::FObjectFinder<UAnimMontage>MeleeFistAttackMontageObject(TEXT("AnimMontage'/Game/Mannequin/Animation2/Punch/MeleeFistAttackMontage.MeleeFistAttackMontage'"));
	if (MeleeFistAttackMontageObject.Succeeded())
	{
		FistMeleeAttackMontage = MeleeFistAttackMontageObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable>PlayerAttackMontageDataTable(TEXT("DataTable'/Game/DataTable/playerAttackMontageDataTable.playerAttackMontageDataTable'"));
	if (PlayerAttackMontageDataTable.Succeeded())
	{
		PlayerAttackDataTable = PlayerAttackMontageDataTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue>PunchSoundCueObject(TEXT("SoundCue'/Game/SFX/PunchSoundCue.PunchSoundCue'"));
	if (PunchSoundCueObject.Succeeded())
	{
		PunchSoundCue = PunchSoundCueObject.Object;
		PunchAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PunchAudioComponent"));
		PunchAudioComponent->SetupAttachment(RootComponent);
		//PunchAudioComponent->SetSound(PunchSoundCue);
	}

	static ConstructorHelpers::FObjectFinder<USoundCue>PunchThrowSoundCueObject(TEXT("SoundCue'/Game/SFX/PunchThrowSoundCue.PunchThrowSoundCue'"));
	if (PunchThrowSoundCueObject.Succeeded())
	{
		PunchThrowSoundCue = PunchThrowSoundCueObject.Object;
		PunchThrowAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PunchThrowAudioComponent"));
		PunchThrowAudioComponent->SetupAttachment(RootComponent);
	}

	LeftMeleeCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftMeleeCollisionBox"));
	RightMeleeCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightMeleeCollisionBox"));

	/**Attach box to fist**/
	//LeftMeleeCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Fist_L_Collision"));
	//RightMeleeCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Fist_R_Collision"));

	/**Attach box to feet**/

	//LeftMeleeCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Foot_L_Collision"));
	//RightMeleeCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Foot_R_Collision"));

	//LeftMeleeCollisionBox->SetCollisionProfileName("NoCollision");
	LeftMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
	//RightMeleeCollisionBox->SetCollisionProfileName("NoCollision");
	RightMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);

	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(false);
	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(false);
	LineTraceType = ELineTraceType::CAMERA_SINGLE;
	LineTraceDistance = 100.f;
	LineTraceSpread = 10.f;
	CurrentComboCount = 0;
}

void ADestructible_demoCharacter::BeginPlay()
{
	Super::BeginPlay();
	LeftMeleeCollisionBox->OnComponentHit.AddDynamic(this, &ADestructible_demoCharacter::OnAttackHit);
	RightMeleeCollisionBox->OnComponentHit.AddDynamic(this, &ADestructible_demoCharacter::OnAttackHit);
	//LeftMeleeCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ADestructible_demoCharacter::OnAttackBeginOverlap);
	//LeftMeleeCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ADestructible_demoCharacter::OnAttackEndOverlap);
	//RightMeleeCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ADestructible_demoCharacter::OnAttackBeginOverlap);
	//RightMeleeCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ADestructible_demoCharacter::OnAttackEndOverlap);
	if (PunchAudioComponent&& PunchSoundCue)
	{
		PunchAudioComponent->SetSound(PunchSoundCue);
	}

	if (PunchThrowAudioComponent && PunchThrowSoundCue)
	{
		PunchThrowAudioComponent->SetSound(PunchThrowSoundCue);
	}


	/*if (PlayerAttackDataTable)
	{
		FPlayerAttackMontage AttackMontage;
		AttackMontage.MeleeFistAttackMontage = NULL;
		AttackMontage.AnimSectionCount = 10;
		AttackMontage.Description = "Create from begin play";
		PlayerAttackDataTable->AddRow(FName(TEXT("New Row")), AttackMontage);
	}*/


}

//////////////////////////////////////////////////////////////////////////
// Input

void ADestructible_demoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADestructible_demoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADestructible_demoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADestructible_demoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADestructible_demoCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ADestructible_demoCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ADestructible_demoCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ADestructible_demoCharacter::OnResetVR);

	PlayerInputComponent->BindAction("Punch", IE_Pressed, this, &ADestructible_demoCharacter::PunchAttack);
	//PlayerInputComponent->BindAction("Attack", IE_Released, this, &ADestructible_demoCharacter::AttackEnd);
	PlayerInputComponent->BindAction("Kick", IE_Pressed, this, &ADestructible_demoCharacter::KickAttack);
	PlayerInputComponent->BindAction("FireLineTrace", IE_Pressed, this, &ADestructible_demoCharacter::FireLineTrace);
	PlayerInputComponent->BindAction("LightAttackModifier", IE_Pressed, this, &ADestructible_demoCharacter::LightAttackStart);
	PlayerInputComponent->BindAction("LightAttackModifier", IE_Released, this, &ADestructible_demoCharacter::LightAttackEnd);

	PlayerInputComponent->BindAction("HeavyAttackModifier", IE_Pressed, this, &ADestructible_demoCharacter::HeavyAttackStart);
	PlayerInputComponent->BindAction("HeavyAttackModifier", IE_Released, this, &ADestructible_demoCharacter::HeavyAttackEnd);

}

void ADestructible_demoCharacter::SetIsKeyboardEnabled(bool Enabled)
{
	IsKeyboardEnabled = Enabled;
}

void ADestructible_demoCharacter::ResetCombo()
{
	CurrentComboCount = 0;
	AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (GameHUD) {
		GameHUD->ResetCombo();
	}
}

void ADestructible_demoCharacter::FireLineTrace()
{
	//Log(ELogLevel::INFO, __FUNCTION__);

	FVector Start, End;

	const float SpreadAngle = FMath::DegreesToRadians(0.5f * LineTraceSpread);
	if (LineTraceType == ELineTraceType::CAMERA_SINGLE || LineTraceType==ELineTraceType::CAMERA_SPREAD)
	{
		FVector Camera_Loc = FollowCamera->GetComponentLocation();
		FRotator Camera_Rot = FollowCamera->GetComponentRotation();
		Start = Camera_Loc;
		if (LineTraceType == ELineTraceType::CAMERA_SPREAD)
		{
			End = Start + FMath::VRandCone(Camera_Rot.Vector(), SpreadAngle, SpreadAngle)*LineTraceDistance;
		}
		else
			End = Start + Camera_Rot.Vector() * LineTraceDistance;
	}

	else if (LineTraceType == ELineTraceType::PLAYER_SINGLE || LineTraceType == ELineTraceType::PLAYER_SPREAD)
	{
		FVector PlayerEyes_Loc; 
		FRotator PlayerEyes_Rot;
		GetActorEyesViewPoint(PlayerEyes_Loc, PlayerEyes_Rot);
		Start = PlayerEyes_Loc;
		if (LineTraceType == ELineTraceType::PLAYER_SPREAD)
		{
			End = Start + FMath::VRandCone(PlayerEyes_Rot.Vector(), SpreadAngle, SpreadAngle)*LineTraceDistance;
		}
		else
			End = Start + PlayerEyes_Rot.Vector() * LineTraceDistance;
	}

	FHitResult HitResult = FHitResult(ForceInit);
	FCollisionQueryParams TraceParams(FName(TEXT("LineTraceParameters")), true, nullptr);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = true;

	bool bLineHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel3, TraceParams);
	if (bLineHit)
	{
		Log(ELogLevel::INFO, "We hit something");
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f, ECC_WorldStatic, 1.f);
		Log(ELogLevel::INFO, HitResult.Actor->GetName());
		Log(ELogLevel::INFO, FString::SanitizeFloat(HitResult.Distance));
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 4.f, 16, FColor::Blue, false, 4.f, ECC_WorldStatic, 1.f);

	}

	else
	{
		Log(ELogLevel::INFO, "We hit nothing");
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.f, ECC_WorldStatic, 1.f);
	}
}

void ADestructible_demoCharacter::AttackInput(EAttackType AttackType)
{
	//Log(ELogLevel::WARNING, __FUNCTION__);
	CurrentAttacktype = AttackType;
	//int MontageSectionIndex = rand() % 3 + 1;
	//FString MontageSection = "Start_" + FString::FromInt(MontageSectionIndex);
	//PlayAnimMontage(FistMeleeAttackMontage, 1.2f, FName(*MontageSection));

	CurrentAttackStrength = EAttackStrength::Medium;
	if (IsHeavyAttack)
		CurrentAttackStrength = EAttackStrength::Heavy;
	else if (IsLightAttack&&!IsHeavyAttack)
		CurrentAttackStrength = EAttackStrength::Light;

	if (PlayerAttackDataTable) {
		static const FString contextString(TEXT("Player attack montage context"));
		FName AttackRowKey;

		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
		switch (AttackType)
		{
		case EAttackType::MELEE_FIST:
			AttackRowKey = FName(TEXT("Punch"));
			LeftMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, FName("Fist_L_Collision"));
			RightMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, FName("Fist_R_Collision"));
			IsKeyboardEnabled = true;
			IsAnimationBlended = true;
			break;

		case EAttackType::MELEE_KICK:
			AttackRowKey = FName(TEXT("Kick"));
			LeftMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, FName("Foot_L_Collision"));
			RightMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, FName("Foot_R_Collision"));
			IsKeyboardEnabled = false;
			IsAnimationBlended = false;
			break;

		default:
			//IsKeyboardEnabled = true;
			IsAnimationBlended = true;
			break;
		}
		AttackMontage = PlayerAttackDataTable->FindRow<FPlayerAttackMontage>(AttackRowKey, contextString, true);
		if (AttackMontage) {
			//Pick the corrected attack type based on the attack strength
			//int MontageSectionIndex = rand() % (AttackMontage->AnimSectionCount) + 1; // change code
			int MontageSectionIndex;

			switch (CurrentAttackStrength)
			{
			case EAttackStrength::Light:
				MontageSectionIndex = 1;
				break;

			case EAttackStrength::Medium:
				MontageSectionIndex = 2;
				break;

			case EAttackStrength::Heavy:
				MontageSectionIndex = 3;
				break;

			default:
				MontageSectionIndex = 2;
				break;
			
			}

			FString MontageSection = "Start_" + FString::FromInt(MontageSectionIndex);
			PlayAnimMontage(AttackMontage->MeleeFistAttackMontage, 1.0f, FName(*MontageSection));
		}
	}
}

void ADestructible_demoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//FireLineTrace();
}

void ADestructible_demoCharacter::PunchAttack()
{
	//Log(ELogLevel::INFO, __FUNCTION__);
	AttackInput(EAttackType::MELEE_FIST);
	
}

void ADestructible_demoCharacter::KickAttack()
{
	//Log(ELogLevel::INFO, __FUNCTION__);
	AttackInput(EAttackType::MELEE_KICK);
}

void ADestructible_demoCharacter::LightAttackStart()
{
	IsLightAttack = true;


}

void ADestructible_demoCharacter::LightAttackEnd()
{
	IsLightAttack = false;
}

void ADestructible_demoCharacter::HeavyAttackStart()
{
	IsHeavyAttack = true;
}

void ADestructible_demoCharacter::HeavyAttackEnd()
{
	IsHeavyAttack = false;
}

void ADestructible_demoCharacter::OnAttackHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Log(ELogLevel::WARNING, Hit.GetActor()->GetName()); //Debug Purpose
	if (PunchAudioComponent && !PunchAudioComponent->IsPlaying())
	{
		if (!PunchAudioComponent->IsActive())
			PunchAudioComponent->Activate(true);
		PunchAudioComponent->SetPitchMultiplier(FMath::RandRange(1.f, 1.3f));
		PunchAudioComponent->Play(0.f);
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		/**Stop provides some sort of solution of mesh overlaping, but not satisfied**/
		//AnimInstance->Montage_Stop(.5f, AttackMontage->MeleeFistAttackMontage);
		//AnimInstance->Montage_Pause(AttackMontage->MeleeFistAttackMontage);
		AnimInstance->Montage_Play(AttackMontage->MeleeFistAttackMontage, AnimationBlendAmount, EMontagePlayReturnType::Duration, AnimInstance->Montage_GetPosition(AttackMontage->MeleeFistAttackMontage), true);
	}
	AGameHUD* GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (GameHUD) {
		CurrentComboCount += 1;
		GameHUD->UpdateComboCount(CurrentComboCount);
	}
	if (!GetWorld()->GetTimerManager().IsTimerActive(ComboResetHandle)) {
		GetWorld()->GetTimerManager().SetTimer(ComboResetHandle, this, &ADestructible_demoCharacter::ResetCombo, 4.f, false);
	}

	else
		GetWorld()->GetTimerManager().ClearTimer(ComboResetHandle);
		GetWorld()->GetTimerManager().SetTimer(ComboResetHandle, this, &ADestructible_demoCharacter::ResetCombo, 4.f, false);

}

//void ADestructible_demoCharacter::OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	Log(ELogLevel::WARNING, OtherActor->GetName());
//}

//void ADestructible_demoCharacter::OnAttackEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	Log(ELogLevel::WARNING, OtherActor->GetName());
//}

void ADestructible_demoCharacter::AttackStart()
{
	//Log(ELogLevel::INFO, __FUNCTION__);
	LeftMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Enabled);
	//LeftMeleeCollisionBox->SetCollisionProfileName("Weapon");
	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(true);
	//LeftMeleeCollisionBox->SetGenerateOverlapEvents(true);

	RightMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Enabled);
	//RightMeleeCollisionBox->SetCollisionProfileName("Weapon");
	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(true);
	//RightMeleeCollisionBox->SetGenerateOverlapEvents(true);

}

void ADestructible_demoCharacter::AttackEnd()
{
	//Log(ELogLevel::INFO, __FUNCTION__);
	LeftMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
	//LeftMeleeCollisionBox->SetCollisionProfileName("NoCollision");
	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(false);
	//LeftMeleeCollisionBox->SetGenerateOverlapEvents(false);
	RightMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
	//RightMeleeCollisionBox->SetCollisionProfileName("NoCollision");
	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(false);
	//RightMeleeCollisionBox->SetGenerateOverlapEvents(false);

	//PlayAnimMontage(FistMeleeAttackMontage, 1.f, FName("End_1"));
}

void ADestructible_demoCharacter::Log(ELogLevel LogLevel, FString Message)
{
	Log(LogLevel, Message, ELogOutput::ALL);
}

void ADestructible_demoCharacter::Log(ELogLevel LogLevel, FString Message, ELogOutput LogOutput)
{
	// only print when screen is selected and the GEngine object is available
	if ((LogOutput == ELogOutput::ALL || LogOutput == ELogOutput::SCREEN) && GEngine)
	{
		// default color
		FColor LogColor = FColor::Cyan;
		// flip the color based on the type
		switch (LogLevel)
		{
		case ELogLevel::TRACE:
			LogColor = FColor::Green;
			break;
		case ELogLevel::DEBUG:
			LogColor = FColor::Cyan;
			break;
		case ELogLevel::INFO:
			LogColor = FColor::White;
			break;
		case ELogLevel::WARNING:
			LogColor = FColor::Yellow;
			break;
		case ELogLevel::ERROR:
			LogColor = FColor::Red;
			break;
		default:
			break;
		}
		// print the message and leave it on screen ( 4.5f controls the duration )
		GEngine->AddOnScreenDebugMessage(-1, 4.5f, LogColor, Message);
	}

	if (LogOutput == ELogOutput::ALL || LogOutput == ELogOutput::OUTPUT_LOG)
	{
		// flip the message type based on error level
		switch (LogLevel)
		{
		case ELogLevel::TRACE:
			UE_LOG(LogTemp, VeryVerbose, TEXT("%s"), *Message);
			break;
		case ELogLevel::DEBUG:
			UE_LOG(LogTemp, Verbose, TEXT("%s"), *Message);
			break;
		case ELogLevel::INFO:
			UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
			break;
		case ELogLevel::WARNING:
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
			break;
		case ELogLevel::ERROR:
			UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
			break;
		default:
			UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
			break;
		}
	}
}


void ADestructible_demoCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ADestructible_demoCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ADestructible_demoCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ADestructible_demoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADestructible_demoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADestructible_demoCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && IsKeyboardEnabled)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ADestructible_demoCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) && IsKeyboardEnabled)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
