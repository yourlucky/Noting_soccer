/**
 * @file CodeReleaseKickAtGoalCard.cpp
 *
 * This file is just practice
 *
 * @author 
 */

#include "Representations/BehaviorControl/Skills.h"
#include "Tools/BehaviorControl/Framework/Card/Card.h"


CARD(CodeReleaseOwnKickoffCard,
 {,
  CALLS(Activity),
  CALLS(LookForward),
  CALLS(Say),
  CALLS(Stand),
});

class CodeReleaseOwnKickoffCard : public CodeReleaseOwnKickoffCardBase
{
  bool preconditions() const override
  {
    return true;
  }

  bool postconditions() const override
  {
    return true;
  }

  void execute() override
  {
    //theActivitySkill(BehaviorStatus::codeReleasePositionForKickOff);
    //theActivitySkill(BehaviorStatus::codeReleaseOwnKickoffCard);
    //theLookForwardSkill();
    //theStandSkill();
    // Not implemented in the Code Release.
    theSaySkill("yes Goal Own Kick off card");
  }
};

MAKE_CARD(CodeReleaseOwnKickoffCard);
