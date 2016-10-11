// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "ProjectActor.h"
#include "MyProject2Character.h"
#include "AIPatrol.h"



// Sets default values
AProjectActor::AProjectActor()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	Root->SetCollisionProfileName("OverlapAllDynamic");
	Root->bGenerateOverlapEvents = true;
	Root->OnComponentBeginOverlap.AddDynamic(this, &AProjectActor::OnOverlapBegin);



	//	this->OnActorHit.AddDynamic(this, &AProjectActor::OnHit);

	RootComponent = Root;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName("NoCollision");
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		Mesh(TEXT("StaticMesh'/Game/Shape_WideCapsule.Shape_WideCapsule'"));
	if (Mesh.Succeeded()) {
		MeshComp->SetStaticMesh(Mesh.Object);
	}
	MeshComp->SetWorldLocation(FVector(0.0f, 0.0f, -30.0f));
	MeshComp->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
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
	ProjectileMovement->InitialSpeed = 6000.0f;
	ProjectileMovement->MaxSpeed = 6000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	InitialLifeSpan = 1.0f;
}
void AProjectActor::BeginPlay()
{
	Super::BeginPlay();

	DefaultY = GetActorLocation().Y;
	DefaultX = GetActorLocation().X;
	DefaultZ = GetActorLocation().Z;



}

void AProjectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// novo

	FHitResult HitResult;

	FVector StartTrace = this->GetActorLocation();
	FVector EndTrace = (Velocity * DeltaTime) + StartTrace;
	EndTrace.Y += this->GetActorRotation().Pitch;
	//	EndTrace.Y += this->GetActorRotation().Yaw;
	//	EndTrace.X += this->GetActorRotation().Roll;




	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);


	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Destructible, CollisionParams))
	{

		if (HitResult.GetActor())
		{
			DrawDebugSolidBox(GetWorld(), HitResult.ImpactPoint, FVector(10.f), FColor::Blue, true);
			ADestructibleActor* Mesh = Cast<ADestructibleActor>(HitResult.GetActor());

			if (Mesh) {

				Mesh->GetDestructibleComponent()->ApplyRadiusDamage(10.f, HitResult.ImpactPoint, 32.f, 10.f, false);

			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" COULD NOT GET MESH. Type is %s"), *HitResult.GetActor()->StaticClass()->GetFName().ToString()));
			}

		}
		Destroy();

	}
	else
	{
		BulletExpiry += DeltaTime;

		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(0.0f, -BulletExpiry * 80.f, 100.f), true);

		SetActorLocation(EndTrace);

		//	Velocity += FVector(0.f, 0.f, -2000.f) * DeltaTime;
	}

	if (BulletExpiry > 3) {

		Destroy();
	}

	//




	RunningTime += DeltaTime;
	float DestroyTime = 1.0f * RunningTime;
	if (DestroyTime > 10.0f) {
		Destroy();
	}
}


void AProjectActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	//	if ((OtherActor != nullptr) && (OtherActor != this) &&
	//	(OtherComp != nullptr) && (OtherActor->IsA(AProjectActor::StaticClass()))) {

	//	AAIPatrol* AIPatrol = Cast<AAIPatrol>(OtherActor);
	//	ProjectActor->Destroy();

	//	UE_LOG(LogTemp, Warning, TEXT("Destruiu a balaaa!!!!!!"));

	//}


	if ((OtherActor != nullptr) && (OtherActor != this) &&
		(OtherComp != nullptr) && (OtherActor->IsA(AAIPatrol::StaticClass()))) {

		AAIPatrol* MyProject2Character = Cast<AAIPatrol>(OtherActor);
		MyProject2Character->SetColetavelLife2(MyProject2Character->GetColetavelLife2() - DamageAmount); // DANO NO PERSONAGEM
		MyProject2Character->OnDeath2();

		UE_LOG(LogTemp, Warning, TEXT("Destruiu o personagem inimigo!!!!!!"));



	}

}



