// Copyright 2024 Ryan Sweeney All Rights Reserved.

#include "RTSSelectorSubsystem.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

URTSSelectorSubsystem::URTSSelectorSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UInputAction>
		SingleSelectActionFinder(TEXT("/OpenRTSCamera/Inputs/SingleSelect"));
	this->SingleSelect = SingleSelectActionFinder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction>
		GroupSelectActionFinder(TEXT("/OpenRTSCamera/Inputs/GroupSelect"));
	this->GroupSelect = GroupSelectActionFinder.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>
		LeftShiftActionFinder(TEXT("/OpenRTSCamera/Inputs/LeftShift"));
	this->LeftShift = LeftShiftActionFinder.Object;

	// static ConstructorHelpers::FObjectFinder<UInputAction>
	// 	LeftControlActionFinder(TEXT("/OpenRTSCamera/Inputs/LeftControl"));
	// this->LeftControl = LeftControlActionFinder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>
		InputMappingContextFinder(TEXT("/OpenRTSCamera/Inputs/OpenRTSCameraInputs"));
	this->InputMappingContext = InputMappingContextFinder.Object;
}

/**
 * Registers the player controller with the subsystem, replacing the existing player controller.
 * Also sets up the HUD reference.
 * @param NewPlayerController 
 */
void URTSSelectorSubsystem::RegisterPlayerController(APlayerController* NewPlayerController)
{
	this->PlayerController = NewPlayerController;
	this->HUD = Cast<ARTSHUD>(NewPlayerController->GetHUD());
	this->HUD->SetPlayerController(NewPlayerController);
	this->HUD->OnSelectedActorsDelegate.AddUObject(this, &URTSSelectorSubsystem::ProcessSelectedActors);
	this->HUD->OnHoveredActorsDelegate.AddUObject(this, &URTSSelectorSubsystem::ProcessHoveredActors);
	
	BindInputActions();
	BindInputMappingContext();
}

// void URTSSelectorSubsystem::ClearSelectedActors()
// {
// 	SelectedSet.Empty();
// }
//
// void URTSSelectorSubsystem::ClearHoveredActors()
// {
// 	HoveredSet.Empty();
// }

/**
 * Processes the selected actors from the HUD, adding/removing them from the selection
 * Sends all selected/deselected rather than just the newly selected/deselected this
 * implies doing a full refresh of any UI that takes this info
 * @param NewSelectedActors 
 */
void URTSSelectorSubsystem::ProcessSelectedActors(const TArray<AActor*>& NewSelectedActors)
{	
	TArray<URTSSelectable*> InputSelectables;
	GetSelectablesFromActors(NewSelectedActors, InputSelectables);

	// Unhover all as selection has happened
	UnhoverActors();
	
	// If it's a deselect, clear the selection and return
	if(InputSelectables.Num() == 0)
	{
		DeselectActors();
		return;
	}

	const TArray<URTSSelectable*> CurrentlySelected = SelectedSet.Array();
	TArray<URTSSelectable*> Selected;
	TArray<URTSSelectable*> Deselected;
	
	// If shift is held, it's an append to selection otherwise, clear current selection and select new actors
	if(bShiftDown)
	{
		if(InputSelectables.Num() == 1)
		{
			URTSSelectable* NewSelectable = InputSelectables[0];

			// If it's already selected, deselect, otherwise select
			if(CurrentlySelected.Contains(NewSelectable))
			{
				Selected = CurrentlySelected;
				Selected.Remove(NewSelectable);

				Deselected.Add(NewSelectable);
			} else
			{
				Selected = CurrentlySelected;
				Selected.Add(NewSelectable);
			}
		}

		if(InputSelectables.Num() > 1)
		{
			Selected = CurrentlySelected;
			for(const auto& Selectable : InputSelectables)
			{
				if(!CurrentlySelected.Contains(Selectable))
				{
					Selected.Add(Selectable);
				}
			}
		}
	} else
	{
		Selected = InputSelectables;
		Deselected = CurrentlySelected;
	}
	
	DeselectActors(Deselected);
	SelectActors(Selected);
}

void URTSSelectorSubsystem::ProcessHoveredActors(const TArray<AActor*>& NewHoveredActors)
{
	TArray<URTSSelectable*> NewSelectables;
	GetSelectablesFromActors(NewHoveredActors, NewSelectables);
	
	UnhoverActors();

	HoverActors(NewSelectables);
}

