//
// Copyright (c) 2011-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/tetris
//

package main

import (
    "os"
    "fmt"
    "sync"
    "time"
    "bufio"
    "unsafe"
    "os/exec"
    "strconv"
    "strings"
    "syscall"
    "math/rand"
)

var box0 = []int{4,30}
var box1 = []int{4,30,4,32}
var box2 = []int{4,30,5,32}
var box3 = []int{4,28,4,30,4,32}
var box4 = []int{4,28,4,30,5,30}
var box5 = []int{4,28,5,30,6,32}
var box6 = []int{4,30,5,28,5,32}
var box7 = []int{4,28,5,30,6,32,7,34}
var box8 = []int{4,30,5,28,5,30,5,32}
var box9 = []int{4,30,5,28,5,32,6,30}
var box10 = []int{4,28,4,30,4,32,4,34}
var box11 = []int{4,28,5,28,5,30,5,32}
var box12 = []int{4,28,4,30,5,30,5,32}
var box13 = []int{4,28,4,30,5,28,5,30}
var box14 = []int{4,28,4,34,5,30,5,32}
var box15 = []int{4,26,4,28,4,30,4,32,4,34}
var box16 = []int{4,30,5,28,5,30,5,32,6,30}
var box17 = []int{4,28,4,32,5,30,6,28,6,32}
var box18 = []int{4,28,4,32,5,28,5,30,5,32}
var box19 = []int{4,28,4,30,5,30,6,30,6,32}
var box20 = []int{4,28,5,28,6,28,6,30,6,32}
var box21 = []int{4,28,4,30,5,30,5,32,6,32}
var box22 = []int{4,26,4,34,5,28,5,30,5,32}
var box23 = []int{4,26,4,34,5,28,5,32,6,30}
var box24 = []int{4,26,5,28,6,30,7,32,8,34}
var box25 = []int{4,28,4,32,5,26,5,30,5,34}
var box26 = []int{4,28,4,34,5,30,5,32,6,30,6,32,7,28,7,34}
var box27 = []int{4,30,5,28,5,32,6,26,6,30,6,34,7,28,7,32,8,30}
var box28 = []int{4,30,5,28,5,30,5,32,6,26,6,28,6,30,6,32,6,34,7,28,7,30,7,32,8,30}
var box29 = []int{4,30,5,30,6,28,6,30,6,32,7,26,7,30,7,34,8,30,9,30,10,30,11,30,12,30}
var box30 = []int{4,26,4,28,4,30,4,34,5,30,5,34,6,26,6,28,6,30,6,32,6,34,7,26,7,30,8,26,8,30,8,32,8,34}
var box31 = []int{4,30,5,28,6,26,6,34,7,28,7,32,7,36,8,22,8,30,8,38,9,24,9,28,9,32,10,26,10,34,11,32,12,30}
var BOX = [][]int{box0,box1,box2,box3,box4,box5,box6,box7,box8,box9,box10,box11,box12,box13,box14,box15,box16,
                  box17,box18,box19,box20,box21,box22,box23,box24,box25,box26,box27,box28,box29,box30,box31}

const toph int = 3
const modw int = 4
var score int = 0
var box, locus []int
var width int = 25
var height int = 30
var prelevel int = 6
var runlevel int = 31
var speedlevel int = 0
var wthm int = 2*width+modw
var dist int = modw+wthm+3
var lower int = height+toph
const unit string = "[]"
const esc string = "\033"
var gmover string = esc+"[?25h"+esc+"[36;26HGame Over!"+esc+"[0m\n"

var out *os.File
var oldt syscall.Termios

type atom struct {
     mu  sync.Mutex
     sig int
}

var cur_thread atom

func init() {
     timens := int64(time.Now().Nanosecond())
     rand.Seed(timens)
}

func wait(n time.Duration) {
     time.Sleep(n)
}

type board struct {}

type max_distance struct {
     max, col     []int
}

type transpose struct {
     mid, new_box, new_coordinate []int
}

type get_time struct {
     time []string
     color, line string
     day, hour, minute, second int
}

type piece struct {
     bd board
     full bool
     count, line int
     trans transpose
     preview_box []int
     dists max_distance
     box_color [][]string
     box_map, next_preview_piece [][]int
     old_preview_block, next_preview_color, color []string
     cur_shadow, cur_color, old_shadow, cur_preview_block string
}

