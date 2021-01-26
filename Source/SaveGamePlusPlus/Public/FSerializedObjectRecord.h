#pragma once

#include "CoreMinimal.h"
#include "FSerializedObjectRecord.generated.h"

USTRUCT(BlueprintType)
struct FSerializedObjectRecord
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool RespawnOnLoad;

	UPROPERTY(BlueprintReadWrite)
	UClass* Class;

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> Data;

	UPROPERTY(BlueprintReadWrite)
	FTransform Transform;

	FSerializedObjectRecord()
	{
		Class = nullptr;
		RespawnOnLoad = false;
	}
};