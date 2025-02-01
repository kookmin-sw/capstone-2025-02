#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Neutral UMETA(DisplayName = "Neutral"),
	ECS_Attacking UMETA(DisplayName = "Attacking"),
	ECS_Rolling UMETA(DisplayName = "Rolling"),
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Attacking UMETA(DisplayName = "Attacking")
};