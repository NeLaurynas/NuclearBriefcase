// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

/*
 * THIS WAS GENERATED BY CHATGPT SO WHATEVER
*/

#ifndef PIEZO_DATA_H
#define PIEZO_DATA_H

/*
 * THIS WAS GENERATED BY CHATGPT SO WHATEVER (my prompt = my copyright kek)
*/

// -------------------- A0..A8 --------------------
#define NOTE_An0  27.50f    // A0
#define NOTE_As0  29.14f    // A#0
#define NOTE_Ab0  25.96f    // G#0 ≈ A♭0

#define NOTE_An1  55.00f    // A1
#define NOTE_As1  58.27f
#define NOTE_Ab1  51.91f

#define NOTE_An2  110.00f   // A2
#define NOTE_As2  116.54f
#define NOTE_Ab2  103.83f

#define NOTE_An3  220.00f   // A3
#define NOTE_As3  233.08f
#define NOTE_Ab3  207.65f

#define NOTE_An4  440.00f   // A4
#define NOTE_As4  466.16f
#define NOTE_Ab4  415.30f

#define NOTE_An5  880.00f   // A5
#define NOTE_As5  932.33f
#define NOTE_Ab5  830.61f

#define NOTE_An6  1760.00f  // A6
#define NOTE_As6  1864.66f
#define NOTE_Ab6  1661.22f

#define NOTE_An7  3520.00f  // A7
#define NOTE_As7  3729.31f
#define NOTE_Ab7  3322.44f

#define NOTE_An8  7040.00f  // A8
#define NOTE_As8  7458.62f
#define NOTE_Ab8  6644.88f

// -------------------- B0..B8 --------------------
#define NOTE_Bn0  30.87f   // B0
#define NOTE_Bs0  32.70f   // B#0 = C1
#define NOTE_Bb0  29.14f   // A#0

#define NOTE_Bn1  61.74f   // B1
#define NOTE_Bs1  65.41f   // B#1 = C2
#define NOTE_Bb1  58.27f   // A#1

#define NOTE_Bn2  123.47f  // B2
#define NOTE_Bs2  130.81f  // B#2 = C3
#define NOTE_Bb2  116.54f  // A#2

#define NOTE_Bn3  246.94f  // B3
#define NOTE_Bs3  261.63f  // B#3 = C4
#define NOTE_Bb3  233.08f  // A#3

#define NOTE_Bn4  493.88f  // B4
#define NOTE_Bs4  523.25f  // B#4 = C5
#define NOTE_Bb4  466.16f  // A#4

#define NOTE_Bn5  987.77f
#define NOTE_Bs5  1046.50f // B#5 = C6
#define NOTE_Bb5  932.33f  // A#5

#define NOTE_Bn6  1975.53f
#define NOTE_Bs6  2093.00f // B#6 = C7
#define NOTE_Bb6  1864.66f // A#6

#define NOTE_Bn7  3951.07f
#define NOTE_Bs7  4186.01f // B#7 = C8
#define NOTE_Bb7  3729.31f // A#7

#define NOTE_Bn8  7902.13f
// B#8 would be C9 (beyond typical 88-key range), so ~8372.02f
// We'll approximate or omit B#8 if desired.
#define NOTE_Bs8  8372.02f // hypothetical
#define NOTE_Bb8  7458.62f // A#8

// -------------------- C0..C8 --------------------
#define NOTE_Cn0  16.35f   // C0
#define NOTE_Cs0  17.32f   // C#0
// "Cb0" would be B-1 in strict sense (~15.43f), typically out of standard range:
#define NOTE_Cb0  15.43f   // if you really want it

#define NOTE_Cn1  32.70f   // C1
#define NOTE_Cs1  34.65f
#define NOTE_Cb1  30.87f   // B0

#define NOTE_Cn2  65.41f   // C2
#define NOTE_Cs2  69.30f
#define NOTE_Cb2  61.74f   // B1

#define NOTE_Cn3  130.81f  // C3
#define NOTE_Cs3  138.59f
#define NOTE_Cb3  123.47f  // B2

#define NOTE_Cn4  261.63f  // C4 (middle C)
#define NOTE_Cs4  277.18f
#define NOTE_Cb4  246.94f  // B3

#define NOTE_Cn5  523.25f
#define NOTE_Cs5  554.37f
#define NOTE_Cb5  493.88f  // B4

#define NOTE_Cn6  1046.50f
#define NOTE_Cs6  1108.73f
#define NOTE_Cb6  987.77f  // B5

#define NOTE_Cn7  2093.00f
#define NOTE_Cs7  2217.46f
#define NOTE_Cb7  1975.53f // B6

#define NOTE_Cn8  4186.01f
#define NOTE_Cs8  4434.92f
#define NOTE_Cb8  3951.07f // B7

// -------------------- D0..D8 --------------------
#define NOTE_Dn0  18.35f
#define NOTE_Ds0  19.45f   // D#0
#define NOTE_Db0  17.32f   // C#0