void URTSSelectorSubsystem::SingleSelectEnd(const FInputActionValue& Value)
{
	HUD->PositionOnMouse();
	HUD->EndGroupSelection();
	bSingleSelect = true;
}

void URTSSelectorSubsystem::GroupSelectStart(const FInputActionValue& Value)
{
	bSingleSelect = false;
	bGroupSelecting = false;
	
	PlayerController->GetMousePosition(StartPosition.X, StartPosition.Y);
	
	HUD->InitStartPosition();
}

void URTSSelectorSubsystem::GroupSelectUpdate(const FInputActionValue& Value)
{
	if(!bSingleSelect && !bGroupSelecting)
	{
		FVector2D CurrentPosition = FVector2D::ZeroVector;
		PlayerController->GetMousePosition(CurrentPosition.X, CurrentPosition.Y);
	
		if(FVector2D::Distance(StartPosition, CurrentPosition) > 20.0)
		{
			bGroupSelecting = true;
		}
	}

	HUD->BeginGroupSelection();
	HUD->UpdateEndPosition();
}

void URTSSelectorSubsystem::GroupSelectEnd(const FInputActionValue& Value)
{
	bGroupSelecting = false;
	HUD->EndGroupSelection();
}

void URTSSelectorSubsystem::ShiftDown(const FInputActionValue& Value)
{
	bShiftDown = true;
}

void URTSSelectorSubsystem::ShiftUp(const FInputActionValue& Value)
{
	bShiftDown = false;
}

void URTSSelectorSubsystem::RegisterHoverStart(URTSSelectable* Selectable)
{
	if(bGroupSelecting)
	{
		return;
	}
	
	this->HoveredSet.Add(Selectable);
	
	Selectable->HoverStart();
}

void URTSSelectorSubsystem::RegisterHoverEnd(URTSSelectable* Selectable)
{
	if(bGroupSelecting)
	{
		return;
	}
	
	// UE_LOG(LogTemp, Warning, TEXT("SelectorSubsystem - Hover End"));

	this->HoveredSet.Remove(Selectable);
	
	Selectable->HoverEnd();
}

void URTSSelectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// UE_LOG(LogTemp, Warning, TEXT("SelectorSubsystem - Initialize"));
}

void URTSSelectorSubsystem::BindInputActions()
{
	if (const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(this->PlayerController->InputComponent))
	{
		EnhancedInputComponent->BindAction(
			this->SingleSelect,
			ETriggerEvent::Completed,
			this,
			&URTSSelectorSubsystem::SingleSelectEnd
		);
		
		EnhancedInputComponent->BindAction(
			this->GroupSelect,
			ETriggerEvent::Started,
			this,
			&URTSSelectorSubsystem::GroupSelectStart
		);
		
		EnhancedInputComponent->BindAction(
			this->GroupSelect,
			ETriggerEvent::Triggered,
			this,
			&URTSSelectorSubsystem::GroupSelectUpdate
		);
		
		EnhancedInputComponent->BindAction(
			this->GroupSelect,
			ETriggerEvent::Completed,
			this,
			&URTSSelectorSubsystem::GroupSelectEnd
		);

		EnhancedInputComponent->BindAction(
			this->LeftShift,
			ETriggerEvent::Started,
			this,
			&URTSSelectorSubsystem::ShiftDown
		);
		
		EnhancedInputComponent->BindAction(
			this->LeftShift,
			ETriggerEvent::Completed,
			this,
			&URTSSelectorSubsystem::ShiftUp
		);
	}
}

void URTSSelectorSubsystem::BindInputMappingContext()
{
	if (PlayerController && PlayerController->GetLocalPlayer())
	{
		if (const auto Input = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			PlayerController->bShowMouseCursor = true;

			// Check if the context is already bound to prevent double binding
			if (!Input->HasMappingContext(this->InputMappingContext))
			{
				Input->ClearAllMappings();
				Input->AddMappingContext(this->InputMappingContext, 0);
			}
		}
	}
}

/**
 * Selects the actors filtering for actors with the selectable component in and broadcasts the selection
 * Calls the select function on the selectable component on all valid actors
 * @param ActorsToSelect 
 */
