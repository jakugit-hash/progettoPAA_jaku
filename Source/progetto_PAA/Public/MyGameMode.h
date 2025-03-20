#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

// Forward declarations
class ACoinTossManager;
class UCoinWidget;

UCLASS()
class PROGETTO_PAA_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameMode();

protected:
	virtual void BeginPlay() override;

public:
	// Function to spawn the grid and units
	void InitializeGame();

	// Turn management
	void StartPlayerTurn();
	void StartAITurn();
	void EndTurn();

	// Handle coin toss result
	UFUNCTION()
	void HandleCoinTossResult(bool bIsPlayerTurnResult);

private:
	// Reference to the GridManager
	UPROPERTY()
	class AGridManager* GridManager;

	// Turn tracking
	bool bIsPlayerTurn;

	// Coin Toss Manager
	UPROPERTY()
	ACoinTossManager* CoinTossManager;


	
	// Coin Toss Widget
	UPROPERTY()
	TSubclassOf<UUserWidget> CoinWidget;

	// Widget class to use for the coin toss UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CoinWidgetClass; // La classe del widget (Blueprint)
	
	UUserWidget* CoinWidgetInstance; // L'istanza effettiva

};