// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoardManager.generated.h"


class ABoardPiece;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TFCOE_API UBoardManager : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBoardManager();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	TMap<FVector2D, ABoardPiece*> GridPairing;
	
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION(BlueprintCallable)
	void AddGridPairing(FVector2D GridCoordinates, ABoardPiece* BoardPiece);

	UFUNCTION(BlueprintCallable)
	ABoardPiece* GetGridPiece(FVector2D GridCoordinates) const;
};
