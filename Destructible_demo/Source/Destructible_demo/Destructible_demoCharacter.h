// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/AudioComponent.h"
#include "Engine/DataTable.h"

#include "Destructible_demoCharacter.generated.h"

USTRUCT(BlueprintType)
struct FPlayerAttackMontage : public FTableRowBase
{
	GENERATED_BODY()
	/**Melee fist attack montage**/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* MeleeFistAttackMontage;

	/**amount of section in each anim montage**/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AnimSectionCount;

	/**Description of anim montage details**/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Description;

};


USTRUCT(BlueprintType)
struct FMeleeCollisionProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Enabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Disabled;

	//Default constructor
	FMeleeCollisionProfile() {
		Enabled = FName(TEXT("Weapon"));
		Disabled = FName(TEXT("NoCollision"));
	}

};

UENUM(BlueprintType)
enum class ELogLevel :uint8
{
	TRACE      UMETA(DisplayName = "Trace"),
	DEBUG	   UMETA(DisplayName = "Debug"),
	INFO	   UMETA(DisplayName = "Info"),
	WARNING    UMETA(DisplayName = "Warning"),
	ERROR      UMETA(DisplayName = "Error")
};

UENUM(BlueprintType)
enum class ELineTraceType :uint8
{
	CAMERA_SINGLE  UMETA(DisplayName = "Camera - Single Trace"),
	PLAYER_SINGLE  UMETA(DisplayName = "Player - Single Trace"),
	CAMERA_SPREAD  UMETA(DisplayName = "Camera - Spread Trace"),
	PLAYER_SPREAD  UMETA(DisplayName = "Player - Spread Trace")
};

UENUM(BlueprintType)
enum class EAttackStrength :uint8
{
	Light   UMETA(DisplayName = "Light Attack"),
	Medium  UMETA(DisplayName = "Medium Attack"),
	Heavy   UMETA(DisplayName = "Heavy Attack")
};

UENUM(BlueprintType)
enum class ELogOutput :uint8
{
	ALL				UMETA(DisplayName = "All levels"),
	OUTPUT_LOG	    UMETA(DisplayName = "Output log"),
	SCREEN          UMETA(DisplayName = "Screen")
};

UENUM(BlueprintType)
enum class EAttackType : uint8 {
	MELEE_FIST			UMETA(DisplayName = "Melee - Fist"),
	MELEE_KICK			UMETA(DisplayName = "Melee - Kick")
};

UCLASS(config=Game)
class ADestructible_demoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FistMeleeAttackMontage;

	/**Melee attack data table**/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UDataTable* PlayerAttackDataTable;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* LeftMeleeCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* RightMeleeCollisionBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* PunchSoundCue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* PunchThrowSoundCue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float AnimationBlendAmount;



public:
	ADestructible_demoCharacter();

	virtual void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	void PunchAttack();
	void KickAttack();

	void LightAttackStart();
	void LightAttackEnd();

	void HeavyAttackStart();
	void HeavyAttackEnd();

	UFUNCTION(BlueprintCallable, Category=Animation)
	FORCEINLINE bool GetIsAnimationBlended() { return IsAnimationBlended; }

	UFUNCTION(BlueprintCallable, Category = Animation)
	void SetIsKeyboardEnabled(bool Enabled);


	UFUNCTION(BlueprintCallable, Category = Animation)
	bool IsArmed();

	UFUNCTION(BlueprintCallable, Category = Animation)
	FORCEINLINE EAttackType GetCurrentAttackType() { return CurrentAttacktype; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LineTrace)
	ELineTraceType LineTraceType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LineTrace)
	float LineTraceDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LineTrace)
	float LineTraceSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LineTrace)
	int32 MaxCountdownToIdle;
	
	UFUNCTION()
	void ResetCombo();

	UFUNCTION()
	void CrouchStart();

	UFUNCTION()
	void CrouchEnd();

	UFUNCTION()
	void ArmPlayer();

	UFUNCTION()
	void TriggerCountdownToIdle();

	void FireLineTrace();

	void AttackInput(EAttackType AttackType);

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnAttackHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	//UFUNCTION()
	//void OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//UFUNCTION()
	//void OnAttackEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	
	/**
* Initiates player attack
*/
	void AttackStart();

	/**
	* Stops player attack
	*/
	void AttackEnd();

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface



public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//UAudioComponent* PunchThrowAudioComponent;

private:
	/**
	* Log - prints a message to all the log outputs with a specific color
	* @param LogLevel {@see ELogLevel} affects color of log
	* @param FString the message for display
	*/
	//friend class PunchThrowAnimNotify;

	
	FPlayerAttackMontage* AttackMontage;
	//FPlayerAttackMontage* KickAttackMontage;

	UAudioComponent* PunchAudioComponent;
	UAudioComponent* PunchThrowAudioComponent;

	//boolian type
	bool IsLightAttack;
	bool IsHeavyAttack;
	bool IsAnimationBlended;
	bool IsKeyboardEnabled;
	bool bIsArmed;


	//int type
	int32 CurrentComboCount;
	int32 CountdownToIdle;

	//FTimerHandle type
	FTimerHandle ComboResetHandle;
	FTimerHandle ArmedToIdleTimerHandle;

	//Enum type
	EAttackType CurrentAttacktype;
	EAttackStrength CurrentAttackStrength;

	//struct type
	FMeleeCollisionProfile MeleeCollisionProfile;

	//friend class
	friend class UPunchThrowAnimNotify;
	friend class UPunchThrowAnimNotifyState;


	void Log(ELogLevel LogLevel, FString Message);

	/**
	* Log - prints a message to all the log outputs with a specific color
	* @param LogLevel {@see ELogLevel} affects color of log
	* @param FString the message for display
	* @param ELogOutput - All, Output Log or Screen
	*/

	void Log(ELogLevel LogLevel, FString Message, ELogOutput LogOutput);
};