func main() {
     if get_args() { os.Exit(0) }
     newt := oldt
     out, _ = os.OpenFile("/dev/tty", syscall.O_WRONLY, 0)
     newt.Lflag &^= syscall.ECHO | syscall.ICANON
     set_termios(out.Fd(), &newt)

     pg := piece{}
     time := get_time{}
     pg.construct()

     go time.current()
     go pg.persig()
     pg.getsig()
}

func (this *atom) lock_thread(new_sig int) int {
      this.mu.Lock()
      old_sig := this.sig
      if new_sig != -1 {
         this.sig = new_sig
      }
      this.mu.Unlock()
      return old_sig
}

func set_termios(fd uintptr, termios *syscall.Termios) error {
     r, _, e := syscall.Syscall(syscall.SYS_IOCTL, fd, uintptr(syscall.TCSETS), uintptr(unsafe.Pointer(termios)))
     if r != 0 {
        return os.NewSyscallError("SYS_IOCTL", e)
     }

     return nil
}

func runleave(n int) {
     if n == 0  {
        fmt.Printf(gmover)
     } else {
        if n == 1 {
           cur_thread.lock_thread(22)
        }
        fmt.Printf("\033[?25h\033[36;4H\n")
     }

     set_termios(out.Fd(), &oldt)
}

func get_args() bool {
     args_len := len(os.Args)
     if args_len > 2 {
        runlevel, _ = strconv.Atoi(os.Args[1])
        prelevel, _ = strconv.Atoi(os.Args[2])
        if args_len > 3 {
           speedlevel, _ = strconv.Atoi(os.Args[3])
           if args_len > 4 {
              width, _ = strconv.Atoi(os.Args[4])
              if args_len > 5 {
                 height, _ = strconv.Atoi(os.Args[5])
              }
           }
        }
        return false
     }

     if args_len == 2 {
        switch os.Args[1] {
        case "-h", "--help":
             fmt.Printf("Usage: "+os.Args[0]+" [ <runlevel> [ <previewlevel> [ <speedlevel> [ <width> [height] ] ] ] ]\n")
             fmt.Printf("Range: [ 0 <= runlevel <= 31 ] [ previewlevel >= 1 ] [ speedlevel <= 30 ] [ width >= 17 ] [ height >= 10 ]\n")
             return true
        case "-v", "--version":
             fmt.Printf("Version 1.0.7\n")
             return true
        default:
             runlevel, _ = strconv.Atoi(os.Args[1])
             return false
        }
     }

     return false
}

func (this *max_distance) clear() *max_distance {
      this.col = []int{}
      this.max = []int{}
      return this
}

func (this *max_distance) max_vertical_coordinate() *max_distance {
      size, j, k := 0, 0, 0
      for i := 0; i != len(box); i += 2 {
          q := box[i]
          p := box[i+1]
          for j = 0; j != size; j++ {
              if ( this.col[j] == p ) {
                    break
              }
          }
          k = 2*j
          if ( j == size ) {
               size++;
               this.col = append(this.col, p)
               this.col = append(this.col, q)
          } else {
               if ( this.col[j+1] < q ) {
                    this.col[j+1] = q
               }
          }
      }
      for i := 0; i != size; i++ {
          k = 2*i
          this.max = append(this.max, this.col[k+1])
          this.max = append(this.max, this.col[k])
      }

      return this
}

func (this *max_distance) max_height(box_map [][]int) int {
      i := 0
      for j := 0; i != height; j += 2 {
          if box_map[this.max[j]+i-modw][this.max[j+1]/2-toph] == 1 {
             return i-1
          }
          if this.max[j]+i == lower {
             return i
          }
          if j+2 == len(this.max) {
             j = -2
             i++
          }
      }

      return 0
}

func (this *transpose) unique(vec []int) []int {
      size, j := 0, 0
      this.mid = []int{}
      for i := 0; i != len(vec); i += 2 {
          p := vec[i]
          q := vec[i+1]
          for j = 0; j != size; j++ {
              k := 2*j
              if ( this.mid[k] == p && this.mid[k+1] == q ) {
                    break
              }
          }
          if ( j == size ) {
               size++
               this.mid = append(this.mid, p)
               this.mid = append(this.mid, q)
          }
      }
      return this.mid
}

func (this *transpose) addbox(new_box []int, k int, j int) {
      for i := 0; i != len(new_box); i += 2 {
          new_box[i+k] += j
      }
}

func (this *transpose) mid_point(mid []int) (int, int) {
      num := len(mid)/2-(len(mid)%4)/2
      return mid[num], mid[num+1]
}

