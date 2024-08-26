// Copyright 2024 Ryan Sweeney All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RTSHUD.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSelectedActorsSignature, const TArray<AActor*>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FHoveredActorsSignature, const TArray<AActor*>&);

UCLASS()
class OPENRTSCAMERA_API ARTSHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Selection Box")
	FLinearColor SelectionBoxColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Selection Box")
	float SelectionBoxThickness;

	// UPROPERTY()
	// Delegate for when any and all actors are selected
	FSelectedActorsSignature OnSelectedActorsDelegate;

	// UPROPERTY()
	// Delegate for when any and all actors are hovered
	FHoveredActorsSignature OnHoveredActorsDelegate;
public:
	ARTSHUD();

	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void SetPlayerController(APlayerController* NewPlayerController);

	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void PositionOnMouse();
	
	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void InitStartPosition();

	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void UpdateEndPosition();
	
	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void BeginGroupSelection();

	UFUNCTION(BlueprintCallable, Category = "Selection Box")
	void EndGroupSelection();
	
protected:
	virtual void DrawHUD() override;
	void DrawSelectionBox();
	void PerformSelection();
	
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController = nullptr;
private:
	
	bool bIsDrawingSelectionBox;
	bool bIsPerformingFinalSelection;
	
	FVector2D SelectionStart;
	FVector2D SelectionEnd;
};
