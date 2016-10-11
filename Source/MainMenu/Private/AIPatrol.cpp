// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "AIPatrol.h"
#include "AIPatrolController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"
#include "MyProject2Character.h"
#include "ProjectActor.h"
#include "Inimigo.h"
#include "InimigoC.h"
//#include "MyProject2TextCharacter"




// Sets default values
AAIPatrol::AAIPatrol()
{
	// Initalize Senses

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(90.f);

	PrimaryActorTick.bCanEverTick = true;


	MyText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("RotatingText"));
	MyText->SetWorldSize(70.f);
	MyText->SetTextRenderColor(FColor::Blue);
	//MyText->SetHorizontalAlignment(EHTA_Center);



	//	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	//Mesh->SetCollisionProfileName("NoCollision");
	//	Mesh->AttachTo(RootComponent);
	//	Mesh->SetCollisionProfileName("OverlapAllDynamic");
	//	Mesh->bGenerateOverlapEvents = true;
	//Mesh->OnComponentBeginOverlap.AddDynamic(this, &AAIPatrol::OnOverlapBegin);

	GetCharacterMovement()->MaxWalkSpeed = 180.f;



}

// Called when the game starts or when spawned
void AAIPatrol::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensingComp) {
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AAIPatrol::OnPlayerCaught);


	}
}



// Called to bind functionality to input
void AAIPatrol::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AAIPatrol::OnPlayerCaught(APawn * Pawn)
{

	AAIPatrolController* AIController = Cast<AAIPatrolController>(GetController());


	if (AIController)

	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("vc esta sendo seguido"));
		AIController->SetPlayerCaught(Pawn);
	}
}

void AAIPatrol::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if ((OtherActor != nullptr) && (OtherActor != this) &&
		(OtherComp != nullptr) && (OtherActor->IsA(AMyProject2Character::StaticClass()))) {

		AMyProject2Character* MyProject2Character = Cast<AMyProject2Character>(OtherActor);
		MyProject2Character->SetColetavelLife(MyProject2Character->GetColetavelLife() - DamageAmount);
		MyProject2Character->OnDeath();
		UE_LOG(LogTemp, Warning, TEXT("Life = %d"), MyProject2Character->GetColetavelLife());

		Destroy();

		UE_LOG(LogTemp, Warning, TEXT("inimigo encostou no persobagem"));
	}



	if ((OtherActor != nullptr) && (OtherActor != this) &&
		(OtherComp != nullptr) && (OtherActor->IsA(AProjectActor::StaticClass()))) {

		AProjectActor* MyProject2Character = Cast<AProjectActor>(OtherActor);

		UE_LOG(LogTemp, Warning, TEXT("Destruiu Parabens o personagem"));
		Destroy();

	}
	if ((OtherActor != nullptr) && (OtherActor != this) &&
		(OtherComp != nullptr) && (OtherActor->IsA(AMyProject2Character::StaticClass()))) {

		AMyProject2Character* AIPatrol = Cast<AMyProject2Character>(OtherActor);

		UE_LOG(LogTemp, Warning, TEXT("Destruiu Parabens o personagem"));
		AIPatrol->Destroy();
		Destroy();


		//UE_LOG(LogTemp, Warning, TEXT("Encostou1"));
	}




}

//void AAIPatrol::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
//if	((OtherActor != nullptr) && (OtherActor != this) &&
//(OtherComp != nullptr) && (OtherActor->IsA(AAIPatrol::StaticClass())))
//{

//	Destroy();
//	UE_LOG(LogTemp, Warning, TEXT("OnHit"));
//}
//}


void AAIPatrol::SetColetavelLife2(int NewColetavelLife2) {
	ColetavelLife2 = NewColetavelLife2;

}
int AAIPatrol::GetColetavelLife2() {
	return ColetavelLife2;
}

void AAIPatrol::OnDeath2() {
	if (ColetavelLife2 <= 0) {
		FVector InitialLocation(-3640.0f, -40.002f, 218.0012f);
		ColetavelLife2 = 3;
		SetActorLocation(InitialLocation);
		UE_LOG(LogTemp, Warning, TEXT("Voce morreu2!"));
	}

}

void AAIPatrol::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	MakeTextFacePlayer();



	RunningTime += DeltaTime;
	float ShootTime = 1.0f * RunningTime;
	if (ShootTime > 2.0f) {
		FActorSpawnParameters SpawnParameters;
		UWorld* World = GetWorld();
		if (World != nullptr) {
			//FRotator Rotation = Mesh->GetComponentRotation();
			//		AInimigoC* Proj = World->SpawnActor<AInimigoC>
			//	(GetActorLocation(), Rotation,
			//	SpawnParameters);
			//	if (Proj != nullptr) {

			//	RunningTime = 0.0f;
			//	}

		}

	}
}

void AAIPatrol::MakeTextFacePlayer() {

	//ARelativeTextCharacter* Character = Cast<ARelativeTextCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	// FRotation NewRotation = char
}