func (this *transpose) multiple(cur_box []int, b int, d int) []int {
      this.new_box=[]int{}
      this.new_box=append(this.new_box, cur_box...)
      for i := 0; i != len(cur_box)-2; i += 2 {
          this.new_box[i+3] = this.new_box[i+1]+(cur_box[i+3]-cur_box[i+1])*b/d
      }

      return this.new_box
}

func (this *transpose) coordinate_transformation(dx float32, m int, n int) {
      size := len(this.new_box)
      new_coordinate := make([]int,size,size)                          // row=(x-m)*zoomx*cos(a)-(y-n)*zoomy*sin(a)+m
      for i := 0; i != size; i += 2 {                                  // col=(x-m)*zoomx*sin(a)+(y-n)*zoomy*cos(a)+n
          new_coordinate[i] = m+this.new_box[i+1]-n                    // a=-pi/2 zoomx=+1 zoomy=+1 dx=0 dy=0
          new_coordinate[i+1] = int(float32(this.new_box[i]-m)*dx)+n   // a=-pi/2 zoomx=-1 zoomy=+1 dx=0 dy=0 
      }                                                                // a=+pi/2 zoomx=+1 zoomy=-1 dx=0 dy=0

      this.new_coordinate = new_coordinate
      if dx == 0.5 {
         this.new_coordinate = []int{}
         this.new_coordinate = append(this.new_coordinate, this.unique(new_coordinate)...)
      }
}

func (this *transpose) abstract(mid []int, i *int, j *int, b int, d int ) {
      *i, *j = this.mid_point(this.multiple(mid, b, d))
}

func (this *transpose) optimize(new_box []int, vec []int) {
      n, k := 0, 0
      for _, j := range vec {
          n++
          if j != 0 {
             if n > 1 {
                k = 1
             } else {
                k = 0
             }
             this.addbox(new_box, k, j)
          }
      }
}

func (this *get_time) sleep(ch chan int) {
      wait(1*1e9)
      ch <- 1
}

func (this *get_time) set_time(lhs *int, rhs *int, value  int, i int) {
      if *lhs == value {
         *rhs++
         *lhs = 0
      }
      this.time[i] = strconv.Itoa(*lhs/10)+strconv.Itoa(*lhs%10)
}

func (this *get_time) current() {
      j := width-9
      this.color = esc+"[1;33m"
      this.line = strings.Repeat("-", j)
      fmt.Printf(esc+"[2;6H"+this.color+this.line+"[Time "+esc+"[2;"+strconv.Itoa(23+j)+"H"+this.color+"]"+this.line+esc+"[0m\n")
      ch := make(chan int)
      for {
               sig := cur_thread.lock_thread(-1)
          if ( sig == 22 ) {
              runleave(0)
              break
          }

          go this.sleep(ch)
          this.time = []string{"", "", "", ""}
          this.set_time(&this.second, &this.minute, 60, 0)
          this.set_time(&this.minute, &this.hour, 60, 1)
          this.set_time(&this.hour, &this.day, 24, 2)
          this.time[3] = strconv.Itoa(this.day/10)+strconv.Itoa(this.day%10)

          str0 := esc+"[2;"+strconv.Itoa(12+j)+"H"+this.color+this.time[3]+":"
          str1 := this.time[2]+":"+this.time[1]+":"+this.time[0]+esc+"[0m\n"
          fmt.Printf(str0+str1)
          this.second += <-ch
      }
}

func (this *piece) construct() {
      if width < 17 { width = 25 }
      if height < 10 { height = 30 }
      if prelevel < 1 { prelevel = 6 }
      if speedlevel > 30 { speedlevel = 0 }
      if runlevel < 0 || runlevel > 31 { runlevel = 31 }

      runlevel++
      lower = height+toph
      wthm = 2*width+modw
      dist = modw+wthm+3

      this.box_map = make([][]int, height, height)
      this.box_color = make([][]string, height, height)
      this.old_preview_block = make([]string, prelevel, prelevel)
      this.next_preview_color = make([]string, prelevel, prelevel)
      this.next_preview_piece = make([][]int, prelevel, prelevel)

      for i := 0; i != height; i++ {
          this.box_map[i] = make([]int, width, width)
          this.box_color[i] = make([]string, width, width)
      }

      this.loop(this.init, this.shift_piece, false)
      fmt.Printf("%c%c%c%c%c%c",27,'[','H',27,'[','J')
      fmt.Printf(esc+"[?25l")

      this.init_color()
      this.bd.border()
      this.bd.matrix()
      this.bd.notify()
      this.show_piece(true)
      this.draw_piece(true,0,0)
}

