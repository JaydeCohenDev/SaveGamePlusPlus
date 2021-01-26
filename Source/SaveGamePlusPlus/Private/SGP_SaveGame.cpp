#include "../Public/SGP_SaveGame.h"
#include "../Public/ISerializableObject.h"
#include "../Public/SGP_SaveGameArchive.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void USGP_SaveGame::SaveObject(UObject* Obj)
{
	if(Obj == nullptr) return;

	FSerializedObjectRecord Record = FSerializedObjectRecord();
	
	// Grab and store SaveID from the interface
	FGuid SID;
	if (Obj->GetClass()->ImplementsInterface(UISerializableObject::StaticClass()))
	{
		SID = IISerializableObject::Execute_GetSaveID(Obj);
		Record.RespawnOnLoad = IISerializableObject::Execute_RespawnOnLoad(Obj);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s cannot be serialized because it does not implement the ISerializableObject interface!"), *Obj->GetName());
		return;
	}
	
	Record.Class = Obj->GetClass();
	Record.Name = Obj->GetName();

	// Get transform if actor
	AActor* Act = Cast<AActor>(Obj);
	if(Act != nullptr) Record.Transform = Act->GetTransform();

	// Get transform if scene component
	USceneComponent* ScC = Cast<USceneComponent>(Obj);
	if(ScC != nullptr) Record.Transform = ScC->GetComponentTransform();

	// Save properties marked with "SaveGame" flag set.
	FMemoryWriter MemoryWriter(Record.Data, true);
	FSGP_SaveGameArchive Ar(MemoryWriter);

	// Serialize object
	Obj->Serialize(Ar);

	// Store record
	SaveData.Add(SID, Record);
}

void USGP_SaveGame::LoadObject(UObject* Obj)
{
	if(Obj == nullptr) return;
	
	// Grab and store SaveID from the interface
	FGuid SID;
	if (Obj->GetClass()->ImplementsInterface(UISerializableObject::StaticClass()))
	{
		SID = IISerializableObject::Execute_GetSaveID(Obj);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s cannot be deserialized because it does not implement the ISerializableObject interface!"), *Obj->GetName());
		return;
	}
	
	FSerializedObjectRecord Record = SaveData[SID];
	Obj->Rename(*Record.Name);

	// Set transform if actor
	AActor* Act = Cast<AActor>(Obj);
	if(Act != nullptr) Act->SetActorTransform(Record.Transform);

	// Set transform if scene component
	USceneComponent* ScC = Cast<USceneComponent>(Obj);
	if(ScC != nullptr) ScC->SetWorldTransform(Record.Transform);

	// Load properties marked with "SaveGame" flag set.
	FMemoryReader MemoryReader(Record.Data);
	FSGP_SaveGameArchive Ar(MemoryReader);
	Obj->Serialize(Ar);
}

void USGP_SaveGame::SpawnAndLoad(FGuid SaveID)
{
	if(!SaveData.Contains(SaveID)) return;

	FSerializedObjectRecord Record = SaveData[SaveID];
	if(!Record.RespawnOnLoad) return;

	if(Record.Class->IsChildOf(AActor::StaticClass()))
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Name = *Record.Name;
		AActor *NewActor = GetWorld()->SpawnActor<AActor>(Record.Class, Record.Transform, SpawnParameters);

		LoadObject(NewActor);
	}
}

void USGP_SaveGame::LoadAll()
{
	// Detect all serializable actors in scene
	TMap<FGuid, AActor*> WorldActors = TMap<FGuid, AActor*>();
	TArray<AActor*> FoundActors = TArray<AActor*>();
	UGameplayStatics::GetAllActorsWithInterface(GEngine->GetWorldContexts()[0].World(), UISerializableObject::StaticClass(), FoundActors);

	//UE_LOG(LogTemp, Error, TEXT("Count %d"), FoundActors.Num());

	for(AActor* a : FoundActors)
	{
		if(a->GetClass()->ImplementsInterface(UISerializableObject::StaticClass()))
		{
			WorldActors.Add(IISerializableObject::Execute_GetSaveID(a), a);
		}
	}

	// Load actors
	for (const TPair<FGuid, FSerializedObjectRecord>& pair : SaveData)
	{
		if(WorldActors.Contains(pair.Key))
		{
			// Load object
			LoadObject(WorldActors[pair.Key]);
			
		} else
		{
			// Spawn and load
			SpawnAndLoad(pair.Key);
		}
	}
}

FSerializedObjectRecord USGP_SaveGame::RetrieveObject(FGuid SID, bool& Success)
{
	Success = SaveData.Contains(SID);
	if(!Success) return FSerializedObjectRecord();
	
	return SaveData[SID];
}
