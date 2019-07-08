# Tetris [![LICENSE](https://img.shields.io/github/license/deepgrace/tetris.svg)](https://github.com/deepgrace/tetris/blob/master/LICENSE_1_0.txt)

> **Awesome Tetris in all kinds of Languages(C, C++, Go, Shell)**

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
