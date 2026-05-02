// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#ifndef PREANGLECALC_H
#define PREANGLECALC_H
#include "WPROUTE.h"
#include "MATHLIB.h"

struct PLANEFT
{
float wxft, wyft, wzft;
float pxft, pyft, pzft;

void normalize(PLANE *plane)
{
  wxft = plane->WPXYZ.nextWPpos->x;
  wyft = plane->WPXYZ.nextWPpos->y;
  wzft = plane->WPXYZ.nextWPpos->z;

  pxft = longToFeet(plane->loc.x, plane->loc.z);
  pyft = plane->loc.y;
  pzft = latToFeet(plane->loc.z);
}

};

#endif
