#include "Mode.hpp"

std::shared_ptr< Mode > Mode::current;
std::shared_ptr< Mode > Mode::demo;
std::shared_ptr< Mode > Mode::play;

void Mode::set_current(std::shared_ptr< Mode > const &new_current) {
	current = new_current;
	//NOTE: may wish to, e.g., trigger resize events on new current mode.
}

void Mode::set_demo(std::shared_ptr< Mode > const &d) {
    demo = d;
    current = demo;
}

void Mode::set_play(std::shared_ptr< Mode > const &p) {
    play = p;
    current = play;
}

void Mode::switch_to_play() {
    current = play;
}