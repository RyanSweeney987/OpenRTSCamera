// Copyright 2024 Ryan Sweeney All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"
#include "RTSSelectable.generated.h"

UENUM(BlueprintType, Category = "RTS Selection")
enum class ESelectionState : uint8
{
	None		= 0,
	Selected	= 1,
	Hovered		= 2
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionStateChangedSignature, ESelectionState, SelectionState);

class URTSSelectorSubsystem;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OPENRTSCAMERA_API URTSSelectable : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "RTS Selection")
	FOnSelectionStateChangedSignature OnSelectionStateChangedDelegate;
	
private:
	UPROPERTY()
	ESelectionState CurrentSelectionState = ESelectionState::None;

	UPROPERTY()
	URTSSelectorSubsystem* SelectorSubsystem = nullptr;

	UPROPERTY()
	bool bSelected = false;

	UPROPERTY()
	bool bHovered = false;
	
public:
	URTSSelectable()
	{
		PrimaryComponentTick.bCanEverTick = false;
	}

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "RTS Selection")
	void BindToActorMouseEvents(AActor* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "RTS Selection")
	void BindToBeginCursorOverEvent(AActor* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "RTS Selection")
	void BindToEndCursorOverEvent(AActor* Owner);
	
	UFUNCTION(BlueprintCallable, Category = "RTS Selection")
	ESelectionState GetSelectionState() const;

	UFUNCTION(BlueprintCallable, Category = "RTS Selection")
	void Select();

	UFUNCTION(BlueprintCallable, Category = "RTS Selection")
	void Deselect();

	UFUNCTION(BlueprintCallable, Category = "RTS Selection")
	void HoverStart();

	UFUNCTION(BlueprintCallable, Category = "RTS Selection")
	void HoverEnd();
private:
	UFUNCTION()
	void OnBeginCursorOver(AActor* TouchedActor = nullptr);
	
	UFUNCTION()
	void OnEndCursorOver(AActor* TouchedActor = nullptr);
};