func (this *piece) check(i int, j int) {
      if this.box_map[i-modw][j/2-toph] == 0 {
         this.full = false
      }
}

func (this *piece) update(i int, j int) {
      pos := esc+"["+strconv.Itoa(i)+";"+strconv.Itoa(j)+"H"
      if this.box_map[i-modw][j/2-toph] == 0 {
         fmt.Printf(pos+"  ")
      } else {
         fmt.Printf(pos+esc+"["+this.box_color[i-modw][j/2-toph]+unit+esc+"[0m")
      }
}

func (this *piece) drop_bottom() float32 {
      return float32(this.dists.clear().max_vertical_coordinate().max_height(this.box_map))
}

func (this *piece) get_piece() []int {
      box = []int{}
      box = append(box, BOX[rand.Intn(runlevel)]...)
      return box
}

func (this *piece) init(i int, j int) {
      this.box_map[i-modw][j/2-toph] = 0
      this.box_color[i-modw][j/2-toph] = ""
}

func (this *piece) init_color() {
      for i := 0; i != 9; i++ {
          for j := 0; j != 9; j++ {
              str := "1;3"+strconv.Itoa(i)+";4"+strconv.Itoa(j)+"m"
              this.color = append(this.color, str)
          }
      }

      for i, j := 0, 38; i != 2; i++ {
          for k := 0; k != 256; k++ {
              str := strconv.Itoa(j)+";5;"+strconv.Itoa(k)+";1m"
              this.color = append(this.color, str)
          }
          j += 10
      }
}

func (this *piece) loop(lhs func (int, int), rhs func (int), call bool) {
      for i, j, l := 4, 6, wthm; i <= lower; j += 2 {
          this.full = true
          lhs(i, j)
          if ! this.full || j == l {
             if this.full && call {
                rhs(i)
             }
             j = 4
             i++
          }
      }
}

func (this *piece) get_replace(src string) string {
      return strings.Replace(src, "[]", "  ", -1)
}

func (this *piece) shift_piece(i int) {
      this.line++
      for j := i; j > toph+1; j-- {
          this.box_color[j-modw] = this.box_color[j-5]
          this.box_map[j-modw] = this.box_map[j-5]
      }

      for k := 0; k != width; k++ {
          this.box_map[0][k] = 0
          this.box_color[0][k] = ""
      }
}

func (this *piece) get_preview(box0 []int, old_block string, n int, new_block_color string) {
      this.cur_preview_block = ""
      for i := 0; i != len(box0); i+=2 {
          col := box0[i+1]-(n-dist)
          this.cur_preview_block += esc+"["+strconv.Itoa(box0[i]-1)+";"+strconv.Itoa(col)+"H"+unit
      }
      fmt.Printf(this.get_replace(old_block)+esc+"["+new_block_color+this.cur_preview_block+esc+"[0m\n")
}

func (this *piece) get_erase() {
      fmt.Printf(this.get_replace(this.old_shadow)+esc+"[0m\n")
}

func (this *piece) pipe_piece(next_preview_box0 *[]int, next_preview_box1 []int,
      old_preview_box *string, n int, cur bool, new_preview_color0 *string, new_preview_color1 string) {
      if cur {
         *next_preview_box0 = this.get_piece()
         this.cur_color = this.color[rand.Intn(len(this.color))]
         *new_preview_color0 = this.cur_color
         this.get_preview(box, *old_preview_box, n, this.cur_color)
      } else {
         *next_preview_box0 = next_preview_box1
         *new_preview_color0 = new_preview_color1
         this.get_preview(next_preview_box1, *old_preview_box, n, new_preview_color1)
      }
      *old_preview_box = this.cur_preview_block
}

func (this *piece) get_invoke(cur bool) {
      for i := 0; i != prelevel-1; i++ {
          this.pipe_piece(&this.next_preview_piece[i], this.next_preview_piece[i+1],
          &this.old_preview_block[i], 12*(2-i), cur, &this.next_preview_color[i], this.next_preview_color[i+1])
      }
}

func (this *piece) show_piece(cur bool) {
      end := prelevel-1
      this.cur_color = this.next_preview_color[0]
      this.preview_box = this.next_preview_piece[0]
      this.get_invoke(cur)
      this.cur_preview_block = ""
      this.next_preview_piece[end] = this.get_piece()
      this.next_preview_color[end] = this.color[rand.Intn(len(this.color))]
      this.get_preview(box, this.old_preview_block[end], 12*(2-end), this.next_preview_color[end])
      this.old_preview_block[end] = this.cur_preview_block
      box = this.preview_box
}

