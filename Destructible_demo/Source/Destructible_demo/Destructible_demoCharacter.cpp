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

	static ConstructorHelpers::FObjectFinder<UDataTable>PlayerAttackMontageDataTable(TEXT("DataTable'/Game/DataTable/PlayerAttckMontageDataTable.PlayerAttckMontageDataTable'"));
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



	LeftFistCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftFistCollisionBox"));
	RightFistCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightFistCollisionBox"));
	LeftFistCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Fist_L_Collision"));
	RightFistCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Fist_R_Collision"));
	
	//LeftFistCollisionBox->SetCollisionProfileName("NoCollision");
	LeftFistCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
	//RightFistCollisionBox->SetCollisionProfileName("NoCollision");
	RightFistCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);

	LeftFistCollisionBox->SetNotifyRigidBodyCollision(false);
	RightFistCollisionBox->SetNotifyRigidBodyCollision(false);
	
}

void ADestructible_demoCharacter::BeginPlay()
{
	Super::BeginPlay();
	LeftFistCollisionBox->OnComponentHit.AddDynamic(this, &ADestructible_demoCharacter::OnAttackHit);
	RightFistCollisionBox->OnComponentHit.AddDynamic(this, &ADestructible_demoCharacter::OnAttackHit);
	//LeftFistCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ADestructible_demoCharacter::OnAttackBeginOverlap);
	//LeftFistCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ADestructible_demoCharacter::OnAttackEndOverlap);
	//RightFistCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ADestructible_demoCharacter::OnAttackBeginOverlap);
	//RightFistCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ADestructible_demoCharacter::OnAttackEndOverlap);
	if (PunchAudioComponent&& PunchSoundCue)
	{
		PunchAudioComponent->SetSound(PunchSoundCue);
	}

	if (PlayerAttackDataTable)
	{
		FPlayerAttackMontage AttackMontage;
		AttackMontage.MeleeFistAttackMontage = NULL;
		AttackMontage.AnimSectionCount = 10;
		AttackMontage.Description = "Create from begin play";
		PlayerAttackDataTable->AddRow(FName(TEXT("New Row")), AttackMontage);
	}
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

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ADestructible_demoCharacter::AttackInput);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ADestructible_demoCharacter::AttackEnd);

}

void ADestructible_demoCharacter::AttackInput()
{
	Log(ELogLevel::INFO, __FUNCTION__);
	int MontageSectionIndex = rand() % 3 + 1;
	FString MontageSection = "Start_" + FString::FromInt(MontageSectionIndex);
	PlayAnimMontage(FistMeleeAttackMontage, 1.2f, FName(*MontageSection));

	if (PlayerAttackDataTable) {
		static const FString contextString(TEXT("Player attack montage context"));
		FPlayerAttackMontage* AttackMontage = PlayerAttackDataTable->FindRow<FPlayerAttackMontage>(FName(TEXT("Punch1")), contextString, true);
		if (AttackMontage) {
			int MontageSectionIndex = rand() % (AttackMontage->AnimSectionCount) + 1;
			FString MontageSection = "Start_" + FString::FromInt(MontageSectionIndex);
			PlayAnimMontage(AttackMontage->MeleeFistAttackMontage, 1.2f, FName(*MontageSection));
		}
	}
}

void ADestructible_demoCharacter::OnAttackHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Log(ELogLevel::WARNING, Hit.GetActor()->GetName());
	if (PunchAudioComponent && !PunchAudioComponent->IsPlaying())
	{
		PunchAudioComponent->SetPitchMultiplier(FMath::RandRange(1.f, 1.3f));
		PunchAudioComponent->Play(0.f);
	}

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
	Log(ELogLevel::INFO, __FUNCTION__);
	LeftFistCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Enabled);
	//LeftFistCollisionBox->SetCollisionProfileName("Weapon");
	LeftFistCollisionBox->SetNotifyRigidBodyCollision(true);
	//LeftFistCollisionBox->SetGenerateOverlapEvents(true);

	RightFistCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Enabled);
	//RightFistCollisionBox->SetCollisionProfileName("Weapon");
	RightFistCollisionBox->SetNotifyRigidBodyCollision(true);
	//RightFistCollisionBox->SetGenerateOverlapEvents(true);

}

void ADestructible_demoCharacter::AttackEnd()
{
	Log(ELogLevel::INFO, __FUNCTION__);
	LeftFistCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
	//LeftFistCollisionBox->SetCollisionProfileName("NoCollision");
	LeftFistCollisionBox->SetNotifyRigidBodyCollision(false);
	//LeftFistCollisionBox->SetGenerateOverlapEvents(false);
	RightFistCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
	//RightFistCollisionBox->SetCollisionProfileName("NoCollision");
	RightFistCollisionBox->SetNotifyRigidBodyCollision(false);
	//RightFistCollisionBox->SetGenerateOverlapEvents(false);

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
	if ((Controller != NULL) && (Value != 0.0f))
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
	if ( (Controller != NULL) && (Value != 0.0f) )
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
