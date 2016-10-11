// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "InimigoC.h"
#include "MyProject2Character.h"
#include "AIPatrol.h"



// Sets default values
AInimigoC::AInimigoC()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	Root->SetCollisionProfileName("OverlapAllDynamic");
	Root->bGenerateOverlapEvents = true;
	Root->OnComponentBeginOverlap.AddDynamic(this, &AInimigoC::OnOverlapBegin);

	RootComponent = Root;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName("NoCollision");
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		Mesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));
	if (Mesh.Succeeded()) {
		MeshComp->SetStaticMesh(Mesh.Object);
	}
	MeshComp->SetWorldLocation(FVector(0.0f, 0.0f, -30.0f));
	MeshComp->SetWorldScale3D(FVector(2.5f, 2.5f, 2.5f));
	MeshComp->AttachTo(RootComponent);

	Particle = CreateDefaultSubobject<UParticleSystemComponent>
		(TEXT("Particle"));
	Particle->bAutoActivate = true;
	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		ParticleSystem(TEXT("ParticleSystem'/Game/particulas/P_Fire.P_Fire'"));
	if (ParticleSystem.Succeeded()) {
		Particle->SetTemplate(ParticleSystem.Object);
	}
	Particle->SetupAttachment(RootComponent);
	//Particle->ToggleActive();

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Root;
	ProjectileMovement->InitialSpeed = 1000.0f;
	ProjectileMovement->MaxSpeed = 1000.0f;

	InitialLifeSpan = 1.0f;
}

// Called when the game starts or when spawned
void AInimigoC::BeginPlay()
{
	Super::BeginPlay();

	DefaultX = GetActorLocation().X;
}

// Called every frame
void AInimigoC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Location = GetActorLocation();
	Location.X = DefaultX;
	SetActorLocation(Location);

	RunningTime += DeltaTime;
	float DestroyTime = 0.8f * RunningTime;
	if (DestroyTime > 0.9f) {
		Destroy();
	}
}

void AInimigoC::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if ((OtherActor != nullptr) && (OtherActor != this) &&
		(OtherComp != nullptr) && (OtherActor->IsA(AMyProject2Character::StaticClass()))) {

		AMyProject2Character* MyProject2Character = Cast<AMyProject2Character>(OtherActor);
		MyProject2Character->SetColetavelLife(MyProject2Character->GetColetavelLife() - DamageAmount);
		MyProject2Character->OnDeath();
		UE_LOG(LogTemp, Warning, TEXT("Life = %d"), MyProject2Character->GetColetavelLife());

		Destroy();

		UE_LOG(LogTemp, Warning, TEXT("Encostou"));
	}

}