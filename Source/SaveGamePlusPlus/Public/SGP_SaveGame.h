#pragma once

#include "CoreMinimal.h"
#include "FSerializedObjectRecord.h"
#include "GameFramework/SaveGame.h"
#include "SGP_SaveGame.generated.h"

/**
 * Custom SaveGame object implementation that holds the serialized data.
 */
UCLASS(Blueprintable)
class SAVEGAMEPLUSPLUS_API USGP_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Save Load")
	TMap<FGuid, FSerializedObjectRecord> SaveData;

	UFUNCTION(BlueprintCallable, Category="Save Load")
	void SaveObject(UObject* Obj);

	UFUNCTION(BlueprintCallable, Category="Save Load")
	void LoadObject(UObject* Obj);

	UFUNCTION(BlueprintCallable, Category="Save Load")
	void SpawnAndLoad(FGuid SaveID);

	UFUNCTION(BlueprintCallable, Category="Save Load")
	void LoadAll();

	UFUNCTION(BlueprintCallable, Category="Save Load")
	FSerializedObjectRecord RetrieveObject(FGuid SID, bool& Success);
	
};
