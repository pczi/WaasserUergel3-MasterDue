
// -1 -> use the plotter to draw
//    - egy = e - EnergyAvg
//    - avg = EnergyAvg - EnergyAvg
//    - thh = threshold - EnergyAvg
//    - var = EnergyVar

// 0 -> use monitor to print level 0 (LED ANIMATION) debug info
// 1 -> use monitor to print level 0+1 (+VALVE OPERATION= debug info
#define verbose_level 1

// 0 -> display VUmeter on the display
// 1 -> display WaterOrgan
#define display_mode 1

// -1 -> automatic animation switching
// 0..max -> set the animation manually and keep it
// max should be 23
#define color_animation_mode -1

// the time in ms before switching to the next animation
#define color_animation_duration 3000

// -1 -> automatic animation switching
// 0..max -> set the animation manually and keep it
// max should be ??
#define water_animation_mode -1

// the time in ms before switching to the next animation
#define water_animation_duration 4000

