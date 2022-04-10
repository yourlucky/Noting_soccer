/**
 * @file CodeReleaseKickAtGoalCard.cpp
 *
 * This file is just practice
 *
 * @author 
 */

#include "Representations/BehaviorControl/FieldBall.h"
#include "Representations/BehaviorControl/Skills.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Modeling/RobotPose.h"
#include "Tools/BehaviorControl/Framework/Card/Card.h"
#include "Tools/BehaviorControl/Framework/Card/CabslCard.h"
#include "Tools/Math/BHMath.h"


#include "Representations/Communication/RobotInfo.h"

#include "Tools/Module/Blackboard.h"
#include "Representations/Infrastructure/GroundTruthWorldState.h"

//#include "Representations/Sensing/FallDownState.h"

#include <cmath> 

CARD(CodeReleaseKickndribbleCard,
{,
  CALLS(Activity),
  CALLS(InWalkKick),
  CALLS(LookForward),
  CALLS(PathToTarget),
  CALLS(Stand),
  CALLS(WalkAtRelativeSpeed),
  CALLS(WalkToTarget),
  CALLS(Say),
  REQUIRES(FieldBall),
  REQUIRES(FieldDimensions),
  REQUIRES(RobotPose),
  REQUIRES(RobotInfo),
  DEFINES_PARAMETERS(
  {,
    (float)(0.8f) walkSpeed,
    (int)(1000) initialWaitTime,
    (int)(7000) ballNotSeenTimeout,
    (Angle)(5_deg) ballAlignThreshold,
    (float)(500.f) ballNearThreshold,
    (Angle)(10_deg) angleToGoalThreshold,
    (float)(400.f) ballAlignOffsetX,
    (float)(100.f) ballYThreshold,
    (Angle)(2_deg) angleToGoalThresholdPrecise,
    (float)(165.f) ballOffsetX,
    (Rangef)({140.f, 170.f}) ballOffsetXRange,
    (float)(40.f) ballOffsetY,
    (Rangef)({20.f, 50.f}) ballOffsetYRange,
    (int)(10) minKickWaitTime,
    (int)(3000) maxKickWaitTime,
    
    (float)(-0.01f) addAngle,

    (float)(0.f)ball_X,
    (float)(0.f)ball_Y,
  }),
});

class CodeReleaseKickndribbleCard : public CodeReleaseKickndribbleCardBase
{
 

  bool preconditions() const override
  {
    return true;
  }

  bool postconditions() const override
  {
    return true;
  }

