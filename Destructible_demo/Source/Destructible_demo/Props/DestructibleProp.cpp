// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleProp.h"
#include "Engine.h"

// Sets default values
ADestructibleProp::ADestructibleProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Destructible Component"));
	DestructibleComponent->SetupAttachment(RootComponent);
	DestructibleComponent->SetNotifyRigidBodyCollision(true);
	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Component"));
	TriggerComponent->SetupAttachment(RootComponent);
	IsDestroyed = false;
	IsTriggerEnabled = false;
	MAXHealth = 10.f;
	DefaultDamage = 1.f;
	DefaultImpluse = 1.f;
	CurrentHealth = MAXHealth;
	TriggerCountdown = 10;
}

void ADestructibleProp::Damage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	if (!IsDestroyed && OtherComp->ComponentHasTag("Weapon")) {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange,OtherComp->GetName());
		if (CurrentHealth - 1.f > 0) {
			CurrentHealth -= 1.f;
		}
		else
			Destroy(DefaultDamage, Hit.Location, NormalImpulse, DefaultImpluse);
	}
}

void ADestructibleProp::Trigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (IsTriggerEnabled && !IsDestroyed && OtherActor->ActorHasTag("Player")) {
		
		//Destroy(DefaultDamage, DestructibleComponent->GetComponentLocation(), OtherActor->GetActorForwardVector(), DefaultImpluse);
		if (GetWorld()->GetTimerManager().IsTimerActive(TriggerDestroyTimerHandle))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, "Pausing Timer");
			GetWorld()->GetTimerManager().ClearTimer(TriggerDestroyTimerHandle);
		}

		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, "Starting Timer");
			GetWorld()->GetTimerManager().SetTimer(TriggerDestroyTimerHandle, this, &ADestructibleProp::TriggerCountdownDestory, 1.f, true);
		}
	}
}

void ADestructibleProp::TriggerDestroy()
{
	if((!IsDestroyed) && IsTriggerEnabled)
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, "about to explosion");
		Destroy(DefaultDamage, DestructibleComponent->GetComponentLocation(), DestructibleComponent->GetUpVector(), DefaultImpluse);
		
}

void ADestructibleProp::TriggerCountdownDestory()
{
	if (--TriggerCountdown <= 0) {
		int32 RandomDelay = FMath::RandRange(1, 4);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, "Trigger explosion after "+FString::FromInt(RandomDelay)+" seconds");
		GetWorld()->GetTimerManager().SetTimer(TriggerDestroyTimerHandle, this, &ADestructibleProp::TriggerDestroy, RandomDelay);
		//TriggerDestroy();
	}

	else
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, "Counting down " + FString::FromInt(TriggerCountdown));
}

void ADestructibleProp::Destroy(float damage, FVector HitLocation, FVector ImpulseDirection, float Impulse)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, __FUNCTION__);
	if (!IsDestroyed) {
		IsDestroyed = true;
		DestructibleComponent->ApplyDamage(damage, HitLocation, ImpulseDirection, Impulse);
	}
}

// Called when the game starts or when spawned
void ADestructibleProp::BeginPlay()
{
	Super::BeginPlay();
	DestructibleComponent->OnComponentHit.AddDynamic(this, &ADestructibleProp::Damage);
	
	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ADestructibleProp::Trigger);
	//TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ADestructibleProp::Destroy);

	
}

// Called every frame
void ADestructibleProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

