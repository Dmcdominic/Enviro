#include "Player.hpp"
#include <iostream>


Scene::Drawable* Player::prefab;

// For now, objects can't move into the player's cell
bool Player::can_fg_obj_move_into(const FgObj& objBeingMoved, const glm::ivec2& displ) {
  return false;
}

// For now, does not apply
void Player::when_fg_obj_moved_into(FgObj& objBeingMoved, const glm::ivec2& displ) {
}

// Sky objects can move over the player
bool Player::can_sky_obj_move_into(const SkyObj& objBeingMoved, const glm::ivec2& displ) {
  return true;
}

// Nothing happens
void Player::when_sky_obj_moved_into(SkyObj& objBeingMoved, const glm::ivec2& displ) {
}

// Controls the player's movement.
// If up/down/left/right is pressed, move in the grid.
bool Player::on_input(const Input& input) {
  glm::ivec2 displ;
  switch (input.type) {
    case InputType::UP:
      displ = glm::ivec2( 0,  1);
      break;
    case InputType::DOWN:
      displ = glm::ivec2( 0, -1);
      break;
    case InputType::LEFT:
      displ = glm::ivec2(-1,  0);
      break;
    case InputType::RIGHT:
      displ = glm::ivec2( 1,  0);
      break;
    default:
      return false;
  }

  glm::ivec2 target_pos = this->cell->pos + displ;
  if (!current_grid->is_valid_pos(target_pos)) return false;
  Cell* target_cell = &current_grid->cells.at(target_pos.x).at(target_pos.y);
  if (!target_cell->can_fg_obj_move_into(*this, displ)) return false;
  target_cell->when_fg_obj_moved_into(*this, displ);
  target_cell->set_fg_obj(this);

  return true;
}