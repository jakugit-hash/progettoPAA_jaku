#include "MyGameMode.h"
#include "Brawler.h"
#include "CoinTossManager.h"
#include "CoinWidget.h"
#include "GridManager.h"
#include "Sniper.h"
#include "Kismet/GameplayStatics.h"

AMyGameMode::AMyGameMode()
{
    // Set default values
    bIsPlayerTurn = false;

    // Initialize pointers to nullptr
    GridManager = nullptr;
    CoinTossManager = nullptr;
    CoinWidget = nullptr;
}

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Log to verify BeginPlay is called
    UE_LOG(LogTemp, Warning, TEXT("Game Mode BeginPlay called!"));

    // Spawn the CoinTossManager
    CoinTossManager = GetWorld()->SpawnActor<ACoinTossManager>();
    if (CoinTossManager)
    {
        // Bind the coin toss result handler
        CoinTossManager->OnCoinTossComplete.AddDynamic(this, &AMyGameMode::HandleCoinTossResult);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn CoinTossManager!"));
    }

    // Create and display the coin toss UI
    if (CoinWidgetClass)
    {
        CoinWidget = CreateWidget<UCoinWidget>(GetWorld(), CoinWidgetClass);
        if (CoinWidget)
        {
            // Pass the CoinTossManager to the widget
            CoinWidget->SetCoinTossManager(CoinTossManager);

            // Add the widget to the viewport
            CoinWidget->AddToViewport();
  // Log to verify the widget is created
            UE_LOG(LogTemp, Warning, TEXT("CoinWidget created and added to viewport!"));
        }
else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create CoinWidget!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CoinWidgetClass is not set!"));
    }
    InitializeGame();
}

void AMyGameMode::InitializeGame()
{
    // Find or spawn the GridManager
    GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
    if (!GridManager)
    {
        UE_LOG(LogTemp, Error, TEXT("GridManager not found in the level! Spawning a new one..."));
        FActorSpawnParameters SpawnParams;
        GridManager = GetWorld()->SpawnActor<AGridManager>(AGridManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    }

    if (!GridManager)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn GridManager!"));
        return;
    }

    // Spawn the grid and obstacles
    GridManager->CreateGrid();
    GridManager->GenerateObstacles();

    // Find random free positions for the Sniper and Brawler
    int32 SniperX, SniperY, BrawlerX, BrawlerY;

    // Find a random free cell for the Sniper
    if (GridManager->FindRandomEmptyCell(SniperX, SniperY))
    {
        FVector2D SniperPosition = FVector2D(SniperX, SniperY);
        ASniper* Sniper = GetWorld()->SpawnActor<ASniper>(ASniper::StaticClass(), GridManager->GetCellWorldPosition(SniperX, SniperY), FRotator::ZeroRotator);
        if (Sniper)
        {
            Sniper->SetGridPosition(SniperPosition);
            UE_LOG(LogTemp, Warning, TEXT("Sniper spawned at (%d, %d)"), SniperX, SniperY);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn Sniper!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No free cell found for Sniper!"));
    }

    // Find a random free cell for the Brawler
    if (GridManager->FindRandomEmptyCell(BrawlerX, BrawlerY))
    {
        // Ensure the Brawler is not spawned in the same cell as the Sniper
        while (BrawlerX == SniperX && BrawlerY == SniperY)
        {
            GridManager->FindRandomEmptyCell(BrawlerX, BrawlerY);
        }

        FVector2D BrawlerPosition = FVector2D(BrawlerX, BrawlerY);
        ABrawler* Brawler = GetWorld()->SpawnActor<ABrawler>(ABrawler::StaticClass(), GridManager->GetCellWorldPosition(BrawlerX, BrawlerY), FRotator::ZeroRotator);
        if (Brawler)
        {
            Brawler->SetGridPosition(BrawlerPosition);
            UE_LOG(LogTemp, Warning, TEXT("Brawler spawned at (%d, %d)"), BrawlerX, BrawlerY);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn Brawler!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No free cell found for Brawler!"));
    }

    // Verify reachability after placing units
    TArray<TArray<bool>> ObstacleMap;
    GridManager->CreateObstacleMap(ObstacleMap);

    if (GridManager->AreAllCellsReachable(ObstacleMap))
    {
        UE_LOG(LogTemp, Warning, TEXT("All cells are reachable after placing units!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Some cells are unreachable after placing units!"));
    }
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

void AMyGameMode::HandleCoinTossResult(bool bIsPlayerTurnResult)
{
    // Log to verify the coin toss result
    UE_LOG(LogTemp, Warning, TEXT("Coin toss result: %s"), bIsPlayerTurnResult ? TEXT("Player starts") : TEXT("AI starts"));

    bIsPlayerTurn = bIsPlayerTurnResult;

    if (bIsPlayerTurn)
    {
        StartPlayerTurn();
    }
    else
    {
        StartAITurn();
    }

    // Remove the coin toss UI
    if (CoinWidget)
    {
        CoinWidget->RemoveFromParent();
        UE_LOG(LogTemp, Warning, TEXT("CoinWidget removed from viewport!"));
    }
}