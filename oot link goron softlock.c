// z_en_go2

void func_80A45288(EnGo2* this, PlayState* play) {
    ...

    //! @bug Link Goron softlock. func_80A44790 calls UpdateTalkState. If player starts talking to Link Goron
    //! before getting Goron Tunic and goes outside of the talk range, the Goron will curl up, which sets isAwake
    //! to false, and the text state will stop updating.
    //! The text state must be set correctly for the choice textbox to work, which causes a softlock.
    //! This can be fixed by adding the condition `|| this->interactInfo.talkState == NPC_TALK_STATE_TALKING` here
    //! (to let text state continue updating) or `&& this->interactInfo.talkState != NPC_TALK_STATE_TALKING` in
    //! func_80A46B40 (to prevent curling up in the first place).
    if ((this->actionFunc != EnGo2_SetGetItem) && (this->isAwake == true)) {
        if (func_80A44790(this, play)) {
            EnGo2_BiggoronSetTextId(this, play, player);
        }
    }
}


void func_80A46B40(EnGo2* this, PlayState* play) {
    ...

    // Here `&& this->interactInfo.talkState != NPC_TALK_STATE_TALKING` can be added to prevent
    // curling up when talking
    if ((!EnGo2_IsCameraModified(this, play)) && (!EnGo2_IsWakingUp(this))) {
        EnGo2_RollingAnimation(this, play);
    }
}