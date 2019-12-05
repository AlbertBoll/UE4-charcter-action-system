// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructibleComponent.h"
#include "DestructibleProp.generated.h"

UCLASS()
class DESTRUCTIBLE_DEMO_API ADestructibleProp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleProp();

	void Damage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void Trigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void Destroy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Destructible")
	bool IsTriggerEnabled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Destructible")
	class UBoxComponent* TriggerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Destructible")
	class UDestructibleComponent* DestructibleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Destructible")
	bool IsDestroyed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Destructible")
	float MAXHealth; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Destructible")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Destructible")
	float DefaultDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Destructible")
	float DefaultImpluse;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};