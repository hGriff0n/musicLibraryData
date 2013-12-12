musicLibraryData
================

console program that displays infometrics about the music in your library


the program scrounges through the file structure of your iTunes library
(couldn't get the necessary data from the .mp3's and .m4a's) extracting
data about each song you own 
You can then request the program to perform
various actions to the gathered data including sorting, showing, and infometrics.

:: Basics ::

the program consists of a series of commands (sort, show, help, info, exit, exec)
and a series of types (artist, album, ext)

use of the program involves inputting a command, establishing a type to operate on,
and specifying a series of arguments to the command

> sort artist

> show artist

these commands first sorts the artist counter
on a most-first basis and then outputs it to the screen

> sort artist

> show

stating the type is not necessary for the second command
if the wanted type is the same as the first command

> sort artist | show

these commands can be shortened into one line by
using the 'pipe' command ( "|" )

> sort artist | show -c 1

each command comes with a series of arguments, or flags ( some of which may be required for each call )
arguments are specified by a -[x] flag followed by a value
if a command has an internal state used for default behavior, a -[x]F flag will cause
the internal state to be set to the passed value after execution is finished
type help [cmd] to learn more about it's arguments

:: Infometrics ::

the program provides three commands useful for infometrics ( total, find, group )
total - how many songs are in your library
find -  how many songs by specific artist/albums/ext
group - how many songs by groups of artist/albums/ext

> total

> find album -a "High Violet" -b

> sort artist | group -t 10 -p

outputs the number of songs in your library
outputs the number of songs in the "High Violet" album and the percentage of your library that is
outputs the percentage of your library held by your top 10 artists

