// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleProp.h"
#include "EngineUtils.h"
#include "Engine.h"

AToggleProp::AToggleProp()
{
	bCanScale = false;
}

void AToggleProp::Interact_Implementation()
{
	for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator) {
		AActor* Actor = *ActorIterator;
		//Ensure the Actor is not null, Actor we cast is not ourself, and it implements the specific interface
		if (Actor && Actor != this && Actor->GetClass()->ImplementsInterface(UInteractiveActor::StaticClass())) {
			//for (FName Tag : Actor->Tags) {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Tag.ToString());
				//if (Actor->ActorHasTag(ToggleTags))
					//IInteractiveActor::Execute_Interact(Actor);
				for (FName TagsToCheck : ToggleTags) {
					if (Actor->ActorHasTag(TagsToCheck)) {
						IInteractiveActor::Execute_Interact(Actor);
					}
				}
			
		}
	}
}
