#include "Actions.h"

void MoveAction::Start()
{
	actor->pose.SetState(Pose::Walking);
}

void MoveAction::Proceed(float percentage_done)
{
	actor->position= move->GetSource()->GetPosition()* (1- percentage_done)+ move->GetDestination()->GetPosition()* percentage_done;
}

void MoveAction::Finish()
{
	actor->pose.SetState(Pose::Standing);
	move->GetDestination()->PutActor(move->GetSource()->RemoveActor());
}

MoveAction::MoveAction(Actor *actor, Move *move_)
	: IntervalAction(actor, SecondsToChronons(move_->GetCost()* 50))
{
	move= move_;
}
