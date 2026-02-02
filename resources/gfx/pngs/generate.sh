#!/usr/bin/env bash
set -e

# directory where this script lives
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

PNGS_DIR="$SCRIPT_DIR"
ROOT_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"

PNG_OPTS=(
  -strip
  -colorspace sRGB
  -define png:compression-level=9
  -define png:compression-filter=5
  -define png:compression-strategy=1
)

# buildings with floor removed + resize
convert "$PNGS_DIR/buildingTiles_001.png" \
        "$PNGS_DIR/mask_black_alpha.png" \
        -gravity South -compose DstOut -composite \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityBuilding_right_red_two.png"

convert "$PNGS_DIR/buildingTiles_002.png" \
        "$PNGS_DIR/mask_black_alpha.png" \
        -gravity South -compose DstOut -composite \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityBuilding_right_brown_two.png"

convert "$PNGS_DIR/buildingTiles_003.png" \
        "$PNGS_DIR/mask_black_alpha.png" \
        -gravity South -compose DstOut -composite \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityBuilding_left_brown_three.png"

convert "$PNGS_DIR/buildingTiles_004.png" \
        "$PNGS_DIR/mask_black_alpha.png" \
        -gravity South -compose DstOut -composite \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityBuilding_left_white_six.png"

convert "$PNGS_DIR/buildingTiles_005.png" \
        "$PNGS_DIR/mask_black_alpha.png" \
        -gravity South -compose DstOut -composite \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityBuilding_right_yellow_four.png"

convert "$PNGS_DIR/buildingTiles_006.png" \
        "$PNGS_DIR/mask_black_alpha.png" \
        -gravity South -compose DstOut -composite \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityBuilding_left_white_one.png"

convert "$PNGS_DIR/buildingTiles_007.png" \
        "$PNGS_DIR/mask_black_alpha.png" \
        -gravity South -compose DstOut -composite \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityBuilding_left_red_three.png"

# tiles
convert "$PNGS_DIR/cityTiles_004.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_wall.png"

convert "$PNGS_DIR/cityTiles_011.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_broken_wall.png"

convert "$PNGS_DIR/cityTiles_043.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_fountain.png"

convert "$PNGS_DIR/cityTiles_066.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_base.png"

convert "$PNGS_DIR/cityTiles_067.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_tree.png"

convert "$PNGS_DIR/cityTiles_073.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_road_left.png"

convert "$PNGS_DIR/cityTiles_081.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_road_right.png"

convert "$PNGS_DIR/cityTiles_103.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_road_t.png"

convert "$PNGS_DIR/cityTiles_106.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_road_d_left.png" 

convert "$PNGS_DIR/cityTiles_112.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_road_d_right.png"

convert "$PNGS_DIR/cityTiles_122.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_road_corner_1.png"

convert "$PNGS_DIR/cityTiles_124.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_road_corner_2.png"

convert "$PNGS_DIR/cityTiles_125.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_road_corner_3.png"

convert "$PNGS_DIR/cityTiles_126.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_road_corner_4.png"

convert "$PNGS_DIR/extra_beton.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_beton.png"

convert "$PNGS_DIR/extra_beton.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_beton_yellow.png"

convert "$PNGS_DIR/extra_beton.png" \
        -resize 32x \
        -colorspace HSL \
        -channel Hue -evaluate add -11% \
        -channel ALL -colorspace sRGB \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_beton_red.png"

convert "$PNGS_DIR/extra_beton_empty.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_beton.png"

convert "$PNGS_DIR/extra_broken_wall_water.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_broken_wall_water.png"

convert "$PNGS_DIR/extra_wall_water.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_wall_water.png"

convert "$PNGS_DIR/extra_water.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_water.png"

convert "$PNGS_DIR/extra_water_waves.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_water_waves.png"

convert "$PNGS_DIR/extra_water_waves_boat.png" \
        -resize 32x \
        "${PNG_OPTS[@]}" \
        "$ROOT_DIR/resources/gfx/sprites/isometric-city/cityTiles_water_waves_boat.png"

# vehicles
for f in "$PNGS_DIR"/garbage_*.png; do
  base=$(basename "$f")
  out=${base/garbage_/garbage_yellow_}
  convert "$f" \
    -resize 50% \
    "${PNG_OPTS[@]}" \
    "$ROOT_DIR/resources/gfx/sprites/isometric-vehicles/$out"
done

for f in "$PNGS_DIR"/garbage_*.png; do
  base=$(basename "$f")
  out=${base/garbage_/garbage_red_}

  convert "$f" \
    -resize 50% \
    -colorspace HSL \
    -channel Hue -evaluate add -11% \
    -channel ALL -colorspace sRGB \
    "${PNG_OPTS[@]}" \
    "$ROOT_DIR/resources/gfx/sprites/isometric-vehicles/$out"
done
