// Copyright 2024 Ryan Sweeney All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CameraBlockingVolume.h"
#include "RTSCameraBoundsVolume.generated.h"

UCLASS()
class OPENRTSCAMERA_API ARTSCameraBoundsVolume : public ACameraBlockingVolume
{
	GENERATED_BODY()

	ARTSCameraBoundsVolume();
};
