// Add remote Hookshot to OoT

// 1) Replace
// Player_UpdateBottleHeld(play, this, ITEM_BOTTLE_EMPTY, PLAYER_IA_BOTTLE);
// in z_player.c (Player_Action_DrinkBottle) with:

Player_UpdateBottleHeld(play, this, (this->itemAction == PLAYER_IA_BOTTLE_MILK_FULL) ?
                                    ITEM_BOTTLE_MILK_HALF : ITEM_BOTTLE_EMPTY,
                                    PLAYER_IA_BOTTLE);

// 2) Replace
// if ((this->heldItemAction == PLAYER_IA_HOOKSHOT) || (this->heldItemAction == PLAYER_IA_LONGSHOT))
// in z_player_lib.c (Player_PostLimbDrawGameplay) with:

if ((this->rightHandType == PLAYER_MODELTYPE_RH_HOOKSHOT))

// Both are changes made in MM.
// 1) makes Link hold the half milk bottle after drinking full milk if an empty bottle is equipped.
// 2) makes so that X rotation of the Hookshot stops updating when starting drinking,
// because the modeltype has changed.
// In OoT, the rotation will update as long as the held IA has not been changed.
// This causes the rotation to stop updating at a downward angle.
// It is still remote hookshot, but very difficult to use.