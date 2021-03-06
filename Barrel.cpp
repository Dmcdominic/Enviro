#include "Barrel.hpp"
#include <iostream>

Barrel::Barrel(Scene *scene, int _rotations) : FgObj(scene, _rotations) {}


// Load all drawables
void Barrel::load_models(Scene* scene) {
  scene->drawables.push_back(model_loader->create_model(get_model_name()));
  this->drawable = &(scene->drawables.back());
};


// Create a copy with no drawables
Barrel* Barrel::clone_lightweight(Cell* new_cell) {
  Barrel* new_barrel = new Barrel(*this);
  new_barrel->cell = new_cell;
  new_barrel->drawable = nullptr;
  new_barrel->extra_drawables.clear();
  new_barrel->saved_target_cell = nullptr;
  return new_barrel;
}


// The barrel can only roll along its long axis.
bool Barrel::can_fg_obj_move_into(FgObj& objBeingMoved, const glm::ivec2& displ) {
  // First check if displ is along the right axis for the barrel to roll.
  if (rolling ||
      ( rollable_up_down() && displ.x != 0) ||
      (!rollable_up_down() && displ.y != 0)) {
    return false;
  }
  // If that's all good, then use default pushable behavior.
  if (FgObj::can_fg_obj_move_into(objBeingMoved, displ)) {
	  push_move_clip();
	  rolling = true;
	  current_grid->rolling = true;
	  current_grid->post_tick_queued = true;
	  glm::ivec2 target_pos = this->cell->pos + displ;
	  if (!current_grid->is_valid_pos(target_pos)) return false;
	  Cell* target_cell = current_grid->cell_at(target_pos);
	  saved_target_cell = target_cell;
	  saved_displ = displ;
	  // so the object before the barrel won't complain
	  this->cell->fgObj = nullptr;
	  this->cell = nullptr;
	  return true;
  } else {
  	return false;
  }
}

void Barrel::roll() {
	drawable->transform->position.x += saved_displ.x * displ_step;
	drawable->transform->position.y += saved_displ.y * displ_step;
	float theta = displ_amt * 3.1415926f/2.0f;
	drawable->transform->rotation *= glm::quat(0.0f, 0.0f, sin(theta/2.0f), cos(theta/2.0f));
	displ_amt += displ_step;
	if (displ_amt >= 1.0f) {
		rolling = false;
		current_grid->rolling = false;
		displ_amt = 0;
		if (saved_target_cell == nullptr) {
			throw std::runtime_error("Previously saved target cell of the rolling barrel is somehow a nullptr");
		}
		saved_target_cell->when_fg_obj_moved_into(*this, saved_displ);
		if (saved_target_cell->fgObj != nullptr) {
			throw std::runtime_error("Trying to move barrel into a cell that seems to still have one");
		}
//		this->cell->fgObj = nullptr;
		saved_target_cell->set_fg_obj(this);
		drawable->transform->rotation *= glm::quat(0.0f, 0.0f, sin(theta / 2.0f), cos(theta / 2.0f));

	}
}

// Does whatever should happen when the given foreground object is moved/pushed into this object.
void Barrel::when_fg_obj_moved_into(FgObj& objBeingMoved, const glm::ivec2& displ) {
	glm::ivec2 norm_displ = Grid::normalize_displ(displ);
	// throw error if barrel is not supposed to be rolling
	if (!try_to_move_by(norm_displ) && displ_amt >= 1.0f) {
		throw std::runtime_error("when_fg_obj_moved_into() somehow called for an object position & displacement that COULDN'T move.");
	}
}

bool Barrel::try_to_move_by(const glm::ivec2 &displ) {
	saved_displ = displ;
	glm::ivec2 target_pos = this->cell->pos + displ;
	if (!current_grid->is_valid_pos(target_pos)) return false;
	Cell* target_cell = current_grid->cell_at(target_pos);
	saved_target_cell = target_cell;
	if (!target_cell->can_fg_obj_move_into(*this, displ)) return false;
	// so the object before the barrel won't complain
	this->cell->fgObj = nullptr;
	this->cell = nullptr;
	return true;
}


// Call barrel->roll() on each barrel that is rolling.
//   Return true iff there is at least 1 such barrel.
bool Barrel::roll_rolling_barrels(Grid* grid) {
	bool found_one = false;
	for (Barrel* barrel : grid->barrels) {
		if (barrel->rolling) {
			barrel->roll();
			found_one = true;
		}
	}
	return found_one;
}


// Returns the audio clip that should be played when this object is moved
std::optional<AudioManager::AudioClip> Barrel::get_move_clip() {
  return std::optional(AudioManager::AudioClip::ROLL_BARREL);
}


// True = rollable_up_down, False = rollable left/right
bool Barrel::rollable_up_down() {
  return rotations % 2 == 0;
}
