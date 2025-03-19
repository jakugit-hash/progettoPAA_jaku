#include "MyGameMode.h"
#include "GridManager.h"
#include "Sniper.h"
#include "Brawler.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "CoinTossManager.h"

AMyGameMode::AMyGameMode()
{
    // Set default values
    bIsPlayerTurn = false; // No turn until the game starts
}

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Spawn the CoinTossManager
    CoinTossManager = GetWorld()->SpawnActor<ACoinTossManager>();
    if (CoinTossManager)
    {
        // Bind the coin toss result handler
        CoinTossManager->OnCoinTossComplete.AddDynamic(this, &AMyGameMode::HandleCoinTossResult);
    }

    // Create and display the coin toss UI
    if (CoinTossWidgetClass)
    {
        CoinTossWidget = CreateWidget<UUserWidget>(GetWorld(), CoinTossWidgetClass);
        if (CoinTossWidget)
        {
            // Pass the CoinTossManager to the widget
            UObject* CoinTossManagerObject = Cast<UObject>(CoinTossManager);
            CoinTossWidget->SetProperty(TEXT("CoinTossManager"), CoinTossManagerObject);

            CoinTossWidget->AddToViewport();
        }
    }

    InitializeGame();
}

void AMyGameMode::InitializeGame()
{
    // Existing grid and unit spawning logic...
}

void AMyGameMode::StartPlayerTurn()
{
    UE_LOG(LogTemp, Warning, TEXT("Player's Turn!"));
    bIsPlayerTurn = true;
}

void AMyGameMode::StartAITurn()
{
    UE_LOG(LogTemp, Warning, TEXT("AI's Turn!"));
    bIsPlayerTurn = false;
}

void AMyGameMode::EndTurn()
{
    if (bIsPlayerTurn)
    {
        StartAITurn(); // Switch to AI's turn
    }
    else
    {
        StartPlayerTurn(); // Switch to player's turn
    }
}

void AMyGameMode::HandleCoinTossResult(bool bIsPlayerTurn)
{
    this->bIsPlayerTurn = bIsPlayerTurn;

    if (bIsPlayerTurn)
    {
        StartPlayerTurn();
    }
    else
    {
        StartAITurn();
    }

    // Remove the coin toss UI
    if (CoinTossWidget)
    {
        CoinTossWidget->RemoveFromParent();
    }
}