func (this *piece) coordinate(lbox []int) {
      this.cur_shadow = ""
      locus = lbox
      for i := 0; i != len(locus); i += 2 {
          this.cur_shadow += esc+"["+strconv.Itoa(locus[i])+";"+strconv.Itoa(locus[i+1])+"H"+unit
      }
}

func (this *piece) draw_piece(cur bool, dx int, dy int) {
      if cur {
         this.cur_color = this.color[rand.Intn(len(this.color))]
         this.coordinate(this.get_piece())
      } else {
         this.cur_color = this.next_preview_color[0]
         this.coordinate(this.next_preview_piece[0])
      }
      this.drawbox()
      if ! this.move_piece(dx, dy) {
         cur_thread.lock_thread(22)
         runleave(0)
      }
}

func (this *piece) random_piece() {
      k := height-1
      this.count++
      for i, j := 0, 6; i != this.count; j += 2 {
          if j == wthm {
             j = 4
             i++
          }
          if rand.Intn(2) != 0 {
             this.box_map[k-i][j/2-toph] = 1
             this.box_color[k-i][j/2-toph] = this.color[rand.Intn(len(this.color))]
          }
      }
      if this.count == k {
         this.count = 0
      }
}

func (this *piece) coord_comp(x *int, y *int, u *int, v *int) {
      if *x <= *y {
         *x = *y
         if *u < *v {
            *u = *v
         }
      }
}

func (this *piece) top_point(cur_box []int) {
      x, y := 0, 0
      u, v := cur_box[0], cur_box[1]
      for i := 0; i != len(cur_box); i += 2 {
          this.coord_comp(&x, &cur_box[i], &y, &cur_box[i+1])
          this.coord_comp(&cur_box[i], &u, &cur_box[i+1], &v)
      }
      if x-u == 3 && y-v == 6 {
         cur_box = []int{x-3, y-6, x-3, y, x, y-6, x, y}
      }
}

func (this *piece) bomb(x int, y int, size int) {
      empty := ""
      radius := []int{x-1, y-2, x-1, y, x-1, y+2, x, y-2, x, y, x, y+2, x+1, y-2, x+1, y, x+1, y+2}
      for j := 0; j != len(radius); j += 2 {
          p := radius[j]
          q := radius[j+1]
          boolq := ( q <= wthm && q > modw )
          boolp := ( p > toph && p <= lower )
          if boolp && boolq {
             limit := ( p+q != x+y && size != 8 )
             if this.box_map[p-modw][q/2-toph] == 0 && limit {
                continue
             }
             empty += esc+"["+strconv.Itoa(p)+";"+strconv.Itoa(q)+"H"+"\040\040"
             this.init(p, q)
          }
      }
      wait(0.03*1e9)
      fmt.Println(empty)
}

func (this *piece) clear_row() int {
      cur_box := locus
      cur_len := len(cur_box)
      if cur_len == 16 {
         this.top_point(cur_box)
      }

      for i := 0; i != len(cur_box); i += 2 {
          x := cur_box[i]
          y := cur_box[i+1]
          if cur_len == 16 {
             this.bomb(x, y, len(cur_box))
          } else {
             this.box_map[x-modw][y/2-toph] = 1
             this.box_color[x-modw][y/2-toph] = this.cur_color
          }
      }

      this.line = 0
      this.loop(this.check, this.shift_piece, true)
      if this.line == 0 {
         return 1
      }

      num := this.line*200-100
      score += num
      fmt.Printf(esc+"[1;34m"+esc+"["+strconv.Itoa(toph+10)+";"+strconv.Itoa(dist+56)+"H"+strconv.Itoa(score)+esc+"[0m\n")
      if score%5000 < num && speedlevel < 30 {
         this.random_piece()
         speedlevel++
         fmt.Printf(esc+"[1;34m"+esc+"["+strconv.Itoa(toph+10)+";"+strconv.Itoa(dist+26)+"H"+strconv.Itoa(speedlevel)+esc+"[0m\n")
      }
      this.loop(this.update, this.shift_piece, false)
      return 0
}

