// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "MyProject2GameMode.h"
#include "MyProject2Character.h"
#include "MyProjectHud.h"





AMyProject2GameMode::AMyProject2GameMode()
{
	HUDClass = AMyProjectHud::StaticClass();
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
