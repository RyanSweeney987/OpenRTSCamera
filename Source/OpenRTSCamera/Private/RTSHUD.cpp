// Copyright 2024 Ryan Sweeney All Rights Reserved.

#include "RTSHUD.h"

#include "RTSSelectable.h"
#include "Engine/Canvas.h"

// Constructor implementation: Initializes default values.
ARTSHUD::ARTSHUD()
{
	SelectionBoxColor = FLinearColor::Green;
	SelectionBoxThickness = 1.0f;
	bIsDrawingSelectionBox = false;
	bIsPerformingFinalSelection = false;
}

void ARTSHUD::SetPlayerController(APlayerController* NewPlayerController)
{
	this->PlayerController = NewPlayerController;
}

void ARTSHUD::PositionOnMouse()
{
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	
	SelectionStart = MousePosition - FVector2D(1, 1);
	SelectionEnd = MousePosition + FVector2D(1, 1);
}

void ARTSHUD::InitStartPosition()
{
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	
	SelectionStart = MousePosition;
}

void ARTSHUD::UpdateEndPosition()
{
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	
	SelectionEnd = MousePosition;
}

// Starts the selection process, setting the initial point and activating the selection flag.
void ARTSHUD::BeginGroupSelection()
{
	bIsDrawingSelectionBox = true;
}

// Ends the selection process and triggers the selection logic.
void ARTSHUD::EndGroupSelection()
{
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	
	SelectionEnd = MousePosition;
	
	bIsDrawingSelectionBox = false;
	bIsPerformingFinalSelection = true;
}

/**
 * Called when the HUD should be drawn.
 * Selection uses a HUD function which needs to be called when the HUD is drawn
 */
void ARTSHUD::DrawHUD()
{
	Super::DrawHUD(); // Call the base class implementation.
		
	// Draw the selection box if it's active.
	if (bIsDrawingSelectionBox)
	{
		DrawSelectionBox();
	}

	if (bIsDrawingSelectionBox || bIsPerformingFinalSelection)
	{
		PerformSelection();
	}
}

// Default implementation of DrawSelectionBox. Draws a rectangle on the HUD.
void ARTSHUD::DrawSelectionBox()
{
	if (Canvas)
	{
		// Calculate corners of the selection rectangle.
		const auto TopRight = FVector2D(SelectionEnd.X, SelectionStart.Y);
		const auto BottomLeft = FVector2D(SelectionStart.X, SelectionEnd.Y);

		// Draw lines to form the selection rectangle.
		Canvas->K2_DrawLine(SelectionStart, TopRight, SelectionBoxThickness, SelectionBoxColor);
		Canvas->K2_DrawLine(TopRight, SelectionEnd, SelectionBoxThickness, SelectionBoxColor);
		Canvas->K2_DrawLine(SelectionEnd, BottomLeft, SelectionBoxThickness, SelectionBoxColor);
		Canvas->K2_DrawLine(BottomLeft, SelectionStart, SelectionBoxThickness, SelectionBoxColor);
	}
}

// Default implementation of PerformSelection. Selects actors within the selection box.
void ARTSHUD::PerformSelection()
{
	// UE_LOG(LogTemp, Warning, TEXT("HUD - Perform Selection"));
	
	// Array to store actors that are within the selection rectangle.
	TArray<AActor*> SelectedActors;
	GetActorsInSelectionRectangle<AActor>(SelectionStart, SelectionEnd, SelectedActors, false, false);

	// if(SelectedActors.Num() > 2)
	// {
	// 	for(const auto& Actor : SelectedActors)
	// 	{
	// 		// UE_LOG(LogTemp, Warning, TEXT("Selected Actor: %s"), *Actor->GetName());
	// 		FBox Box = Actor->GetComponentsBoundingBox();
	// 		DrawDebugBox(GetWorld(), Box.GetCenter(), Box.GetExtent(), FColor::Green, false, 0.1f);
	// 	}
	// }
	
	if(bIsPerformingFinalSelection) {
		OnSelectedActorsDelegate.Broadcast(SelectedActors);
	} else
	{
		OnHoveredActorsDelegate.Broadcast(SelectedActors);
	}

	bIsPerformingFinalSelection = false;
}