func (this *piece) persig() int {
      j := 1
      for j != 0 {
          if j != 1 {
             wait(0.02*1e9)
          }
          sigswap := cur_thread.lock_thread(0)
          switch sigswap {
            case 22 : runleave(0   ); return 0
            case 23 : this.transform(-1,  1)
            case 24 : this.transform(1,   1)
            case 25 : this.transform(-2,  1)
            case 26 : this.transform(0.5, 1)
            case 27 : this.transform(0,  -2)
            case 28 : this.transform(0,   2)
            case 29 : this.transform(1,   0)
            case 30 : this.transform(-1,  0)
            case 31 : this.transform(this.drop_bottom(),0)
          }
          if j == 31-speedlevel {
             this.transform(1, 0)
             j = 0
          }
          j++
      }

      return 0
}

func (this *piece) getsig() int {
      reader := bufio.NewReader(os.Stdin)
      var arry [3]byte
      cmd := exec.Command("/bin/stty", "-F", "/dev/tty", "min", "1")
      for {
          sig := 0
          cmd.Run()
          key, _ := reader.ReadByte()
          arry[0], arry[1], arry[2] = arry[1], arry[2], key
          if key == '\r' || key == ' ' {
             sig = 31
          } else if key == '\033' && arry[1] == '\033' {
             runleave(1)
             return 0
          } else if arry[0] == '\033' && arry[1] == '[' {
             switch key {
               case 'A':  sig = 23
               case 'B':  sig = 29
               case 'D':  sig = 27
               case 'C':  sig = 28
             }
          } else {
             switch key {
               case 'T', 't' : sig = 24
               case 'M', 'm' : sig = 25
               case 'N', 'n' : sig = 26
               case 'S', 's' : sig = 29
               case 'A', 'a' : sig = 27
               case 'D', 'd' : sig = 28
               case 'U', 'u' : sig = 30
               case 'P', 'p' : sig = 19
               case 'R', 'r' : sig = 18
               case 'Q', 'q' : runleave(1); return 0
             }
          }
          if sig == 22 {
             runleave(1)
             return 0
          }
          if sig != 0 {
             cur_thread.lock_thread(sig)
          }
      }

      return 0
}

func (this *piece) drawbox() {
      this.old_shadow = this.cur_shadow
      fmt.Printf(esc+"["+this.cur_color+this.cur_shadow+esc+"[0m\n")
}

func (this *piece) move_piece(dx int, dy int) bool {
      cur_len := len(locus)
      for i := 0; i != cur_len; i += 2 {
          x := locus[i]+dx
          y := locus[i+1]+dy
          boolx := ( x <= toph || x > lower )
          booly := ( y >= wthm+2 || y <= modw+1 )
          if boolx || booly {
             return false
          }

          if this.box_map[x-modw][y/2-toph] == 1 {
             if cur_len == 2 {
                for j := lower; j > x; j-- {
                    if this.box_map[j-modw][y/2-toph] == 0 {
                       return true
                    }
                }
             }
             return false
          }
      }

      return true
}

func (this *piece) ghost_cross() {
      i := locus[0]
      j := locus[1]
      if this.box_map[i-modw][j/2-toph] == 1 {
         fmt.Printf(esc+"["+strconv.Itoa(i)+";"+strconv.Itoa(j)+"H"+esc+"["+this.box_color[i-modw][j/2-toph]+unit+esc+"[0m\n")
      }
}

func (this *piece) plus(dx int, dy int) {
      if len(locus) == 2 {
         this.ghost_cross()
      }
      this.trans.optimize(box, []int{dx, dy})
      this.coordinate(box)
}

func (this *piece) move(dx int, dy int) {
      if this.move_piece(dx, dy) {
         this.get_erase()
         this.plus(dx, dy)
         this.drawbox()
      } else {
         if dx == 1 {
            this.clear_row()
            this.draw_piece(false, dx, dy)
            this.show_piece(false)
         }
      }
}

func (this *piece) rotate(dx float32, dy int) {
      m, n, p, q := 0, 0, 0, 0
      mp,nq := this.trans.mid_point(box)
      this.trans.abstract(box, &m, &n, 1, 2)
      this.trans.coordinate_transformation(dx, m, n)
      this.trans.abstract(this.trans.new_coordinate, &p, &q, 2, 1)
      this.trans.optimize(this.trans.new_box, []int{mp-p, nq-q})
      locus = this.trans.new_box
      if this.move_piece(0, 0) {
         this.get_erase()
         this.coordinate(this.trans.new_box)
         this.drawbox()
         box = locus
      } else {
         locus = box
      }
}

