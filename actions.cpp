#include "Actions.h"


void MoveAction::Start()
{
	IntervalAction::Start();

	actor->pose.SetState(Pose::Walking);
}

void MoveAction::Proceed(float percentage_done)
{
	IntervalAction::Proceed(percentage_done);

	Vec3f source_position= move->GetSource()->GetPosition();
	Vec3f destination_position= move->GetDestination()->GetPosition();

	actor->position= source_position* (1- percentage_done)+ destination_position* percentage_done;

	float rotation= acos((Vec2f(destination_position.x, destination_position.z)- Vec2f(source_position.x, source_position.z)).Normalize().Dot(Vec2f(0, 1)));
	actor->rotation= destination_position.x< source_position.x ? (float)(2* M_PI- rotation) : rotation;
}

void MoveAction::Finish()
{
	IntervalAction::Finish();

	actor->pose.SetState(Pose::Standing);
	move->GetDestination()->PutActor(move->GetSource()->RemoveActor());
}

MoveAction::MoveAction(Actor *actor, Move *move_)
	: IntervalAction(actor, SecondsToChronons(move_->GetCost()* 1))
{
	move= move_;
}

string MoveAction::GetName()
{
	return "walk";
}

float MoveAction::GetDimensionalProgress()
{
	return GetProgress()* move->GetSource()->GetPosition().Distance(move->GetDestination()->GetPosition());
}
