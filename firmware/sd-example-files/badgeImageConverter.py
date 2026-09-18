#!/usr/bin/env python3
"""
Convert an image (JPG, PNG, ecc.) in raw format used for the main badge's image.

Usage:
    python3 badgeImageConverter.py picture.jpg output.raw
    python3 badgeImageConverter.py picture.jpg output.raw --threshold 140
    python3 badgeImageConverter.py picture.jpg output.raw --no-dither

Dependences:
    Pillow: pip3 install Pillow
"""

import argparse
from PIL import Image

BADGE_IMAGE_SIZE = 48 # BADGE_IMAGE_DIAMETER of tools/badgeData.h

def convert(input_path, output_path, threshold=None, dither=True):
    img = Image.open(input_path).convert("L") # Grayscale

    # Cut in the center
    w, h = img.size
    side = min(w, h)
    left = (w - side) // 2
    top = (h - side) // 2
    img = img.crop((left, top, left + side, top + side))

    img = img.resize((BADGE_IMAGE_SIZE, BADGE_IMAGE_SIZE), Image.LANCZOS)

    if threshold is not None:
        img = img.point(lambda p: 255 if p > threshold else 0)
        img = img.convert("1")
    elif dither:
        img = img.convert("1")
    else:
        img = img.convert("1", dither=Image.NONE)

    pixels = img.load()
    row_bytes = (BADGE_IMAGE_SIZE + 7) // 8

    with open(output_path, "wb") as f:
        f.write(bytes([BADGE_IMAGE_SIZE, BADGE_IMAGE_SIZE]))

        for row in range(BADGE_IMAGE_SIZE):
            row_data = bytearray(row_bytes)

            for col in range(BADGE_IMAGE_SIZE):
                if pixels[col, row] == 0:
                    row_data[col // 8] |= (0x80 >> (col % 8))
            
            f.write(bytes(row_data))

    total = 2 + row_bytes * BADGE_IMAGE_SIZE
    print(f"Wrote {output_path}: {BADGE_IMAGE_SIZE}x{BADGE_IMAGE_SIZE}, {total} byte")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("input", help="Source image (jpg/png/etc.)")
    parser.add_argument("output", help="File .raw to copy in the SD")
    parser.add_argument("--threshold", type=int, default=None, help="Choose the light of the bits (0-255)")
    parser.add_argument("--no-dither", action="store_true", help="Use automatic light")
    args = parser.parse_args()

    convert(args.input, args.output, threshold=args.threshold, dither=not args.no_dither)
