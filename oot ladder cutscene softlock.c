// When entering a cutscene during ladder dismount, player is softlocked.
// The cutscene state cannot be cleared.
// This is because:
// - Player is in an action where movement is decided by animation and not by usual functions for updating
// position and velocity (ANIM_FLAG_OVERRIDE_MOVEMENT is set). In ladder dismount, player cannot do inputs,
// movement is purely animation driven. If the animation is stopped, player cannot do anything.
// - Slightly before end frame of dismount animation (and action switch), action handlers are called as
// part of try action interrupt, according to the idle list (starts with 0).
// - Because cutscenes set unk_6AD (special camera flag) to 3, action handler 0 will always call handler
// 13 and return 1 - even though handler 13 cannot start a CS action, because player is not on the ground.
// - This action interrupt causes the climbing flag to get removed and action execution ends with return,
// before the animation is updated.
// - When cutscene is finished, PLAYER_CSACTION_7 is set as csAction to clear the cutscene state from player.
// - However, because player no longer is in climbing state, func_80852944 (basically CS action 7) cannot
// be called through Player_UpdateCommon (which will instead keep setting unk_6AD to 3).
// - Because player is not on the ground, action handler 13 cannot start a CS action to execute CS action 7.
// - Every time the dismount ladder action is run, it is cut short because of interrupt, before animation.
// - Player doesn't fall due to animation override.
// - Player is softlocked because the animation cannot finish to set a new action, and player cannot do
// anything that would let PLAYER_CSACTION_7 run such as get onto ground/water, into climb state (or the
// other states that would lead to func_80852944).

// Fix: Can be approached in several ways, but simply changing the interrupt check to
// if (interruptResult == PLAYER_INTERRUPT_NEW_ACTION && this->unk_6AD != 3)
// retains all original function while only preventing interrupt in the very rare case of active cutscene.
// As it lets the animation play fully, it also looks natural.

void Player_Action_DismountLadder(Player* this, PlayState* play) {
    ...

    interruptResult = Player_TryActionInterrupt(play, this, &this->skelAnime, 4.0f);

    //! @bug Ladder dismount cutscene softlock. Player_TryActionInterrupt will always return true in cutscene
    //! as AH 0 calls AH 13 (due to unk_6AD > 0) and then returns true. This both removes PLAYER_STATE1_CLIMBING
    //! below, which causes Player_UpdateCommon to continuously set unk_6AD to 3, and causes the final
    //! LinkAnimation_Update (that normally exits the action and sets idle action) to never run.
    //! Adding `&& this->unk_6AD != 3` prevents this softlock, as the animation update can play.
    if (interruptResult == PLAYER_INTERRUPT_NEW_ACTION) {
        // This causes Player_UpdateCommon to keep setting unk_6AD = 3
        this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING;
        return;
    }

    // Animation is updated here, but function doesn't run this far after 4.0 frames before endframe
    if ((interruptResult >= PLAYER_INTERRUPT_MOVE) || LinkAnimation_Update(play, &this->skelAnime)) {
        Player_SetupIdlePlayOnce(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_CLIMBING;
        return;
    }
    ...
}

void Player_UpdateCommon(Player* this, PlayState* play, Input* input) {
    ...
    if (this->csAction != PLAYER_CSACTION_NONE) {
        // Player dismounting ladder is in climbing state until slightly before end dismount
        // and could have entered into func_80852944 if hadn't been removed
        if ((this->csAction != PLAYER_CSACTION_7) ||
            !(this->stateFlags1 & (PLAYER_STATE1_HANGING | PLAYER_STATE1_CLIMB_JUMP_UP | PLAYER_STATE1_CLIMBING | PLAYER_STATE1_KNOCKBACK_FROZEN))) {
            this->unk_6AD = 3;
        } else if (Player_Action_CsAction != this->actionFunc) {
            func_80852944(play, this, NULL);
        }
    } else {
        this->prevCsAction = PLAYER_CSACTION_NONE;
    }
    ...
}

s32 Player_ActionHandler_13(Player* this, PlayState* play) {
    ...
    // This lets player who is swimming or riding start CS action
    if ((this->unk_6AD != 0) &&
        (Player_SwimmingWithoutIronBoots(this) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
         (this->stateFlags1 & PLAYER_STATE1_RIDING))) {

        if (!Player_StartCsAction(play, this)) {
            ...
        }
    }
}

