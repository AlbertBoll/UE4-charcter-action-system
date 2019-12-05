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
}

void ADestructibleProp::Damage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, __FUNCTION__);
}

void ADestructibleProp::Trigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, __FUNCTION__);
}

void ADestructibleProp::Destroy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// Called when the game starts or when spawned
void ADestructibleProp::BeginPlay()
{
	Super::BeginPlay();
	DestructibleComponent->OnComponentHit.AddDynamic(this, &ADestructibleProp::Damage);
	
	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ADestructibleProp::Trigger);
	TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ADestructibleProp::Destroy);

	
}

// Called every frame
void ADestructibleProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

