# Tetris [![LICENSE](https://img.shields.io/github/license/deepgrace/tetris.svg)](https://github.com/deepgrace/tetris/blob/master/LICENSE_1_0.txt)

> **Stunning Tetris in all kinds of Languages(C, C++, Go, Shell)**

# Screenshots
<img alt = "Tetris" src="https://raw.githubusercontent.com/deepgrace/tetris/master/screenshots/png/p1.png">
<img alt = "Color" src="https://raw.githubusercontent.com/deepgrace/tetris/master/screenshots/png/color.png">

# Linux Platform

## Algorithm
                                                                    [][][]
                 [][][]                                         [][][][]
                 []                  [][][]                         [][]
     [][] [][]   []  [][][][]  [][][][][]    [][]             [][]  []   [][] [][]
     [] row []   []  [] (x-m)*zoomx  [][]    []  cos(a) sin(a)  []  []   []  m  []
     []     [] = []  []              []      []                 []  [] + []     []
     [] col []   []  [] (y-n)*zoomy  []      [] -sin(a) cos(a)  []  []   []  n  []
     [][] [][]   []  [][][][]  [][][][]      [][]             [][]  []   [][] [][]
                 []                                                 []
                 [][][]                                         [][][]

## Run
     runlevel     :  Complexity of the pieces              ( Default 31 )
     previewlevel :  Number of the pieces to be previewed  ( Default 6  )
     speedlevel   :  Speed of the current piece's descent  ( Default 0  )
     width        :  Width of the view                     ( Default 25 )
     height       :  Height of the view                    ( Default 30 )

     Usage        :  ./tetris [ <runlevel> [ <previewlevel> [ <speedlevel> [ <width> [height] ] ] ] ] 
     Range        :  [ 0 <= runlevel <= 31 ] [ previewlevel >= 1 ] [ speedlevel <= 30 ] [ width >= 17 ] [ height >= 10 ]
     Notice       :  runlevel previewlevel speedlevel width height belong to integer

 ## Tetris: The Soviet 'mind game' that took over the world
Tetris is a puzzle game in which geometric shapes called "tetrominoes" fall down onto a playing field, and the player has to arrange them to form gapless lines. Pajitnov   took inspiration from pentomino, a classic puzzle game consisting of all the different shapes that can be made by combining five squares -- 12 in total -- with the goal of arranging them in a wooden box like a jigsaw puzzle.<br><br>
To simplify things, he knocked that down to four squares, thus reducing the number of shapes from 12 to seven. He called the game Tetris, combining the Greek numeral      "tetra" (meaning four) and tennis, his favorite sport.
