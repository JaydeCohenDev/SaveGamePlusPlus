#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

/**
* Save Game Archiver.
*/
struct FSGP_SaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSGP_SaveGameArchive(FArchive& InInnerArchive) 
             : FObjectAndNameAsStringProxyArchive(InInnerArchive,true)
	{
		ArIsSaveGame = true;
		ArNoDelta = true; // Optional, useful when saving/loading variables without resetting the level.
		// Serialize variables even if weren't modified and mantain their default values.
	}
};