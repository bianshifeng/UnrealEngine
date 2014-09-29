// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AIModulePrivate.h"
#include "Perception/AISense_Prediction.h"

UAISense_Prediction::UAISense_Prediction(const class FPostConstructInitializeProperties& PCIP) :
	Super(PCIP)
{
}

float UAISense_Prediction::Update()
{
	AIPerception::FListenerMap& ListenersMap = *GetListeners();

	for (int32 EventIndex = 0; EventIndex < RegisteredEvents.Num(); ++EventIndex)
	{
		const FAIPredictionEvent& Event = RegisteredEvents[EventIndex];

		if (Event.Requestor != NULL && Event.PredictedActor != NULL)
		{
			IAIPerceptionListenerInterface* PerceptionListener = Cast<IAIPerceptionListenerInterface>(Event.Requestor);
			if (PerceptionListener != NULL)
			{
				UAIPerceptionComponent* PerceptionComponent = PerceptionListener->GetPerceptionComponent();
				if (PerceptionComponent != NULL && ListenersMap.Contains(PerceptionComponent->GetListenerId()))
				{
					// this has to succeed, will assert a failure
					FPerceptionListener& Listener = ListenersMap[PerceptionComponent->GetListenerId()];

					// calculate the prediction here:
					const FVector PredictedLocation = Event.PredictedActor->GetActorLocation() + Event.PredictedActor->GetVelocity() * Event.TimeToPredict;

					Listener.RegisterStimulus(Event.PredictedActor, FAIStimulus(GetSenseIndex(), 1.f, PredictedLocation, Listener.CachedLocation));
				}
			}
		}
	}

	RegisteredEvents.Reset();

	// return decides when next tick is going to happen
	return SuspendNextUpdate;
}

void UAISense_Prediction::RegisterEvent(const FAIPredictionEvent& Event)
{
	RegisteredEvents.Add(Event);
	RequestImmediateUpdate();
}