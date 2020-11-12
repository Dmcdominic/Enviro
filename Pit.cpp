#include "Pit.hpp"
#include "Player.hpp"


bool Pit::can_fg_obj_move_into(FgObj& objBeingMoved, const glm::ivec2& displ) {
  if (dynamic_cast<Player*>(&objBeingMoved) != nullptr) return false;
  return BgTile::can_fg_obj_move_into(objBeingMoved, displ);
}