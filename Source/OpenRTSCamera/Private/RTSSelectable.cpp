// Copyright 2024 Ryan Sweeney All Rights Reserved.

#include "RTSSelectable.h"

#include "Engine/LocalPlayer.h"
#include "RTSSelectorSubsystem.h"

void URTSSelectable::BeginPlay()
{
	Super::BeginPlay();
	
	SelectorSubsystem = GetWorld()->GetFirstLocalPlayerFromController()->GetSubsystem<URTSSelectorSubsystem>();
}

/*
 * Binds to all the mouse events of the parent actor. So that the actor can essentially select itself.
 */
void URTSSelectable::BindToActorMouseEvents(AActor* Owner)
{
	// UE_LOG(LogTemp, Warning, TEXT("Binding to Actor Mouse Events"));
	
	if(Owner)
	{
		Owner->OnBeginCursorOver.AddDynamic(this, &URTSSelectable::OnBeginCursorOver);
		Owner->OnEndCursorOver.AddDynamic(this, &URTSSelectable::OnEndCursorOver);
	}
}

void URTSSelectable::BindToBeginCursorOverEvent(AActor* Owner)
{
	if(Owner)
	{
		Owner->OnBeginCursorOver.AddDynamic(this, &URTSSelectable::OnBeginCursorOver);
	}
}

void URTSSelectable::BindToEndCursorOverEvent(AActor* Owner)
{
	if(Owner)
	{
		Owner->OnEndCursorOver.AddDynamic(this, &URTSSelectable::OnEndCursorOver);
	}
}

ESelectionState URTSSelectable::GetSelectionState() const
{
	return CurrentSelectionState;
}

void URTSSelectable::Select()
{
	bSelected = true;
	bHovered = false;
	CurrentSelectionState = ESelectionState::Selected;
	
	OnSelectionStateChangedDelegate.Broadcast(CurrentSelectionState);
}

void URTSSelectable::Deselect()
{
	bSelected = false;

	if(bHovered) {
		CurrentSelectionState = ESelectionState::Hovered;
	} else {
		CurrentSelectionState = ESelectionState::None;
	}
	
	OnSelectionStateChangedDelegate.Broadcast(CurrentSelectionState);
}

void URTSSelectable::HoverStart()
{
	bHovered = true;
	CurrentSelectionState = ESelectionState::Hovered;
	
	OnSelectionStateChangedDelegate.Broadcast(CurrentSelectionState);
}

void URTSSelectable::HoverEnd()
{
	bHovered = false;

	if(bSelected) {
		CurrentSelectionState = ESelectionState::Selected;
	} else {
		CurrentSelectionState = ESelectionState::None;
	}
	
	OnSelectionStateChangedDelegate.Broadcast(CurrentSelectionState);
}

void URTSSelectable::OnBeginCursorOver(AActor* TouchedActor)
{
	SelectorSubsystem->RegisterHoverStart(this);
}

void URTSSelectable::OnEndCursorOver(AActor* TouchedActor)
{
	SelectorSubsystem->RegisterHoverEnd(this);
}