  option
  {
    //theActivitySkill(BehaviorStatus::codeReleaseKickndribble);

    initial_state(start)
    {
      transition
      {
        if(state_time > initialWaitTime)
          goto giverole;
      }

      action
      {
        theLookForwardSkill();
        theStandSkill();
      }
    }

    state(giverole)
    {
        transition
        {
          if(theRobotInfo.number == 1)
          {
            int c_time = 0; //current time
            
            if (state_time > c_time + 5000) //if not fallen for 10secs
            {
              c_time = state_time;
              goto walkToBall;
            }

          }  
          else
            goto runner;
          }                     
        action
        {              
           if(theRobotInfo.number == 1)
                theLookForwardSkill();
        }
    }

    state(kicks)
    {
      transition
      {
       if(!theFieldBall.ballWasSeen(ballNotSeenTimeout))
          goto giverole;
      }

      action
      {    
        const GroundTruthWorldState&theGroundTruthWorldState =
        static_cast<const GroundTruthWorldState&>(Blackboard::getInstance()["GroundTruthWorldState"]);
        
        const Pose2f _ownPosition = theGroundTruthWorldState.ownPose;
        const Vector2f _ballPosition = theGroundTruthWorldState.balls[0].position.head<2>();
        
        ballOffsetX =0.5f;
        ballOffsetY= 0.5f;

        Angle v_angle =-0.1*pi;
        theInWalkKickSkill(WalkKickVariant(WalkKicks::forward, Legs::left), Pose2f(v_angle,theFieldBall.positionRelative.x() - ballOffsetX, theFieldBall.positionRelative.y() - ballOffsetY));
      }
    }

    state(runner)
    {
      transition
        {
            //if(!theFieldBall.ballWasSeen(ballNotSeenTimeout))
               //goto giverole;
            
        }
        action
        {
          //theLookForwardSkill();
          walkSpeed =1.f;
          Angle v_angle =0.f*pi;
          theWalkToTargetSkill(Pose2f(walkSpeed, walkSpeed, walkSpeed), Pose2f(v_angle, 3500,0));
        }
    }

    state(walkToBall)
    {
      transition
      {
        if(!theFieldBall.ballWasSeen(ballNotSeenTimeout))
          goto giverole;
        if(ball_X <0.2f && ball_Y < 0.2f)
            goto kicks;       
      }

      action
      {
        theSaySkill("time up");
        
        const GroundTruthWorldState&theGroundTruthWorldState =
        static_cast<const GroundTruthWorldState&>(Blackboard::getInstance()["GroundTruthWorldState"]);
        
        const Pose2f _ownPosition = theGroundTruthWorldState.ownPose;
        const Vector2f _ballPosition = theGroundTruthWorldState.balls[0].position.head<2>();
        const Pose2f _firstteam = theGroundTruthWorldState.firstTeamPlayers[0].pose;
        
        ball_X = std::abs(_ownPosition.translation.x()-_ballPosition(0));
        ball_Y =  std::abs(_ownPosition.translation.y()-_ballPosition(1));
        
        ballOffsetX =0.1f;
        ballOffsetY= 0.1f;

        const Angle v_angle=(theRobotPose.inversePose * Vector2f(_firstteam.translation.x(),_firstteam.translation.y())).angle();
        
        theWalkToTargetSkill(Pose2f(walkSpeed, walkSpeed, walkSpeed), Pose2f(v_angle, theFieldBall.positionRelative.x() - ballOffsetX,theFieldBall.positionRelative.x() - ballOffsetX));
     //Angle v_angle =-0.1*pi;
     // theInWalkKickSkill(WalkKickVariant(WalkKicks::forward, Legs::left), Pose2f(v_angle,0.f,0.f));
      
      }
    }
    





    // state(turnToBall)
    // {
    //   transition
    //   {
    //     if(!theFieldBall.ballWasSeen(ballNotSeenTimeout))
    //       goto searchForBall;
    //     if(std::abs(theFieldBall.positionRelative.angle()) < ballAlignThreshold)
    //       goto walkToBall;
    //   }

    //   action
    //   {
    //     theLookForwardSkill();
    //     theWalkToTargetSkill(Pose2f(walkSpeed, walkSpeed, walkSpeed), Pose2f(theFieldBall.positionRelative.angle(), 0.f, 0.f));
    //     theSaySkill("inital");
    //   }
    // }

    // state(walkToBall)
    // {
    //   transition
    //   {
    //     if(!theFieldBall.ballWasSeen(ballNotSeenTimeout))
    //       goto searchForBall;
    //     if(theFieldBall.positionRelative.squaredNorm() < sqr(ballNearThreshold))
    //       goto alignToGoal;
    //   }

    //   action
    //   {
    //     theLookForwardSkill();
    //     theWalkToTargetSkill(Pose2f(walkSpeed, walkSpeed, walkSpeed), theFieldBall.positionRelative);
    //     theSaySkill("walk to ball");
      
    //   }
    // }

    // state(alignToGoal)
    // {
    //   const Angle angleToGoal = calcAngleToGoal();

    //   transition
    //   {
    //     if(!theFieldBall.ballWasSeen(ballNotSeenTimeout))
    //       goto searchForBall;
    //     if(std::abs(angleToGoal) < angleToGoalThreshold && std::abs(theFieldBall.positionRelative.y()) < ballYThreshold)
    //       goto alignBehindBall;
    //   }

    //   action
    //   {
    //     theLookForwardSkill();
    //     theWalkToTargetSkill(Pose2f(walkSpeed, walkSpeed, walkSpeed), Pose2f(angleToGoal, theFieldBall.positionRelative.x() - ballAlignOffsetX, theFieldBall.positionRelative.y()));
    //     theSaySkill("align to goal");
    //   }
    // }

    // state(alignBehindBall)
    // {
    //   const Angle angleToGoal = calcAngleToGoal();

    //   transition
    //   {
    //     if(!theFieldBall.ballWasSeen(ballNotSeenTimeout))
    //       goto searchForBall;
    //     if(std::abs(angleToGoal) < angleToGoalThresholdPrecise && ballOffsetXRange.isInside(theFieldBall.positionRelative.x()) && ballOffsetYRange.isInside(theFieldBall.positionRelative.y()))
    //       goto kick;
    //   }

    //   action
    //   {
    //     theLookForwardSkill();
    //     theWalkToTargetSkill(Pose2f(walkSpeed, walkSpeed, walkSpeed), Pose2f(angleToGoal, theFieldBall.positionRelative.x() - ballOffsetX, theFieldBall.positionRelative.y() - ballOffsetY));
    //     theSaySkill("align behind ball");
    //   }
    // }

    // state(kick)
    // {
    //   const Angle angleToGoal = calcAngleToGoal();

    //   transition
    //   {
    //     if(state_time > maxKickWaitTime || (state_time > minKickWaitTime && theInWalkKickSkill.isDone()))
    //       goto start;
    //   }

    //   action
    //   {
    //     theLookForwardSkill();
    //     theInWalkKickSkill(WalkKickVariant(WalkKicks::forward, Legs::left), Pose2f(angleToGoal, theFieldBall.positionRelative.x() - ballOffsetX, theFieldBall.positionRelative.y() - ballOffsetY));
    //     theSaySkill("kick");
    //   }
    // }

    // state(searchForBall)
    // {
    //   transition
    //   {
    //     if(theFieldBall.ballWasSeen())
    //       goto turnToBall;
    //   }

    //   action
    //   {
    //     theLookForwardSkill();
    //     theWalkAtRelativeSpeedSkill(Pose2f(walkSpeed, 0.f, 0.f));
    //     theSaySkill("search for ball");
    //   }
    // }
  }

  // Angle calcAngleToGoal() const
  // {
  //   return (theRobotPose.inversePose * Vector2f(theFieldDimensions.xPosOpponentGroundline, 0.f)).angle()*(1/10)+addAngle;
  // }

  
  
};

MAKE_CARD(CodeReleaseKickndribbleCard);