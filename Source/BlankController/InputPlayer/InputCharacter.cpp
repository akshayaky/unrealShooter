// Fill out your copyright notice in the Description page of Project Settings.
#include "BlankController/InputPlayer/InputCharacter.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include <GameFramework/SpringArmComponent.h>
#include "GameFramework/CharacterMovementComponent.h"
#include <BlankController/Gun/Gun.h>
// Sets default values
AInputCharacter::AInputCharacter()
{

	SpringArmCreated = CreateDefaultSubobject<USpringArmComponent>("Spring Arm(Created)");
	SpringArmCreated->SetupAttachment(RootComponent);
	SpringArmCreated->bUsePawnControlRotation = true;
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);
	//Camera->bUsePawnControlRotation = true;

	///*USpringArmComponent* SpringArm = this->FindComponentByClass<USpringArmComponent>();
	Camera->SetupAttachment(SpringArmCreated);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	 
}

// Called when the game starts or when spawned
void AInputCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->SpawnActor<AGun>();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called every frame
void AInputCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AInputCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		//Get Local Player subsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//Add input Context
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
	
	if(UEnhancedInputComponent * Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInputCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInputCharacter::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AInputCharacter::Jump);
		Input->BindAction(RunAction, ETriggerEvent::Started, this, &AInputCharacter::StartRunning);
		Input->BindAction(RunAction, ETriggerEvent::Completed, this, &AInputCharacter::StopRunning);
		Input->BindAction(SwitchViewAction, ETriggerEvent::Completed, this, &AInputCharacter::StopRunning);
	}

}

void AInputCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();
	if (IsValid(Controller))
	{
		//Get Forward Direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//Add Movement inout
		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

void AInputCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();
	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
	}
}

void AInputCharacter::Jump()
{
	ACharacter::Jump();
}

void AInputCharacter::StartRunning(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AInputCharacter::StopRunning(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AInputCharacter::SwitchView(const FInputActionValue& Value)
{
	if (FirstPersonCamera->IsActive())
	{
		FirstPersonCamera->Deactivate();
		ThirdPersonCamera_->Activate();
	}
	else
	{
		FirstPersonCamera->Activate();
		ThirdPersonCamera_->Deactivate();

	}
}

