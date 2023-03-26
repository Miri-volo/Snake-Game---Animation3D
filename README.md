# Snake Game - Animation Course Project

# Additional Features

Apart from the required features, we have also implemented the following:

1. Non-linear Level System.
2. Animation Composition.
3. Bezier Curve Animations of Any Degree.
4. Texturing and Texture Coordinates.
5. Special Snakey Locomotion.
6. Snake Self Collision Checking.
7. Custom Snake Model.
8. Music.
9. Fixed CMake Build.

## Non-linear Level System

Created a non-linear levelling system with `Scene` switching.
Said system allows playing the same level multiple times, going back levels or even creating multiple paths in the levels graph to winning the game.

Getting `Scene` switching to work without use after free crashes was a major pain.

The `SceneSwitcher` class employes the abstract factory design pattern internally for creating scenes.

## Animation Composition

Animation composition system for applying multiple animations to models.
This animation system allows chaining multiple animations together, and even chaining composed animations with other animations.

This system employs the Composable design pattern.

## Bezier Curve Animations of Any Degree

Implemented Bezier curve animations of any degree `n`.
Created an efficient implementation with Eigen, resulting in very efficient animation updates.

## Texturing and Texture Coordinates

Added textures and proper handling of texture coordinates.
Created models with UV map information (texture coordinates) and refactored the mesh loader to load texture coordinates from `obj` files.

## Special Snakey Locomotion

Modified the FABRIK algorithm to create snakey looking locomotion.

## Snake Self Collision Checking

Added collision checking, failing the level when the snake eats it's own tail.

## Custom Snake Model

Created fully custom snake model in Blender with UV map and modified texture.

**We did not have any experience with Blender prior to this.**

## Music

Added menu and level music.

## Fixed CMake Build

Fixed the CMake setup that has been broken the entire semester.

We realized CMake was failing because of manually patched libraries residing in the external folder. We instead applied patches with CMake after downloading libraries.
**If you wish we can send a PR to the engine repo.**