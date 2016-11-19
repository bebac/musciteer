Calculating Replaygain From Spotify Audio Features
==================================================

There doesn't seem to be much documentation around explaining how to utilize
the spotify web api audio features.

Since Musciteer is a mix of local files and spotify tracks, we need some way
to translate the loudness attribute into something comparable to replaygain.

I saved some tracks streamed from spotify and converted the audio to flac
then calculated replaygain on them. Then I tried to come up with a formula to
calculate the replaygain. With just the loudness attribute I couldn't really
make it fit. After looking through a few tracks I got the idea that energy
might be involved somehow and came up with this formula


    -((12+loudness)*energy)-3   ~   replaygain


| Song                                      | RG      | loudness | energy | calculated RG
| ----------------------------------------- | ------- | -------- | ------ | -------------
| Carrie Underwood - Church Bells           | -11.52  | -1.772   | 0.844  | -11.63
| Kim Larsen - Tik Tik                      | -4.96   | -9.264   | 0.609  | -4.666
| Micky & The Motorcars - Heart from above  | -9.80   | -2.794   | 0.905  | -11.21
| Jeff Buckley - Halleluja                  | -3.78   | -10.33   | 0.136  | -3.227
| Dire Straits - The Man Too Strong         | -4.21   | -14.482  | 0.285  | -2.292
| Laura Marling - I Was An Eagle            | -2.36   | -18.077  | 0.11   | -2.332


Please don't ask me explain this formula, cause I can't. I think spotify streams
audio at -12 LUFS which might explain the 12. I guess it makes some sense that
the energy plays a role. The offset of 3db - I don't have a clue and as you can
see it is off for some tracks, so maybe it's just all completely wrong!!!