void URTSSelectorSubsystem::SelectActors(const TArray<URTSSelectable*>& ActorsToSelect)
{
	TArray<AActor*> BroadcastActors;

	// Looks for actors with the selectable component and calls the select function on them
	for (const auto& Selectable : ActorsToSelect)
	{
		this->SelectedSet.Add(Selectable);
		Selectable->Select();
		BroadcastActors.Add(Selectable->GetOwner());
	}

	if(BroadcastActors.Num() > 0)
	{
		OnActorsSelectedDelegate.Broadcast(BroadcastActors);
	}
}

/**
 * Deselects the actors filtering for actors with the selectable component in and broadcasts the deselection
 * Calls the deselect function on the selectable component on all valid actors
 * @param ActorsToDeselect 
 */
void URTSSelectorSubsystem::DeselectActors(const TArray<URTSSelectable*>& ActorsToDeselect)
{
	TArray<AActor*> BroadcastActors;
	
	// Iterate over currently selected actors
	for (const auto& Selectable : ActorsToDeselect)
	{
		SelectedSet.Remove(Selectable);
		Selectable->Deselect();
		BroadcastActors.Add(Selectable->GetOwner());
	}
	
	if(BroadcastActors.Num() > 0)
	{
		OnActorsDeselectedDelegate.Broadcast(BroadcastActors);
	}
}

/**
 * Deselects all actors and broadcasts the deselection
 */
void URTSSelectorSubsystem::DeselectActors()
{
	TArray<AActor*> BroadcastActors;
	
	// Iterate over currently selected actors
	for (const auto& Selectable : SelectedSet)
	{
		Selectable->Deselect();
		BroadcastActors.Add(Selectable->GetOwner());
	}

	SelectedSet.Empty();

	if(BroadcastActors.Num() > 0)
	{
		OnActorsDeselectedDelegate.Broadcast(BroadcastActors);
	}
}

/**
 * Hover over the actors filtering for actors with the selectable component in and broadcasts the hover start
 * Calls the hover start function on the selectable component on all valid actors
 * @param ActorsToHover 
 */
void URTSSelectorSubsystem::HoverActors(const TArray<URTSSelectable*>& ActorsToHover)
{
	TArray<AActor*> BroadcastActors;
	
	for (const auto& Selectable : ActorsToHover)
	{
		this->HoveredSet.Add(Selectable);
		Selectable->HoverStart();
		BroadcastActors.Add(Selectable->GetOwner());
	}

	if(BroadcastActors.Num() > 0)
	{
		OnActorsHoverStartDelegate.Broadcast(BroadcastActors);
	}
}

/**
 * Unhover over the actors filtering for actors with the selectable component in and broadcasts the hover end
 * Calls the hover end function on the selectable component on all valid actors
 * @param ActorsToUnhover 
 */
void URTSSelectorSubsystem::UnhoverActors(const TArray<URTSSelectable*>& ActorsToUnhover)
{
	TArray<AActor*> BroadcastActors;

	// Iterate over currently selected actors
	for (const auto& Hovered : ActorsToUnhover)
	{
		HoveredSet.Remove(Hovered);
		Hovered->HoverEnd();
		BroadcastActors.Add(Hovered->GetOwner());
	}

	if(BroadcastActors.Num() > 0)
	{
		OnActorsHoverEndDelegate.Broadcast(BroadcastActors);
	}
}

/**
 * Unhover over the actors filtering for actors with the selectable component in and broadcasts the hover end
 * Calls the hover end function on the selectable component on all valid actors
 */
void URTSSelectorSubsystem::UnhoverActors()
{
	TArray<AActor*> BroadcastActors;
	
	for (const auto& Hovered : HoveredSet)
	{
		// Call OnDeselected for actors that are no longer selected
		Hovered->HoverEnd();
		BroadcastActors.Add(Hovered->GetOwner());
	}

	HoveredSet.Empty();

	if(BroadcastActors.Num() > 0)
	{
		OnActorsHoverEndDelegate.Broadcast(BroadcastActors);
	}
}

/**
 * Gets the selectable components from the actors
 * @param Actors 
 * @param OutSelectables 
 */
void URTSSelectorSubsystem::GetSelectablesFromActors(const TArray<AActor*>& Actors,
	TArray<URTSSelectable*>& OutSelectables)
{
	for (const auto& Actor : Actors)
	{
		if (URTSSelectable* SelectableComponent = Actor->FindComponentByClass<URTSSelectable>())
		{
			OutSelectables.Add(SelectableComponent);
		}
	}
}