#define NOTE_Dn1  36.71f
#define NOTE_Ds1  38.89f
#define NOTE_Db1  34.65f

#define NOTE_Dn2  73.42f
#define NOTE_Ds2  77.78f
#define NOTE_Db2  69.30f

#define NOTE_Dn3  146.83f
#define NOTE_Ds3  155.56f
#define NOTE_Db3  138.59f

#define NOTE_Dn4  293.66f
#define NOTE_Ds4  311.13f
#define NOTE_Db4  277.18f

#define NOTE_Dn5  587.33f
#define NOTE_Ds5  622.25f
#define NOTE_Db5  554.37f

#define NOTE_Dn6  1174.66f
#define NOTE_Ds6  1244.51f
#define NOTE_Db6  1108.73f

#define NOTE_Dn7  2349.32f
#define NOTE_Ds7  2489.02f
#define NOTE_Db7  2217.46f

#define NOTE_Dn8  4698.64f
#define NOTE_Ds8  4978.03f
#define NOTE_Db8  4434.92f

// -------------------- E0..E8 --------------------
#define NOTE_En0  20.60f
#define NOTE_Es0  21.83f   // E#0 = F0 (~21.83)
#define NOTE_Eb0  19.45f   // D#0

#define NOTE_En1  41.20f
#define NOTE_Es1  43.65f   // E#1 = F1
#define NOTE_Eb1  38.89f   // D#1

#define NOTE_En2  82.41f
#define NOTE_Es2  87.31f
#define NOTE_Eb2  77.78f

#define NOTE_En3  164.81f
#define NOTE_Es3  174.61f
#define NOTE_Eb3  155.56f

#define NOTE_En4  329.63f
#define NOTE_Es4  349.23f
#define NOTE_Eb4  311.13f

#define NOTE_En5  659.26f
#define NOTE_Es5  698.46f
#define NOTE_Eb5  622.25f

#define NOTE_En6  1318.51f
#define NOTE_Es6  1396.91f
#define NOTE_Eb6  1244.51f

#define NOTE_En7  2637.02f
#define NOTE_Es7  2793.83f
#define NOTE_Eb7  2489.02f

#define NOTE_En8  5274.04f
#define NOTE_Es8  5587.65f
#define NOTE_Eb8  4978.03f

// -------------------- F0..F8 --------------------
#define NOTE_Fn0  21.83f   // F0
#define NOTE_Fs0  23.12f   // F#0
#define NOTE_Fb0  20.60f   // E0 (approx)

#define NOTE_Fn1  43.65f
#define NOTE_Fs1  46.25f
#define NOTE_Fb1  41.20f

#define NOTE_Fn2  87.31f
#define NOTE_Fs2  92.50f
#define NOTE_Fb2  82.41f

#define NOTE_Fn3  174.61f
#define NOTE_Fs3  185.00f
#define NOTE_Fb3  164.81f

#define NOTE_Fn4  349.23f
#define NOTE_Fs4  369.99f
#define NOTE_Fb4  329.63f

#define NOTE_Fn5  698.46f
#define NOTE_Fs5  739.99f
#define NOTE_Fb5  659.26f

#define NOTE_Fn6  1396.91f
#define NOTE_Fs6  1479.98f
#define NOTE_Fb6  1318.51f

#define NOTE_Fn7  2793.83f
#define NOTE_Fs7  2959.96f
#define NOTE_Fb7  2637.02f

#define NOTE_Fn8  5587.65f
#define NOTE_Fs8  5919.91f
#define NOTE_Fb8  5274.04f

// -------------------- G0..G8 --------------------
#define NOTE_Gn0  24.50f
#define NOTE_Gs0  25.96f   // G#0
#define NOTE_Gb0  23.12f   // F#0

#define NOTE_Gn1  49.00f
#define NOTE_Gs1  51.91f
#define NOTE_Gb1  46.25f

#define NOTE_Gn2  98.00f
#define NOTE_Gs2  103.83f
#define NOTE_Gb2  92.50f

#define NOTE_Gn3  196.00f
#define NOTE_Gs3  207.65f
#define NOTE_Gb3  185.00f

#define NOTE_Gn4  392.00f
#define NOTE_Gs4  415.30f
#define NOTE_Gb4  369.99f

#define NOTE_Gn5  783.99f
#define NOTE_Gs5  830.61f
#define NOTE_Gb5  739.99f

#define NOTE_Gn6  1567.98f
#define NOTE_Gs6  1661.22f
#define NOTE_Gb6  1479.98f

#define NOTE_Gn7  3135.96f
#define NOTE_Gs7  3322.44f
#define NOTE_Gb7  2959.96f

#define NOTE_Gn8  6271.93f
#define NOTE_Gs8  6644.88f
#define NOTE_Gb8  5919.91f

// ----------------- Rest -----------------
#define NOTE_OFF  0.00f

float piezo_data_get_frequency(char note, char accidental, char octaveChar);

#endif //PIEZO_DATA_H