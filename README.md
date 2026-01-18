# Tiny Ecosystem Clicker

A small clicker+simulator game written in C++ using Raylib.

## Build

To compile this project, make sure you have the following dependencies installed:

- Raylib
- Git
- Make

Then clone this repository and build the project by running:

    make

## Features

- Aquariums - containers for building small ecosystems.
- Algae - food source, floats in the water.
- Soil, gravel and sand - their particles fall to the bottom of the aquarium forming a substrate.
- Ostracod - moves in the water in search of food (algae). Can die from starvation.
- Seed - falls to the bottom of the aquarium or onto the substrate and grows into a simple plant.

## Graphical interface

At the top of the game window are action buttons. Each button represents an action that will be performed when you click on the game area.

Available actions:
1. Build/expand aquarium
2. Add water to the aquarium
3. Remove water from the aquarium
4. Throw algae into the aquarium
5. Throw soil
6. Throw gravel
7. Throw sand
8. Throw an ostracod
9. Throw a seed

Below the action buttons is an experience bar that shows your current level and the leveling progress.

## How to play

Steps:
1. Select the first action (aquarium) using the buttons at the top of the game window.
2. Move mouse to the game area (everything below the experience bar) and click the left mouse button - now you have a small aquarium!
3. You can click on the edges of the aquarium to expand it.
4. Select any other action button.
5. Click the left mouse button above or inside the aquarium. Each click performs the selected action.
6. You can switch actions at any time.

Final step: Enjoy your aquarium!

## Screenshots

<br>
<img src="screenshots/showcase.png" width="700"/>
<br>