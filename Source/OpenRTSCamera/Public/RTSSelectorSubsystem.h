// Copyright 2024 Ryan Sweeney All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "RTSHUD.h"
#include "RTSSelectable.h"
#include "Components/ActorComponent.h"
#include "RTSSelectorSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorsSelectedSignature, const TArray<AActor*>&, SelectedActors);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorsDeselectedSignature, const TArray<AActor*>&, DeselectedActors);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorsHoverStartSignature, const TArray<AActor*>&, HoverStartActors);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorsHoverEndSignature, const TArray<AActor*>&, HoverEndActors);

/**
 * 
 */
UCLASS()
class OPENRTSCAMERA_API URTSSelectorSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	bool bShiftDown = false;
	// bool bControlDown = false;
	bool bSingleSelect = false;
	bool bGroupSelecting = false;
	FVector2D StartPosition = FVector2D::ZeroVector;
	double SelectStartTime = 0;
	
public:
	// BlueprintReadWrite allows access and modification in Blueprints
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	TObjectPtr<UInputAction> GroupSelect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	TObjectPtr<UInputAction> SingleSelect;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RTSCamera - Inputs")
	TObjectPtr<UInputAction> LeftShift;
	
	UPROPERTY(BlueprintReadOnly, Category = "RTSCamera - Selection")
	TSet<URTSSelectable*> SelectedSet;

	UPROPERTY(BlueprintReadOnly, Category = "RTSCamera - Selection")
	TSet<URTSSelectable*> HoveredSet;

	UPROPERTY(BlueprintAssignable, Category = "RTSCamera - Selection")
	FOnActorsSelectedSignature OnActorsSelectedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "RTSCamera - Selection")
	FOnActorsDeselectedSignature OnActorsDeselectedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "RTSCamera - Selection")
	FOnActorsHoverStartSignature OnActorsHoverStartDelegate;

	UPROPERTY(BlueprintAssignable, Category = "RTSCamera - Selection")
	FOnActorsHoverEndSignature OnActorsHoverEndDelegate;
	
public:
	URTSSelectorSubsystem();

	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void RegisterPlayerController(APlayerController* NewPlayerController);
	
	// Function to clear selected actors, can be overridden in Blueprints
	// UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	// void ClearSelectedActors();
	//
	// UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	// void ClearHoveredActors();
	
	// Function to handle selected actors, can be overridden in Blueprints
	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void ProcessSelectedActors(const TArray<AActor*>& NewSelectedActors);

	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void ProcessHoveredActors(const TArray<AActor*>& NewHoveredActors);

	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void SingleSelectEnd(const FInputActionValue& Value);
	
	// BlueprintCallable to allow calling from Blueprints
	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void GroupSelectStart(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void GroupSelectUpdate(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void GroupSelectEnd(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void ShiftDown(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void ShiftUp(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void RegisterHoverStart(URTSSelectable* Selectable);
	
	UFUNCTION(BlueprintCallable, Category = "RTSCamera - Selection")
	void RegisterHoverEnd(URTSSelectable* Selectable);

	static URTSSelectorSubsystem* Get(const APlayerController* PlayerController)
	{
		return CastChecked<URTSSelectorSubsystem>(PlayerController->GetLocalPlayer()->GetSubsystem<URTSSelectorSubsystem>());
	}
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY()
	TObjectPtr<ARTSHUD> HUD = nullptr;
	
	void BindInputActions();
	void BindInputMappingContext();

	void SelectActors(const TArray<URTSSelectable*>& ActorsToSelect);
	void DeselectActors(const TArray<URTSSelectable*>& ActorsToDeselect);
	void DeselectActors();

	void HoverActors(const TArray<URTSSelectable*>& ActorsToHover);
	void UnhoverActors(const TArray<URTSSelectable*>& ActorsToUnhover);
	void UnhoverActors();

	void GetSelectablesFromActors(const TArray<AActor*>& Actors, TArray<URTSSelectable*>& OutSelectables);
};