func (this *piece) transform(dx float32, dy int) {
      if dy == 1 {
         this.rotate(dx, dy)
      } else {
         this.move(int(dx), dy)
      }
}

func (this *board) border() {
      boucol := esc+"[38;5"
      num := rand.Intn(145)+6
      for i :=6; i<=wthm; i+=2 {
          str1 := boucol+";"+strconv.Itoa(num+i)+";1m"+esc+"["+strconv.Itoa(toph)+";"+strconv.Itoa(i)+"H=="
          str2 := boucol+";"+strconv.Itoa(num+i+25)+";1m"+esc+"["+strconv.Itoa(lower+1)+";"+strconv.Itoa(i)+"H=="+esc+"[0m"
          fmt.Printf(str1+str2+"\n")
      }
      for i := toph; i<=lower+1; i++ {
          str1 := boucol+";"+strconv.Itoa(num+i)+";1m"+esc+"["+strconv.Itoa(i)+";"+strconv.Itoa(modw)+"H||"
          str2 := boucol+";"+strconv.Itoa(num+i+30)+";1m"+esc+"["+strconv.Itoa(i)+";"+strconv.Itoa(wthm+2)+"H||"+esc+"[0m"
          fmt.Printf(str1+str2+"\n")
      }
}

func (this *board) notify() {
      str0 := esc+"[1;31m"+esc+"["+strconv.Itoa(toph+9)+";"+strconv.Itoa(dist)+"HRunLevel"+esc+"[1;31m"+esc+"["
      str1 := strconv.Itoa(toph+9)+";"+strconv.Itoa(dist+11)+"HPreviewLevel"+esc+"[0m"
      fmt.Printf(str0+str1+"\n")
      str2 := esc+"[1;31m"+esc+"["+strconv.Itoa(toph+9)+";"+strconv.Itoa(dist+26)+"HSpeedLevel"+esc+"[1;31m"+esc+"["
      str3 := strconv.Itoa(toph+9)+";"+strconv.Itoa(dist+56)+"HScore"+esc+"[0m"
      fmt.Printf(str2+str3+"\n")
      str4 := esc+"[1;31m"+esc+"["+strconv.Itoa(toph+9)+";"+strconv.Itoa(dist+39)+"HWidth"+esc+"[1;31m"+esc+"["
      str5 := strconv.Itoa(toph+9)+";"+strconv.Itoa(dist+47)+"HHeight"+esc+"[0m"
      fmt.Printf(str4+str5+"\n")
      str6 := esc+"[1;34m"+esc+"["+strconv.Itoa(toph+10)+";"+strconv.Itoa(dist)+"H"+strconv.Itoa(runlevel-1)
      str7 := esc+"[1;34m"+esc+"["+strconv.Itoa(toph+10)+";"+strconv.Itoa(dist+11)+"H"+strconv.Itoa(prelevel)+esc+"[0m"
      fmt.Printf(str6+str7+"\n")
      str8 := esc+"[1;34m"+esc+"["+strconv.Itoa(toph+10)+";"+strconv.Itoa(dist+26)+"H"+strconv.Itoa(speedlevel)
      str9 := esc+"[1;34m"+esc+"["+strconv.Itoa(toph+10)+";"+strconv.Itoa(dist+56)+"H"+strconv.Itoa(score)+esc+"[0m"
      fmt.Printf(str8+str9+"\n")
      str10 := esc+"[1;34m"+esc+"["+strconv.Itoa(toph+10)+";"+strconv.Itoa(dist+39)+"H"+strconv.Itoa(width)
      str11 := esc+"[1;34m"+esc+"["+strconv.Itoa(toph+10)+";"+strconv.Itoa(dist+47)+"H"+strconv.Itoa(height)+esc+"[0m"
      fmt.Printf(str10+str11+"\n")
      fmt.Printf(esc+"[38;5;34;1m"+esc+"["+strconv.Itoa(toph+12)+";"+strconv.Itoa(dist)+"HM|m      ===   double         N|n          ===   half\n")
      fmt.Printf(esc+"["+strconv.Itoa(toph+13)+";"+strconv.Itoa(dist)+"HQ|q|ESC  ===   exit           U|u          ===   one step up\n")
      fmt.Printf(esc+"["+strconv.Itoa(toph+14)+";"+strconv.Itoa(dist)+"HP|p      ===   pause          S|s|down     ===   one step down\n")
      fmt.Printf(esc+"["+strconv.Itoa(toph+15)+";"+strconv.Itoa(dist)+"HR|r      ===   resume         A|a|left     ===   one step left\n")
      fmt.Printf(esc+"["+strconv.Itoa(toph+16)+";"+strconv.Itoa(dist)+"HW|w|up   ===   rotate         D|d|right    ===   one step right\n")
      fmt.Printf(esc+"["+strconv.Itoa(toph+17)+";"+strconv.Itoa(dist)+"HT|t      ===   transpose      Space|enter  ===   drop all down\n")
      fmt.Printf(esc+"[38;5;106;1m"+esc+"["+strconv.Itoa(toph+19)+";"+strconv.Itoa(dist)+"HVersion 1.0.7\n")
      str12 := esc+"["+strconv.Itoa(toph+20)+";"+strconv.Itoa(dist)+"Hhttps://github.com/deepgrace"
      str13 := esc+"["+strconv.Itoa(toph+21)+";"+strconv.Itoa(dist)+"HGrace <complex.invoke@gmail.com>"
      fmt.Printf(str12+str13+"\n")
}

