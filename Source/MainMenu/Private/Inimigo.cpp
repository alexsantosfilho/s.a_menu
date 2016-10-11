// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "Inimigo.h"
#include "MyProject2Character.h"
#include "ProjectActor.h"

// Sets default values
AInimigo::AInimigo()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	Root->bGenerateOverlapEvents = true;
	Root->SetCollisionProfileName("OverlapAllDynamic");
	Root->OnComponentBeginOverlap.AddDynamic(this, &AInimigo::OnOverlapBegin);
	RootComponent = Root;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName("NoCollision");
	MeshComp->AttachTo(RootComponent);

}

// Called when the game starts or when spawned
void AInimigo::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AInimigo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector LocalizacaoAtual = GetActorLocation();
	float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
	LocalizacaoAtual.Y += DeltaHeight * 400.0f;
	RunningTime += DeltaTime;
	SetActorLocation(LocalizacaoAtual);


}

void AInimigo::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor->IsA(AProjectActor::StaticClass())) {

		Destroy();
		UE_LOG(LogTemp, Warning, TEXT("destruiu a caixa"));
	}
}

//void ATeste::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
//if (OtherActor && (OtherActor != this) && OtherComp)
//{
//OtherComp->AddImpulseAtLocation(AMyActor->Velocity * 100.0f, Hit.ImpactPoint);
//}
//}