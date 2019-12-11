// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../InteractiveActor.h"
#include "InteractiveProp.generated.h"

UCLASS()
class DESTRUCTIBLE_DEMO_API AInteractiveProp : public AActor, public IInteractiveActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractiveProp();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interactive")
	class UStaticMeshComponent* BaseMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bCanScale;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Interactive)
	void Interact(); //prototype declaration
	virtual void Interact_Implementation() override; // actual implementation of interact method

private:
	bool bIsBig;
};
