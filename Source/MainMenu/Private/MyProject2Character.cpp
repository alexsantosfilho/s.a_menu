// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "MyProject2Character.h"
#include "ProjectActor.h"
#include "Inimigo.h"
#include "AIPatrol.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Item.h"
#include "Alavanca.h"




const FName AMyProject2Character::LookUpBinding("LookUp");
const FName AMyProject2Character::LookRightBinding("Turn");

//////////////////////////////////////////////////////////////////////////
// AMyProject2Character

AMyProject2Character::AMyProject2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

												  //MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
												  //	MeshComp->SetCollisionProfileName("NoColission");
												  //MeshComp->AttachTo(GetCapsuleComponent());




												  //	InternalCameraOrigin = FVector(0.0f, -40.0f, 120.0f);

	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	//	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.0f;
	InternalCamera->SetupAttachment(InternalCameraBase);




	CollectCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollectCollision"));
	CollectCollisionComp->InitSphereRadius(200.0f);
	CollectCollisionComp->AttachTo(RootComponent);

	AutoPossessPlayer = EAutoReceiveInput::Player0;


	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(InternalCamera);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;

}
void AMyProject2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor
}


void AMyProject2Character::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Run", IE_Pressed, this, &AMyProject2Character::StartRun);
	InputComponent->BindAction("Run", IE_Released, this, &AMyProject2Character::StopRun);

	InputComponent->BindAction("Drop", IE_Pressed, this, &AMyProject2Character::DropProjectActor);


	InputComponent->BindAxis("MoveForward", this, &AMyProject2Character::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyProject2Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AMyProject2Character::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	InputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AMyProject2Character::OnToggleCamera);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &AMyProject2Character::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &AMyProject2Character::TouchStopped);



	InputComponent->BindAction("Collect", IE_Pressed, this, &AMyProject2Character::OnCollect);

}


void AMyProject2Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void AMyProject2Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}


void AMyProject2Character::StartRun() {
	GetCharacterMovement()->MaxWalkSpeed = 3000;

}
void AMyProject2Character::StopRun() {
	GetCharacterMovement()->MaxWalkSpeed = 1500;

}

void AMyProject2Character::DropProjectActor() {

	FActorSpawnParameters SpawnParameters;
	UWorld* World = GetWorld();

	if (World != nullptr) {
		const FRotator Rotation = Controller->GetControlRotation();

		AProjectActor* proj = World->SpawnActor<AProjectActor>(GetActorLocation(), Rotation, SpawnParameters);
		FVector NewVelocity = GetActorForwardVector() * 5000.f;

		//ProjectileClass->Velocity = FVector(NewVelocity);

		if (proj != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("Span ok!"));
		}
	}

}


void AMyProject2Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}



void AMyProject2Character::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyProject2Character::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
		//	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());

	}
}

void AMyProject2Character::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {



	if (OtherActor->IsA(AAIPatrol::StaticClass())) {

		Destroy();
		UE_LOG(LogTemp, Warning, TEXT("destruiu o principal"));
	}


	if ((OtherActor != nullptr) && (OtherActor != this) &&
		(OtherComp != nullptr) && (OtherActor->IsA(AProjectActor::StaticClass()))) {

		AProjectActor* MyProject2Character = Cast<AProjectActor>(OtherActor);

		UE_LOG(LogTemp, Warning, TEXT("Destruiu Parwwwwwwwabens"));
		Destroy();

	}


}

void AMyProject2Character::OnToggleCamera()
{
	UE_LOG(LogTemp, Warning, TEXT("carro"));

	EnableIncarView(!bInCarCameraActive);
}

void AMyProject2Character::EnableIncarView(const bool bState, const bool bForce)
{
	if ((bState != bInCarCameraActive) || (bForce == true))
	{
		bInCarCameraActive = bState;

		if (bState == true)
		{
			FollowCamera->Deactivate();
			InternalCamera->Activate();
			UE_LOG(LogTemp, Warning, TEXT("InternalCamera"));

		}
		else
		{
			InternalCamera->Deactivate();
			FollowCamera->Activate();
			UE_LOG(LogTemp, Warning, TEXT("FollowCamera"));

		}


	}
}
void AMyProject2Character::Tick(float Delta)
{
	Super::Tick(Delta);

	// controla a camera e tudo da camera 2
	if ((InputComponent) && (bInCarCameraActive == true))
	{

		FRotator HeadRotation = InternalCamera->RelativeRotation;
		HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
		HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
		InternalCamera->RelativeRotation = HeadRotation;
	}
}


void AMyProject2Character::SetColetavelLife(int NewColetavelLife) {
	ColetavelLife = NewColetavelLife;

}
int AMyProject2Character::GetColetavelLife() {
	return ColetavelLife;
}

void AMyProject2Character::OnDeath() {
	if (ColetavelLife <= 0) {
		FVector InitialLocation(-7414.0f, 137.002579f, 226.001587f);
		ColetavelLife = 3;
		SetActorLocation(InitialLocation);
		UE_LOG(LogTemp, Warning, TEXT("Voce morreu!3"));
	}

}


void AMyProject2Character::OnCollect() {
	TArray<AActor*> AtoresColetados;
	CollectCollisionComp->GetOverlappingActors(AtoresColetados);

	for (int i = 0; i < AtoresColetados.Num(); i++) {
		if (AtoresColetados[i]->IsA(AItem::StaticClass())) {
			AItem* ItemColetado = Cast<AItem>(AtoresColetados[i]);
			Inventory.Add(ItemColetado);
			ItemColetado->Destroy();
			UE_LOG(LogTemp, Warning, TEXT("%d"), Inventory.Num());
		}
		else if (AtoresColetados[i]->IsA(AAlavanca::StaticClass())) {
			AAlavanca* Alavanca = Cast<AAlavanca>(AtoresColetados[i]);
			Alavanca->OnPressed();
		}
	}
}