func (this *board) matrix() {
      one := " "
      end := esc+"[0m"
      block := "[][]"
      two := one+one
      cube := one+two
      five := two+cube
      ten := five+five
      equ := esc+"[38;5;191;1m"
      colbon := esc+"[38;5;47;1m"
      mcol := esc+"[38;5;30;1m"
      str5 := ten+ten
      str4 := mcol+"[]"+end
      str0 := colbon+"[]"+end
      str1 := colbon+block+end
      str6 := mcol+"[]"+block+end
      str2 := colbon+"[]"+block+end
      str3 := colbon+block+block+end
      str := cube+str0+two+str3+two+str3
      str7 := str1+one+str1+str+str4+two+two
      str8 := str0+one+equ+"row"+one+str0+cube+str0+two+str0+one+equ+"(x-m)*zoomx"+two
      str9 := one+equ+"="+one+str0+two+str0+ten+cube+one+str0+cube+equ+cube
      str10 := ten+five+two+str0+two+str0+one+equ+"+"+one+str0+five+str0
      str11 := cube+one+str0+two+equ+"cos(a)"+one+equ+"sin(a)"+two+str0+two+str0+cube+str0+two+equ+"m"+two+str0
      str12 := one+equ+"col"+one+str0+cube+str0+two+str0+one+equ+"(y-n)*zoomy"+two+str0+five+one
      str13 := one+equ+"-sin(a)"+one+equ+"cos(a)"+two+str0+two+str0+cube+str0+two+equ+"n"+two+str0
      str14 := str1+one+str1+str+five+one
      str15 := str1+two+str0+cube+str1+one+str1
      fmt.Printf(esc+"["+strconv.Itoa(toph+23)+";"+strconv.Itoa(dist)+"H"+colbon+"          "+two+str2+one+str5+str5+str2+str4+"\n")
      fmt.Printf(esc+"["+strconv.Itoa(toph+30)+";"+strconv.Itoa(dist)+"H"+ten+two+str0+two+two+five+str5+str5+str0+"\n")
      fmt.Printf(esc+"["+strconv.Itoa(toph+25)+";"+strconv.Itoa(dist)+"H"+str7+str1+ten+cube+str1+two+str0+cube+str1+one+str1+"\n")
      str16 := esc+"["+strconv.Itoa(toph+26)+";"+strconv.Itoa(dist)+"H"+str8+str0+str4
      str17 := str11+esc+"["+strconv.Itoa(toph+28)+";"+strconv.Itoa(dist)+"H"+str0+str12+str0+str13
      fmt.Printf(str16+str17+"\n")
      fmt.Printf(esc+"["+strconv.Itoa(toph+24)+";"+strconv.Itoa(dist)+"H"+two+ten+str0+ten+cube+two+cube+str6+str5+five+str0+str4+"\n")
      str18 := esc+"["+strconv.Itoa(toph+22)+";"+strconv.Itoa(dist)+"H"+cube+str5+str5+str5+str6+esc+"["
      str19 := strconv.Itoa(toph+29)+";"+strconv.Itoa(dist)+"H"+str14+str1+ten+cube+str15
      fmt.Printf(str18+str19+"\n")
      str20 := esc+"["+strconv.Itoa(toph+27)+";"+strconv.Itoa(dist)+"H"+str0+five+str0+str9+str0+str10
      str21 := esc+"["+strconv.Itoa(toph+31)+";"+strconv.Itoa(dist)+"H"+ten+two+str2+str5+str5+" "+str2
      fmt.Printf(str20+str21+"\n")
}
